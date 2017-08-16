
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
		<div class="headertitle">CCIEF MASTER CONFIGURATION</div>
		<div class="headerlogo"><img src="images/cclink_fld.jpg" width="320" height="41"  alt=""/></div>
		</div>
		
		
	</div>	
	
	
	<!--Content-->
	<div id="content">
	
	<div id="inner_content">

<?php
$filenameRange = 'MasterParameter.csv';

// 入力内容を表示する
$postId = array("","","","","","","","","","","","","","","","");
$postData = array("","","","","","","","","","","","","","","","");
$postComment = array("","","","","","","","","","","","","","","","");

$idx = 0;
$currentAttr = "";
$colSize = 0;
$intId = 0;
for($idx=0;$idx<count(@$_POST["textA"]);$idx++) {
	if (@$_POST["textA"][$idx] == "Group") {
		$currentAttr = "Group";
	} else if (@$_POST["textA"][$idx] == "Slave"){
		$currentAttr = "Slave";
	}
	if(($currentAttr == "Group") && (isset($_POST['sub2'])) && (@$_POST["textA"][$idx] == "")&& (@$_POST["textB"][$idx] == "")&& (@$_POST["textC"][$idx] == "")) {
		//add groupボタン
		$intId++;
	    $postId[$colSize] = $intId++;
	    $postData[$colSize] = "";
	    $postComment[$colSize] = "Group Cyclic transmission timeout";
	    $colSize++;
	    $postId[$colSize] = $intId++;
	    $postData[$colSize] = "";
	    $postComment[$colSize] = "Group Count of cyclic transmission timeout";
	    $colSize++;
	    $postId[$colSize] = $intId++;
	    $postData[$colSize] = "";
	    $postComment[$colSize] = "Group Constant link scan time";
	    $colSize++;
	}
	$postId[$colSize] = @$_POST["textA"][$idx];
	$intId = intval($postId[$colSize]);
	$postData[$colSize] = @$_POST["textB"][$idx];
	$postComment[$colSize] = @$_POST["textC"][$idx];
	$colSize++;
}


if (isset($_POST['sub1'])) {
	//saveボタン
	$writeDataRange = "";
	for ($i = 0; $i < count($postId); $i++) {
	    $writeDataRange .= $postId[$i].",".$postData[$i].",".$postComment[$i]."\n";
	}
	if(@$_POST["str1"]=="str2"){
		file_put_contents($filenameRange, $writeDataRange);
	}
	// センサレンジ設定読み込み
	$file = fopen($filenameRange,"r");
	$idx = 0;
	while ($array = fgetcsv($file, 1000, ",")) {
	    $postId[$idx] = $array[0];
	    $postData[$idx] = $array[1];
	    $postComment[$idx] = $array[2];
	    $idx += 1;
	}
	$colSize = $idx;
}elseif ( isset($_POST['sub2']) ){
	//add groupボタン
}elseif ( isset($_POST['sub3']) ){
	//add slaveボタン
		$intId++;
	    $postId[$colSize] = $intId++;
	    $postData[$colSize] = "";
	    $postComment[$colSize] = "Slave IP address";
	    $colSize += 1;
	    $postId[$colSize] = $intId++;
	    $postData[$colSize] = "";
	    $postComment[$colSize] = "Slave Number of occupied stations";
	    $colSize += 1;
	    $postId[$colSize] = $intId++;
	    $postData[$colSize] = "";
	    $postComment[$colSize] = "Slave Number of group";
	    $colSize += 1;
}else {
	// 初回読込
	// センサレンジ設定読み込み
	$file = fopen($filenameRange,"r");
	$idx = 0;
	while ($array = fgetcsv($file, 1000, ",")) {
	    $postId[$idx] = $array[0];
	    $postData[$idx] = $array[1];
	    $postComment[$idx] = $array[2];
	    $idx += 1;
	}
	$colSize = $idx;
}

