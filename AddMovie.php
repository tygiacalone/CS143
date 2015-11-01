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
    <h2>Add Movie</h2>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">


    Movie Title: <input type="text" name="title"> <br>
	Release Year: <input type="text" name="year"> <br>
    Company: <input type="text" name="company"> <br>
    Genre: <input type="text" name="genre"> <br>
    MPA Rating: 
    <input type="radio" name="MPArating" value="G" checked>G
    <input type="radio" name="MPArating" value="PG" >PG
    <input type="radio" name="MPArating" value="PG13" >PG-13
    <input type="radio" name="MPArating" value="R" >R
    <input type="radio" name="MPArating" value="NC17" >NC-17


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

    if (!empty($_GET['year'])) {
    $year =  $_GET['year'];
    //echo $year;
    }
    else $year = 0;

    if (!empty($_GET['company'])) {
    $company = "\"".$_GET['company']."\"";
    //echo $company;
    }
    else $company = "\"\"";

    if (!empty($_GET['genre'])) {
    $genre = "\"".$_GET['genre']."\"";
    //echo $genre;
    }
    else $genre = "\"\"";

    if(!empty($_GET['MPArating'])){
    $MPArating = "\"" . $_GET['MPArating'] . "\"";
    //echo $MPArating;
    }
    else $MPArating = "\"\"";


        
    $result = mysql_query("SELECT id FROM MaxMovieID;", $db_connection);
    $maxID = mysql_fetch_row($result);
    $query = "Query is not yet set";


            
    $query = "INSERT INTO Movie VALUES (" . $maxID[0] . ", ". $title . ", ". $year. ", ". $MPArating . ", ". $company . ");";
 
    $added = mysql_query($query, $db_connection);

    echo '<br>'.$query .'<br>';
    //Update MaxPersonID:
    $updateMax = "UPDATE MaxMovieID SET id = id + 1 ;" ;
    mysql_query($updateMax, $db_connection);
    $result2 = mysql_query("SELECT id FROM MaxMovieID;", $db_connection);
    $newmaxID = mysql_fetch_row($result2);

    // echo '<br>' ."Movie successfully added!" .'<br>';


    mysql_close($db_connection);
?>
  </div>  
</body>
</html>
