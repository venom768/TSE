<?php
/* 
  @file Modul zur Datenbankanbindung des Projektes TSE Lagerverwaltung
*/

define( "__INC_DBFUNCS__", true );

define('DB_SERVER', 'localhost');
define('DB_DATABASE', 'tse_db');

/* ***************************************************************************************************************
  dbconnect( string $user )

  @brief 		  	Liefert eine DB-Verbindung als Objekt vom Typ mysqli

  @function 		Baut eine Verbindung zum DB-Server und der DB auf. 
					Benutzt wird die PHP-API mysqli.
					Die Passwörter sind sha256 verschlüsselt

  @param 		  	$user ein Username als String

  @return 			ein Objekt der Klasse mysqli im Erfolgsfall sonst wird das Script verlassen
**************************************************************************************************************** */

function dbconnect( $user )
{
	// Zugriffsrechte für die Datenbank überprüfen
	switch ( $user )
	{
	case 0:			$login		= "tseinitial";
					$password	= "4DDCE1B9E5A66BF3634B1512C5AC8E3C8451AC3D4AABC040CE2BE40040A2DBC6";
					break;
    case 1:			$login  	= "tseadmin";
					$password 	= "E753EB6DA0AC8FF54FA913C725044CC1AD2324991BCEED49ADEDFF5F6B8C635B";
					break;
	case 2:			$login  	= "tsewm";
					$password 	= "E1541287B2A6A92DD7F910615C0E4FD49BBC7FF76969CC7DC922899CAB0A65C1";
					break;    			  		  
	case 3: 		$login  	= "tseuser";
					$password 	= "CB738636BA094FB236DF5C510946934876C88C1F7F2CE49FE44FAA22C5C6CDFC";
					break;    				  
    case 4: 		$login  	= "tseguest";
					$password 	= "48A8294F0323D2AC8E64300E56B6942128980D6DE15584016CD5A6D5F572704C";
					break;
    case "root" : 	// Interne Fehlernummer in die Logdatei schreiben
					CreateErrorLogfile( 49 );
					// Weiterleitung zur Fehlerseite
					header("location: error.php?error=200");
					break;
    default     : 	$login  = NULL;
					$password = NULL;
					// Interne Fehlernummer in die Logdatei schreiben
					CreateErrorLogfile( 48 );
					// Weiterleitung zur Fehlerseite
					header("location: error.php?error=200");
					break;
	}

	// Datenbankverbindung herstellen
	$db = new mysqli(DB_SERVER, $login, $password, DB_DATABASE);
	
	if( $db->connect_error )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung in Logdatei schreiben
		CreateErrorLogfile( 10, $db->connect_errno, $db->connect_error);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	else if( TSE_DEBUG == TRUE )
	{
		// Ausgabe des Datenbankservers bei eingeschaltetem Debugging
		echo 'Datenbankverbindung zu folgendem Host aufgebaut:' .$db->host_info;			
	}
	
	return $db;
}

/* ***************************************************************************************************************
  GetUserIDByLogin( $db, $login, $password )

  @brief 		Liefert eine user_ID über die Anmeldedaten

  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli
				$login  	Ein Anmeldename als String
				$password 	Ein Passwort verschlüsselt durch password_hash()

  @return 		eine user_ID als Ganzzahl im Erfolgsfall, sonst false
**************************************************************************************************************** */

function GetUserIDByLogin( $db, $email, $password )
{
	// Schutz vor MySQL injection
	$email 		= 	stripslashes($email);
	$email 		= 	mysqli_real_escape_string($db, $email);
	$password 	= 	mysqli_real_escape_string($db, $password);
	
	// Vom Benutzer eingegebenes Passwort mit dem sha1 Verschlüsselungsalgorithmus verschlüsseln
	$password = sha1($password);
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// E-Mail Adresse und Passwort mit der Datenbank vergleichen
	$sql = "SELECT user_ID FROM tse_user WHERE user_email=? AND user_password=?";
	
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("ss", $email, $password);

		// Das Prepared Statement ausführen
		$prepstmt->execute();

		// Ergebnis holen
		$result = $prepstmt->get_result();

		// Datensatz fetchen
		$ds = $result->fetch_assoc();

		// Das Prepared Statement beenden
		$prepstmt->close();
		
		if ( $ds == NULL )
		{
			$user_ID = false;			    // kein DS gefunden 
		}
		else
		{
			$user_ID = $ds['user_ID'];		// User_ID holen
		}
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 20, $db->prepstmt_errno, $db->prepstmt_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $user_ID;
}

