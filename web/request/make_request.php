<?php
require_once "../util.php";
require_once "../connection.php";
require_once "./xml/request_xml_templates.php";

header('Content-Type: text/plain');

$json_req = file_get_contents("php://input");
$req = json_decode($json_req, true);

// DANGEROUS: This is very very dangeurous!!!! Anyone could set xml_function to, let's say, system, and execute any, ANY command from the server.
$xml_function = $req["xml_function"];

$has_data = $req["has_data"];
if($has_data === true){
	$data = $req["data"];
}else{
	$data = null;
}

// 1. Create xml
$requestXML = $xml_function($data);

// 2. Send xml to server
$sock_fd = server_connect();

if($sock_fd != -1){
	server_send($sock_fd, $requestXML);

	// 3. Read response xml and set devices
	$response = server_read($sock_fd);

	server_close($sock_fd);

	echo $response;
} else {
	http_response_code(503);
	echo "Error: Could not connect to TCP server.";
}

exit;
?>
