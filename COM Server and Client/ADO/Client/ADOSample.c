/*------------------------------------------------------------------------------*
 * File Name: ADOSample.c	 													*
 * Creation: 7/31/2002															*
 * Purpose: OriginC Source C file												*
 * Copyright (c) Originlab Corp. 2002											*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 * SY 10/28/2005 QA70-8170 v8.0320 OC_SAMPLE_FOR_ORACLE				
 * Olive 02/02/2008 UPDATE_DATASOURCE											*
 *------------------------------------------------------------------------------*/
 
////////////////////////////////////////////////////////////////////////////////////
// you can include just this typical header file for most Origin built-in functions and classes
// and it takes a reasonable amount of time to compile, 
#include <origin.h>

////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
// This sample shows how to use through OriginC COM interface to access ADO object. 
// Through ADO, SQL and Access database can be imported into worksheet and
// the data in the worksheet can be put back into database to replace old entry or 
// append new entry.
////////////////////////////////////////////////////////////////////////////////////

enum CursorLocationEnum
{
    adUseNone = 1,
    adUseServer = 2,
    adUseClient = 3,
    adUseClientBatch = 3
};

typedef enum LockTypeEnum {
    dbOptimisticValue = 1,
    dbPessimistic = 2,
    dbOptimistic = 3,
    dbOptimisticBatch = 5
} LockTypeEnum;


#define DB_SETUP_STR			"Provider=Microsoft.ACE.OLEDB.16.0;Password="";User ID=Admin;Data Source="
#define DB_FILE_FULLPATH		GetAppPath(TRUE) + "Samples\Import and Export\stars.mdb"
#define TABLE_NAME 				"Stars"
#define STR_ACCESS_FILE_EXT 	".mdb"


//***************************************
//function to retrieve data and put into a worksheet
//***************************************
void	ReadRecordset2Wks()
{	
	//create the ADODB.Recorset object
	Object	ocrs;
	ocrs = CreateObject("ADODB.Recordset");		
	if( !ocrs )
		return;
	
	// prepare database connection string, the connection string for OLEDB driver for Access database access
	string strConn = DB_SETUP_STR + DB_FILE_FULLPATH + ";";

	// prepare query string to retrieve data from database		
	string strQuery = "select * from " + TABLE_NAME + ";";

	
	//CursorLocation properties
	//adUseClient 3 
	//	Uses client-side cursors supplied by a local cursor library. Local cursor services 
	//	often will allow many features that driver-supplied cursors may not, so using this 
	//	setting may provide an advantage with respect to features that will be enabled. 
	//	For backward compatibility, the synonym adUseClientBatch is also supported. 
	//adUseNone 1 
	//	Does not use cursor services. (This constant is obsolete and appears solely for the 
	//	sake of backward compatibility.) 
	//adUseServer 2 
	//	Default. Uses data-provider or driver-supplied cursors. These cursors are sometimes 
	//	very flexible and allow for additional sensitivity to changes others make to the data source. 
	//	However, some features of the Microsoft Cursor Service for OLE DB (such as disassociated 
	//	Recordset objects) cannot be simulated with server-side cursors and these features will be 
	//	unavailable with this setting. 
	ocrs.CursorLocation = 3;
	
	
	//open method prototype and its explaination
	// recordset.Open(QueryString, ActiveConnection, CursorType, LockType, Options)
	// explaination:
	// QueryString : strQuery
	// 	Build by user using SQL language
	
	// ActiveConnection : strConn
	//	The dtring to indicate the connection method to a database. This sample only shows a string
	//	useing SQLOLEDB as provider to connect to a MS SQL database.
	
	// CursorType: optional
	//adOpenDynamic 2 
	//	Uses a dynamic cursor. Additions, changes, and deletions by other users are visible, 
	//	and all types of movement through the Recordset are allowed, except for bookmarks, 
	//	if the provider doesn't support them. 
	//adOpenForwardOnly 0 
	//	Default. Uses a forward-only cursor. Identical to a static cursor, except that you 
	//	can only scroll forward through records. This improves performance when you need to 
	//	make only one pass through a Recordset. 
	//adOpenKeyset 1 
	//	Uses a keyset cursor. Like a dynamic cursor, except that you can't see records that 
	//	other users add, although records that other users delete are inaccessible from your Recordset. 
	//	Data changes by other users are still visible. 
	//adOpenStatic 3 
	//	Uses a static cursor. A static copy of a set of records that you can use to find data 
	//	or generate reports. Additions, changes, or deletions by other users are not visible. 
	//adOpenUnspecified -1 
	//	Does not specify the type of cursor.
	
	//LockType : optional	
	//adLockBatchOptimistic 4 
	//	Indicates optimistic batch updates. Required for batch update mode. 
	//adLockOptimistic 3 
	//	Indicates optimistic locking, record by record. The provider uses optimistic locking, 
	//	locking records only when you call the Update method. 
	//adLockPessimistic 2 
	//	Indicates pessimistic locking, record by record. The provider does what is necessary 
	//	to ensure successful editing of the records, usually by locking records at the data source 
	//	immediately after editing. 
	//adLockReadOnly 1 
	//	Indicates read-only records. You cannot alter the data. 
	//adLockUnspecified -1 
	//	Does not specify a type of lock. For clones, the clone is created with the same lock 
	//	type as the original.  	
	ocrs.open(strQuery, strConn, 1, 3);	
	
	//initialize the worksheet object
	Worksheet		wks;
	wks.Create("origin");	
	
	//put data into the worksheet.
	BOOL			bRet = wks.ReadRecordset(ocrs);
	out_int("bRet = ", bRet);	
	
	if (ocrs.State == 1 ) //adStateOpen
         ocrs.Close();
}


