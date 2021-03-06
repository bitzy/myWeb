<?php
session_start();
require_once dirname(__FILE__).'/global.php';
require_once dirname(__FILE__).'/imgData.php';

//$id = $_SERVER["REMOTE_ADDR"]."-".$_SERVER['REMOTE_PORT'];
$id = 100;
$_SESSION['id'] = $id;
$exe_img	= $GrunPath."/input/$id.jpg";	//exe_img: input/100.jpg
if (!isset($_FILES['file']) || !move_uploaded_file($_FILES['file']['tmp_name'], $exe_img)) { 
	exit(1);
}

$fname = $_FILES["file"]["name"];		//fname: 111.jpg
$xmlfname = $GrunPath."/data/".substr($fname, 0, strpos($fname, '.')).".xml";	//xmlfname: data/100.xml
$doc = new DOMDocument();

if($doc->load($xmlfname) == FALSE) {	
	$_SESSION['SxmlDataExist'] = 0;
} else {
	$_SESSION['SxmlDataExist'] = 1;
	//load data:
	$_SESSION['Phead'] = getValue($doc, 'head');
	$_SESSION['Pneck'] = getValue($doc, 'neck');
	$_SESSION['Pleft_shoulder'] = getValue($doc, 'left_shoulder');
	$_SESSION['Pleft_elbow'] = getValue($doc, 'left_elbow');
	$_SESSION['Pleft_hand'] = getValue($doc, 'left_hand');
	$_SESSION['Pright_shoulder'] = getValue($doc, 'right_shoulder');
	$_SESSION['Pright_elbow'] = getValue($doc, 'right_elbow');
	$_SESSION['Pright_hand'] = getValue($doc, 'right_hand');
	$_SESSION['Pleft_hip'] = getValue($doc, 'left_hip');
	$_SESSION['Pleft_knee'] = getValue($doc, 'left_knee');
	$_SESSION['Pleft_ankle'] = getValue($doc, 'left_ankle');
	$_SESSION['Pright_hip'] = getValue($doc, 'right_hip');
	$_SESSION['Pright_knee'] = getValue($doc, 'right_knee');
	$_SESSION['Pright_ankle'] = getValue($doc, 'right_ankle');
	$_SESSION['AColor1'] = getValue($doc, 'Color1');
	if($_SESSION['AColor1'] != NULL) {
		$_SESSION['AColor1'] = getColor($_SESSION['AColor1']);
	}
	$_SESSION['AColor2'] = getValue($doc, 'Color2');
	if($_SESSION['AColor2'] != NULL) {
		$_SESSION['AColor2'] = getColor($_SESSION['AColor2']);		
	}
	$_SESSION['AColor3'] = getValue($doc, 'Color3');
	if($_SESSION['$AColor3'] != NULL) {
		$_SESSION['AColor3'] = getColor($_SESSION['AColor3']);		
	}
	$_SESSION['Acollar'] = getValue($doc, 'collar');
	$_SESSION['Asleeve'] = getValue($doc, 'sleeve');
	$_SESSION['AsleeveValue'] = getValue($doc, 'sleeveValue');
	$_SESSION['Aup_length'] = getValue($doc, 'up_length');
	$_SESSION['AlenthValue'] = getValue($doc, 'lenthValue');
	$_SESSION['Atexture'] = getValue($doc, 'texture');
}
?>

<script src="js/jquery-1.11.3.min.js"></script>
<script>
function loadImgSuccess() {
	$('#searchGDDiv', parent.document).css("display", "block");
	$('#resDiv', parent.document).css("display", "block");
	$('#previewTxt', parent.document).css("display", "block");
	$('#noDataTxt', parent.document).css("display", "none");
	$('#groundTruth', parent.document).css("display", "block");
	$('#beginDiv', parent.document).css("display", "block");
}
function loadImgFailed() {
	$('#resDiv', parent.document).css("display", "block");
	$('#previewTxt', parent.document).css("display", "none");
	$('#noDataTxt', parent.document).css("display", "block");
	$('#groundTruth', parent.document).css("display", "none");
	$('#beginDiv', parent.document).css("display", "block");
	$('#searchGDDiv', parent.document).css("display", "none");
}<?php if($_SESSION['SxmlDataExist'] === 1) { ?>
	loadImgSuccess();
	$('#attr1', parent.document).text('<?php echo $Gcollar[$_SESSION["Acollar"]];?>');

	var clothlenhtml = '<?php echo $Gup_length[$_SESSION["Aup_length"]];?>';
	$('#attr2', parent.document).text(clothlenhtml);
	$('#attr2ref', parent.document).text('<?php echo $_SESSION["AlenthValue"];?>%');

	var sleevehtml = '<?php echo $Gsleeve[$_SESSION["Asleeve"]];?>';
	$('#attr3', parent.document).text(sleevehtml);
	$('#attr3ref', parent.document).text('<?php echo $_SESSION["AsleeveValue"];?>%');

	$('#attr4', parent.document).text('<?php echo $Gtexture[$_SESSION["Atexture"]];?>');

	var colorhtml = '<table><tbody><tr style="border:none;">';
	<?php if($_SESSION['AColor1'] != NULL) { ?>
	colorhtml = colorhtml + '<td width="55" height="20" bgcolor="#';
	colorhtml = colorhtml + '<?php echo $_SESSION["AColor1"];?>';
	colorhtml = colorhtml + '"></td>';
	<?php } if($_SESSION['AColor2'] != NULL) {?>
	colorhtml += '<td width="55" height="20" bgcolor="#';
	colorhtml += '<?php echo $_SESSION["AColor2"];?>';
	colorhtml += '"></td>';
	<?php } if($_SESSION['AColor3'] != NULL) {?>
	colorhtml += '<td width="55" height="20" bgcolor="#';
	colorhtml += '<?php echo $_SESSION["AColor3"];?>';
	colorhtml += '"></td>';<?php }?>
	colorhtml += '</tr></tbody></table>';
	$('#attr5', parent.document).html(colorhtml);
<?php } else {?>
	loadImgFailed();
<?php }?>
</script>
<?php
function getValue($file, $attr) 
{
	$values = $file->getElementsByTagName($attr);
	$value = $values->item(0)->nodeValue;
	return trim($value);
}

function getColor($AColor1) {
	$r = ($AColor1 >> 16)&0xff;
		$r = base_convert($r, 10, 16);
		$g = ($AColor1 >> 8)&0xff;
		$g = base_convert($g, 10, 16);
		$b = ($AColor1)&0xff;
		$b = base_convert($b, 10, 16);
		$res = $b.$g.$r;
 return $res;
}
?>
