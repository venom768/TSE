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

PrintHeader( "Benutzer &Uumlbersicht" );
PrintNavbar($_SESSION['group_ID']);
?>

	<div id="content">
		<div id="overview">
		<h2>Übersicht</h2>
		<?php 
			// Übersichtstabelle daýnamisch erstellen
			OverviewUsers( $_SESSION['device_data_overviewusers'] );
		?>
		</div>
	</div>
<?php
PrintFooter()
?>