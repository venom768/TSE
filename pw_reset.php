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

// Überprüfung ob der Submit-Button gedrückt wurde
if( isset( $_POST["submit"] ) )
{
	// Eingegebene Daten überprüfen
	$email = ValidateEmail($db, $_POST['email']);

	// E-Mail Adresse erfolgreich validiert und bereits in der Datenbank vorhanden
	if( $email !== false && CheckUserEmail($db, $_POST["email"]) === true )
	{
		// Passwort- Code und Zeitstempel bereits vorhanden
		if( GetPasswordCodeAndTime( $db, $email )[0] === false && GetPasswordCodeAndTime( $db, $email )[1] === false)
		{
			// Zufälligen String erzeugen
			$rstring = RandomString();
			// Passwort- Code und Zeitstempel setzen
			SetPasswordCodeAndTime( $db, $email, $rstring );
			
			// ein Objekt vom Typ PHPMailer erstellen
			$mail = new PHPMailer;
			//E-Mail erstellen
			$url = "http://localhost/tse/pw_renew.php?email=".$email."&code=".$rstring;
			
			$mail->CharSet = 'utf-8';				// Zeichensatz aktivieren
			$mail->isSMTP();						// Server als SMTP kennzeichnen
			$mail->Host = 'mail.gmx.net';			// SMTP-Server Hostname
			$mail->SMTPAuth = true;					// SMTP-Server Authentifizierung aktivieren
			$mail->Username = 'tselv@gmx.de';		// Username zur Anmeldung am SMTP-Server
			$mail->Password = 'Steffen123!';		// Passwort zur Anmeldung am SMTP-Server
			$mail->SMTPSecure = 'tls';				// Verschlüsselungsmethode --> TLS
			$mail->Port = 587;						// SMTP-Server Port
			$mail->setFrom('tselv@gmx.de', 'TSE');	// Absenderadresse
			
			$mail->addAddress( $email, 'TSE Lagerverwaltung' );			// Empfängeradresse
			
			$mail->Subject  = 	"TSE Lagerverwaltung - Passwort zurücksetzen";
			$mail->Body     = 	"Hallo User,\n\n\n".
								"Für ihren Account wurde die Passwort zurücksetzen Funktion ausgelöst.\n".
								"Bitte benutzen Sie folgenden Link um ein neues Passwort für Ihren Account zu vergeben: \n\n".$url."\n\n".
								"Sollte Ihnen ihr Passwort wieder eingefallen sein, fahren Sie nicht weiter fort und löschen Sie diese E-Mail.\n".
								"Sollten Sie diese E-Mail nicht angefordert haben, versucht eventuell jemand Zugriff auf ihr Benutzerkonto zu erlangen.\n".
								"Melden Sie Unstimmigkeiten umgehend dem zuständigen Administrator.\n".
								"Diese E-Mail hat eine Gültigkeit von 24-Stunden, danach verfällt der Link automatisch.\n\n\n".
								"Mit freundlichen Grüßen\n\n".
								"Administrator der TSE Lagerverwaltung";
			// E-Mail versenden
			if(!$mail->send()) 
			{
				$GLOBALS['errstr'] = "Beim Versenden der E-Mail ist ein Fehler aufgetreten.";
				echo 'Mailer error: ' . $mail->ErrorInfo;
			} 
			else 
			{
				echo "test Mail";
				$GLOBALS['errstr'] = "E-Mail erfolgreich versendet.";
			}
			
		}	
	}
}

PrintHeader( "Passwort Vergessen", false, "./index.php" );					// kompletten Header ausgeben
?>

	<div id="content">
	
	<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" id="change_password">
		<fieldset>
		<legend>Passwort zurücksetzen</legend>
		  <ul>
			<?php echo $GLOBALS['errstr'];?>
			<!--<p>Aktuelles Passwort</p>-->
			<li><input type="email" name="email" placeholder="E-Mail Adresse eingeben"/></li>
			<li><input type="submit" name="submit" value="Passwort ändern" /></li>
			<a href="./index.php">Zurück zum Login</a>
		  </ul>
		</fieldset>
    </form>
	
	</div>

<?php
PrintFooter()										// kompletten Footer ausgeben
?>