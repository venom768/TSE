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

// Überprüfung ob der Submit-Button gedrückt wurde
if( isset( $_POST["submit"] ) )
{
	// Altes Passwort auf Gültigkeit überprüfen
	$current_password = ValidatePassword($db, $_POST['current_password']);
	
	// Ungültiges altes Passwort
	if( $current_password !== false )
	{
		// Altes Passwort mit der Datenbank vergleichen
		if( CheckUserPassword( $db, $current_password ) !== false )
		{
			// Neue Passwörter auf Gültigkeit überprüfen
			$new_password1 = ValidatePassword($db, $_POST['new_password1']);
			$new_password2 = ValidatePassword($db, $_POST['new_password2']);
			
			// Ungültiges neues Passwort 1 oder 2
			if( $new_password1 !== false && $new_password2 !== false)
			{
				// Die neuen Passwörter stimmen nicht überein
				if( PasswordComparsion( $new_password1, $new_password2 ) === true )
				{
					// Passwort im entsprechenden User aktualisieren
					UpdateUserPassword( $db, $_SESSION['user_ID'], $new_password1 );
				}
			}
		}
	}
}

PrintHeader( "Paswort ändern");					// kompletten Header ausgeben
PrintNavbar($_SESSION['group_ID']);
?>

	<div id="content">
	
	<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" id="change_password">
		<fieldset>
		<legend>Passwort ändern</legend>
		  <ul>
			<?php echo $GLOBALS['errstr'];?>
			<!--<p>Aktuelles Passwort</p>-->
			<li><input type="password" name="current_password" placeholder="Aktuelles Passwort eingeben"/></li>
			<!--<p>Neues Passwort</p>-->
			<li><input type="password" name="new_password1" placeholder="Neues Passwort eingeben"/></li>
			<!--<p>Neues Passwort wiederholen</p>-->
			<li><input type="password" name="new_password2" placeholder="Neues Passwort wiederholen"/></li>
			<li><input type="submit" name="submit" value="Passwort ändern" /></li>
		  </ul>
		</fieldset>
    </form>
	</div>
<?php
PrintFooter()
?>