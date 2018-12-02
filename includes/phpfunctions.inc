<?php
/* 
  @file Funktionen zur Ablaufsteuerung der Programmlogik innerhalb der TSE Lagerverwaltung
*/

define( "__INC_PHPFUNCS__", true );

/* ***************************************************************************************************************
Init()

@brief 		  	Initialisiert globale Einstellungen für alle .php Seiten

**************************************************************************************************************** */

function Init( $debugmode=false )
{
	// Includes laden
	include_once('./includes/dbfunctions.inc'); 	// Datenbankfunktionen laden
	include_once('./includes/htmlfunctions.inc');	// HTML-Funktionen laden
	include_once('./includes/phpfunctions.inc'); 	// PHP-Funktionen laden
	include_once('./includes/debug.inc'); 			// Debugfunktionen laden
	
	// Überprüfung ob Datenbankfunktionen korrekt geladen wurden
	if ( !defined("__INC_DBFUNCS__") )
	{
		// Interne Fehlernummer in Logdatei schreiben
		CreateErrorLogfile( 11 );
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	// Überprüfung ob die HTML-Funktionen korrekt geladen wurden
	if ( !defined("__INC_HTMLFUNCS__") )
	{
		// Interne Fehlernummer in Logdatei schreiben
		CreateErrorLogfile( 13 );
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	// Überprüfung ob die Debug-Funktionen korrekt geladen wurden
	if ( !defined("__INC_DEBUG__") )
	{
		// Interne Fehlernummer in Logdatei schreiben
		CreateErrorLogfile( 14 );
		// Weiterleitung zur Fehlerseite
		header("location: error.php?error=200");
	}
	
	define("TSE_DEBUG", $debugmode);			// Debugmodus ein- oder ausschalten
	
	Debug();									// Fehlerausgabe ein- oder ausschalten
	
	// Leerstring für Fehlermeldungen oder Informationen als Gloable Variable initialisieren
	$GLOBALS['errstr'] = "";
	
	if( isset( $_GET['error'] ) )
	{
		// Fehlermeldungen welche über Get von anderen Seiten übertragen wurden
		$GLOBALS['errstr'] = ErrorByNumber( $_GET['error'] );
	}	
}

/* *******************************************************************************
Login( $db )

@brief		Führt den Loginprozess aus

@function	Überprüft die vom Benutzer eingegebenen Daten und vergleicht diese mit der Datenbank
			Bei Erfolg, Starten einer Session und Datenübergabe in die Session
			Setzen eines Zeitstempels bei erfolgreichem Login
			Weiterleitung auf main.php

@param		$db eine Datenbankverbindung
******************************************************************************* */

function Login( $db )
{
	// Beginn einer Session
	session_start();
	
	// Überprüfung ob der Submit-Button bereits gedrückt wurde
	if( isset( $_POST['submit'] ) )					
	{
		// Überprüfung ob beide Felder ausgefüllt wurden
		if( empty( $_POST['email'] ) || empty( $_POST['password'] ) )		
		{
			$GLOBALS['errstr'] = ErrorByNumber( 100 );
		}
		else
		{
			$email = $_POST['email'];
			$password = $_POST['password'];
			
			// Datenbank mit utf8 auslesen
			$db->set_charset('utf8');
			
			// user_ID und user_group_ID aus der Datenbank holen
			$user_ID = GetUserIDByLogin($db, $email, $password);
			$user_group = GetUserGroupByUserID($db, $user_ID);
			
			// Wenn eine user_ID und eine user_group_ID passend zu den Logindaten vorhanden ist dann Daten in die Session übergeben
			if($user_ID !== false && $user_group !== false)
			{
				// Zeitstempel für den letzten erfolgreichen Loginversuch setzen
				SetLoginTimeByUserID( $db, $user_ID );
				// Passwort- Code und Zeitstempel zurücksetzen
				DeletePasswordCodeAndTime( $db, $email );
			
				// Session initialisieren
				$_SESSION['email'] = $email; 										// E-Mail Adresse in Session übergeben
				$_SESSION['user_ID'] = $user_ID;									// User ID in Session übergeben
				$_SESSION['group_ID'] = $user_group[0];								// Gruppen ID übergeben
				$_SESSION['group_description'] = $user_group[1];					// Gruppenbezeichnung übergeben
				$_SESSION['IP'] = $_SERVER['REMOTE_ADDR'];							// IP-Adresse des Client's übergeben
				
				// Weiterleitung zur Hauptseite
				header("location: main.php");
			}
			else
			{
				// E-Mail Adresse oder Passwort ungültig
				$GLOBALS['errstr'] = ErrorByNumber( 101 );
			}
		}
	}
}

/* *******************************************************************************
CheckLogin( $db )

@brief		Überprüft ob ein User bereits eingeloggt ist

@function	Überprüft auf eine gespeicherte E-Mail Adresse in der Session
			Überprüft ob E-Mail Adresse aus der Session auch in der Datenbank vorhanden ist
			Vergleicht die in der Session gespeicherte IP-Adresse mit der aktuellen IP-Adresse vom Client
			Bei Erfolg, Datensätze werden aktualisiert
			Bei Fehler, Session wird beendet und User wird auf Login Seite weitergeleitet
			
@param		$db eine Datenbankverbinung
******************************************************************************* */

function CheckLogin( $db )
{
	// Überprüfen ob E-Mail Adresse in der Session vorhanden ist und diese mit der Datenbank vergleichen
	if( isset( $_SESSION['email'] ) && CheckUserEmail( $db, $_SESSION['email'], false ) === true )
	{
		// Überprüfen ob die User_ID in der Session vorhanden und gültig ist
		if( isset( $_SESSION['user_ID'] ) && $_SESSION['user_ID'] !== false )
		{
			// Überprüfen ob die Group_ID in der Session vorhanden und gültig ist
			if( isset( $_SESSION['group_ID'] ) && $_SESSION['group_ID'] !== false )
			{
				// Überprüfen ob IP-Adresse des Client zu der in der Session gespeicherten IP-Adresse passt
				if( $_SERVER['REMOTE_ADDR'] === $_SESSION['IP'] )
				{
					// Datenbank mit utf8 auslesen
					$db->set_charset('utf8');
			
					// Device Datensätze aus der Datenbank holen die im Wartungsintervall liegen
					$device_data_wklist	= GetDataForWklist($db);
					// Datensätze aus der Datenbank holen für die Eventlist
					$device_data_eventlist = GetDataForEventlist($db);
					// Datensätze aus der Datenbank holen für die Übersicht Geräte
					$device_data_overviewdevices = GetDataForOverviewDevices($db);
					// Datensätze aus der Datenbank holen für die Übersicht Benutzer
					$device_data_overviewusers = GetDataForOverviewUsers($db);
					// Datensätze aus der Datenbank holen für die Ansprechpartner
					$data_for_contacts = GetDataForContacts($db);
					
					// Daten in die Session übergeben
					$_SESSION['device_data_wklist'] = $device_data_wklist;					
					$_SESSION['device_data_eventlist'] = $device_data_eventlist;		
					$_SESSION['device_data_overviewdevices'] = $device_data_overviewdevices;
					$_SESSION['device_data_overviewusers'] = $device_data_overviewusers;
					$_SESSION['data_for_contacts'] = $data_for_contacts;	
				}
				else
				{
					// Session beenden
					session_regenerate_id(); 							// Neue Session ID vergeben, mit Session nichtmehr gefunden wird
					unset( $_SESSION );  								// Inhalt der Session löschen
					session_destroy();
					// Weiterleitung auf index.php (Login)
					header('Location: index.php?error=116');	
				}
			}
			else
			{
				// Weiterleitung auf index.php (Login)
				header('Location: index.php?error=116');
			}
		}
		else
		{
			// Weiterleitung auf index.php (Login)
			header('Location: index.php?error=116');
		}
	}
	else
	{
		// Weiterleitung auf index.php (Login)
		header('Location: index.php?error=116');
	}
}

/* *******************************************************************************
Logout()

@brief		Loggt einen bereits eingeloggten User wieder aus

@function	Vergibt zuerst eine neue Session ID
			Löscht dann die Werte aus der Session
			Beendet die Session
******************************************************************************* */

function Logout()
{
	session_regenerate_id(); 	// Neue Session ID vergeben, mit Session nichtmehr gefunden wird
	unset( $_SESSION );  		// Inhalt der Session löschen
	session_destroy();
}

/* *******************************************************************************
CheckLogout()

@brief		Überprüft ob ein User sich ausgeloggt hat

@function	Überprüft ob das Hidden Feld im Logout Formular über $_POST gesendet wurde
			Zusätzlich wird verhindert das die index.php im eingeloggten Zustand aufgerufen werden kann
			Dazu wird überprüft ob bereits eine E-Mail Adresse in der Session gespeichert ist
			Wenn ein User den Logout-Button betätigt hat, dann wird die Session beendet
			Dazu wird die Funktion Logout() verwendeten
			Ist ein User eingeloggt und versucht die index.php aufzurufen, wird er auf die main.php zurück geleitet

******************************************************************************* */

function CheckLogout()
{
	if ( isset($_POST['logout']) ) 				
	{
		// Wenn der Logout-Button betätigt wurde dann Session beenden
		Logout();			
	}
	else if( !isset($_POST['logout']) && isset($_SESSION['email']) ) 
	{
		// Wenn der User ohne Betätigung des Logout-Button die index.php aufruft Weiterleitung auf die main.php
		header('Location: main.php');
	}
}

/* ***************************************************************************************************************
ValidateEmail( $db, $email )

@brief 		  	Liefert eine gültige E-Mail Adresse zurück oder setzt die entsprechende Fehlermeldung

@function		Überprüft eine E-Mail Adresse auf bestimmte Kriterien
				- E-Mail Eingabe leer
				- php Funktion filter_var() wird zur Validierung der E-Mail Adresse nach aktuellen Standards verwendet
				
				Wendet Mechanismen zur Verhinderung von MySQL Injection andere
				Setzt bei Fehler die entsprechende Fehlermeldung

@param 		  	$db eine Datenbankverbindung, $email eine E-Mail Adresse als string

@return 		eine gültige E-Mail Adresse im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function ValidateEmail( $db, $email )
{
	if(empty($email))										// Überprüfung ob E-Mail Adresse ausgefüllt wurde
	{ 
		$GLOBALS['errstr'] = ErrorByNumber( 102 );
		$email = false;
	}
	else if(!filter_var($email, FILTER_VALIDATE_EMAIL))		// Überprüfung ob E-Mail Adresse gültig ist
	{
		$GLOBALS['errstr'] = ErrorByNumber( 103 );
		$email = false;
	}
	else		
	{
		// Schutz vor MySQL Injection
		$email = stripslashes($email);
		$email = mysqli_real_escape_string($db, $email);
	}
	
	return ( $email );
}

/* ***************************************************************************************************************
ValidatePassword( $db, $password )

@brief 		  	Liefert ein gültiges verschlüsseltes Passwort zurück oder setzt die entsprechende Fehlermeldung

@function		Überprüft ein Passwort auf bestimmt Kriterien
				- Passworteingabe leer
				- Passwort kürzer als 8 Zeichen
				
				Wendet Mechanismen zur Verhinderung von MySQL Injection andere
				Verschlüsselt das Passwort
				Setzt bei Fehler die entsprechende Fehlermeldung

@param 		  	$db eine Datenbankverbindung, $password ein Passwort als string

@return 		eine gültiges verschlüsseltes Passwort im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function ValidatePassword( $db, $password )
{
	if( empty($password) )
	{
		// Wenn kein Passwort eingegeben wurde
		$GLOBALS['errstr'] = ErrorByNumber( 105 );
		$password = false;
	}
	else if( strlen($password) < 8 )
	{
		// Wenn das Passwort die minimale Länge von 8 Zeichen unterschreitet
		$GLOBALS['errstr'] = ErrorByNumber( 106 );
		$password = false;
	}
	else
	{
		// Schutz vor MySQL Injection
		$password = mysqli_real_escape_string($db, $password);
		// Passwort verschlüsseln
		$password = sha1($password);
	}
	
	return( $password );
}

/* ***************************************************************************************************************
PasswordComparsion( $password1, $password2 )

@brief 		  	Vergleicht zwei Passwörter miteinander

@function		greift auf die vorhandene Methode ValidatePassword zu
				Zuerst werden beide Passwörter verschlüsselt dann verglichen

@param 		  	$password1 ein Passwort als string, $password2 ein Passwort als string

@return 		ein true wenn die Passwörter übereinstimmen, ein false wenn sie nicht übereinstimmen
**************************************************************************************************************** */

function PasswordComparsion( $password1, $password2 )
{
	$password1 = sha1($password1, 256);
	$password2 = sha1($password2, 256);
	
	if( $password1 === $password2)
	{
		// Passwort 1 und Passwort 2 stimmen überein
		$comparsion = true;
	}
	else
	{
		// Passwort 1 und Passwort 2 stimmen nicht überein
		$comparsion = false;
		$GLOBALS['errstr'] = ErrorByNumber( 113 );
	}

	return( $comparsion );
}

/* ***************************************************************************************************************
ValidateTitle( $db, $password )

@brief 		  	Liefert einen gültigen Titel zurück oder setzt die entsprechende Fehlermeldung

@function		Überprüft einen Titel anhand von Regulären Ausdrücken auf bestimmte Kriterien
				- Darf keine Zahlen enthalten
				- Darf keine Sonderzeichen enthalten außer '.' und Leerzeichen
				
				Wendet Mechanismen zur Verhinderung von MySQL Injection andere
				Setzt bei Fehler die entsprechende Fehlermeldung

@param 		  	$db eine Datenbankverbindung, $title ein Titel als string

@return 		ein gültiger Titel im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function ValidateTitle( $db, $title )
{
	if( empty($title) )
	{
		// Wenn kein Titel eingegeben wurde
		$title = NULL;
	}
	else if( !preg_match("/[a-zA-Z]+\s*?.?\s?/", $title) )
	{
		// Wenn die Überprüfung des Titels mit dem regulären Ausdruck fehlschlägt
		$GLOBALS['errstr'] = ErrorByNumber( 107 );
		$title = false;
	}
	else
	{
		// Schutz vor MySQL Injection
		$title = mysqli_real_escape_string( $db, $title );
	}
	
	return( $title );
}

/* ***************************************************************************************************************
ValidateFirstname( $db, $firstname )

@brief 		  	Liefert einen gültigen Vornamen zurück oder setzt die entsprechende Fehlermeldung

@function		Überprüft einen Vornamen anhand von Regulären Ausdrücken auf bestimmte Kriterien
				- Darf keine Zahlen enthalten
				- Darf keine Sonderzeichen enthalten außer Leerzeichen
				
				Wendet Mechanismen zur Verhinderung von MySQL Injection andere
				Setzt bei Fehler die entsprechende Fehlermeldung

@param 		  	$db eine Datenbankverbindung, $firstname ein Vorname als string

@return 		ein gültiger Vorname im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function ValidateFirstname( $db, $firstname )
{
	if(!preg_match("/[a-zA-Z]+\s?/", $firstname))
	{
		$GLOBALS['errstr'] = ErrorByNumber( 108 );
		$firstname = false;
	}
	else
	{
		// Schutz vor MySQL Injection
		$firstname = mysqli_real_escape_string($db, $firstname);
	}
	
	return ( $firstname );
}

/* ***************************************************************************************************************
ValidateLastname( $db, $lastname )

@brief 		  	Liefert einen gültigen Nachnamen zurück oder setzt die entsprechende Fehlermeldung

@function		Überprüft einen Nachnamen anhand von Regulären Ausdrücken auf bestimmte Kriterien
				- Darf keine Zahlen enthalten
				- Darf keine Sonderzeichen enthalten außer '-' und Leerzeichen
				
				Wendet Mechanismen zur Verhinderung von MySQL Injection andere
				Setzt bei Fehler die entsprechende Fehlermeldung

@param 		  	$db eine Datenbankverbindung, $lastname ein Nachname als string

@return 		ein gültiger Nachname im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function ValidateLastname( $db, $lastname )
{
	if( !preg_match("/[a-zA-Z]+\s?-?/", $lastname) )
	{
		$GLOBALS['errstr'] = ErrorByNumber( 109 );
		$lastname = false;
	}
	else
	{
		// Schutz vor MySQL Injection
		$lastname = mysqli_real_escape_string($db, $lastname);
	}
	
	return( $lastname );
}

/* ***************************************************************************************************************
ValidateTel( $db, $tel )

@brief 		  	Liefert eine gültige Telefonnumer zurück oder setzt die entsprechnende Fehlermeldung

@function		Überprüft eine Telefonnummer anhand von Regulären Ausdrücken auf bestimmte Kriterien
				- Darf nur Zahlen enthalten oder '-' und ' '
				- Die Telefonnummer muss im Format '+Ländervorwahl Ortsvorwahl Nummer - Durchwahl' eingegeben werden
				
				Wendet Mechanismen zur Verhinderung von MySQL Injection andere
				Setzt bei Fehler die entsprechende Fehlermeldung

@param 		  	$db eine Datenbankverbindung, $tel ein Telefonnummer als string

@return 		eine gültige Telefonnummer im Erfolgsfall, sonst ein false
**************************************************************************************************************** */

function ValidateTel( $db, $tel )
{
	if( !preg_match("/(\+\d{2}\s?\d{2}\s?)(\d{4,12})(( - )|( )|(-))?(\d{1,3})?/", $tel) )
	{
		$GLOBALS['errstr'] = ErrorByNumber( 110 );
		$tel = false;
	}
	else
	{
		// Schutz vor MySQL Injection
		$tel = mysqli_real_escape_string($db, $tel);
	}
	
	return( $tel );
}

/* ***************************************************************************************************************
RandomString()

@brief 		  	Erzeugt einen zufälligen verschlüsselten String

@function		Verschlüsselung über sha1

@return 		ein zufälliger verschlüsselter String
**************************************************************************************************************** */

function RandomString()
{
	if(function_exists('random_bytes'))
	{
		$bytes = random_bytes(16);
		$str = bin2hex($bytes);
	}
	else if(function_exists('openssl_random_pseudo_bytes'))
	{
		$bytes = openssl_random_pseudo_bytes(16);
		$str = bin2hex($bytes);
	}
	else if(function_exists('mcrypt_create_iv'))
	{
		$bytes = mcrypt_create_iv(16, MCRYPT_DEV_URANDOM);
		$str = bin2hex($bytes);
	}
	
	return $str;
}

/* *******************************************************************************
ErrorByNumber( $error )

@brief		erzeugt Fehlermeldungen in Abhängigkeit einer Fehlernummer

@function	erzeugt ein Div mit Fehlerstring

@param		$error Eine Fehlernummer als Ganzzahl
			
@return		ein String der das komplette Fehlerdiv enthält

******************************************************************************* */

function ErrorByNumber( $error )
{
	// Auswahl der Fehlermeldung
	switch( $error )
	{
	case 100: 	$desc 	= "<div id=\"error_100\">Bitte tragen Sie eine E-Mail Adresse und ein Passwort ein.</div>";
	break;
		
	case 101: 	$desc 	= "<div id=\"error_101\">Die E-Mail Adresse oder das Passwort war ungültig.</div>";
	break;
		
	case 102: 	$desc 	= "<div id=\"error_102\">Bitte füllen Sie das Feld E-Mail Adresse aus.</div>";
	break;
	
	case 103: 	$desc 	= "<div id=\"error_103\">Die E-Mail Adresse war ungültig.</div>";
	break;
	
	case 104: 	$desc 	= "<div id=\"error_104\">Diese E-Mail Adresse ist bereits vorhanden.</div>";
	break;
	
	case 105: 	$desc 	= "<div id=\"error_105\">Bitte füllen Sie das Feld Passwort aus.</div>";
	break;
	
	case 106: 	$desc 	= "<div id=\"error_106\">Das Passwort muss mindestens 8 Zeichen haben.</div>";
	break;
	
	case 107: 	$desc 	= "<div id=\"error_107\">Bitte geben Sie einen gültigen Titel ein.</div>";
	break;
	
	case 108: 	$desc 	= "<div id=\"error_108\">Bitte geben Sie einen gültigen Vornamen ein.</div>";
	break;
	
	case 109: 	$desc 	= "<div id=\"error_109\">Bitte geben Sie einen gültigen Nachnamen ein.</div>";
	break;
	
	case 110: 	$desc 	= "<div id=\"error_110\">Bitte geben Sie eine gültige Telefonnummer ein.</div>";
	break;
	
	case 111: 	$desc 	= "<div id=\"error_111\">Das Passwort stimmt nicht mit dem gespeichertem Passwort überein.</div>";
	break;
	
	case 112: 	$desc 	= "<div id=\"error_112\">Ihr neues Passwort war ungültig.</div>";
	break;
	
	case 113: 	$desc 	= "<div id=\"error_113\">Die Passwörter stimmen nicht überein.</div>";
	break;
	
	case 114: 	$desc 	= "<div id=\"error_114\">Die E-Mail Adresse ist nicht vorhanden.</div>";
	break;
	
	case 115: 	$desc 	= "<div id=\"error_115\">Sie haben bereits ein neues Passwort angefordert.</div>";
	break;
	
	case 116: 	$desc 	= "<div id=\"error_115\">Bitte zuerst einloggen.</div>";
	break;
	
	case 117: 	$desc 	= "<div id=\"error_115\">Passwort erfolgreich geändert.</div>";
	break;
	
	case 118: 	$desc 	= "<div id=\"error_115\">Passwort zurücksetzen fehlgeschlagen. Bitte versuchen Sie es erneut.</div>";
	break;
	
	case 119: 	$desc 	= "<div id=\"error_115\">Der Link zum Zurücksetzen des Passworts ist abgelaufen.</div>";
	break;
	
	case 200: 	$desc 	= "<div id=\"error_200\">Es ist ein unbekannter Fehler auf der Webseite aufgetreten. Bitte versuchen Sie es erneut.</div>";
	break;
	
	default:	$desc 	= "<div id=\"error_999\">Unbekannter Fehler aufgetreten.</div>";
	break;
	}
	
	return $desc;
}

/* ***************************************************************************************************************
CreateErrorLogfile( $InternalError, $db_errno, $db_error, $sql_query )

@brief 		  	Erstellt ein Logfile in welches Fehlermeldung und Informationen geschrieben werden

@function			Das Logfile wird folgendermaßen benannt: "YYYY-MM-DD_UserID_Error_Logfile.FileFormat"
  
					Fehler die vor dem Login passieren kriegen User_ID = 0
  
					In das Logfile werden folgende Daten geschrieben: 
					ERROR_NUMBER	Die interne Fehlernummer
					DB_ERRNO		Fehlernummer der MySQLi Datenbank (kein Wert = NO_ERROR)
					DB_ERROR		Fehlerbeschreibung der MySQLi Datenbank (kein Wert = NO_ERROR)
					SQL_QUERY		Ausgabe der fehlerhaften Datenbankabfrage (kein Wert = NONE)
					TIME			Zeitstempel des Request
					SESSION_IP		Die in der Session gespeicherte IP-Adresse
					REMOTE_ADDR		Die aktuelle IP-Adresse des Client
					USER_ID			Die User_ID des aktuell angemeldeten Users
					PAGE			Die Seite welche aufgerufen wurde
					BROWSER			Informationen zum vom Client verwendeten Betriebsystem und Browser
					
					Bedeutung der internen Fehlernummern:
					
					** 				Allgemeine Fehlermeldungen
					
					10				Aufbau der Datenbankverbindung fehlgeschlagen
					11				Laden der Funktionsbibliothek dbfunctions.inc fehlgeschlagen
					12				Laden der Funktionsbibliothek phpfunctions.inc fehlgeschlagen
					13				Laden der Funktionsbibliothek htmlfunctions.inc fehlgeschlagen
					14				Laden der Funktionsbibliothek debug.inc fehlgeschlagen
					
					** 				Fehlermeldungen bezogen auf >> dbfunctions.inc << 
					
					20				GetUserIDByLogin 					--> Fehlerhafte Datenbankabfrage
					21				GetUserGroupByUserID				--> Fehlerhafte Datenbankabfrage
					22				SetLoginTimeByUserID				--> Fehlerhafte Datenbankabfrage
					23				GetDataforWklist					--> Fehlerhafte Datenbankabfrage
					24				GetDataforEventlist					--> Fehlerhafte Datenbankabfrage
					25				GetDataforOverviewDevices			--> Fehlerhafte Datenbankabfrage
					26				GetDataforOverviewUsers				--> Fehlerhafte Datenbankabfrage
					27				GetDataforEditUsersByUserID			--> Fehlerhafte Datenbankabfrage
					28				CreateUser							--> Fehlerhafte Datenbankabfrage
					29				UpdateUser							--> Fehlerhafte Datenbankabfrage
					30				SetPasswordCodeAndTime				--> Fehlerhafte Datenbankabfrage
					31				GetPasswordCodeAndTime				--> Fehlerhafte Datenbankabfrage
					32				GetOberkategorie					--> Fehlerhafte Datenbankabfrage
					33				GetUnterkategorieByOberkategorie	--> Fehlerhafte Datenbankabfrage
					34				GetModelByUnterkategorie			--> Fehlerhafte Datenbankabfrage
					35				UpdateUserPassword					--> Fehlerhafte Datenbankabfrage
					36				DeleteUser							--> Fehlerhafte Datenbankabfrage
					37				CheckUserEmail						--> Fehlerhafte Datenbankabfrage
					38				UpdateUserPasswordByPasswordCode	--> Fehlerhafte Datenbankabfrage
					39				DeletePasswordCodeAndTime			--> Fehlerhafte Datenbankabfrage
					
					48				dbconnect							--> Verbindungsversuch ohne Username und Passwort
					49				dbconnect							--> Verbindungsversuch mit nicht existierendem User 'root'
					
					** 				Fehlermeldungen bezogen auf >> phpfunctions.inc << 
					
					50				CheckLogin							--> Fehlerhafte Datenbankabfrage
					
					
					
  
@param			$InternalError 	die Nummer des internen Fehlers
				$db_errno		Datenbankfehlernummer
				$db_error		Datenbankfehlerbeschreibung
				$sql_query		Die fehlerhafte SQL-Abfrage

@return 			
**************************************************************************************************************** */

function CreateErrorLogfile( $InternalError, $ErrorDescription='NO_ERROR_DESCRIPTION', $db_errno='NO_ERROR', $db_error='NO_ERROR', $sql_query='NONE' )
{	
	// Überprüfen ob Fehler vor oder nach der Anmeldung passiert ist
	if( !empty($_SESSION['user_ID']) )
	{
		// User_ID des eingeloggten User's in das Logfile schreiben
		$user_ID = $_SESSION['user_ID'];
	}
	else
	{
		// User_ID = 0 vergeben, da noch nicht eingeloggt
		$user_ID = 0;
	}

	// Dateinamen generieren
	$name = date(Y."-".m."-".d)."_".$user_ID."_Error_Logfile.txt";
	
	// Datei zum Schreiben öffnen oder erstellen
	$logfile = fopen("./logfiles/".$name, "a");
	
	// Inhalt des Logfiles erzeugen
	$content = 	"ERROR_NUMBER = ".$InternalError."\n".
				"ERROR_DESC = ".$ErrorDescription."\n".
				"DB_ERRNO = ".$db_errno."\n".
				"DB_ERROR = ".$db_error."\n".
				"SQL_QUERY = ".$sql_query."\n".
				"DATE = ".date('c')."\n".
				"TIME = ".$_SERVER['REQUEST_TIME']."\n".
				"SESSION_IP = ".$_SESSION['IP']."\n".
				"REMOTE_ADDR = ".$_SERVER['REMOTE_ADDR']."\n".
				"USER_ID = ".$_SESSION['user_ID']."\n".
				"REQUEST_METHOD = ".$_SERVER['REQUEST_METHOD']."\n".
				"PAGE = ".$_SERVER['PHP_SELF']."\n".
				"BROWSER = ".$_SERVER['HTTP_USER_AGENT']."\n".
				"\n";
	
	// Inhalt in die Datei schreiben
	fwrite( $logfile, $content);
	
	// Datei schließen
	fclose( $logfile);
}
?>