<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

echo <<<_END
<h1>Add a New Movie:</h1><br>
<form method="get" action="addMovie.php">
Title: <input type="text" name="title"><br><br>
Company: <input type="text" name="company"><br><br>
Year: <input type="text" name="year"><br><br>
MPAA Rating: 
<select name="rating">
<option value="G">G</option>
<option value="PG">PG</option>
<option value="PG-13">PG-13</option>
<option value="R">R</option>
<option value="NC-17">NC-17</option>
</select><br><br>
Genre: 
<input type="checkbox" name="genre[]" value="Comedy"> Comedy
<input type="checkbox" name="genre[]" value="Action">Action
<input type="checkbox" name="genre[]" value="Adult">Adult
<input type="checkbox" name="genre[]" value="Adventure">Adventure
<input type="checkbox" name="genre[]" value="Animation">Animation
<input type="checkbox" name="genre[]" value="Crime">Crime
<input type="checkbox" name="genre[]" value="Documentary">Documentary
<input type="checkbox" name="genre[]" value="Drama">Drama
<input type="checkbox" name="genre[]" value="Family">Family
<input type="checkbox" name="genre[]" value="Fantasy">Fantasy
<input type="checkbox" name="genre[]" value="Horror">Horror
<input type="checkbox" name="genre[]" value="Musical">Musical
<input type="checkbox" name="genre[]" value="Mystery">Mystery
<input type="checkbox" name="genre[]" value="Romance">Romance
<input type="checkbox" name="genre[]" value="Sci-Fi">Sci-Fi
<input type="checkbox" name="genre[]" value="Short">Short
<input type="checkbox" name="genre[]" value="Thriller">Thriller
<input type="checkbox" name="genre[]" value="War">War
<input type="checkbox" name="genre[]" value="Western">Western<br><br>
<input type="submit" value="Add Movie">
</form><br><hr>
_END;

  if (isset($_GET['title']) && 
    isset($_GET['company']) && 
    isset($_GET['year']) &&
    isset($_GET['rating']))
  {
    $title = $_GET['title'];
    $company = $_GET['company'];
    $year = $_GET['year'];
    $rating = $_GET['rating'];

    $maxMovieID_result = mysql_query("select * from MaxMovieID", $db_connection);
    if(!$maxMovieID_result) die("Database access failed:" . mysql_error());

    $maxMovieID = mysql_fetch_row($maxMovieID_result);

    $newMovieID = $maxMovieID[0] + 1;

    if(!mysql_query("update MaxMovieID set id='$newMovieID'", $db_connection))
      die("Database update failed:" . mysql_error());

    if(!mysql_query("insert into Movie values" . 
                    "('$newMovieID', '$title', '$year', '$rating', '$company')", $db_connection))
      die("Database insert failed:" . mysql_error());

    $genre = $_GET['genre'];
    if(!empty($genre)){
      foreach($genre as $gen){
        if(!mysql_query("insert into MovieGenre values" . 
                        "('$newMovieID', '$gen')", $db_connection))
          die("Database insert failed:" . mysql_error());
      }
    }

    echo "<h3>-- New movie added --</h3><br>";
    echo "Movie ID: $newMovieID<br>";
    echo "Title: $title<br>";
    echo "Company: $company<br>";
    echo "Year: $year<br>";
    echo "MPAA Rating: $rating<br>";

    if(!empty($genre)){
      echo "Genre: ";
      foreach($genre as $gen){
        echo "$gen ";
      }
    }

  }

  mysql_close($db_connection);
?>