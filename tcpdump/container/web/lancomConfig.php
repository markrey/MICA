
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

	
	
	<!--Content-->
	<div id="content">

<?php
$filenameConfig = 'lancomConfig.ini';
$filenameInterfaces = '/etc/network/interfaces';

// 入力内容を表示する
$confValue = array("","","","","","","","","","","","","","","","");
$confName = array("","","","","","","","","","","","","","","","");
$idx=0;

if(@$_POST["confText00"]!=""){
	$confName[$idx] = @$_POST["confText00"];
	$confValue[$idx] = @$_POST["textA00"];
}
$idx++;
if(@$_POST["confText01"]!=""){
	$confName[$idx] = @$_POST["confText01"];
	$confValue[$idx] = @$_POST["textA01"];
}
$idx++;
if(@$_POST["confText02"]!=""){
	$confName[$idx] = @$_POST["confText02"];
	$confValue[$idx] = @$_POST["textA02"];
}
$idx++;
if(@$_POST["confText03"]!=""){
	$confName[$idx] = @$_POST["confText03"];
	$confValue[$idx] = @$_POST["textA03"];
}
$idx++;
if(@$_POST["confText04"]!=""){
	$confName[$idx] = @$_POST["confText04"];
	$confValue[$idx] = @$_POST["textA04"];
}
$idx++;
if(@$_POST["confText05"]!=""){
	$confName[$idx] = @$_POST["confText05"];
	$confValue[$idx] = @$_POST["textA05"];
}
$idx++;
if(@$_POST["confText06"]!=""){
	$confName[$idx] = @$_POST["confText06"];
	$confValue[$idx] = @$_POST["textA06"];
}
$idx++;
if(@$_POST["confText07"]!=""){
	$confName[$idx] = @$_POST["confText07"];
	$confValue[$idx] = @$_POST["textA07"];
}
$idx++;
if(@$_POST["confText08"]!=""){
	$confName[$idx] = @$_POST["confText08"];
	$confValue[$idx] = @$_POST["textA08"];
}
$idx++;
if(@$_POST["confText09"]!=""){
	$confName[$idx] = @$_POST["confText09"];
	$confValue[$idx] = @$_POST["textA09"];
}
$idx++;
if(@$_POST["confText10"]!=""){
	$confName[$idx] = @$_POST["confText10"];
	$confValue[$idx] = @$_POST["textA10"];
}
$idx++;
if(@$_POST["confText11"]!=""){
	$confName[$idx] = @$_POST["confText11"];
	$confValue[$idx] = @$_POST["textA11"];
}
$idx++;
if(@$_POST["confText12"]!=""){
	$confName[$idx] = @$_POST["confText12"];
	$confValue[$idx] = @$_POST["textA12"];
}
$idx++;
if(@$_POST["confText13"]!=""){
	$confName[$idx] = @$_POST["confText13"];
	$confValue[$idx] = @$_POST["textA13"];
}
$idx++;
if(@$_POST["confText14"]!=""){
	$confName[$idx] = @$_POST["confText14"];
	$confValue[$idx] = @$_POST["textA14"];
}
$idx++;
if(@$_POST["confText15"]!=""){
	$confName[$idx] = @$_POST["confText15"];
	$confValue[$idx] = @$_POST["textA15"];
}

$writeDataConfig = "";
for ($idx = 0; $idx < count($confName); $idx++) {
	if($confName[$idx]!=""){
		$writeDataConfig .= $confName[$idx]."=".$confValue[$idx]."\n";
	}
}

if(@$_POST["str1"]=="str2"){
	file_put_contents($filenameConfig, $writeDataConfig);
}

// 設定読み込み
$file = fopen($filenameConfig,"r");
$idx = 0;
while ($array = fgetcsv($file, 1000, "=")) {
	$idx = -1;
	if($array[0]=="DEVICENAME"){
		$idx = 0;
	}else if($array[0]=="BAUDRATE"){
		$idx = 1;
	}else if($array[0]=="DATABIT"){
		$idx = 2;
	}else if($array[0]=="STOPBIT"){
		$idx = 3;
	}else if($array[0]=="PARITY"){
		$idx = 4;
	}else if($array[0]=="NETMODE"){
		$idx = 5;
	}else if($array[0]=="IPADDRESS"){
		$idx = 6;
	}else if($array[0]=="SUBNETMASK"){
		$idx = 7;
	}else if($array[0]=="GATEWAYADDRESS"){
		$idx = 8;
	}else if($array[0]=="DNSADDRESS"){
		$idx = 9;
	}else if($array[0]=="BROADCAST"){
		$idx = 10;
	}

	if(idx>=0){
		$confName[$idx] = $array[0];
		$confValue[$idx] = $array[1];
	}
}
fclose($file);



