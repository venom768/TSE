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

PrintHeader( "Kontakt" );
PrintNavbar($_SESSION['group_ID']);

?>
<div id="content">
	<div id="contact">
		<h2>Ansprechpartner</h2>	
		<?php 
			// Wartungstabelle dynamisch erstellen
			ContactList( $_SESSION['data_for_contacts'] );
		?>
	</div>
</div>
<?php
PrintFooter()

?>