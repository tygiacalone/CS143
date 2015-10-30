<!DOCTYPE html>
<html>
    <title>CS143 Project 1C</title>
<body>
    <h1>CS 143 Database Search</h1>
    <h2>Add Movie/Actor Relation</h2>

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
	Actor Name:
	<select name="Actor">
	  <option value="">Select...</option>
	  <?php

		 $db_connection = mysql_connect( "localhost", "cs143", "");
		 mysql_select_db("CS143", $db_connection);
		 if (!$db_connection) {
    		$err = mysql_erro($db_connection);
    		echo "Connection failed to database: $err <br>";
    	 }

		 $sql = "SELECT CONCAT(first, \" \", last) FROM Actor";
		
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

	Role: <input type="text" name="role"> <br>


    <br> <br>
	<input type="submit">
    </form>
</body>
</html>





