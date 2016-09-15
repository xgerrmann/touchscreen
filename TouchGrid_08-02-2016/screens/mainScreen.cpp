/*
  screen.cpp - Library for managing multiple screens
  Created by Xander G. Gerrmann, Februari 8, 2015.
*/
#include "mainScreen.h"
#include "Arduino.h"
#define RGB_GRAY    0xBDD7 
#define RGB_WHITE   0xFFFF 
#define RGB_PINK    0xE8EC 
#define RGB_BLUE  	0x055E
#define RGB_GREEN   0x9EC7


void mainScreen::Initialize(UTFT myGLCD){
	LCD = myGLCD;
	// Create menu for the home screen
	menu = new block*[NUMBERofMENU];
	for(int i=0; i<NUMBERofMENU;i++){
		menu[i] = new block(myGLCD);
	}
	
	// Create person blocks
	persons = new person*[PERSONBLOCKS];
	for(int i=0; i<PERSONBLOCKS;i++){
		persons[i] = new person(myGLCD);
	
	}
	
	// Initialize the menu
	int blockwidth = 158;
	int xspace = 2;
	int blockheight = 79;
	int yspace = 2;

	// Create menu
	for (int i = 0; i < NUMBERofMENU; i++) {
		int screen = -1; // both actually
		String txt = menuStrings[i];
		menu[i]->setText(txt,"");
		int col = i;
		int row = 0;
		if(i==3){
		  col =2; row = 3; 
		}
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
	
	// Create persons
	for (int i = 0; i < PERSONBLOCKS; i++) {
		//Serial.print(i);Serial.print(" - ");
		//Serial.print(NUMBERofPERSONS); Serial.print("\n");
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
		int screen = floor(((double)(i-personMod))/((double) PEOPLEonPAGE));
		persons[i]->setScreen(screen);
		persons[i]->setActiveColor(RGB_GREEN);
		persons[i]->setPassiveColor(RGB_GRAY);
	}
}

bool mainScreen::Touch(touchAction action){
	bool screenChange = false;
	
	// If the action is a swipe:
	// Go to the next page
	if(action.isSwipe()){
		PAGE = (PAGE+1)%2; // ASSUMES 2 pages of persons
		drawPage(PAGE);
	}
	// Otherwise, the action is a regular touch
	// According button must be pressed
	else{
	// create integer containers for the coordinates
	int16_t x,y;
	action.getCoordinates(&x, &y); // retrieve coordinates through sending pointers
	int button = coor2Button(x, y); // Find the button that corresponds with the coordinates
	if(button !=-1){// buuton = -1 is the default value and means: no button has been found that corresponds with the coordinates
	  if(button==11){// This is the OK button, the summary must be shown
		  menu[3]->Tap();
		  screenChange = true; // This means the state is changed (summary screen must be shown)
	  }
	 else if(button==0||button==1||button==2){ // These are the choice buttons for: beer, red wine and white wine
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
	    drawPage(PAGE);
	  }
	  else{ // One of hte person buttons has been pressed
	    if(PAGE==1){
	      button = button + PEOPLEonPAGE;
	    }
	    button = button - 3;
	    Serial.print("Button: ");Serial.println(button);
	    if(persons[button]->isClickable()){ //only if the person button contains a person
	      persons[button]->turf(); // increases the counter of a person
	      persons[button]->Tap();
	    }
	  }
	  
	
	}
 }
return screenChange;
		
}
  
int mainScreen::coor2Button(int xCor, int yCor){
  int blockWidth = 480/3;
  int blockHeight = 320/4;
  double margin = 0.25;
  int xMargin = (int) round(margin*blockWidth);
  int yMargin = (int) round(margin*blockHeight);
  int column  = (xCor-xCor%blockWidth)/blockWidth+1;
  int row     = (yCor-yCor%blockHeight)/blockHeight+1;
  int Xpadding = xCor%blockWidth;
  int Ypadding = yCor%blockHeight;
  //Serial.print("xMargin: ");
  //Serial.print(xMargin);
  //Serial.print("\n");
  
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

void mainScreen::Draw(){
	LCD.fillScr(RGB_WHITE);
	drawMenu();
	drawPage(PAGE);
}

void mainScreen::drawPage(int page){
  for(int i=0; i<PERSONBLOCKS; i++){
    if(persons[i]->onScreen(page)){
      persons[i]->Draw();  
    }
  }
}

void mainScreen::drawMenu(){
  for(int i=0; i<NUMBERofMENU; i++){
    menu[i]->Draw();
  }
}



void mainScreen::resetState(){
  // Reset the counters of the people and redraw the screen
  for(int p =0; p<NUMBERofPERSONS;p++){
    persons[p]->reset();
  }
  //drawPage(PAGE);
}


int mainScreen::getOrder(String *names, int* order){
	int counter = 0;
	for(int i=0; i<NUMBERofPERSONS; i++){
		int amount = persons[i]->getOrder();
		if(amount>0){
			order[counter] = amount;
			names[counter] =  persons[i]->getName();
			Serial.print("Name: ");Serial.println(names[counter]);
			Serial.print("Amount: ");Serial.println(amount);
			counter ++;
		}
	}
	return counter;
}

