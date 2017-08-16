<?php
$str1 = file_get_contents("php://input");

$xml = simplexml_load_string($str1);
$name = $xml->name;
$price = $xml->price;

$str2 = <<<XML
<?xml version="1.0"?>
<putResponse>
  <error>
    <code></code>
    <msg></msg>
  </error>
</putResponse>
XML;
$res = simplexml_load_string($str2);

$link = sqlite_open('snack_db', 0666, $errormsg);
if (!$link) {
  //die('接続失敗です。'.$errormsg);
  $res->error->code = 1;
  $res->error->msg = "接続失敗です。:".$errormsg.",con:".$con.",name:".$name.",price:".$price.",str1:".$str1;
}

$sql = "SELECT name, price FROM price_table";
$result = sqlite_query($link, $sql, SQLITE_BOTH, $sqliteerror);
if (!$result) {
  //die('クエリーが失敗しました。'.$sqliteerror);
  $res->error->code = 2;
  $res->error->msg = "クエリーが失敗しました:".$sqliteerror.",con:".$con.",name:".$name.",price:".$price.",str1:".$str1;
}

$selectResult = "";
for ($i = 0 ; $i < sqlite_num_rows($result) ; $i++){
    $rows = sqlite_fetch_array($result, SQLITE_ASSOC);
    $selectResult = $selectResult."name=".$rows['name'].",price=".$rows['price']."<br>";
}

if ($name) {
  $sql = "UPDATE price_table SET price = $price WHERE name='$name'";
  sqlite_query($link, $sql, SQLITE_BOTH, $sqliteerror);
  $res->error->code = 0;
  $res->error->msg = "更新に成功しました:".$sqliteerror.",name:".$name.",price:".$price.",selectResult".$selectResult;
} else {
  $res->error->code =3;
  $res->error->msg = "更新に失敗しました:".$sqliteerror.",con:".$con.",name:".$name.",price:".$price.",str1:".$str1;
}

sqlite_close($link);


header("Content-Type: application/xml; charset=utf-8");
echo $res->asXML();
?>
