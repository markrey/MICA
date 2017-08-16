
<!DOCTYPE html> 
<html>
<head>
	<meta http-equiv="refresh" content="10">
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

</head>

<body>

	<div class="headercontainer"> 
		<!--Metanav-->
		<div class="meta">
			<div class="left"></div>
			<div class="middle" id="hostname"></div>
			<div class="right"></div>
		</div>
		<!--END Metanav--> 
		
		<div class="header" id ="header_cnt">
		<div class="header_app_icon_outer">
			<div class="header_app_icon_inner"><img src="images/Header.png" width="68" height="68"  alt=""/></div>
		</div>
		<div class="headertitle">ALARM LIST</div>
		<div class="headerlogo"><img src="images/dhtd_logo.gif" width="205" height="42"  alt=""/></div>
	</div>
		
		
	</div>	
	
	
	<!--Content-->
	<div id="content">
	
	<div id="inner_content">

	<table border="1" width="80%">
		<tr>
			<th width="30%" align="left">Date/Time</th>
			<th width="15%" align="left">Sensor</th>
			<th width="*" align="left">alarm</th>
		</tr>

<?php
// ファイルを変数に格納
$filename = 'alarm.log';

// ファイル取得

$file = new SplFileObject($filename); 
$file->setFlags(SplFileObject::READ_CSV);

// ファイル内のデータループ
foreach ($file as $key => $line) {
	foreach( $line as $str ){
		$records[ $key ][] = $str ;
	}
	if($records[$key][0]!=""){
		echo "<tr><td>".$records[$key][0]."</td><td>".$records[$key][1]."</td><td>".$records[$key][2]."</td></tr>";
	}
}
?>
	</table>

</div><div class="gap60">
</div>
<!--Footer-->
<div class="footer">
<div>HARTING IIC MICA</div>
</div>
<!--END Footer--> 
</div></body></html>




