<?php
/* 
  @file Modul zur Darstellung von HTML-Code des Projektes TSE Lagerverwaltung
*/

define( "__INC_HTMLFUNCS__", true );


/* ***************************************************************************************************************
  WkList( array $device_data_wklist )

  @brief 		  	Erstellt eine tabelle mit den anstehenden Wartungen je nach anzahl der Datensätze

  @function 		Erstellt eine Dynamische Tabelle

  @param 		  	$device_data_wklist ein array mit mehreren Datensätzen


 
**************************************************************************************************************** */

function WkList( $device_data_wklist )
{
    // Table header erstellen
	echo									
	"
	<table class=\"table\">				
			  <tr>
			    <th>ID</th>
				<th>Ger&aumlt</th>
				<th>f&aumlllig am</th>
				<th>Status</th>
				<th>M&aumlngel</th>
				<th>Zubeh&oumlr</th>
			  </tr>
	";
	// Der Tabellenrumpf
		foreach( $device_data_wklist AS $wk  )
		{
		echo <<<EOT
			  <tr>
				<td>{$wk['device_ID']}</td>
				<td>{$wk['modeldescription']}</td>
				<td>{$wk['device_wk_date']}</td>
				<td>{$wk['device_status']}</td>
				<td>{$wk['device_defects']}</td>
				<td>{$wk['device_equipment']}</td>
			  </tr>
EOT;
		}
	  // Das Tabellenende
	echo
	"</table>";

}



/* ***************************************************************************************************************
  EventList( array $device_data_eventlist )

  @brief 		  	Erstellt eine Tabelle mit den letzten Buchungen,der letzen 30 Tage, je nach anzahl der Datensätze

  @function 		Erstellt eine Dynamische Tabelle 

  @param 		  	$device_data_Eventlist ein array mit mehreren Datensätzen

**************************************************************************************************************** */

function EventList( $device_data_eventlist )
{
	// Table header erstellen
	echo									
	"
	<table class=\"table\">				
			  <tr>
			    <th colspan=\"2\">Wer</th>
				<th></th>
				<th>ID</th>
				<th>Ger&aumlt</th>
				<th>Wann</th>
				<th colspan=\"2\">Wohin</th>
				
			  </tr>
	";
	// Der Tabellenrumpf
		foreach( $device_data_eventlist AS $event  )
		{
		echo <<<EOT
			  <tr>
				<td>{$event['user_firstname']}</td>
				<td>{$event['user_lastname']}</td>
				<td>bucht</td>
				<td>{$event['device_ID']}</td>
				<td>{$event['modeldescription']}</td>
				<td>{$event['stamp_lend']}</td>
				<td>auf</td>
				<td>{$event['pos_address']} {$event['pos_plz']}</td>
			  </tr>
EOT;
		}
	   // Das Tabellenende
	echo
	"</table>";

}



/* ***************************************************************************************************************
  Contactlist( array $Get_Data_for_Contacts )

  @brief 		  	Erstellt eine Tabelle mit den Admins und Lagerverwaltern als Ansprechpartner

  @function 		Erstellt eine Dynamische Tabelle 

  @param 		  	$Get_Data_for_Contacts ein array mit mehreren Datensätzen
					

**************************************************************************************************************** */

function ContactList( $data_for_contacts )
{
	// Table header erstellen
	echo									
	"
	<table class=\"tbl1\">
				<tr>
					<th>Bereich</th>
					<th>Ansprechpartner</th>
					<th>Telefonnummer</th>
					<th>E-Mail</th>
				</tr>
	";
	// Der Tabellenrumpf
		foreach( $data_for_contacts AS $contact  )
		{
		echo <<<EOT
			  	<tr>
					<td>{$contact['group_description']}</td>
					<td>{$contact['user_salutation']} {$contact['user_firstname']} {$contact['user_lastname']}</td>
					<td>{$contact['user_telnr']}</td>
					<td><a href="mailto:tom.hausbeck@gmail.com">{$contact['user_email']}</a></td>
				</tr>
EOT;
		}
	   // Das Tabellenende
	echo
	"</table>";

}


/* ***************************************************************************************************************
  companylist( array $Get_Data_for_Company )

  @brief 		  	Erstellt eine Tabelle mit der Firmenadresse

  @function 		Erstellt eine Dynamische Tabelle 

  @param 		  	$Get_Data_for_Company ein array mit dem Firmenadressen Datensatz
					

**************************************************************************************************************** */

function CompanyList( $data_for_companyadress )
{
	echo <<<EOT
			  	<p>{$data_for_companyadress['pos_companyname']}</p>
				<p>{$data_for_companyadress['pos_adress']}</p>
				<p>{$data_for_companyadress['pos_plz']} {$data_for_companyadress['pos_place']}</p>
EOT;

}



