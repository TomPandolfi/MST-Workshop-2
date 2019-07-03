#pragma once


void buildMatrixDialogButtons(Sheet* pg, int i);			// rows and columns	
void buildTableDialogButtons(Sheet* pg, int i);				// column of values
void buildLiteralDialogButtons(Sheet* pg, int i);			// column of literals
void buildBuss2InPinNamesDialogButtons(Sheet* pg, int i);	// column of literals
void buildLedDialogButtons(Sheet* pg, int i);				// two radio groups
void buildUniversalPinDialogButtons(Sheet* pg, int i);
void buildOutputDialogButtons(Sheet* pg, int i);			// number and literal
void buildInputDialogButtons(Sheet* pg, int i);				// number, literal, value

void buildDetailsDialogButtons(Sheet* pg, int i);			// this is how the details show
void buildDetailsDialogButtonsFromComponentType(Sheet* pg, LPCTSTR componentType); // details from button group hit

void buildUserLabelDialogButtons(Sheet* pg, int i);			// just one literal
void buildValueFontDialogButtons(Sheet* pg, int i);			// just one literal
void buildBussInDialogButtons(Sheet* pg, int i);			// just one literal
void buildBussOutDialogButtons(Sheet* pg, int i);			// one literal plus N radio buttons
void buildSubroutineNameDialogButtons(Sheet* pg, int sheetIndex);	// just one literal
void buildFsmDialogButtons(Sheet* pg, int i);				// literal and 3 checkboxes
void buildLogicOptionsDialogButtons(Sheet* pg, int i);		void buildLogicLogicsDialogButtons(Sheet* pg, int i);
void buildDtoAOptionsDialogButtons(Sheet* pg, int i);		void buildDtoALogicsDialogButtons(Sheet* pg, int i);
void buildFindComponentDialogButtons(Sheet* pg);
void buildHelpComponentDialogButtons(Sheet* pg);
void buildUTFDialogButtons(Sheet* pg, int i);			
void buildRotationDialogButtons(Sheet* pg, int i);
void buildRotation3dDialogButtons(Sheet* pg, int i);


void buildMultipleValuesDialog(Sheet*, int i,
	vector<string> doubleLabels, vector<double> doubleSettingIndex, int doubleFieldWidth,
	vector<string> checkLabels, vector<bool> checkValues,
	vector<string> radioLabels, int radioIndex,
	void(*_returnMethod2)(Sheet*, int, double*, bool*, int));

void buildMultipleValuesDialog(Sheet*, int i,
	vector<string> doubleLabels, vector<double> doubleSettingIndex, int doubleFieldWidth,
	void(*_returnMethod2)(Sheet*, int, double*, bool*, int));

bool processModalDialogComponent(Sheet* pg, int x, int y);
void mouseMoveInModalDialog(Sheet* pg, int x, int y);
void mouseUpInModalDialog(Sheet* pg);

void drawModalScreenDialog(Sheet* pg, HDC hDC);
bool keyboardDialogChar(Sheet* pg, int key);
bool keyboardDialogKeydown(Sheet* pg, int key);
