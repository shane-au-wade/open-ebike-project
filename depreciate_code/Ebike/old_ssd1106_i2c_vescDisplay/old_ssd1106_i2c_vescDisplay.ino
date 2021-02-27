#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <VescUart.h>
VescUart UART;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

void setup() {
  Serial.begin(9600); // pc output
  Serial1.begin(115200);
  while (!Serial1) {;}
  UART.setSerialPort(&Serial1);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
 // Clear the buffer
  display.clearDisplay();
  
  // Show initial display buffer contents on the screen --
  
  testdrawroundrect(); // Draw rounded rectangles (outlines)
  testfillroundrect(); // Draw rounded rectangles (filled)
  
  
  display.display();
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

 UART.getVescValues();
 
  display.clearDisplay();
 // display.setTextSize(2); // Draw 2X-scale text
 
  display.setRotation(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  
//  display.println(F("Battery:"));

display.drawBitmap(
    0,
    0,
    bike_bmp, BIKE_WIDTH, BIKE_HEIGHT, 1);
    
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(0, 24);
  
  display.println(UART.data.inpVoltage, 1);
  
  //display.setCursor(24, 50);
  display.println(F("v"));
  display.display();      // Show initial text
  
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(500);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
  delay(500);
}





  
 
