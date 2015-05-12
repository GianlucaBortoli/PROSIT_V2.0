<?php //file uploader
$xmlString;
$xmlFilename;
$target;
$xml;
$got_file = false;
if (isset($_FILES['xmlfile'])){
	$xmlFilename = basename($_FILES['xmlfile']['name']);
	$target = "_data/" . $xmlFilename;	
	if (move_uploaded_file($_FILES['xmlfile']['tmp_name'], $target)){
		$got_file = true;
	}
}
/////////////////////////
// HTTP AUTHENTICATION //
/////////////////////////
$realm = 'Restricted area';
//user => password
$users = array('yellow' => 'giallo131293');
if (empty($_SERVER['PHP_AUTH_DIGEST'])) {
    header('HTTP/1.1 401 Unauthorized');
    header('WWW-Authenticate: Digest realm="'.$realm.
           '",qop="auth",nonce="'.uniqid().'",opaque="'.md5($realm).'"');

    die('Text to send if user hits Cancel button');
}
// analyze the PHP_AUTH_DIGEST variable
if (!($data = http_digest_parse($_SERVER['PHP_AUTH_DIGEST'])) ||
    !isset($users[$data['username']]))
    die('Wrong Credentials!');
// generate the valid response
$A1 = md5($data['username'] . ':' . $realm . ':' . $users[$data['username']]);
$A2 = md5($_SERVER['REQUEST_METHOD'].':'.$data['uri']);
$valid_response = md5($A1.':'.$data['nonce'].':'.$data['nc'].':'.$data['cnonce'].':'.$data['qop'].':'.$A2);
if ($data['response'] != $valid_response)
    die('Wrong Credentials!');
// function to parse the http auth header
function http_digest_parse($txt)
{
    // protect against missing data
    $needed_parts = array('nonce'=>1, 'nc'=>1, 'cnonce'=>1, 'qop'=>1, 'username'=>1, 'uri'=>1, 'response'=>1);
    $data = array();
    $keys = implode('|', array_keys($needed_parts));

    preg_match_all('@(' . $keys . ')=(?:([\'"])([^\2]+?)\2|([^\s,]+))@', $txt, $matches, PREG_SET_ORDER);

    foreach ($matches as $m) {
        $data[$m[1]] = $m[3] ? $m[3] : $m[4];
        unset($needed_parts[$m[1]]);
    }

    return $needed_parts ? false : $data;
}
?>

<!DOCTYPE html>
<html>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>PROSIT V2.0 WEB INTERFACE</title>
<script type="text/javascript" src="js/ext/jquery-1.4.js"></script>
<script type="text/javascript" src="js/ext/jquery-color.js"></script>
<script type="text/javascript" src="js/ext/GLR/GLR.js"></script>
<script type="text/javascript" src="js/ext/GLR/GLR.messenger.js"></script>
<script type="text/javascript" src="js/loc/xmlEditor.js"></script>
<link href="css/main.css" type="text/css" rel="stylesheet"/>

