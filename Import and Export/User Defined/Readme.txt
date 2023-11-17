Description: In the source file, there are 20 integers in binary format. This example shows you how to import them with Origin C functions into a 5*4 matrix or a workbook. 

Usage:
  1. Make a workbook or a matrix active. Then open the Import Wizard.
  2. On the Source page:
     1) Select "User Defined" for "Data Type", choose "twenty_intergers_a.dat" and/or "twenty_intergers_b.dat" for "file".
     2) Choose either "Worksheet" or ''Matrix'' for "Target Window" and choose an import mode.
  3. Click "Next" twice. On the User Defined Filters page, make sure that "Origin C Source File" points to "ImportToMatOrWks.c" under the data folder and "Origin C Function" is "ImportIntegers". Click "Finish" to complete the importing.
