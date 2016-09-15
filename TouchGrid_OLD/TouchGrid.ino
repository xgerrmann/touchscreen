// XGGerrmann All Rights Reserved

#include <UTFT.h>
#include <UTouch.h>

// Define the orientation of the touch screen. 
#define TOUCH_ORIENTATION  PORTRAIT

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Set the pins to the correct ones for your development shield
// Remember to change the model parameter to suit your display module!
UTFT myGLCD(ILI9481 , 38, 39, 40, 41); 

// Initialize touchscreen
UTouch  myTouch( 6, 5, 4, 3, 2);

class block {
    int xCorL, yCorL, xCorR, yCorR;
    String text;
    int counter;

  public:
    //block(String nameIn, int xCorLIn, int yCorLIn, int xCorRIn, int yCorRIn);
    block();
    void set_Text(String textIn);
    void set_Coordinates(int xCorL, int yCorL, int xCorR, int yCorR);
    bool in_Area(int xCor, int yCor);
    void increment();
    void decrement();
    void resetCounter();
    void Draw(bool touch);
    void Tap();

};

//block::block(String nameIn, int xCorLIn, int yCorLIn, int xCorRIn, int yCorRIn){
//  text  = nameIn;
//  xCorL = xCorLIn;
//  yCorL = yCorLIn;
//  xCorR = xCorRIn;
//  yCorR = yCorRIn;
//}
block::block() {};

void block::increment(){
  counter++;
}


void block::decrement(){
  if(counter>0){
    counter--;
  }
}

void block::resetCounter(){
  counter = 0;
}

void block::set_Text(String textIn) {
  text = textIn;
}

void block::set_Coordinates(int xCorLIn, int yCorLIn, int xCorRIn, int yCorRIn) {
  xCorL = xCorLIn;
  yCorL = yCorLIn;
  xCorR = xCorRIn;
  yCorR = yCorRIn;
}

void block::Draw(bool active){
  // Draw rectangle
  if(active == false){
    myGLCD.setColor(VGA_GRAY);
    myGLCD.setBackColor(VGA_GRAY);
  }
  else{
    myGLCD.setColor(VGA_LIME);
    myGLCD.setBackColor(VGA_LIME);
  }
  myGLCD.fillRect(xCorL, yCorL,xCorR, yCorR);
   // Draw text
   myGLCD.setFont(BigFont);
   int fontXSize = myGLCD.getFontXsize();
   int fontYSize = myGLCD.getFontYsize();
   int textLength = text.length();;
   int blockWidth =  xCorR - xCorL;
   int blockHeight = yCorR - yCorL;
   int xText = xCorL+0.5*blockWidth-0.5*fontXSize*textLength;
   int yText = yCorL+0.5*blockHeight-0.5*fontYSize;
   myGLCD.setColor(VGA_WHITE);
   myGLCD.print(text, xText, yText);
}

void block::Tap(){
  Draw(true);
  increment();
  set_Text(String(counter));
  delay(50);
  Draw(false);
  
  
}

// global variables
block blocks[15]; // maakt 15 blocks van de class block

void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));

  String buttonTypes[15] = {"Bier", "Wit", "Rood", "Naam", "Naam", "Naam", "Naam", "Naam", "Naam", "Naam", "Naam", "Naam", "Naam", "Annuleer", "Plaats"};
  
  // Setup the LCD
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);

  // Setup touch screen (digitizer)
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_EXTREME);

  // Draw the grid of blocks
  myGLCD.fillScr(VGA_WHITE);

  
  int blockwidth = 156;
  int xspace = 4;
  int blockheight = 61;
  int yspace = 4;
  for (int i = 0; i < 15; i++) {
    String txt = buttonTypes[i];
    blocks[i].set_Text(txt);
    blocks[i].resetCounter();
    int col = i % 3;
    int row = (i - i % 3) / 3;
    int xcorL = col * (blockwidth + xspace);
    int ycorL = row * (blockheight + yspace);
    int xcorR = xcorL + blockwidth;
    int ycorR = ycorL + blockheight;
    blocks[i].set_Coordinates(xcorL, ycorL, xcorR, ycorR);
    blocks[i].Draw(false);
  }
}

void loop()
{
  if(myTouch.dataAvailable()){
  //myGLCD.clrScr();
  
  int xCor;
  int yCor;
  bool succes = getTouchCor(&xCor, &yCor);
  if(succes){
    int button = coor2Button(xCor, yCor);
    blocks[button].Tap();
  }
  
 }
 
}

bool getTouchCor(int *xCor, int *yCor){
  myTouch.read();
  int xRAW = myTouch.TP_X;
  int yRAW = myTouch.TP_Y;

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

  if(x0>0 && y0>0){
    *xCor = x0;
    *yCor = y0;
    return true;
  }
  else{
    return false;
  }
  
}

int coor2Button(int xCor, int yCor){
  int blockWidth = 480/3;
  int blockHeight = 320/5;

  int column  = (xCor-xCor%blockWidth)/blockWidth+1;
  int row     = (yCor-yCor%blockHeight)/blockHeight+1;

  int button = (row-1)*3+column-1;
  return button;
}

