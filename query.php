<?php


?>


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
    error_reporting( E_ALL);

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
	$query = $_GET['query'];
	echo $query;
    }

    if (strlen($query)) {
	// Logic here...
    }
    else {
	echo 'Your query is empty!';
    }

?>
