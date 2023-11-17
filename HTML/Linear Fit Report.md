# Linear Fit Report

**Origin Project:** *{{str://%X%G.%@Q}}*

**File Name:** *{{var://page.info.SYSTEM.IMPORT.FILENAME$}}*

**File Date:** *{{var://page.info.SYSTEM.IMPORT.FILEDATE$}}*

## Fitted Curve Graph

![{{graph://FitLine}}]("" =400x)

## Fit Parameters
|Parameter|Value|Error|
|---|---|---|
| Intercept | {{cell://FitLinear1!Parameters.Intercept.Value}} | {{cell://FitLinear1!Parameters.Intercept.Error}} |
| Slope | {{cell://FitLinear1!Parameters.Slope.Value}} | {{cell://FitLinear1!Parameters.Slope.Error}} |

*{{cell://FitLinear1!ANOVAs.Footnote}}*

## Fit Statistics
|Stat|Value|
|---|---|
| Pearson's r | {{cell://FitLinear1!RegStats.C1.Correlation}} |
| R-Square(COD) | {{cell://FitLinear1!RegStats.C1.RSqCOD}} |
| Adj. R-Square | {{cell://FitLinear1!RegStats.C1.AdjRSq}} |
