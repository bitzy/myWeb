<?php
session_start();
require_once dirname(__FILE__).'/global.php';
require_once dirname(__FILE__).'/imgData.php';
?><!DOCTYPE html>
<html>
<head>
<meta charset="UTF8">
<link rel="stylesheet" type="text/css" href="css/global.css"/>
<link rel="stylesheet" type="text/css" href="css/imgRecognize.css"/>
<script src="js/jquery-1.11.3.min.js"></script>
</head>

<body>
<div style="display:none;">
<?php
$id = $_SESSION['id'];
$SxmlDataExist = $_SESSION['SxmlDataExist'];

//GrunPath:
$exe_img	= "input/$id.jpg";	//exe_img: input/111.jpg
$pos_img 	= "tmp/pos.jpg";	//pos_img: tmp/111-pos.jpg
$res_collar	= "tmp/collar";
$res_sleeve	= "tmp/sleeve";
$res_uplenth	= "tmp/uplenth";
$res_color	= "tmp/color";
$res_texture	= "tmp/texture";
$midRes_len	= "tmp/attrres.continuslen";

//============================== pos data ====================================
//================= Attention: pos data has special layout ! =================
//======================= 0testDataForm.config ===============================
$pos_data = "tmp/posTmp.txt";
if($SxmlDataExist === 1) {
	//write $variable to posDataFile;	
	$myfile = fopen(getFileFullPath($pos_data), "w") 
		or die("unable to open file!");	
	fwrite($myfile, $_SESSION['Phead']."\n");
	fwrite($myfile, $_SESSION['Pneck']."\n");	
	fwrite($myfile, $_SESSION['Pright_shoulder']."\n");
	fwrite($myfile, $_SESSION['Pright_elbow']."\n");
	fwrite($myfile, $_SESSION['Pright_hand']."\n");
	fwrite($myfile, $_SESSION['Pleft_shoulder']."\n");
	fwrite($myfile, $_SESSION['Pleft_elbow']."\n");
	fwrite($myfile, $_SESSION['Pleft_hand']."\n");
	fwrite($myfile, $_SESSION['Pright_hip']."\n");
	fwrite($myfile, $_SESSION['Pleft_hip']."\n");
	fwrite($myfile, $_SESSION['Pright_knee']."\n");
	fwrite($myfile, $_SESSION['Pleft_knee']."\n");
	fwrite($myfile, $_SESSION['Pright_ankle']."\n");
	fwrite($myfile, $_SESSION['Pleft_ankle']."\n");
	fclose($myfile);
} else {	
	//execute program to generate the poseDataFile;
}
getPosImage($exe_img, $pos_img, $pos_data);
//============================== attr get ====================================
getAttrRes($exe_img, $pos_data, $res_collar, $res_sleeve, 
	$res_uplenth, $res_color, $res_texture);

//collar:
$collarValue = $Gcollar[getAttrValue($res_collar)];
$uplenthNum  = getAttrValue($res_uplenth);
$uplenthValue;
if($uplenthNum < 30) $uplenthValue = 'short';
else if($uplenthNum < 55) $uplenthValue = 'normal';
else if($uplenthNum < 75) $uplenthValue = 'mid_long';
else $uplenthValue = 'long';

//sleeve:
$sleeveNum   = getAttrValue($res_sleeve);
$sleeveValue;
if($sleeveNum < 5) $sleeveValue = 's_no';
else if($sleeveNum < 40) $sleeveValue = 's_short';
else if($sleeveNum < 60) $sleeveValue = 's_half';
else $sleeveValue = 's_long';

//color:
$colorValue = array();
$num = getColorValue($res_color);
for($i = 0; $i < $num; $i++)
	echo $colorValue[$i][0].":".$colorValue[$i][1]."\n";
?>
</div>
<script>
$(document).ready(function() {
<?php $poseFileName = getFileFullPath($pos_img);
	if(file_exists($poseFileName)) {?>	
	$('#resTxtDiv').css("display", "none");
	$('#recognizeArea').css("display", "block");
	$('#resImg').attr("src", "<?php echo $poseFileName;?>");
<?php } else { ?>
	$('#resTxtDiv').css("display", "block");
	$('#recognizeArea').css("display", "none");
<?php }?>
//get attrRes:
$('#Res2').text('<?php echo $Gup_length[$uplenthValue];?>');
$('#Res3').text('<?php echo $Gsleeve[$sleeveValue];?>');
$('#Res4').text($('#attr4', parent.document).html());
//color show set:
<?php
if($num == 0) {?>
	$('#rw5').html('<img src="css/2.png" class="flagImg"/ >');
<?php } else { ?>
	var colorReshtml = '<table><tbody><tr style="border:none;">';
<?php 	for($i = 0; $i < $num; $i++) { ?>
	colorReshtml += '<td width="<?php echo $colorValue[$i][0];?>"';
	colorReshtml += 'height="20" bgcolor="#';
	colorReshtml += '<?php echo $colorValue[$i][1];?>';
	colorReshtml += '"></td>';
<?php }?>
	colorReshtml += '</tr></tbody></table>';
	$('#Res5').html(colorReshtml);
	$('#rw5').html('<img src="css/1.png" class="flagImg"/ >');
<?php	
}?>
for(var i = 1; i <= 4; i++) {
	var resId = '#Res' + i;
	var gtres = '#attr'+ i;
	var flag = '#rw' + i;
	//alert($(resId).text() +"***"+ $(gtres, parent.document).text());
	if($(resId).text() == $(gtres, parent.document).text()) {	
		var htmlValue = '<img src="css/1.png" class = "flagImg"/ >';	
		$(flag).html(htmlValue);
	} else {
		var htmlValue = '<img src="css/2.png" class = "flagImg"/ >';
		$(flag).html(htmlValue);
	}
}
});

