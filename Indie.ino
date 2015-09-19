/*************************************************** 
  This is an example for the TMP006 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit TMP006 Breakout 
  ----> https://www.adafruit.com/products/1296

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Servo.h>//libraries
#include <Adafruit_Sensor.h>
#include "Adafruit_TMP006.h"

// Connect VCC to +3V (its a quieter supply than the 5V supply on an Arduino
// Gnd -> Gnd
// SCL connects to the I2C clock pin. On newer boards this is labeled with SCL
// otherwise, on the Uno, this is A5 on the Mega it is 21 and on the Leonardo/Micro digital 3
// SDA connects to the I2C data pin. On newer boards this is labeled with SDA
// otherwise, on the Uno, this is A4 on the Mega it is 20 and on the Leonardo/Micro digital 2

Servo motorOne;  //left 
Servo motorTwo;  //right
Servo arm;
char direct;

Adafruit_TMP006 tmp006;
//Adafruit_TMP006 tmp006(0x41);  // start with a diferent i2c address!
int loopCounter = 0;
int lastRealBeat = 0;
int sinceLastBeat = 0;

int prevBeats[3] = {0,0,0};

int pulsePin = 0;  
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = false;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse

int pushPrevBeats(int b) {
  prevBeats[0] = prevBeats[1];
  prevBeats[1] = prevBeats[2];
  prevBeats[2] = b;
}

void setup() { 
  Serial.begin(9600);
  motorOne.attach(6);
  motorTwo.attach(7);
  arm.attach(12);
  
  // you can also use tmp006.begin(TMP006_CFG_1SAMPLE) or 2SAMPLE/4SAMPLE/8SAMPLE to have
  // lower precision, higher rate sampling. default is TMP006_CFG_16SAMPLE which takes
  // 4 seconds per reading (16 samples)
  if (! tmp006.begin()) {
    Serial.println("No sensor found");
    while (1);
  }
  interruptSetup();  
}

void loop() {
  // Check for sleep/wake command.
  loopCounter++;

  if (loopCounter > 399) {
    loopCounter = 0;
  }
  
  if (Serial.available() > 0){ // motor control
    if (Serial.peek() == 'a'){
      Serial.println(Serial.read());
      direct = 1;
    }
    else if (Serial.peek() == 'd'){
      Serial.println(Serial.read());
      direct = 2;
    }
    else if (Serial.peek() == 'w'){
      Serial.println(Serial.read());
      direct = 3;
    }
    else if (Serial.peek() == 's'){
      Serial.println(Serial.read());
      direct = 4;
    }
    else if (Serial.peek() == ' '){
      Serial.println(Serial.read());
      direct = 5;
    }
    else if (Serial.peek() == 'k'){
      Serial.println(Serial.read());
      arm.write(1700);
      delay(150);
      arm.write(1500);
    }
    else if (Serial.peek() == 'l'){
      Serial.println(Serial.read());
      arm.write(1300);
      delay(150);
      arm.write(1500);
    }
  if (direct == 1){//full right
    motorOne.write(2000);//if controls are inverted replace with 1300
    motorTwo.write(2000);
  }
  else if (direct == 2){//full left
    motorOne.write(1000);//if controls are inverted replace with 1700
    motorTwo.write(1000);
  }
  else if (direct == 3){// full back
    motorOne.write(1000);//if controls are inverted switch values
    motorTwo.write(2000);
  }
  else if (direct == 4){// full forwards
    motorOne.write(2000);//if controls are inverted switch values
    motorTwo.write(1000);
  }
  else if (direct == 5){//stop
    motorOne.write(1500);
    motorTwo.write(1500);
  }
  
 }

  if ((loopCounter % 10) == 0){
    sinceLastBeat += 100;
    if (QS == true){ 
      digitalWrite(13,HIGH);
      if (BPM >= 60 && BPM <= 140){
        pushPrevBeats(BPM);
        if ( abs((prevBeats[0] + prevBeats[1] + prevBeats[2]) / 3 - prevBeats[0]) <= 15) {
          lastRealBeat = prevBeats[0];
          sinceLastBeat = 0;
        }
      }
      QS = false;
    } else {
      digitalWrite(13,LOW); 
    }
    if (sinceLastBeat > 1000 && lastRealBeat > 0) {
      sinceLastBeat = 0;
      lastRealBeat = 0;
      prevBeats[0] = 0;
      prevBeats[1] = 0;
      prevBeats[3] = 0;
    }
    if ((loopCounter % 100) == 0){
      Serial.print("B"); Serial.println(lastRealBeat);
    }
  }
  
  if (loopCounter == 0) {
    // Grab temperature measurements and print them.
    float objt = tmp006.readObjTempC();
    Serial.print("T"); Serial.println(objt); 
  }

  delay(10);
}
