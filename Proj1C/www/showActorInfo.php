<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

echo <<<_END
<head>
<title>Actor Information</title>
</head>
<h1>Actor Information</h1>
<form method="get" action="showActorInfo.php">
<input type="hidden" name="actorid">
</form><br>
_END;

  if (isset($_GET['actorid']))
    $actorid = $_GET['actorid'];
  else {
    $query_id = "select min(id) from Actor";
    $result_id = mysql_query($query_id, $db_connection);
    if(!$result_id)   die('Database select failed: ' . mysql_error());
    $row_id = mysql_fetch_row($result_id);
    $actorid = $row_id[0];
  }

    $query_actor = "select * from Actor where id=$actorid";
    $result_actor = mysql_query($query_actor, $db_connection);
    if(!$result_actor)   die('Database select actor failed: ' . mysql_error());
    $row_actor = mysql_fetch_row($result_actor);

    if($row_actor[5] == "")
      $row_actor[5] = "-- Still Alive --";

    echo <<<_END
    Name: $row_actor[2] $row_actor[1]<br>
    Sex: $row_actor[3]<br>
    Date of Birth: $row_actor[4]<br>
    Date of Death: $row_actor[5]<br><br>
    -- Act in: --<br>
_END;

    $query_movie = "select ma.role, m.title, m.id " . 
                  "from Movie m, MovieActor ma " . 
                  "where ma.aid = $actorid and m.id = ma.mid " . 
                  "order by m.title";

    $result_movie = mysql_query($query_movie, $db_connection);
    if(!$result_movie)   die('Database select movie failed: ' . mysql_error());

    while($row_movie = mysql_fetch_row($result_movie)) {
      echo "Act as $row_movie[0] in " . 
      "<a href=\"./showMovieInfo.php?movieid=$row_movie[2]\" target=\"main\">$row_movie[1]</a><br>";
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