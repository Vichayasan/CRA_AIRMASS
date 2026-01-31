#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Adafruit_MLX90614.h>
// #include <WiFiManager.h>

#include "MySensor.h"
#include "MyLCD.h"
#include "task.h"
#include "HardwareSerial_NB_BC95.h"
#include "sendProtocal.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
HardwareSerial_NB_BC95 AISnb;
// WiFiManager wm;

unsigned long previousMillis, taskMillis, timeTask;

const char* ssid = "greenioIoT";
const char* password = "*********";

String deviceToken, mqttTK, control_version;

const char *ntpServer1 = "pool.ntp.org";
const char *ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600 * 7;

#define WDTPin    4

void callOTA(){
  // Port defaults to 3232
//  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  // ArduinoOTA.setTimeout(300000);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
      tft.setTextColor(TFT_GOLD);
      tft.setFreeFont(FSSB9);
      tft.setTextSize(0.5);
      tft.drawString("OTA oP", 265 - 40, 27);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%% /n" + progress / (total / 100));
      // tft.setTextDatum(MC_DATUM);
      // tft.drawString("OTA", tft.width() / 2, 5, GFXFF);
      // tft.fillRect(220, 5, 45, 35, 0x0000);
      // tft.drawString("OTA: " + String(progress / (total / 100)) + "%", 315, 30, TFT_GOLD);
      // tft.fillRect(tft.width() / 2, 15, 100, 18, TFT_GREEN);
      
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

int mapRSSIToDBm(int rssi)
{
  if (rssi == 99){
    return -113; // Not detectable
  }else{
    return -113 + (rssi * 2); // Map RSSI to dBm
  }
}

void getMac()
{
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("Get Mac");
  Serial.println("OK");
  Serial.print("+deviceToken: ");
  Serial.println(WiFi.macAddress());
  // SerialBT.print("Get Mac");
  // SerialBT.println("OK");
  // SerialBT.print("+deviceToken: ");
  // SerialBT.println(WiFi.macAddress());
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 0x10) {
      deviceToken += "0"; // Add leading zero if needed
    }
    deviceToken += String(mac[i], HEX); // Convert byte to hex
  }
  deviceToken.toUpperCase();
  // ESPUI.updateLabel(GUI.idLabel, String(deviceToken));

  // --- NEW CODE TO EXTRACT DIGITS ---
//  for (int i = 0; i < deviceToken.length(); i++) {
//    // Check if the character at the current position is a digit
//    if (isDigit(deviceToken.charAt(i))) {
//      // If it is a digit, add it to our new string
//      digitsOnlyToken += deviceToken.charAt(i);
//    }
//  }

}

int mapRSSI(){
  int rssi = map(WiFi.RSSI(), -90, -50, 25, 100);
  if (rssi > 100) rssi = 100;
  if (rssi < 0) rssi = 0;
  return rssi;
}

void _initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFiStatText = "Wi-Fi Connection Failed!";
    Serial.println(WiFiStatText);
    connectWifi = false;
    Serial.println(connectWifi);

    delay(2000);

  }else{
    WiFiStatText = "Wi-Fi Connection Succeed!";
    Serial.println(WiFiStatText);
    // delay(2000);
    configTime(gmtOffset_sec, 0, ntpServer1, ntpServer2);
    delay(2000);
    connectWifi = true;
    Serial.println(connectWifi);
  }

}

// void _initMN(){
//   WiFi.mode(WIFI_STA);
//   // wm.resetSettings();
//   wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
//   bool res;
//   // res = wm.autoConnect(); // auto generated AP name from chipid
//   // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
//   String hostMN = "CRA-wm-" + deviceToken;
//   res = wm.autoConnect(hostMN.c_str()); // password protected ap

//   if(!res) {
//     WiFiStatText = "Wi-Fi Connection Failed!";
//     Serial.println(WiFiStatText);
//     connectWifi = false;
//     Serial.println(connectWifi);

//     delay(2000);
//     // ESP.restart();
//   } 
//   else {
//     //if you get here you have connected to the WiFi    
//     WiFiStatText = "Wi-Fi Connection Succeed!";
//     Serial.println(WiFiStatText);
//     // delay(2000);
//     configTime(gmtOffset_sec, 0, ntpServer1, ntpServer2);
//     delay(2000);
//     connectWifi = true;
//     Serial.println(connectWifi);
//   }
// }

void heartBeat()
{
  // Sink current to drain charge from watchdog circuit
  pinMode(WDTPin, OUTPUT);
  digitalWrite(WDTPin, LOW);

  // Return to high-Z
  pinMode(WDTPin, INPUT);


  Serial.println("Heartbeat");
}

