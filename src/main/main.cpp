// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_GFX.h> // Core graphics library
#include "UIobjects.h"// Custom library for User interface objects
#include <UTouch.h>
#include <TouchManager.h> // Library for filtering touchscreen data
#include <MatrixMath.h>  // Library for matrix calculations
#include "header.h"

void setup(void)
{
	sMngrMemFn donothing	= &screenManager::donothing;
	sMngrMemFn nextScreen	= &screenManager::nextScreen;
	sManager	= new screenManager();
	// Create screens and attach to manager
	for(int i = 0; i < NUMBER_SCREENS; i++)
	{
		screens[i] = new Screen( &lcd, NROWS, NCOLS );
		sManager->attach_screen(screens[i]);
	}

	int block_counter = 0;

	// Normal buttons
	for(int s=0; s<NUMBER_SCREENS; s++)
	{
		for(int r=0; r<NROWS; r++)
		{
			for(int c=0; c<(NCOLS-1)/2; c++)
			{
				blocks[block_counter] = new Block(screens[0], c*2+1, r+1,	2,	1, donothing);
				screens[s]->attach_block(blocks[block_counter]);
				block_counter ++;
			}
		}
	}
	// Navigation
	for(int s=0; s<NUMBER_SCREENS; s++)
	{
		for(int r=0; r<NROWS; r++)
		{
			//sMngrMemFn func;
			//if(r==4)
			//{
			//	func = donothing;
			//} else {
			//	func = nextScreen;
			//}
			blocks[block_counter] = new Block(screens[0], NCOLS, r+1,	1,	2, nextScreen);
			screens[s]->attach_block(blocks[block_counter]);
			block_counter ++;
		}
	}
	
	// Other
	Serial.begin(9600);
	lcd.begin(0x9488);
	lcd.setRotation(3);

	// Draw first screen
	sManager->refresh();

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
