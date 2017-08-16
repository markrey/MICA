<?php
$name = urlencode("ケーキ");
$url = "http://luckyfields.sakura.ne.jp/RESTtest/3-5-1-Server2.php?name=".$name;
$xml = simplexml_load_file($url);
if(!$xml) {
  die($url."をパースできませんでした");
}
print '<!DOCTYPE html> <html><head><meta http-equiv="X-UA-Compatible" content="IE=EmulateIE10"><meta http-equiv="content-type" content="text/html; charset=utf-8"/></head><body>';
print '商品名：　'.$xml->name.'<br />';
print '単価：　'.$xml->price.'円<br />';
print 'message：　'.$xml->message.'<br />';
print '</body></html>';
?>
