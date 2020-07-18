//USE this file for safety as this is manual mode only 
#include<Stepper.h>
#include <HX711.h>

#define DOUT  A0
#define CLK  A1

HX711 loadCell;//(A0, A1);

float  scalefactor = -882.86; ///for big load cell

int switch1 = 12; //down or up button
int switch2 = 11; //down or p button
int switch3 =10; //speed change switch

///motor basic step units
const int stepsPerRevolution = 200  ;// number of steps of motor 
const float GEAR_RATIO = 20; // Gear ratio
const float STEPS_PER_OUTREV = stepsPerRevolution * GEAR_RATIO ;/// total steps for one rev = steps of motor in one rev * gear ratio / number of steps for each (this needs to be changed) 
int stepsrequired; /// using this as a constant instead 
int rpm = 500;


//variables
String inputString;
bool debug = false ;
byte mode =0;
byte modeAddition =0 ;

//motor pins 
Stepper myStepper(stepsPerRevolution, 2, 3);/////
//int dirpin = 3;
//int steppin = 2;

void setup() {
Serial.begin(115200);
    // set the speed at custom rpm:
  myStepper.setSpeed(rpm);///////
  stepsrequired = STEPS_PER_OUTREV;

//all about switches
 pinMode(switch1, INPUT);
 pinMode(switch2, INPUT);
 pinMode(switch3,INPUT);
 
digitalWrite(switch1,HIGH);
digitalWrite(switch2,HIGH);
digitalWrite(switch3,HIGH);
  

//LOADCELL SETUP
loadCell.begin(DOUT, CLK);
 
 loadCell.set_scale();
loadCell.tare();
loadCell.set_scale(scalefactor); 

}

void loop() {
 
          //digitalWrite(dirpin,HIGH);
       //myStepper.step(-stepsrequired); ///negative sign for opposite direction
  //delayMicroseconds(2000);
  
  if (!digitalRead(switch1)){
    mode =4;}
   else if (!digitalRead(switch2)){
    mode =4;}
 
    ///////////////// MANUAL MODE ///////////////
 if (mode ==4) {
             //boolean performStep = 0;

if (!digitalRead(switch3))
     {
       stepsrequired = 200;     
    if (!digitalRead(switch1))
    {
       myStepper.step(stepsrequired); ///step function counts steps for steps required
  delayMicroseconds(100);
    }  
    if(!digitalRead(switch2))
    {
 myStepper.step(-stepsrequired); ///step function counts steps for steps required
  delayMicroseconds(100);
    }
    }
    else if (digitalRead(switch3))
    {
     stepsrequired = 20;     
    if (!digitalRead(switch1))
    {
       myStepper.step(stepsrequired); ///step function counts steps for steps required
  delayMicroseconds(100);
    }  
    if(!digitalRead(switch2))
    {
 myStepper.step(-stepsrequired); ///step function counts steps for steps required
  delayMicroseconds(100);
    }
    }
  }
  

  //LOAD
float rawvalue = loadCell.get_units();
float W = round(rawvalue);////((rawvalue/5)*10); //for big load cell
loadCell.set_scale(scalefactor);


  //Serial.print("load :  ");
  Serial.println(W);
}

    
     
     
