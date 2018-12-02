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

	if( $_POST["submit"] === "Ja" )
		{
			// wenn Ja gedrückt Benutzer löschen
			DeleteUser( $db, $_SESSION['delete_userID']);
			// Variable delete_userID frei machen
			$_SESSION['delete_userID'] = NULL;
		}
	if(($_POST["submit"]) === "Nein")
	{
		// zurück zur Übersicht User
		header("location: user_main.php?");
		// Variable delete_userID frei machen
		$_SESSION['delete_userID'] = NULL;
	}



// User Id aus der tabelle via Post in eine lokale Variable speichern um damit weiter zu arbeiten
if(isset($_GET['delete_userID']) )
{
	$_SESSION['delete_userID'] = $_GET['delete_userID'];
}
// Datensätze aus der Datenbank holen von dem zu bearbeitenden Benutzer
$user_data_for_Delete	= GetDataForDeleteUsersByUserID($db, $_SESSION['delete_userID'] );
// Daten in die Session übergeben
$_SESSION['user_data_for_Delete'] = $user_data_for_Delete;

PrintHeader( "Benutzer l&oumlschen" );
PrintNavbar($_SESSION['group_ID']);
?>


	<div id="content">
	 
	<form method="post" action="<?php echo $_SERVER['PHP_SELF']; ?>" id="create_user">
		<fieldset>
		<legend>Benutzer l&oumlschen</legend>
		<ul>
			<div class="errstr"><?php echo $GLOBALS['errstr'];?><!-- Ausgabe der Fehlermeldung -->
			</div>
			<?php if(!isset($_POST["submit"]))
			{
			 echo "<p>Wollen Sie den Benutzer</p><h4>$user_data_for_Delete[user_firstname] $user_data_for_Delete[user_lastname]</h4><p>wirklich l&oumlschen?</p>
			
			<div>
				<li><input type=\"submit\" name=\"submit\" value=\"Ja\" /></li>
				<li><input type=\"submit\" name=\"submit\" value=\"Nein\" /></li>
			</div>
			";
			}?>
		<ul>
		</fieldset>
		</form>
</div>
	
<?php
PrintFooter()
?>