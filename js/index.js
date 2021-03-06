function loadImg()
{
	var input = document.getElementById("file");
	var file = input.files[0];
	var filename = file.name;

	if(typeof FileReader == 'undefined') {
		tipsArea.innerHTML("FileReader not support!");
		input.setAttribute('disabled', 'disabled');
		return;
	} else {
		var reader = new FileReader();
		reader.onload = function(e) {
			document.getElementById('imgView').src = e.target.result;

			//read data from xml;
			var form = document.getElementById("inputForm");			
			form.target = "dataFrame";			
			form.action = "getXmlData.php";
			form.submit();	
		}
		reader.readAsDataURL(file);
	}
}

function recognize()
{
	var form = document.getElementById("inputForm");
	form.target="recognizeFrame";
	form.action="imgRecognize.php";
	form.submit();
}

function setIframeHeight(iframe) 
{
	if(iframe) {
		var iframeWin = iframe.contentWindow||iframe.contentDocument.parentWindow;
		if(iframeWin.document.body) {
			iframe.height = iframeWin.document.documentElement.scrollHeight
			||iframeWin.document.body.scrollHeight;
		}
	}
}

function searchByGroundTruth() 
{
	var form = document.getElementById("inputForm");
	form.target="searchFrame";
	form.action="imgSearch.php";
	form.submit();
	document.getElementById("searchFrame").style.visibility = "visible";
}
