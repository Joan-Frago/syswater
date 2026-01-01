<?php
require_once "util.php";
require_once "connection.php";

function getAllDevices(){
	// 1. Create xml
	$requestXML = getAllDevicesXML();

	// 2. Send xml to server
	$sock_fd = server_connect();
	server_send($sock_fd, $requestXML);

	// 3. Read response xml and set devices
	$response = server_read($sock_fd);
	write2console("Server response ".$response);

	server_close($sock_fd);

	return 0;
}

function getAllDevicesXML(){
	$xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	$xml .= "<request>\n";
	$xml .= "<function>get_all_devices</function>\n";
	$xml .= "</request>";

	return $xml;
}

?>
