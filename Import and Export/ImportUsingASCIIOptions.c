/*------------------------------------------------------------------------------*
 * File Name: ImportUsingASCIIOptions.c											*
 * Creation: ER, 02/27/06														*
 * Purpose: Programming Example													*
 * Copyright (c) OriginLab Corp. 2006, 2007, 2008, 2009, 2010					*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
#include <Origin.h>

/////////////////////////////////////////////////////////////////////////////////
// This example shows you how to:
//			get a list of files in a folder using wild card
//			get worksheet ASCII options and change a setting
//			import all files in list using updated options
//
void test_ascii_options()
{
	// First get a list of all files in "\Samples\Import and Export" subfolder 
	// with wild card setting of S*.dat
	// 


	string strPath = GetAppPath(TRUE) + "Samples\\Import and Export\\";
	string strSpec = "S*.dat";
	StringArray saFileNames;
	if( !okutil_find_files_from_name_or_ext(&saFileNames, strPath, strSpec) )
	{
		out_str("Failed to find any file");
		return;
	}
	
	// Create a new workbook
	WorksheetPage wpg;
	wpg.Create("Origin");
	Worksheet wks = wpg.Layers(0);
	
	// We will import all files into the active worksheet
	// by adding new columns for each file
	
	// First get ASCII options from worksheet into a tree
	ASCIMP ascimp; 
	wks.GetASCIMP(ascimp);
	Tree tr;
	tr = ascimp;
	// Uncomment following line to print tree to command window
	// out_tree(tr);

	// See ASCIMP structure definition in OC_Types.h for details on all keywords 

	// Change import mode to add columns
	tr.iMode.nVal = ASCIMP_MODE_APPEND_COLS;
	// Turn on adding file name to comments for each col, but no file path
	tr.flags.nVal |= AI_FLAG_FNAME_TO_COL_COMMENTS | AI_FLAG_REMOVE_FNAME_PATH;
	// Turn off renaming worksheet to file name
	tr.iRenameWks.nVal = 0;

	// Put new settings back from tree
	ascimp = tr;
	wks.SetASCIMP(ascimp);
	 
	// Loop over file names
	int numFiles = saFileNames.GetSize();
	for(int i = 0; i < numFiles; i++)
	{
		// Import the file and set type of first column of imported data to type X
		ASCIMPRESULT ascimpres;		
		if( 0 == wks.ImportASCII(strPath + saFileNames[i], ascimp, &ascimpres) )
			wks.Columns(ascimpres.nC1).SetType(OKDATAOBJ_DESIGNATION_X);
	}
		
	// Turn on display of column labels so user can see file name
	wks.CheckAddLabelByType(TRUE, RCLT_COMMENT, -1, TRUE);
}