/* ***************************************************************************************************************
  OverviewDevices( array $device_data_overwievdevices )

  @brief 		  	Erstellt eine Tabelle mit allen vorhandenen Geräten

  @function 		Erstellt eine Dynamische Tabelle 

  @param 		  	$device_data_overwievdevices ein array mit mehreren Datensätzen

**************************************************************************************************************** */

function OverviewDevices( $device_data_overviewdevices )
{
	// Table header erstellen
	echo  
	"
	<table class=\"table\">				
			  <tr>
			    <th>ID</th>
				<th>Model</th>
				<th>Oberkategorie</th>
				<th>Unterkategorie</th>
				<th>Seriennummer</th>
				<th>Status</th>
				<th>M&aumlngel</th>
				<th>Zubeh&oumlr</th>
				<th>Wartungstermin</th>
				<th colspan=\"2\">gebucht auf</th>
				
			  </tr>
	";

		// Der Tabellenrumpf
		foreach( $device_data_overviewdevices AS $device  )
		{
		echo <<<EOT
			  <tr>
		<td>{$device['device_ID']}</td>
				<td>{$device['modeldescription']}</td>
				<td>{$device['upperclass_description']}</td>
				<td>{$device['underclass_description']}</td>
				<td>{$device['device_sn']}</td>
				<td>{$device['device_status']}</td>
				<td>{$device['device_defects']}</td>
				<td>{$device['device_equipment']}</td>
				<td>{$device['device_wk_date']}</td>
				<td>{$device['user_firstname']} {$device['user_lastname']}</td>
				<td>{$device['pos_companyname']} {$device['pos_address']} {$device['pos_plz']} {$device['pos_place']}</td>
			  </tr>
EOT;
		}
	// Das Tabellenende
	echo  
	"</table>";
}


/* ***************************************************************************************************************
  OverviewUsers( array $device_data_overwievusers )

  @brief 		  	Erstellt eine Tabelle mit allen vorhandenen Benutzern

  @function 		Erstellt eine Dynamische Tabelle 

  @param 		  	$device_data_overwievdevices ein array mit mehreren Datensätzen

**************************************************************************************************************** */

function OverviewUsers( $device_data_overviewusers )
{
	// Table header erstellen
	echo"	
	<table class=\"table\">				
			  <tr>
			    <th>ID</th>
				<th>Anrede</th>
				<th>Titel</th>
				<th>Vorname</th>
				<th>Nachname</th>
				<th>E-Mail</th>
				<th>Telefonnummer</th>
				<th>Status</th>
				<th>Gruppe</th>
				<th>Reg. Datum</th>
				<th>Letzer Login</th>
				<th colspan=\"2\"></th>				
			  </tr>
	";
		// Der Tabellenrumpf
		foreach( $device_data_overviewusers AS $user  )
		{
		echo <<<EOT
			  <tr>
		<td>{$user['user_ID']}</td>
				<td>{$user['user_salutation']}</td>
				<td>{$user['user_title']}</td>
				<td>{$user['user_firstname']}</td>
				<td>{$user['user_lastname']}</td>
				<td>{$user['user_email']}</td>
				<td>{$user['user_telnr']}</td>
				<td>{$user['user_status']}</td>
				<td>{$user['group_description']}</td>
				<td>{$user['user_create_stamp']}</td>
				<td>{$user['user_last_login']}</td>
				<td><a href="user_edit.php?update_userID={$user['user_ID']}"><img src="./pictures/BearbeitenButton25x25.png" alt="Bearbeiten"></td>
				<td><a href="user_delete.php?delete_userID={$user['user_ID']}"><img src="./pictures/LöschenButton25x20.png" alt="L&oumlschen"></td>
			  </tr>
EOT;
		}
	// Das Tabellenende
	echo   
	"</table>";
}


/* ***************************************************************************************************************
  printheader($str, $user)

  @brief 		  	Erstellt den gesammten Header der Seite und auf Wunsch auch den UserDiv

  @function			gibt den kompletten HTML-Code für den Header aus und für den UserDiv

  @param 		  	$str ein String der dann als <title> in die Seite eingefügt wird
					$user ein Boolean welches angibt ob ein UserDiv mit der Funktion PrintUserDiv() erstellt werden soll
**************************************************************************************************************** */

function PrintHeader( $str, $user=true, $link="main.php" )
{
echo <<<EOT
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
  
<html>
<head>

  <meta http-equiv="content-type" content="text/html; charset=utf-8" />
  <title>TSE - LV - $str</title>

  <link rel="stylesheet" href="./css/style.css" type="text/css" />
  <link rel="shortcut icon" href="./pictures/TSElogo.ico" type="image/x-icon" />
  
</head>

<body>

  <div id="wrapper">
	  
    <div id="header">
	  
	  <a href="./$link"><img src="./pictures/Logo.png" width="250" alt="TSE Logo"  /></a>
	  <div id="slogan">
		  <h1>Lagerverwaltung</h1>
		  <p><i>- a good System is the best Way -</i></p>
	  </div>
EOT;

	if( $user === true)
	{
		PrintUserDiv();
	}
	
echo <<<EOT
	</div>
EOT;
}

