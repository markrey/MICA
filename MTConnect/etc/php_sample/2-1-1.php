<!DOCTYPE html> 
<html>
<head>
	<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE10">
	<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
</head>
<body>

<?php
$xml = simplexml_load_file("XML2-1-1.xml");
print "elemBの内容は".$xml->elemA->elemB."<br />";
print "attrBの内容は".$xml->elemA->elemB['attrB']."<br />";
?>
</body>
</html>

