<?php

include_once('./includes/phpfunctions.inc');		// PHP-Funktionen laden
require './phpmailer/PHPMailerAutoload.php';		// Mail-Funktionen laden

// Überprüfung ob PHP Funktionen korrekt geladen wurden
if ( !defined("__INC_PHPFUNCS__") )
	{
		// Interne Fehlernummer in Logdatei schreiben
		CreateErrorLogfile( 12 );
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}

Init( true );										// Globale Parameter und Einstellungen initialisieren
$db = dbconnect( 0 );								// Datenbankverbindung aufbauen							

// Überprüfen ob eine E-Mail Adresse und ein Passwort Code mit Get übergeben wurden
if( !empty( $_GET['email'] ) && !empty( $_GET['code'] ) )
{
	// E-Mail Adresse überprüfen
	$email = ValidateEmail( $db, $_GET['email'] );
	// Passwort Code hashen
	$pwcode = $_GET['code'];

	// Überprüfung ob der Submit-Button gedrückt wurde
	if( isset( $_POST['submit'] ) )
	{	
		// Passwort Zeitstempel holen
		$time = GetPasswordCodeAndTime( $db, $email, false )[1];

		// Überprüfung ob der Passwort Zeitstempel älter als 24h oder NULL ist
		if( strtotime( $time ) > time()-24*3600 && $time !== NULL )
		{
			// Passwort Code holen
			$dbcode = GetPasswordCodeAndTime( $db, $email, false )[0];
			
			// Überprüfung ob der Passwort Code mit dem Code in der Datenbank übereinstimmt und nicht NULL ist
			if( sha1( $pwcode ) === $dbcode && $pwcode !== NULL )
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
						UpdateUserPasswordByPasswordCode( $db, $email, sha1( $pwcode ), $new_password1 );
						// Passwort Code und Zeitstempel im User löschen
						DeletePasswordCodeAndTime( $db, $email);
						// Weiterleitung zur Index Seite (Login)
						header("Location: ./index.php?error=117");
					}
				}
			}
			else
			{
				// Weiterleitung zur Index Seite (Login)
				header("Location: ./index.php?error=118");
			}
		}
		else
		{
			// Weiterleitung zur Index Seite (Login)
			header("Location: ./index.php?error=119");
		}
	}
}
else
{
	// Weiterleitung zur Index Seite (Login)
	header("Location: ./index.php");
}

PrintHeader( "Passwort Vergessen", false, "./index.php" );			// Header ohne User Div ausgeben
?>

<div id="content">
	
	<form action="<?php echo $_SERVER['PHP_SELF']."?email=".$email."&code=".$pwcode; ?>" method="post" id="change_password">
		<fieldset>
		<legend>Passwort ändern</legend>
		  <ul>
			<?php echo $GLOBALS['errstr'];?>
			<!--<p>Neues Passwort</p>-->
			<li><input type="password" name="new_password1" placeholder="Neues Passwort eingeben"/></li>
			<!--<p>Neues Passwort wiederholen</p>-->
			<li><input type="password" name="new_password2" placeholder="Neues Passwort wiederholen"/></li>
			<li><input type="submit" name="submit" value="Passwort ändern" /></li>
			<a href="./index.php">Zurück zum Login</a>
		  </ul>
		</fieldset>
    </form>
	</div>

<?php
PrintFooter()										// kompletten Footer ausgeben
?>