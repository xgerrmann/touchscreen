#ifndef _TOUCHMANAGER_
#define _TOUCHMANAGER_

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <MatrixMath.h>  // Library for matrix calculations
#include <UTouch.h>

#define EVER true

struct touchAction
{
	float location[4]; // x, y, dx/dt, dy/dt (Location and speed for x and y)
	// Type is one of:
	// 1: Touch
	// 2: ...
	// 3: ...
	int type;
};

class trackFilter{
	private:
		// Variables
		float	A[16];
		float	A_t[16];
		float	R[4];
		float	C[8];
		float	Q[16];
		float	P[16];
		//int width_max, height_max;
		float	dt;
		float loc[4]; // [x,y,dx/dt,dy/dt]
		float Pscale = 100;
		float Qscale = 0.00001;

		// Procedures
		void setConstantMatrices();
		void setP();
		void setLoc(float location[4]);
	public:
		// Variables

		// Procedures
		trackFilter(int width, int height, float d_t);
		void reset(float measurement[2]);
		void setMatrices();
		void update(float measurement[2], float location[2]);
};

class touchManager
{
	private:
		UTouch* tScreen;
		trackFilter* filter;
		int max_interval_ms = 80;	//  [ms] Max interval between consecutive data points belonging to the same touch action
		int dt				= 7;	// [ms]
		// For debugging purposes:
		Adafruit_TFTLCD* lcd= NULL;
	public:
		touchManager( UTouch* tScreen);
		touchManager( UTouch* tScreen, Adafruit_TFTLCD* lcd);
		touchAction getAction( void );
};
#endif
