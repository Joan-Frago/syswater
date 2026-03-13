<?php
	require_once "home.php";
?>

<!DOCTYPE html>
<html lang="es">
<head>
	<meta charset="UTF-8">
	<title>Home Control</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="./home.css">
	<script src="./home.js"></script>
</head>
<body>
	<header class="header">
		<h1>Control Panel</h1>
	</header>
	<main class="house-grid">

		<section id="1" class="device">
			<div class="device-svg">
				<?php insert_svg("svg/persiana.svg", 1, "BLIND"); ?>
			</div>
			<h2>Persiana Dormitorio</h2>
			<button class="device-btn off">Bajada</button>
			<div>
				<p id="DI1.4">Input: </p>
				<p id="RO2.1">Output: </p>
			</div>
		</section>

		<section id="2" class="device">
			<div class="device-svg">
				<?php insert_svg("svg/bulb.svg", 2, "LIGHT_BULB"); ?>
			</div>
			<h2>Llum de proves</h2>
			<button class="device-btn off">Off</button>
			<div>
				<p id="DI1.2">Input: </p>
				<p id="RO2.2">Output: </p>
			</div>
		</section>

	</main>
	
	<div class="invisible">
	</div>

	<script>
		// get_all_devices();
		set_all_devices();
		load_pin_data();
	</script>
</body>
</html>
