<!DOCTYPE html>
<html>
    <title>CS143 Project 1C</title>
<body>
  
  <nav style="display: inline block; max-width: 30%; float: left;">
      <ul><a href="/~cs143/AddMovie.php">Add Movie</a></ul>
      <ul><a href="/~cs143/AddActor.php">Add Actor or Director</a></ul>
      <ul><a href="/~cs143/AddMovieActor.php">Add Movie/Actor</a></ul>
      <ul><a href="/~cs143/AddMovieDirector.php">Add Movie/Director</a></ul>
      <ul><a href="/~cs143/SearchActorMovie.php">Search Actor/Movie</a></ul>
      <ul><a href="/~cs143/ShowActor.php">Show Actor</a></ul>
      <ul><a href="/~cs143/ShowMovie.php">Show Movie</a></ul>
  </nav>

<div style="display: inline block; padding-left: 30%;">
    <h1>CS 143 Database Search</h1>
    <h2>Show Actor</h2>

<?php
    ini_set('display_errors', 1);
   // error_reporting( E_ALL);

    $db_connection = mysql_connect( 'localhost', 'cs143', '');
    mysql_select_db('CS143', $db_connection);

    if (!$db_connection) {
	$err = mysql_erro($db_connection);
	echo "Connection failed to database: $err <br>";
    }

    if (!empty($_GET['aid'])) {
	$aid = $_GET['aid'];
    $query = "SELECT CONCAT(first, \" \" ,last) as name, dob as birthdate, sex, dod as 'date of death' FROM Actor WHERE id = " . $aid . ";"; 
      
    $moviequery = "select m.role as role, Movie.title as movie, m.mid as link from (SELECT mid, role from MovieActor where aid = " . $aid . ") as m JOIN Movie ON m.mid = Movie.id" . ";";
      
	echo 'Your query was: ' . $query;
	echo '<br>';

    echo '<h3>Actor Info:</h3>';
    if (strlen($query)) {
      // Logic here...
      $result = mysql_query($query, $db_connection);
      $error = mysql_error(); 
      $row = @mysql_fetch_assoc($result);

      if ($error) {
      echo $error;
      }
      else if(!$row){
      echo "Result is empty" . '<br>';
      }
      else {
      $num_rows = mysql_num_fields($result);
      echo '<table border="1" style="border-collapse: collapse">';

      // Print column names
      foreach( array_keys($row) as $val) {
          echo '<td style="background-color: lightblue">' . $val . '</td>';	
      } 
      echo '</tr>';

      // Print values across each row	
      while($row) {
          echo '<tr>';
          foreach( array_keys($row) as $val) {
            echo '<td>';
            $movie = 'movie';
            
            if ($val == "date of death"){
              if (!strlen($row[$val])) //If length zero
                echo "Still alive";
              else
                echo $row[$val];
              echo "<br>";
            } 
            else {
              echo $row[$val]; 
            }
            
            echo '</td>';
          }
          echo '</tr>';
          $row = mysql_fetch_assoc($result);
      }
      echo '</table>';
      // Print out the data in a table...
      } 
   }
      
    echo '<br>';
      
    echo '<h3>Seen In:</h3>';
    if (strlen($moviequery)) {
      // Logic here...
      $result = mysql_query($moviequery, $db_connection);
      $error = mysql_error(); 
      $row = @mysql_fetch_assoc($result);

      if ($error) {
      echo $error;
      }
      else if(!$row){
      echo "Result is empty" . '<br>';
      }
      else {
      $num_rows = mysql_num_fields($result);
      echo '<table border="1" style="border-collapse: collapse">';

      // Print column names
      foreach( array_keys($row) as $val) {
          echo '<td style="background-color: lightblue">' . $val . '</td>';	
      } 
      echo '</tr>';

      // Print values across each row	
      while($row) {
          echo '<tr>';
        
          foreach( array_keys($row) as $val) {
            echo '<td>';
            $movie = 'movie';
            
            if ($val == "link"){
              print "<a href=\"ShowMovie.php?mid=$row[$val]\">$row[$movie]</a><br>";
              echo "<br>";
            } 
            else {
              echo $row[$val]; 
            }
            
            echo '</td>';
          }
          echo '</tr>';
          $row = mysql_fetch_assoc($result);
      }
      echo '</table>';
      // Print out the data in a table...
      } 
   }
  else {
      echo 'Your query is empty!';
  }
      
    }
    mysql_close($db_connection);
?>
  
    </div>
</body>
</html>

