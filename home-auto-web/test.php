<?php
error_reporting(E_ALL);

echo "Trying to connect to TCP server\n";

$address = 'localhost';
$service_port = 8080;

$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false) {
	echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
} else {
	echo "OK.\n";
}

echo "Attempting to connect to '$address' on port '$service_port'...";
$result = socket_connect($socket, $address, $service_port);
if ($result === false) {
	echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
} else {
	echo "OK.\n";
}

for($i = 0;$i<100;$i++){

	$in = (string)$i;
	$in .= "\n";

	echo "Sending message...";
	socket_write($socket, $in, strlen($in));
	echo "OK.\n";

	echo "Reading response:\n";
	$out = '';
	$out = socket_read($socket, 2048);

	if($out == false){
		echo "Error reading or server disconnected\n";
		break;
	}
	echo $out;
}
echo "Closing socket...";
if($socket){
	socket_close($socket);
}
echo "OK.\n\n";
?>
