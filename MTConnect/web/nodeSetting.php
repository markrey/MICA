
<!DOCTYPE html> 
<html>
<head>
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
		<div class="headertitle">SENSOR CONFIGURATION</div>
		<div class="headerlogo"><img src="images/dhtd_logo.gif" width="205" height="42"  alt=""/></div>
		</div>
		
		
	</div>	
	
	
	<!--Content-->
	<div id="content">
	
	<div id="inner_content">

<?php
$filenameStatus = 'nodeSetting.ini';
$filenameRange = 'nodeRange.ini';

// 入力内容を表示する
$checkStatus = array("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0");
$rangeZero = array("","","","","","","","","","","","","","","","");
$rangeFull = array("","","","","","","","","","","","","","","","");
$mistUnit[$idx] =  array("","","","","","","","","","","","","","","","");
$preAlarm[$idx] =  array("","","","","","","","","","","","","","","","");
$highAlarm[$idx] =  array("","","","","","","","","","","","","","","","");

$sensorName = array("A1","A2","A3","A4","A5","A6","A7","A8","A9","A10","A11","A12","A13","A14","A15","A16");

for ($i = 0; $i < count(@$_POST["check1"]); $i++) {
	$checkStatus[intval($_POST["check1"][$i])] = "1";
}
$idx = 0;
$sensorName[$idx] = @$_POST["sensName00"];
$rangeZero[$idx] = @$_POST["textA00"];
$rangeFull[$idx] = @$_POST["textB00"];
$mistUnit[$idx] =  @$_POST["textC00"];
$preAlarm[$idx] =  @$_POST["textD00"];
$highAlarm[$idx] =  @$_POST["textE00"];
$idx++;
$sensorName[$idx] = @$_POST["sensName01"];
$rangeZero[$idx] = @$_POST["textA01"];
$rangeFull[$idx] = @$_POST["textB01"];
$mistUnit[$idx] =  @$_POST["textC01"];
$preAlarm[$idx] =  @$_POST["textD01"];
$highAlarm[$idx] =  @$_POST["textE01"];
$idx++;
$sensorName[$idx] = @$_POST["sensName02"];
$rangeZero[$idx] = @$_POST["textA02"];
$rangeFull[$idx] = @$_POST["textB02"];
$mistUnit[$idx] =  @$_POST["textC02"];
$preAlarm[$idx] =  @$_POST["textD02"];
$highAlarm[$idx] =  @$_POST["textE02"];
$idx++;
$sensorName[$idx] = @$_POST["sensName03"];
$rangeZero[$idx] = @$_POST["textA03"];
$rangeFull[$idx] = @$_POST["textB03"];
$mistUnit[$idx] =  @$_POST["textC03"];
$preAlarm[$idx] =  @$_POST["textD03"];
$highAlarm[$idx] =  @$_POST["textE03"];
$idx++;
$sensorName[$idx] = @$_POST["sensName04"];
$rangeZero[$idx] = @$_POST["textA04"];
$rangeFull[$idx] = @$_POST["textB04"];
$mistUnit[$idx] =  @$_POST["textC04"];
$preAlarm[$idx] =  @$_POST["textD04"];
$highAlarm[$idx] =  @$_POST["textE04"];
$idx++;
$sensorName[$idx] = @$_POST["sensName05"];
$rangeZero[$idx] = @$_POST["textA05"];
$rangeFull[$idx] = @$_POST["textB05"];
$mistUnit[$idx] =  @$_POST["textC05"];
$preAlarm[$idx] =  @$_POST["textD05"];
$highAlarm[$idx] =  @$_POST["textE05"];
$idx++;
$sensorName[$idx] = @$_POST["sensName06"];
$rangeZero[$idx] = @$_POST["textA06"];
$rangeFull[$idx] = @$_POST["textB06"];
$mistUnit[$idx] =  @$_POST["textC06"];
$preAlarm[$idx] =  @$_POST["textD06"];
$highAlarm[$idx] =  @$_POST["textE06"];
$idx++;
$sensorName[$idx] = @$_POST["sensName07"];
$rangeZero[$idx] = @$_POST["textA07"];
$rangeFull[$idx] = @$_POST["textB07"];
$mistUnit[$idx] =  @$_POST["textC07"];
$preAlarm[$idx] =  @$_POST["textD07"];
$highAlarm[$idx] =  @$_POST["textE07"];
$idx++;
$sensorName[$idx] = @$_POST["sensName08"];
$rangeZero[$idx] = @$_POST["textA08"];
$rangeFull[$idx] = @$_POST["textB08"];
$mistUnit[$idx] =  @$_POST["textC08"];
$preAlarm[$idx] =  @$_POST["textD08"];
$highAlarm[$idx] =  @$_POST["textE08"];
$idx++;
$sensorName[$idx] = @$_POST["sensName09"];
$rangeZero[$idx] = @$_POST["textA09"];
$rangeFull[$idx] = @$_POST["textB09"];
$mistUnit[$idx] =  @$_POST["textC09"];
$preAlarm[$idx] =  @$_POST["textD09"];
$highAlarm[$idx] =  @$_POST["textE09"];
$idx++;
$sensorName[$idx] = @$_POST["sensName10"];
$rangeZero[$idx] = @$_POST["textA10"];
$rangeFull[$idx] = @$_POST["textB10"];
$mistUnit[$idx] =  @$_POST["textC10"];
$preAlarm[$idx] =  @$_POST["textD10"];
$highAlarm[$idx] =  @$_POST["textE10"];
$idx++;
$sensorName[$idx] = @$_POST["sensName11"];
$rangeZero[$idx] = @$_POST["textA11"];
$rangeFull[$idx] = @$_POST["textB11"];
$mistUnit[$idx] =  @$_POST["textC11"];
$preAlarm[$idx] =  @$_POST["textD11"];
$highAlarm[$idx] =  @$_POST["textE11"];
$idx++;
$sensorName[$idx] = @$_POST["sensName12"];
$rangeZero[$idx] = @$_POST["textA12"];
$rangeFull[$idx] = @$_POST["textB12"];
$mistUnit[$idx] =  @$_POST["textC12"];
$preAlarm[$idx] =  @$_POST["textD12"];
$highAlarm[$idx] =  @$_POST["textE12"];
$idx++;
$sensorName[$idx] = @$_POST["sensName13"];
$rangeZero[$idx] = @$_POST["textA13"];
$rangeFull[$idx] = @$_POST["textB13"];
$mistUnit[$idx] =  @$_POST["textC13"];
$preAlarm[$idx] =  @$_POST["textD13"];
$highAlarm[$idx] =  @$_POST["textE13"];
$idx++;
$sensorName[$idx] = @$_POST["sensName14"];
$rangeZero[$idx] = @$_POST["textA14"];
$rangeFull[$idx] = @$_POST["textB14"];
$mistUnit[$idx] =  @$_POST["textC14"];
$preAlarm[$idx] =  @$_POST["textD14"];
$highAlarm[$idx] =  @$_POST["textE14"];
$idx++;
$sensorName[$idx] = @$_POST["sensName15"];
$rangeZero[$idx] = @$_POST["textA15"];
$rangeFull[$idx] = @$_POST["textB15"];
$mistUnit[$idx] =  @$_POST["textC15"];
$preAlarm[$idx] =  @$_POST["textD15"];
$highAlarm[$idx] =  @$_POST["textE15"];
$idx++;

