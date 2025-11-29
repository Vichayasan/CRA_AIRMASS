#define title1 "PM2.5" // Text that will be printed on screen in any font
#define title2 "PM1"
#define title3 "PM10"
#define title4 "CO2"
#define title5 "VOC"
#define title6 "Update"
#define title7 "ug/m3"
#define title8 "RH"
#define title9 "T"

#include <TimeLib.h>
#include "Drawing.h"
#include "lv1.h"
#include "lv2.h"
#include "lv3.h"
#include "lv4.h"
#include "lv5.h"
#include "lv6.h"

struct tm timeinfo;

unsigned long time_s = 0;
unsigned long _epoch = 0;

// int xpos = 0;
// int ypos = 0;

bool connectWifi;

int testNum = 0;

void t2CallShowEnv(){
  tft.setTextDatum(MC_DATUM);
  xpos = tft.width() / 2 ; // Half the screen width

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.setFreeFont(CF_OL24);
  int mid = (tftMax / 2) - 72;
  tft.setTextPadding(100);
  tft.drawString(title7, xpos - 70, 125 - 6, GFXFF); // Print the test text in the custom font

  tft.setFreeFont(CF_OL32);
  tft.drawString(title1, xpos - 70, 155 - 6, GFXFF); // Print the test text in the custom font

  // ################################################################ for testing
  // if (testNum == 100){
  //   testNum = 0;
  // }
  //        data.pm25_env = testNum;    //for testing
  //        testNum+=10;
  // ################################################################ end test
  

  drawPM2_5(data.pm25_env, mid, 45 - 5);

  tft.setTextSize(1);
  tft.setFreeFont(CF_OL32);                 // Select the font

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);

  tft.setFreeFont(FSB9);   // Select Free Serif 9 point font, could use:

  drawPM1(data.pm01_env, 6, 195);
  tft.drawString(title2, 40, 235, GFXFF); // Print the test text in the custom font

  drawPM10(data.pm100_env, 55, 195);
  tft.drawString(title3, 100, 235, GFXFF); // Print the test text in the custom font
  
  drawCO2(sgp.eCO2, 115, 195);
  tft.drawString(title4, 150, 235, GFXFF); // Print the test text in the custom font

  drawVOC(sgp.TVOC, 170, 195);
  tft.drawString(title5, 210, 235, GFXFF); // Print the test text in the custom font


  tft.drawString(title8, 250, 215, GFXFF); // Print the test text in the custom font
  drawH(hum, 255, 195);
  tft.drawString("%", 312, 215, GFXFF);

  tft.drawString(title9, 250, 235, GFXFF); // Print the test text in the custom font
  drawT(temp, 255, 215);
  tft.drawString("C", 312, 235, GFXFF);

  //Clear Stage

  ind.createSprite(320, 10);
  ind.fillSprite(TFT_BLACK);{

  if ((data.pm25_env >= 0) && (data.pm25_env <= 15.0)) {
    tft.setWindow(0, 25, 55, 55);
    tft.pushImage(tft.width() - lv1Width - 6, 45 - 5, lv1Width, lv1Height, lv1);
    ind.fillTriangle(0, 0, 5, 5, 10, 0, FILLCOLOR1);

  } else if ((data.pm25_env >= 15.1) && (data.pm25_env <= 25.0)  ) {
    tft.pushImage(tft.width() - lv2Width - 6, 45 - 5, lv2Width, lv2Height, lv2);
    ind.fillTriangle(55 + 8, 0, 60 + 8, 5, 65 + 8, 0, FILLCOLOR1);

  } else  if ((data.pm25_env >= 25.1) && (data.pm25_env <= 37.5)  ) {
    tft.pushImage(tft.width() - lv3Width - 6, 45 - 5, lv3Width, lv3Height, lv3);
    ind.fillTriangle(105 + 21, 0, 110 + 21, 5, 115 + 21, 0, FILLCOLOR1);

  } else  if ((data.pm25_env >= 37.6) && (data.pm25_env <= 75.0)  ) {
    tft.pushImage(tft.width() - lv4Width - 6, 45 - 5, lv4Width, lv4Height, lv4);
    ind.fillTriangle(155 + 34, 0, 160 + 34, 5, 165 + 34, 0, FILLCOLOR1);

  } else  if ((data.pm25_env >= 75.1)) {
    tft.pushImage(tft.width() - lv5Width - 6, 45 - 5, lv5Width, lv5Height, lv5);
    ind.fillTriangle(210 + 42, 0, 215 +42, 5, 220 + 42, 0, FILLCOLOR1);

  // } else {
  //   tft.pushImage(tft.width() - lv6Width - 6, 45, lv6Width, lv6Height, lv6);
  //   ind.fillTriangle(265, 0, 270, 5, 275, 0, FILLCOLOR1);

  }
    ind.pushSprite(29, 175);
    ind.deleteSprite();
  }


  tft.setTextColor(0xFFFF);
  int mapX = 315;
  int mapY = 30;

  if (connectWifi == true){
    int rssi = map(WiFi.RSSI(), -90, -50, 25, 100);
    if (rssi > 100) rssi = 100;
    if (rssi < 0) rssi = 0;
    tft.fillRect(275, 5, 45, 35, 0x0000);
    tft.drawString(String(rssi) + "%", mapX, mapY, GFXFF);
    tft.fillCircle(256, 16, 16, 0x001F);
    tft.setTextColor(0xB7E0);
    tft.setFreeFont(FSSB9);
    tft.drawString("W", 265, 27);
  }else{
    tft.fillRect(275, 5, 45, 35, 0x0000);
    tft.drawString("No Signal", mapX, mapY, GFXFF);
    tft.fillCircle(256 - 35, 16, 16, 0x001F);
    tft.setTextColor(0x0000);
    tft.setFreeFont(FSSB9);
    tft.drawString("W", 265 - 35, 27);
  }
  

}

String a0(int n) {
  return (n < 10) ? "0" + String(n) : String(n);
}

void t7showTime() {


  topNumber.createSprite(200, 40);
  //  stringPM1.fillSprite(TFT_GREEN);
  topNumber.setFreeFont(FS9);
  topNumber.setTextColor(TFT_WHITE);
  topNumber.setTextSize(1);           // Font size scaling is x1


  unsigned long NowTime = _epoch + ((millis() - time_s) / 1000) + (7 * 3600);
  String timeS = "";

  if (connectWifi == false) {
    // timeS = a0(day(NowTime)) + "/" + a0(month(NowTime)) + "/" + String(year(NowTime)) + "  [" + a0(hour(NowTime)) + ":" + a0(minute(NowTime)) + "]";

  } else {
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      timeS = "Failed to obtain time";
      //      ESP.restart();
      return;
    }else{
      timeS = a0(timeinfo.tm_mday) + "/" + a0(timeinfo.tm_mon + 1) + "/" + String(timeinfo.tm_year + 1900) + "  " + a0(timeinfo.tm_hour) + ":" + a0(timeinfo.tm_min) + "";
    }
    
  }

  topNumber.drawString(timeS, 5, 10, GFXFF);

  topNumber.pushSprite(5, 5);
  topNumber.deleteSprite();


}