/* ***************************************************************************************************************
  printuserdiv()

  @brief 		  	Erstellt den User Div
  
  @ function		Der User Div gibt die E-Mail Adresse und die aktuellen Rechte des Benutzers aus
					Zusätzlich enhält er das Formular für den Logout Button
**************************************************************************************************************** */
function PrintUserDiv()
{
echo <<<EOT
<div id="user">
		<ul>
			<li><b>User:</b> {$_SESSION['email']} </li>
			<li><b>Angemeldet als:</b> {$_SESSION['group_description']}</li>
		<ul>
			<li><a href="./profile.php" class="bildwechsel" alt="Einstellungen" ></a></li>
			<li>
				<form action="./index.php" method="post">
				<input type="submit" value="Abmelden" />
				<input type="hidden" name="logout" value="true" />
				</form>
			</li>
		</ul>
		</ul>
	  </div>
EOT;
}
/* ***************************************************************************************************************
  PrintNavbar( string $user_group_ID )

  @brief 		  	Stellt je nach Benutzerrechten die passende Navigation dar

  @function 		Baut eine Verbindung zum DB-Server und der DB auf. 

  @param 		  	$user_group_ID eine group_ID als Ganzzahl

**************************************************************************************************************** */

function PrintNavbar( $user_group_ID )
{
	if($user_group_ID == 1)					// Navigation eines Admins laden
	{
	echo
	"
	<div id =\"navigation\">
	<nav role=\"navigation\">
	  <ul>
		<li><a href=\"main.php\">Startseite</a></li>
		<li class=\"sub-menu\"><a href=\"#\">Werkzeuge / Pr&uumlfmittel</a>
			<ul>
				<li><a href=\"devices_main.php\">&Uumlbersicht</a></li>
				<li><a href=\"devices_create.php\">Anlegen</a></li>
				<li><a href=\"devices_edit.php\">Bearbeiten</a></li>
			</ul>
			</li>
		<li><a href=\"devices_lend.php\">Ger&aumlt buchen</a></li>
		<li><a href=\"#\">Benutzerverwaltung</a>
			<ul>
				<li><a href=\"user_main.php\">&Uumlbersicht</a></li>
				<li><a href=\"user_create.php\">Anlegen</a></li>
			</ul>
			</li>
	  </ul>
	</nav>
	</div>
	";
	}
	else if($user_group_ID == 2)			// Navigation eines Lagerverwalters laden
	{
	echo
	"
	<div id =\"navigation\">
	<nav role=\"navigation\">
	  <ul>
		<li><a href=\"main.php\">Startseite</a></li>
		<li><a href=\"#\">Werkzeuge / Pr&uumlfmittel</a>
			<ul>
				<li><a href=\"devices_main.php\">&Uumlbersicht</a></li>
				<li><a href=\"devices_create.php\">Anlegen</a></li>
				<li><a href=\"devices_edit.php\">Bearbeiten</a></li>
			</ul>
			</li>
		<li><a href=\"devices_lend.php\">Ger&aumlt buchen</a></li>
		<li><a href=\"mydevices.php\">Meine Ger&aumlte</a></li>
	  </ul>
	  </nav>
	</div>
	";
	}
	else if($user_group_ID == 3)			// Navigation eines Users laden
	{
	echo
	"
	<div id =\"navigation\"><nav role=\"navigation\">
	<nav role=\"navigation\">
	  <ul>
		<li><a href=\"main.php\">Startseite</a></li>
		<li><a href=\"#\">Werkzeuge / Pr&uumlfmittel</a>
			<ul>
				<li><a href=\"devices_main.php\">&Uumlbersicht</a></li>
			</ul>
			</li>
		<li><a href=\"devices_lend.php\">Ger&aumlt buchen</a></li>
		<li><a href=\"mydevices.php\">Meine Ger&aumlte</a></li>
	  </ul>
	  </nav>
	</div>
	";
	}
	else if($user_group_ID == 4)			// Navigation eines Gastes laden
	{
	echo
	"
	<div id =\"navigation\">
	<nav role=\"navigation\">
	  <ul>
		<li><a href=\"main.php\">Startseite</a></li>
		<li><a href=\"#\">Werkzeuge / Prüfmittel</a>
			<ul>
				<li><a href=\"devices_main.php\">&Uumlbersicht</a></li>
			</ul>
			</li>
	  </ul>
	  </nav>
	</div>
	";
	}
}


/* ***************************************************************************************************************
  PrintFooter()

  @brief 		  	Erstellt den gesammten Footer der Seite

  @ 

  @
**************************************************************************************************************** */

function PrintFooter()
{
echo <<<EOT
	<div id="footer">
	  <ul>
		<li><a href="contact.php">Kontakt</a></li>
	  </ul>
	</div>

  </div>

</body>
</html>

EOT;

}

?>