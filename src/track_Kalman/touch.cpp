// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <MatrixMath.h>  // Library for matrix calculations
#include <UTouch.h>
#include "definitions.h"

trackFilter* filter;

float	dt = 7.0/1000.0; // 7 [ms]
int		n_miss = 10;

void setup(void)
{
	Serial.begin(9600);
	
	tft.reset();
	
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_LOW);
	
	uint16_t identifier = 0x9488 ;
	
	tft.begin(identifier);
	
	tft.fillScreen(BLACK);
	
	tft.setCursor(280, 10);
	tft.setTextColor(WHITE);
	tft.setTextSize(3);
	
	filter = new trackFilter(420,380,dt);
}

void loop()
{
	int x_meas, y_meas, x_filt, y_filt;
	float measurement[2]= {};
	float location[2]	= {};
	int t_elapsed		= 0;	// [ms]
	int t_start_touch	= 0;	// [ms]
	int t_start_loop	= 0;
	int t_end_loop		= 0;
	bool filter_init	= false;
	bool data_available = myTouch.dataAvailable();
	if(data_available)
	{
		t_start_touch = millis(); // Timer for the duration of the touch action
		for(EVER;;)
		{
			if(!data_available)
			{
				if((millis()-t_end_loop)>n_miss*dt*1000)
				{
					break; // Break while loop, touch action is interrupted
				}
				data_available = myTouch.dataAvailable();
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
			myTouch.read();
			y_meas = myTouch.getX();
			x_meas = myTouch.getY();
			if ((x_meas!=-1) and (y_meas!=-1))
			{
				y_meas=480-y_meas;
				
				measurement[0] = (float) x_meas;
				measurement[1] = (float) y_meas;
				filter->update(measurement,location);
				x_filt	= location[0];
				y_filt	= location[1];
				
				tft.fillCircle(x_meas, y_meas, 1, RED);
				tft.fillCircle(x_filt, y_filt, 1, BLUE);
			}
			t_end_loop	= millis();
			t_elapsed	= t_end_loop-t_start_loop;
			// Check if data is available for the next loop
			data_available = myTouch.dataAvailable();
		}
	}
}
