#include <Encoder.h>
#include <PIDController.h>
//Pin Constants
#define motorDirPin 9
#define motorSpeed 3
#define encoderA 2
#define encoderB 8
#define homing 7
#define currSen A0
#define brk 10

#include "pitches.h"

//Tones for buzzer
int melody[] = {
  NOTE_C4, NOTE_C5, NOTE_C6, NOTE_A6
};

int end_melody[] = {
  NOTE_C4, NOTE_A6
};

int test_melody[] = {
  NOTE_C3, NOTE_A3, NOTE_C4, NOTE_A4, NOTE_C5
};

int noteDurations[] = {
  4, 6, 4, 4
};

int noteDurations2[] = {
  4, 6,
};

int noteDurations3[] = {
  4, 9, 4, 7, 4,
};


//Speed Control
int motor_value = 127;


//Tempreture module
const int temp = A5;
float tempc;
float vout;


//Current sensing
const double currentFactor = 2 / 3.3; // 3.3V per 2A which is 0.909

char incomingByte;
unsigned int integerValue = 0;


//Clockwise Or Counter Clockwise
const int CW  = 1;
const int CCW = 0;

volatile long int encoder_pos = 0;

PIDController pos_pid;

void setup()
{

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(4, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(4);
  }
  //Start Serial monitor
  Serial.begin(9600);
  //Motor Inputs
  pinMode(encoderA, INPUT);
  pinMode(encoderB, INPUT);

  pinMode(temp, INPUT);

  //homing button
  pinMode(homing, INPUT);
  digitalWrite(homing, HIGH);

  //motorSetup Handles the MOTOR I/O
  motorSetup();
  
  //Interrupt Service Routine (ISR) 
  attachInterrupt(digitalPinToInterrupt(encoderA), encoder, RISING);

  pos_pid.begin();
  pos_pid.tune(21, 0, 1000); // Kp,Ki,Kd
  //Speed Control
  pos_pid.limit(-motor_value, motor_value);
}


void loop()
{

  if (Serial.available() > 0)
  {
    integerValue = 0;
    while (1)
    {
      incomingByte = Serial.read();
      if (incomingByte == '\n') break;
      if (incomingByte == -1) continue;
      integerValue *= 10;
      integerValue = ((incomingByte - 48) + integerValue);
      pos_pid.setpoint(integerValue);
    }
  }
  motor_value = pos_pid.compute(encoder_pos);


  if (motor_value > 0)
  {
    if (digitalRead(homing) == LOW)
    {
      homingPro(0);    
    }
    //motorMove is the method that handles the motors movement.
    //Requires direction (CW or CCW)and speed paramater (Entered in Serial Monitor)
    motorMove(CW, motor_value);
    if (tempc >= 40)
    {
      Serial.println(" Tempreture High! ");
      brake(1);
      delay(5000);
    }
    else
    {
      brake(0);
    }
  }
  else
  {
    if (digitalRead(homing) == LOW)
    {
      homingPro(0);
    }
    motorMove(CCW, abs(motor_value));
    if (tempc >= 40)
    {
      Serial.println(" Tempreture High! ");
      brake(1);
      delay(5000);
    }
    else
    {
      brake(0);
    }
  }
}

void motorMove( int dir, int speed)
{
  int motor;
  int motorPinSpeed;

  motor = motorDirPin;
  motorPinSpeed = motorSpeed;

  if (speed > 0)
  {
    digitalWrite(motor, dir);// direction
    analogWrite(motorPinSpeed, speed);// speed
  }
  else
  {
    digitalWrite(motor, LOW);
    analogWrite(motorPinSpeed, speed);
  }
  tempreture();
  currentSen();

  Serial.print(" ");
  Serial.print("Encoder Postition ");
  Serial.print("\t");
  Serial.print(encoder_pos);
}

void motorSetup()
{
  //Motor Outputs
  pinMode(motorDirPin, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(brk, OUTPUT);
  
  digitalWrite(motorDirPin, LOW);
  digitalWrite(motorSpeed, LOW);
}

//Encoder Routine
void encoder()
{
  if (digitalRead(encoderB) == HIGH)
  {
    encoder_pos--;
  }
  else
  {
    encoder_pos++;
  }
}

void tempreture()
{
  vout = analogRead(temp);
  vout = (vout * 500) / 1023;
  tempc = vout; //Degree Celsius
  Serial.print(" ");
  Serial.print(" | Tempreture(°C)");
  Serial.print("\t");
  Serial.println(tempc);
  Serial.print(" ");
}

void currentSen()
{
  double currentVA = map(analogRead(currSen), 0, 1024, 0, 5000) / 1000;
  double currentA = currentVA * currentFactor; // get channel A current from voltage and current factor (from data sheet)

  Serial.print(" ");
  Serial.print(" | Current");
  Serial.print("\t");
  Serial.println(currentA);
  Serial.print(" ");
}

void homingPro(int speed)
{
  int m;
  m = motorSpeed;
  for (int thisNote = 0; thisNote < 2; thisNote++)
  {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations2[thisNote];
    tone(4, end_melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(4);
  }
  motorMove(CW, 0);
  encoder_pos = 0;
  analogWrite(m, 0);
  Serial.print(" | Homing Procedure");
}

void brake(int brk_int)
{
  digitalWrite(brk, brk_int);// brake

}

//void motorTest(int dir, int speed)
//{
//  int motort;
//  int motorPinSpeedt;
//  int motordel = 500;
//  int motordel2 = 2000;
//  int mMax = 127;
//  int mMin = 127;
//  int cycle = 0;
//
//  //    for (int thisNote = 0; thisNote < 5; thisNote++)
//  //  {
//  //    // to calculate the note duration, take one second divided by the note type.
//  //    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//  //    int noteDuration = 1000 / noteDurations3[thisNote];
//  //    tone(4, test_melody[thisNote], noteDuration);
//  //
//  //    // to distinguish the notes, set a minimum time between them.
//  //    // the note's duration + 30% seems to work well:
//  //    int pauseBetweenNotes = noteDuration * 1.30;
//  //    delay(pauseBetweenNotes);
//  //    // stop the tone playing:
//  //    noTone(4);
//  //  }
//  int motor;
//  int motorPinSpeed;
//
//  motor = motorDirPin;
//  motorPinSpeed = motorSpeed;
//
//
//
//  if (speed > 0)
//  {
//    digitalWrite(motor, dir);// direction
//    analogWrite(motorPinSpeed, speed);// speed
//  }
//  else
//  {
//    digitalWrite(motor, LOW);
//    analogWrite(motorPinSpeed, speed);
//  }
//  tempreture();
//  currentSen();
//
//  Serial.print(" ");
//  Serial.print("Encoder Postition ");
//  Serial.print("\t");
//  Serial.print(encoder_pos);
//  //delay(10);
//}
