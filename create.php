<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<?php
  session_start();

  define( "MYDEBUG", true);
  error_reporting( E_ALL ); // Alle Fehler (auch notices) aktivieren
  $errstr = "";				// Leere Fehlermeldung initialisieren
 
  if( MYDEBUG )
  {
    echo "<pre>";
     echo"Post ";
    print_r($_POST);
    echo"Session ";
    print_r($_SESSION);
    print_r($_REQUEST);
    echo "</pre>"; 
  }
  // inlcude('debug.php');
  
  //Verbindung zur Datenbank erstellen
  $dbcon = new mysqli("localhost","tsedummy","tsedummy","tse_db");
  // include('dbconnection.php');
  
  if(MYDEBUG)
  {
    //erfolgreich?
    if($dbcon->connect_error)
    {
      die ("Fehler bei der Verbindung!");
    }
    else
    {
      echo "Verbindung erfolgreich!";
    }
    
  }       
  
?>

<html>
  <head>
    
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <title>TSE - LV - Anlegen</title>

    <link rel="stylesheet" href="./css/style.css" type="text/css" />
    <link rel="shortcut icon" href="./pictures/TSElogo.ico" type="image/x-icon" />
    
  </head>

  <body>

    <div id="wrapper">
      
      <div id="header">
      
      
      <h1>Lagerverwaltung</h1>
      
    </div>
    
    <div id ="navigation">
      <ul>
        <li><a href="index.html">Login</a></li>
        <li><a href="user.html">&Uuml;bersicht</a></li>
        <li><a href="artikel.html">Artikelstamm</a></li>
      </ul>
    </div>

    <div id="content">
      <form action="create.php" method="post">
        <label>Oberkategorie </label>
        <select name="oberkategorie">
          <option value="1">Werkzeug</option>
          <option value="2">Pr&uuml;fmittel</option>
        </select>
        <button type="button" name="submit" value="bestätigen">Bestätigen</button>
      </form>
      
      <?php
          if(isset($_REQUEST['submit']))
          {
            echo "Yarak";
          }
           
           /*if(isset ($_REQUEST['submit'])) 
           {
             if(isset ($_POST['oberkategorie'])) 
             {
              $_SESSION ['oberkategorie']= $_POST['oberkategorie'];
             }
             //SQL-Abfrage zusammenbasteln
             $sqlString = "SELECT underclass_description FROM underclass WHERE upperclass_ID = '".$_SESSION['oberkategorie']."';";
             //" WHERE login ='".$_POST['login']." 'AND passwd=MD5('".$_POST['passwd']."');";
   
              //Abfrage abschicken und Ergebnis entgegen nehmen
              $dberg = $dbcon->query($sqlString);
            
              //Abfrage fehlerhaft?
              if($dberg === false)
              {
                  echo "<div>Fehlerhafte Abfrage: ".$sqlString."<br/> (".$dbcon->erno.")".$dbcon->error."</div>\n";
                  die("und tschüß");
              }
              //erstelle eine neue Dropdwonliste 
              echo "<p>";
              echo"<form action=\"create.php\" method=\"post\">";
              echo "<label>Unterkategorie </label>";
              echo "<select name='unterkategorie'>";
              $index=1;
              while($row = mysqli_fetch_array($dberg))
              {
                //Hilfsvariable für Index
                echo "<option value='".$index."'>".$row['underclass_description']." </option>";
                $index++;
              }
              echo"</select>";
              echo"<button type=\"submit\" name=\"submit\" value=\"bestätigen\">Bestätigen</button>";
              echo"</form>";
              echo "</p>";
           }*/
      ?>     
    </div>

    <div id="footer">
      <ul>
      <li><a href="kontakt.html">Impressum</a></li>
      <li><a href="impressum.html">Kontakt</a></li>
      </ul>
    </div>

    </div>

  </body>
</html>