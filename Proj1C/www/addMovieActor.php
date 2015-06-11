<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

echo <<<_END
<form method="get" action="addMovieActor.php">
<h2>Add an actor in a movie:</h2>
Movie: 
<select name="movie">
_END;

  $query_movie = "select id, title, year from Movie order by title";
  $result_movie = mysql_query($query_movie, $db_connection);
  if(!$result_movie)   die('Database select failed: ' . mysql_error());
  while($row_movie = mysql_fetch_row($result_movie)){
    echo "<option value=\"$row_movie[0]\">$row_movie[1] ($row_movie[2])</option>";
  }

echo <<<_END
</select><br><br>
Actor: 
<select name="actor">
_END;

  $query_actor = "select id, last, first, dob from Actor order by last";
  $result_actor = mysql_query($query_actor, $db_connection);
  if(!$result_actor)   die('Database select failed: ' . mysql_error());
  while($row_actor = mysql_fetch_row($result_actor)){
    echo "<option value=\"$row_actor[0]\">$row_actor[1], $row_actor[2] (DOB: $row_actor[3])</option>";
  }

echo <<<_END
</select><br><br>
Role: <input type="text" name="role"><br><br>
<input type="submit" value="Add!!!">
</form><br><hr>
_END;

  if (isset($_GET['movie']) && isset($_GET['actor']) && isset($_GET['role']))
  {
    $movieid = $_GET['movie'];
    $actorid = $_GET['actor'];
    $role = $_GET['role'];

    if(!mysql_query("insert into MovieActor values" . 
                    "('$movieid', '$actorid', '$role')", $db_connection))
      die("Database insert failed:" . mysql_error());

    echo "<h3>-- New Movie-Actor Relation Added --</h3><br>";
  }

  mysql_close($db_connection);
?>