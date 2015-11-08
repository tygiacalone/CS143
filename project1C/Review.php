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
    <h2>Add Actor/Director</h2>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">
    Movie id: <input type="text" name="mid" value="<?php echo $_GET['mid'];?>">
    <br>
    Your Name: <input type="text" name="name"> 
    <br>
    Review: 
    <br>
    <textarea rows="10" cols="80" type="text" name="review"> </textarea>
    <br>
    Score:  
    <input type="radio" name="score" value="1" checked>1 - Terrible
    <input type="radio" name="score" value="2">2 - Poor
    <input type="radio" name="score" value="3">3 - Average
    <input type="radio" name="score" value="4">4 - Very Good
    <input type="radio" name="score" value="5">5 - Excellent
    <br>
	
	<input type="submit">
    </form>

<?php
    //ini_set('display_errors', 1);
   // error_reporting( E_ALL);

    $db_connection = mysql_connect( 'localhost', 'cs143', '');
 
    mysql_select_db('CS143', $db_connection);

    if (!$db_connection) {
	$err = mysql_erro($db_connection);
	echo "Connection failed to database: $err <br>";
    }

    if (!empty($_GET['mid'])) {
	 $mid = $_GET['mid'];
	 //echo $mid;
      
    }
    else $mid = "\"\"";


    if (!empty($_GET['name'])) {
	$name = "\"". $_GET['name']."\"";
	//echo $name;
    }
    else $name = "\"\"";

    if (!empty($_GET['review'])) {
    $review = "\"". $_GET['review']."\"";
    //echo $review;
    }

    if (!empty($_GET['score'])) {
	$score = $_GET['score'];
	//echo $score;
    }

    $query = "INSERT INTO Review VALUES (" . $name . ", NOW(), " . $mid . ", ". $score . ", " . $review . ");";
 
    $result = mysql_query($query, $db_connection);
    $error = mysql_error(); 
    $row = @mysql_fetch_assoc($result);

    if ($error) {
      //echo $error;
    }
    else {
      echo "Review successfully posted!";
    }

    mysql_close($db_connection);
?>


  </div>
</body>
</html>




