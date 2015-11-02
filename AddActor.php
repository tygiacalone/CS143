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
    First name: <input type="text" name="firstname"> <br>
    Last name: <input type="text" name="lastname"> <br>
    Profession:  
    <input type="radio" name="type" value="Actor">Actor
    <input type="radio" name="type" value="Director">Director
    <br>
   	Gender: 
    <input type="radio" name="sex" value="Male">Male
    <input type="radio" name="sex" value="Female">Female
    <br>
	Birth Month:
	

	<br>
    Date of Birth: (MM DD YYYY)<br>
	<input type="text" name="bday">
	<input type="text" name="bmonth">
	<input type="text" name="byear">
	<br>
    Date of Death: (MM DD YYYY) <br>
	<input type="text" name="dday">
	<input type="text" name="dmonth">
    <input type="text" name="dyear">
	<br>
	
	<input type="submit">
    </form>

<?php
    ini_set('display_errors', 1);
   // error_reporting( E_ALL);

    $db_connection = mysql_connect( 'localhost', 'cs143', '');
 
    mysql_select_db('CS143', $db_connection);

    if (!$db_connection) {
	$err = mysql_erro($db_connection);
	echo "Connection failed to database: $err <br>";
    }

    if (!empty($_GET['firstname'])) {
	$firstname = "\"".$_GET['firstname']."\"";
	//echo $firstname;
    }
    else $firstname = "\"\"";


    if (!empty($_GET['lastname'])) {
	$lastname = "\"". $_GET['lastname']."\"";
	//echo $lastname;
    }
    else $lastname = "\"\"";

    if (!empty($_GET['type'])) {
    $type = $_GET['type'];
    echo $type;
    }

    if (!empty($_GET['gender'])) {
	$gender = "\"".$_GET['gender']."\"";
	//echo $gender;
    }

    $gender = "\"" . $_GET['sex'] . "\"";
    //print $gender;

    // Format for DOB: "YYYY-MM-DD"
    if (!empty($_GET['bday'])) {
	$bday = $_GET['bday'];
	//echo $bday;
    }

    if (!empty($_GET['bmonth'])) {
	$bmonth = $_GET['bmonth'];
	//echo $bmonth;
    }

    if (!empty($_GET['byear'])) {
	$byear = $_GET['byear'];
	//echo $byear;
    }

    if (!empty($_GET['dday'])) {
    $dday = $_GET['dday'];
    //echo $dday;
    }

    if (!empty($_GET['dmonth'])) {
    $dmonth = $_GET['dmonth'];
    //echo $dmonth;
    }

    if (!empty($_GET['dyear'])) {
    $dyear = $_GET['dyear'];
    //echo $dyear;
    }

    // if (strlen($query)) {
	    // Logic here...


        // Format DOB:
        if (isset($byear) and isset($bday) and isset($bmonth))
        {
            $DOBString = "\"" . $byear . "-" . $bmonth . "-" . $bday . "\"";
    	    //echo '<br>' . $DOBString;
        }
        else
            $DOBString = NULL;
        // Format DOD:
        if (isset($dyear) and isset($dday) and isset($dmonth))
        {
            $DODString = "\"" . $dyear . "-" . $dmonth . "-" . $dday . "\"";
            echo '<br>' . $DODString;
        }   
        else
            $DODString = NULL;

    	$result = mysql_query("SELECT id FROM MaxPersonID;", $db_connection);
    	$maxID = mysql_fetch_row($result);
    	$query = "Query is not yet set";
        if($type == "Actor"){
            if($DOBString != NULL and $DODString != NULL)
        	   $query = "INSERT INTO Actor (id, last, first, sex, dob, dod) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender . ", " . $DOBString .", " . $DODString. ")";
            else if($DODString != NULL and $DOBString == NULL)
                $query = "INSERT INTO Actor (id, last, first, sex, dod) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender . ", "  . $DODString. ")";
            else if ($DOBString != NULL and $DODString == NULL)
                $query = "INSERT INTO Actor (id, last, first, sex, dob) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender . ", " . $DOBString . ")";
            else
                $query = "INSERT INTO Actor (id, last, first, sex) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender .  ")";
        }
        else if($type == "Director")
        {   
            if($DOBString != NULL and $DODString != NULL)
               $query = "INSERT INTO Director (id, last, first, dob, dod) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $DOBString .", " . $DODString. ")";
            else if($DODString != NULL and $DOBString == NULL)
                $query = "INSERT INTO Director (id, last, first, dod) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $DODString. ")";
            else if ($DOBString != NULL and $DODString == NULL)
                $query = "INSERT INTO Director (id, last, first, dob) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $DOBString . ")";
            else
                $query = "INSERT INTO Director (id, last, first) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ")";
    
        }
    	$added = mysql_query($query, $db_connection);

    		echo '<br>';
    	 	echo $query;
    	 	echo '<br>';


    	 //Update MaxPersonID:
    	 $updateMax = "UPDATE MaxPersonID SET id = id + 1 ;" ;
    	 mysql_query($updateMax, $db_connection);
		$result2 = mysql_query("SELECT id FROM MaxPersonID;", $db_connection);
    	$newmaxID = mysql_fetch_row($result2);


    	echo "New maxID: " .  $newmaxID[0];

    	 //$result = mysql_query($query, $db_connection);
    	 

    mysql_close($db_connection);
?>


  </div>
</body>
</html>




