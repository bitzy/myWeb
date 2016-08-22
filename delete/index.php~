<!DOCTYPE html>
<?php require_once dirname(__FILE__).'/global.php';?>
<?php require_once dirname(__FILE__).'/imgData.php';?>
<html>
<head>
<meta charset="UTF8">
<link rel="stylesheet" type="text/css" href="css/global.css"/>
<link rel="stylesheet" type="text/css" href="css/index.css"/>
<script src="js/jquery-1.11.3.min.js"></script>
<script src="js/index.js"></script>
<title>基于属性分析的服装图像检索</title>
</head>

<body>
	<h1 id = "pageHead">基于属性分析的服装检索系统的研究与实现——展示平台</h1>
	<hr>
	<p id = "pageNote">注意：本网站用于展示服装属性分析及其检索结果。用户可以从本地选择人物服装图片（需提供姿态），即可查看到属性识别结果和检索结果。</p>

	<!-- 上半部分属性识别 -->
	<div id = "topContent">
		<form id = "inputForm" method = "post" enctype = "multipart/form-data">
			<p class = "titleClass">选择检索图片：</p>
			<div id = "fileDiv"><input type="file" name="file" id="file" accept="image/*" onchange="loadImg(); return false;"/></div>
			
			<div id = "beginDiv"><input type="button" name="begin" id="begin" onclick="recognize(); return false;" value="Recognize"/></div>
			<div id = "searchGDDiv"><input type="button" name="searchGD" id="searchGD" onclick="searchByGroundTruth(); return false;" value="GT Search"/></div>

			<div class = "imgArea"><img id = "imgView" alt="图片预览"/></div>
			<div id = "resDiv">
				<p class = "titleClass" id = "previewTxt">图片相关信息：</p>
				<p class = "titleClass" id = "noDataTxt">没有找到数据文件</p>			
				<table class = "table table-bordered" id="groundTruth">
				<thead>
				<tr><th class = "attrKey">属性</th><th class = "attrValue">属性值</th><th class = "attrValue">参考值</th></tr>
				</thead>
				<tbody>
				<tr><td>领型</td><td id = "attr1"></td><td id = "attr1ref">-</td></tr>
				<tr><td>衣长</td><td id = "attr2"></td><td id = "attr2ref"></td></tr>
				<tr><td>袖长</td><td id = "attr3"></td><td id = "attr3ref"></td></tr>
				<tr><td>纹理</td><td id = "attr4"></td><td id = "attr4ref">-</td></tr>
				<tr><td>颜色</td><td id = "attr5"></td><td id = "attr5ref">-</td></tr>
				</tbody>
				</table>
			</div>
		</form>	
	
		<!-- 右半部分 -->
		<iframe id = "recognizeFrame" name = "recognizeFrame" onload = "setIframeHeight(this)"></iframe>
		<p style="clear:both;"></p>
	</div>

	<!-- 检索结果显示面板（隐藏）-->
	<iframe id = "searchFrame" name = "searchFrame" onload = "setIframeHeight(this)"></iframe>

	<hr>
	<!-- 页面脚注 -->		
	<p id = "foot">Copyright&nbsp;&copy;&nbsp;2016&nbsp;&nbsp;GAIT</p>

	<iframe id = "dataFrame" name = "dataFrame"></iframe>
</body>
</html>
