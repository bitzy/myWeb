<?php
session_start();
require_once dirname(__FILE__).'/global.php';
?><!DOCTYPE html>
<html>
<head>
<meta charset="UTF8">
<link rel="stylesheet" type="text/css" href="css/global.css"/>
<script src="js/jquery-1.11.3.min.js"></script>
</head>

<body style="margin:0; padding:0;width:100%;">
<div style="display:;">
<?php
$id 	= $_SESSION['id'];
//execute the system program & generate searchRes file

$searchRes	= "tmp/searchRes.txt";
//read search result from file
$imgs = array();
$myfile = fopen(getFileFullPath($searchRes), "r") 
	or die("unable to open file!");
$imgRootPath = getFileFullPath("img");
$resImgNum = trim(fgets($myfile));
for($i = 0; $i < $resImgNum; $i++) {
	$imgs[$i] = trim(fgets($myfile));	
}
fclose($myfile);
?>
</div>
<script>
$(document).ready(function() {
<?php if($resImgNum > 0) {?>	
	$('#searchResArea').css("display", "block");
	$('#searchTxtDiv').css("display", "none");
	var countPerRow = parseInt($('#searchResArea').width()/150);

	var imghtml = "<tbody>";
	var rowCount = 0;
<?php for($i = 0; $i < $resImgNum; $i++) {?>
	if(rowCount == 0) imghtml += '<tr>';
	imghtml += '<td><img style="width:140px;" src="';
	imghtml += '<?php echo "$imgRootPath/$imgs[$i]";?>';
	imghtml += '"/ ></td>';
	rowCount++;
	if(rowCount == countPerRow) { 
		imghtml += '</tr>';
		rowCount = 0;
	}
<?php }?>
	if(rowCount != countPerRow) {imghtml += '</tr>';}
	imghtml += "</tbody>";	
	$('#searchResTable').html(imghtml);
<?php } else { ?>
	$('#searchResArea').css("display", "none");
	$('#searchTxtDiv').css("display", "block");
<?php }?>
});
</script>
<hr>
<div id = "searchResArea">	
	<p class = "titleClass">检索结果：<span>匹配到<?php echo $resImgNum;?>/
		<?php system("ls $imgRootPath/*.jpg|wc -l");?>张图片.</span></p>
	<table id = "searchResTable"></table>
</div>
<p id = "searchTxtDiv" class = "titleClass">没有检索到匹配图片!</p>
</body>
</html>
