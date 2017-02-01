/*
 * Adjusted by Xander Gerrmann for improved precision
  UTouch.cpp - Arduino/chipKit library support for Color TFT LCD Touch screens 
  Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
  
  Basic functionality of this library are based on the demo-code provided by
  ITead studio.

  You can find the latest version of the library at 
  http://www.RinkyDinkElectronics.com/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#include "UTouch.h"
#include "UTouchCD.h"
#include <math.h>   

#if defined(__AVR__)
	#include "hardware/avr/HW_AVR.inc"
#elif defined(__PIC32MX__)
	#include "hardware/pic32/HW_PIC32.inc"
#elif defined(__arm__)
	#include "hardware/arm/HW_ARM.inc"
#endif

UTouch::UTouch(byte tclk, byte tcs, byte din, byte dout, byte irq)
{
	T_CLK	= tclk;
	T_CS	= tcs;
	T_DIN	= din;
	T_DOUT	= dout;
	T_IRQ	= irq;
}

void UTouch::InitTouch(byte orientation)
{
	orient					= orientation;
	_default_orientation	= CAL_S>>31;
	prec					= 10;

	P_CLK	= portOutputRegister(digitalPinToPort(T_CLK));
	B_CLK	= digitalPinToBitMask(T_CLK);
	P_CS	= portOutputRegister(digitalPinToPort(T_CS));
	B_CS	= digitalPinToBitMask(T_CS);
	P_DIN	= portOutputRegister(digitalPinToPort(T_DIN));
	B_DIN	= digitalPinToBitMask(T_DIN);
	P_DOUT	= portInputRegister(digitalPinToPort(T_DOUT));
	B_DOUT	= digitalPinToBitMask(T_DOUT);
	P_IRQ	= portInputRegister(digitalPinToPort(T_IRQ));
	B_IRQ	= digitalPinToBitMask(T_IRQ);

	pinMode(T_CLK,  OUTPUT);
    pinMode(T_CS,   OUTPUT);
    pinMode(T_DIN,  OUTPUT);
    pinMode(T_DOUT, INPUT);
    pinMode(T_IRQ,  OUTPUT);

	sbi(P_CS, B_CS);
	sbi(P_CLK, B_CLK);
	sbi(P_DIN, B_DIN);
	sbi(P_IRQ, B_IRQ);
}

void UTouch::read()
{
	unsigned long tx=0, temp_x=0;
	unsigned long ty=0, temp_y=0;
	unsigned long minx=99999, maxx=0;
	unsigned long miny=99999, maxy=0;
	double xBuf[prec];
	double yBuf[prec];
	int datacount=0;

	cbi(P_CS, B_CS);                    

	pinMode(T_IRQ,  INPUT);
	for (int i=0; i<prec; i++)
	{
		if (!rbi(P_IRQ, B_IRQ))
		{
			touch_WriteData(0x90);        
			pulse_high(P_CLK, B_CLK);
			temp_x=touch_ReadData();
			if (!rbi(P_IRQ, B_IRQ))
			{
				touch_WriteData(0xD0);      
				pulse_high(P_CLK, B_CLK);
				temp_y=touch_ReadData();

				if ((temp_x>0) and (temp_x<4096) and (temp_y>0) and (temp_y<4096))
				{
					// Add data to buffer
					
					if(prec==PREC_CUSTOM||prec == PREC_REGISTER){
					xBuf[datacount] = temp_x;
					yBuf[datacount] = temp_y;
					}

					datacount++;
				}
			}
		}
	}


	pinMode(T_IRQ,  OUTPUT);

	sbi(P_CS, B_CS);  
	
	if(prec == PREC_REGISTER){
		// First get the type of touch (now swipe or regular touch)
		// calculate scaled variances and threshold
		double xmean = GetAverage(xBuf,datacount);
		double ymean = GetAverage(yBuf,datacount);
		//double xmean = GetMedian(xBuf,datacount);
		//double ymean = GetMedian(yBuf,datacount);
		double xStd =  CalcStdev(xBuf,xmean, datacount);
		//double yStd =  CalcStdev(yBuf,ymean, datacount);
		xStdev = xStd;
		//yStdev = yStd;
		TP_X = ymean;
		TP_Y = xmean;
	}
	// Clean up the data and get accurate points
	if(prec == PREC_CUSTOM){
		double xmed = GetMedian(xBuf,datacount);
		double ymed = GetMedian(yBuf,datacount);
		TP_X=ymed;
		TP_Y=xmed;
		//if (orient == _default_orientation){
		//	TP_X=xmed;
		//	TP_Y=ymed;
		//}else{
		//	TP_X=ymed;
		//	TP_Y=xmed;
		//}
		//char buffer[120];
		//sprintf(buffer,"TP_X: %4d, TP_Y: %4d",TP_X,TP_Y);
		//Serial.println(buffer);
	}

}

bool UTouch::dataAvailable()
{
	bool avail;
	pinMode(T_IRQ,  INPUT);
	avail = !(rbi(P_IRQ, B_IRQ));
	pinMode(T_IRQ,  OUTPUT);
	return avail;
}

void UTouch::calcCors()
{	  
	//Serial.print("test");
	double x1 = (double)(TP_X-XA)/(XB-XA)*wX;
	double x2 = (double)(TP_X-XC)/(XD-XC)*wX;
	//char buffer[120];
	//sprintf(buffer,"TP_X: %4d, XA: %4d, XB: %4d, wX: %4d, x1: %6d, x2: %6d",TP_X,XA,XB,wX,x1,x2);
	//Serial.println(buffer);

	double y1 = (double)(YA-TP_Y)/(YA-YC)*hY;
	double y2 = (double)(YB-TP_Y)/(YB-YD)*hY;

	double x0 = (double)(wX*hY*x1+wX*(x2-x1)*y1)/(wX*hY-(x2-x1)*(y2-y1));
	double y0 = (double)(wX*hY*y1+hY*(y2-y1)*x1)/(wX*hY-(y2-y1)*(x2-x1));
	  
	tX = x0;
	tY = y0;
}

int16_t UTouch::getX()
{
	return tX;
}

int16_t UTouch::getY()
{
	return tY;
}

void UTouch::setPrecision(byte precision)
{
	switch (precision)
	{
		case PREC_LOW:
			prec=1;		// DO NOT CHANGE!
			break;
		case PREC_MEDIUM:
			prec=12;	// Iterations + 2
			break;
		case PREC_HI:
			prec=27;	// Iterations + 2
			break;
		case PREC_EXTREME:
			prec=102;	// Iterations + 2
			break;
		case PREC_CUSTOM:
			prec= PREC_CUSTOM;	// Iterations
			break;
		case PREC_REGISTER:
			prec= PREC_REGISTER;	// Iterations
			break;
		default:
			prec=12;	// Iterations + 2
			break;
	}
}

void UTouch::calibrateRead()
{
	unsigned long tx=0;
	unsigned long ty=0;

	cbi(P_CS, B_CS);                    

	touch_WriteData(0x90);        
	pulse_high(P_CLK, B_CLK);
	tx=touch_ReadData();

	touch_WriteData(0xD0);      
	pulse_high(P_CLK, B_CLK);
	ty=touch_ReadData();

	sbi(P_CS, B_CS);                    

	TP_X=ty;
	TP_Y=tx;
}

double UTouch::GetMedian(double daArray[], int iSize) {
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

double  UTouch::GetAverage(double daArray[], int iSize){
	double sum = 0;
	for(int i =0; i<iSize;i++){
		sum = sum + daArray[i];
	}
	double mean = sum/iSize;
	return mean;
}

double  UTouch::CalcStdev(double daArray[], double mean, int iSize){
	double sum = 0;
	double val = 0;
	for(int i =0; i<iSize;i++){
		val = daArray[i]-mean;
		sum = sum + val*val;
	}
	double stdev = sqrt(sum); //(1/(iSize+1))*sqrt(sum)
	return stdev;
}

double UTouch::GetStdev(){
	return xStdev;
}