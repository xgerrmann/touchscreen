// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchManager.h> // Library for filtering touchscreen data
#include <MatrixMath.h>  // Library for matrix calculations
#include <UTouch.h>
#include "definitions.h"

void setup(void)
{
	Serial.begin(9600);

	tft.reset();

	myTouch.InitTouch();
	myTouch.setPrecision(PREC_LOW);

	tft.begin(0x9488);
	tft.fillScreen(BLACK);

	tft.setCursor(280, 10);
	tft.setTextColor(WHITE);
	tft.setTextSize(3);

	tManager		= new touchManager(&myTouch);
}

void loop()
{
	bool data_available = myTouch.dataAvailable();
	if(data_available)
	{
		touchAction tAction = tManager->getAction();
		tft.fillCircle(tAction.location[0], tAction.location[1], 1, BLUE);
	}
}
