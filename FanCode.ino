#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 8, 5, 4, 3, 2);
int tempPin = A0;   // the output pin of LM35
int fan = 11;       // the pin where fan is
       // led pin
int temp;
int tempMin = 20;   // the temperature to start the fan
int tempMax = 40;   // the maximum temperature when fan is at 100%
int fanSpeed;
int fanLCD;

int temperature;


void setup() {
  Serial.begin(9600);
  pinMode(fan, OUTPUT);
  pinMode(tempPin, INPUT);
  lcd.begin(16,2);  
}

void loop()
{ 
  temp = analogRead(tempPin);
  temperature = (temp - 100);
  temperature = (temperature / 2.05);
  // get the temperature
   if(temperature  < tempMin) 
   { // if temp is lower than minimum temp 
      fanSpeed = 0; // fan is not spinning 
      digitalWrite(fan, LOW); 
     //Serial.println(temp);
   } 
   if((temperature  >= tempMin) && (temperature <= tempMax)) { // if temperature is higher than minimum temp 
      fanSpeed = map(temperature, tempMin, tempMax, 32, 255); // the actual speed of fan 
      fanLCD = map(temperature, tempMin, tempMax, 0, 100); // speed of fan to display on LCD 
      analogWrite(fan, fanSpeed); // spin the fan at the fanSpeed speed 
   } 
   
   lcd.print("TEMP: ");
   lcd.print(temperature);      // display the temperature
   lcd.print("C ");
   lcd.setCursor(0,1);   // move cursor to next line
   lcd.print("FANS: ");
   lcd.print(fanLCD);    // display the fan speed
   lcd.print("%");
   delay(200);
   lcd.clear();

}
