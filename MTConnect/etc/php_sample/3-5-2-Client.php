<?php
require_once('HTTP/Request.php');
$url = "http://luckyfields.sakura.ne.jp/RESTtest/3-5-2-Server.php";
//$url = "https://mica-g62i9/#1_https://mica-g62i9/REST/3-5-2-Server.php";
$data = <<< XML
<?xml version="1.0"?>
<put>
  <name>ケーキ</name>
  <price>400</price>
</put>
XML;

$req =& new HTTP_Request($url);
$req->addHeader("Content-Type: application/xml");
$req->setMethod(HTTP_REQUEST_METHOD_PUT);
$req->addRawPostData($data);

if (!PEAR::isError($req->sendRequest())) {
  header("Content-Type: application/xml; charset=utf-8");
  echo $req->getResponseBody();
} else {
  print "error";
}
?>
