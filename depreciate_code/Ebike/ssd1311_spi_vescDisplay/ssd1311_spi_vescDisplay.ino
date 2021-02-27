
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>

#include <VescUart.h>
VescUart UART;

#define SCREEN_WIDTH 96 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define clear() fillScreen(0)
#define show endWrite
/*
SD1331 Pin      Arduino ESP8266   rPi
1 GND
2 VCC
3 SCL/SCK/CLK/D0  13  GPIO14/D5 GPIO11/SPI0-SCLK  
4 SDA/MOSI/D1   11  GPIO13/D7 GPIO10/SPI0-MOSI  
5 RES/RST   9 GPIO15/D8 GPIO24      
6 DC/A0 (data)    8 GPIO05/D1 GPIO23      
7 CS      10  GPIO04/D2 GPIO08    
*/
// You can use any (4 or) 5 pins
// hwspi hardcodes those pins, no need to redefine them
#define sclk 15
#define mosi 16
#define cs   9
#define rst  10
#define dc   18

// Option 2: must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
#pragma message "Using HWSPI"
Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);

// This could also be defined as display.color(255,0,0) but those defines
// are meant to work for adafruit_gfx backends that are lacking color()
#define LED_BLACK    0

#define LED_RED_VERYLOW   (3 <<  11)
#define LED_RED_LOW     (7 <<  11)
#define LED_RED_MEDIUM    (15 << 11)
#define LED_RED_HIGH    (31 << 11)

#define LED_GREEN_VERYLOW (1 <<  5)   
#define LED_GREEN_LOW     (15 << 5)  
#define LED_GREEN_MEDIUM  (31 << 5)  
#define LED_GREEN_HIGH    (63 << 5)  

#define LED_BLUE_VERYLOW  3
#define LED_BLUE_LOW    7
#define LED_BLUE_MEDIUM   15
#define LED_BLUE_HIGH     31

#define LED_ORANGE_VERYLOW  (LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW    (LED_RED_LOW     + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM (LED_RED_MEDIUM  + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH   (LED_RED_HIGH    + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW  (LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW    (LED_RED_LOW     + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM (LED_RED_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH   (LED_RED_HIGH    + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW  (LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW    (LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM   (LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH   (LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW (LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW   (LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM  (LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH    (LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)


#define BIKE_HEIGHT   32
#define BIKE_WIDTH    32
const unsigned char bike_bmp [] PROGMEM = {
  // 'bike, 32x32px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3e, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0xe0, 0x00, 0x10, 0x01, 0x20, 
  0x00, 0x3f, 0xff, 0x60, 0x00, 0x57, 0xf9, 0x00, 0x00, 0x97, 0xf3, 0x00, 0x00, 0x10, 0x04, 0x00, 
  0x1f, 0x10, 0x09, 0xf0, 0x31, 0x94, 0x13, 0x18, 0x60, 0xd2, 0x26, 0x0c, 0x40, 0x52, 0x44, 0x04, 
  0x44, 0x4f, 0x84, 0x44, 0x40, 0x42, 0x04, 0x04, 0x60, 0xc2, 0x06, 0x0c, 0x31, 0x81, 0x03, 0x18, 
  0x1f, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//void display_bitmap(uint8_t bmp_num, uint16_t color) { 
//    static uint16_t bmx,bmy;
//
//    // Clear the space under the bitmap that will be drawn as
//    // drawing a single color pixmap does not write over pixels
//    // that are nul, and leaves the data that was underneath
//    display.fillRect(bmx,bmy, bmx+8,bmy+8, LED_BLACK);
//    display.drawBitmap(bmx, bmy, mono_bmp[bmp_num], 8, 8, color);
//    bmx += 8;
//    if (bmx >= mw) bmx = 0;
//    if (!bmx) bmy += 8;
//    if (bmy >= mh) bmy = 0;
//    display.show();
//}

void testdrawroundrect(void) {
  display.clear();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, LED_CYAN_HIGH);
    display.show();
    delay(70);
  }
  delay(70);
}


void setup() {
  Serial.begin(9600); // pc output
  Serial1.begin(115200);
  while (!Serial1) {;}
  UART.setSerialPort(&Serial1);

//     display.begin();
     display.begin(80000000); // higher 80mhz display speed

     display.setTextWrap(false);
     display.setTextColor(LED_PURPLE_HIGH, LED_BLACK);
    display.setAddrWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Test full bright of all LEDs. If brightness is too high
    // for your current limit (i.e. USB), decrease it.
//    display.fillScreen(LED_WHITE_HIGH);
testdrawroundrect();
    
   
    display.clear();
    display.setCursor(0, 0);

  
//  display.println(F("Battery:"));

display.drawBitmap(
    0,
    0,
    bike_bmp, BIKE_WIDTH, BIKE_HEIGHT, LED_WHITE_HIGH);
    
}

void loop() {
  
//    Serial.println(UART.data.rpm);
//    Serial.println(UART.data.inpVoltage);
//    Serial.println(UART.data.ampHours);
//    Serial.println(UART.data.tachometerAbs);

//  }
//  else
//  {
//    Serial.println("Failed to get data!");
//  }

 if(UART.getVescValues())
 {

  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(0, 24);
  display.print(UART.data.inpVoltage, 1);
  display.println(F("v"));
  display.show();      // Show initial text
 }
 
}
