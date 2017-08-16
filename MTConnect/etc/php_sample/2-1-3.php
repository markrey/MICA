<!DOCTYPE html> 
<html>
<head>
	<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE10">
	<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
</head>
<body>

<?php
$xml = simplexml_load_file("XML2-1-2.xml");
foreach ($xml->parent->child as $child){
  print $child."<br />";
  print $child[attr]."<br />";
}
?>
</body>
</html>

