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
    <h2>Add Movie/Director Relation</h2>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">
Movie Title:
	<select name="title">
	  <option value="">Select...</option>
	  <?php

		 $db_connection = mysql_connect( "localhost", "cs143", "");
		 mysql_select_db("CS143", $db_connection);
		 if (!$db_connection) {
    		$err = mysql_erro($db_connection);
    		echo "Connection failed to database: $err <br>";
    	 }

		$sql = "SELECT title FROM Movie;";
		
		$result = mysql_query($sql, $db_connection);

		while($row = mysql_fetch_row($result)) 
		{	
			echo "<option value=\"" . $row[0] ."\">" . $row[0] ."</option>";
		}

		mysql_free_result($result);
		mysql_close($db_connection);
	  ?>
	</select>
	</p>

	<p>
	Director Name:
	<select name="Director">
	  <option value="">Select...</option>
	  <?php

		 $db_connection = mysql_connect( "localhost", "cs143", "");
		 mysql_select_db("CS143", $db_connection);
		 if (!$db_connection) {
    		$err = mysql_erro($db_connection);
    		echo "Connection failed to database: $err <br>";
    	 }

		 $sql = "SELECT CONCAT(first, \" \", last ,\"  (\", dob, \")\" ) FROM Director";
		
		 $result = mysql_query($sql, $db_connection);

		while($row = mysql_fetch_row($result)) 
		{	
			echo "<option value=\"" . $row[0] ."\">" . $row[0] ."</option>";
		}

		mysql_free_result($result);
		mysql_close($db_connection);
	  ?>
	</select>
	</p>
	<br> <br>
	<input type="submit">
    </form>

<?php
    ini_set('display_errors', 1);
   //error_reporting( E_ALL);

    $db_connection = mysql_connect( 'localhost', 'cs143', '');
 
    mysql_select_db('CS143', $db_connection);

    if (!$db_connection) {
    $err = mysql_erro($db_connection);
    echo "Connection failed to database: $err <br>";
    }

    if (!empty($_GET['title'])) {
    $title = "\"".$_GET['title']."\"";
    //echo $title;
    }
    else $title = "\"\"";


    if (!empty($_GET['Director'])) {
    $director = "\"".$_GET['Director']."\"";
    //echo $director;
    }
    else $director = "\"\"";
    
    $midquery = "SELECT id FROM Movie WHERE title = " . $title.";"; 
    //echo '<br>' .$midquery. '<br>'  ;
    $didquery = "SELECT id FROM Director WHERE CONCAT(first, \" \", last ,\"  (\", dob, \")\" ) = " . $director . ";";       
    //echo '<br>' .$didquery. '<br>'  ;


    $midresult = mysql_query($midquery, $db_connection);
    $mid = mysql_fetch_row($midresult);
    //echo '<br>' . $mid[0] . '<br>'  ;

    $didresult = mysql_query($didquery, $db_connection);
    $did = mysql_fetch_row($didresult);
    //echo '<br>' . $did[0] . '<br>'  ;


    $query = "INSERT INTO MovieDirector VALUES (" . $mid[0] . ", " . $did[0] .");";
    //echo '<br>'.$query .'<br>';
    echo '<br>'."Added Movie/Director relation!" .'<br>';
  	
  	$added = mysql_query($query, $db_connection);

    mysql_close($db_connection);
?>

  </div>
</body>
</html>













