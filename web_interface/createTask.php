<?php //Creates a new XML file from the form inputs
//task attrbuted
$tname = $_POST["name"];
$ttype = $_POST["type"];
$tschedule = $_POST["schedule"];
$talgorithm = $_POST["algorithm"];
$tperiod = $_POST["taskPeriod"];
//server parameters
$speriod = $_POST["serverPeriod"];
$sbudget = $_POST["serverBudget"];
//pmf computation
$pmfcomp = $_POST["pmfComputation"];
$cmin = $_POST["cmin"];
$cmax = $_POST["cmax"];
$cstep = $_POST["cstep"];
$csize = $_POST["csize"];
$cdumpFlag = $_POST["cdump"];
//pmf interarrival
$pmfint = $_POST["pmfInterarrival"];
$imin = $_POST["imin"];
$imax = $_POST["imax"];
$istep = $_POST["istep"];
$isize = $_POST["isize"];
$idumpFlag = $_POST["idump"];
//qos function
$qosfun = $_POST["qosFunction"];
$qmin = $_POST["qmin"];
$qmax = $_POST["qmax"];
$qscale = $_POST["qscale"];
$qoffset = $_POST["qoffset"];
//other parameters
$delta = $_POST["delta"];
$maxDeadline = $_POST["maxDeadline"]; 
$filename = $_POST["filename"];

$string = <<<XML
<document> 
  <solve>
    <task type="$ttype" schedule="$tschedule" name="$tname" algorithm="$talgorithm">
      <period>$tperiod</period>
      <serverPeriod>$speriod</serverPeriod>
      <serverBudget>$sbudget</serverBudget>
      <pmfComputation type="$pmfcomp">
        <step>$cstep</step>
        <file>test_case_1.txt</file>
      </pmfComputation>
      <pmfInterarrival type="$pmfint">
        <step>$istep</step>
        <file>z_2.txt</file>
      </pmfInterarrival>
      <qosfun type="$qosfun">
        <pmin>$qmin</pmin>
        <pmax>$qmax</pmax>
        <scale>$qscale</scale>
        <offset>$qoffset</offset>
      </qosfun>
      <Delta>$delta</Delta>
      <maxDeadline>$maxDeadline</maxDeadline>
    </task>
  </solve>
</document>
XML;

$xml = new SimpleXMLElement($string);
//save to file
$xml->asXML("_data/".$filename.".xml");
header('Location: index.php');

?>