// when bReplace is true, it will delete all entry firstly then copy data from current worksheet to table.
// NOTE if you wants to use append, and there is a Key field which not allow duplicate data, then you have to make sure after appending no duplicate data in key field/column. 
int  WriteWks2Recordset(bool bReplace = TRUE)
{
	Worksheet		wks = Project.ActiveLayer();
	if(!wks)
	{
		// assume use active wks
		out_str("NO active wks!");
		return -1;
	}
 
	Object ocrs;
	ocrs = CreateObject("ADODB.Recordset");
	if( !ocrs )
	{
		out_str(" ADO init error!");
		return -1;
	}
 
	// Get Access file name
	string strDBFileName = DB_FILE_FULLPATH;
	if ( !strDBFileName.IsFile() )
	{
		out_str("Invalid database file path!");
		return -1; 
	}
 
	// prepare and open database recordset
	string strConn = DB_SETUP_STR + strDBFileName; 
	ocrs.CursorLocation = 3;
	string strQuery = "Select * From "+ TABLE_NAME  + ";";
	ocrs.open( strQuery, strConn, 0, 3);
 
	int nOption = LAYWKSETRECORDSET_APPEND;	
	if ( bReplace )
		nOption = LAYWKSETRECORDSET_REPLACE;
 
	//write worksheet data to recordset.
	int nRet =  wks.WriteRecordset(ocrs, nOption);
	out_int("nRet = ", nRet); // 0 means no error
	if ( nRet )
		return nRet;
	
	// close dataset connection
	if (ocrs.State == 1 ) //adStateOpen
         ocrs.Close();
	return 0;
}



