/*
  screen.h - Library for touchable blocks
  Created by Xander G. Gerrmann, Februari 8, 2015.
*/
#ifndef mainScreen_h
#define mainScreen_h

#include "Arduino.h"
#include <UTFT.h>
#include <UTouch.h>
#include <block.h>
#include <person.h>
#include <touchAction.h>
extern uint8_t BigFont[];

class mainScreen {
	int currentPage; // this screen has multiple pages
	
    block** 	menu;
    person** 	persons;
    
    int menuButtons = 4; 
	#define NUMBERofPERSONS 15 //must be equal to number of persons in array
	#define PEOPLEonPAGE 8
	#define NUMBERofMENU 4
	int PERSONBLOCKS = (int) ceil((double)NUMBERofPERSONS/PEOPLEonPAGE)*((double)PEOPLEonPAGE); // equal to the amount of pages neccesary  * max people per pages
	
    String personStrings[NUMBERofPERSONS] 	= {  "Harrie",   "OPT",   "Twan",   "Knor",   "Trucker",   "Tess",   "Twilight",   "Sopje",   "Kotser",   "Bambi",  "Daggie",  "Marina",  "Zapp",  "Jepsen",   "Arnold"};
    String menuStrings[NUMBERofMENU]  		= {  "Bier",   "Rood",   "Wit",   "OK"};
    
    int PAGE = 0;
       
	UTFT LCD;
    
    bool state = false; // true state is active, false if inactive
    
    void drawPage(int page);
    void drawMenu();
    int coor2Button(int xCor, int yCor);
    

  public:
    void resetState();
    int getOrder(String *names, int* order);
    void Initialize(UTFT myGLCD);
    bool Touch(touchAction action);
    void Draw();    
};

#endif
