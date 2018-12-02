<?php
/* 
  @file Funktionen für das Debugging
*/

define( "__INC_DEBUG__", true );


/* *******************************************************************************
Debug()

@brief		Aktiviert oder Deaktiviert den Debug-Modus

@function	aktiviert oder deaktiviert alle Fehlermeldungen, auch Notices
******************************************************************************* */

function Debug()
{
	// Debugeinstellungen

	if(TSE_DEBUG == true)
	{
		error_reporting(E_ALL); 				// Alle Fehler (auch Notices) aktivieren
	}
	else
	{
		error_reporting( 0 ); 					// Alle Fehler (auch Notices) deaktivieren
	}

}

/* *******************************************************************************
DebugPrintArr( $array, $name, $verbose )

@brief		Gibt den Inhalt eines Array's aus

@param		$array 		das Array welches augegeben werden sollen
			$name		der Arrayname zur Übersichtlichkeit bei Ausgabe mehrerer Array's
			$verbose	aktiviert Ausgabe umfangreicher Informationen

@function	Gibt den Inhalt eines Array's aus
			Optional können noch umfangreiche Informationen zum Inhalt ausgegeben werden
******************************************************************************* */

function DebugPrintArr( $array, $name='DEBUG_ARRAY', $verbose=false )
{ 
	// Wenn TSE_DEBUG definiert und true ist
	if ( defined("TSE_DEBUG") && TSE_DEBUG )
	{
		echo 	"\n<pre>".$name."-";
		if ( $verbose )
		{
			// Wenn umfangreiche Informationen zum Array Inhalt angezeigt werden sollen
			var_dump( $array );
		}
		else
		{
			// Normale Ausgabe vom Array Inhalt
			print_r( $array );
		}
		echo "\n</pre>";
	}
}

/* *******************************************************************************
DebugPrintVar()

@brief		Gibt den Inhalt einer Variabnle aus

@function	Gibt den Inhalt einer Variablen mit umfangreichen Informationen aus
******************************************************************************* */

function DebugPrintVar( $variable )
{
	// Wenn TSE_DEBUG definiert und true ist
	if( defined("TSE_DEBUG") && TSE_DEBUG )
	{
		echo "\n<pre>";
		
		var_dump( $variable );
		
		echo "\n</pre>";
	}
	
	
}

