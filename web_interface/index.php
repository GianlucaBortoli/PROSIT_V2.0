<head>
<title>PROSIT v2.0 WEB INTERFACE</title>
</head>
<body>
<?php
echo "<style type='text/css'>
      body{
      color: #ffff;
      font-family:'Lucida Console',sans-serif !important;
      font-size: 13px;
      }</style>";
// upload file form
error_reporting(2047);
if(isset($_POST["send"])) {
  $complete_path = "../uploads/" . $_FILES['fileToUpload']['name'];
  if (is_uploaded_file($_FILES['fileToUpload']['tmp_name'])) {
    if (move_uploaded_file($_FILES['fileToUpload']['tmp_name'], $complete_path)) {
      header('Refresh: 2;url=index.php'); //auto redirect
      echo $_FILES['fileToUpload']['name'].' uploaded successfully!';
    } else {
      echo "Error while uploading ".$_FILES["fileToUpload"]["error"];
    }
  } else {
    echo "Error while uploading ".$_FILES["fileToUpload"]["error"];
  }
} else {
  echo '<form enctype="multipart/form-data" method="post" action="">
          <h2>Select a file to upload on server:</h2>
          <input type="file" name="fileToUpload"><br><br>
          <input type="submit" value="Upload" name="send"><br><br>
        </form>';
}

// file selector
$dir = "../uploads/";
$file_list = array();
if(!isset($_POST["select_file"])){
  if($d = opendir($dir)){
    while(($file = readdir($d)) !== false){
      if(!is_dir($dir . $file)){
        $file_list[] = $file;
      }
    }
    closedir($d);
  } else {
    echo "Directory can't be opened";
  }

  if(!empty($file_list)){
    echo '<form action="" method="post">';
    echo '============================================';
    echo '<h2>Select input file:</h2>';
    foreach ($file_list as $f) {
      echo '<input type="radio" name="file_list" value="'.$f.'">'.$f.'<br>';
    }
    echo '<br><button type="submit" name="select_file">Select & Run</button></form>';
  } else {
    echo "The upload folder is empty";
  }
} else {
  if(isset($_POST["file_list"])){
    $old_path = getcwd();
    chdir('/var/www/PROSIT_V2.0/test/'); //change dir to correct one
    $out = shell_exec('./test_command_line');
    
    print_r($out);
    echo "<h4>COMPUTATION COMPLETED</h4>";
    chdir($old_path);
  } else {
    header('Refresh: 2;url=index.php');
    exit("No file selected");
  }
}

?>
</body>
</html>