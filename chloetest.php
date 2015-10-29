<!DOCTYPE html>
<html>
    <title>CS143 Project 1C</title>
<body>
    <h1>CS 143 Database Search</h1>
    First name:
	<br>
	<textarea rows="5" cols="50" type="text" name="firstname"></textarea>
	<br>
	Last name:
	<br>
	<textarea rows="5" cols="50" type="text" name="lastname"></textarea>
   	<br>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">
    Profession:  
    <input type="radio" name="type" value="Actor" checked>Actor
    <input type="radio" name="type" value="Director">Director
    <br>
   	Gender: 
    <input type="radio" name="sex" value="Male" checked>Male
    <input type="radio" name="sex" value="Female">Female
    <br>
	Birth Month:
	<input type="text" name="bmonth">

	<br>
	Birth Day:
	<input type="text" name="bday">
	<br>
	Birth Year:
	<input type="text" name="byear">
	<br>
	Death Month:
	<input type="text" name="dmonth">

	<br>
	Death Day:
	<input type="text" name="dday">
	<br>
	Death Year:
	<input type="text" name="dyear">
	<input type="submit">
    </form>
</body>
</html>

<?php
    ini_set('display_errors', 1);
   // error_reporting( E_ALL);

    $db_connection = mysql_connect( 'localhost', 'cs143', '');
 
    mysql_select_db('TEST', $db_connection);

    if (!$db_connection) {
	$err = mysql_erro($db_connection);
	echo "Connection failed to database: $err <br>";
    }

    if (!empty($_GET['firstname'])) {
	$firstname = "\"".$_GET['firstname']."\"";
	echo $firstname;
    }

    if (!empty($_GET['lastname'])) {
	$lastname = "\"". $_GET['lastname']."\"";
	echo $lastname;
    }

    if (!empty($_GET['type'])) {
    $type = $_GET['type'];
    echo $type;
    }
    
    if (!empty($_GET['gender'])) {
	$gender = "\"".$_GET['gender']."\"";
	echo $gender;
    }

    $gender = "\"" . $_GET['sex'] . "\"";
    print $gender;
    //$gender = test_input($_POST["sex"]);
    // if (!empty ($_GET['male'])){
    //     echo "ITS A BOY!" .'<br>';
    // }
    // Format for DOB: "YYYY-MM-DD"
    if (!empty($_GET['bday'])) {
	$bday = $_GET['bday'];
	echo $bday;
    }

    if (!empty($_GET['bmonth'])) {
	$bmonth = $_GET['bmonth'];
	echo $bmonth;
    }

    if (!empty($_GET['byear'])) {
	$byear = $_GET['byear'];
	echo $byear;
    }

    if (!empty($_GET['dday'])) {
    $dday = $_GET['dday'];
    echo $dday;
    }

    if (!empty($_GET['dmonth'])) {
    $dmonth = $_GET['dmonth'];
    echo $dmonth;
    }

    if (!empty($_GET['dyear'])) {
    $dyear = $_GET['dyear'];
    echo $dyear;
    }

    // if (strlen($query)) {
	    // Logic here...


        // Format DOB:
        if (isset($byear) and isset($bday) and isset($bmonth))
        {
            $DOBString = "\"" . $byear . "-" . $bmonth . "-" . $bday . "\"";
    	    echo '<br>' . $DOBString;
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
    	
        if($DOBString != NULL and $DODString != NULL)
    	   $query = "INSERT INTO Actor (id, last, first, sex, dob, dod) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender . ", " . $DOBString .", " . $DODString. ")";
        else if($DODString != NULL and $DOBString == NULL)
            $query = "INSERT INTO Actor (id, last, first, sex, dod) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender . ", "  . $DODString. ")";
        else if ($DOBString != NULL and $DODString == NULL)
            $query = "INSERT INTO Actor (id, last, first, sex, dob) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender . ", " . $DOBString . ")";
        else
            $query = "INSERT INTO Actor (id, last, first, sex) VALUES (" . $maxID[0] . ", " . $lastname . ", " . $firstname . ", " . $gender .  ")";
    	$added = mysql_query($query, $db_connection);

    		echo '<br>';
    	 	echo $query;
    	 	echo '<br>';
    	 	echo $maxID[0];
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







