#include "Free_Fonts.h"
#include "Logo.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "color.h"

#define CF_OL24 &Orbitron_Light_24
#define CF_OL32 &Orbitron_Light_32

#define FILLCOLOR1 0xFFFF

#define TFT_BURGUNDY  0xF1EE

#define swTFTPin 32

#define SERIAL1_RXPIN 16
#define SERIAL1_TXPIN 17

int xpos = 0;
int ypos = 0;

boolean ready2display = false;

String WiFiStatText;

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite stringPM25 = TFT_eSprite(&tft);
TFT_eSprite stringPM1 = TFT_eSprite(&tft);
TFT_eSprite stringPM10 = TFT_eSprite(&tft);
TFT_eSprite stringUpdate = TFT_eSprite(&tft);
TFT_eSprite stringCO2 = TFT_eSprite(&tft);
TFT_eSprite stringVOC = TFT_eSprite(&tft);

TFT_eSprite topNumber = TFT_eSprite(&tft);
TFT_eSprite ind = TFT_eSprite(&tft);
TFT_eSprite H = TFT_eSprite(&tft);
TFT_eSprite T = TFT_eSprite(&tft);

void splash(String DeviceID, String DeviceIP, String DeviceV) {
  int xpos =  0;
  int ypos = 40;
  tft.init();
  // Swap the colour byte order when rendering
  tft.setSwapBytes(true);
  tft.setRotation(1);  // landscape

  tft.fillScreen(TFT_BLACK);
  // // Draw the icons
  tft.pushImage(tft.width() / 2 - logoWidth / 2, 39, logoWidth, logoHeight, Logo);
  tft.setTextColor(TFT_GREEN);
  tft.setTextDatum(TC_DATUM); // Centre text on x,y position

  tft.setFreeFont(FSB9);
  xpos = tft.width() / 2; // Half the screen width
  ypos = 150;
  tft.drawString("AIRMASS2.5 Inspector", xpos, ypos, GFXFF);  // Draw the text string in the selected GFX free font
  tft.drawString("version: " + DeviceV, xpos, ypos + 20, GFXFF); // Draw the text string in the selected GFX free font

  //
  String DeviceIDStr = "";
  DeviceIDStr.concat("DeviceID: ");
  DeviceIDStr.concat(DeviceID);
  String DeviceIPStr = "";
  DeviceIPStr.concat("NCCID: ");
  DeviceIPStr.concat(DeviceIP);
  //delay(4000);
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FSB9);
  tft.drawString(DeviceIDStr, xpos, ypos + 40, GFXFF);
  tft.drawString(DeviceIPStr, xpos, ypos + 60, GFXFF);
  delay(5000);

  tft.setTextFont(GLCD);

  tft.fillScreen(TFT_DARKCYAN);
  // Select the font
  ypos += tft.fontHeight(GFXFF);                      // Get the font height and move ypos down
  tft.setFreeFont(FSB9);
  //  tft.pushImage(tft.width() / 2 - (Splash2Width / 2) - 15, 3, Splash2Width, Splash2Height, Splash2);



  delay(1200);
  tft.setTextPadding(180);
  tft.setTextColor(TFT_GREEN);
  tft.setTextDatum(MC_DATUM);
  Serial.println(F("Start..."));
  for ( int i = 0; i < 170; i++)
  {
    tft.drawString("Waiting for Wi-Fi", xpos, 100, GFXFF);
    tft.drawString(".", 1 + 2 * i, 150, GFXFF);
    delay(10);
    //    Serial.println(i);
  }
  tft.drawString(WiFiStatText, xpos, 150 + 30, GFXFF);
  delay(1200);
  Serial.println("end");
}

void _initLCD() {
    pinMode(swTFTPin, OUTPUT);
    digitalWrite(swTFTPin, LOW);    // turn the LED off by making the voltage LOW
    
    tft.fillScreen(TFT_BLACK);
  // TFT
  // MLX
//   mlx.begin();
}