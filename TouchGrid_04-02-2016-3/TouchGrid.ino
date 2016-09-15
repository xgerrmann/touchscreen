// XGGerrmann All Rights Reserved

#include <UTFT.h>
#include <UTouch.h>
#include <stdio.h>
#include <block.h>
#include <person.h>
#include <math.h>

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
    int col;
    int row;
    if(i<3){row = 0; col = i % 3;} else{row = 3; col = 2;} 
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    menu[i]->setCoordinates(xcorL, ycorL, xcorR, ycorR);
    menu[i]->setActiveColor(RGB_BLUE);
    menu[i]->setPassiveColor(RGB_GRAY);
    if(i==0){
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
    int personMod = i%8; 
    int col = personMod % 3; //columns are zero based
    int row = 1+(personMod-personMod%3)/3; // rows are zero based
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    persons[i]->setCoordinates(xcorL, ycorL, xcorR, ycorR);
    persons[i]->reset();
    int screen = 1+(int) ((double)(i-personMod))/((double)8);
    persons[i]->setScreen(screen);
    persons[i]->setActiveColor(RGB_GREEN);
    persons[i]->setPassiveColor(RGB_GRAY);
  }
  

  drawMenu();
  drawScreen(CS);
}

void loop()
{
  if(myTouch.dataAvailable()){
  //myGLCD.clrScr();
  myTouch.read();
  myTouch.calcCors();
  int16_t x = myTouch.getX();
  int16_t y = myTouch.getY();
  int button = coor2Button(x, y);
  if(button !=-1){
    if(button == 11){
      CS = CS%2+1;
      menu[3]->Tap();
      drawScreen(CS);
    }
    else if(button==0||button==1||button==2){
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
        button = button + 8;
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

