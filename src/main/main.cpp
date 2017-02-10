// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_GFX.h> // Core graphics library
#include "UIobjects.h"			// Custom library for User interface objects
#include <UTouch.h>
#include <TouchManager.h> // Library for filtering touchscreen data
#include <MatrixMath.h>  // Library for matrix calculations
#include "header.h"

// Create array of screens for easy access
Screen*	screens[NUMBER_SCREENS];
Block*	blocks[MAX_BLOCKS];

void tmpfunc( void )
{
	screens[0]->draw();
}

void donothing( void ){}

void setup(void)
{
	// Setup Blocks
	for(int i = 0; i < NUMBER_SCREENS; i++){
		screens[i] = new Screen( &lcd, NROWS, NCOLS );
	}
	
	// Screen 1
	blocks[0] = new Block(screens[0], NCOLS,	1,			1,	NROWS/2,	&donothing);
	blocks[1] = new Block(screens[0], NCOLS,	NROWS/2+1,	1,	NROWS/2,	&donothing);
	blocks[2] = new Block(screens[0], 3,	4,	2,	1,	&donothing);
	blocks[3] = new Block(screens[0], 1,	1,	2,	1,	&donothing);
	blocks[4] = new Block(screens[0], 3,	1,	2,	1,	&donothing);
	blocks[5] = new Block(screens[0], 1,	3,	2,	1,	&tmpfunc);
	
	// Screen 2
	Screen screen2( &lcd, NROWS, NCOLS );
	blocks[6] = new Block(screens[1], NCOLS,	1,			1,	NROWS/2,	&donothing);
	blocks[7] = new Block(screens[1], NCOLS,	NROWS/2+1,	1,	NROWS/2,	&donothing);
	blocks[8] = new Block(screens[1], 3,	4,	2,	1,	&donothing);
	

	// Other
	Serial.begin(9600);
	lcd.begin(0x9488);
	lcd.setRotation(3);

	// Attach screen 1
	screens[0]->attach_block(blocks[0]);
	screens[0]->attach_block(blocks[1]);
	screens[0]->attach_block(blocks[2]);
	screens[0]->attach_block(blocks[3]);
	screens[0]->attach_block(blocks[4]);
	screens[0]->attach_block(blocks[5]);
	
	// Attach screen 2
	screens[1]->attach_block(blocks[6]);
	screens[1]->attach_block(blocks[7]);
	screens[1]->attach_block(blocks[8]);
	
	// Draw first screen
	screens[0]->draw();
	screen_active = 0;

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
		screens[screen_active]->touch(tAction.location[0],tAction.location[1]-160);
	}
}
