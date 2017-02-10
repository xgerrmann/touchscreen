// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <MatrixMath.h>  // Library for matrix calculations
#include <UTouch.h>
#include "definitions.h"


float	dt = 7.0/1000.0; // 7 [ms]
int		n_miss = 10;

struct touchAction
{
	float location[4]; // x, y, dx/dt, dy/dt (Location and speed for x and y)
	// Type is one of:
	// 1: Touch
	// 2: ...
	// 3: ...
	int type;
};

class touchManager
{
	private:
		UTouch* tScreen;
		trackFilter* filter;
	public:
		touchManager( UTouch* tScreen );
		touchAction getAction( void );
};

touchManager::touchManager( UTouch* Tscreen )
{
	// Attach screen
	this->tScreen = Tscreen;
	
	// Initialize filter
	this->filter	= new trackFilter(420,380,dt);
}

touchAction touchManager::getAction()
{
	int x_meas, y_meas;
	float x_filt, y_filt;
	float measurement[2]= {};
	float location[2]	= {};
	int t_elapsed		= 0;	// [ms]
	int t_start_touch	= 0;	// [ms]
	int t_start_loop	= 0;
	int t_end_loop		= 0;
	bool filter_init	= false;
	t_start_touch = millis(); // Timer for the duration of the touch action
	for(EVER;;)
	{
		if(!this->tScreen->dataAvailable())
		{
			if((millis()-t_end_loop)>n_miss*dt*1000)
			{
				break; // Break while loop, touch action is interrupted
			}
			continue; // Restart while loop
		}
		t_start_loop = millis();
	
		// Reset the filter for each NEW touch action
		if(!filter_init)
		{
			// Reset filter for current touch action
			filter->reset(measurement);
			filter_init = true;
		}
		tScreen->read();
		y_meas = tScreen->getX();
		x_meas = tScreen->getY();
		if ((x_meas!=-1) and (y_meas!=-1))
		{
			y_meas=480-y_meas;
			
			measurement[0] = (float) x_meas;
			measurement[1] = (float) y_meas;
			filter->update(measurement,location);
			x_filt	= (float) location[0];
			y_filt	= (float) location[1];
		}
		t_end_loop	= millis();
		t_elapsed	= t_end_loop-t_start_loop;
		// Check if data is available for the next loop
	}
	touchAction tAction;
	tAction.location[0] = x_filt;
	tAction.location[1] = y_filt;
	tAction.type	= 1;
	return tAction;
}

touchManager* tManager;

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
	
	tManager	= new touchManager(&myTouch);
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
