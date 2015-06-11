<?php 
  $db_connection = mysql_connect("localhost", "cs143", "");

  if (isset($_GET['query'])){
    
  }
  else {

  }

  echo <<<_END
  <html>
    <head>
      <title>Query</title>
    </head>
    <body>
    
    <h1>Query</h1>
    <form method="get" action="query.php">
      Type a query in the following box: <br>
      <input type="textarea" name="query">
      <input type="submit" value="Calculate">
    </form>
    </body>
  </html>
_END;
?>