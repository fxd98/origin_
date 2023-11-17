/*------------------------------------------------------------------------------*
 * File Name: freqresp.c														*
 * Creation: DVT 2/5/03															*
 * Purpose: Demonstrates COM call to NI LabView acting as ActiveX Server		*
 * Copyright (c)OriginLab Corp. 2003											*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
////////////////////////////////////////////////////////////////////////////////////
// you can include just this typical header file for most Origin built-in functions and classes
// and it takes a reasonable amount of time to compile, 
#include <origin.h>
// this file include most of the other header files except the NAG header, which takes longer to compile
// NAG routines
//#include <OC_nag.h> // this contains all the NAG headers, 
#include <variant.h>
#include <VariantTypes.h>


////////////////////////////////////////////////////////////////////////////////////
// start your functions here

//This example is based on VBA script in: National Instruments\LabVIEW 6.1\examples\comm\freqresp.xls
///Sophy 12/27/2011 CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
//void freqresp()
void freqresp(bool bQuitLabview = false)
///end CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
{
	//Imputs
	float Amplitude(1);
	int NumberOfSteps(100);
	float LowFrequency(1);
	float HighFrequency(1000);
	
	Worksheet wks;
	wks.Create();
	Dataset dsx(wks, 0);
	Dataset dsy(wks, 1);
	///Sophy 12/27/2011 CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
	//LoadData( Amplitude,  NumberOfSteps, LowFrequency, HighFrequency, dsx, dsy );
	LoadData( Amplitude,  NumberOfSteps, LowFrequency, HighFrequency, dsx, dsy, bQuitLabview);
	///end CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
}

/*
' LoadData Macro
'
' This is an example to demonstrate LabVIEW's Active-X server capabilities.
' Executing this macro loads a LabVIEW supplied example VI "Frequency Response.vi",
' runs it and plots the result on an Origin Plot.
*/
///Sophy 12/27/2011 CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
//void LoadData( _VARIANT vAmplitude,  _VARIANT vNumberOfSteps,  _VARIANT vLowFrequency,  _VARIANT vHighFrequency, Dataset &dsx, Dataset &dsy )
void LoadData( _VARIANT vAmplitude,  _VARIANT vNumberOfSteps,  _VARIANT vLowFrequency,  _VARIANT vHighFrequency, Dataset &dsx, Dataset &dsy, bool bQuitLabview = false)
///end CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
{
	Object lvapp = CreateObject("LabVIEW.Application");
	string viPath = lvapp.ApplicationDirectory + "\\examples\\apps\\freqresp.llb\\Frequency Response.vi";

	Object vi = lvapp.GetVIReference(viPath);				//Load the vi into memory
	vi.FPWinOpen = true;									//Open front panel

	/*
	' The Frequency Response vi has
	' 4 inputs - Amplitude, Number of Steps, Low Frequency & High Frequency and
	' 1 output - Response Graph.
	' To run the Frequency Response VI, we invoke the Run method with names of inputs
	' and outputs passed along with their values.
	*/
	StringArray saparamNames(5);
	saparamNames[0] = "Amplitude";
	saparamNames[1] = "Number of Steps";
	saparamNames[2] = "Low Frequency";
	saparamNames[3] = "High Frequency";
	saparamNames[4] = "Response Graph";
	_VARIANT paramNames = saparamNames;
	
	//initialize input values to the vi
	_VARIANT paramVals;
	paramVals.CreateAsArray( VT_VARIANT, 5 );
	paramVals.SetOneVariantInArray( vAmplitude, 0 );		//Amplitude value
	paramVals.SetOneVariantInArray( vNumberOfSteps, 1 );	//# steps value
	paramVals.SetOneVariantInArray( vLowFrequency, 2 );		//Low Frequency
	paramVals.SetOneVariantInArray( vHighFrequency, 3 );	//High Frequency
	/*
	' paramVals(4) will contain the value of Response Graph after
	' running the vi.
	*/
	
	//run the vi
	vi.Call(&paramNames, &paramVals);
	
	/*
	'paramVals(4) contains value for Response Graph -  a cluster of 2 arrays
	'In Active-X we view a cluster as an array of variants
	'so, a cluster of 2 elements x & y is an array of 2 variant elements
	*/
	
	_VARIANT vResponseGraph;
	paramVals.GetSubVariant(4, vResponseGraph);
	_VARIANT x, y;
	vResponseGraph.GetSubVariant(0, x);					// x co-ordinates
	vResponseGraph.GetSubVariant(1, y);					// y co-ordinates
	
	/*
	'Fill the Origin columns 1 & 2 with the graph co-ordinates
	'These columns are used by Origin to plot the chart
	*/
	dsx = x;
	dsy = y;
	
	///Sophy 12/27/2011 CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
	if ( bQuitLabview )
		lvapp.Quit();
	///end CLOSE_LABVIEW_AFTER_LOAD_DATA_FOR_QA_AUTO_TESTING
}