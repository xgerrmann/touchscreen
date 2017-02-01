/*
  screen.cpp - Library for managing multiple screens
  Created by Xander G. Gerrmann, Februari 8, 2015.
*/
#include "summaryScreen.h"
#include "Arduino.h"
#define RGB_GRAY    0xBDD7 
#define RGB_WHITE   0xFFFF 
#define RGB_PINK    0xE8EC 
#define RGB_BLUE  	0x055E
#define RGB_GREEN   0x9EC7

void summaryScreen::Initialize(UTFT myGLCD){
	LCD 	= myGLCD;
	
	// Create menu for the summary
	menu = new block*[2];
	for(int i=0; i<2;i++){
			menu[i] = new block(myGLCD);
	}
	
	// Initialize the menu
	int blockwidth = 156;
	int xspace = 4;
	int blockheight = 77;
	int yspace = 2;

	for(int i = 0; i < 2; i++) {
		int screen = -1; // both actually
		String txt = menuStrings[i];
		menu[i]->setText(txt,"");
		
		int xcorL = 479 - blockwidth;
		int ycorL = i*(0.5*320+yspace/2);
		int xcorR = 479;
		int ycorR = 0.5*320-yspace/2 + i*(0.5*320+yspace);
		menu[i]->setCoordinates(xcorL, ycorL, xcorR, ycorR);
		menu[i]->setActiveColor(RGB_BLUE);
		menu[i]->setPassiveColor(RGB_GRAY);
	}	
}



bool summaryScreen::Touch(touchAction action){
	Serial.println("Enter touch summaryscreen");
	int16_t x,y;
	action.getCoordinates(&x, &y);
	bool accept = false;
	int button = coor2Button(x, y);
	if(button != -1){ // a button must have been pressed, -1 means no button press has been recognized
		Serial.print("Button: ");
		Serial.println(button);
	  if(button == 1){
		  menu[1]->Tap();
		  Serial.println("Accept");
		  accept =  true; 
	  }
	  else{
		  menu[0]->Tap();
		  Serial.println("Declined");
	  }	
	}
	return accept;

}

int summaryScreen::coor2Button(int xCor, int yCor){
	Serial.println("Touch summary screen");
	int blockWidth = 480/3;
	int blockHeight = 320/2;
	double margin = 0.25;
	int xMargin = (int) round(margin*blockWidth);
	int yMargin = (int) round(margin*blockHeight);
	int column  = (xCor-xCor%blockWidth)/blockWidth; // zero based
	int row     = (yCor-yCor%blockHeight)/blockHeight; // zero based
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
	if((Xpadding>Xmin)&&(Xpadding<Xmax)&&(Ypadding>Ymin)&&(Ypadding<Ymax)&&column == 2){ // both buttons are in the first column
	  button = row; // 0 or 1
	}
	return button;
}

void summaryScreen::Draw(String* nameList, int* orderList, int numberOfOrders){
	Serial.print("Name: ");Serial.println(nameList[0]);
	Serial.print("Amount: ");Serial.println(orderList[0]);
	LCD.fillScr(RGB_WHITE);
	drawMenu();
	drawSummary(nameList, orderList , numberOfOrders);
}

void summaryScreen::drawSummary(String *nameList, int *orderList, int numberOfOrders){
	//int fontXSize = LCD.getFontXsize();
	int fontYSize = LCD.getFontYsize();
	int fontXSize = LCD.getFontXsize();
	int vSpacing = 5;
	int totalHeight = numberOfOrders*(fontYSize+vSpacing)-vSpacing;
	int y0 = 320/2-round(totalHeight/2);
	int x0 = 40;
	int y;
	LCD.setColor(VGA_BLACK);
	LCD.setBackColor(VGA_TRANSPARENT);
	int maxStringLength = 0;
	int stringLength;
	for(int i=0; i<numberOfOrders; i++){
		y = y0+i*(fontYSize+vSpacing);
		LCD.print(nameList[i],x0,y);
		stringLength = nameList[i].length();
		if(stringLength>maxStringLength){
			maxStringLength = stringLength;
		}
	}
	
	int x1 = x0 + maxStringLength * fontXSize + 30;
	for(int i=0; i<numberOfOrders; i++){
			y = y0+i*(fontYSize+vSpacing);
			LCD.print(nameList[i],x0,y);
			LCD.printNumI(orderList[i],x1,y);
	}
}

void summaryScreen::drawMenu(){
	Serial.println("Start drawing menu of summary");
	for(int i=0; i<NUMBERofMENU; i++){
		menu[i]->Draw();
	}
	Serial.println("Finished drawing menu of summary");
}
