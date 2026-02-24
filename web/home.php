<?php
require_once "util.php";

function insert_svg($svg_path){
	$svg_file = fopen($svg_path, "r") or die(write2console("Error: could not open ".$svg_path." file."));
	echo fread($svg_file, filesize($svg_path));
	fclose($svg_file);
}

?>
