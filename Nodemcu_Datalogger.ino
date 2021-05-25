/*feel free to contact
 * sreeramaj53@gmail.com
 * www.youtube.com/ZenoModiff
 * last updated - time 09:16 AM - date 25 may 2021
 Github Link :-- https://github.com/Zenomodiff/Nodemcu-Datalogger-With-DHT22-MQ135-NTP-Time-Saving-It-To-Sd-Card
 */

#include "SPI.h"
#include "SD.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define DHTPIN D2   
#define DHTYPE DHT22

const int chipSelect = D8; 
const int mq135 = A0;
DHT dht(DHTPIN, DHTYPE);

const char *ssid     = "PASTE SSID NAME";
const char *password = "PASTE PASSWORD";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

#define SDREADLEDPIN D1 

String weekDays[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

float m = -0.6527; 
float b = 1.30;  
float R0 = 21.91;

void setup() {

  pinMode(mq135, INPUT);
  Serial.begin(115200);
  
  pinMode(SDREADLEDPIN, OUTPUT);
  digitalWrite(SDREADLEDPIN, LOW);

    while (!Serial) {
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    while (1);
  }

  dht.begin();
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();
  timeClient.setTimeOffset(19786);
}

void loop() {

  timeClient.update();

  double mq135_value = analogRead(mq135);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  unsigned long epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime(); 
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();
  String weekDay = weekDays[timeClient.getDay()];    
  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = months[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  
  String fullstring = String(weekDay) + "," + String(currentMonthName) + "," + String(monthDay)+ "," + String(currentYear)   + "," + 
  String(currentHour) + "," + String(currentMinute) + "," + String(currentSecond) + "," + 
  String(t) + "," + String(h) + (",") + String(mq135_value);
  
  Serial.print("Full Data: ");
  Serial.println(fullstring);
  Serial.println("");
  delay(1000);
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  digitalWrite(SDREADLEDPIN, HIGH);
  if (dataFile) {
    dataFile.println(fullstring);
    dataFile.close();
    delay(1000);
    digitalWrite(SDREADLEDPIN, LOW);
  }
  
  else {
    Serial.println("error opening datalog.txt");
  }
  delay(2000);
}
