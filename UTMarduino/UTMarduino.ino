
#include<Stepper.h>
#include <HX711.h>

#define DOUT  A0
#define CLK  A1  

HX711 loadCell;//(A0, A1);

float  scalefactor = -882.86;//for big load cell

int switch1 = 12;
int switch2 = 11;
int switch3 =10;

///motor basic step units
int rpm = 500;
const int stepsPerRevolution = 200  ;// number of steps of motor 
const float GEAR_RATIO = 20; // Gear ratio
const float STEPS_PER_OUTREV = stepsPerRevolution * GEAR_RATIO ;/// total steps for one rev = steps of motor in one rev * gear ratio / number of steps for each 
int stepsrequired; /// using this as a constant instead 


//variables
String inputString; 
bool debug = false ;
byte mode =0;
byte modeAddition =0 ;

//motor pins 
Stepper myStepper(stepsPerRevolution, 2, 3);
//int dirpin = 3;
//int steppin = 2;

void setup() {

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
  
Serial.begin(115200);
//LOADCELL SETUP
loadCell.begin(DOUT, CLK);
 
 loadCell.set_scale();
loadCell.tare();
loadCell.set_scale(scalefactor); 
//char a = 'b';
//while(a!='a')
//a = Serial.read();

}

void loop() {
  bool isRead = false;
  //Serial COmmunication
 inputString = "" ;
  while (Serial.available())
  {
    inputString = Serial.readString();
    isRead = true;
  }
  if (isRead) {
    String taskPart;
    String rest;
    taskPart = inputString.substring(0, inputString.indexOf(" "));
    rest = inputString.substring(inputString.indexOf(" ") + 1);
    if (taskPart == "C") {
      
       mode =1;
        loadCell.tare();
        
         }  
    else if (taskPart == "T" )
     {
        mode =2;
         loadCell.tare();
            //digitalWrite(dirpin,HIGH);
  }
  else if (taskPart == "Stop")
  {
  mode = 3;
   loadCell.tare();
  }
  else if (taskPart == "M")
  {
    mode =4;
   loadCell.tare();
  }
    
  }

if (mode ==1) {
    
   myStepper.step(stepsrequired); ///step function counts steps for steps required
  delayMicroseconds(2000);

    if (!digitalRead(switch1)){
    mode =4;}
   else if (!digitalRead(switch2)){
    mode =4;}

}
 if(mode ==2) {
  
    myStepper.step(-stepsrequired); ///negative sign for opposite direction
  delayMicroseconds(2000);
  
  if (!digitalRead(switch1)){
    mode =4;}
   else if (!digitalRead(switch2)){
    mode =4;}
 }
 ///////////// STOP //////////////////////////
  else if(mode==3) {
    boolean performStep =0;
    performStep =1; // machine stops here
  }
  ///////////////// MANUAL MODE ///////////////
  else if (mode ==4) {
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
float W = (rawvalue/5)*10;//(rawvalue); // for big load cell
loadCell.set_scale(scalefactor);


  //Serial.print("load :  ");
  Serial.println(W);
}
    
     
     
