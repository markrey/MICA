<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<HTML>
<HEAD>
	<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE10">
	<meta http-equiv="content-type" content="text/html; charset=utf-8"/>
	<title>HARTING IIC MICA</title>
	<link rel="stylesheet" type="text/css" href="css/base.css" />
	<link rel="stylesheet" type="text/css" href="css/framing.css" />
	<link rel="stylesheet" type="text/css" href="css/home.css" />
	<link rel="stylesheet" type="text/css" href="css/progress.css" />
	<link rel="stylesheet" type="text/css" href="css/login.css" />
	<link rel="stylesheet" type="text/css" href="css/switches.css" />
	
	<!-- jquery -->
	<script type="text/javaScript" src="js/jquery-1.11.1.min.js" ></script>
	
	<!-- other js sources -->
	<script type="text/javaScript" src="js/dialogs.js?v1" ></script>
	<script type="text/javaScript" src="js/helper.js?v1" ></script>
	<script type="text/javaScript" src="js/json_rpc.js?v1" ></script>
	<script type="text/javaScript" src="js/sha256.js?v1" ></script>
	
	<script type="text/javaScript" src="js/container_control.js?v2" ></script>
<meta http-equiv="refresh" content="15">
<script type="text/javascript"> 
<!-- 
function lancomconf(){ 
   window.open( "lancomConfig.php" ,"config"); 
} 
function basehalt(){ 
  if(window.confirm('System Halt?')){
     window.open("basehalt.php"); 
  }
} 
//end --> 
</script> 
</HEAD>

<body>
<?php
// ホスト名をファイルに書き出します
$filename = "/var/www/html/basehalt.txt";
$current = $_SERVER['HTTP_HOST'].".local";
file_put_contents($filename, $current);


// 引数 $file_list2 配列の[N][1] でソートする関数
function order_by_desc($a, $b)
{
    if ( strtotime($a[1]) > strtotime($b[1]) )
    {
        return -1;
    } else if(strtotime($a[1]) < strtotime($b[1])) {
        return 1;
    } else{
        return 0;
    }
}

$rep = 1; // 最終更新日(表示=1 非表示=0)
$sze = 1; // ファイルサイズ(表示=1 非表示=0)
$lst = "sdcard"; // 表示するリストの名前(パス)
$dlDir = "sdcard";

// ディレクトリハンドルの取得
$dir_h = opendir( $lst."/" ) ;

 
// ファイル・ディレクトリの一覧を $file_list に
while (false !== ($file_list[] = readdir($dir_h))) ;
closedir( $dir_h ) ;
 
$file_list2 = array() ;
 
$i = 0 ;
foreach ( $file_list as $file_name )
{
  $lfl = $lst."/".$file_name;
  //ファイルのみを表示
  if( is_file( $lfl) )
  {
    //$file_list2[N] の [0]にファイル名、[1]にファイル更新日
    $file_list2[$i][0] = $file_name ;
    // ファイルの更新日時を取得
    $file_list2[$i][1] = date("Y/m/d H:i", filemtime( $lst."/" . $file_name )) ;
    $file_list2[$i][2] = round(filesize($lfl)/1024)."KB" ;
    $i++ ;
  }
}

// $file_list2 をファイルの更新日時でソート
usort($file_list2, "order_by_desc") ;

// 出力
$i = 0 ;
foreach ( $file_list2 as $file_name )
{
  //ファイルのみを表示
  if( is_file( $lst."/" . $file_name[0]) )
  {
    print("<LI>");
    print("<a href=".$dlDir."/".$file_name[0].">".$file_name[0]."</a>");
    // ファイル更新日
    print("<FONT size='-1'> (");
    echo $file_list2[$i][1];
    print(", ");
    // ファイルサイズ
    echo $file_name[2];
    print(")</FONT> ");
    print("</LI>");
    $i++ ;
  }
}

?>
<p>
<p>
<p>
<p>
<div>
<input type="submit" value="　　Config　　" name="sub1" onclick="return lancomconf();">
<input type="submit" value="　　System Halt　　" name="sub2" onclick="return basehalt();">
</div>

</body>

</html>




