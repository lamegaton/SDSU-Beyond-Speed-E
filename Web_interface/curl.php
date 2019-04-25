<!DOCTYPE html>
<html>
<body>
<h1>hi</h1>
<!-- snip -->
<p>vehicle count: </p>
<div id="all"></div>
<div id="test"></div>
<div id="dom-target" style="display: none;">
	<?php
		// Generated by curl-to-PHP: http://incarnate.github.io/curl-to-php/
		$ch = curl_init();

		curl_setopt($ch, CURLOPT_URL, 'https://iotfam_esp32_singlechannel.data.thethingsnetwork.org/api/v2/query?last=2d');
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
		curl_setopt($ch, CURLOPT_CUSTOMREQUEST, 'GET');


		$headers = array();
		$headers[] = 'Accept: application/json';
		$headers[] = 'Authorization: key ttn-account-v2.3QymOg8dnB5OCLdVo_qpkNHHnALWXFXHI2R6BmorGJo';
		curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);

		$result = curl_exec($ch);
		if (curl_errno($ch)) {
			echo 'Error:' . curl_error($ch);
		}
		
		curl_close ($ch);
		
		//$newstr =substr($result, 1 ,(strlen($result) - 3));
		//$data = json_encode($newstr);
		//echo $data;
		//echo $result;
		echo htmlspecialchars($result); /* You have to escape because the result
										will not be valid HTML otherwise. */
	?>        
</div>
<script>
	var div = document.getElementById("dom-target");
	var mydata = div.textContent;
	var myjson = JSON.parse(mydata);
	document.getElementById("test").innerHTML = myjson[0].presence_1;
	document.getElementById("all").innerHTML = mydata;
</script>

</body>
</html>