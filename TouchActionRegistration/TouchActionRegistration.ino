// UTouch_Calibration 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program can be used to calibrate the touchscreen
// of the display modules.
// This program requires the UTFT library and a touch
// screen module that is compatible with UTFT.
//
// It is assumed that the display module is connected to an
// appropriate shield or that you know how to change the pin 
// numbers in the setup.
//
// Instructions will be given on the display.
//

#include <UTFT.h>
#include <UTouch.h>

// Define the orientation of the touch screen. Further 
// information can be found in the instructions.
#define TOUCH_ORIENTATION  PORTRAIT


class touchList{
  int maxElems, currentElems;
  int coordinateList[][2];
  public:
    touchList();
    void Add(int xCor, int yCor);
};

touchList::touchList(){
  currentElems = 0;
  maxElems = 20;
}

void touchList::Add(int xCor, int yCor){
  if(currentElems<maxElems){ // If list is not completely full
    coordinateList[currentElems][0] = xCor;
    coordinateList[currentElems][1] = yCor;
    currentElems++;
  }
  else{ // If list is already filled
    // Remove first element and rotate
    for(int i =0; i<maxElems;i++){
      coordinateList[i][0] = coordinateList[i+1][0];
      coordinateList[i][1] = coordinateList[i+1][1];   
    }
    // Update Last element
    coordinateList[maxElems][0] = xCor;
    coordinateList[maxElems][1] = yCor; 
  }
}

touchList tList;
// Initialize display
// Remember to change the model parameter to suit your display module!
UTFT    myGLCD(ILI9481,38,39,40,41);

// Initialize touchscreen
UTouch  myTouch( 6, 5, 4, 3, 2);

// Declare which fonts we will be using
extern uint8_t SmallFont[];

uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center;
uint32_t calx, caly, cals;
char buf[13];

void setup()
{
  Serial.begin(9600);
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);

  myTouch.InitTouch(TOUCH_ORIENTATION);
  //myTouch.setPrecision(PREC_EXTREME);
  myTouch.setPrecision(PREC_CUSTOM);
}

void loop()
{
  // Always process the touches
 touchProcessing();
 touchRegistration();

 // if button is pressed
 
  
}

void touchRegistration(){
  // Touch types
  // Tap: Press + Release
  // Swipe: Left of Right
  
}

void touchProcessing(){
  if(myTouch.dataAvailable()){
  // Place new data in list of pressed coordinates
  int xCor;
  int yCor;
  bool success = getTouchCor(&xCor, &yCor);
  //if(success){
    myGLCD.fillCircle(xCor,yCor,3);
    char buffer[25];
    sprintf(buffer,"xCor: %i, yCor: %i \n",xCor,yCor);
    Serial.println(buffer);
    //tList.Add(xCor,yCor);
    //Serial.print("Test");
  //}
 }
}

bool getTouchCor(int *xCor, int *yCor){
  myTouch.read();
  int xRAW = myTouch.TP_X;
  int yRAW = myTouch.TP_Y;
  if((xRAW>-1)&&(yRAW>-1)){
    int xA = 227;
    int xB = 3842;
    int xC = 256;
    int xD = 3810;
    
    int yA = 3671;
    int yB = 3618;
    int yC = 755;
    int yD = 842;
    
    double b = (double) 480;
    double h = (double) 320;
    
    double xR = (double) xRAW;
    double yR = (double) yRAW;
  
    double x1 = (xR-xA)/(xB-xA)*b;
    double x2 = (xR-xC)/(xD-xC)*b;
  
    double y1 = (yA-yR)/(yA-yC)*h;
    double y2 = (yB-yR)/(yB-yD)*h;
  
    double x0 = (b*h*x1+b*(x2-x1)*y1)/(b*h-(x2-x1)*(y2-y1));
    double y0 = (b*h*y1+h*(y2-y1)*x1)/(b*h-(y2-y1)*(x2-x1));
  
    *xCor = x0;
    *yCor = y0;
    return true;
  }
  return false;
  
}

