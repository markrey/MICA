<?php
$name=$_GET["name"];
$prices = array('ケーキ' => 350, 'クッキー' => 350, '煎餅' => 350, 'あられ' => 350);
$price = $prices[$name];
$xml = <<< XML
<?xml version="1.0"?>
<queryResponse>
  <name>$name</name>
  <price>$price</price>
</queryResponse>
XML;
header("Content-Type: application/xml; charset=utf-8");
echo $xml;
?>
