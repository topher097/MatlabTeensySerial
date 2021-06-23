#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define BAUDRATE    115200    // Baudrate of serial connection
#define IN_LED      15        // Pin of data in LED
#define OUT_LED     14        // Pin of data out LED
#define SEND_BUTTON 12        // Pin of send button

LiquidCrystal_I2C lcd(0x27,20,4);         // LCD object

String inString;
String outString;
uint32_t inTime;
uint32_t outTime;
char terminator = 'CR/LF';

void updateLCD(){
  lcd.setCursor(0, 0);
  lcd.print("Data out: " + (String)outTime + " ms");
  lcd.setCursor(0, 1);
  lcd.print(outString);
  lcd.setCursor(0, 2);
  lcd.print("Data in: " + (String)inTime + " ms");
  lcd.setCursor(0, 3);
  lcd.print(inString);
}

void setup() {
  // Setup the serial port (USB/Serial0)
  Serial.begin(BAUDRATE);

  // Setup the LCD screen
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial.available() > 0){
    inString = Serial.readStringUntil(terminator);
  }
}