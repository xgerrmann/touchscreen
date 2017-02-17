// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_GFX.h> // Core graphics library
#include "UIobjects.h"// Custom library for User interface objects
#include <UTouch.h>
#include <TouchManager.h> // Library for filtering touchscreen data
#include <MatrixMath.h>  // Library for matrix calculations
#include <LinkedList.h>
#include "header.h"

void setup(void)
{
	Serial.begin(9600);

	persons.add(new Person("Knor",1,20));
	persons.add(new Person("Kotser",2,12));
	persons.add(new Person("Trucker",3,15));
	persons.add(new Person("Dijntje",4,3));
	persons.add(new Person("'Rina",5,2));
	persons.add(new Person("Herrie",6,5));
	persons.add(new Person("JP",7,8));
	persons.add(new Person("OPT",8,3));
	persons.add(new Person("Bami",9,2));

	// Blocks
	sManager				= new screenManager();
	// Create screens and attach to manager
	//TODO base number of screens on number of persons
	int i = 0;
	for(; i < NUMBER_SCREENS; i++)
	{
		screens[i] = new Screen( &lcd, NROWS, NCOLS );
		sManager->attach_screen(screens[i]);
	}
	// Dialog screen
	i++;
	Serial.print(i);
	screens[i] = new Screen( &lcd, NROWS, NCOLS);
	sManager->attach_screen(screens[i]);

	int block_counter = 0;
	int s = 0;
	int r = 0;
	int c = 0;
	for(int i=0; i<persons.size(); i++)
	{
		s = (int) floor((double)block_counter/8);
		r = (int) floor((double)(block_counter %(s*8))/(NCOLS/2)) ;
		c = block_counter%(NCOLS/2);
		blocks[block_counter] = new personBlock(screens[s], c*2+1, r+1,	2,	1, &increment, persons.get(i));
		screens[s]->attach_block(blocks[block_counter]);
		block_counter ++;
	}
	// Navigation
	for(int s=0; s<NUMBER_SCREENS; s++)
	{
		for(int r=0; r<NROWS; r++)
		{
			void(*func_action)(Block*);
			void(*func_draw)(Block*);
			void(*func_clear)(Block*);
			uint16_t color;
			switch(r)
			{
				case 0:	func_action	= &donothing;
						func_draw = NULL;
						func_clear = NULL;
						color = info_color;
						break; // Button for drinks
				case 1:	func_action	= &dialogScreen;
						func_draw = &fillDraw;
						func_clear = NULL;
						color = success_color;
						break; // Button for approval
				case 2:	func_action	= &regular_cancel;
						func_draw = &fillDraw;
						func_clear = NULL;
						color = danger_color;
						break; // Button for cancel
				case 3:	func_action	= &nextScreen;
						func_draw = NULL;
						func_clear = NULL;
						color = info_color;
						break; // Button for next screen
				default:func_action = &donothing;
						func_draw = &donothing;
						func_clear = NULL;
						color = info_color;
						break;
			}
			blocks[block_counter] = new menuBlock(screens[s], NCOLS, r+1,	1,	1, func_action, func_draw, func_clear);
			blocks[block_counter]->setColor(color);
			screens[s]->attach_block(blocks[block_counter]);
		}
	}

	// Dialog
	blocks[block_counter] = new menuBlock(screens[3], NCOLS, NROWS-2,	1,	1, &donothing, &fillDraw,NULL);
	blocks[block_counter]->setColor(success_color);
	screens[3]->attach_block(blocks[block_counter]);
	block_counter ++;
	blocks[block_counter] = new menuBlock(screens[3], NCOLS, NROWS-1,	1,	1, &dialog_cancel, &fillDraw, NULL);
	blocks[block_counter]->setColor(danger_color);
	screens[3]->attach_block(blocks[block_counter]);
	block_counter ++;
	blocks[block_counter] = new menuBlock(screens[3], 2, 1,	1,	1, &donothing, &drawList, &clearList );
	screens[3]->attach_block(blocks[block_counter]);
	blocks[block_counter]->setColor(success_color);
	block_counter ++;
	

	// Other
	lcd.begin(0x9488);
	lcd.setRotation(3);
	lcd.setTextColor(BLACK);

	// Draw first screen
	sManager->refresh(0);
	// Initialize digitizer
	tScreen.InitTouch(0);
	tScreen.setPrecision(PREC_LOW);

	tManager	= new touchManager(&tScreen);
}

void loop(void)
{
	if (tScreen.dataAvailable() == true)
	{
		touchAction tAction = tManager->getAction();
		sManager->touch(tAction.location[0],tAction.location[1]-160);
	}
}
