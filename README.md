[Rostella](https://rostella.github.io) IOT Automatic / Remote Garden System
===================
---
Please read https://rostella.github.io to introduce yourself to the system.

Our system works by making a two-way connection between the device and database. By using a request through HTTP we can send and receive data on a WiFi network. Our database runs on MySQL specifically.

The database can include any info, specifically with our setup:
 - Soil moisture %
 -  Update time
 -  Water thresholds (min, max)
 -  Last watered
 -  Enviornment lighting

To build our model, we recommend either wood or plastic (plastic if the unit will come into contact with moisture) and we have a parts list for the base model attached. 

The unit runs on the ESP12-E, which can change to work with other devices or integrated devices if necessary.
 ![blueprints](https://i.imgur.com/K705pEQ.png)