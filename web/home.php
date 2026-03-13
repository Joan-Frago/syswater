<?php
require_once "util.php";

function replace_templates($file, $id_device, $device_type){
	$new_file = "";
	while(!feof($file)) {
		$line = fgets($file);
		// For more replacements, see documentation https://www.php.net/manual/en/function.str-replace.php
		// I think arrays can be passed so multiple templates are replaced at one time.
		$line = str_replace("#@ID_DEVICE@#", $id_device, $line);
		$line = str_replace("#@DEVICE_TYPE@#", $device_type, $line);
		$new_file .= $line;
	}
	return $new_file;
}

function insert_svg($svg_path, $id_device, $device_type){
	$svg_file = fopen($svg_path, "r") or die(write2console("Error: could not open ".$svg_path." file."));

	$new_svg_str = replace_templates($svg_file, $id_device, $device_type);
	fclose($svg_file);

	echo $new_svg_str;
}

?>
