<?php
$xml = simplexml_load_file("XML2-1-2.xml");
$xml->parent->child[1] = "foo";
$xml->parent->child[1]['attr'] = "bar";

header("Content-Type: application/xml; charset=utf-8");
echo $xml->asXML();
?>

