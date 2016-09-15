/*
  screen.h - Library for touchable blocks
  Created by Xander G. Gerrmann, Februari 8, 2015.
*/
#ifndef summaryScreen_h
#define summaryScreen_h

#include "Arduino.h"
#include <UTFT.h>
#include <UTouch.h>
#include <touchAction.h>
#include <block.h>
#include <person.h>
extern uint8_t BigFont[];

class summaryScreen {
    block** 	menu;
    
	#define NUMBERofMENU 2
    int persons = 15;
	
	String menuStrings[NUMBERofMENU] = {  "Annuleren",   "Plaatsen"};  
           
	UTFT LCD;
    
    bool state = false; // true state is active, false if inactive
    
    int coor2Button(int xCor, int yCor);
    void resetState();
    void drawMenu();
    void drawSummary(String *nameList, int *orderList, int numberOfOrders);

  public:
    void Initialize(UTFT myGLCD);
    bool Touch(touchAction action);
    void Draw(String *nameList, int *orderList,int numberOfOrders);    
};

#endif
