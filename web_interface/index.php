<head>
<title>PROSIT v2.0 WEB INTERFACE</title>
</head>
<body>
<?php
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

/////////////////////////////
// HERE STARTS THE WEB GUI //
/////////////////////////////
echo "<style type='text/css'>
      body{
        color: #ffff;
        font-family:'Lucida Console',sans-serif !important;
        font-size: 13px;
      }
      #all{
        padding-top: 20px;
        width: 60%;
        margin: 0 auto;
      }
      </style>";
// file uploader
error_reporting(2047);
if(isset($_POST["send"])) {
  $complete_path = "../uploads/" . $_FILES['fileToUpload']['name'];

  $extension = pathinfo($complete_path);
  if($extension['extension'] !== 'xml'){ // check file extension
    header('Refresh: 2;url=index.php'); //auto redirect
    exit("The file is not an XML");
  }

  if (is_uploaded_file($_FILES['fileToUpload']['tmp_name'])) {
    if (move_uploaded_file($_FILES['fileToUpload']['tmp_name'], $complete_path)) {
      header('Refresh: 2;url=index.php'); 
      exec('chmod 777 '.$complete_path);
      echo '<div id="all">Permission for uploaded file changed<br>';
      echo $_FILES['fileToUpload']['name'].' uploaded successfully!</div>';
    } else {
      echo "Error while uploading ".$_FILES["fileToUpload"]["error"];
    }
  } else {
    echo "Error while uploading ".$_FILES["fileToUpload"]["error"];
  }
} else {
  echo '<div id="all"><form enctype="multipart/form-data" method="post" action="">
          <h2>Select a file to upload on server:</h2>
          <input type="file" name="fileToUpload"><br><br>
          <input type="submit" value="Upload" name="send"><br><br>
        </form></div>';
}

// file selector
$dir = "../uploads/";
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
    echo '<div id="all"><form action="" method="post">';
    echo '============================================';
    echo '<h2>Select input file:</h2>';
    foreach ($file_list as $f) {
      echo '<input type="radio" name="file_list" value="'.$f.'">'.$f.'<br>';
    }
    echo '<br><button type="submit" name="select_file">Select & Run</button></form></div>';
  } else {
    echo "The upload folder is empty";
  }
} else {
  if(isset($_POST["file_list"])){
    echo '<div id="all">============================================<br><br>';
    $old_path = getcwd();
    chdir('/var/www/uploads/'); //change dir to correct one
    // streaming command output
    $pid = popen('./bin/xml_solver '.$_POST["file_list"].' --verbose 2>&1', "r");

    while(!feof($pid)){
      echo fgets($pid, 256).'<br>';
      flush();
      ob_flush();
      echo "<script>window.scrollTo(0,99999);</script>";
      usleep(10);
    }
    pclose($pid);
    echo "<script>window.scrollTo(0,99999);</script>";
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