$writeDataStatus = "";
for ($i = 0; $i < count($checkStatus); $i++) {
	if(strlen($writeDataStatus)!=0){
	    $writeDataStatus .= ",";
	}
    $writeDataStatus .= $checkStatus[$i];
}
$writeDataStatus .= "\n";

$writeDataRange = "";
for ($i = 0; $i < count($sensorName); $i++) {
    $writeDataRange .= $sensorName[$i].",".$rangeZero[$i].",".$rangeFull[$i].",".$mistUnit[$i].",".$preAlarm[$i].",".$highAlarm[$i]."\n";

}

if(@$_POST["str1"]=="str2"){
	file_put_contents($filenameStatus, $writeDataStatus);
	file_put_contents($filenameRange, $writeDataRange);
}


// センサ有効設定読み込み
$content = file_get_contents($filenameStatus);
$lines = str_getcsv($content, ','); 
foreach ($lines as $line) {
    $setStatus[] = str_getcsv($line); 
} 

$checkStatus = array("0","0","0","0","0","0","0","0","0","0","0","0","0","0","0","0");
for ($i = 0; $i < count($setStatus); $i++) {
	$checkStatus[$i] = $setStatus[$i][0];
}

// センサレンジ設定読み込み
$file = fopen($filenameRange,"r");
$idx = 0;
while ($array = fgetcsv($file, 1000, ",")) {
    $sensorName[$idx] = $array[0];
    $rangeZero[$idx] = $array[1];
    $rangeFull[$idx] = $array[2];
    $mistUnit[$idx] = $array[3];
    $preAlarm[$idx] = $array[4];
    $highAlarm[$idx] = $array[5];
    $idx += 1;
}
fclose($file);



