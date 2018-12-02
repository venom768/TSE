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

PrintHeader( "Startseite" );
PrintNavbar( $_SESSION['group_ID'] );
?>

	<div id="content">
		<div id="wk_list">
		<h2>bevorstehende Wartungen</h2>
		<?php 
			// Wartungstabelle dynamisch erstellen
			wklist( $_SESSION['device_data_wklist'] );
		?>
		
		</div>
		<div id="event_list">
		<h2>Ereignissprotokoll</h2>
		<?php 
			// Eventtabelle dynamisch erstellen
			eventlist( $_SESSION['device_data_eventlist'] );
		?>
		</div>
	</div>
<?php
PrintFooter()
?>