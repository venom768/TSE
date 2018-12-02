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
				UpdateUser( $db, $_SESSION['update_userID'], $salutation, $firstname, $lastname, $title, $email, $tel, $rights, $status );
				// Variable update_userID frei machen
				$_SESSION['update_userID'] = NULL;
			}	
}
// User Id aus der tabelle via Post in eine lokale Variable speichern um damit weiter zu arbeiten
if(isset($_GET['update_userID']) )
{
	$_SESSION['update_userID'] = $_GET['update_userID'];
}
// Datensätze aus der Datenbank holen von dem zu bearbeitenden Benutzer
$user_data_for_Edit	= GetDataForEditUsersByUserID($db, $_SESSION['update_userID'] );
// Daten in die Session übergeben
$_SESSION['user_data_for_Edit'] = $user_data_for_Edit;

PrintHeader( "Benutzer bearbeiten", true );
PrintNavbar($_SESSION['group_ID']);
?>


	<div id="content">
	
		<form method="post" action="user_edit.php" id="create_user">
		<fieldset>
		<legend>Benutzer bearbeiten</legend>
		<ul>
			<div class="errstr"><?php echo $GLOBALS['errstr'];?><!-- Ausgabe der Fehlermeldung -->
			</div>			
		
			<div class="switch">
				<li><input type="radio" id="left" name="salutation" value="Herr" <?php if($user_data_for_Edit[ 'user_salutation' ] == 'Herr' ) echo ' checked="checked"';?> required />
				<label for="left">Herr</label>
				<input type="radio" id="right" name="salutation" value="Frau" <?php if($user_data_for_Edit[ 'user_salutation' ] == 'Frau' ) echo ' checked="checked"';?> required />
				<label for="right">Frau</label></li>
			</div>
			<div class="switch">
				<li>
					<input name="status" type="radio" id="left2" value="Aktiv" <?php if($user_data_for_Edit[ 'user_status' ] == 'Aktiv') echo ' checked="checked"';?> required />
					<label for="left2">Aktiv</label>
					
					<input name="status" type="radio" id="center2" value="Inaktiv" <?php if($user_data_for_Edit[ 'user_status' ] == 'Inaktiv' ) echo ' checked="checked"';?> required />
					<label for="center2">Inaktiv</label>
					
					<input name="status" type="radio" id="right2" value="Gesperrt" <?php if($user_data_for_Edit[ 'user_status' ] == 'Gesperrt' ) echo ' checked="checked"';?> required />
					<label for="right2">Gesperrt</label>
				</li>
			</div>
			<div>
			<div class="switch">
				<li>
					<input name="rights" type="radio" id="left1" value="1" <?php if($user_data_for_Edit[ 'group_description' ] == 'Admin' ) echo ' checked="checked"';?> required />
					<label for="left1">Admin</label>
					
					<input name="rights" type="radio" id="centerleft" value="2" <?php if($user_data_for_Edit[ 'group_description' ] == 'Lagerverwalter' ) echo ' checked="checked"';?> required />
					<label for="centerleft">LV</label>
					
					<input name="rights" type="radio" id="centerright" value="3" <?php if($user_data_for_Edit[ 'group_description' ] == 'Benutzer' ) echo ' checked="checked"';?> required />
					<label for="centerright">User</label>
					
					<input name="rights" type="radio" id="right1" value="4" <?php if($user_data_for_Edit[ 'group_description' ] == 'Gast' ) echo ' checked="checked"';?> required />
					<label for="right1">Gast</label>
				</li>
			</div>
			
				<li><input type="text" name="title" placeholder="Titel" Value="<?php echo $user_data_for_Edit[ 'user_title' ] ?>" /></li>
			</div>
			<div>
				<li><input name="firstname" type="text" placeholder="Vorname" Value="<?php echo $user_data_for_Edit[ 'user_firstname' ] ?>" required /></li>
			</div>
			<div>
				<li><input name="lastname" type="text" placeholder="Nachname" Value="<?php echo $user_data_for_Edit[ 'user_lastname' ] ?>" required /></li>
			<div>
				<li><input name="tel" type="text" placeholder="Telefonnummer" Value="<?php echo $user_data_for_Edit[ 'user_telnr' ] ?>" required /></li>
			</div>
			</div>
			<div>
				<li><input name="email" type="email" placeholder="E-Mail" Value="<?php echo $user_data_for_Edit[ 'user_email' ] ?>"  required /></li>
			</div>
			
			<div>
				<li><input type="submit" name="submit" value="Benutzer ändern" /></li>
			</div>
		<ul>
		</fieldset>
		</form>
	</div>
<?php
PrintFooter()
?>