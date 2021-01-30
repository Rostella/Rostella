#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


int lcdColumns = 20;
int lcdRows = 4;
String lastWatered = "";
String specialstatus = "";
int automatic = 0;
double soil = 100.0;
int seconds = 30;
int threshold = 100;
int wateruntil = 100;
boolean light = false;
boolean error = false;
boolean wateringnow = false;
boolean requestedwateringnow = false;
const char* ssid = "WIFI_USER";
const char* password = "WIFI_PASS";

const char* ssid2 = "WIFI_USER_2";
const char* password2 = "WIFI_PASS_2";

String lastResponse;
String serverName = "http://YOURDATABASE.ORG/";

// 0x27 is the LCD address for I2C
// You can find this from any I2C scanner
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

unsigned long lastTime = 0;
unsigned long lastInfo = 0;
unsigned long waterStop = 0;
unsigned long lastStatus = 0;
unsigned long timerDelay = 60*1000*10; //unused
unsigned long statusTime = 20*1000;
int wificonn = 1; //connect to network 1, 2

void setup(){
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  
  lcd.init();         
  lcd.backlight();
  pinMode(D5, OUTPUT);
  pinMode(D7, INPUT);
  digitalWrite(D5, LOW);
  lcd.setCursor(0,0);
  lcd.print("Loading");
  if(wificonn == 1){
    WiFi.begin(ssid, password); 
  }else if(wificonn == 2){ 
    WiFi.begin(ssid2, password2); 
  }
  delay(6000);
  int tries = 0;
  while(WiFi.status() != WL_CONNECTED){
    tries++;
    lcd.setCursor(0,2);
    Serial.println(WiFi.status());
    lcd.print(String(WiFi.status()));
    if(tries > 5) {
      if(wificonn == 1){
        wificonn = 2;
        lcd.setCursor(0, 2);
        lcd.print("1 -> 2");
      }else if(wificonn == 2){
        wificonn = 1;
        lcd.setCursor(0, 2);
        lcd.print("2 -> 1");
      }
      WiFi.disconnect(); setup(); break;
    } 
    delay(1000);
  }
}

void loop(){
  if(lastStatus + statusTime < millis()){
    if(wateringnow){
      specialstatus = "Watering -> " + String(wateruntil) + "%"; 
      lastStatus = millis();
    }else{
      specialstatus = "";
    }
  }
  if((soil < threshold) && !wateringnow){
    if(automatic == 1){
      wateringnow = true;
      autoWater();
      specialstatus = "Watering -> " + String(wateruntil) + "%"; 
      lastStatus = millis();
    }else if(automatic == 0){
      specialstatus = "Low soil%&auto off";
      lastStatus = millis();
    }
  }
  if(((soil > wateruntil) && automatic == 1) && wateringnow){
    wateringnow = false;
    stopWater();
    specialstatus = "Done watering"; 
    lastStatus = millis();
  }
  if((millis() > waterStop) && requestedwateringnow){
    stopWater();
    specialstatus = "Stopped watering";
    lastStatus = millis();
  }
  if((millis() - lastTime) > 10*1000){
    updateInfo();
  }else if((millis() - lastInfo) > 1000){
    info();
  }
}

void info(){

  int read = analogRead(A0);
  int maxv = 760; int minv = 434; int ad = maxv-minv;
  soil = (min((double)100, max(100 - (double)(read-minv)/ad*100, (double)0)));
  light = digitalRead(D7);
  lastInfo = millis();

  lcdUpdate();
}

int tryc = 0;

void updateInfo(){
  info();
  
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + "info.php?username={USERNAME}&password={PASSWORD}&soil="+String((int)soil)+"&light="+String(light);
      Serial.println(serverPath);
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        String payload = http.getString();
        lastResponse = payload;
        error=false;
        tryc = 0;
      }
      else {
        Serial.println(String(httpResponseCode) + " "); 
        if(tryc < 3) {
          delay(200);
          tryc++;
          updateInfo();
        }else{
          lastResponse = "Error : " + String(httpResponseCode) + ". ";
          error=true;
        }
      }
      // Free resources
      http.end();
    }
    else {
      lastResponse = "Restarting";
      error=true;
      lcd.setCursor(0,2);
    lcd.print(WiFi.status());
      WiFi.disconnect();
      delay(1);
      WiFi.begin(ssid, password);
    }
  
    StaticJsonDocument<400> doc;
    deserializeJson(doc, lastResponse);
    lastWatered = doc[0]["water"].as<String>();
    seconds = doc[0]["seconds"].as<int>();  
    automatic = doc[0]["auto"].as<int>();
    threshold = doc[0]["min"].as<int>();
    wateruntil = doc[0]["max"].as<int>();
    
    if((int)(doc[0]["requested"].as<int>() == 1)){
      water();
      specialstatus="Requested.";
      lastStatus = millis();
    }
    lastTime = millis();
    lcdUpdate();
}

void water(){
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + "water.php?username={USERNAME}&password={PASSWORD}";
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      http.end();
      requestedwateringnow = true;
      waterStop = millis() + seconds*1000L;
      digitalWrite(D5, HIGH);
    }
    else {
      lastResponse = "Restarting";
      error=true;
      lcd.setCursor(0,2);
      lcd.print(WiFi.status());
      WiFi.disconnect();
      delay(1);
      WiFi.begin(ssid, password);
    }
}

void autoWater(){
  if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName + "water.php?username={USERNAME}&password={PASSWORD}";
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      http.end();

      waterStop = millis() + 60*1000;
      digitalWrite(D5, HIGH);
    }
    else {
      lastResponse = "Restarting";
      error=true;
      lcd.setCursor(0,2);
      lcd.print(WiFi.status());
      WiFi.disconnect();
      delay(1);
      WiFi.begin(ssid, password);
    }
}

void stopWater(){
    digitalWrite(D5, LOW);
    wateringnow = false;
    requestedwateringnow = false;
}

void lcdUpdate(){
  if(error){
    lcd.setCursor(0, 0);
    lcd.print("Error: " + lastResponse);
  }else{
    if(!(wateringnow || requestedwateringnow)){
      lcd.setCursor(0,0);
      lcdLine("Watered: " + String(lastWatered)); 
    }else{
      lcd.setCursor(0,0);
      lcd.print("Watering now...");
    }
  }

  
  lcd.setCursor(0,1);
  lcdLine("Soil: " + String(soil) + "%");
  lcd.setCursor(0,2);
  lcdLine("Light: " + String(light==0?"On":"Off"));
  lcd.setCursor(0,3);
  lcdLine(specialstatus);
}

void lcdLine(String s){ 
  String k = "                    ";
  lcd.print(s + k.substring(0, 20-s.length()));
}
