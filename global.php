<?php
$Gcollar = array(
	"Round" => "圆领",
	"V-collar"=>"V领",
	"F-collar"=>"翻领");
$Gsleeve = array(
	"s_no" => "无袖",
	"s_short"=>"短袖",
	"s_half"=>"中长袖",
	"s_long"=>"长袖");
$Gup_length = array(
	"short"=>"短款",
	"normal"=>"常规",
	"mid_long"=>"中长",
	"long"=>"长款");
$Gtexture = array(
	"Single"=>"纯色",
	"Vertical"=>"竖条纹",
	"Horizontal"=>"横条纹",
	"Grid"=>"格子",
	"Point"=>"波点",
	"Leopard"=>"豹纹",
	"Other"=>"其他");
$GexePath	= $_SERVER['PATH_TRANSLATED']."WebExe";
$GrunPath	= $_SERVER['PATH_TRANSLATED']."WebData";
$GrootPath 	= dirname($_SERVER['SCRIPT_FILENAME']);
//dirname(__FILE__): /var/www/html/
//$_SERVER['PATH_TRANSLATED']: /
//$_SERVER['DOCUMENT_ROOT']: /var/www/html/

//eg: WebData/input/a.jpg
function getFileFullPath($fileName) {
	global $GrunPath;
	return $GrunPath."/".$fileName;
}

//eg:/var/www/html/myWeb/WebData/input/a.jpg
function getAbsoluteFPath($fileName) {
	global $GrootPath;
	return $GrootPath."/".$fileName;
}
?>
