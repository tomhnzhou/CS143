<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

echo <<<_END
<head>
<title>Search</title>
</head>
<h1>Search for Movie or Actor</h1>
<form method="get" action="search.php">
Search: <input type="text" name="searchtext"><br><br>
<input type="submit" value="Search">
</form><br><hr>
_END;

  if (isset($_GET['searchtext']))
  {
    $searchtext = $_GET['searchtext'];
    $search_query = format_query($searchtext);

    $result_movie = mysql_query($search_query[0], $db_connection);
    if(!$result_movie)   die('Database select failed: ' . mysql_error());

    $result_actor = mysql_query($search_query[1], $db_connection);
    if(!$result_actor)   die('Database select failed: ' . mysql_error());

    echo "<h2>Search results for Movie:</h2><br>";
    print_result($result_movie, "Movie");
    echo "<h2>Search results for Actor:</h2><br>";
    print_result($result_actor, "Actor");
  }


  mysql_close($db_connection);

  function format_query($st) {
    if(preg_match("/ and /i", $st))
    {
      $words = preg_split("/ and /i", $st);
      //print_r($words);
      $query[0] = "select * from Movie " . 
                  "where title like '%$words[0]%' and title like '%$words[1]%' " . 
                  "order by title";
      $query[1] = "select * from Actor " . 
                  "where (first like '%$words[0]%' and first like '%$words[1]%') " . 
                  "or (last like '%$words[0]%' and last like '%$words[1]%') " . 
                  "order by last";
    }
    else if(preg_match("/ /i", $st)){
      $words = preg_split("/[\s]+/i", $st);
      //print_r($words);
      $query[0] = "select * from Movie " . 
                  "where title like '%$words[0]%' and title like '%$words[1]%' " . 
                  "order by title";
      $query[1] = "select * from Actor " . 
                  "where first like '%$words[0]%' and last like '%$words[1]%' " . 
                  "order by last";
    }
    else
    {
      $query[0] = "select * from Movie " . 
                  "where title like '%$st%' " . 
                  "order by title";
      $query[1] = "select * from Actor " . 
                  "where first like '%$st%' or last like '%$st%' " . 
                  "order by last";
    }
    return $query;
  }

  function print_result($result, $table)
  {
    if($table == "Movie"){
      while($row = mysql_fetch_row($result)) 
        echo "Movie: <a href=\"./showMovieInfo?movieid=$row[0]\">$row[1]</a> ($row[2])<br>";
    }
    else if($table == "Actor") {
      while($row = mysql_fetch_row($result))
        echo "Actor: <a href=\"./showActorInfo.php?actorid=$row[0]\">$row[1], $row[2]</a> ($row[4])<br>";
    }
  }
?>