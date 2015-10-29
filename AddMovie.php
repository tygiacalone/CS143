<!DOCTYPE html>
<html>
    <title>CS143 Project 1C</title>
<body>
    <h1>CS 143 Database Search</h1>
    <h2>Add Movie</h2>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>">


    Movie Title: <input type="text" name="title"> <br>
	Release Year: <input type="text" name="year"> <br>
    Rating (1-10): <input type="text" name="rating"> <br>
    Company: <input type="text" name="company"> <br>
    Genre: <input type="text" name="genre"> <br>
    MPA Rating: 
    <input type="radio" name="MPArating" value="G" checked>G
    <input type="radio" name="MPArating" value="PG" checked>PG
    <input type="radio" name="MPArating" value="PG13" checked>PG-13
    <input type="radio" name="MPArating" value="R" checked>R
    <input type="radio" name="MPArating" value="NC17" checked>NC-17


    <br> <br>
	<input type="submit">
    </form>
</body>
</html>