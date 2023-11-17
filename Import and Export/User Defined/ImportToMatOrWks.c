/*------------------------------------------------------------------------------*
 * File Name:				 													*
 * Creation: 	OriginLab Corp. Jan. 17, 2007									*
 * Purpose: An example function that imports 20 integers into a					*
 *					matrix or a workbook										*
 * Copyright (c) OriginLab Corp. 2007											*
 * All Rights Reserved															*
 * 																				*
 * Modification Log:															*
 *------------------------------------------------------------------------------*/
 
#include <Origin.h>

////////////////////////////////////////////////////////////////////////////////////
// Import into a worksheet
////////////////////////////////////////////////////////////////////////////////////
static int _import_to_wks(Layer &lyTarget, vectorbase &vv)
{
	Worksheet wks(lyTarget);
	if ( !wks.IsValid() ) return 1;
	
	/// RVD 8/4/17 ORG-14200 IMPWIZ_UNDO_USERDEFINED
	//wks.SetSize(4, 5, true);
	wks.SetSize(4, 5, WSS_CLEAR_DATA|OCD_UNDO);
	/// end IMPWIZ_UNDO_USERDEFINED
	wks.SetColDesignations("y");

	for(int ii = 0; ii < 5; ii++)
	{
		Column cc = wks.Columns(ii);
		vectorbase &vb = cc.GetDataObject();
		vv.GetSubVector(vb, ii * 4, ii * 4 + 3);
	}
		
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// Import into a matrix
////////////////////////////////////////////////////////////////////////////////////
static int _import_to_mat(Layer &lyTarget, vectorbase &vv)
{
	MatrixLayer mlMat(lyTarget);
	if ( !mlMat.IsValid() ) return 1;
	
	mlMat.SetSize(1, 4, 5);
	
	Matrix mat(mlMat);
	if( !mat.IsValid() )
		return 1;
	
	return mat.SetByVector(vv, false);
}


////////////////////////////////////////////////////////////////////////////////////
// Function used in the Import Wizard to handle the importing of file 
// that has 20 integers into a matrix or a workbook.
////////////////////////////////////////////////////////////////////////////////////
int ImportIntegers(Layer &lyTarget, TreeNode &trFilter, LPCSTR lpcszFile, int nFile)
{
	if ( trFilter.Type.nVal != FILTER_TYPE_USERDEFINED )	return 1;
	
	//Read data from source file
	file fIn;
	vector<int> vN(20);
	
	if( fIn.Open(lpcszFile, file::modeRead | file::shareDenyWrite) == FALSE ) return 1;
	fIn.Read(vN, 20 * sizeof(int));	
	fIn.Close();
	
	Page pg = lyTarget.GetPage();
	int iPageType = pg.GetType();
	int iResult = 1;
	
	//Fill target page with numbers
	switch ( iPageType )
	{
	case EXIST_WKS:
		iResult = _import_to_wks(lyTarget, vN);		
		break;
	case EXIST_MATRIX:
		iResult = _import_to_mat(lyTarget, vN);
		break;
	default:
		break;
	}
	
	return iResult;
}