/* ***************************************************************************************************************
  GetUserGroupIDByLogin( $db, $login, $password )

  @brief 		Liefert eine user_group_ID über die Anmeldedaten

  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli
				$user_ID  	Eine user_ID als Ganzzahl

  @return 		eine user_Group_ID als Ganzzahl und eine group_description als string im Erfolgsfall, sonst false
**************************************************************************************************************** */

function GetUserGroupByUserID( $db, $user_ID )
{
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// User_ID mit der Datenbank vergleichen
	$sql	=	"SELECT tse_user.user_group_ID, tse_group.group_description ". 
				"FROM tse_user JOIN tse_group ". 
				"ON tse_user.user_group_ID = tse_group.group_ID ". 
				"WHERE user_ID =?";
			
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("i", $user_ID);

		// Das Prepared Statement ausführen
		$prepstmt->execute();

		// Ergebnis holen
		$result = $prepstmt->get_result();

		// Datensatz fetchen
		$ds = $result->fetch_assoc();

		// Das Prepared Statement beenden
		$prepstmt->close();
		
		if ( $ds === NULL )
		{
			$user_group_ID = false;							// kein DS gefunden 
			$group_description = false;						// kein DS gefunden 	
		}
		else
		{
			$user_group_ID = $ds['user_group_ID'];			// User_ID holen
			$group_description = $ds['group_description'];	// Beschreibung der Gruppe holen
		}
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 21, $db->prepstmt_errno, $db->prepstmt_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return array($user_group_ID, $group_description);
}

/* ***************************************************************************************************************
  SetLoginTimeByUserID( $db, $user_ID )

  @brief 		Setzt den Zeitstempel des letzten erfolgreichen Loginversuchs für eine User_ID

  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli
				$user_ID 	Eine gültige user_ID

  @return 		true im Erfolgsfall, sonst false
**************************************************************************************************************** */

