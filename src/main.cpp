#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

#define BAUDRATE    115200    // Baudrate of serial connection
#define IN_LED      14        // Pin of data in LED (green)
#define OUT_LED     15        // Pin of data out LED (red)
#define SEND        12        // Pin of send button

LiquidCrystal_I2C lcd(0x27,20,4);         // LCD object
Bounce sendButton = Bounce();             // Button object

int inTime;
int outTime;
string inString;
string matlab_terminator  = "CR/LF";
char teensy_terminator    = '?';
char delimiter            = ',';
int var1in        = 0;
int var2in        = 0;
int var1out       = 0;
int var2out       = 0;
int count         = 0;
bool firstRun     = true;
bool updateNeeded = false;

void updateLCD(){
  count++;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("Data out: %i ms  %i", outTime, count);
  lcd.setCursor(0, 1);
  lcd.printf("v1: %i ,v2: %i", var1out, var2out);
  lcd.setCursor(0, 2);
  lcd.printf("Data in: %i ms", inTime);
  lcd.setCursor(0, 3);
  //lcd.printf("%s", inString);
  lcd.printf("v1: %i ,v2: %i", var1in, var2in);
  updateNeeded = false;
}

void setup() {
  // Setup the serial port (USB/Serial0)
  Serial.begin(BAUDRATE);

  // Setup the LCD screen
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);

  // Setup button
  sendButton.attach(SEND, INPUT);
  sendButton.interval(25);

  // Setup LED pins
  pinMode(IN_LED, OUTPUT);
  pinMode(OUT_LED, OUTPUT);
  digitalWrite(IN_LED, LOW);
  digitalWrite(OUT_LED, LOW);

  // Display something to LCD
  lcd.setCursor(0, 0);
  lcd.print("Make sure MATLAB is");
  lcd.setCursor(0, 1);
  lcd.print("running and is");
  lcd.setCursor(0, 2);
  lcd.print("connected to Teensy");
}

void loop() {
  // put your main code here, to run repeatedly:
  sendButton.update();
  if (sendButton.fell()){
    int start = millis();
    if (firstRun){
      var1out = rand()%100;
      var2out = rand()%100;
      firstRun = false;
    }
    else{
      var1out = var1in+1;
      var2out = var2in+1;
    }
    digitalWrite(OUT_LED, HIGH);
    delay(10);
    outTime = millis()-start;
    Serial.print(outTime);            Serial.print(delimiter);
    Serial.print(var1out);            Serial.print(delimiter);
    Serial.print(var2out);
    Serial.write(13);   // Carriage return "CR"
    Serial.write(10);   // Linefeed "LF"
    delay(10);
    digitalWrite(OUT_LED, LOW);
  }

  if (Serial.available() > 5){
    digitalWrite(IN_LED, HIGH);
    vector<float> v;
    inString = Serial.readStringUntil(teensy_terminator).c_str();
    //inString = Serial.readString().c_str();
    stringstream ss(inString);
    while (ss.good()){
      string substr;
      float temp;
      getline(ss, substr, ',');
      istringstream ss2(substr);
      ss2 >> temp;
      v.push_back(temp);
    }

    int i = 0;
    inTime  = (int)v[i];  i++;
    var1in  = (int)v[i];  i++;
    var2in  = (int)v[i];
    delay(10);     
    digitalWrite(IN_LED, LOW);
    updateNeeded = true;
  }
  if (updateNeeded){
    updateLCD();
    delay(10);
  }
 
  
}