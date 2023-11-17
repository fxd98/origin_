/*------------------------------------------------------------------------------*
 * File Name: ReportCreation.c
 * Creation:  Larry 02/05/08
 * Purpose: riginC example for COM connectivity with MSOffice
 * Copyright (c) OriginLab Corp.	2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010 *
 * All Rights Reserved *
 * *
 * Modification Log: *
 *------------------------------------------------------------------------------*/
 
////////////////////////////////////////////////////////////////////////////////////
//
#include <origin.h>
//
////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
// This example illustrates accessing MSOffice programs from Origin C using COM.
// Development Kit is required for this program to function.
// The function CreateReport() does the following:
//	1> Opens an Excel Workbook named ExcelData.XLS from the \Samples\COM Server and Client\MS Office\Client\ folder
//	2> Reads the data in all (2) worksheets of this sample Excel file
//	3> Graphs the data in Origin using a custom template, and performs nonlinear curve fitting
//	4> Exports the resulting graph with fit curve, to a BMP file
//	5> Creates a new Word document for each worksheets, using custom template
//	6> Places the graph image, and other relevant information into the Word document
//
// To run this function, first compile this file, then go to the Origin Script Window
// (Origin menu item: Window | Script Window), and type the following and hit Enter:
//		CreateReport

void CreateReport()
{
	
	// Bring up wait cursor
	waitCursor wCursor;	
	printf("Processing...\n");
	
	// All files associated with this C file are in the subfolder \Samples\COM Server and Client\MS Office\Client\
	// under the Origin EXE path
	string strFldPath = GetAppPath(true) + "Samples\\COM Server and Client\\MS Office\\Client\\";

	// Declare Excel objects
	Object	oExcel, oExcelWkbks, oExcelWkbk, oExcelWksh;
	
	// Create an Excel application object and set it as invisible
	oExcel = CreateObject("excel.application");
	oExcel.Visible = false;
	
	// Get the workbooks collection of the Excel object
	oExcelWkbks = oExcel.Workbooks;
	
	// Open the workbook with data to be analyzed
	oExcelWkbk = oExcelWkbks.Open(strFldPath+ "ExcelData.xls");
		
	// Create a Word application object and set as invisible
	Object oWord, oWordDoc;
	oWord = CreateObject("word.application");
	oWord.Visible = false;
	
	// Loop over each worksheet in the Excel workbook
	foreach (oExcelWksh in oExcelWkbk.Worksheets)
	{	
		// Read the data collection date
		string strDate = oExcelWksh.Cells(1,2).Value;
		// Read the SampleID
		string strSampleID = oExcelWksh.Cells(2,2).Value;
		// Read the user name
		string strUserName = oExcelWksh.Cells(3,2).Value;
		// Read the number of points in the dataset
		int npts = oExcelWksh.Cells(7,2).Value;
		// Read start x value
		double xstart = oExcelWksh.Cells(5,2).Value;
		// Read step x value
		double xstep = oExcelWksh.Cells(6,2).Value;
	
		// Define Origin vectors to hold data
		vector vecX, vecY;
		vecX.SetSize(npts);
		vecY.SetSize(npts);

		// Fill x vector using x start and x step values
		for (int ii = 0; ii<npts; ii++)
		{
			vecX[ii] = xstart + ii * xstep;
		}
		// Read y values direclty into vector y
		vecY = oExcelWksh.Range(oExcelWksh.Cells(10,1), oExcelWksh.Cells(npts+10-1,1)).Value;
		
		// Create a workbook to save data
		WorksheetPage wkbk;
		if( !wkbk.Create("Origin") )
		{
			printf("Failed to create new workbook...\n");
		}
		Worksheet wks = wkbk.Layers();
		wks.SetName(strSampleID);
		wks.SetSize(npts, 3);
		
		wks.Columns(0).SetLongName("Raw X");
		wks.Columns(1).SetLongName("Raw Y");
		wks.Columns(2).SetLongName("Fitted Y");
		
		Dataset dsX(wks, 0);
		Dataset dsY(wks, 1);
		dsX = vecX;
		dsY = vecY;
		
		// Using LabTalk X-Functions to fit curve	
		LT_execute("nlbegin(2, Gauss, opt);");
		LT_execute("nlfit;");
		// Generate fitted Y values
		LT_execute("col(c) = fit( col(a) );");
		LT_execute("nlend();");
		
		// Get LabTalk operation result tree object to OC
		LT_execute("Tree tt;");
		LT_execute("tt = opt;");
		TreeNode tr;
		Project.GetTree("tt", tr);
		
		// Save the fitted parameter values
		double centroid, width, area, offset;
		offset = tr.y0.dVal;
		centroid = tr.xc.dVal;
		width = tr.w.dVal;
		area = tr.A.dVal;	
		
		// Create an Origin graph, using a custom template
		GraphPage gp;
		bool bRetG = gp.Create(strFldPath+"Custom.OTP");
	
		// Point to active layer in current graph page
		GraphLayer gl = gp.Layers();
					
		// Declare a curve object using x,y columns of worksheet
		Curve crvRaw(wks, 0, 1);
		Curve crvFit(wks, 0, 2);
	
		// Plot data curve to active layer
		int	nRawPlot = gl.AddPlot(crvRaw, IDM_PLOT_SCATTER);
		int nFitPlot = gl.AddPlot(crvFit, IDM_PLOT_LINE);
		
		///end
		gl.Rescale();
		
		string strUffPath = GetAppPath(false);
		//string imgFilePath = strFldPath + "image.bmp";
		string imgFilePath = strUffPath + "image.bmp";
		bool bRet = export_page_to_image(imgFilePath, "BMP", gp, 640, 480, 24, 0, false);
	
		// Add a new document to the Word application using custom template
		oWordDoc = oWord.Documents.Add(Template := strFldPath + "ReportTemplate.dot");

		// Insert BMP image into document
		//oWordDoc.InlineShapes.AddPicture(FileName := strFldPath + "image.bmp");
		oWordDoc.InlineShapes.AddPicture(FileName := imgFilePath);
	
		// Get info. on dataset, and fit parameters, and place them in table cells
		oWordDoc.Tables(1).Cell(Row := 3, Column := 2).Range.InsertAfter(Text := strDate);
		oWordDoc.Tables(1).Cell(Row := 5, Column := 2).Range.InsertAfter(Text := strSampleID);
		oWordDoc.Tables(1).Cell(Row := 4, Column := 2).Range.InsertAfter(Text := strUserName);

		string strTemp;
		strTemp.Format( "%f", centroid);
		oWordDoc.Tables(1).Cell(Row := 7, Column := 2).Range.InsertAfter(Text := strTemp);
		strTemp.Format( "%f", width);
		oWordDoc.Tables(1).Cell(Row := 8, Column := 2).Range.InsertAfter(Text := strTemp);
		strTemp.Format( "%f", area);
		oWordDoc.Tables(1).Cell(Row := 9, Column := 2).Range.InsertAfter(Text := strTemp);
		strTemp.Format( "%f", offset);
		oWordDoc.Tables(1).Cell(Row := 10, Column := 2).Range.InsertAfter(Text := strTemp);

		// Save the Word document to a unique file with SampleID as part of file name
		//oWordDoc.SaveAs(FileName := strFldPath + "Report_" + strSampleID + ".doc");
		//printf("Report saved as %s\n",strFldPath + "Report_" + strSampleID + ".doc");
		oWordDoc.SaveAs(FileName := strUffPath + "Report_" + strSampleID + ".doc");
		printf("Report saved as %s\n",strUffPath + "Report_" + strSampleID + ".doc");
		
	}
	
	// Close the Excel and Word applications
	oExcel.Quit();
	oWord.Quit();
	printf("Done!\n");
}