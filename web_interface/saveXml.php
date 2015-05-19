<?php //Saves POST input as an XML file and returns a JSON response
if (isset($_POST['xmlString'])){
	$filename  = $_POST['xmlFilename'];
	$xmlString = stripslashes($_POST['xmlString']);
	$newFile = "_data/".$filename.".edited.xml";
	echo $filename;
	echo $xmlString;
	$handle = fopen($newFile, "w") or die("Unable to open file!"); 
	if (fwrite($handle, $xmlString) === false) { 
		echo "{error:\"Couldn't write to file $newFile.\"}";  
	} else {
		echo "{filename:\"".$newFile."\"}";
	}
	fclose($handle); 	
}
?>