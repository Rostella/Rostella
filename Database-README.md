# Database explanation  
![https://i.imgur.com/CuJwCU4.png](https://i.imgur.com/CuJwCU4.png)

Our files use the database table 'currentdata' for all info. The server name, username, and password will be seperate and will be changed in the php files.
In the arduino/esp12e software, you change the URL username and passwords to match the URL_USERNAME and URL_PASSWORD in the php files.

- number: For the ease of having multiple systems on one database, we use a numbering system. If you only have one unit, use 1.
- soil: Updated value from the unit, 0-100.
- lastupdate: Last refresh / data update.
- water: Last water, automatic or remote.
- light: Value 0 or 1, 0 if low light, 1 if lit (inversed)
- requested: 0 unless there is a pending request from a user (from request.php)
- min: Threshold value that automatically turns on the device to water.
- max: What the unit will water to automatically from min. 
- seconds: How long the unit will water if remotely requested from a user.
- auto: Value 0 or 1, 0 to disable automatic watering (still allows for remote requests), makes min and max not used. 1 for normal usage.
