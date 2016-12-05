#include <DFPlayer_Mini_Mp3.h>

#include <SPI.h>

#include <SoftwareSerial.h>

#include <Servo.h>

Servo motor;

const int xpin = A1;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A3;                  // z-axis
const int motionSensor = 2;
const int reedSwitch = 4;
int xaccel;
int yaccel;
int zaccel;
int xval;
int yval;
int zval;
int switchVal = 0;
int motionVal = 0;
int flag = 1; //software flag to keep track of lid position: 1 is open, 0 is closed
int i;
int x, y, z;
int swtichVal;


void setup() {
  pinMode(reedSwitch, INPUT);
  pinMode(motionSensor, INPUT);
  motor.attach(9);
  motor.write(115);
  Serial.begin (9600);
  mp3_set_serial (Serial);      //set Serial for DFPlayer-mini mp3 module
  delay(10);

  SPI.begin();

  mp3_set_volume (30); // value 0~30
  delay(5000); //so we can unplug usb before it starts snapping closed
}

void loop() {
  switchVal = 0;
  for ( i = 0; i < 15; i++) { //sum values of motionVal sensor over a time period to avoid false positives
    switchVal = switchVal + digitalRead(reedSwitch);
    Serial.print("switchVal: ");
    Serial.println(switchVal);
    delay(15);
  }
  if (switchVal == 15) {
    motor.write(115); //open lid
    mp3_play(1);       //'Enjoy your cookies'.
    delay(10000);   //keep lid open for 10 seconds so cookie can be obtained without holding the magnet in place
  }
  else  { //system NOT deactivated

    motionVal = 0;
    for ( i = 0; i < 20; i++) { //sum values of motionVal sensor over a time period to avoid false positives
      motionVal = motionVal + digitalRead(motionSensor);
      Serial.println(motionVal);
      delay(15);
    }

    if ((motionVal < 10) && (flag == 1)) {   //if movement and lid open
      xval = analogRead(xpin);
      yval = analogRead(ypin);
      zval = analogRead(zpin);
      Serial.print("x, y, z: ");
      Serial.println(xval);
      Serial.println(yval);
      Serial.println(zval);
      xaccel = 0;
      yaccel = 0;
      zaccel = 0;
      if ((xval > 340) || (xval < 320)) {
        xaccel = 1;
      }
      if ((yval > 280) || (yval < 255)) {
        yaccel = 1;
      }
      if ((zval > 360) || (zval < 330)) {
        zaccel = 1;
      }
      if ((xaccel == 1) || (yaccel == 1) || (zaccel == 1)) { //if box is moved
        motor.write(172);               //close lid
        mp3_play(3);                //play alarm sound
        flag = 0;
        delay(10000);                     //hold closed for 10 seconds
      }
      else {
        motor.write(172);             //close lid
        flag = 0;                     //lid is closed
        mp3_play(2);                  //play 'these aren't your cookies!//
        delay(4000);                   //hold closed for 10 seconds
      }

    }
    else if (flag == 0) {            //lid closed
      motor.write(115);             //open lid
      flag = 1;
      delay(5000);                   //hold open for 4 seconds to ensure movement is finished
    }

    else {
      delay(100);
    }
  }
}

