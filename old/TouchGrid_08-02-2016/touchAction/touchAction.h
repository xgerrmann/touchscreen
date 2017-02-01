/*
  Made by Xander Gerrmann
*/

#ifndef touchAction_h
#define touchAction_h

#include <math.h>

class touchAction
{
	private:
	bool Swipe;
	char SwipeDirection; // Dir is the direction of the swipe can be either 'L' or 'R'
	int Coordinates[2]; 
	
	public:
		bool isSwipe();
		char getSwipeDir();
		void setSwipe(bool swipe);
		void setSwipeDir(char dir);
		void getCoordinates(int* xCor, int* yCor);
		void setCoordinates(int xCor, int yCor);
		
		touchAction();
    
};

double CalcStdev(int daArray[], double mean, int iSize);
double GetMedian(int daArray[], int iSize);
double GetAverage(int daArray[], int iSize);

#endif