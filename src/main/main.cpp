// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#define __cxa_guard_acquire 0
#define __cxa_guard_release 0

#include <Adafruit_TFTLCD.h> 	// Hardware-specific library
#include <Adafruit_GFX.h> 		// Core graphics library
#include <UIobjects.h> 			// Custom library for User interface objects
#include <UTouch.h>				
#include <TouchManager.h> 		// Library for filtering touchscreen data
#include <MatrixMath.h> 			// Library for matrix calculations
#include <LinkedList.h>
#include "header.h"
#include <ArduinoJson.h>
#include "httpGETdata.h"
#include "UIElementDefinitions.h"


void setup(void)
{
	Serial.begin(115200);
	esp.begin(115200);		// Start ESP8266
	reset();				// Reset ESP
	while(connectWifi(ssid, pass))
	{
		Serial.print("Could not connect. Retry.");
	}
	
	getPersons();
	getProducts();
	//addPersons();

	SCREENS_P		= (int) ceil((double)persons.size()/(NROWS*2)); // Screens for person blocks
	SCREENS_O		= 1;							// Other screens
	SCREENS_TOTAL	= SCREENS_P + SCREENS_O;
	MAX_BLOCKS		= SCREENS_TOTAL*NROWS*NCOLS;
	N_BLOCKS		= 0;

	UIElements();

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
	iManager	= new itemManager();
	Serial.println("Setup finished");
}

void loop(void)
{
	if (tScreen.dataAvailable() == true)
	{
		touchAction tAction = tManager->getAction();
		sManager->touch(tAction.location[0],tAction.location[1]-160);
	}
}
