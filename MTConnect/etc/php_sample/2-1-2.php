<!DOCTYPE html> 
<html>
<head>
	<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE10">
	<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
</head>
<body>

<?php
$xml = simplexml_load_file("XML2-1-2.xml");
print "1番目の子要素の内容は".$xml->parent->child[0]."<br />";
print "1番目の子要素の属性値は".$xml->parent->child[0]['attr']."<br />";
print "2番目の子要素の内容は".$xml->parent->child[1]."<br />";
print "2番目の子要素の属性値は".$xml->parent->child[1]['attr']."<br />";
print "3番目の子要素の内容は".$xml->parent->child[2]."<br />";
print "3番目の子要素の属性値は".$xml->parent->child[2]['attr']."<br />";
?>
</body>
</html>

