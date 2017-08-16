<?php
$name = urlencode("ケーキ");
$url = "http://luckyfields.sakura.ne.jp/RESTtest/3-5-1-Server.php?name=".$name;
$xml = simplexml_load_file($url);
if(!$xml) {
  die($url."をパースできませんでした");
}

print '商品名：　'.$xml->name.'<br />';
print '単価：　'.$xml->price.'円<br />';
?>
