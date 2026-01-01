<?php
	require_once "home.php";
?>

<!DOCTYPE html>
<html lang="es">
<head>
	<meta charset="UTF-8">
	<title>Home Control</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="home.css">
</head>
<body>
	<header class="header">
		<h1>Control Panel</h1>
	</header>
	<main class="house-grid">

		<!-- Elemento: Luz -->
		<section class="device">
			<div class="device-svg">
				<img src="svg/luz.svg" alt="Luz del salón">
			</div>
			<h2>Luz Salón</h2>
			<button class="device-btn on">Encendida</button>
		</section>

		<!-- Elemento: Persiana -->
		<section class="device">
			<div class="device-svg">
				<img src="svg/persiana.svg" alt="Persiana dormitorio">
			</div>
			<h2>Persiana Dormitorio</h2>
			<button class="device-btn off">Bajada</button>
		</section>

		<!-- Elemento: Caldera -->
		<section class="device">
			<div class="device-svg">
				<img src="svg/caldera.svg" alt="Caldera">
			</div>
			<h2>Caldera</h2>
			<button class="device-btn on">Activa</button>
		</section>

		<!-- Elemento: Calefacción -->
		<section class="device">
			<div class="device-svg">
				<img src="svg/calefaccion.svg" alt="Calefacción">
			</div>
			<h2>Calefacción</h2>
			<button class="device-btn off">Apagada</button>
		</section>
	</main>
	
	<?php
		getAllDevices();
	?>
</body>
</html>
