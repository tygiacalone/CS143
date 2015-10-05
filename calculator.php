<!DOCTYPE HTML>

<html>
<body>
    <h1>My Calculator:</h1>
    <br>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">
	Input: <input type="text" name="expression">
	<input type="submit">
    </form>

    <?php
        

        function isGreaterOrEq($op1, $op2)
        {
            if($op1 == '+' || $op1 == '-'){
                if($op2 == '+' || $op2 == '-'){
                    return true;
                }
                else
                {
                    return false;
                }
            }
            
            if($op1 != '+' && $op1 != '-'){
                    return true;
            }

        }
        
        function performOp($op, $val1, $val2)
        {
            if($op == '+')
                return $val1 + $val2;
            
            if($op == '-')
                return $val2 - $val1;
            
            if($op == '*')
                return $val1 * $val2;
	    
	    else 
                return $val2 / $val1;
        }
        
        
        
        
        
        
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
	if (preg_match('/[^0-9]\.\[^0-9]/', $input) || preg_match('/[+\-\*\/\.][ ]*[+\*\/\.]/', $input) || preg_match('/[^0-9][.][0-9]/', $input) || preg_match('/[.]$/', $input) || preg_match('/^[.\+\*\/]/', $input) || preg_match('/[+\-\*\/\.]$/', $input) || preg_match('/[+\-\*\/][-][-]/', $input) ) {
	    echo "Invalid expression";
	    return;
	}

        $strlength = strlen($input);

	if ($strlength == 0){
	    return;        
	}
        
	echo "$input = ";
        
	$stackOp = array();
        $stackOpNum = 0;
        
        $stackVal = array();
        $stackValNum = 0;
        
        $isNeg = false;
        $hasDecYet = false;
        
        for($i = 0; $i < $strlength; $i++)
        {
            //Creating decimal numbers
            if($input[$i] == '.')
            {
                //Check that there was number before decimal, if not, return error
                if($stackValNum <= 0 || $hasDecYet){
                    echo "Invalid expression";
                    return;
                }
                
                $hasDecYet = true;
                
                //Pop last number off of stack
                $curVal1 = $stackVal[$stackValNum - 1];
                $stackValNum--;
                
                $j = 1;
                while(($i+$j) < $strlength && is_numeric($input[$i + $j]))
                {
                    $newVal1 = $input[$i + $j] * pow(0.1, $j);
                    if($curVal1 >= 0)
                        $curVal1 += $newVal1;
                    if($curVal1 < 0)
                        $curVal1 -= $newVal1;
                    $j++;
                }
                
                $i += ($j - 1);
                
                //Add new value to value stack
                $stackVal[$stackValNum] = $curVal1;
                $stackValNum++;
                echo "Adding $curVal1 to stack in decimal area <br/>";
                
            }
            
            elseif(is_numeric($input[$i]))
            {
                $hasDecYet = false;
                $curVal = $input[$i];
                
                $j = 1;
                while(($i+$j) < $strlength && is_numeric($input[$i + $j]))
                {
                    $curVal *= 10;
                    $curVal += $input[$i + $j];
                    $j++;
                }
                
                $i += ($j - 1);
                if($isNeg){
                    $curVal *= -1;
                    $isNeg = false;
                }

                $stackVal[$stackValNum] = $curVal;
                $stackValNum++;
                echo "Adding $curVal to stack in is_numeric part <br/>";
                
            }
            
            //If a negative number (- with operator in front, or first character is -)
            elseif(($i > 0 && $input[$i] == '-' && ($input[$i-1] == '+' || $input[$i-1] == '-' || $input[$i-1] == '*' || $input[$i-1] == '/')) || $i==0 && $input[$i] == '-')
            {
                $isNeg = true;
            }
            
            
            elseif($input[$i] == '+' || $input[$i] == '-' || $input[$i] == '*' || $input[$i] == '/')
            {
                //While the operator stack is not empty, and the top thing on the
                //operator stack has the same or greater precedence as thisOp
                
                while($stackOpNum > 0 && isGreaterOrEq($stackOp[$stackOpNum - 1],$input[$i]))
                {
                    //Pop the operator from the operator stack.
                    $newOp = $stackOp[$stackOpNum - 1];
                    $stackOpNum--;
                    
                    //Pop the value stack twice, getting two operands.
                    $val1 = $stackVal[$stackValNum - 1];
                    $stackValNum--;
                    echo "Popping $val1 off of stack <br/>";
                    
                    $val2 = $stackVal[$stackValNum - 1];
                    $stackValNum--;
                    echo "Popping $val2 off of stack <br/>";
                    
                    //Apply the operator to the operands, in the correct order.
                    $newVal = performOp($newOp, $val1, $val2);
                    
                    //Push the result onto the value stack
                    $stackVal[$stackValNum] = $newVal;
                    $stackValNum++;
                    echo "$val2". "$newOp" . "$val1" . " IS $newVal <br/>";
                    
                }
                //Push thisOp onto the operator stack.
                $stackOp[$stackOpNum] = $input[$i];
                $stackOpNum++;
            }
        }
        
        for($k = 0; $k < sizeof($stackVal); $k++)
            echo "Remaining values in stackVal: $stackVal[$k] <br/>";
        
        //While the operator stack is not empty
        while($stackOpNum > 0)
        {
            //Pop the operator from the operator stack.
            $newOp = $stackOp[$stackOpNum - 1];
            $stackOpNum--;
            
            //Pop the value stack twice, getting two operands.
            $val1 = $stackVal[$stackValNum - 1];
            $stackValNum--;
            echo "Popping $val1 off of stack at end <br/>";
            
            $val2 = $stackVal[$stackValNum - 1];
            $stackValNum--;
            echo "Popping $val2 off of stack at end <br/>";
            
            //Apply the operator to the operands, in the correct order.
            $newVal = performOp($newOp, $val1, $val2);
            
            //Push the result onto the value stack
            $stackVal[$stackValNum] = $newVal;
            $stackValNum++;
            echo "$val2". "$newOp" . "$val1" . " is $newVal <br/>";
        }
        if($stackOpNum != 0) //This should never happen, if calculator implemented properly.
        {
            echo "ERROR: The operator stack does not have exactly zero values left. <br/>";
        }
        if($stackValNum != 1) //This should never happen, if calculator implemented properly.
        {
            echo "ERROR: The value stack does not have exactly one value left. <br/>";
        }
        
        $endVal = $stackVal[0];
        echo "Result is: $endVal <br/>";
	    $result = $input;
	echo "$input = $endVal";
	}
    ?>
</body>
</html>
