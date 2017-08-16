<?php
include 'xmlrpc.inc';

$xmlrpc_internalencoding = 'UTF-8';
$server_path = dirname($_SERVER["PHP_SELF"])."/"."3-2-1-Server.php";
$server_host = $_SERVER["SERVER_NAME"];
$server_port = $_SERVER["SERVER_PORT"];

$server = new xmlrpc_client($server_path, $server_host, $server_port);

$message = new xmlrpcmsg('example.query', array(new xmlrpcval('ケーキ','string')));

$result = $server->send($message);

if(!$result) {
  print "HTTPサーバに接続できませんでした";
} else {
  header("Content-Type: application/xml; charset=utf-8");
  print $result->serialize();
}
?>