function SetLoginTimeByUserID( $db, $user_ID )
{
	$logintime = false;
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// Auf vorhandene User_ID prüfen
	if(user_ID !== false)
	{
		// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
		// Zeitstempel im entsprechenden User aktualisieren
		$sql 	= 	"UPDATE tse_user SET user_last_login='".date('c')."' WHERE user_ID =?";
	
		// SQL-Abfrage mit Prepared Statement durchführen
		if( $prepstmt->prepare( $sql ) )
		{
			// Entsprechende Daten an die Platzhalter binden
			$prepstmt->bind_param("i", $user_ID);

			// Das Prepared Statement ausführen
			if( !$prepstmt->execute() )
			{
				// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
				CreateErrorLogfile( 22, 'Execute fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
				// Weiterleitung zur Fehlerseite
				header("location: error.php?error=200");	
			}
			else
			{
				$logintime = true;
			}
			// Das Prepared Statement beenden
			$prepstmt->close();
		}
		else
		{
			// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
			CreateErrorLogfile( 22, 'Prepare fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
			// Weiterleitung zur Fehlerseite
			header("location: error.php?error=200");
		}
	}
		
	return $logintime;
}

/* ***************************************************************************************************************
  GetDataforWklist( $db )

  @brief 		Es wird abgefragt ob das wk_date zwischen dem heutigen Datum und dem heutigen datum +30 Tage liegt

  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli

  @return 		ein Array mit allen relevanten Datensätzen von der Tabelle Device/Model/Status die in den Wartungsintervall fallen
**************************************************************************************************************** */

function GetDataForWklist($db)
{
	// alles relevanten Daten aus den Tabllen via Join holen
	$sql	=	"SELECT device_ID, device_wk_date, device.model_ID, model.modeldescription, device_defects, device_equipment, device_status ".
				"FROM device INNER JOIN model ".
				"ON device.model_ID = model.model_ID ".
				"WHERE device_wk_date BETWEEN DATE_SUB(DATE(NOW()), INTERVAL 1 YEAR) AND DATE_ADD(DATE(NOW()), INTERVAL 30 DAY) ";

	
	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 23, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Associatives Array
	$ds = $result->fetch_all(MYSQLI_ASSOC);
	
	return $ds;
} 

/* ***************************************************************************************************************
  GetDataforEventlist($db)

  @brief 		Es weden die letzen Ereignisse, der letzen 30 Tage, aus der Datenbank abgefragt

  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli

  @return 		ein Array mit allen relevanten Datensätzen von der Tabelle Device/lend die in den Eventintervall fallen
**************************************************************************************************************** */

function GetDataForEventlist($db)
{
	// alles relevanten Daten aus den Tabllen via Join holen
	$sql	=	"SELECT stamp_lend,stamp_return,tse_user.user_firstname,tse_user.user_lastname,device.device_ID,tse_position.pos_address,tse_position.pos_plz, model.modeldescription ".
				"FROM lend ".
                "INNER JOIN device ON (lend.device_ID = device.device_ID) ".
                "INNER JOIN model ON (model.model_ID = device.model_ID) ".
	            "INNER JOIN tse_position ON (device.pos_ID = tse_position.pos_ID) ".
                "INNER JOIN tse_user ON (lend.user_ID = tse_user.user_ID) ".
				"WHERE stamp_return BETWEEN DATE_SUB(DATE(NOW()), INTERVAL 30 DAY) AND DATE(NOW()) OR stamp_lend BETWEEN DATE_SUB(DATE(NOW()), INTERVAL 30 DAY) AND DATE(NOW())".
				"ORDER BY stamp_lend ASC, stamp_return ASC ";

	
	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 24, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Associatives Array
	$ds = $result->fetch_all(MYSQLI_ASSOC);
	
	return $ds;
}

/* ***************************************************************************************************************
  GetDataforOverviewDevices($db)

  @brief 		Es werden alle benötigen Daten aus der Tabelle Device geladen um diese in einer Tabelle anzuzeigen

  @param 		$db	Datenbankverbindung als Objekt der Klasse mysqli

  @return 		ein Array mit allen relevanten Datensätzen von der Tabelle Device/tse_pos/tse_user 
**************************************************************************************************************** */

function GetDataForOverviewDevices($db)
{
	// alles relevanten Daten aus den Tabllen via Join holen
	$sql	=	"SELECT device_ID,device_qr_code,device_sn,device_defects,device_equipment,device_status,device_wk_date,tse_position.pos_address,tse_position.pos_plz,tse_user.user_firstname,tse_user.user_lastname,upperclass.upperclass_description,underclass.underclass_description,model.modeldescription ".
				"FROM device ".
	            "INNER JOIN tse_position ON (device.pos_ID = tse_position.pos_ID) ".
                "INNER JOIN tse_user ON (device.user_ID = tse_user.user_ID) ".
				"INNER JOIN upperclass ON (device.upperclass_ID = upperclass.upperclass_ID) ".
				"INNER JOIN underclass ON (device.underclass_ID = underclass.underclass_ID) ".
				"INNER JOIN model ON (device.model_ID = model.model_ID) ".
				"ORDER BY device_ID ASC ";
	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 25, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Associatives Array
	$ds = $result->fetch_all(MYSQLI_ASSOC);
	
	return $ds;
}

/* ***************************************************************************************************************
  GetDataforOverviewUsers($db)

  @brief 		Es werden alle benötigen Daten aus der Tabelle tse_user geladen um diese in einer Tabelle anzuzeigen

  @param 		$db	Datenbankverbindung als Objekt der Klasse mysqli

  @return 		ein Array mit allen relevanten Datensätzen von der Tabelle tse_user/tse_group 
**************************************************************************************************************** */

function GetDataForOverviewUsers($db)
{
	// alles relevanten Daten aus den Tabllen via Join holen
	$sql	=	"SELECT user_ID,user_salutation,user_firstname,user_lastname,user_title,user_email,user_telnr, user_create_stamp, user_last_login,user_status,tse_group.group_description ".
				"FROM tse_user ".
	            "INNER JOIN tse_group ON (tse_user.user_group_ID = tse_group.group_ID) ".
				"ORDER BY user_ID ASC ";
				
	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 26, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Associatives Array
	$ds = $result->fetch_all(MYSQLI_ASSOC);
	
	return $ds;
}

/* ***************************************************************************************************************
  GetDataforEditUsersByUserID($db, $userID)

  @brief 		Es werden alle User informationen zum updaten des Users aus der Datenbank geholt je nach der mitgegebenen ID

  @param 		$db	Datenbankverbindung als Objekt der Klasse mysqli, $userID die übergebene ID aus dem GET (bearbeiten)

  @return 		einen Datensatz um das bearbeiten Formular zu füllen 
**************************************************************************************************************** */

function GetDataForEditUsersByUserID($db, $userID)
{
	// Datensatz
	$sql	=	"SELECT user_ID,user_salutation,user_firstname,user_lastname,user_title,user_email,user_telnr, user_create_stamp, user_last_login,user_status,tse_group.group_description ".
				"FROM tse_user ".
	            "INNER JOIN tse_group ON (tse_user.user_group_ID = tse_group.group_ID) ".
				"WHERE user_ID = '$userID' ";

	
	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 27, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Array
	$ds = $result->fetch_assoc();
	
	return $ds;
}

/* ***************************************************************************************************************
  GetDataforDeleteUsersByUserID($db, $userID)

  @brief 		Es werden User Vor- und Nachname aus der Datenbank geholt zur anzeige 

  @param 		$db	Datenbankverbindung als Objekt der Klasse mysqli, $userID die übergebene ID aus dem GET (löschen)

  @return 		einen Datensatz um das bearbeiten Formular zu füllen 
**************************************************************************************************************** */

function GetDataForDeleteUsersByUserID($db, $userID)
{
	// Datensatz
	$sql	=	"SELECT user_ID,user_firstname,user_lastname ".
				"FROM tse_user ".
				"WHERE user_ID = '$userID' ";

	
	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 27, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Array
	$ds = $result->fetch_assoc();
	
	return $ds;
}
	 	 		 	 	 	 
/* ***************************************************************************************************************
  GetDataforContacts($db)

  @brief 		Es werden alle Admins und Lagerverwalter Daten aus der Tabelle tse_user geladen um diese bei den Kontaktdaten anzuzeigen

  @param 		$db	Datenbankverbindung als Objekt der Klasse mysqli

  @return 		ein Array mit allen relevanten Datensätzen von der Tabelle tse_user
**************************************************************************************************************** */

function GetDataForContacts($db)
{
	// alles relevanten Daten aus den Tabllen via Join holen
	$sql	=	"SELECT user_salutation,user_firstname,user_lastname,user_title,user_email,user_telnr,tse_group.group_description ".
				"FROM tse_user ".
	            "INNER JOIN tse_group ON (tse_user.user_group_ID = tse_group.group_ID) ".
				"WHERE tse_group.group_description = 'Admin' OR tse_group.group_description = 'Lagerverwalter' ".
				"ORDER BY tse_group.group_description ASC ";

	// Abfrage abschicken und Ergebnis entgegennehmen
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 26, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	
	// Ergebnis vorhanden? Datensatz fetchen als  Associatives Array
	$ds = $result->fetch_all(MYSQLI_ASSOC);
	
	return $ds;
}

/* ***************************************************************************************************************
  CreateUser ( $db, $user_salutation, $user_firstname, $user_lastname, $user_title, $user_email, $user_password, $user_tel, $user_group_ID, $user_status )

  @brief 		Legt einen neuen Benutzer an
  
  @function		Legt einen neuen Benutzer über ein SQL Statement mit den vorher übergeben Werten an

  @param 		$db	eine Datenbankverbindung

  @return 		ein true im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function CreateUser( $db, $user_ID, $user_salutation, $user_firstname, $user_lastname, $user_title, $user_email, $user_password, $user_tel, $user_group_ID, $user_status )
{
	$ucreate = false;
	
	// Zeitstempel der Registrierung
	$regtime = date('c');
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// Benutzerdaten in die Datenbank einfügen
	$sql	=	"INSERT INTO tse_user (user_salutation, user_firstname, user_lastname, user_title, user_email, user_password, user_telnr, user_group_ID, user_status, user_create_by_ID, user_create_stamp)
				 VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
							
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("sssssssisis", $user_salutation, $user_firstname, $user_lastname, $user_title, $user_email, $user_password, $user_tel, $user_group_ID, $user_status, $user_ID, $regtime);

		// Das Prepared Statement ausführen
		if( !$prepstmt->execute() )
		{
			// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
			CreateErrorLogfile( 28, 'Execute fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
			// Weiterleitung zur Fehlerseite
			header("location: error.php?error=200");	
		}
		else
		{
			$ucreate = true;
			$GLOBALS['errstr'] = "Sie haben erfolgreich einen neuen Benutzer angelegt!";
			return true;
		}
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 28, 'Prepare fehlgeschlagen!', $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $ucreate;
}

/* ***************************************************************************************************************
  UpdateUser ( $db, $user_salutation, $user_firstname, $user_lastname, $user_title, $user_email, $user_tel, $user_group_ID, $user_status )

  @brief 		Aktualisiert einen vorhandenen Benutzer an
  
  @function		Aktualisiert einen Benutzer über ein SQL Statement mit den vorher übergeben Werten an

  @param 		$db	eine Datenbankverbindung

  @return 		ein true im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function UpdateUser( $db, $user_ID, $user_salutation, $user_firstname, $user_lastname, $user_title, $user_email, $user_tel, $user_group_ID, $user_status )
{
	// Zeitstempel der Änderung
	$modifytime = date('c');
	
	// Benutzerdaten einfügen
	$sql	=	"UPDATE tse_user SET user_salutation='$user_salutation', user_firstname='$user_firstname', user_lastname='$user_lastname', user_title='$user_title', user_email='$user_email', user_telnr='$user_tel', user_group_ID='$user_group_ID', user_status='$user_status', user_modify_by_ID='$user_ID', user_modify_stamp='$modifytime' ".
				"WHERE user_ID = '$user_ID'";
			
	// Abfrage abschicken und Benutzer Aktualisieren
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		return false;
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 29, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	else				
	{	
		$GLOBALS['errstr'] = "Sie haben den Benutzer erfolgreich aktualisiert!";
		return true;
	}
}


/* ***************************************************************************************************************
  DeleteUser ( $db, $user_salutation, $user_firstname, $user_lastname, $user_title, $user_email, $user_tel, $user_group_ID, $user_status )

  @brief 		Löscht einen vorhandenen Benutzer nach mitgegebender ID
  
  @function		Löscht einen Benutzer über ein SQL Statement mit den mitgegebenen ID

  @param 		$db	eine Datenbankverbindung

  @return 		ein true im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function DeleteUser( $db, $user_ID)
{
	
	// Benutzerdaten einfügen
	$sql	=	"DELETE FROM tse_user ".
				"WHERE user_ID = '$user_ID'";
			
	// Abfrage abschicken und Benutzer Aktualisieren
	$result	=	$db->query( $sql );
	
	// Abfrage fehlerhaft
	if ( $result === false )
	{
		return false;
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 36, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	else				
	{	
		$GLOBALS['errstr'] = "Sie haben den Benutzer erfolgreich gelöscht!";
		return true;
	}
}



/* ***************************************************************************************************************
  CheckUserEmail( $db, $email )

  @brief 		  	Überprüft ob eine E-Mail Adresse in der Datenbank bereits vorhanden ist

  @param 		  	$db 	eine Datenbankverbindung
					$email 	eine E-Mail Adresse als string
					$error 	Fehlerausgabe ein- oder ausschalten

  @return 			eine true wenn E-Mail Adresse bereits vorhanden, ein false wenn E-Mail Adresse nicht vorhanden
**************************************************************************************************************** */

function CheckUserEmail( $db, $email, $error=true )
{
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// Überprüfung auf bereits vorhandene E-Mail Adressen
	$sql = "SELECT user_email FROM tse_user WHERE user_email =?";
	
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("s", $email);

		// Das Prepared Statement ausführen
		$prepstmt->execute();

		// Ergebnis holen
		$result = $prepstmt->get_result();

		// Datensatz fetchen
		$ds = $result->fetch_assoc();

		// Das Prepared Statement beenden
		$prepstmt->close();
		
		if ( $ds != NULL )
		{
			// Wenn E-Mail Adresse vorhanden
			$existent = true;
			if( $error === true )
			{
				$GLOBALS['errstr'] = ErrorByNumber( 104 );
			}
		}
		else
		{
			// Wenn E-Mail Adresse nicht vorhanden
			$existent = false;
			if( $error === true )
			{
				$GLOBALS['errstr'] = ErrorByNumber( 114 );
			}
		}
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 37, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $existent;
}

/* ***************************************************************************************************************
  CheckUserPassword ( $db, $password )
  
  @brief 		Überprüft ob ein Passwort mit dem in der Datenbank übereinstimmt
  
  @function		Die E-Mail Adresse des Benutzers wird direkt aus der Session genommen
				Das übergebene Passwort wird direkt in der Datenbank verglichen
				Die User_ID des zurück gegebenen Datensatzes wird mit der User_ID aus der Session verglichen
  
  @param 		$db				eine Datenbankverbindung
				$password		ein Passwort als String

  @return 		eine true wenn die Passwörter übereinstimmen, sonst ein false
**************************************************************************************************************** */

function CheckUserPassword ( $db, $password )
{
	$comparsion = false;
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// Benutzername und Passwort mit der Datenbank abgleichen
	$sql = "SELECT user_ID FROM tse_user WHERE user_email=? AND user_password=?";
		
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("ss", $_SESSION['email'], $password);

		// Das Prepared Statement ausführen
		$prepstmt->execute();

		// Ergebnis holen
		$result = $prepstmt->get_result();

		// Datensatz fetchen
		$ds = $result->fetch_assoc();

		// Das Prepared Statement beenden
		$prepstmt->close();
		
		// Zusätzliche Überprüfung der User_ID
		if ( $ds['user_ID'] !== $_SESSION['user_ID'] )
		{
			// Das alte Passwort stimmt nicht mit dem in der Datenbank überein
			$GLOBALS['errstr'] = ErrorByNumber( 111 );
		}
		else
		{
			$comparsion = true;
		}
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 35, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $comparsion;
}

/* ***************************************************************************************************************
  UpdateUserPassword ( $db, $current_password, $new_password )
  
  @brief 		Ändert das Passwort eines Benutzers
  
  @function		Ändert das Passwort eines Benutzers und fragt dazu das alte Passwort ab

  @param 		$db					eine Datenbankverbindung
				$current_password	das aktuelle Passwort
				$password			das neue Passwort
				

  @return 		eine true im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function UpdateUserPassword ( $db, $user_ID, $password )
{
	$updateupw = false;
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// Passwort im entsprechenden User aktualisieren
	$sql = "UPDATE tse_user SET user_password=? WHERE user_ID =?";
	
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("si", $password, $user_ID);

		// Das Prepared Statement ausführen
		if( !$prepstmt->execute() )
		{
			// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
			CreateErrorLogfile( 35, 'Execute fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
			// Weiterleitung zur Fehlerseite
			header("location: error.php?error=200");
		}
		else
		{
			$updateupw = true;
			$GLOBALS['errstr'] = "Sie haben erfolgreich ihr Passwort geändert!";
		}
		// Das Prepared Statement beenden
		$prepstmt->close();
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 35, 'Prepare fehlgeschlagen!' ,$db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $updateupw;
}

/* ***************************************************************************************************************
  UpdateUserPasswordByPasswordCode ( $db, $email, $password_code, $new_password )
  
  @brief 		Ändert das Passwort eines Benutzers
  
  @function		Ändert das Passwort eines Benutzers anhand der E-Mail Adresse und des Passwort Code

  @param 		$db					eine Datenbankverbindung
				$email				eine E-Mail Adresse als String
				$password_code		eine Zufallszahl die mit sha256 gehashed wurde
				$new_password		das neue Passwort
				
  @return 		eine true im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function UpdateUserPasswordByPasswordCode ( $db, $email, $password_code, $new_password )
{
	$updateupw = false;
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
	// Passwort im entsprechenden User aktualisieren
	$sql = "UPDATE tse_user SET user_password=? WHERE user_email=? AND password_code=?";
	
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("sss", $new_password, $email, $password_code);

		// Das Prepared Statement ausführen
		if( !$prepstmt->execute() )
		{
			// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
			CreateErrorLogfile( 38, 'Execute fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
			// Weiterleitung zur Fehlerseite
			header("location: error.php?error=200");
		}
		else
		{
			$updateupw = true;
			$GLOBALS['errstr'] = "Sie haben erfolgreich ihr Passwort geändert!";
		}
		// Das Prepared Statement beenden
		$prepstmt->close();
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 38, 'Prepare fehlgeschlagen!' ,$db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $updateupw;
}

/* ***************************************************************************************************************
  SetPasswordCodeAndTime( $db, $email)

  @brief 		Setzt den Passwort- Code und Zeitstempel für einen Benutzer
  
  @function		Setzt den Passwort- Code und Zeitstempel anhand einer E-Mail Adresse
				Der Passwort Code wird vor dem abspeichern in der Datenbank gehashed

  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli
				$email	 	Eine E-Mail Adresse als string
				$rstring	Eine Zufälliger String

  @return 		true im Erfolgsfall, sonst false
**************************************************************************************************************** */

function SetPasswordCodeAndTime( $db, $email, $rstring )
{
	$pwcode = false;
	$rstring = sha1( $rstring );
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// E-Mail erneut überprüfen
	if( $email !== false)
	{
		// SQL-Abfrage erstellen und Platzhalter für Prepared Statements setzen
		// Passwort- Code und Zeitstempel setzen
		$sql 	= 	"UPDATE tse_user SET password_code=?, password_time=NOW() WHERE user_email =?";
		$result	=	$db->query( $sql );

		// SQL-Abfrage mit Prepared Statement durchführen
		if( $prepstmt->prepare( $sql ) )
		{
			// Entsprechende Daten an die Platzhalter binden
			$prepstmt->bind_param("ss", $rstring, $email);

			// Das Prepared Statement ausführen
			if( !$prepstmt->execute() )
			{
				// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
				CreateErrorLogfile( 35, 'Execute fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
				// Weiterleitung zur Fehlerseite
				header("location: error.php?error=200");
			}
			else
			{
				$pwcode = true;
			}
			// Das Prepared Statement beenden
			$prepstmt->close();
		}
		else
		{
			// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
			CreateErrorLogfile( 35, 'Prepare fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
			// Weiterleitung zur Fehlerseite
			header("location: error.php?error=200");
		}
		
		return $pwcode;
	}
}

/* ***************************************************************************************************************
  GetPasswordCodeAndTime( $db, $email)

  @brief 		Holt falls vorhanden einen Passwort- Code und den dazugehörigen Zeitstempel einens Benutzers
  
  @function		Überprüft Passwort- Code und Zeit auf NULL
  
  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli
				$email	 	Eine E-Mail Adresse als string
				$error 		Fehlerausgabe ein- oder ausschalten

  @return 		einen Passwort-Code als String, false wenn Code oder Zeitstempel nicht vorhanden
**************************************************************************************************************** */

function GetPasswordCodeAndTime( $db, $email, $error=true )
{
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// Passwort- Code und Zeitstempel holen
	$sql	=	"SELECT password_code, password_time ". 
				"FROM tse_user ".  
				"WHERE user_email =? AND password_code IS NOT NULL AND password_time IS NOT NULL";
	
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("s", $email);

		// Das Prepared Statement ausführen
		$prepstmt->execute();

		// Ergebnis holen
		$result = $prepstmt->get_result();

		// Datensatz fetchen
		$ds = $result->fetch_assoc();
		
		// Das Prepared Statement beenden
		$prepstmt->close();

		if( $ds === NULL )	
		{
			// Passwort- Code und Zeitstempel nicht vorhanden
			$code = false;
			$time = false;
		}
		else					
		{
			// Passwort- Code und Zeitstempel vorhanden
			$code = $ds['password_code'];
			$time = $ds['password_time'];
			if( $error === true)
			{
				$GLOBALS['errstr'] = ErrorByNumber( 115 );
			}
		}
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 31, 'Prepare fehlgeschlagen!',$db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return array( $code, $time );
}

/* ***************************************************************************************************************
  DeletePasswordCodeAndTime( $db, $email)

  @brief 		Löscht den Passwort- Code und den dazugehörigen Zeitstempel einens Benutzers
  
  @function		Setzt die Datenbankfelder auf NULL
  
  @param 		$db	 		Datenbankverbindung als Objekt der Klasse mysqli
				$email	 	Eine E-Mail Adresse als string

  @return 		ein true bei Erfolg, sonst ein false
**************************************************************************************************************** */

function DeletePasswordCodeAndTime( $db, $email)
{
	$pwcode = false;
	
	// Ein Objekt vom Typ mysqli_stmt
	$prepstmt = $db->stmt_init();
	
	// Passwort- Code und Zeitstempel holen
	$sql	=	"UPDATE tse_user SET password_code=NULL, password_time=NULL ".
				"WHERE user_email =?";
	
	// SQL-Abfrage mit Prepared Statement durchführen
	if( $prepstmt->prepare( $sql ) )
	{
		// Entsprechende Daten an die Platzhalter binden
		$prepstmt->bind_param("s", $email);

		// Das Prepared Statement ausführen
		if( !$prepstmt->execute() )
		{
			// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
			CreateErrorLogfile( 39, 'Execute fehlgeschlagen!',$db->prepstmt_errno, $db->prepstmt_error, $sql);
			// Weiterleitung zur Fehlerseite
			header("location: error.php?error=200");
		}
		else
		{
			$pwcode = true;
		}
		// Das Prepared Statement beenden
		$prepstmt->close();
	}
	else
	{
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 39, 'Prepare fehlgeschlagen!',$db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	return $pwcode;
}

/* ***************************************************************************************************************
  GetOberkategorie($db)

  @brief 		  Holt alle Oberkategorien aus der Datenbank
  
  @function		Holt die Beschreibung und ID der Oberkategorie aus der Datenbank
  
  @param 		  $db Datenbankverbindung als Objekt der Klasse mysqli
              
  @return 	  ein mehrdimensionales Array 
**************************************************************************************************************** */
function GetOberkategorie($db)
{
  $db -> set_charset('utf8');
  //SQL-Abfrage erstellen
  $sqlString = "SELECT upperclass_ID, upperclass_description FROM upperclass";

  //Abfrage abschicken und Ergebnis entgegen nehmen
  $result = $db->query($sqlString);

  //Abfrage fehlerhaft?
  if($result === false)
  {
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 32, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
  }
  
  // Ergebnis vorhanden? Datensatz fetchen
  $ds = $result->fetch_all();
  
  if ($ds === NULL)
  {
   $uppeclass_description = false;		
   $upperclass_ID=false;
  }

  return ($ds);
}

/* ***************************************************************************************************************
  GetUnterkategorieByOberkategorie($oberkategorie,$db)

  @brief 		  Holt die Unterkategorien aus der Datenbank
  
  @function		Holt die passende Beschreibung der Unterkategorie aus der Datenbank
  
  @param 		  $oberkategorie  Wert der zuvor ausgewählten Oberkategorie durch eine Dropdown Liste
              $db	 		         Datenbankverbindung als Objekt der Klasse mysqli
           
  @return 	  ein mehrdimensionales Array 
**************************************************************************************************************** */
function GetUnterkategorieByOberkategorie($oberkategorie,$db)
{
 $db -> set_charset('utf8');
 //SQL-Abfrage erstellen
 $sqlString = "SELECT upperclass.upperclass_description, underclass.underclass_description, underclass.underclass_ID FROM upperclass INNER JOIN underclass ON upperclass.upperclass_ID = underclass.upperclass_ID WHERE upperclass_description = '$oberkategorie'";
  //Abfrage abschicken und Ergebnis entgegen nehmen
  $result = $db->query($sqlString);

  //Abfrage fehlerhaft?
  if($result === false)
  {
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 33, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
  }
  
  // Ergebnis vorhanden? Datensatz fetchen
  $ds = $result->fetch_all();
  
  if ($ds === NULL)
  {
   $underclass_description = false;		
   $underclass_ID=false;
  }

  return ($ds);
}

/* ***************************************************************************************************************
  GetModelByUnterkategorie($unterkategorie,$db)

  @brief 		  Holt den Typ aus der Datenbank
  
  @function		Holt die passende Beschreibung der Unterkategorie aus der Datenbank
  
  @param 		  $unterkategorie  Wert der zuvor ausgewählten Oberkategorie durch eine Dropdown Liste
              $db	 		         Datenbankverbindung als Objekt der Klasse mysqli
           
  @return 	  ein mehrdimensionales Array 
**************************************************************************************************************** */
function GetModelByUnterkategorie($unterkategorie,$db)
{
 $db -> set_charset('utf8');
 //SQL-Abfrage erstellen
 $sqlString = "SELECT model_ID, modeldescription FROM model WHERE underclass_ID = ".$unterkategorie.";";
 

  //Abfrage abschicken und Ergebnis entgegen nehmen
  $result = $db->query($sqlString);

  //Abfrage fehlerhaft?
  if($result === false)
  {
		// Interne Fehlernummer und MySQLi Fehlernummer und Beschreibung sowie fehlerhafte SQL-Abfrage in Logdatei schreiben
		CreateErrorLogfile( 34, $db->connect_errno, $db->connect_error, $sql);
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
  }
  
  // Ergebnis vorhanden? Datensatz fetchen
  $ds = $result->fetch_all();
  
  if ($ds === NULL)
  {
   $uppeclass_description = false;		
   $upperclass_ID=false;
  }

  return ($ds);
}
?>