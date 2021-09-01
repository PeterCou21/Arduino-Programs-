

#include <PIDController.h>
volatile long int encoder_pos = 0;
PIDController pos_pid; 
int motor_value = 255;
unsigned int integerValue=0;  // Max value is 65535
char incomingByte;


int dira=12;
int speed_a=3;

void setup() {

  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(8, INPUT);
  
  pinMode(dira, OUTPUT);
  pinMode(speed_a, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), encoder, RISING);

  pos_pid.begin();    
  pos_pid.tune(10, 0, 2000);    
  pos_pid.limit(-255, 255);
}

void loop() {

if (Serial.available() > 0)
{  
    integerValue = 0;        
    while(1) 
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
if(motor_value > 0)
{
  MotorCounterClockwise(motor_value);
}
else
{
  MotorClockwise(abs(motor_value));
}
  Serial.println(encoder_pos);
  delay(1000);
}


void encoder(){

  if(digitalRead(8) == HIGH){
    encoder_pos++;
  }else{
    encoder_pos--;
  }
}

void MotorClockwise(int power){
  if(power > 100){
  digitalWrite(dira, HIGH);
  analogWrite(speed_a, power);
  }else{
    digitalWrite(dira, LOW);
  }
}

void MotorCounterClockwise(int power){
  if(power < 100){
  digitalWrite(dira, LOW);
  analogWrite(speed_a, power);
  }else{
    digitalWrite(dira, LOW);

  }
}