?>
<form method="POST" action="nodeSetting.php">
	<table border="10" bordercolor="white" width="80%">
		<tr>
			<th width="15%" align="right">NODE</th>
			<th width="17%">ZERO</th>
			<th width="17%">FULL</th>
			<th width="17%">UNIT</th>
			<th width="17%">PRE</th>
			<th width="17%">HIGH</th>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="0" <?php print ($checkStatus[0]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[0]; ?><input type="hidden" name="sensName00" value="<?php echo $sensorName[0]; ?>"></td>
			<td><input type="text" size="5" name="textA00" value="<?php echo $rangeZero[0]; ?>"></td>
			<td><input type="text" size="5" name="textB00" value="<?php echo $rangeFull[0]; ?>"></td>
			<td><input type="text" size="5" name="textC00" value="<?php echo $mistUnit[0]; ?>"></td>
			<td><input type="text" size="5" name="textD00" value="<?php echo $preAlarm[0]; ?>"></td>
			<td><input type="text" size="5" name="textE00" value="<?php echo $highAlarm[0]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="1" <?php print ($checkStatus[1]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[1]; ?><input type="hidden" name="sensName01" value="<?php echo $sensorName[1]; ?>"></td>
			<td><input type="text" size="5" name="textA01" value="<?php echo $rangeZero[1]; ?>"></td>
			<td><input type="text" size="5" name="textB01" value="<?php echo $rangeFull[1]; ?>"></td>
			<td><input type="text" size="5" name="textC01" value="<?php echo $mistUnit[1]; ?>"></td>
			<td><input type="text" size="5" name="textD01" value="<?php echo $preAlarm[1]; ?>"></td>
			<td><input type="text" size="5" name="textE01" value="<?php echo $highAlarm[1]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="2" <?php print ($checkStatus[2]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[2]; ?><input type="hidden" name="sensName02" value="<?php echo $sensorName[2]; ?>"></td>
			<td><input type="text" size="5" name="textA02" value="<?php echo $rangeZero[2]; ?>"></td>
			<td><input type="text" size="5" name="textB02" value="<?php echo $rangeFull[2]; ?>"></td>
			<td><input type="text" size="5" name="textC02" value="<?php echo $mistUnit[2]; ?>"></td>
			<td><input type="text" size="5" name="textD02" value="<?php echo $preAlarm[2]; ?>"></td>
			<td><input type="text" size="5" name="textE02" value="<?php echo $highAlarm[2]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="3" <?php print ($checkStatus[3]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[3]; ?><input type="hidden" name="sensName03" value="<?php echo $sensorName[3]; ?>"></td>
			<td><input type="text" size="5" name="textA03" value="<?php echo $rangeZero[3]; ?>"></td>
			<td><input type="text" size="5" name="textB03" value="<?php echo $rangeFull[3]; ?>"></td>
			<td><input type="text" size="5" name="textC03" value="<?php echo $mistUnit[3]; ?>"></td>
			<td><input type="text" size="5" name="textD03" value="<?php echo $preAlarm[3]; ?>"></td>
			<td><input type="text" size="5" name="textE03" value="<?php echo $highAlarm[3]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="4" <?php print ($checkStatus[4]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[4]; ?><input type="hidden" name="sensName04" value="<?php echo $sensorName[4]; ?>"></td>
			<td><input type="text" size="5" name="textA04" value="<?php echo $rangeZero[4]; ?>"></td>
			<td><input type="text" size="5" name="textB04" value="<?php echo $rangeFull[4]; ?>"></td>
			<td><input type="text" size="5" name="textC04" value="<?php echo $mistUnit[4]; ?>"></td>
			<td><input type="text" size="5" name="textD04" value="<?php echo $preAlarm[4]; ?>"></td>
			<td><input type="text" size="5" name="textE04" value="<?php echo $highAlarm[4]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="5" <?php print ($checkStatus[5]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[5]; ?><input type="hidden" name="sensName05" value="<?php echo $sensorName[5]; ?>"></td>
			<td><input type="text" size="5" name="textA05" value="<?php echo $rangeZero[5]; ?>"></td>
			<td><input type="text" size="5" name="textB05" value="<?php echo $rangeFull[5]; ?>"></td>
			<td><input type="text" size="5" name="textC05" value="<?php echo $mistUnit[5]; ?>"></td>
			<td><input type="text" size="5" name="textD05" value="<?php echo $preAlarm[5]; ?>"></td>
			<td><input type="text" size="5" name="textE05" value="<?php echo $highAlarm[5]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="6" <?php print ($checkStatus[6]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[6]; ?><input type="hidden" name="sensName06" value="<?php echo $sensorName[6]; ?>"></td>
			<td><input type="text" size="5" name="textA06" value="<?php echo $rangeZero[6]; ?>"></td>
			<td><input type="text" size="5" name="textB06" value="<?php echo $rangeFull[6]; ?>"></td>
			<td><input type="text" size="5" name="textC06" value="<?php echo $mistUnit[6]; ?>"></td>
			<td><input type="text" size="5" name="textD06" value="<?php echo $preAlarm[6]; ?>"></td>
			<td><input type="text" size="5" name="textE06" value="<?php echo $highAlarm[6]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="7" <?php print ($checkStatus[7]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[7]; ?><input type="hidden" name="sensName07" value="<?php echo $sensorName[7]; ?>"></td>
			<td><input type="text" size="5" name="textA07" value="<?php echo $rangeZero[7]; ?>"></td>
			<td><input type="text" size="5" name="textB07" value="<?php echo $rangeFull[7]; ?>"></td>
			<td><input type="text" size="5" name="textC07" value="<?php echo $mistUnit[7]; ?>"></td>
			<td><input type="text" size="5" name="textD07" value="<?php echo $preAlarm[7]; ?>"></td>
			<td><input type="text" size="5" name="textE07" value="<?php echo $highAlarm[7]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="8" <?php print ($checkStatus[8]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[8]; ?><input type="hidden" name="sensName08" value="<?php echo $sensorName[8]; ?>"></td>
			<td><input type="text" size="5" name="textA08" value="<?php echo $rangeZero[8]; ?>"></td>
			<td><input type="text" size="5" name="textB08" value="<?php echo $rangeFull[8]; ?>"></td>
			<td><input type="text" size="5" name="textC08" value="<?php echo $mistUnit[8]; ?>"></td>
			<td><input type="text" size="5" name="textD08" value="<?php echo $preAlarm[8]; ?>"></td>
			<td><input type="text" size="5" name="textE08" value="<?php echo $highAlarm[8]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="9" <?php print ($checkStatus[9]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[9]; ?><input type="hidden" name="sensName09" value="<?php echo $sensorName[9]; ?>"></td>
			<td><input type="text" size="5" name="textA09" value="<?php echo $rangeZero[9]; ?>"></td>
			<td><input type="text" size="5" name="textB09" value="<?php echo $rangeFull[9]; ?>"></td>
			<td><input type="text" size="5" name="textC09" value="<?php echo $mistUnit[9]; ?>"></td>
			<td><input type="text" size="5" name="textD09" value="<?php echo $preAlarm[9]; ?>"></td>
			<td><input type="text" size="5" name="textE09" value="<?php echo $highAlarm[9]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="10" <?php print ($checkStatus[10]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[10]; ?><input type="hidden" name="sensName10" value="<?php echo $sensorName[10]; ?>"></td>
			<td><input type="text" size="5" name="textA10" value="<?php echo $rangeZero[10]; ?>"></td>
			<td><input type="text" size="5" name="textB10" value="<?php echo $rangeFull[10]; ?>"></td>
			<td><input type="text" size="5" name="textC10" value="<?php echo $mistUnit[10]; ?>"></td>
			<td><input type="text" size="5" name="textD10" value="<?php echo $preAlarm[10]; ?>"></td>
			<td><input type="text" size="5" name="textE10" value="<?php echo $highAlarm[10]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="11" <?php print ($checkStatus[11]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[11]; ?><input type="hidden" name="sensName11" value="<?php echo $sensorName[11]; ?>"></td>
			<td><input type="text" size="5" name="textA11" value="<?php echo $rangeZero[11]; ?>"></td>
			<td><input type="text" size="5" name="textB11" value="<?php echo $rangeFull[11]; ?>"></td>
			<td><input type="text" size="5" name="textC11" value="<?php echo $mistUnit[11]; ?>"></td>
			<td><input type="text" size="5" name="textD11" value="<?php echo $preAlarm[11]; ?>"></td>
			<td><input type="text" size="5" name="textE11" value="<?php echo $highAlarm[11]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="12" <?php print ($checkStatus[12]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[12]; ?><input type="hidden" name="sensName12" value="<?php echo $sensorName[12]; ?>"></td>
			<td><input type="text" size="5" name="textA12" value="<?php echo $rangeZero[12]; ?>"></td>
			<td><input type="text" size="5" name="textB12" value="<?php echo $rangeFull[12]; ?>"></td>
			<td><input type="text" size="5" name="textC12" value="<?php echo $mistUnit[12]; ?>"></td>
			<td><input type="text" size="5" name="textD12" value="<?php echo $preAlarm[12]; ?>"></td>
			<td><input type="text" size="5" name="textE12" value="<?php echo $highAlarm[12]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="13" <?php print ($checkStatus[13]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[13]; ?><input type="hidden" name="sensName13" value="<?php echo $sensorName[13]; ?>"></td>
			<td><input type="text" size="5" name="textA13" value="<?php echo $rangeZero[13]; ?>"></td>
			<td><input type="text" size="5" name="textB13" value="<?php echo $rangeFull[13]; ?>"></td>
			<td><input type="text" size="5" name="textC13" value="<?php echo $mistUnit[13]; ?>"></td>
			<td><input type="text" size="5" name="textD13" value="<?php echo $preAlarm[13]; ?>"></td>
			<td><input type="text" size="5" name="textE13" value="<?php echo $highAlarm[13]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="14" <?php print ($checkStatus[14]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[14]; ?><input type="hidden" name="sensName14" value="<?php echo $sensorName[14]; ?>"></td>
			<td><input type="text" size="5" name="textA14" value="<?php echo $rangeZero[14]; ?>"></td>
			<td><input type="text" size="5" name="textB14" value="<?php echo $rangeFull[14]; ?>"></td>
			<td><input type="text" size="5" name="textC14" value="<?php echo $mistUnit[14]; ?>"></td>
			<td><input type="text" size="5" name="textD14" value="<?php echo $preAlarm[14]; ?>"></td>
			<td><input type="text" size="5" name="textE14" value="<?php echo $highAlarm[14]; ?>"></td>
		</tr>
		<tr>
			<td align="right"><input type="checkbox" name="check1[]" value="15" <?php print ($checkStatus[15]=="1") ? ' checked="checked"' : ''; ?>><?php echo $sensorName[15]; ?><input type="hidden" name="sensName15" value="<?php echo $sensorName[15]; ?>"></td>
			<td><input type="text" size="5" name="textA15" value="<?php echo $rangeZero[15]; ?>"></td>
			<td><input type="text" size="5" name="textB15" value="<?php echo $rangeFull[15]; ?>"></td>
			<td><input type="text" size="5" name="textC15" value="<?php echo $mistUnit[15]; ?>"></td>
			<td><input type="text" size="5" name="textD15" value="<?php echo $preAlarm[15]; ?>"></td>
			<td><input type="text" size="5" name="textE15" value="<?php echo $highAlarm[15]; ?>"></td>
		</tr>
		<tr>
			<td colspan="6">
				<input type="hidden" name="str1" value="str2"/>
				<input type="submit" value="　　SAVE　　" name="sub1">
			</td>
		</tr>
	</table>
</form>

</div><div class="gap60">
</div>
<!--Footer-->
<div class="footer">
<div>HARTING IIC MICA</div>
</div>
<!--END Footer--> 
</div></body></html>
