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
<script type="text/javascript" src="js/loc/modal.js"></script>
<link href="css/main.css" type="text/css" rel="stylesheet"/>
<link href="css/modal.css" type="text/css" rel="stylesheet"/>

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
				var fn = "<?=$xmlFilename?>"; //filename
				var newXml = xmlEditor.getXmlAsString(); //new xml content
				var h = "xmlString="+newXml+"&xmlFilename="+fn; //request header
				var req = new XMLHttpRequest(); //create&send request to saveXml.php
				req.onreadystatechange = function(data){
					if (data.error){
						GLR.messenger.show({msg:data.error,mode:"error"});
					} else {
						GLR.messenger.inform({msg:"Done", mode:"success"});
					}
				}
				req.open("POST", "saveXml.php", true);
				req.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
				req.send(h);
			});
		});
	<?php } else { ?>
		$("#xml").html("<span style='font:italic 14px georgia,serif; color:#f30;'>Please upload a valid XML file</span>").show();
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
		<input type="submit" value="Upload" id="upload"/>
	</form>

	<!-- MODAL START -->
	<div class="modal" id="get_parameters">
		<section name="parameters">
			<a class="boxclose" id="boxclose" onclick="modal_close('get_parameters')"></a>
			<form id="set_parameters">
				Task name:
				<input type="text" name="name" value="Insert task name..."/><br>
				Task type:
				<select>
					<option value="periodic">Periodic</option>
					<option value="aperiodic">Aperiodic</option>
				</select><br>
				Task schedule:
				<select>
					<option value="rr">Resource reservation</option>
					<option value="fp">Fixed priority</option>
				</select><br>
				Algorithm:
				<select>
					<option value="analytic">Analytic</option>
					<option value="companion">Companion</option>
					<option value="cyclic">Cyclic reduction</option>
					<option value="latouche">Latouche</option>
				</select><br>
				Server period:
				<input type="text" name="serverPeriod" value="100000"/><br>
				Server budget:
				<input type="text" name="serverBudget" value="15000"/><br>
				
				<br>PMF Computation:
				<select><option value="beta">Beta</option></select><br>
				Min:
				<input type="text" name="cmin" value="0"/><br>
				Max:
				<input type="text" name="cmax" value="99500"/><br>
				Step:
				<input type="text" name="step" value="500"/><br>
				Size:
				<input type="text" name="size" value="100000"/><br>
				Dump:
				<input type="checkbox">&nbsp;<br><br>

				<br>PMF Interarrival:
				<select><option value="beta">Beta</option></select><br>
				Min:
				<input type="text" name="cmin" value="0"/><br>
				Max:
				<input type="text" name="cmax" value="99500"/><br>
				Step:
				<input type="text" name="step" value="500"/><br>
				Size:
				<input type="text" name="size" value="100000"/><br>
				Dump:
				<input type="checkbox">&nbsp;<br><br>

				Delta:
				<input type="text" name="delta" value="7500"/><br>
				Max Deadline:
				<input type="text" name="maxDeadline" value="1"/><br>

				<input type="submit" value="Save" id="save"/>
				<input type="reset" value="Reset" id="reset"/>
			</form>
		</section>
	</div>
	<!-- MODAL END -->
	
	<div id="xml" style="display:none;"></div>
	<div id="actionButtons" style="display:none;">
		<button id="saveFile" name="saveFile">Save XML</button>
		<a href="index.php"><button id="back">Back</button></a>
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
	    echo '<div id="myActionButtons"><form style="color: #fff" id="uploadForm" method="post" id="selectForm">';
	    echo '<label>Select input file:</label><br><br>';
	    sort($file_list);
	    foreach ($file_list as $f) {
	      echo '<input style="vertical-align: bottom" type="radio" name="file_list" value="'.$f.'">'.$f.'<br>';
	    }
	    ?>
	    <br>
	    <button type="submit" name="select_file">Select &amp; Run</button></form>
	    <button id="create_task" onclick="modal_open('get_parameters')">Create task</button></div>
	    <?php
	  } 
	} else {
	  if(isset($_POST["file_list"])){
	    echo '<div id="outputResult">';
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