//***************************************
// Put update back from the active worksheet to the database. This sample handles only string fields
//***************************************
void	Update()
{	
	Worksheet		wks = Project.ActiveLayer();
	if(!wks)
	{
		out_str("Invalid worksheet! Before running, please activise a worksheet.");
		return;
	}
	
	//move the record index to the first one
	Object	ocrs;
	ocrs = CreateObject("ADODB.Recordset");
	if( !ocrs )
	{
		out_str(" ADO init error!");
		return;
	}
	
	// Get Access file name
	string strDBFileName = DB_FILE_FULLPATH;
	if ( !strDBFileName.IsFile() )
	{
		out_str("Invalid database file path!");
		return; 
	}
 
	// prepare and open database recordset
	string strConn = DB_SETUP_STR + strDBFileName; 
	ocrs.CursorLocation = 3;
	string strQuery = "Select * From "+ TABLE_NAME  + ";";
	ocrs.open( strQuery, strConn, 0, 3);
	
	
	ocrs.MoveFirst();
	
	int nRowIndex = 0;
	out_int("Total records = ", ocrs.RecordCount);	
	
	// Loop through the recordset and compare the value between recordset and Origin worksheet. 
	// If data is different, then update that field
	Object		ocfield;
	while( !ocrs.eof )
	{	
		
		out_int("nRowIndex = ", nRowIndex);
		if( nRowIndex > ocrs.RecordCount )
			break;
		
	
		for(int ii = 0; ii < ocrs.fields.Count; ii++)
		{
			ocfield = ocrs.fields(ii);
			
			//////Possible data types for a database field
			// String types
			//adLongVarChar 201 Indicates a long string value (Parameter object only). 
			//adLongVarWChar 203 Indicates a long null-terminated Unicode string value (Parameter object only). 
			//adVarChar 200 Indicates a string value (Parameter object only). 
			//adVarNumeric 139 Indicates a numeric value (Parameter object only). 
			//adVarWChar 202 Indicates a null-terminated Unicode character string. 
			//adWChar 130 Indicates a null-terminated Unicode character string (DBTYPE_WSTR). 
			//adBSTR 8 Indicates a null-terminated character string (Unicode) (DBTYPE_BSTR). 
			//adChapter 136 Indicates a four-byte chapter value that identifies rows in a child rowset (DBTYPE_HCHAPTER). 
			//adChar 129 Indicates a string value (DBTYPE_STR). 
			
			//Integer types
			//AdArray (Does not apply to ADOX.) 0x2000 A flag value, always combined with another data type constant, that indicates an array of that other data type.  
			//adBigInt 20 Indicates an eight-byte signed integer (DBTYPE_I8). 
			//adInteger 3 Indicates a four-byte signed integer (DBTYPE_I4). 
			//adSmallInt 2 Indicates a two-byte signed integer (DBTYPE_I2). 
			//adTinyInt 16 Indicates a one-byte signed integer (DBTYPE_I1). 
			//adUnsignedBigInt 21 Indicates an eight-byte unsigned integer (DBTYPE_UI8). 
			//adUnsignedInt 19 Indicates a four-byte unsigned integer (DBTYPE_UI4). 
			//adUnsignedSmallInt 18 Indicates a two-byte unsigned integer (DBTYPE_UI2). 
			//adUnsignedTinyInt 17 Indicates a one-byte unsigned integer (DBTYPE_UI1). 
			
			//float or bouble
			//adDecimal 14 Indicates an exact numeric value with a fixed precision and scale (DBTYPE_DECIMAL). 
			//adVarNumeric 139 Indicates a numeric value (Parameter object only). 
			//adDouble 5 Indicates a double-precision floating-point value (DBTYPE_R8). 
			//adNumeric 131 Indicates an exact numeric value with a fixed precision and scale (DBTYPE_NUMERIC). 
			//adSingle 4 Indicates a single-precision floating-point value (DBTYPE_R4). 
			
			
			// Binary type
			//adBinary 128 Indicates a binary value (DBTYPE_BYTES). 
			//adLongVarBinary 205 Indicates a long binary value (Parameter object only). 
			//adChapter 136 Indicates a four-byte chapter value that identifies rows in a child rowset 
			//	(DBTYPE_HCHAPTER). 
			
			// BOOL type
			//adBoolean 11 Indicates a boolean value (DBTYPE_BOOL). 
			
			// currency type
			//adCurrency 6 Indicates a currency value (DBTYPE_CY). Currency is a fixed-point number with 
			//		four digits to the right of the decimal point. It is stored in an eight-byte signed integer 
			//		scaled by 10,000. 
			
			// date/time types
			//adDate 7 Indicates a date value (DBTYPE_DATE). A date is stored as a double, the whole part of 
			//		which is the number of days since December 30, 1899, and the fractional part of which is 
			//		the fraction of a day. 
			//adDBDate 133 Indicates a date value (yyyymmdd) (DBTYPE_DBDATE). 
			//adDBTime 134 Indicates a time value (hhmmss) (DBTYPE_DBTIME). 
			//adDBTimeStamp 135 Indicates a date/time stamp (yyyymmddhhmmss plus a fraction in billionths) 
			//		(DBTYPE_DBTIMESTAMP). 
			//adFileTime 64 Indicates a 64-bit value representing the number of 100-nanosecond intervals 
			//		since January 1, 1601 (DBTYPE_FILETIME). 
			
			// misc
			//adEmpty 0 Specifies no value (DBTYPE_EMPTY). 
			//adError 10 Indicates a 32-bit error code (DBTYPE_ERROR). 
			//adGUID 72 Indicates a globally unique identifier (GUID) (DBTYPE_GUID). 
			//adIDispatch 9 Indicates a pointer to an IDispatch interface on a COM object (DBTYPE_IDISPATCH). 
			//		Note   This data type is currently not supported by ADO. Usage may cause unpredictable results.
			//adIUnknown 13 Indicates a pointer to an IUnknown interface on a COM object (DBTYPE_IUNKNOWN). 
			//		Note   This data type is currently not supported by ADO. Usage may cause unpredictable results.
			//adPropVariant 138 Indicates an Automation PROPVARIANT (DBTYPE_PROP_VARIANT). 
			//adUserDefined 132 Indicates a user-defined variable (DBTYPE_UDT). 
			//adVarBinary 204 Indicates a binary value (Parameter object only). 
			//adVariant 12 Indicates an Automation Variant (DBTYPE_VARIANT). 
			//		Note   This data type is currently not supported by ADO. Usage may cause unpredictable results.
			 

			//get the field type
			int nFieldType = ocfield.Type;
			
			//if the field type is a string type, do the update
			if( nFieldType == 200 || nFieldType == 201 || nFieldType == 202 || 
				nFieldType == 203 || nFieldType == 129 || nFieldType == 130 ||
				nFieldType == 8 )
			{
				string		strValue, strCell;
				strValue = "";			
				
				//get value from the recordset
				if( ocfield.Value != NULL )
					strValue = (string)ocfield.Value;
							
				//get value from worksheet
				if( wks.GetCell(nRowIndex, ii, strCell) )
				{
					//if two values do not match, update the database
					if( strValue.CompareNoCase(strCell) != 0 )
						ocrs.Update(ocfield.Name, strCell); 
					
				}
			}	
			
			
		}
		
		//advance the index
		ocrs.MoveNext();
		nRowIndex++;
		
		if( ocrs.eof )
			break;
		
	}	
		
}
 

/// SY 10/28/2005 QA70-8170 v8.0320 OC_SAMPLE_FOR_ORACLE
BOOL Odbtest()
{
	Object ocora;
	
	try
	{
		ocora = CreateObject("ADODB.Recordset");
	}
	catch(int nError)
	{
		out_str("Failed to create ADODB.Recordset");
		return FALSE;
	}
	
	// prepare database connection string
	// the connection string for MSSQL database 
	string strConn = "Provider=MSDAORA; Password=tiger; User ID=scott; Data Source=odbt";
	
	//prepare query string to retrieve data from database
	//this sample is to query from customers table in northwind sample database
	string strQuery = "select * from emp";

	ocora.CursorLocation = 3;
	try
	{
		ocora.open(strQuery, strConn, 1, 3);	
	}
	catch(int nError)
	{
		out_str("Failed to open Oracle database");
		return FALSE;
	}
	
	//initialize the worksheet object
	Worksheet wks = Project.ActiveLayer();
	
	//put data into the worksheet.
	BOOL			bRet = wks.PutRecordset(ocora);
	out_int("bRet = ", bRet);
	return bRet;
}
/// end OC_SAMPLE_FOR_ORACLE
