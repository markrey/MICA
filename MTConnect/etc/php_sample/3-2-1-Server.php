<?php
include 'xmlrpc.inc';
include 'xmlrpcs.inc';

$xmlrpc_internalencoding = 'UTF-8';
$stock = array('ケーキ'=> 50,'クッキー'=> 80,'煎餅'=> 30,'あられ'=> 55);

function query($param)
{
  global $stock;
  
  $name = $param->getParam(0)->scalarval();
  
  $reply = $name."は、".$stock[$name]."個あります";
  return new xmlrpcresp(new xmlrpcval($reply, 'string'));
}

new xmlrpc_server(array('example.query' => array('function' => 'query')));
?>
