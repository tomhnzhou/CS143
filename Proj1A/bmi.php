<?php 
  if (isset($_GET['height']) && isset($_GET['weight'])){
    $height = $_GET['height'];
    $weight = $_GET['weight'];
    if ($height != 0 && $weight != 0)
    {
      $bmi = $weight / ($height * $height);
      if ($bmi <= 18.5)
        $result = "underweight";
      else if ($bmi <= 24.9)
        $result = "normal weight";
      else if ($bmi <= 29.9)
        $result = "overweight";
      else if ($bmi <= 39.9)
        $result = "obese";
      else
        $result = "morbidly obese";
      $report = "You are " . $result;
    }
    else
    {
      $bmi = 0;
      $report = "Wrong input!!!";
    }
    
  }
  else {
    $bmi = 0;
    $report = "Wrong input!!!";
  }

  echo <<<_END
  <html>
    <head>
      <title>BMI Calculator</title>
    </head>
    <body>
    
    <h1>Body Mass Index Calculator</h1>
    <form method="get" action="bmi.php">
      Height (m): 
      <input type="text" name="height">
      Weight (kg):
      <input type="text" name="weight"><br>
      <input type="submit" value="Calculate BMI">
    </form>
    <hr>
    <h2>Report</h2>
    BMI is: $bmi<br>
    $report<br>
    </body>
  </html>
_END;
?>