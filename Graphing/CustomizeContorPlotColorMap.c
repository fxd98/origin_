/*------------------------------------------------------------------------------*
 * File Name: CustomizeContourPlotColorMap.c									*
 * Creation: ER, 02/24/06														*
 * Purpose: Programming Example													*
 * Copyright (c) OriginLab Corp. 2006, 2007, 2008, 2009, 2010					*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
#include <Origin.h>

/////////////////////////////////////////////////////////////////////////////////
// This example shows you how to:
//			create a matrix and fill values using formula
//			plot matrix as contour plot
//			customize contour plot settings
//
void test_colormap()
{
	// First create a matrix page
	MatrixPage matPage;
	matPage.Create("Origin", CREATE_HIDDEN);

	// Get the first matrix object in first layer of that page 
	// Default page has only one layer and one object
	MatrixLayer matLayer = matPage.Layers(0);
	MatrixObject matObject(matLayer, 0);
	// Set row and col size
	matObject.SetNumRows(50);
	matObject.SetNumCols(50);

	// Now declare Matrix using this matrix object
	Matrix Mat(matLayer);
	// Set X, Y xo-ordinate start and end values
	Mat.SetXMin(0.1);
	Mat.SetXMax(5.0);
	Mat.SetYMin(0.01);
	Mat.SetYMax(0.5);
	// Put some data into the matrix
	for(int irow = 0; irow < Mat.GetNumRows(); irow++)
	{
		for(int icol = 0; icol < Mat.GetNumCols(); icol++)
		{
			Mat[irow][icol] = Mat.GetXValue(icol) * Mat.GetYValue(irow);
			
		}
	}
		
	// Create a color contour plot
	GraphPage gPage;
	gPage.Create("CONTOUR");
	GraphLayer gLayer = gPage.Layers(0);

	// Add matrix object to layer
	gLayer.AddPlot(matObject, IDM_PLOT_CONTOUR);
	gLayer.Rescale();
	
	// Duplicate this graph so user can see difference between
	// default graph and modifications made by code
	// Need LabTalk to duplicate
	LT_execute("win -d;");
	// Add label to this page
	GraphPage gPage2 = Project.Pages();
	gPage2.Label = "This is the default graph";
	gPage2.TitleShow = WIN_TITLE_SHOW_BOTH;

	// Set previous graph page active and continue
	gPage.SetShow();
	gPage.Label = "This is the modified graph";
	gPage.TitleShow = WIN_TITLE_SHOW_BOTH;
	
	// Turn off speed mode
	gLayer.Speed.Matrix.nVal = 0;
	
	// Point to data plot in active layer
	DataPlot dp = gLayer.DataPlots(0);

	// Get color map into tree object
	Tree tr;
	if( !dp.GetColormap(tr) ) 
	{
		out_str("Failed to get color map to tree");
		return;
	}
	// Uncomment next line to dump tree in script window to see what is available
	// out_tree(tr);
	// You can also examine tree from the Save Theme dialog by editing theme
	
	// Get current contour levels
	vector vLevels;
	vLevels = tr.Details.Levels.dVals;
	
	// Now let us change levels, colors etc
	// Following code uses some sample numbers for each
	// Modify code as applicable to your data
	
	// Change number of levels and values (arbitrary, change as needed)
	// In this example we are setting to a nonlinear scale, 2nd order polynomial
	int nLevels = 64;
	vLevels.SetSize(nLevels);
	for(int i = 0; i < nLevels; i++)
	{
		// Note that level needs to be set as % of (max-min)
		vLevels[i] = 100 * (i^2)/(nLevels^2);
		// Actual value in dialog/GUI will be:
		//printf("%f\n", dMin + vLevels[i]*(dMax-dMin)/100);
	}
	
	// Write new level info to tree
	tr.Details.Levels.dVals = vLevels;

	// Now let us set colors for each level, using RGB values
	vector<uint>	vColors;
	vColors.SetSize(nLevels);
	// Set colors using some abitrary nonlinear function for R, G, B
	// Could instead read from say a palette file and assign values
	for(i = 0; i < nLevels; i++)
	{
		int nRed = 255 * i / nLevels;  // linear in R
		int nGreen = 255 * i^2 / nLevels^2; // power of 2 in G
		int nBlue = 255 * sin(Pi * i / nLevels) ; // sinusoidal in B
		vColors[i] = RGB(nRed, nGreen, nBlue) | OCOLOR_RGB_FLAG_BIT;
		// The above sets colors using direct RGB values
		// The OCOLOR_RGB_FLAG_BIT indicates to Origin to interpret as RGB
	}
	
	// Set above and below color values, such as:
	tr.Details.BelowColor.nVal = 0;	// set to 4th color in Origin color list 
	tr.Details.AboveColor.nVal = 1; // set to 2nd color in Origin color list
	// So can use color list index, or RGB as shown previously

	// Write colors to tree
	tr.Details.Colors.nVals = vColors;

	// Set new color map properties using updated tree
	if( !dp.SetColormap(tr) )
		printf("Failed to set color map\n");

	// Turn off contour lines - need to turn of all of them
	// Need to first get back tree from updated plot
	if( !dp.GetColormap(tr) ) 
	{
		out_str("Failed to get color map to tree");
		return;
	}
	// Declare vector, get line setting, and reset to zero
	vector vecLines;
	vecLines = 	tr.Details.ShowLines.nVals;
	vecLines = 0;
	tr.Details.ShowLines.nVals = vecLines;
	// Set tree back again
	if( !dp.SetColormap(tr) )
		printf("Failed to set color map\n");
}
//
// Note also that settings can be saved as Theme or entire graph can be saved
// as template, so it is not always necessary to programmatically modify 
// settings for each graph.