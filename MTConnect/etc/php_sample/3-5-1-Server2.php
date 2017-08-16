<?php
$name=$_GET["name"];

$link = sqlite_open('snack_db', 0666, $message);
if (!$link) {
} else {

  $sql = "SELECT name, price FROM price_table WHERE name = '$name'";
  $result = sqlite_query($link, $sql, SQLITE_BOTH, $message);
  if (!$result) {
  } else {
    $rows = sqlite_fetch_array($result, SQLITE_ASSOC);
    $price = $rows['price'];
  }
  sqlite_close($link);
}

$xml = <<< XML
<?xml version="1.0"?>
<queryResponse>
  <name>$name</name>
  <price>$price</price>
  <message>$message</message>
</queryResponse>
XML;
header("Content-Type: application/xml; charset=utf-8");
echo $xml;
?>