fclose($file);


?>
<form method="POST" action="masterSetting.php">
	<table border="10" bordercolor="white" width="80%">
		<tr>
			<th width="15%"></th>
			<th width="20%"></th>
			<th width="50%"></th>
			<th width="15%"></th>
		</tr>
<?php 
	$currentAttr = "";
	for($idx=0;$idx<$colSize;$idx++) {
		if(($postId[$idx]=="") && ($postData[$idx]=="") && ($postComment[$idx]=="")) {
			print "\t\t<tr>\n";
			print "\t\t\t<td colspan='4'>\n";
			print "\t\t\t\t<input type='hidden' name='textA[]' value='".$postId[$idx]."'>\n";
			print "\t\t\t\t<input type='hidden' name='textB[]' value='".$postData[$idx]."'>\n";
			print "\t\t\t\t<input type='hidden' name='textC[]' value='".$postComment[$idx]."'>\n";
			print "\t\t\t</td>\n";
			print "\t\t</tr>\n";
			$currentAttr = "";
		} else if((($postId[$idx]=="CCIEF-BASIC Master Sample Parameter") || ($postId[$idx]=="Group") || ($postId[$idx]=="Slave")) && ($postData[$idx]=="") && ($postComment[$idx]=="")) {
			print "\t\t<tr>\n";
			print "\t\t\t<td colspan='4'>\n";
			print "\t\t\t\t<input type='hidden' name='textA[]' value='".$postId[$idx]."'>\n";
			print "\t\t\t\t<input type='hidden' name='textB[]' value='".$postData[$idx]."'>\n";
			print "\t\t\t\t<input type='hidden' name='textC[]' value='".$postComment[$idx]."'>\n";
			print "\t\t\t\t".$postId[$idx]."\n";
			print "\t\t\t</td>\n";
			print "\t\t</tr>\n";
			$currentAttr = $postId[$idx];
		} else if(($postId[$idx]=="ID") && ($postData[$idx]=="DATA") && ($postComment[$idx]=="COMMENT")) {
			print "\t\t<tr>\n";
			print "\t\t\t<th><input type='hidden' name='textA[]' value='".$postId[$idx]."'>".$postId[$idx]."</th>\n";
			print "\t\t\t<th><input type='hidden' name='textB[]' value='".$postData[$idx]."'>".$postData[$idx]."</th>\n";
			print "\t\t\t<th><input type='hidden' name='textC[]' value='".$postComment[$idx]."'>".$postComment[$idx]."</th>\n";
			print "\t\t\t<th> </th>\n";
			print "\t\t</tr>\n";
		} else {
			print "\t\t<tr>\n";
			print "\t\t\t<td><input type='text' size='5' name='textA[]' value='".$postId[$idx]."'></td>\n";
			print "\t\t\t<td><input type='text' size='10' name='textB[]' value='".$postData[$idx]."'></td>\n";
			print "\t\t\t<td><input type='text' size='40' name='textC[]' value='".$postComment[$idx]."'></td>\n";
			if(($currentAttr == "Group") && (((intval($postId[$idx])-3) % 3)==0)){
				print "\t\t\t<td><input type='submit' value='DEL' name='delGr".$postId[$idx]."'></td>\n";
			} else if(($currentAttr == "Slave") && (((intval($postId[$idx])-2) % 3)==0)){
				print "\t\t\t<td><input type='submit' value='DEL' name='delSl".$postId[$idx]."'></td>\n";
			} else {
				print "\t\t\t<td> </td>\n";
			}
			print "\t\t</tr>\n";
		}
	}
?>
		<tr>
			<td colspan="3">
				<input type="hidden" name="str1" value="str2"/>
				<input type="submit" value="　　SAVE　　" name="sub1">
				<input type="submit" value="　　Add Group　　" name="sub2">
				<input type="submit" value="　　Add Slave　　" name="sub3">
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
