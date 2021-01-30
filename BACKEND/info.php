<?php
$servername = "DATABASE_SERVERNAME";
$username = "DATABASE_USERNAME";
$password = "DATABASE_PASSWORD";

// Create connection
$conn = new mysqli("localhost", $username, $password, $servername);

// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

if(isset($_GET['username']) && (strcmp($_GET['username'], "URL_USERNAME") == 0 && strcmp($_GET['password'], "URL_PASSWORD") == 0)){
    
    date_default_timezone_set('America/Los_Angeles');
    $date = date("m-d G:i");
    
    $sql = "UPDATE currentdata SET soil = " . $_GET['soil'] . ", lastupdate = '" . $date . "', light = " . $_GET['light'] . " WHERE number = 1;";
    $result = mysqli_query($conn, $sql);
}
$sql = 'SELECT * FROM `currentdata` WHERE number = 1 LIMIT 1';
$data = mysqli_query($conn, $sql);
$result = array();
while ( $row = $data->fetch_assoc())  {
    $result[]=$row;
}
echo json_encode($result);

?>
