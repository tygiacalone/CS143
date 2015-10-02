<!DOCTYPE HTML>

<html>
<body>
    <h1>My Calculator:</h1>
    <br>

    <form method="get" action="calculator.php">
	Input: <input type="text" name="expression">
	<input type="submit">
    </form>

    <?php
	ini_set( 'display_errors', 1);
	error_reporting( E_ALL);
	if ($_SERVER["REQUEST_METHOD"] == "GET") {
	    $input = $_GET['expression'];

	    //$output = 
	    //echo $input;

	//Only operators, decimal points, or numbers are valid.
    	if (!preg_match('/^[0-9\+\-\/\*\.\ ]*$/', $input)){
    	    echo "Invalid expression";
	    return;
    	}

	// Get rid of malformed floats, do not allow repeated operators or operators
	// Beginning or ending an expression.
	if (preg_match('/[^0-9]\.\[^0-9]/', $input) || preg_match('/[+\-\*\/\.][ ]*[+\*\/\.]/', $input) || preg_match('/[^0-9][.][0-9]/', $input) || preg_match('/[.]$/', $input) || preg_match('/^[.\+\*\/]/', $input) || preg_match('/[+\-\*\/\.]$/', $input) ) {
	    echo "Invalid expression";
	    return;
	}

	   echo "$input = ";

	    // Logic here on computing the expression

	    $result = $input; 
	}
    ?>
</body>
</html>