void setup() {
  
  Serial.begin(115200);
  Serial.println("Booting");
  getMac();
  _initBME280();
  _initSGP30();
  _initLCD();
  mlx.begin();
  tft.fillScreen(TFT_BLACK);            // Clear screen
  _initWiFi();
  // _initMN();
  
  control_version = "4.0";
  // delay(2000);
  AISnb.debug = true;
  AISnb.setupDevice("");
  mqttTK = AISnb.getNCCID();

  serverName.concat(mqttTK);
  serverName.concat("/telemetry");
  Serial.println("Server Name: " + serverName);
  // delay(2000);
  splash(deviceToken, mqttTK, control_version);
  // Serial.println(AISnb.getNCCID());

  callOTA();
  delay(2000);

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // tft.fillScreen(TFT_DARKCYAN);
  
  delay(2000);

  tft.fillScreen(TFT_BLACK);            // Clear screen
  tft.fillRect(5, 185, tft.width() - 15, 5, TFT_SKYBLUE); // Print the test text in the custom font
  tft.fillRect(63 + 5, 185, tft.width() - 15, 5, TFT_GREEN); // Print the test text in the custom font
  tft.fillRect(113 + 18, 185, tft.width() - 15, 5, TFT_YELLOW); // Print the test text in the custom font
  tft.fillRect(166 + 28, 185, tft.width() - 15, 5, TFT_ORANGE); // Print the test text in the custom font
  tft.fillRect(219 + 38, 185, tft.width() - 15, 5, TFT_RED); // Print the test text in the custom font
  // tft.fillRect(272, 185, tft.width() - 15, 5, TFT_BURGUNDY); // Print the test text in the custom font

  t2CallShowEnv();

}

void loop() {
  ArduinoOTA.handle();
  unsigned long ms = millis();

  if (connectWifi == true){
    // Serial.println(connectWifi);
    if(!WiFi.isConnected()){
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
      }
    }
  }

  if (ms - timeTask >= 10000){
    timeTask += 10000;
    t7showTime();
    t2CallShowEnv();
    // Serial.println(AISnb.getNCCID());
  }

  if (ms - taskMillis >= 30000){
    taskMillis += 30000;
    getBME280Data();
    getDataSGP30();
    t1CallGetProbe();
    
    
  }

  if (ms - previousMillis >= 60000){
    previousMillis += 60000;
//     Serial.println("Attach WiFi for, OTA "); 
//     Serial.println(WiFi.RSSI());
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
// //    getMac();
    // callOTA();
    String jsonMsg = "";
    // jsonMsg.concat("{\"DevicToken\":\"");
    // jsonMsg.concat(deviceToken);
    // jsonMsg.concat("\",\"ccid\":\"");
    // jsonMsg.concat(mqttTK);
    // jsonMsg.concat("\",\"version\":\"");
    jsonMsg.concat("{\"v\":\"");
    jsonMsg.concat(control_version);

    jsonMsg.concat("\",\"hum\":");
    jsonMsg.concat(hum);
    jsonMsg.concat(",\"temp\":");
    jsonMsg.concat(temp);
    jsonMsg.concat(",\"pres\":");
    jsonMsg.concat(pres);

    jsonMsg.concat(",\"co2\":");
    jsonMsg.concat(sgp.eCO2);
    jsonMsg.concat(",\"voc\":");
    jsonMsg.concat(sgp.TVOC);

    jsonMsg.concat(",\"rssi\":");
    jsonMsg.concat(mapRSSI());

    jsonMsg.concat(",\"pm1\":");
    jsonMsg.concat(data.pm01_env);
    jsonMsg.concat(",\"pm10\":");
    jsonMsg.concat(data.pm100_env);
    jsonMsg.concat(",\"pm2.5\":");
    jsonMsg.concat(data.pm25_env);

    jsonMsg.concat(",\"pn03\":");
    jsonMsg.concat(data.particles_03um);
    jsonMsg.concat(",\"pn05\":");
    jsonMsg.concat(data.particles_05um);
    jsonMsg.concat(",\"pn10\":");
    jsonMsg.concat(data.particles_10um);
    jsonMsg.concat(",\"pn25\":");
    jsonMsg.concat(data.particles_25um);
    jsonMsg.concat(",\"pn50\":");
    jsonMsg.concat(data.particles_50um);
    jsonMsg.concat(",\"pn100\":");
    jsonMsg.concat(data.particles_100um);

    jsonMsg.concat("}");

    Serial.println(jsonMsg);
    if (connectWifi){
        httpSend(jsonMsg);
      }
  }
}
