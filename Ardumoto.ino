#define CW  0
#define CCW 1
#define MOTOR_A 0

const byte PWMA = 3;  // PWM control (speed) for motor A
const byte DIRA = 12; // Direction control for motor A

void setup()
{
  Serial.begin(9600);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
    // All pins should be setup as outputs:
  pinMode(PWMA, OUTPUT);
  pinMode(DIRA, OUTPUT);

  // Initialize all pins as low:
  digitalWrite(PWMA, LOW);
  digitalWrite(DIRA, LOW);
  
}

void loop()
{
  int senValue = digitalRead(7);
    // Drive motor A (and only motor A) at various speeds, then stop.
  driveArdumoto(MOTOR_A, CCW, 200); // Set motor A to CCW at max
  delay(1000);  // Motor A will spin as set for 1 second
  driveArdumoto(MOTOR_A, CCW, 100);  // Set motor A to CW at half
  delay(1000);  // Motor A will keep trucking for 1 second 
  stopArdumoto(MOTOR_A);  // STOP motor A 
  Serial.println(senValue);
}

// driveArdumoto drives 'motor' in 'dir' direction at 'spd' speed
void driveArdumoto(byte motor, byte dir, byte spd)
{
  if (motor == MOTOR_A)
  {
    digitalWrite(DIRA, dir);
    analogWrite(PWMA, spd);
  }
}

void stopArdumoto(byte motor)
{
  driveArdumoto(motor, 0, 0);
}
