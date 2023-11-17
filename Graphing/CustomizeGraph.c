/*------------------------------------------------------------------------------*
 * File Name: CustomizeGraphAxes.c												*
 * Creation: ER, 02/28/06														*
 * Purpose: Programming Example													*
 * Copyright (c) OriginLab Corp. 2006, 2007, 2008, 2009, 2010					*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
#include <Origin.h>

/////////////////////////////////////////////////////////////////////////////////
// This example shows you how to:
//			create worksheet and fill data using column formula
//			create a graph using existing template
//			customize various graph properties
//			apply an existing theme to graph
//
void test_graph()
{
	// Create a worksheet
	WorksheetPage wpg;
	wpg.Create("Origin");
	Worksheet wks = wpg.Layers(0);
	
	// Set worksheet with three columns: X,Y,Y
	wks.SetSize(-1, 3);
	// Set first col as type X - default col type is Y
	wks.Columns(0).SetType(OKDATAOBJ_DESIGNATION_X);
	// Note that in code below Data Range is used and 
	// with data range, one can over ride column type
	// when adding to range, so above statement of setting
	// first col to type X is not really necessary.
	// It is necessary only if you want the worksheet to 
	// display the column as type X
	
	// Fill X with some data
	Dataset dsX(wks, 0);
	dsX.Data(1.0, 10.0, 1.0);
	// Use Column Formula to fill cols 2, 3
	wks.Columns(1).SetFormula("10^col(1)");
	wks.Columns(1).ExecuteFormula();
	wks.Columns(2).SetFormula("col(1)^3");
	wks.Columns(2).ExecuteFormula();

	// Add long names to columns
	wks.Columns(0).SetExtendedLabel("x", RCLT_LONG_NAME);
	wks.Columns(1).SetExtendedLabel("10\+(x)", RCLT_LONG_NAME);
	wks.Columns(2).SetExtendedLabel("x\+(3)", RCLT_LONG_NAME);
	
	// Create double-Y axis graph
	GraphPage gpg;
	gpg.Create("DOUBLEY");
	// Declare layer objects for the two layers
	GraphLayer gly1 = gpg.Layers(0);
	GraphLayer gly2 = gpg.Layers(1);
	
	// Now create two data ranges with col(1,2) and col(1,3)
	// and add them to the two layers
	DataRange dr1, dr2;
	dr1.Add(wks, 0, "X");
	dr1.Add(wks, 1, "Y");
	dr2.Add(wks, 0, "X");
	dr2.Add(wks, 2, "Y");
	gly1.AddPlot(dr1, IDM_PLOT_LINE);
	gly1.Rescale();
	gly2.AddPlot(dr2, IDM_PLOT_LINESYMB);
	gly2.Rescale();

	// Change properties of y axis of layer 1
	Axis axY1 = gly1.YAxis;
	// Get axis scale format to tree
	Tree tr;
	tr = axY1.Scale;
	// Make changes to tree
	tr.Type.nVal = 1;
	tr.Value.dVal = 1;
	tr.MinorTicksCount.nVal = 0;
	// Put tree back and rescale layer
	axY1.Scale = tr;
	gly1.Rescale();
	// Note that you do not have to get to tree first, can also set directly
	// such as:
	// gly1.YAxis.Scale.Type.nVal = 1;
	// 
	// Change label display to scientific notation
	AxisObject aoY1 = gly1.YAxis.AxisObjects(AXISOBJPOS_LABEL_FIRST);
	// Get label properties to tree and print to command window to see what is available
	// Uncomment following two lines to get and print tree
	// tr = aoY1.LeftLabels;
	// out_tree(tr);
	// Change NumericFormat to scientific, which is 2nd item in drop-down in GUI
	// Can change directly without using tree
	aoY1.LeftLabels.NumericFormat.nVal = 1;

	// The doubley template sets the color of right y axis labels of layer 2 as blue
	// Let us change that to black
	AxisObject aoY2 = gly2.YAxis.AxisObjects(AXISOBJPOS_LABEL_SECOND);
	aoY2.RightLabels.Color.nVal = 0;
	// Also the color of the right Y axis and the ticks is blue - change that also to black
	aoY2 = gly2.YAxis.AxisObjects(AXISOBJPOS_AXIS_SECOND);
	aoY2.RightTicks.Color.nVal = 0;
	
	// The axis title text color for right y axis on 2nd layer is also blue - change to black
	// The title text needs to be accessed as a graph object referred to by name
	// Can right-click on object and check Label Control to see name - in this case name is "YR"
	GraphObject gobj;
	gobj = gly2.GraphObjects("YR");
	if( gobj )
		gobj.Label.Color.nVal = 0;

	// Now make modifications to the data plot themselves
	DataPlot dp1 = gly1.DataPlots(0);
	// The plot in layer 1 is a line plot - change the line color and thickness
	dp1.Curve.Line.Color.nVal = 1;
	dp1.Curve.Line.Width.dVal = 3.0;
	// The plot in layer 1 is a line+symbol plot
	DataPlot dp2 = gly2.DataPlots(0);
	// Change line color, symbol type, and symbol color
	// Use tree this time
	Tree trFormat;
	trFormat = dp2.Curve;
	// Uncomment next line to dump tree to command window
	// out_tree(trFormat);
	// Change line color
	trFormat.Line.Color.nVal = 2;
	// Change symbol shape and size
	trFormat.Symbol.Shape.nVal = 2;
	trFormat.Symbol.Size.nVal = 12;
	// Change symbol color to 12th entry in color list
	trFormat.Symbol.EdgeColor.nVal = 11;
	// Can set colors also using rgb directly
	int nRed = 150, nGreen = 30, nBlue = 70;
	trFormat.Symbol.EdgeColor.nVal = nRed + 256 * nGreen + 65536 * nBlue + OCOLOR_RGB_BITS;
	// Set properties by putting back tree
	dp2.Curve = trFormat;
	
	// Default legend in Double Y graph shows only one dataplot
	// Update to show both - legend is by default attached to layer 1
	gly1.LT_execute("legend -s;");
	// Move legend to be inside layer and make it bigger
	// Legend is a graph object with name "Legend"
	GraphObject gobjLegend = gly1.GraphObjects("legend");
	if( gobjLegend )
	{
		// Set units to inches and position from top, left in inches
		gobjLegend.Label.Dimension.Units.nVal = 0;
		gobjLegend.Label.Dimension.Left.dVal = 2.5;
		gobjLegend.Label.Dimension.Top.dVal = 1.25;
		// Make font size larger
		gobjLegend.Label.Font.Size.nVal = 30;
	}
	
	// Now add a new text label as title and position at top of page centered
	// Need to add label using LabTalk - add to layer 1
	gly1.LT_execute("label -n PgTitle ""some text"";");
	// Now get the added object as a graph object
	GraphObject gobjTitle = gly1.GraphObjects("PgTitle");
	if( gobjTitle )
	{
		// Set text string, bold style, font size and color
		gobjTitle.Text = "\\b(Double-Y graph showing two functions of x)";
		gobjTitle.Label.Font.Size.nVal = 36;
		gobjTitle.Label.Color.nVal = 3;
		// To position centered on page, we will center to layer x scale
		// First get x axis span
		double dX1 = gly1.XAxis.Scale.From.dVal;
		double dX2 = gly1.XAxis.Scale.To.dVal;
		double dXCenter = dX1 + 0.5 * (dX2 - dX1);
		printf("%f\n", dXCenter);
		// To set this value as X for center of label, need to use LabTalk
		string strCMD;
		strCMD.Format("PgTitle.x = %e;", dXCenter);
		gly1.LT_execute(strCMD);
		// X is centered
		// Now just change attachment to page and set y position at small y value
		gobjTitle.Label.Dimension.Attachment.nVal = 1;
		gobjTitle.Label.Dimension.Units.nVal = 0;
		gobjTitle.Label.Dimension.Top.dVal = 0.3;
	}

	// Now apply existing "ticks all in" theme to graph
	string strTheme = GetAppPath(TRUE) + "Themes\\Graph\\Ticks All In.OTH";
	gpg.ApplyFormat(strTheme);

}


////////////////////////////////////////////////////////////////////////////////////////////////////
/*
The following table summarizes what objects are available for the X, Y and Z axes in a graph:

Property		What object to use			X Axis				Y Axis					Z Axis
------------	-------------------			------------------	--------------------	------------
Minor grids		AXISOBJPOS_MINOR_GRID		VerticalMinorGrids	HorizontalMinorGrids	MinorGrids
Major grids		AXISOBJPOS_MAJOR_GRID		VerticalMajorGrids	HorizontalMajorGrids	MajorGrids
1st label		AXISOBJPOS_LABEL_FIRST		BottomLabels		LeftLabels				FrontLabels
2nd label		AXISOBJPOS_LABEL_SECOND		TopLabels			RightLabels				BackLabels
1st ticks		AXISOBJPOS_AXIS_FIRST		BottomTicks			LeftTicks				FrontTicks
2nd ticks		AXISOBJPOS_AXIS_SECOND		TopTicks			RightTicks				BackTicks

/*
////////////////////////////////////////////////////////////////////////////////////////////////////