function search() {
	$("#inputForm", parent.document).attr("target", "searchFrame");
	$("#inputForm", parent.document).attr("action", "imgSearch.php");
	$("#inputForm", parent.document).submit();
	$("#searchFrame", parent.document).css("visibility", "visible");
}
</script>
<div id = "recognizeArea">
	<p class = "titleClass">属性识别：</p>
	<div id = "searchDiv"><input type="button" name="searchBegin" id="searchBegin" onclick="search(); return false;" value = "Search By Attributions"/></div>
	<div class = "imgArea" id = "resImgDiv"><img id = "resImg" alt="姿态识别结果"/></div>
	<div id = "recogResDiv">	
		<p class = "titleClass">属性识别值:</p>
		<table class = "table table-bordered" id = "recognizeAttr">
		<thead>
		<tr>
			<th class = "attrKey">属性</th>
			<th class = "attrValue">属性值</th>
			<th class = "attrValue">正确与否</th>
			<th class = "attrValue">参考值</th>
		</tr>
		</thead>
		<tbody>
		<tr>
			<td>领型</td><td id = 'Res1'><?php echo $collarValue;?></td><td id = 'rw1'></td>
			<td id = 'Res1ref'>-</td>
		</tr>
		<tr>
			<td>衣长</td><td id = 'Res2'></td><td id = 'rw2'></td>
			<td id = 'Res2ref'><?php echo $uplenthNum;?>%</td>
		</tr>
		<tr>
			<td>袖长</td><td id = 'Res3'></td><td id = 'rw3'></td>
			<td id = 'Res3ref'><?php echo $sleeveNum;?>%</td>
		</tr>
		<tr>
			<td>纹理</td><td id = 'Res4'></td><td id = 'rw4'></td>
			<td id = 'Res4ref'>-</td>
		</tr>
		<tr>
			<td>颜色</td><td id = 'Res5'></td><td id = 'rw5'></td>
			<td id = 'Res5ref'>-</td>
		</tr>
		</tbody>
		</table>
	</div>	
</div>
<p id = "resTxtDiv">无可用姿态数据,请重新选择图片.</p>
</body>
</html>
<?php
//draw pose of $data to $img
//store the posImage to $outPath
function getPosImage($img, $outPath, $data) {
	global $GexePath;
	global $GrunPath;
	$exe = getAbsoluteFPath($GexePath)."/pose/drawPos";
	$cmd = "$exe $img $outPath $data";	
	//======================================================
	$pwd = getcwd(); 
	$runFullPath = getAbsoluteFPath($GrunPath);
	chdir($runFullPath); 
	system($cmd);	
	chdir($pwd);
	//======================================================
}

//execute the program to recognize the attrs
//store the attrRes to the special file
function getAttrRes($img, $posFile, $fattr1, $fattr2, $fattr3, $fattr4, $fattr5) {
	global $GexePath;
	global $GrunPath;	
	$exe = getAbsoluteFPath($GexePath)."/attr/attr";
	$cmd = "$exe $img $posFile $fattr1 $fattr2 $fattr3 $fattr4 $fattr5";		
	//======================================================
	$pwd = getcwd(); 
	$runFullPath = getAbsoluteFPath($GrunPath);
	chdir($runFullPath);
	system($cmd);
	chdir($pwd);
	//======================================================
}

//get value from file
function getAttrValue($fpath) {
	$fFullPath = getFileFullPath($fpath);
	$myfile = fopen($fFullPath, "r");
	$res;
	if($myfile == FALSE) $res = "NULL";
	else $res = trim(fgets($myfile));
	fclose($myfile);
	return $res;
}

//get color from file
function getColorValue($fpath) {
	global $colorValue;
	$fFullPath = getFileFullPath($fpath);

	$num = 0;
	$count = 0;
	$myfile = fopen($fFullPath, "r");
	if($myfile == TRUE) {
		//echo "open success\n";
		for($i = 0; $i < 5; $i++) {
			$line = trim(fgets($myfile));
			//echo $line."\n";
			sscanf($line, "%d%d%d%d", $percent, $r, $g, $b);
			$count += $percent;
			$num++;
			$width = (int)($percent);
			$r = base_convert($r, 10, 16);
			$g = base_convert($g, 10, 16);
			$b = base_convert($b, 10, 16);
			$rgb = $r.$g.$b;
			$colorValue[$i] = array($width, $rgb);		
			if($count > 80) break;
		}
	}
	fclose($myfile);
	return $num;
}
?>
