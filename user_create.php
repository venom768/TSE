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
if(isset($_POST["submit"]))
{
	// E-Mail Adresse auf Gültigkeit überprüfen
	$email = ValidateEmail($db, $_POST["email"]);
	
	// E-Mail Adresse erfolgreich validiert und nicht bereits in der Datenbank vorhanden
	if( $email !== false && CheckUserEmail($db, $email) === false)
	{
		// Beide Passwörter auf Gültigkeit überprüfen
		$password1 = ValidatePassword($db, $_POST["password1"]);
		$password2 = ValidatePassword($db, $_POST["password2"]);
		
		// Passwörter erfolgreich validiert und beide Passwörter stimmen überein
		if( $password1 !== false && $password2 !== false && PasswordComparsion( $password1, $password2 ) !== false)
		{
			// Restliche eingegebene Werte überprüfen
			$firstname = ValidateFirstname($db, $_POST["firstname"]);
			$lastname = ValidateLastname($db, $_POST["lastname"]);
			$title = ValidateTitle($db, $_POST["title"]);
			$tel = ValidateTel($db, $_POST["tel"]);
			// Eingegebene Werte direkt in lokale Variable speichern (Keine Überprüfung erforderlich, da keine Eingabe)
			$salutation = $_POST["salutation"];
			$status = $_POST["status"];
			$rights = $_POST["rights"];		
			
			// Restliche eingegebene Werte erfolgreich validiert
			if( $firstname !== false && $lastname !== false && $title !== false && $tel !== false )
			{
				// Benutzer anlegen
				CreateUser( $db, $_SESSION['user_ID'], $salutation, $firstname, $lastname, $title, $email, $password1, $tel, $rights, $status );
			}
		}
	}
}

PrintHeader( "Benutzer anlegen" );
PrintNavbar($_SESSION['group_ID']);
?>

	<div id="content">
	
		<form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>" id="create_user">
		<fieldset>
		<legend>Benutzer anlegen</legend>
		<ul>
			<div class="errstr"><?php echo $GLOBALS['errstr'];?><!-- Ausgabe der Fehlermeldung -->
			</div>			
		
			<div class="switch">
				<li><input type="radio" id="left" name="salutation" value="Herr" required />
				<label for="left">Herr</label>
				<input type="radio" id="right" name="salutation" value="Frau" required />
				<label for="right">Frau</label></li>
			</div>
			<div class="switch">
				<li>
					<input name="status" type="radio" id="left2" value="aktiv" required />
					<label for="left2">Aktiv</label>
					
					<input name="status" type="radio" id="center2" value="inaktiv" required />
					<label for="center2">Inaktiv</label>
					
					<input name="status" type="radio" id="right2" value="gesperrt" required />
					<label for="right2">Gesperrt</label>
				</li>
			</div>
			<div>
			<div class="switch">
				<li>
					<input name="rights" type="radio" id="left1" value="1" required />
					<label for="left1">Admin</label>
					
					<input name="rights" type="radio" id="centerleft" value="2" required />
					<label for="centerleft">LV</label>
					
					<input name="rights" type="radio" id="centerright" value="3" required />
					<label for="centerright">User</label>
					
					<input name="rights" type="radio" id="right1" value="4" required />
					<label for="right1">Gast</label>
				</li>
			</div>
			
				<li><input type="text" name="title" placeholder="Titel" /></li>
			</div>
			<div>
				<li><input name="firstname" type="text" placeholder="Vorname" required /></li>
			</div>
			<div>
				<li><input name="lastname" type="text" placeholder="Nachname" required /></li>
			<div>
				<li><input name="tel" type="text" placeholder="Telefonnummer" required /></li>
			</div>
			</div>
			<div>
				<li><input name="email" type="email" placeholder="E-Mail" required /></li>
			</div>
			<div>
				<li><input name="password1" type="password" placeholder="Passwort" required /></li>
			</div>
			<div>
				<li><input name="password2" type="password" placeholder="Passwort wiederholen" required /></li>
			</div>
			
			<div>
				<li><input type="submit" name="submit" value="Benutzer anlegen" /></li>
			</div>
		<ul>
		</fieldset>
		</form>
	</div>
	
<?php
PrintFooter()
?>