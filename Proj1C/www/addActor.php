<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

echo <<<_END
<h1>Add a New Actor:</h1><br>
<form method="get" action="addActor.php">
First Name: <input type="text" name="firstname"><br><br>
Last Name: <input type="text" name="lastname"><br><br>
Sex: <input type="radio" name="sex" value="Male" checked>Male
<input type="radio" name="sex" value="Female">Female<br><br>
Date of Birth: <input type="text" name="dob"><br><br>
Date of Die: <input type="text" name="dod"> (leave blank if still alive)<br><br>
<br>
<input type="submit" value="Add Actor">
</form><br><hr>
_END;

  if (isset($_GET['firstname']) && 
    isset($_GET['lastname']) && 
    isset($_GET['dob']))
  {
    $firstname = $_GET['firstname'];
    $lastname = $_GET['lastname'];
    $sex = $_GET['sex'];
    $dob = $_GET['dob'];
    
    if(isset($_GET['dod'])){
      $dod = $_GET['dod'];
      if($dod == "") $dod = "null";
    }
    else
      $dod = "null";



    $maxPersonID_result = mysql_query("select * from MaxPersonID", $db_connection);
    if(!$maxPersonID_result) die("Database access failed:" . mysql_error());

    $maxPersonID = mysql_fetch_row($maxPersonID_result);

    $newPersonID = $maxPersonID[0] + 1;

    if(!mysql_query("update MaxPersonID set id='$newPersonID'", $db_connection))
      die("Database update failed:" . mysql_error());

    $dod = $_GET['dod'];
    if(!isset($_GET['dod']) || $dod == "")
      $insert_result = mysql_query("insert into Actor values" . 
                  "('$newPersonID', '$lastname', '$firstname', '$sex', '$dob', null)", $db_connection);
    else
      $insert_result = mysql_query("insert into Actor values" . 
                  "('$newPersonID', '$lastname', '$firstname', '$sex', '$dob', '$dod')", $db_connection);

    if(!$insert_result)
      die("Database insert failed:" . mysql_error());

    echo "<h3>-- New Actor Added --</h3><br>";
    echo "Actor ID: $newPersonID<br>";
    echo "Name: $firstname $lastname<br>";
    echo "Sex: $sex<br>";
    echo "Date of Birth: $dob<br>";
  }


  mysql_close($db_connection);
?>