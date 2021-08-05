<html>
    <head>
	<meta charset="utf-8" />
	<title>Configurable C Code Crawler</title>
	<link rel="stylesheet" href="style.css" />
	<script language="javascript">
	 <?php require_once ("script.js"); ?>
	</script>
    </head>
    <body>
	<h1>C coding style compliance tester</h1>
	<table style="height: 90%;">
	    <tr><td style="width: 49%;">
		<div style="height: 33.333%; width: 100%;" id="code">
		    A
		</div>
		<div style="height: 33.333%; width: 100%;" id="warning">
		    B
		</div>
		<div class="overflow buttons" style="height: 33.333%;" id="files">
		    <?php for ($i = 0; $i < 20; ++$i) { ?>
			<div id="<?=$i; ?>"><?=$i; ?></div>
		    <?php } ?>
		</div>
	    </td><td style="width: 2%;">
	    </td><td>
		<div class="full scroll">
		    <h2>Configuration</h2>
		    <form method="post" action="index.php">
			<?php require_once ("configuration.php"); ?>
		    </form>
		</div>
	    </td><tr/>
	</table>
    </body>
</html>
