// XGGerrmann All Rights Reserved

#include <UTFT.h>
#include <UTouch.h>
#include <stdio.h>
#include <block.h>

// Define the orientation of the touch screen. 
#define TOUCH_ORIENTATION  PORTRAIT
#define NUMBERofBLOCKS 20
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
block **blocks;

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
    
  String buttonTypes[] = {
  "Bier", 
  "Wit", 
  "Rood", 
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
  "Max",
  "Jepsen", 
  "Timmeh",
  "Annuleer", 
  "Plaats"};
  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);

  // Setup touch screen (digitizer)
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_CUSTOM);

  
  blocks = new block*[NUMBERofBLOCKS];
  for(int i=0; i<15;i++){
      blocks[i] = new block(myGLCD);
  }
  
  // Draw the grid of blocks
  myGLCD.fillScr(VGA_WHITE);

  int blockwidth = 156;
  int xspace = 4;
  int blockheight = 77;
  int yspace = 4;
  int screens = (NUMBERofBLOCKS-NUMBERofBLOCKS%12)/12;
  for(int s =1; s<screens+1;s++){
    for (int i = 0; i < 12; i++) {
      String txt = buttonTypes[i];
      blocks[i]->set_Text(txt);
      blocks[i]->resetCounter();
      int col = i % 3;
      int row = (i - i % 3) / 3;
      int xcorL = col * (blockwidth + xspace);
      int ycorL = row * (blockheight + yspace);
      int xcorR = xcorL + blockwidth;
      int ycorR = ycorL + blockheight;
      blocks[i]->set_Coordinates(xcorL, ycorL, xcorR, ycorR);
      blocks[i]->setScreen(s);
      Serial.print("Block: ");
      Serial.print(i);
      Serial.print("\n");
      Serial.print("Screen: ");
      Serial.print(s);
      Serial.print("\n");
    }
  }
  drawScreen(1);
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
    Serial.print("Button: ");
    Serial.print(button);
    Serial.print("\n");
    blocks[button]->Tap(); 
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
  for(int i=0; i<NUMBERofBLOCKS; i++){
    if(blocks[i]->onScreen(screen)){
      Serial.print("Screen: ");
      Serial.print(screen);
      Serial.print("\n");
      blocks[i]->Draw(false);  
    }
  }
}