?>
<form method="POST" action="lancomConfig.php">
	<table border="15" bordercolor="white" width="70%">
		<tr>
			<th width="30%" align="left">Name</th>
			<th width="70%" align="left">Value</th>
		</tr>
		<tr>
			<td><?php echo $confName[0]; ?><input type="hidden" name="confText00" value="<?php echo $confName[0]; ?>"></td>
			<td><input type="hidden" name="textA00" value="<?php echo $confValue[0]; ?>"><?php echo $confValue[0]; ?></td>
		</tr>
		<tr>
			<td><?php echo $confName[1]; ?><input type="hidden" name="confText01" value="<?php echo $confName[1]; ?>"></td>
			<td>
				<select name="textA01" style="width:200px;">
					<option value="1200" <?php if($confValue[1]=="1200") {print " selected";}?>>1200</option>
					<option value="2400" <?php if($confValue[1]=="2400") {print " selected";}?>>2400</option>
					<option value="4800" <?php if($confValue[1]=="4800") {print " selected";}?>>4800</option>
					<option value="9600" <?php if($confValue[1]=="9600") {print " selected";}?>>9600</option>
					<option value="14400" <?php if($confValue[1]=="14400") {print " selected";}?>>14400</option>
					<option value="19200" <?php if($confValue[1]=="19200") {print " selected";}?>>19200</option>
					<option value="38400" <?php if($confValue[1]=="38400") {print " selected";}?>>38400</option>
					<option value="57600" <?php if($confValue[1]=="57600") {print " selected";}?>>57600</option>
					<option value="115200" <?php if($confValue[1]=="115200") {print " selected";}?>>115200</option>
					<option value="128000" <?php if($confValue[1]=="128000") {print " selected";}?>>128000</option>
					<option value="256000" <?php if($confValue[1]=="256000") {print " selected";}?>>256000</option>
				</select> 
			</td>
		</tr>
		<tr>
			<td><?php echo $confName[2]; ?><input type="hidden" name="confText02" value="<?php echo $confName[2]; ?>"></td>
			<td>
				<select name="textA02" style="width:200px;">
					<option value="7" <?php if($confValue[2]=="7") {print " selected";}?>>7</option>
					<option value="8" <?php if($confValue[2]=="8") {print " selected";}?>>8</option>
				</select> 
			</td>
		</tr>
		<tr>
			<td><?php echo $confName[3]; ?><input type="hidden" name="confText03" value="<?php echo $confName[3]; ?>"></td>
			<td>
				<select name="textA03" style="width:200px;">
					<option value="1" <?php if($confValue[3]=="1") {print " selected";}?>>1</option>
					<option value="2" <?php if($confValue[3]=="2") {print " selected";}?>>2</option>
				</select> 
			</td>
		</tr>
		<tr>
			<td><?php echo $confName[4]; ?><input type="hidden" name="confText04" value="<?php echo $confName[4]; ?>"></td>
			<td>
				<select name="textA04" style="width:200px;">
					<option value="ODD" <?php if($confValue[4]=="ODD") {print " selected";}?>>ODD</option>
					<option value="EVEN" <?php if($confValue[4]=="EVEN") {print " selected";}?>>EVEN</option>
					<option value="NONE" <?php if($confValue[4]=="NONE") {print " selected";}?>>NONE</option>
				</select> 
			</td>
		</tr>
		<tr>
			<td><?php echo $confName[5]; ?><input type="hidden" name="confText05" value="<?php echo $confName[5]; ?>"></td>
			<td>
				<select name="textA05" style="width:200px;">
					<option value="static" <?php if($confValue[5]=="static") {print " selected";}?>>static</option>
					<!-- <option value="DHCP" <?php if($confValue[5]=="DHCP") {print " selected";}?>>DHCP</option> -->
				</select> 
			</td>
		</tr>
		<tr>
			<td><?php echo $confName[6]; ?><input type="hidden" name="confText06" value="<?php echo $confName[6]; ?>"></td>
			<td><input type="text" name="textA06" value="<?php echo $confValue[6]; ?>"></td>
		</tr>
		<tr>
			<td><?php echo $confName[7]; ?><input type="hidden" name="confText07" value="<?php echo $confName[7]; ?>"></td>
			<td><input type="text" name="textA07" value="<?php echo $confValue[7]; ?>"></td>
		</tr>
		<!--
		<tr>
			<td><?php echo $confName[8]; ?><input type="hidden" name="confText08" value="<?php echo $confName[8]; ?>"></td>
			<td><input type="text" name="textA08" value="<?php echo $confValue[8]; ?>"></td>
		</tr>
		<tr>
			<td><?php echo $confName[9]; ?><input type="hidden" name="confText09" value="<?php echo $confName[9]; ?>"></td>
			<td><input type="text" name="textA09" value="<?php echo $confValue[9]; ?>"></td>
		</tr>
		<tr>
			<td><?php echo $confName[10]; ?><input type="hidden" name="confText10" value="<?php echo $confName[10]; ?>"></td>
			<td><input type="text" name="textA10" value="<?php echo $confValue[10]; ?>"></td>
		</tr>
		-->
		<tr>
			<td>
				<input type="hidden" name="confText08" value="<?php echo $confName[8]; ?>"> <input type="hidden" name="textA08" value="<?php echo $confValue[8]; ?>">
				<input type="hidden" name="confText09" value="<?php echo $confName[9]; ?>"> <input type="hidden" name="textA09" value="<?php echo $confValue[9]; ?>">
				<?php echo $confName[10]; ?><input type="hidden" name="confText10" value="<?php echo $confName[10]; ?>">
			</td>
			<td><input type="text" name="textA10" value="<?php echo $confValue[10]; ?>"></td>
		</tr>

		<tr>
			<td colspan="6">
				<input type="hidden" name="str1" value="str2"/>
				<input type="submit" value="　　SAVE　　" name="sub1">
			</td>
		</tr>
	</table>
</form>
	<div id="inner_content">


</div><div class="gap60">
</div>
<!--Footer-->
<div class="footer">
<div>HARTING IIC MICA</div>
</div>
<!--END Footer--> 
</div></body></html>
