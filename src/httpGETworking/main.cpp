// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

// TODO: Watch the formatting here, otherwise errors will result!! #include_<...>_//Comment  The spaces '_' are crucial!!
#include <Adafruit_TFTLCD.h>	// Hardware-specific library
#include <Adafruit_GFX.h>		// Core graphics library
#include <UTouch.h>				// Library for touch action registration and processing
#include <UIobjects.h>			// Custom library for User interface objects
#include <TouchManager.h>		// Library for filtering touchscreen data
#include <MatrixMath.h>			// Library for matrix calculations
#include <LinkedList.h>			//
#include <WiFiEsp.h>			//
#include <ArduinoJson.h>
#include "header.h"
#include "UIElementDefinitions.h"
// Forward declarations

void setup(void)
{
	Serial.begin(115200);	// Start serial
	esp.begin(115200);		// Start ESP8266
	reset();				// Reset ESP
	while(connectWifi(ssid, pass))
	{
		Serial.println("\r\nCould not connect. Retry.");
	}

	getProducts();
	delay(3000);
	getPersons();

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
	
	//uint8_t** bmp;
	//bitmap_encode_rgb(tmp, int 4, int 4, uint8_t** bmp)
	//lcd.drawBitmap(0,0,bitmap,32,16,0x00);
}

void loop(void)
{
	if (tScreen.dataAvailable() == true)
	{
		touchAction tAction = tManager->getAction();
		sManager->touch(tAction.location[0],tAction.location[1]-160);
	}
}
