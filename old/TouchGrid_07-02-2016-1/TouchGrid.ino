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
#define NUMBERofPERSONS 15 //must be equal to number of persons in array
#define PEOPLEonPAGE 9
#define NUMBERofMENU 3
int PERSONBLOCKS = (int) ceil((double)NUMBERofPERSONS/PEOPLEonPAGE)*((double)PEOPLEonPAGE); // equal to the amount of pages neccesary  * max people per pages

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
  randomSeed(analogRead(0));
    
  String personStrings[] = {
  "Harrie", 
  "OPT", 
  "Twan", 
  "Knor", 
  "Trucker", 
  "Tess", 
  "Twilight", 
  "Sopje", 
  "Kotser", 
  "Bambi",
  "Daggie",
  "Marina",
  "Zapp",
  "Jepsen", 
  "Arnold"};

  String menuStrings[] = {
  "Bier", 
  "Rood", 
  "Wit", 
  "Next"};
  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(OCR_A_Extended_M);

  // Setup touch screen (digitizer)
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_CUSTOM);

  
  menu = new block*[NUMBERofMENU];
  for(int i=0; i<NUMBERofMENU;i++){
      menu[i] = new block(myGLCD);
  }
 
  Serial.print(String(PERSONBLOCKS)+"\n");
  persons = new person*[PERSONBLOCKS];
  for(int i=0; i<PERSONBLOCKS;i++){
      persons[i] = new person(myGLCD);
  }
  
  // Draw the grid of blocks
  myGLCD.fillScr(RGB_WHITE);

  int blockwidth = 156;
  int xspace = 4;
  int blockheight = 77;
  int yspace = 4;

  for (int i = 0; i < NUMBERofMENU; i++) {
    int screen = -1; // both actually
    String txt = menuStrings[i];
    menu[i]->setText(txt,"");
    int col = i;
    int row = 0;
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    menu[i]->setCoordinates(xcorL, ycorL, xcorR, ycorR);
    menu[i]->setActiveColor(RGB_BLUE);
    menu[i]->setPassiveColor(RGB_GRAY);
    if(i==0){ //set the first menubutton as active
      menu[i]->changeState();
    }
  }

  for (int i = 0; i < PERSONBLOCKS; i++) {
    Serial.print(i);Serial.print(" - ");
    Serial.print(NUMBERofPERSONS); Serial.print("\n");
    if(i<NUMBERofPERSONS){ // for the people
      String txt = personStrings[i];
      persons[i]->setText(txt,"");
      persons[i]->setName(txt);
      persons[i]->setClickable(true);
    }
    else{
      persons[i]->setText("","");
      persons[i]->setName("");
      persons[i]->setClickable(false);
    }
    int personMod = i%PEOPLEonPAGE; 
    int col = personMod % 3; //columns are zero based
    int row = 1+(personMod-personMod%3)/3; // rows are zero based
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    persons[i]->setCoordinates(xcorL, ycorL, xcorR, ycorR);
    persons[i]->reset();
    int screen = 1+floor(((double)(i-personMod))/((double) PEOPLEonPAGE));
    persons[i]->setScreen(screen);
    persons[i]->setActiveColor(RGB_GREEN);
    persons[i]->setPassiveColor(RGB_GRAY);
  }
  drawMenu();
  drawScreen(CS);
}

void loop()
{
  //myGLCD.clrScr();
  touchAction action;
  action = touchEngine();
  if(action.isSwipe()){
      CS = CS%2+1;
      drawScreen(CS);
  }
  else{
  int16_t x,y;
  action.getCoordinates(&x, &y);
  int button = coor2Button(x, y);
  if(button !=-1){
    if(button==0||button==1||button==2){
      // Find the currently pressed button
      int activeButton=-1;
      for(int i=0; i<3;i++){
        if(menu[i]->getState()){
          activeButton = i;
        }
      }
      // Change the states of the buttons 
      if(!menu[button]->getState()){// If the button was not already pressed
        menu[button]->changeState();  
        menu[activeButton]->changeState();  
        drawMenu();
      }
      resetState();      
    }
    else{
      if(CS==2){
        button = button + 9;
      }
      button = button - 3;
      if(persons[button]->isClickable()){ //only if the person button contains a person
        persons[button]->turf();
        persons[button]->Tap();
      }
    }

  }
 
  }
}

int coor2Button(int xCor, int yCor){
  int blockWidth = 480/3;
  int blockHeight = 320/4;
  double margin = 0.25;
  int xMargin = (int) round(margin*blockWidth);
  int yMargin = (int) round(margin*blockHeight);
  int column  = (xCor-xCor%blockWidth)/blockWidth+1;
  int row     = (yCor-yCor%blockHeight)/blockHeight+1;
  int Xpadding = xCor%blockWidth;
  int Ypadding = yCor%blockHeight;
  Serial.print("xMargin: ");
  Serial.print(xMargin);
  Serial.print("\n");
  
  int Xmax = blockWidth-xMargin;
  int Xmin = xMargin;
  int Ymax = blockHeight-yMargin;
  int Ymin = yMargin;

  int button = -1; // default
  if((Xpadding>Xmin)&&(Xpadding<Xmax)&&(Ypadding>Ymin)&&(Ypadding<Ymax)){
    button = (row-1)*3+column-1;
  }
  return button;
}

void drawScreen(int screen){
  for(int i=0; i<PERSONBLOCKS; i++){
    if(persons[i]->onScreen(screen)){
      persons[i]->Draw();  
    }
  }
}

void drawMenu(){
  for(int i=0; i<NUMBERofMENU; i++){
    menu[i]->Draw();
  }
}

void resetState(){
  // Reset the counters of the people and redraw the screen
  for(int p =0; p<NUMBERofPERSONS;p++){
    persons[p]->reset();
  }
  drawScreen(CS);
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
  //myGLCD.printNumI(xdev,0,0);
  int swipeThreshold = 30;
  if(xdev>swipeThreshold){
    // Swipe has been detected, calculate direction
    
    action.setSwipe(true);
    int midIndex = round(counter/2);
    // nubmer of points must be large enough
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

