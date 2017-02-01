// XGGerrmann All Rights Reserved

#include <UTFT.h>
#include <URTouch.h>
#include <stdio.h>
#include <block.h>
#include <person.h>
#include <touchAction.h>
#include <mainScreen.h>
#include <summaryScreen.h>

// Define the orientation of the touch screen. 
#define TOUCH_ORIENTATION  PORTRAIT

// Define color scheme
#define RGB_GRAY    0xBDD7 
#define RGB_WHITE   0xFFFF 
#define RGB_PINK    0xE8EC 
#define RGB_BLUE    0x055E
#define RGB_GREEN   0x9EC7

// Declare which fonts we will be using
//extern uint8_t SmallFont[];
//extern uint8_t BigFont[];
//extern uint8_t Grotesk24x48[];
//extern uint8_t franklingothic_normal[];
extern uint8_t OCR_A_Extended_M[];

#define NUMBERofPEOPLE 15

// Set the pins to the correct ones for your development shield
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(ILI9481 , 38, 39, 40, 41); //ILI9481

// Initialize touchscreen
//UTouch  myTouch( 6, 5, 4, 3, 2);
UTouch  myTouch( 53, 52, 51, 50, 49);

mainScreen*     mainScr     = new mainScreen();
summaryScreen*  summaryScr  = new summaryScreen();

int STATE = 0; // 0-> main, 1-> summary

void setup()
{
  Serial.begin(9600);
  String buf = "Serial Started";
  Serial.println(buf);
  randomSeed(analogRead(0));

  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(OCR_A_Extended_M);

  // Setup touch screen (digitizer)
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_CUSTOM);

  // Draw the grid of blocks
  myGLCD.fillScr(RGB_WHITE);
  mainScr->Initialize(myGLCD);
  summaryScr->Initialize(myGLCD);
 
  mainScr->Draw();
}

void loop()
{
  Serial.println("Loop");
  touchAction action;
  action = touchEngine();
  Serial.println("Touch registered");
  bool showSummary = false;
  bool acceptOrder = false;
   Serial.print("State:");  Serial.println(STATE);
  switch(STATE){
    case 0: showSummary = mainScr->Touch(action); Serial.println("main touch executed"); break;
    case 1: acceptOrder = summaryScr->Touch(action); Serial.println("summary touch executed"); break;
    default: ;break;
  }
  Serial.println(acceptOrder);
  if(showSummary && STATE ==0){
    int orderList[NUMBERofPEOPLE];
    String nameList[NUMBERofPEOPLE];
    int numberOfOrders = mainScr->getOrder(nameList, orderList);
    //Serial.print("Name: ");Serial.println(nameList[0]);
    //Serial.print("Amount: ");Serial.println(orderList[0]);
    summaryScr->Draw(nameList, orderList, numberOfOrders);
    STATE = 1;
  }
  else if(STATE==1){
    if(acceptOrder){
      Serial.println("Order Accepted");
    //order();
    mainScr->resetState();
    }
    else{
       Serial.println("Order Rejected");
       mainScr->resetState();
    }
    // reset all counters
    mainScr->Draw();
    STATE = 0;
  }
}

touchAction touchEngine(){
   touchAction action;
  while(!myTouch.dataAvailable()){
    delay(5);
  }
  int xList[200];
  int yList[200];
  int counter = 0;
  while(myTouch.dataAvailable()&&counter<200){
    //myGLCD.clrScr();
    myTouch.read();
    myTouch.calcCors();
    int x = myTouch.getX();
    int y = myTouch.getY();
    //myGLCD.fillCircle(x,y,2);

    xList[counter] = x;
    yList[counter] = y;
    counter ++;
  }
  //myGLCD.print("NODATA",CENTER,CENTER);
  double xmean = GetAverage(xList, counter-1);// dont use last point
  double xdev  = CalcStdev(xList,xmean,counter-1);
  //myGLCD.printNumI(xdev,0,0);
  int swipeThreshold = 30;
  if(xdev>swipeThreshold){
    // Swipe has been detected, calculate direction
    
    action.setSwipe(true);
    int midIndex = round(counter/2);
    // number of points must be large enough
    if(xList[0]<xList[midIndex]){// Swipe Right
      action.setSwipeDir('R');
      //myGLCD.print("SWIPE right",CENTER,CENTER);
    }
    else{// Swipe Left
      action.setSwipeDir('L');
      //myGLCD.print("SWIPE left",CENTER,CENTER);
    } 
  }
  else{//if no swipe has been detected, calculate coordinates (improve accuracy)
    double xCor, yCor;
    xCor = GetMedian(xList,counter);
    yCor = GetMedian(yList,counter);
    action.setCoordinates(xCor,yCor);
  }
   
  return action;
}

//void showSummary(){
//  // make array of all people that need drinks
//  String people[NUMBERofPERSONS];
//  int orderSize[NUMBERofPERSONS];
//  int orderCount = 0;
//  for(int i=0; i<NUMBERofPERSONS; i++){
//    int amount = persons[i]->getOrder();
//    if(amount > 0){
//      String Name = persons[i]->getName();
//      people[orderCount] = Name;
//      orderSize[orderCount] = amount;
//      orderCount++;
//    }
//  }
//  if(orderCount>0){
//    // Leeg het scherm
//    myGLCD.fillScr(RGB_WHITE);
//    // Teken de menuknoppen
//    menuB[0]->Draw();  
//    menuB[1]->Draw();  
//    // Toon alle bestellingen
//    myGLCD.setColor(VGA_BLACK);
//    myGLCD.setBackColor(VGA_TRANSPARENT);
//    for(int i=0; i<orderCount; i++){
//      String Name = people[i];
//      int amount  = orderSize[i]; 
//      myGLCD.print(Name,60,60+i*25);
//      myGLCD.printNumI(amount,200,60+i*25);      
//    }
//
//   }
//  else{
//    myGLCD.setColor(VGA_RED);
//    myGLCD.setBackColor(VGA_WHITE);
//    String st = "Niks besteld";
//    myGLCD.print(st,CENTER,CENTER);
//    //delay(500);
//    myGLCD.fillScr(RGB_WHITE);
//    drawMenu();
//    drawScreen(CS);
//  }
//}

