//Include Wire Communication Library
#include <Wire.h>

//Input Parameters
const int stabilityDelay = 5;
const int collectedSamples = 10;
const boolean serialDisplay = false;

//Declare Pin Numbers
const int SDAPin = A4;
const int SCLPin = A5;
const int LED_RED = 2;
const int LED_IR = 3;
const int LED_Display = 4;
const int detectorPin = A0;

//Declare Variables for Calculation
float redAC = 0;
float redDC = 1024;
float irAC = 0;
float irDC = 1024;
int count = 0;
float rVal;
float SpO2;

void setup() {
  Serial.begin(9600); //Start Serial Communication with 9600 Baud Rate
  analogReference(INTERNAL); //Sets Reference to 1.1V
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_IR, OUTPUT);
  pinMode(LED_Display, OUTPUT); //Sets LED_RED, LED_IR, and LED_Display as an output pin
  pinMode(detectorPin, INPUT); //Sets detectorPin as an input pin
}

void loop() {
  float redVal = readAbsorbance("Red"); //Store absorbance voltage for red LED in "redVal"
  float irVal = readAbsorbance("IR"); //Store absorbance voltage for IR LED in "irVal"

  if(SpO2 > 80)
    digitalWrite(LED_Display, LOW); //Turns off LED if SpO2 is greater than 80%
  
  if(redVal > redAC)
    redAC = redVal; //Makes redAC the max value for the red LED

  if(redVal < redDC)
    redDC = redVal; //Makes redDC the min value for the red LED

  if(SpO2 < 90)
    digitalWrite(LED_Display, HIGH); //Turns on LED if SpO2 is less than 90%. LED will flash if between 80-90%, stay off if over 90%, and stay on if under 80%
  
  if(irVal > irAC)
    irAC = irVal; //Makes irAC the max value for the IR LED

  if(irVal < irDC)
    irDC = irVal; //Makes irDC the min value for the IR LED

  count++; //Increments the count variable

  if(count >= collectedSamples)
  {
    rVal = (redAC / redDC) / (irAC / irDC); //Calculates R for the data
    SpO2 = rVal * (-1.0/3.0) + (3.4 / 3.0); //Calculates SpO2 for the data
    
    //Reset all variables
    count = 0;
    redAC = 0;
    redDC = 1024;
    irAC = 0;
    irDC = 1024;

    if(serialDisplay) //Checks display boolean
    {
      Serial.print(rVal);
      Serial.print(",");
      Serial.println(SpO2); //Prints rVal and Sp02 to the serial monitor/plotter
    }
  }
}

float readAbsorbance(String pin)
{
  if (pin == "Red")
  {
    digitalWrite(LED_IR, LOW);
    digitalWrite(LED_RED, HIGH);
  }
  else
  {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_IR, HIGH);
  }

  delay(stabilityDelay);

  return analogRead(detectorPin);
}
