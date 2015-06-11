<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");
  if(!$db_connection) {
    die('Could not connect: ' . mysql_error());
  }
  mysql_select_db("CS143", $db_connection);

  if (isset($_GET['query']))
      $query = $_GET['query'];

  echo <<<_END
  <html>
    <head>
      <title>Query</title>
    </head>
    <body>
    
    <h1>Query</h1>
    Type a query in the following box: <br>
    <textarea rows="10" cols="80" name="query" form="queryform">$query</textarea>
    <form method="get" action="query.php" id="queryform">
      <input type="submit" value="Submit">
    </form>
_END;

  echo "<br><h2>Query: </h2>";
  echo "$query<br>";
  echo "<hr><h2>Results from Mysql: </h2>";

  if (isset($_GET['query'])){
    $result = mysql_query($query, $db_connection);
    if(!$result) {
      die('Query failed: ' . mysql_error());
    }

    $n_column = mysql_num_fields($result);
    
    echo "<table border='1'><tr>";
    for($i=0; $i < $n_column; $i++){
      $attribute = mysql_fetch_field($result, $i);
      echo "<td>" . $attribute->name . "</td>";
    }
    echo "</tr>";

    while($row = mysql_fetch_row($result)) {
      echo "<tr>";
      for($i=0; $i < $n_column; $i++){
        echo "<td>$row[$i]</td>";
      }
      echo "</tr>";
    }
  }
  else
    echo "Query was empty";

  echo "</body></html>";

  mysql_close($db_connection);
?>