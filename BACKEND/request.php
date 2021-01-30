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
    $sql = "UPDATE currentdata SET requested=1, seconds=30 WHERE number = 1;"; 
    if(isset($_GET['seconds'])){
        $sql = "UPDATE currentdata SET requested=1, seconds=" . $_GET['seconds'] ." WHERE number = 1;";
    }
    $result = mysqli_query($conn, $sql);
    echo $result==1?"Requested":"FAILURE";
}
?>
