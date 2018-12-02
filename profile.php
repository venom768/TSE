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
if( isset( $_POST["submit1"] ) )
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

if( isset( $_POST["submit2"] ) )
{
	// passende pos_ID übergeben, da der Admin die Firmenadresse ändert und der LV doe Lageradresse ändert
		// Admin ändert Firmenadresse mit der ID 0
			if($_SESSION['group_ID'] == 1 )
			{
				$pos_ID = 0;
			}
			// Lagerverwalter ändert Lageradresse mit der ID 1
			if($_SESSION['group_ID'] == 2 )
			{
				$pos_ID = 1;
			}
	// eingegebene Werte übergeben
			$companyname = $_POST["companyname"];
			$adress = $_POST["adress"];
			$PLZ = $_POST["PLZ"];
			$place = $_POST["place"];
				
			// Adresse ändern
			UpdateAdress( $db, $pos_ID, $companyname, $adress, $PLZ, $place);
	
}
PrintHeader( "Einstellungen");					// kompletten Header ausgeben
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
			<li><input type="submit" name="submit1" value="Passwort ändern" /></li>
		  </ul>
		</fieldset>
    </form>

	<form action="<?php echo $_SERVER['PHP_SELF']; ?>" method="post" id="create_user">
		<fieldset>
		<!-- Je nach Benutzergruppe die passende Anzeige für Adresse anziegen -->
		<legend> <?php if($_SESSION['group_ID'] == 1 ) echo ' Firmenadresse';?><?php if($_SESSION['group_ID'] == 2 ) echo ' Lageradresse';?>  ändern</legend>
		  <ul>
			<?php echo $GLOBALS['errstr'];?>
			<!--<p>Firmenname</p>-->
			<li><input type="text" name="companyname" placeholder="Firmenname eingeben"/></li>
			<!--<p>Adresse mit Hausnummer</p>-->
			<li><input type="text" name="adress" placeholder="Adresse und Hausnr. eingeben"/></li>
			<!--<p>PLZ</p>-->
			<li><input type="text" name="PLZ" placeholder="Postleitzahl eingeben"/></li>
			<!--<p>Ort</p>-->
			<li><input type="text" name="place" placeholder="Ort eingeben"/></li>
			<li><input type="submit" name="submit2" value="Adresse ändern" /></li>
		  </ul>
		</fieldset>
    </form>
	</div>
<?php
PrintFooter()
?>