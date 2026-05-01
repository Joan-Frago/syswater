<?php
	require_once "home.php";
?>

<!DOCTYPE html>
<html lang="es">
<head>
	<meta charset="UTF-8">
	<title>Riego automatico</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="./home.css">
	<script src="./home.js"></script>
</head>
<body>
	<header class="header">
		<h1>Riego Automatico</h1>
	</header>
	<main class="house-grid">

		<?php insert_svg(svg_path: "svg/garden-automatic-watering.svg", id_device: 1, device_type: "WATERING_SYSTEM", width: 220, height: 270); ?>

	</main>
	
	<div class="invisible">
	</div>

	<script>
		set_all_devices();
		load_pin_data();
	</script>
</body>
</html>
