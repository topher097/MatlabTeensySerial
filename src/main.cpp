#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

#define BAUDRATE    115200    // Baudrate of serial connection
#define IN_LED      15        // Pin of data in LED
#define OUT_LED     14        // Pin of data out LED
#define SEND        12        // Pin of send button

LiquidCrystal_I2C lcd(0x27,20,4);         // LCD object
Bounce sendButton = Bounce();             // Button object

string inString;        // C++ string type
String outString;       // Arduino String type
uint32_t inTime;
uint32_t outTime;
char terminator = 'CR/LF';
char delimiter  = ',';
char buffer[100];

void updateLCD(){
  lcd.setCursor(0, 0);
  lcd.printf("Data out: %i ms", outTime);
  lcd.setCursor(0, 1);
  lcd.print(outString);
  lcd.setCursor(0, 2);
  lcd.printf("Data in: %i ms", inTime);
  lcd.setCursor(0, 3);
  lcd.printf("%s", inString);
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
  sendButton.update();
  if (sendButton.fell()){
    outTime = millis();
    float var1 = rand()*10;
    float var2 = rand()*10;
    outString = sprintf(buffer, "%i,%d,%d%s", outTime, var1, var2, terminator);
    inString = 'NaN';
    Serial.print(outString);
    updateLCD();
  }

  if (Serial.available() > 0){
    vector<float> v;
    inString = Serial.readStringUntil(terminator).c_str();
    stringstream ss(inString);
    while (ss.good()){
      string substr;
      float temp;
      getline(ss, substr, ',');
      istringstream ss2(substr);
      ss2 >> temp;
      v.push_back(temp);
    }

    int i = 1;
    inTime      = (int)v[i];  i++;
    float var1  = v[i];       i++;
    float var2  = v[i];     
    updateLCD();  
  }
}