##Useful String Expression

The ***str*** notation can take any string expression.

The ***var*** notation only support a single variable.

|Meaning|Display|Syntax|
|---|---|---|
| Current Project File Path | {{str://%X%G.%@Q}}|{\\{str://%X%G.%@Q}} |
| Current Login User Name | {{str://%@S}}|{\\{str://%@S}} |
| Current PE Folder | {{str://%@F}}|{\\{str://%@F}} |
| Current Program Folder | {{var://system.path.program$}}|{\\{var://system.path.program$}} |
| Current Date | {{str://$(@D,D10)}}|{\\{str://$(@D,D10)}} |
| File Date (Default) | {{var://page.info.SYSTEM.IMPORT.FILEDATE$}}|{\\{var://page.info.SYSTEM.IMPORT.FILEDATE$}} |
| File Date (Custom) | {{str://$(page.info.SYSTEM.IMPORT.FILEDATE, D10)}}|{\\{str://$(page.info.SYSTEM.IMPORT.FILEDATE, D10)}} |
| File Name | {{var://page.info.SYSTEM.IMPORT.FILENAME$}}|{\\{var://page.info.SYSTEM.IMPORT.FILENAME$}} |
