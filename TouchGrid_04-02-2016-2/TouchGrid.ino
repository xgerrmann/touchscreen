// XGGerrmann All Rights Reserved

#include <UTFT.h>
#include <UTouch.h>
#include <stdio.h>
#include <block.h>

// Define the orientation of the touch screen. 
#define TOUCH_ORIENTATION  PORTRAIT
#define NUMBERofPERSONS 16
#define NUMBERofMENU 4

// Define color scheme
#define RGB_GRAY    0xA44F //HEX: A18B79
#define RGB_WHITE   0xFFFF //
#define RGB_RED     0xFA24 //HEX: FF4522
#define RGB_LGREEN  0x5FF1 //HEX: 5BFF8D
#define RGB_DGREEN  0x2E66 //HEX: 2ECC31


// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Set the pins to the correct ones for your development shield
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(ILI9481 , 38, 39, 40, 41); 

// Initialize touchscreen
UTouch  myTouch( 6, 5, 4, 3, 2);

// global variables
//block blocks[15]; // maakt 15 blocks van de class block
block **person;
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
  "Timmeh"};

  String menuStrings[] = {
  "Bier", 
  "Rood", 
  "Wit", 
  "Next"};
  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);

  // Setup touch screen (digitizer)
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_CUSTOM);

  
  menu = new block*[NUMBERofMENU];
  for(int i=0; i<NUMBERofMENU;i++){
      menu[i] = new block(myGLCD);
  }

  person = new block*[NUMBERofPERSONS];
  for(int i=0; i<NUMBERofPERSONS;i++){
      person[i] = new block(myGLCD);
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
    menu[i]->set_Text(txt);
    int col;
    int row;
    if(i<3){row = 0; col = i % 3;} else{row = 3; col = 2;} 
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    menu[i]->set_Coordinates(xcorL, ycorL, xcorR, ycorR);
    menu[i]->setScreen(screen);
  }

  for (int i = 0; i < NUMBERofPERSONS; i++) {
    String txt = personStrings[i];
    person[i]->set_Text(txt);
    int personMod = i%8; 
    int col = personMod % 3;
    int row = 1+(personMod-personMod%3)/3;
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    person[i]->set_Coordinates(xcorL, ycorL, xcorR, ycorR);
    person[i]->resetCounter();
    int screen = 1+(i-personMod)/8;
    person[i]->setScreen(screen);
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
      button = 3;
      menu[button]->Tap();
    }
    else{
      if(CS==2){
        button = button + 8;
      }
      button = button - 3;
      person[button]->Tap();
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
  for(int i=0; i<NUMBERofPERSONS; i++){
    if(person[i]->onScreen(screen)){
      person[i]->Draw(false);  
    }
  }
}

void drawMenu(){
  for(int i=0; i<NUMBERofMENU; i++){
    menu[i]->Draw(false);
  }
}

