<!DOCTYPE html>
<html>
    <title>CS143 Project 1C</title>
<body>
    <h1>CS 143 Database Search</h1>
    <h2>Add Movie/Actor Relation</h2>

    <form method="get" action="<?php echo $_SERVER["PHP_SELF"];?>


    Movie Title: <input type="text" name="title"> <br>
    Release Year: <input type="text" name="year"> <br>
    Company: <input type="text" name="company"> <br>
    Genre: <input type="text" name="genre"> <br>
    MPA Rating: 
    <input type="radio" name="MPArating" value="G" checked>G
    <input type="radio" name="MPArating" value="PG" >PG
    <input type="radio" name="MPArating" value="PG13" >PG-13
    <input type="radio" name="MPArating" value="R" >R
    <input type="radio" name="MPArating" value="NC17" >NC-17

    <p>
    What is your Gender?
    <select name="formGender">
      <option value="">Select...</option>
      <option value="M">Male</option>
      <option value="F">Female</option>

    </select>
    </p>





    <br> <br>
    <input type="submit">
    </form>
</body>
</html>





