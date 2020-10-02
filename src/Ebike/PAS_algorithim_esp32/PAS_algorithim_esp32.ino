
//screen setup
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

// end screen setup
// pin setup
//const byte hallPower1 = 2; // hall effect power wire
//const byte hallPower2 = 3; // second hall effect power
const byte throttleSensor = 36; // ADC to read throttle value
//const byte pwmOut = A7; // ADC to send pwm single to motor controller
//const byte ledOut = D5;

// Variables 
bool engageThrottle = false;
int currTV = 0; // current throttle value
int prevTV = 0; // previous throttle value

float rps = 0.f;
float revs = 0.f;
float seconds = 1.f;

int hallCount = 1;
int inactiveCount = 0;

unsigned long prevTime;
unsigned long currTime;
const unsigned long period = 1000;  //the value is a number of milliseconds, ie 1 second

int pwmVal = 0;

//Servo esc;

void updateCurrTV()
{
  prevTV = currTV;
  currTV = analogRead(throttleSensor);
}

void setup() {
 // usb serial setup 
  Serial.begin(115200); // pc output
  Serial1.begin(115200); // vesc input/output

 // throttle setup
  pinMode(throttleSensor, INPUT); //Hall sensor
  prevTime = millis();
  updateCurrTV();

//screen stup
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

 // throttle code
  currTime = millis();  //get the current "time" (actually the number of milliseconds since the program started
  if (currTime - prevTime >= period)  //test whether the period has elapsed
  {
    if(engageThrottle)
    {
      seconds += 1.f;
    }
    prevTime = currTime;
  }

  
  if(prevTV < 500 && currTV > 500)
  {
    hallCount ++;
    inactiveCount = 0;
  }
  else if(prevTV > 500 && currTV < 500)
  {
    hallCount ++;
    inactiveCount = 0;
  }
  else if(engageThrottle)
  {
    inactiveCount++;
  }
  
  if(inactiveCount >= 1000)
  {
    revs = 0.f;
    seconds = 1.f;
    rps = 0.f;
    engageThrottle = false;
  }
  
  if(hallCount == 24)
  {
    revs += 1.f; 
    engageThrottle = true;
    rps = revs/seconds;
    hallCount = 1;
  }
    
   pwmVal = (int)(466.6667*rps + 833.3334);

//  Serial.print("revs: ");
//  Serial.print(revs);
//  Serial.print("   |   ");
//  Serial.print("seconds: ");
//  Serial.print(seconds);
//  Serial.print("   |   ");
//  Serial.print("rps: ");
//  Serial.print(rps);
//  Serial.print("   |   ");
//  Serial.print("i.c: ");
//  Serial.print(inactiveCount);
//  Serial.print("   |   ");
  Serial.print("pwm: ");
 
  if(engageThrottle)
  {
    Serial.println(pwmVal);
    //esc.writeMicroseconds(pwmVal);
  }
  else
  {
    Serial.println("throttle disengaged");
    //esc.writeMicroseconds(1300);
  }
  
  updateCurrTV();

 // end throttle code

 // display code
 if(UART.getVescValues())
 {
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
 
 // end display code
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
