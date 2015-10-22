<!DOCTYPE html>
<html>
    <title>CS143 Project 1B</title>
<body>
    <h1>CS 143 Database Search</h1>
    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">
	Enter your query in the box:
	<br>
	<textarea rows="5" cols="50" type="text" name="query"></textarea>
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

    if (!empty($_GET['query'])) {
	$query = $_GET['query'];
	echo 'Your query was: ' . $query;
	echo '<br>';

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
			echo '<td>' . $row[$val] . '</td>'; 
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
