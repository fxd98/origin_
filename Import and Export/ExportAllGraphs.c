/*------------------------------------------------------------------------------*
 * File Name: ExportAllGraphs.c													*	
 * Creation: ER, 03/16/06														*
 * Purpose: Programming Example													*
 * Copyright (c) OriginLab Corp. 2006, 2007, 2008, 2009, 2010					*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
#include <Origin.h>

/////////////////////////////////////////////////////////////////////////////////
// This example shows you how to:
//			loop thru all graph pages in current project
//			export all graph pages as BMP with fixed width in pixels


void test_BMP()
{
	// Loop over all pages in project
	foreach( PageBase pgb in Project.Pages )
	{
		// If it is a graph page...
		if( EXIST_PLOT == pgb.GetType() )
		{
			// Set export format in string
			string strFormat = "BMP";
			// Set width in pixels - change as desired
			int nWidth = 1024;
			// Get page properties and set (approx) height, maintaining aspect ratio
			GraphPage gpg = pgb;
			int nHeight = (int) (nWidth * gpg.Dimension.Height.dVal / gpg.Dimension.Width.dVal);
			// Set bits per pixel
			int nBitsPerPixel = 8;
			// Set export file path to be in the User Files folder - change as desired
			string strImagePath = GetAppPath() + gpg.GetName() + ".BMP";
			printf("File: %s\n", strImagePath);
			// Call function to export page with desired settings
			BOOL bRet = export_page_to_image(strImagePath, strFormat, pgb, nWidth, nHeight, nBitsPerPixel);
			// Note that the above function is overloaded and another version is available that
			// can export according to INI settings or can prompt user with dialog for settings.
			//
			// Report if error
			if( !bRet )
				printf("Failed to export graph page: %s\n", pgb.GetName()); 
		}
	}
}