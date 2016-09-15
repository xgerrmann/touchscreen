// XGGerrmann All Rights Reserved

#include <UTFT.h>
#include <UTouch.h>
#include <stdio.h>
#include <block.h>
#include <person.h>
#include <math.h>
#include <touchAction.h>

// Define the orientation of the touch screen. 
#define TOUCH_ORIENTATION  PORTRAIT
#define NUMBERofPERSONS 15 //must be equal to nubm,er of persons in array
#define PEOPLEonPAGE 8
#define NUMBERofMENU 4
int PERSONBLOCKS = (int) ceil((double)NUMBERofPERSONS/PEOPLEonPAGE)*((double)PEOPLEonPAGE);

// Define color scheme
#define RGB_GRAY    0xBDD7 
#define RGB_WHITE   0xFFFF 
#define RGB_PINK    0xE8EC 
#define RGB_BLUE    0x055E
#define RGB_GREEN   0x9EC7


// Declare which fonts we will be using
extern uint8_t SmallFont[];
//extern uint8_t BigFont[];
//extern uint8_t Grotesk24x48[];
//extern uint8_t franklingothic_normal[];
//extern uint8_t OCR_A_Extended_M[];

// Set the pins to the correct ones for your development shield
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(ILI9481 , 38, 39, 40, 41); 

// Initialize touchscreen
UTouch  myTouch( 6, 5, 4, 3, 2);

// global variables
//block blocks[15]; // maakt 15 blocks van de class block
person **persons;
block **menu;

int CS = 1; // CS = current screen (the screen that is shown)
void setup()
{
  Serial.begin(9600);

  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);

  // Setup touch screen (digitizer)
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_CUSTOM);
}

void loop()
{
  myGLCD.clrScr();
  touchAction action;
  action = touchEngine();
  
 
}

double  GetAverage(int daArray[], int iSize){
  double sum = 0;
  for(int i =0; i<iSize;i++){
    sum = sum + daArray[i];
  }
  double mean = (double)sum/iSize;
  return mean;
}

double  GetMedian(int daArray[], int iSize){
  // Allocate an array of the same size and sort it.
    double* dpSorted = new double[iSize];
    // Copy values
    for (int i = 0; i < iSize; ++i) {
        dpSorted[i] = daArray[i];
    }
    // Sort  values
    for (int i = iSize - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (dpSorted[j] > dpSorted[j+1]) {
                double dTemp = dpSorted[j];
                dpSorted[j] = dpSorted[j+1];
                dpSorted[j+1] = dTemp;
            }
        }
    }

    // Middle or average of middle values in the sorted array.
    double dMedian = 0.0;
    if ((iSize % 2) == 0) {
        dMedian = (dpSorted[iSize/2] + dpSorted[(iSize/2) - 1])/2.0;
    } else {
        dMedian = dpSorted[iSize/2];
    }
    delete [] dpSorted;
    return dMedian;
}

double  CalcStdev(int daArray[], double mean, int iSize){
  double sum = 0;
  double val = 0;
  for(int i =0; i<iSize;i++){
    val = (double)daArray[i]-mean;
    sum = sum + val*val;
  }
  double stdev = sqrt(sum); //(1/(iSize+1))*sqrt(sum)
  return stdev;
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
  myGLCD.printNumI(xdev,0,0);
  int swipeThreshold = 30;
  if(xdev>swipeThreshold){
    // Swipe has been detected, calculate direction
    
    action.setSwipe(true);
    int midIndex = round(counter/2);
    // nubmer of points must be large enough
    if(xList[0]<xList[midIndex]){// Swipe Right
      action.setSwipeDir('R');
      myGLCD.print("SWIPE right",CENTER,CENTER);
    }
    else{// Swipe Left
      action.setSwipeDir('L');
      myGLCD.print("SWIPE left",CENTER,CENTER);
    } 
  }
  else{//if no swipe has been detected, calculate coordinates (improve accuracy)
    double xCor, yCor;
    xCor = GetMedian(xList,counter);
    action.setCoordinates(xCor,yCor);
  }
   
  return action;
}


