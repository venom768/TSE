<?php

include_once('./includes/phpfunctions.inc');		// PHP-Funktionen laden

// Überprüfung ob PHP Funktionen korrekt geladen wurden
if ( !defined("__INC_PHPFUNCS__") )
	{
		// Interne Fehlernummer in Logdatei schreiben
		CreateErrorLogfile( 12 );
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}

Init( true );										// Globale Parameter und Einstellungen initialisieren
session_start();									// Session wiederaufnehmen
$db = dbconnect( $_SESSION['group_ID'] );			// Datenbankverbindung aufbauen					
CheckLogin( $db );									// Login Status überprüfen

PrintHeader( "Fehler", false);
?>

	<div id="content">
		<h2>Fehler</h2>
		<p>
		<?php echo $GLOBALS['errstr']; ?>
		<a href="./main.php">Zurück zur Startseite</a>
		</p>
	</div>
	
<?php
PrintFooter();
?>