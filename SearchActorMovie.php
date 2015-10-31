


<!DOCTYPE html>
<html>
    <title>CS143 Project 1C</title>
<body>
    <h1>CS 143 Database Search</h1>
    <h2>Search Actor/Movie</h2>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">
    Search: <input type="text" name="search"> <br>
	<input type="submit">
    </form>
</body>
</html>

<?php
    ini_set('display_errors', 1);
   //error_reporting( E_ALL);

    $db_connection = mysql_connect( 'localhost', 'cs143', '');
 
    mysql_select_db('CS143', $db_connection);

    if (!$db_connection) {
    $err = mysql_erro($db_connection);
    echo "Connection failed to database: $err <br>";
    }

    if (!empty($_GET['search'])) {
    $search = "\"%".$_GET['search']."%\"";
   // echo $search;
    }
    else $search = "\"\"";

    
    $midquery = "SELECT id FROM Movie WHERE title like " . $search.";"; 
    //echo '<br>' .$midquery. '<br>'  ;

    $aidquery = "SELECT id FROM Actor WHERE CONCAT(first, \" \" ,last) like " . $search . " OR  first like " . $search ." OR last like " . $search . ";";       
   // echo '<br>' .$aidquery. '<br>'  ;

   
     $midresult = mysql_query($midquery, $db_connection);
     //$mid = mysql_fetch_array($midresult);
     //echo '<br>' . $mid[0] . '<br>'  ;
     echo '<br>'. "Matching Movie Titles:" . '<br>';
     while ($row = mysql_fetch_array($midresult))
     {
        $movId = $row[0];
        //echo '<br>'. $movId . '<br>';
        $movTitleQuery = "SELECT title from Movie WHERE id = " . $movId.";";
        //echo '<br>'. $movTitleQuery . '<br>';
        //Print movie's titles:
        $titleResult = mysql_query($movTitleQuery, $db_connection);
        $title = mysql_fetch_row($titleResult);
        echo "--  " .$title[0] .'<br>';
     }

    $aidresult = mysql_query($aidquery, $db_connection);
     //$mid = mysql_fetch_array($midresult);
     //echo '<br>' . $mid[0] . '<br>'  ;
     echo '<br>'. "Matching Actor Names:" . '<br>';
     while ($row = mysql_fetch_array($aidresult))
     {
        $actId = $row[0];
        //echo '<br>'. $movId . '<br>';
        $actorTitleQuery = "SELECT CONCAT(first, \" \", last) from Actor WHERE id = " . $actId.";";
        //echo '<br>'. $movTitleQuery . '<br>';
        //Print movie's titles:
        $nameResult = mysql_query($actorTitleQuery, $db_connection);
        $name = mysql_fetch_row($nameResult);
        echo "--  " . $name[0] .'<br>';
     }

    // $aidresult = mysql_query($aidquery, $db_connection);
    // $aid = mysql_fetch_row($aidresult);
    // //echo '<br>' . $aid[0] . '<br>'  ;

    // $query = "INSERT INTO MovieActor VALUES (" . $mid[0] . ", " . $aid[0] . ", " . $role .");";
    // echo '<br>'.$query .'<br>';
    
    // $added = mysql_query($query, $db_connection);

    mysql_close($db_connection);
?>
