/*
 * Made by Xander Gerrmann

*/
#include "touchAction.h"

touchAction::touchAction(){
	Swipe = false;
	SwipeDirection = 'NoSwipe';
	Coordinates[0] = -1;
	Coordinates[1] = -1;
}


bool touchAction::isSwipe(){
	return Swipe;
}

char touchAction::getSwipeDir(){
	return SwipeDirection;
}

void touchAction::getCoordinates(int* xCor, int* yCor){
	*xCor = Coordinates[0];
	*yCor = Coordinates[1];
}

void touchAction::setCoordinates(int xCor, int yCor){
	Coordinates[0] = xCor;
	Coordinates[1] = yCor;
}

void touchAction::setSwipe(bool swipe){
	Swipe = swipe;
}

void touchAction::setSwipeDir(char dir){
	SwipeDirection = dir;
}

double  GetAverage(int daArray[], int iSize){
  double sum = 0;
  for(int i =0; i<iSize;i++){
    sum = sum + daArray[i];
  }
  double mean = (double)sum/iSize;
  return mean;
}

double  GetMedian(int daArray[], int iSize){
  // Allocate an array of the same size and sort it.
    double* dpSorted = new double[iSize];
    // Copy values
    for (int i = 0; i < iSize; ++i) {
        dpSorted[i] = daArray[i];
    }
    // Sort  values
    for (int i = iSize - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (dpSorted[j] > dpSorted[j+1]) {
                double dTemp = dpSorted[j];
                dpSorted[j] = dpSorted[j+1];
                dpSorted[j+1] = dTemp;
            }
        }
    }

    // Middle or average of middle values in the sorted array.
    double dMedian = 0.0;
    if ((iSize % 2) == 0) {
        dMedian = (dpSorted[iSize/2] + dpSorted[(iSize/2) - 1])/2.0;
    } else {
        dMedian = dpSorted[iSize/2];
    }
    delete [] dpSorted;
    return dMedian;
}

double  CalcStdev(int daArray[], double mean, int iSize){
  double sum = 0;
  double val = 0;
  for(int i =0; i<iSize;i++){
    val = (double)daArray[i]-mean;
    sum = sum + val*val;
  }
  double stdev = sqrt(sum); //(1/(iSize+1))*sqrt(sum)
  return stdev;
}