<script type="text/javascript">
	$(document).ready(function(){
	<?php if ($got_file){ ?>
		GLR.messenger.show({msg:"Loading XML..."});
		console.time("loadingXML");
		
		xmlEditor.loadXmlFromFile("<?=$target?>", "#xml", function(){
			console.timeEnd("loadingXML");
			$("#xml").show();
			$("#actionButtons").show();																																				
			xmlEditor.renderTree();
			$("button#saveFile").show().click(function(){
				GLR.messenger.show({msg:"Generating file...", mode:"loading"});	
				var req = new XMLHttpRequest();
				req.onreadystatechange = function(data){
					if (data.error){
						GLR.messenger.show({msg:data.error,mode:"error"});
					} else {
						GLR.messenger.inform({msg:"Done", mode:"success"});
					}
				}
				req.open("POST", "saveXml.php", true);
				req.setRequestHeader("Content-Type", "json");
				req.send("xmlString=xmlEditor.getXmlAsString()&xmlFilename='<?=$xmlFilename?>'");
				/*
				$.ajax({
					type: "POST",
					data: {xmlString:xmlEditor.getXmlAsString(), xmlFilename:"<?=$xmlFilename?>"},
					dataType: "json",
					url: "saveXml.php",
					cache: false,
					success: function(){alert("ok");},
					error: function(error){alert("error" + eval(error));}
				});*/
				/*
				$.post("saveXml.php", {xmlString:xmlEditor.getXmlAsString(), xmlFilename:"<?=$xmlFilename?>"}, function(data){
					GLR.messenger.inform({msg:"Done.", mode:"success"});
					if (data.error){
						GLR.messenger.show({msg:data.error,mode:"error"});
					} else {
						GLR.messenger.inform({msg:"Done.", mode:"success"});
						if (!$("button#viewFile").length){
							$("<button id='viewFile'>View Updated File</button>")
								.appendTo("#actionButtons div")
								.click(function(){ window.open(data.filename); });
						}
					}
				}, "json");*/
			});
		});
	<?php } else { ?>
		$("#xml").html("<span style='font:italic 14px georgia,serif; color:#f30;'>Please upload a valid XML file.</span>").show();
		<?php if ($target && !$xml){ ?>
			GLR.messenger.showAndHide({msg:"Uploaded file is not valid XML and cannot be edited.", mode:"error", speed:3000});
		<?php } ?>
	<?php } ?>
	});
</script>

</head>
<body> 
	<form id="uploadForm" action="index.php" method="post" enctype="multipart/form-data">
		<label for="xmlfile">Specify XML file to edit:</label>
		<input type="file" name="xmlfile" id="xmlfile"/>
		<input type="submit" value="Upload"/>
	</form>
	<div id="xml" style="display:none;"></div>
	<div id="actionButtons" style="display:none;">
		<div></div>
		<button id="saveFile">Save XML</button>
	</div>
	<div id="nodePath"></div>
	</div>

	<?php
	$dir = "_data/";
	$file_list = array();
	if(!isset($_POST["select_file"])){
	  if($d = opendir($dir)){
	    while(($file = readdir($d)) !== false){
	      $ext = pathinfo($file);
	      if((!is_dir($dir . $file)) && ($ext['extension']) === 'xml'){
	        $file_list[] = $file;
	      }
	    }
	    closedir($d);
	  } else {
	    echo "Directory can't be opened";
	  }

	  if(!empty($file_list)){
	    echo '<div id="actionButtons"><form id="uploadForm" method="post">';
	    echo '<label>Select input file:</label><br>';
	    sort($file_list);
	    foreach ($file_list as $f) {
	      echo '<input type="radio" name="file_list" value="'.$f.'">'.$f.'<br>';
	    }
	    echo '<br><button type="submit" name="select_file">Select & Run</button></form></div>';
	  } 
	} else {
	  if(isset($_POST["file_list"])){
	    echo '<div id="actionButtons">';
	    $old_path = getcwd();
	    chdir('/var/www/html/_data/'); //change dir to correct one
	    // streaming command output
	    $pid = popen('./bin/xml_solver '.$_POST["file_list"].' --verbose 2>&1', "r");
	    ini_set("auto_detect_line_endings", true);
	    while(!feof($pid)){
	      echo fgets($pid, 256).'<br>';
	      flush();
	      ob_flush();
	      usleep(10);
	      echo "<script type='text/javascript'>window.scrollTo(0,document.body.scrollHeight);</script>"; //autoscroll to bottom of page
	    }
	    pclose($pid);
	    echo '<h4>COMPUTATION COMPLETED !</h4><a href="index.php"><button>Back</button></a></div>';
	    chdir($old_path);
	  } else {
	    header('Refresh: 2;url=index.php');
	    exit("No file selected");
	  }
	}
	?>
</body>
</html>