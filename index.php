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
$db = dbconnect( 0 );								// Initialisierende Datenbankverbindung aufbauen										
Login( $db );										// Loginskript starten
CheckLogout();										// Überprüfung ob User sich ausgeloggt hat

PrintHeader( "Login", false, "index.php" );
?>

	<div id="content">
	
	<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" id="login">
		<fieldset>
		<legend>Authentifizierung</legend>
		  <ul>
		  
			<?php echo $GLOBALS['errstr'];?>
				
			<!--<p>E-Mail-Adresse</p>-->
			<li><input type="email" name="email" placeholder="E-Mail"  /></li>
			<!--<p>Passwort</p>-->
			<li><input type="password" name="password" placeholder="Passwort"/></li>
			<li><input type="submit" name="submit" value="Login" /></li>
			<a href="pw_reset.php">Passwort vergessen</a>
		  </ul>
		</fieldset>
    </form>
	</div>
<?php
PrintFooter();
?>