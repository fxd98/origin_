/*------------------------------------------------------------------------------*
 * File Name:	WordWriteReadTextUsingTables.c		 							*
 * Creation: 	ER, 08/24/05													*
 * Purpose:		OriginC example to write and read text to word doc tables		*
 * Copyright (c) OriginLab Corp. 2001, 2002, 2003, 2004, 2005, 2006, 2007		*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
////////////////////////////////////////////////////////////////////////////////////
//
#include <origin.h>
//
////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
// This file has two functions that demonstrate how to write to and read back
// textual information to a Word document using tables.
// The main functions are:
//		write_text_to_word_tables()
//		read_text_from_word_tables()
////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
// Word constants
uint wdGotoBookmark = -1;
uint wdEndOfDocument = -1;
#define wdGoToTable					2
#define wdGoToNext					2
////////////////////////////////////////////////////////////////////////////////////
// Static functions:
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// This static function opens word application and adds a blank document
static bool word_open_add_doc(Object& oWord, Object& oWordDoc, bool bVisible = false)
{
	// Launch Word application
	oWord = CreateObject("word.application");
	if( !oWord ) return false;
	// Set as visible/invisible
	oWord.Visible = bVisible;
	// Add a blank document
	oWordDoc = oWord.Documents.Add();
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// This static function goes to end of current Word doc and adds a bookmark
// with specified name
static bool word_doc_add_bookmark(Object& oWord, Object& oWordDoc, string str)
{
	// Go to end of document
	try
	{
		oWord.Selection.GoTo(What:= wdGotoBookmark, Name:="\endofdoc");
	}
	catch(int nError)
	{
		return false;
	}	
	// Add bookmark
	try
	{
		oWordDoc.Bookmarks.Add(str);
	}
	catch(int nError)
	{
		return false;
	}
	return true;	
}

////////////////////////////////////////////////////////////////////////////////////
// This static function adds text to current cursor location
static bool word_doc_add_text(Object& oWord, string str)
{
	// Add text
	try
	{
		oWord.Selection.TypeText(str);	
	}
	catch(int nError)
	{
		return false;
	}
	return true;
}
			
////////////////////////////////////////////////////////////////////////////////////
// This static function adds a line of text for table name,
// then a 1x1 table and text inside the table cell
static bool word_doc_add_table(Object& oWord, Object& oWordDoc, string strTableName, string strTableText)
{
	// Add text
	strTableName = strTableName + ":\r\n";
	if( ! word_doc_add_text(oWord, strTableName) )
		return false;
	
	// Add 1x1 table
	Object oWordTable;
	try
	{
		oWordTable = oWordDoc.Tables.Add(oWordDoc.Bookmarks.Item("\endofdoc").Range, 1, 1);
	}
	catch(int nError)
	{
		return false;
	}

	// Insert text in table cell
	try
	{
		oWordTable.Cell(Row := 1, Column := 1).Range.InsertAfter(Text := strTableText);
	}
	catch(int nError)
	{
		return false;
	}
	
	// Position cursor at end of doc
	try
	{
		oWord.Selection.GoTo(What:= wdGotoBookmark, Name:="\endofdoc");
	}
	catch(int nError)
	{
		return false;
	}
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////
// This static function saves the word document and closes the word application
static bool word_doc_save_and_close(Object& oWord, Object& oWordDoc, string strWordFile)
{
	// Save file
	bool bRet = true;
	try
	{
		oWordDoc.SaveAs(strWordFile);
	}
	catch(int nError)
	{
		bRet = false;
	}
	
	oWord.Quit();
	oWord = NULL;
	return bRet;
}

//This static function get info from only one table.
static void add_word_info_to_vector(Object& oWord, vector<string>& vsInfo)
{		
		Object oWordTable;
		string strInfo;
		
		//get the table info
		oWordTable = oWord.Selection.GoTo(What := wdGoToTable, Which := wdGoToNext);
		oWord.Selection.SelectCell();
		strInfo = oWord.Selection.Text;
		vsInfo.Add(strInfo);
}

////////////////////////////////////////////////////////////////////////////////////
// This static function read the info from the word table
static int read_info_from_word_table(Object& oWord, LPCSTR lpcszBookMark, vector<string>& vsInfo)
{
	string strTableBookMark(lpcszBookMark);
	
	try
	{
		//Goto table
		oWord.Selection.GoTo(What:= wdGotoBookmark, Name:=lpcszBookMark);
	}
	catch(int nError)
	{
		return -2; //not find the book mark
	}
	
	try
	{
		//get long name
		add_word_info_to_vector(oWord, vsInfo);
			
		//get unit
		add_word_info_to_vector(oWord, vsInfo);
		
		//get comment
		add_word_info_to_vector(oWord, vsInfo);
	}
	catch(int nError)
	{
		printf("Fail to find table: %s\n", strTableBookMark);
		return 0
	}
	
	return 1;
}


////////////////////////////////////////////////////////////////////////////////////
// Main functions:
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// This function creates a new Word document and writes out column properties such
// as long name, units and comments, in separate 1-cell table objects for each 
// property for each column
//
// Start with an Origin worksheet that has columns with properties such as 
// Long Name, Units, Comments filled in, and then call this function from 
// Command Window. 
int write_text_to_word_tables(string strWordFile = "c:\\test.doc")
{
	//
	Worksheet wks = Project.ActiveLayer();
	if( !wks ) -1;
	
	Object oWord, oWordDoc, oWordRng;
	int iRet;
	
	// Launch Word application and set as invisible
	if( !word_open_add_doc(oWord, oWordDoc, false) ) return -2;	
	
	// Loop over all columns of the Origin worksheet
	waitCursor wCur;
	int nCols = wks.GetNumCols();
	string str, strTableTitle, strTableText;
	Object oWordTable;
	for(int iCol = 0; iCol < nCols; iCol++)
	{
		// Add a bookmark for each column
		str.Format("Column%d", iCol);
		if( !word_doc_add_bookmark(oWord, oWordDoc, str) ) return -3;
		
		// Write out column name
		str.Format("COLUMN %s\r\n", wks.Columns(iCol).GetName());
		if( !word_doc_add_text(oWord, str) ) return -4;		
	
		// Insert table for long name 
		strTableTitle = "Long Name";
		strTableText = wks.Columns(iCol).GetLongName();
		if( !word_doc_add_table(oWord, oWordDoc, strTableTitle, strTableText) ) return -5;
		
		// Insert table for units 
		strTableTitle = "Units";
		strTableText = wks.Columns(iCol).GetUnits();
		if( !word_doc_add_table(oWord, oWordDoc, strTableTitle, strTableText) ) return -6;

		// Insert table for comments 
		strTableTitle = "Comments";
		strTableText = wks.Columns(iCol).GetComments();
		if( !word_doc_add_table(oWord, oWordDoc, strTableTitle, strTableText) ) return -7;

		// Insert a few line breaks
		if( !word_doc_add_text(oWord, "\r\n\r\n") ) return -8;
	}
	
	// Save Word document and close application
	if( !word_doc_save_and_close(oWord, oWordDoc, strWordFile) ) return -9;

	return 0;
}



////////////////////////////////////////////////////////////////////////////////////
// This function reads from a Word document and creates a worksheet and sets the 
// worksheet column properties such as long name, units and comments from the 
// contents of various tables in the Word document
//
// Call this function from the Command Window
int read_text_from_word_tables(string strWordFile = "c:\\test.doc")
{
	//Create a new WorkBook
	WorksheetPage wkp;
	
	wkp.Create("Origin");
	
	int nIndex = wkp.AddLayer();
	
	//Add a new worksheet
	Worksheet wks;
	wks = wkp.Layers(nIndex);
	if( !wks ) -1;

	Object oWord, oWordDoc, oWordRng;
	int iRet;
				
	// Launch Word application
	oWord = CreateObject("word.application");
	if( !oWord ) return false;
	// Set as visible/invisible
	oWord.Visible = true;
	
	//Open the Document
	/// Harvey 09/19/2017 ORG-14509-P1 CHECK_BOOKMARK_EXISTS
	//oWord.Documents.Open(strWordFile);
	oWordDoc = oWord.Documents.Open(strWordFile);
	/// END CHECK_BOOKMARK_EXISTS
	
	// Get the column numbers
	waitCursor wCur;
	int nCols = wks.GetNumCols();
	string strBookMark;
	
	//Loop
	vector<string> vsInfo;
	for(int ii = 0; ; ii++)
	{

		//Set the bookmark
		strBookMark.Format("Column%d", ii);
		
		//clear the info vector
		vsInfo.RemoveAll();
		
		/// Harvey 09/19/2017 ORG-14509-P1 CHECK_BOOKMARK_EXISTS
		if(!oWordDoc.Bookmarks.Exists(strBookMark))
			break;
		/// END CHECK_BOOKMARK_EXISTS
			
		//Read the data from word table
		int nRet = read_info_from_word_table(oWord, strBookMark, vsInfo);
		
		//If not find the bookmark, break;
		if(nRet < 0)
			break;
		
		//if not find the table, break;
		if(nRet < 1)
		{
			printf("It can't find table\n");
			break;
		}	
		
		//if the worksheet column not found ,add the new column
		if(nCols < ii + 1)
		{
			wks.AddCol();
		}
		
		//set the column property
		Column oCol = wks.Columns(ii);
		
		oCol.SetLongName(vsInfo[0]);
		
		oCol.SetUnits(vsInfo[1]);
		
		oCol.SetComments(vsInfo[2]);
	}
	
	
	// Save Word document and close application
	if( !word_doc_save_and_close(oWord, oWordDoc, strWordFile) ) return -9;

	return 0;
}


