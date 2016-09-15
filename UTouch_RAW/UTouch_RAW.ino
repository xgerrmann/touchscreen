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
#include <stdio.h>

// Define the orientation of the touch screen. Further 
// information can be found in the instructions.
#define TOUCH_ORIENTATION  PORTRAIT

// Initialize display
// ------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : <display model>,19,18,17,16
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Teensy 3.x TFT Test Board                   : <display model>,23,22, 3, 4
// ElecHouse TFT LCD/SD Shield for Arduino Due : <display model>,22,23,31,33
//
// Remember to change the model parameter to suit your display module!
UTFT    myGLCD(ILI9481,38,39,40,41);

// Initialize touchscreen
// ----------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : 15,10,14, 9, 8
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2
// CTE TFT LCD/SD Shield for Arduino Due       :  6, 5, 4, 3, 2
// Teensy 3.x TFT Test Board                   : 26,31,27,28,29
// ElecHouse TFT LCD/SD Shield for Arduino Due : 25,26,27,29,30
//
UTouch  myTouch( 6, 5, 4, 3, 2);

// ************************************
// DO NOT EDIT ANYTHING BELOW THIS LINE
// ************************************

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
  myTouch.setPrecision(PREC_CUSTOM);
  //myTouch.setPrecision(PREC_EXTREME);
  dispx=myGLCD.getDisplayXSize();
  dispy=myGLCD.getDisplayYSize();
  int MAX_OUT_CHARS = 50;
  char   buffer[MAX_OUT_CHARS + 1]; 
  sprintf(buffer,"Display width: %d \n Display height: %d \n",dispx,dispy);
  Serial.print(buffer);
  text_y_center=(dispy/2)-6;

  myGLCD.drawPixel(0,0);
  myGLCD.drawPixel(475,0);
  myGLCD.drawPixel(0,320);
  myGLCD.drawPixel(475,318);
}

void drawCrossHair(int x, int y)
{
  myGLCD.drawRect(x-10, y-10, x+10, y+10);
  myGLCD.drawLine(x-5, y, x+5, y);
  myGLCD.drawLine(x, y-5, x, y+5);
}



void loop()
{
 if(myTouch.dataAvailable()){
  //myGLCD.clrScr();
  myTouch.read();
  int xRAW = myTouch.TP_X;
  int yRAW = myTouch.TP_Y;
  //int x = myTouch.getX();
  //int y = myTouch.getY();
  myGLCD.print("xRAW",0,0);
  myGLCD.print("yRAW",0,30);
  myGLCD.print("x1",0,60);
  myGLCD.print("x2",0,90);
  myGLCD.print("x",0,120);
  myGLCD.print("y1",0,150);
  myGLCD.print("y2",0,180);
  myGLCD.print("y",0,210);

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

  myGLCD.printNumF(xR,2,100,0);
  myGLCD.printNumF(yR,2,100,30);
  myGLCD.printNumF(x1,2,100,60);
  myGLCD.printNumF(x2,2,100,90);
  myGLCD.printNumF(x0,2,100,120);
  myGLCD.printNumF(y1,2,100,150);
  myGLCD.printNumF(y2,2,100,180);
  myGLCD.printNumF(y0,2,100,210);

  myGLCD.fillCircle(x0,y0,3);
 }
 
  
}
