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

// DEBUG Ausgabe der Array's (ARRAY, ARRAY_NAME)
DebugPrintArr( $_SESSION, 'SESSION');
DebugPrintArr( $_POST, 'POST');

if( isset($oberkategorie) )
{
	DebugPrintArr( $oberkategorie, 'Oberkategorie');
}
if( isset($unterkategorie) )
{
	DebugPrintArr( $unterkategorie, 'Unterkategorie');
}

PrintHeader( "Startseite" );
printnavbar($_SESSION['group_ID']);
?>
 
<div id="content">
 <?php
 
//Hole alle Oberkategorien aus der Datenbank
$oberkategorie=GetOberkategorie($db);

echo
	"
	<p>
		<form action=\"devices_create.php\" method=\"POST\">
		<label>Oberkategorie</label>
		<select name=\"oberkategorie\">
	";
	$count=1; //Hilfsvariable
	for($n=0; !empty($oberkategorie[$n]['1']); $n++)
	{
		echo"<option value='".$oberkategorie[$n]['1']."'>" .$oberkategorie[$n]['1']. "</option>";
		$count++;
	}
	echo
		"
		</select>
		<button type=\"submit\" name=\"submit\" value=\"submit_Oberkategorie\">Bestätigen</button>
		</form>
		</p>
		";

if( isset($_POST['submit']) )
{
	//wenn die Auswahl der 'Oberkategorie' bestätigt wurde dann...
	if($_POST['submit']=='submit_Oberkategorie')
	{
		//Hole alle Unterkategorien aus der Datenbank abhängig von der ausgwählten Oberkategorie
		$unterkategorie=GetUnterkategorieByOberkategorie($_POST['oberkategorie'],$db);
		
		// wenn die Wahl der Oberkategorie bestätigt wurde dann...

		if($_POST['submit']=='submit_Oberkategorie')
		{
			echo
			"
			<p>
				<form action=\"devices_create.php\" method=\"POST\"
				 <label>Unterkategorie</label>
				 <select name=\"unterkategorie\">
			";
			$count=1; //Hilfsvariable
			for($n=0; !empty($unterkategorie[$n]['1']); $n++)
			{
				echo"<option value='".$count."'>" .$unterkategorie[$n]['1']. "</option>";
				$count++;
			}
			echo
			"
			 </select>
			 <button type=\"submit\" name=\"submit\" value=\"submit_Unterkategorie\">Bestätigen</button>
			</form> 
			</p>
			";
		}
	}
}

 ?>
</div>
 
<?php
printfooter()
?>