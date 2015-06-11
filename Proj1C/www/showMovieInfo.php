<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

echo <<<_END
<head>
<title>Movie Information</title>
</head>
<h1>Movie Information</h1>
<form method="get" action="showMovieInfo.php">
<input type="hidden" name="movieid">
</form><br>
_END;

  if (isset($_GET['movieid']))
    $movieid = $_GET['movieid'];
  else {
    $query_id = "select min(id) from Movie";
    $result_id = mysql_query($query_id, $db_connection);
    if(!$result_id)   die('Database select failed: ' . mysql_error());
    $row_id = mysql_fetch_row($result_id);
    $movieid = $row_id[0];
  }
    $query_movie = "select * from Movie where id=$movieid";
    $result_movie = mysql_query($query_movie, $db_connection);
    if(!$result_movie)   die('Database select failed: ' . mysql_error());
    $row_movie = mysql_fetch_row($result_movie);

    $query_dir = "select first, last " . 
                  "from Director " . 
                  "where id = (select did from MovieDirector " . 
                              "where mid = $movieid)";

    $result_dir = mysql_query($query_dir, $db_connection);
    if(!$result_dir)   die('Database select failed: ' . mysql_error());
    $row_dir = mysql_fetch_row($result_dir);

    $query_genre = "select genre from MovieGenre where mid = $movieid";

    $result_genre = mysql_query($query_genre, $db_connection);
    if(!$result_genre)   die('Database select failed: ' . mysql_error());
    $row_genre = mysql_fetch_row($result_genre);

    echo <<<_END
    Title: $row_movie[1]<br>
    Producer: $row_movie[4]<br>
    MPAA Rating: $row_movie[3]<br>
    Director: $row_dir[1], $row_dir[0]<br>
    Genre: $row_genre[0]<br><br>
    -- Actors in this movie: --<br>
_END;

    $query_actor = "select a.first, a.last, ma.role, a.id " . 
                  "from Actor a, MovieActor ma " . 
                  "where ma.mid = $movieid and a.id = ma.aid " . 
                  "order by a.last";

    $result_actor = mysql_query($query_actor, $db_connection);
    if(!$result_actor)   die('Database select failed: ' . mysql_error());

    while($row_actor = mysql_fetch_row($result_actor)) {
      echo "<a href=\"./showActorInfo.php?actorid=$row_actor[3]\" target=\"main\">$row_actor[1], $row_actor[0]</a> act as $row_actor[2]<br>";
    }

    echo <<<_END
    <hr>
    <form method="get" action="search.php">
    Search: <input type="text" name="searchtext"><br><br>
    <input type="submit" value="Search">
    </form><br>
_END;


  mysql_close($db_connection);

?>