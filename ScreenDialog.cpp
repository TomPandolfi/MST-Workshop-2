#include "stdafx.h"
#include <vector>
#include "Sheet.h"
#include "ScreenDialog.h"
#include "Text.h"
#include "Details.h"
#include "color.h"
#include "MethodComponents.h"
#include "LogicComponents.h"
#include "StickyConnections.h"
#include "Fileio.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

extern std::vector<Detail*>detailList;
extern string cleanFileName(LPTSTR filePath);

extern vector< InnerSheet*> isL;
extern Sheet* page;


#define GLOBAL_CHECKBOX_WIDTH  170

void(*finishDialog)(Sheet* pg, int);
void finishLogic2Dialog(Sheet* pg, int i);
void finishDtoADialog(Sheet* pg, int i);
void finishTableDialog(Sheet* pg, int i);
void finishMatrixDialog(Sheet* pg, int i);
void finishLiteralDialog(Sheet* pg, int i);
void finishUserLabelDialog(Sheet* pg, int i);
void finishValueFontDialog(Sheet* pg, int i);
void finishTestDialog(Sheet* pg, int i);
void finishPresetDialog(Sheet* pg, int i);
void finishRemoteDialog(Sheet* pg, int i);
void finishLedDialog(Sheet* pg, int i);
void finishUniversalPinDialog(Sheet* pg, int i);
void finishBussInDialog(Sheet* pg, int i);
void finishBussOutDialog(Sheet* pg, int i);
void finishBuss2InNamesDialog(Sheet* pg, int i);
void finishFsmDialog(Sheet* pg, int i);
void finishLogicOptionsDialog(Sheet* pg, int i);
void finishLogicLogicsDialog(Sheet* pg, int i);
void finishDtoAOptionsDialog(Sheet* pg, int i);
void finishDtoALogicsDialog(Sheet* pg, int i);
void finishOutputDialog(Sheet* pg, int i);
void finishInputDialog(Sheet* pg, int i);
void finishSubroutineNameDialog(Sheet* pg, int i);
void finishUTFDialog(Sheet* pg, int i);
void finishRotationDialog(Sheet* pg, int i);
void finishRotation3dDialog(Sheet* pg, int i);

// Matrix:			N numerical values
// Table:			N numerical values
// Literal:			N string values
// User Label:		1 string value
// BussInOut		1 string value
// Constant:		1 numerical value						(buildMultipleValuesDialog)
// Buffer:			1 numerical value						(buildMultipleValuesDialog)
// Fifo				1 numerical value						(buildMultipleValuesDialog)
// Permute			1 numerical value						(buildMultipleValuesDialog)
// Counter			1 numerical value						(buildMultipleValuesDialog)
// Buss2In Count	1 numerical value (uses size of literal)
// Buss2In Names	N string values
// Button:			1 numerical value, 1 checkbox, 1 choice of 3(buildMultipleValuesDialog)
// FSM				1 literal, 3 Checkboxes
// matrix join		1 radio group of 3						(buildMultipleValuesDialog)
// Least Squares	1 radio group of 4						(buildMultipleValuesDialog)
// Matrix Split		1 radio group of 3						(buildMultipleValuesDialog)
// Image Threshold	1 radio group of 5						(buildMultipleValuesDialog)
// Image Erode/di.	1 radio group of 3						(buildMultipleValuesDialog)
// 2D Delta			1 radio group of 3						(buildMultipleValuesDialog)
// Knob:			4 numerical values						(buildMultipleValuesDialog)
// Meter:			2 numerical values						(buildMultipleValuesDialog)
// Least Squares N	2 numerical values @@@ maybe
// Torus			2 numerical values, 1 radio group of 3	(buildMultipleValuesDialog)
// Wave Generator	2 numerical values						(buildMultipleValuesDialog)
// Sphere			2 numerical values, 1 radio group of 3	(buildMultipleValuesDialog)
// Cylinder			2 numerical values, 2 checkboxes, 1 radio group of 3	(buildMultipleValuesDialog)
// Lathe			1 numerical value, 2 checkboxes, 1 radio group of 3		(buildMultipleValuesDialog)
// Extrude			1 numerical value, 2 checkboxes, 1 radio group of 3		(buildMultipleValuesDialog)
// Cone				2 numerical values, 1 radio group of 3	(buildMultipleValuesDialog)
// Matrix Size		2 numerical values						(buildMultipleValuesDialog)
// Switch:			1 numerical value (uses setting.size())
// Literal Size		1 numerical value (uses literal.size())
// Table Size		1 numerical value (uses setting.size() - 2)
// LED				1 numerical value, 2 groups of radio buttons (4 and 8)
// Chart			1 numerical value, 1 radio group of 5	(buildMultipleValuesDialog)
// String Joiner	1 numerical value, 1 radio group of 3	(buildMultipleValuesDialog)
// 2dVector			1 numerical value, 9 checkboxes			(buildMultipleValuesDialog)
// 2d Geom			1 numerical value, 10 checkboxes		(buildMultipleValuesDialog)
// Slider			4 numerical values						(buildMultipleValuesDialog)
// xy Slider		4 numerical values						(buildMultipleValuesDialog)
// Logic options	3 numerical values but calculated
// Logic Logics		complex
// D to A options	2 numerical values -- uses one setting and one size of elements
// D to A logics	complex
// Rack				5 numerical values						(buildMultipleValuesDialog)
// Geom				6 numerical values						(buildMultipleValuesDialog)
// universal pin:	N checkboxes
// trig				1 checkbox
// component details - big paragraph





// code:
#define MODAL_CODE_Frame				 0
#define MODAL_CODE_Ok					 1
#define MODAL_CODE_Cancel				 2
#define MODAL_CODE_InputButton			 3
#define MODAL_CODE_OutputButton			 4
#define MODAL_CODE_FeedbackButton		 5
#define MODAL_CODE_TagNumber			 6
#define MODAL_CODE_InputLabel			 7
#define MODAL_CODE_OutputLabel			 8
#define MODAL_CODE_NumericalValue		 9
#define MODAL_CODE_LiteralValue			10
#define MODAL_CODE_Checkbox				11
#define MODAL_CODE_PlaceComponent		12
#define MODAL_CODE_CheckAll				13
#define MODAL_CODE_ScanForLabels		14
#define MODAL_CODE_ScanForUserLabels	15
#define MODAL_CODE_ScanForNotes			16
#define MODAL_CODE_PinName				17
#define MODAL_CODE_StaticText			18
#define MODAL_CODE_RadioGroup			19


//extern int modalDialogComponent; // block everything for this modal dialog
extern int modalDialogKeyboardEntry; // enables keyboard entry and holds the button"s index



void dialUniversalText(HDC hDC, int x, int y, int width, int height, std::string text, int cursorPos, int selectionWidth) {

	//contex.lineWidth = 1;
	//if (contex == ctx)
	//	contex.font = '9px ' + defaultFontFamily;
	//else
	//	contex.font = '9px ' + default_2D_FontFamily;

	// create the white frame
	HBRUSH fillColor = GetColorBrush(RGB(255, 255, 255));
	HBRUSH frameColor = GetColorBrush(0);
	RECT aRect;
	aRect.left = x;
	aRect.top = y;
	aRect.right = x + width;
	aRect.bottom = y + height;
	FillRect(hDC, &aRect, fillColor);
	FrameRect(hDC, &aRect, frameColor);

	int selStart = calcStringWidthFont(hDC, text.substr(0, cursorPos));
	int selEnd = calcStringWidthFont(hDC, text.substr(0, cursorPos+selectionWidth));

	// if selection range color it
	if (selectionWidth != 0) {
		int left;
		int right;
		if (selEnd > selStart) {
			left = selStart;
			right = selEnd;
		}
		else {
			left = selEnd;
			right = selStart;
		}
		HBRUSH fillColor = GetColorBrush(RGB(255, 205, 155));
		RECT aRect;
		aRect.left = x + left +1;
		aRect.top = y+1;
		aRect.right = x + right +1;
		aRect.bottom = y + height-1;
		FillRect(hDC, &aRect, fillColor);
	}
	else // draw the red cursor
	if (cursorPos >= 0) {
		if (selStart < width) {
			SelectObject(hDC, GetColorPen(RGB(255, 0, 0), 1));
			BeginPath(hDC);
			MoveToEx(hDC, x + selStart + 1, y + height, NULL);
			LineTo(hDC, x + selStart + 1, y);
			EndPath(hDC);
			StrokePath(hDC);
			SelectObject(hDC, GetColorPen(0, 1)); // put it back for the next guy
		}
	}

	//  write the text
	if (text.size()) {
		std::string trimText = trimTextToFit(hDC, text, width);
		drawTextString(hDC, x + 2, y + 2, 0, trimText);
	}
}



class ModalDialogButton {
public:
	int x;
	int y;
	int w;
	int h;
	int code;
	int line;
	double index;
	std::string state;
	bool enableKeyboard = false;
	std::string keyboardText;
	int keyboardCursorPos;
	int selectionWidth = 0;
//	string pinName;
	ModalDialogButton(int inx, int iny, int inw, int inh, int incode, int in_line, double inindex, std::string instate);
	//ModalDialogButton(int inx, int iny, int inw, int inh, int incode, int in_line, double inindex, LPCTSTR instate);
	void draw(HDC hDC);
	bool isHit(int inx, int iny);
};

bool ModalDialogButton::isHit(int inx, int iny) {
	if (inx < x)
		return false;
	if (iny < y)
		return false;
	if (inx > x + w)
		return false;
	if (iny > y + h)
		return false;
	return true;
}


ModalDialogButton::ModalDialogButton(int inx, int iny, int inw, int inh, int incode, int in_line, double inindex, std::string instate) {
	x = inx;
	y = iny;
	w = inw;
	h = inh;
	code = incode;
	line = in_line; // for radio group this is the on/off for the line
	index = inindex; // for the radio group this is the group
	state = instate;
}

void ModalDialogButton::draw(HDC hDC) {
	std::string text;
	COLORREF fillStyle = 0xFFFFFF;

	switch (code) {
		case MODAL_CODE_Frame:    text = state;       fillStyle = 0xEEEEEE; break;
		case MODAL_CODE_Ok:       text = "   OK";     fillStyle = 0xFFFFFF; break;
		case MODAL_CODE_CheckAll: text = "Check All"; fillStyle = 0xFFFFFF; break;
		case MODAL_CODE_Cancel:   text = "Cancel";    fillStyle = 0xFFFFFF; break;
		case MODAL_CODE_InputButton:
			switch (state[0]) {
				case 'T': text = "True";  fillStyle = 0xFFD0FF; break;
				case 'F': text = "False"; fillStyle = 0xD0FFFF; break;
				case 'P': text = "Rise";  fillStyle = 0xFFFFD0; break;
				case 'N': text = "Fall";  fillStyle = 0xE0E0D0; break;
				case 'X': text = " ";     fillStyle = 0xF0F0F0; break;
			}
		break;
		case MODAL_CODE_FeedbackButton:
			switch (state[0]) {
				case 'T': text = "True";  fillStyle = 0xFFD0FF; break;
				case 'F': text = "False"; fillStyle = 0xD0FFFF; break;
				case 'X': text = " ";     fillStyle = 0xF0F0F0; break;
			}
		break;
	case MODAL_CODE_OutputButton:
		switch (state[0]) {
			case 'T': text = "True";  fillStyle = 0xFFD0FF; break;
			case 'F': text = "False"; fillStyle = 0xD0FFFF; break;
			case 'X': text = " ";     fillStyle = 0xF0F0F0; break;
			case '0': text = "In 0";  fillStyle = 0xFFFFD0; break;
			case '1': text = "In 1";  fillStyle = 0xFFFFD0; break;
			case '2': text = "In 2";  fillStyle = 0xFFFFD0; break;
			case '3': text = "In 3";  fillStyle = 0xFFFFD0; break;
			case '4': text = "In 4";  fillStyle = 0xFFFFD0; break;
			case '5': text = "In 5";  fillStyle = 0xFFFFD0; break;
			case '6': text = "In 6";  fillStyle = 0xFFFFD0; break;
			case '7': text = "In 7";  fillStyle = 0xFFFFD0; break;
			case '8': text = "In 8";  fillStyle = 0xFFFFD0; break;
			case '9': text = "In 9";  fillStyle = 0xFFFFD0; break;
			case 'a': text = "Out 0";  fillStyle = 0xE0E0D0; break;
			case 'b': text = "Out 1";  fillStyle = 0xE0E0D0; break;
			case 'c': text = "Out 2";  fillStyle = 0xE0E0D0; break;
			case 'd': text = "Out 3";  fillStyle = 0xE0E0D0; break;
			case 'e': text = "Out 4";  fillStyle = 0xE0E0D0; break;
			case 'f': text = "Out 5";  fillStyle = 0xE0E0D0; break;
			case 'g': text = "Out 6";  fillStyle = 0xE0E0D0; break;
			case 'h': text = "Out 7";  fillStyle = 0xE0E0D0; break;
			case 'i': text = "Out 8";  fillStyle = 0xE0E0D0; break;
			case 'j': text = "Out 9";  fillStyle = 0xE0E0D0; break;
		}
		break;
	case MODAL_CODE_TagNumber:      text = state + tomString(index) + ":"; break;
	case MODAL_CODE_InputLabel:
	case MODAL_CODE_OutputLabel:
		if (!enableKeyboard) {
			text = state;
			fillStyle = 0xF0F0F0;
			break;
		}
		else {
			dialUniversalText(hDC, x, y, w, h, keyboardText, keyboardCursorPos, selectionWidth);
			return;// true;
		}
	case MODAL_CODE_NumericalValue:
		if (!enableKeyboard) {
			text = tomString(index);
			//text = trimTextToFit(hDC, text, w);
			fillStyle = 0xFFFFFF;
		}
		else {
			dialUniversalText(hDC, x, y, w, h, keyboardText, keyboardCursorPos, selectionWidth);
			return;// true;
		}
		break;
	case MODAL_CODE_LiteralValue:
		if (!enableKeyboard) {
			text = state;
			fillStyle = 0xFFFFFF;
		}
		else {
			dialUniversalText(hDC, x, y, w, h, keyboardText, keyboardCursorPos, selectionWidth);
			return;// true;
		}
		break;
	case MODAL_CODE_Checkbox:       
		text = state; 
		fillStyle = 0xFFFFFF; 
		break;
	case MODAL_CODE_RadioGroup:     
		text = state; 
		fillStyle = 0xFFFFFF;
		break;
	case MODAL_CODE_PlaceComponent:
		text = state;
	//	fillStyle = page->cL[index]->fillColor;
		break;
	case MODAL_CODE_ScanForLabels:
		text = state;
		fillStyle = 0xFFF0F0;
		break;
	case MODAL_CODE_ScanForUserLabels:
		text = state;
		fillStyle = 0xFFF0F0;
		break;
	case MODAL_CODE_ScanForNotes:
		text = state;
		fillStyle = 0xFFF0F0;
		break;
	case MODAL_CODE_PinName:
		break;
	case MODAL_CODE_StaticText:
		break;
	}

	if ((w > 0) && (h > 0)) {
		HBRUSH frameColor = GetColorBrush(RGB(0, 0, 0)); // CreateSolidBrush
		HBRUSH fillColor = GetColorBrush(RGB(240, 240, 240)); // CreateSolidBrush
		if (code == MODAL_CODE_RadioGroup) {
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			BeginPath(hDC);
			Ellipse(hDC, x, y, x + w, y + w);
			EndPath(hDC);
			StrokePath(hDC);
			if (line) {
				BeginPath(hDC);
				Ellipse(hDC, x+4, y+4, x + w-4, y + w -4);
				EndPath(hDC);
				SelectObject(hDC, GetStockObject(BLACK_BRUSH));
				FillPath(hDC);
				SelectObject(hDC, GetStockObject(WHITE_BRUSH));
			}
		}
		else if (code == MODAL_CODE_Checkbox) {
			BeginPath(hDC);
			MoveToEx(hDC, x, y, NULL);
			LineTo(hDC, x, y + w);
			LineTo(hDC, x + w, y + w);
			LineTo(hDC, x + w, y);
			LineTo(hDC, x, y);
			if (line) { // for checkbox this is the on/off for the check
				MoveToEx(hDC, x, y, NULL);
				LineTo(hDC, x + w, y + w);
				MoveToEx(hDC, x+w, y, NULL);
				LineTo(hDC, x, y + w);
			}
			EndPath(hDC);
			StrokePath(hDC);
		}
		else
		{
			RECT aRect;
			aRect.left = x;
			aRect.top = y;
			aRect.right = x + w;
			aRect.bottom = y + h;
			HBRUSH fillColor = GetColorBrush(fillStyle); 
			FillRect(hDC, &aRect, fillColor);
			FrameRect(hDC, &aRect, frameColor);
		}
	}

	if (code == MODAL_CODE_PinName) {
		int textWidth = calcStringWidthFont( hDC, state);
		if (textWidth < w) {
			drawTextString(hDC, x + 3, y + 0, 0, state);
		}
		else {
			std::string fullString = state;
			std::string shortString;
			int yy = y;
			for (int k = 0; k < fullString.size(); k++) {
				if (fullString[k] == '\n') { // break on programmer's command
					drawTextString(hDC, x + 3, yy, 0, shortString);
					shortString.clear();
					yy += 12;
				}
				shortString += fullString[k];
				if ((fullString[k] == ' ') && (calcStringWidthFont(hDC, shortString) > w - 80)) { // break on a space if possible
					drawTextString(hDC, x + 3, yy, 0, shortString);
					shortString.clear();
					yy += 12;
				}
				if (calcStringWidthFont(hDC, shortString) > w - 10) { // last ditch
					drawTextString(hDC, x + 3, y, 0, shortString);
					shortString.clear();
					yy += 12;
				}
			}
			if( shortString.size() > 0 )
				drawTextString(hDC, x+3, yy, 0, shortString);
		}
		return;
	}
	if (code == MODAL_CODE_StaticText) {
		drawTextString(hDC, x + 3, y + 2, 0, state);
		return;
	}

	if ((code == MODAL_CODE_RadioGroup) || (code == MODAL_CODE_Checkbox) ){
		drawTextString(hDC, x + 23, y + 3, 0, state);
		return;
	}
	//ctx.fillStyle = 0x000000;
	std::string trimText;
	if (w < 20)
		trimText = text;
	else
		trimText = trimTextToFit(hDC, text, w);
	if (h >= 20)
		drawTextString(hDC, x + 5, y + 5, 0, trimText);
	else
		 drawTextString(hDC, x + 2, y + 2, 0, trimText);

}





std::vector<ModalDialogButton>modalDialogButtonList;

// ******************************** Matrix *****************************************

void buildMatrixDialogButtons(Sheet* pg, int i) {
	finishDialog = finishMatrixDialog;

	int x, y, w, h, n, line;
	int lGap = 50;
	int wid = 70;
	int tGap = 30;
	int yStep = 20;
	int xStep = 80;
	n = 0;

	// first 2 settings are width and height of icon
	// next 2 settings are the width and height of the matrix (columns and rows)
	// remaining settings are the internal values
	int columns = pg->dgrm.cL[i]->setting[2];
	int rows = pg->dgrm.cL[i]->setting[3];

	// build the dialog frame
	int mdlWidth = lGap + columns * xStep + 10;
	int mdlHeight = tGap + rows * yStep + 40;
	int mdlLeft = 30;
	int mdlTop = 30;

	
	if (mdlWidth > pg->workspaceWidth - 50) {
		wid = 30;
		xStep = 40;
		mdlWidth = lGap + columns * xStep + 10;
	}

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	//var text = trimTextToFit(ctx, "Matrix: " +pg->dgrm.cL[i]->userLabel, mdlWidth);
	std::string text = "Matrix";
	modalDialogButtonList.push_back( ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	x = mdlLeft + 10;
	for (int col = 0; col < columns; col++) {
		modalDialogButtonList.push_back( ModalDialogButton(x + lGap, mdlTop + 10, 0, 0, MODAL_CODE_TagNumber, 0, col + 1, "Col ")); // line label
		x += xStep;
	}
	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	x = mdlLeft + 10;
	y = mdlTop + tGap;
	h = 14;
	int j = 4;
	for (line = 0; line < rows; line++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_TagNumber, 0, line, "Row ")); // line label

		for (int col = 0; col < columns; col++) {
			double value = pg->dgrm.cL[i]->setting[j++];
			modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_NumericalValue, line, value, "")); // value
			x += xStep;
		}
		y += yStep;
		x = mdlLeft + 10;
	}
}


//************************************* Table *********************************


void buildTableDialogButtons(Sheet* pg, int i) {
	finishDialog = finishTableDialog;

	int x, y, w, h, n, line; // j
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	// first 2 settings are width and height of icon
	// remaining settings are the internal values
	int values = pg->dgrm.cL[i]->setting.size() - 2;

	// build the dialog frame
	int mdlWidth = lGap + wid + 30;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	int columns = 1;
	int valuesPerColumn = values;

	if (mdlHeight > pg->workspaceHeight - 50) {
		valuesPerColumn = floor((pg->workspaceHeight - 50 - tGap - 50) / yStep);
		columns = ceil(values / valuesPerColumn);
		mdlWidth = lGap + (wid + lGap) * columns;
		mdlHeight = tGap + valuesPerColumn * yStep + 50;
	}


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Table: ";
	text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	for (line = 0; line < values; line++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_TagNumber, 0, line, "Value ")); // line label

		double value = pg->dgrm.cL[i]->setting[line + 2];
		modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_NumericalValue, line, value, "")); // value
		lineIndex++;
		y += yStep;
		if (lineIndex == valuesPerColumn) {
			y = mdlTop + tGap;
			x += wid + lGap + 10;
			lineIndex = 0;
		}
	}
}



// **************************** literal ***************************************

void buildLiteralDialogButtons(Sheet* pg, int i) {
	finishDialog = finishLiteralDialog;

	int x, y, w, h, n, line;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	// first 2 settings are width and height of icon
	// remaining settings are the internal values
	int litValues = pg->dgrm.cL[i]->literal.size();
	int valuesPerColumn = floor((pg->workspaceHeight - 50 - tGap - 50) / yStep);
	int columns = ceil(litValues / valuesPerColumn);
	if (columns < 1)
		columns = 1;
	int mdlWidth = pg->workspaceWidth - 50;

	// build the dialog frame
  
	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	// var mdlWidth = lGap + (wid + lGap) * columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	if (mdlHeight > tGap + valuesPerColumn * yStep + 50)
		mdlHeight = tGap + valuesPerColumn * yStep + 50;


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Literal: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	for (line = 0; line < litValues; line++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_TagNumber, 0, line, "")); // line label

		std::string litValue = pg->dgrm.cL[i]->literal[line];
		modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_LiteralValue, line, 0, litValue)); // value

		lineIndex++;
		y += yStep;
		if (lineIndex == valuesPerColumn) {
			y = mdlTop + tGap;
			x += wid + lGap + 10;
			lineIndex = 0;
		}
	}
}



// **************************** UTF ***************************************

vector<string> UTF_labels0 = { "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta",   "theta", "iota",  "kappa", "lambda", "mu" };
vector<string> UTF_values0 = { "&945;", "&946;", "&947;", "&948;", "&949;", "&950;", "&951;", "&952;", "&953;", "&954;", "&955;", "&956;" };

vector<string> UTF_labels1 = { "nu",    "xi",    "omicron", "pi",    "rho",   "sigma", "tau",   "upsilon", "phi",   "chi",   "psi", "omega" };
vector<string> UTF_values1 = { "&957;", "&958;", "&959;",   "&960;", "&961;", "&963;", "&964;", "&965;",   "&966;", "&967;", "&968;", "&969;" };

vector<string> UTF_labels2 = { "ALPHA", "BETA",  "GAMMA", "DELTA", "EPSILON", "ZETA", "ETA",   "THETA", "IOTA", "KAPPA", "LAMBDA", "MU" };
vector<string> UTF_values2 = { "&913;", "&914;", "&915;", "&916;", "&917;",  "&918;", "&919;", "&920;", "&921;", "&922;", "&923;", "&924;" };

vector<string> UTF_labels3 = { "NU",    "XI", "OMICRON",  "PI",    "RHO",  "SIGMA",  "TAU",  "UPSILON",  "PHI", "CHI" , "PSI" , "OMEGA" };
vector<string> UTF_values3 = { "&925;", "&926;", "&927;", "&928;", "&929;", "&931;", "&932;", "&933;", "&934;", "&935;", "&936;", "&937;" };

vector<string> UTF_labels4 = { "super 0", "super 1", "super 2", "super 3", "super 4", "super 5", "super 6", "super 7", "super 8", "super 9", "super +", "super -" };
vector<string> UTF_values4 = { "&8304;",  "&185;",   "&178;",   "&179;",   "&8308;",  "&8309;",  "&8310;",  "&8311;",  "&8312;",  "&8313;",  "&8314;",  "&8315;" };

vector<string> UTF_labels5 = { "sub 0",  "sub 1", "sub 2",   "sub 3",  "sub 4",  "sub 5",  "sub 6",  "sub 7",  "sub 8",  "sub 9", "Integral", "Partial Differential" };
vector<string> UTF_values5 = { "&8320;", "&8321;", "&8322;", "&8323;", "&8324;", "&8325;", "&8326;", "&8327;", "&8328;", "&8329;",  "&8747;",  "&8706;" };

vector<string> UTF_labels6 = { "Plus",  "Minus", "Plus/Minus", "Multiply", "Dot",   "Divide", "Degrees", "Infinity", "Union", "Intersection", "Overline" };
vector<string> UTF_values6 = { "&043;", "&8722;", "&177;",     "&215;",    "&8901;", "&247;",  "&176;",   "&8734;",  "&8746;",  "&8745;",      "&773;" };

vector<vector<string>> allLabels = { UTF_labels0, UTF_labels1, UTF_labels2, UTF_labels3, UTF_labels4, UTF_labels5, UTF_labels6 };
vector<vector<string>> allValues = { UTF_values0, UTF_values1, UTF_values2, UTF_values3, UTF_values4, UTF_values5, UTF_values6 };

void buildUTFDialogButtons(Sheet* pg, int i) {
	finishDialog = finishUTFDialog;

	int x, y, w, h, n, line;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int litValues = 0;
	for (int col = 0; col < allLabels.size(); col++)
		if (litValues < allLabels[col].size())
			litValues = allLabels[col].size();

	int columns = allLabels.size();
	
	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int columnStep = 90;
	int mdlWidth = lGap + columnStep * columns + lGap;
	int mdlHeight = tGap + litValues * yStep + 50;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "UTF: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	
	x = mdlLeft + lGap;
	y = mdlTop + tGap;

	for (int col = 0; col < columns; col++) {
		for (line = 0; line < allLabels[col].size(); line++) {
			modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, 0, 0, allLabels[col][line]));
			y += yStep;
		}

		x += columnStep;
		y = mdlTop + tGap;
	}
}

void finishUTFDialog(Sheet* pg, int i) {

	string radioName;
	int radioValue = -1;
	int k_rad = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		
		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			if (modalDialogButtonList[but].line) {
				radioValue = k_rad;
				radioName = modalDialogButtonList[but].state;
				break;
			}
			k_rad++;
			break;
		}
	}

	if (radioValue == -1)
		return;

	for (int col = 0; col < allLabels.size(); col++) {
		if (radioValue < allLabels[col].size()) {
			pg->dgrm.cL[i]->literal[0] = allValues[col][radioValue];
			pg->dgrm.cL[i]->literal[1] = radioName;
			pg->dgrm.cL[i]->output[0].stringValue[0] = allValues[col][radioValue];
			break;
		}
		radioValue -= allValues[col].size();
	} 
}

// **************************** USER LABEL ***************************************

void buildUserLabelDialogButtons(Sheet* pg, int i) {
	finishDialog = finishUserLabelDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	
	int columns = 1;
	int litValues = 1;
	int mdlWidth = 250;

	// build the dialog frame
  
	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	if (mdlHeight > tGap + yStep + 50)
		mdlHeight = tGap + yStep + 50;


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "User Label: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	
	std::string litValue = pg->dgrm.cL[i]->userLabel;
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value

}


// **************************** value font ***************************************

void buildValueFontDialogButtons(Sheet* pg, int i) {
	finishDialog = finishValueFontDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;


	int columns = 1;
	int litValues = 1;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	if (mdlHeight > tGap + yStep + 50)
		mdlHeight = tGap + yStep + 50;


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Set Font: ", mdlWidth); 
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	std::string litValue;
	if(pg->dgrm.cL[i]->literal.size() == 0 )
		litValue = "";
	else
		litValue = pg->dgrm.cL[i]->literal[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value

}


// **************************** Output ***************************************

void buildOutputDialogButtons(Sheet* pg, int i) {
	finishDialog = finishOutputDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;


	int columns = 1;
	int litValues = 2;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	
	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Output: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, 0, "Pin Number:")); // line label
	double value = pg->dgrm.cL[i]->setting[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, 30, h, MODAL_CODE_NumericalValue, 0, value, "")); // value
	y += yStep;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, 0, "Output Name:")); // line label
	std::string litValue = pg->dgrm.cL[i]->literal[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + 80, y, 80, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value

}

// **************************** Input ***************************************

void buildInputDialogButtons(Sheet* pg, int i) {
	finishDialog = finishInputDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;


	int columns = 1;
	int litValues = 3;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	int mdlHeight = tGap + litValues * yStep + 50;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Input: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, 0, "Pin Number:")); 
	double value = pg->dgrm.cL[i]->setting[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, 30, h, MODAL_CODE_NumericalValue, 0, value, "")); 
	y += yStep;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, 0, "Input Name:")); 
	std::string litValue = pg->dgrm.cL[i]->literal[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, 80, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); 
	y += yStep;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, 0, "Default Value:")); 
	value = pg->dgrm.cL[i]->setting[1];
	modalDialogButtonList.push_back(ModalDialogButton(x + 80, y, 60, h, MODAL_CODE_NumericalValue, 0, value, "")); 
	
}


// ************************************* Subroutine name *****************************


int globalSheetIndex;
void buildSubroutineNameDialogButtons(Sheet* pg, int sheetIndex) {
	globalSheetIndex = sheetIndex;
	finishDialog = finishSubroutineNameDialog;
	pg->modalDialogComponent = 22222;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;


	int columns = 1;
	int litValues = 1;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	int mdlHeight = tGap + litValues * yStep + 50;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text;
	if(sheetIndex == 0)
		text = trimTextToFit(pg->hDC, "Main Program Name: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	else
		text = trimTextToFit(pg->hDC, "Subroutine Name: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, 0, "Name:")); // line label
	
	std::string litValue;
	if (sheetIndex == 0)
		litValue = cleanFileName(pg->filePath);
	else
		litValue  = isL[sheetIndex]->name;
	modalDialogButtonList.push_back(ModalDialogButton(x + 50, y, 150, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value
}

void finishSubroutineNameDialog(Sheet* pg, int i) {

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			if (globalSheetIndex == 0) {
				pg->filePath = ConvertToLPWSTR( modalDialogButtonList[but].state);
				setTitleFromPathName(pg);
			}
			else {
				isL[globalSheetIndex]->name = modalDialogButtonList[but].state;
				int butIdx = pg->subroutineNameIndex + globalSheetIndex - 1;
				pg->buttonList[butIdx]->visibleText = ConvertToLPWSTR(
					"Sub " + tomString(globalSheetIndex) + ": " + modalDialogButtonList[but].state);
			}
			break;
		}
	}
	
}


// ****************************************** Buss In ****************************************

void buildBussInDialogButtons(Sheet* pg, int i) {
	finishDialog = finishBussInDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	//componentDetailsType = true;
	//userEditsUserLabel = true;

	int columns = 1;
	int litValues = 1;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	// var mdlWidth = lGap + (wid + lGap) * columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	if (mdlHeight > tGap + yStep + 50)
		mdlHeight = tGap + yStep + 50;


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Bus Name: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	std::string litValue = pg->dgrm.cL[i]->literal[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value

}

// ****************************************** Bus Out ****************************************

void buildBussOutDialogButtons(Sheet* pg, int i) {
	finishDialog = finishBussOutDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int columns = 1;
	int litValues = 1;
	int mdlWidth = 250;

	vector<string> radioNames;
	radioNames.push_back(" ");
	int comps = pg->dgrm.cL.size();
	for (int k = 0; k < comps; k++) {
		if( pg->dgrm.cL[k]->type == L"BussIn" )
			radioNames.push_back(pg->dgrm.cL[k]->literal[0]);
	}
	litValues = radioNames.size();

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	// var mdlWidth = lGap + (wid + lGap) * columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Bus Name: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 50;
	y = mdlTop + tGap;

	std::string litValue = pg->dgrm.cL[i]->literal[0];
	modalDialogButtonList.push_back(ModalDialogButton(x+20, y, 140, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value

	for (int line = 0; line < litValues; line++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (line==0), 0, radioNames[line]));

		lineIndex++;
		y += yStep;
		//if (lineIndex == valuesPerColumn) {
		//	y = mdlTop + tGap;
		//	x += wid + lGap + 10;
		//	lineIndex = 0;
		//}
	}

}


// **************************** Bus 2 Pin Names ***************************************

void buildBuss2InPinNamesDialogButtons(Sheet* pg, int i) {
	finishDialog = finishBuss2InNamesDialog;

	int x, y, w, h, n, line;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	// first 2 settings are width and height of icon
	// remaining settings are the internal values
	int litValues = pg->dgrm.cL[i]->literal.size();
	int valuesPerColumn = floor((pg->workspaceHeight - 50 - tGap - 50) / yStep);
	int columns = ceil(litValues / valuesPerColumn);
	if (columns < 1)
		columns = 1;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	// var mdlWidth = lGap + (wid + lGap) * columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	if (mdlHeight > tGap + valuesPerColumn * yStep + 50)
		mdlHeight = tGap + valuesPerColumn * yStep + 50;


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Bus 2: ", mdlWidth); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	for (line = 0; line < litValues; line++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_TagNumber, 0, line, "")); // line label

		std::string litValue = pg->dgrm.cL[i]->literal[line];
		modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_LiteralValue, line, 0, litValue)); // value

		lineIndex++;
		y += yStep;
		if (lineIndex == valuesPerColumn) {
			y = mdlTop + tGap;
			x += wid + lGap + 10;
			lineIndex = 0;
		}
	}
}


void finishBuss2InNamesDialog(Sheet* pg, int i) {

	int k;
	int litValues = pg->dgrm.cL[i]->literal.size();

	pg->dgrm.cL[i]->literal.resize(litValues);

	k = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[k] = modalDialogButtonList[but].state;
			k++;
			break;
		}
	}
	pg->dgrm.cL[i]->onLoad(pg);
	updateAllBuss2Outs(pg);
}



//************************************* FSM *********************************

void buildFsmDialogButtons(Sheet* pg, int i) {
	finishDialog = finishFsmDialog;

	int x, y, w, h, n, line;
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 30;
	n = 0;

	int values = 4;

	// build the dialog frame
	int mdlWidth = lGap + wid + 30;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Finite State Machine Options: ";
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));



	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Group Name:")); // line label
	std::string litValue = pg->dgrm.cL[i]->literal[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap + 20, y, 90, h, MODAL_CODE_LiteralValue, 0, 0, litValue)); // value
	y += yStep;
	
	int bit = pg->dgrm.cL[i]->setting[0];
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 20, 20, MODAL_CODE_Checkbox, bit, 0, "Initial State"));
	y += yStep;

	bit = ! pg->dgrm.cL[i]->setting[1]; // reversed
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 20, 20, MODAL_CODE_Checkbox, bit, 0, "Needs Clock"));
	y += yStep;

	// just start off
	bit = 0;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 20, 20, MODAL_CODE_Checkbox, bit, 0, "Force On (test)"));
	y += yStep;

}



//************************************* LED *********************************

void buildLedDialogButtons(Sheet* pg, int i) {
	finishDialog = finishLedDialog;

	int x, y, w, h, n, line; // j
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int values = 8;

	// build the dialog frame
	int mdlWidth = lGap + wid + 80;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Light: ";
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));


	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Number of Lights:")); // line label
	double value = pg->dgrm.cL[i]->setting[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap + 40, y, 30, h, MODAL_CODE_NumericalValue, line, value, "")); // value



	x = mdlLeft + 10;
	y = mdlTop + tGap + 30;
	line = 0;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	int lit = pg->dgrm.cL[i]->setting[1];
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 0, "Left to Right"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 0, "Bottom to Top"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 2), 0, "Right to Left"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 3), 0, "Top to Bottom"));
	
	
	x = mdlLeft + 180;
	y = mdlTop + tGap;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	lit = pg->dgrm.cL[i]->setting[2];
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 1, "WhiteLED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 1, "Red LED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 2), 1, "Orange LED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 3), 1, "Yellow LED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 4), 1, "Green LED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 5), 1, "Blue LED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 6), 1, "Violet LED"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 7), 1, "Gray LED")); 
	
}



//************************************* Rotation *********************************

void buildRotationDialogButtons(Sheet* pg, int i) {
	finishDialog = finishRotationDialog;

	int x, y, w, h, n, line;
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int values = 4;

	// build the dialog frame
	int mdlWidth = lGap + wid + 80;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Rotation: ";
	
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));


	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	int lit = (pg->dgrm.cL[i]->rotation & 4)/4;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 0, "Clockwise"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 0, "Counter Clockwise"));


	x = mdlLeft + 180;
	y = mdlTop + tGap;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	lit = pg->dgrm.cL[i]->rotation & 3;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 1, "Zero is North"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 1, "Zero is East"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 2), 1, "Zero is South"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 3), 1, "Zero is West"));


	x = mdlLeft + 10;
	y = mdlTop + tGap + 3* yStep;
	line = 0;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	lit = (pg->dgrm.cL[i]->rotation & 8) / 8;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 2, "Radians"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 2, "Degrees"));

}


//************************************* Rotation 3D *********************************

void buildRotation3dDialogButtons(Sheet* pg, int i) {
	finishDialog = finishRotation3dDialog;

	int x, y, w, h, n, line;
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int values = 14;

	// build the dialog frame
	int mdlWidth = lGap + wid + 80;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Rotation: ";

	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));


	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap + yStep;
	line = 0;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	int lit = (pg->dgrm.cL[i]->rotation >> 2) & 1;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 0, "X Clockwise"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 0, "X Counter Clockwise"));
	y += 4 * yStep;

	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	lit = (pg->dgrm.cL[i]->rotation >> 5) & 1;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 1, "Y Clockwise"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 1, "Y Counter Clockwise"));
	y += 4 * yStep;

	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	lit = (pg->dgrm.cL[i]->rotation >> 8) & 1;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 2, "Z Clockwise"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 2, "Z Counter Clockwise"));


	x = mdlLeft + 180;
	y = mdlTop + tGap;
	//line -- for radio group this is the on/off for the line
	//index -- for the radio group this is the group
	lit = pg->dgrm.cL[i]->rotation & 3;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 3, "X zero is North"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 3, "X zero is East"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 2), 3, "X zero is South"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 3), 3, "X zero is West"));
	y += 2*yStep;

	lit = (pg->dgrm.cL[i]->rotation >> 3) & 3;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 4, "Y zero is North"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 4, "Y zero is East"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 2), 4, "Y zero is South"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 3), 4, "Y zero is West"));
	y += 2*yStep;

	lit = (pg->dgrm.cL[i]->rotation >> 6) & 3;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 0), 5, "Z zero is North"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 1), 5, "Z zero is East"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 2), 5, "Z zero is South"));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (lit == 3), 5, "Z zero is West"));
}






//************************************* Logic Options *********************************

void buildLogicOptionsDialogButtons(Sheet* pg, int i) {
	finishDialog = finishLogicOptionsDialog;

	int x, y, w, h, n, line; // j
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int values = 1;

	// build the dialog frame
	int mdlWidth = lGap + wid + 30;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	int columns = 1;
	int valuesPerColumn = values;

	if (mdlHeight > pg->workspaceHeight - 50) {
		valuesPerColumn = floor((pg->workspaceHeight - 50 - tGap - 50) / yStep);
		columns = ceil(values / valuesPerColumn);
		mdlWidth = lGap + (wid + lGap) * columns;
		mdlHeight = tGap + valuesPerColumn * yStep + 50;
	}

	mdlHeight = tGap + 4 * yStep + 50;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Logic Options: ";
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Number of Inputs:")); // line label
	double value = pg->dgrm.cL[i]->setting[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + 130, y, 30, h, MODAL_CODE_NumericalValue, line, value, "")); // value
	lineIndex++;
	y += yStep;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Number of Outputs:")); // line label
	value = pg->dgrm.cL[i]->setting[1];
	modalDialogButtonList.push_back(ModalDialogButton(x + 130, y, 30, h, MODAL_CODE_NumericalValue, line, value, "")); // value
	lineIndex++;
	y += yStep;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Number of Logic Lines:")); // line label
	value = pg->dgrm.cL[i]->literal.size() - pg->dgrm.cL[i]->setting[0] - pg->dgrm.cL[i]->setting[1];
	modalDialogButtonList.push_back(ModalDialogButton(x + 130, y, 30, h, MODAL_CODE_NumericalValue, line, value, "")); // value
	lineIndex++;
	y += yStep;
}

//************************************* Logic Logics *********************************

void buildLogicLogicsDialogButtons(Sheet* pg, int i) {
	finishDialog = finishLogicLogicsDialog;

	int x, y, w, h, n, j, line;
	int lGap = 50;
	int xStep = 42;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int lines = pg->dgrm.cL[i]->literal.size();
	int inputs = pg->dgrm.cL[i]->setting[0];
	int outputs = pg->dgrm.cL[i]->setting[1];

	int mdlWidth = lGap + (inputs + outputs) * xStep + 50;
	int mdlHeight = tGap + (lines - inputs - outputs) * yStep + 80;
	int mdlLeft = 30;
	int mdlTop = 30;

	 // check the size of the window vs this dialog
	if (mdlWidth > pg->workspaceWidth - 40) {
		MessageBox(NULL, L"There are too many inputs and outputs to fit horizontally on the screen.", L"Logic Component", MB_OK);
		return;
	}
	if (mdlHeight > pg->workspaceHeight - 30) {
		MessageBox(NULL, L"There are too many logics to fit vertically on the screen.", L"Logic Component", MB_OK);
		return;
	}
	//modalDialogComponent = i;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "Logic Lines: ";
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	// build the Input names
	w = xStep;
	h = 14;
	int k;
	string lit;
	x = mdlLeft + lGap;
	for (j = 0; j < inputs; j++) {
		lit = pg->dgrm.cL[i]->literal[j];
		modalDialogButtonList.push_back(ModalDialogButton(x, mdlTop + 20, 0, 0, MODAL_CODE_TagNumber, 0, j, "In ")); // input column label
		modalDialogButtonList.push_back(ModalDialogButton(x, mdlTop + 40, w, h, MODAL_CODE_InputLabel, 0, j, lit)); // input column name
		x += xStep;
	}
	// build the output names
	x += 20;
	for (j = 0; j < outputs; j++) {
		lit = pg->dgrm.cL[i]->literal[inputs + j];
		modalDialogButtonList.push_back(ModalDialogButton(x - 2, mdlTop + 20, 0, 0, MODAL_CODE_TagNumber, 0, j, "Out ")); // output column label
		modalDialogButtonList.push_back(ModalDialogButton(x, mdlTop + 40, w, h, MODAL_CODE_OutputLabel, 0, j, lit)); // output column name
		x += xStep;
	}

	// build the Logic lines
	y = mdlTop + tGap + w + 2;
	for (line = 0; line < lines - inputs - outputs; line++) {
		// Linde ID
		modalDialogButtonList.push_back(ModalDialogButton(mdlLeft + 8, y, 0, 0, MODAL_CODE_TagNumber, 0, line, "Line ")); // line label
		// Input logic buttons
		x = mdlLeft + lGap;
		k = 0;
		lit = pg->dgrm.cL[i]->literal[inputs + outputs + line];
		for (j = 0; j < inputs; j++) {
			modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_InputButton, line, j, lit.substr(k++, 1))); // input button
			x += xStep;
		}
		// output logic buttons
		x += 20;
		for (j = 0; j < outputs; j++) {
			modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_OutputButton, line, j, lit.substr(k++, 1))); // output button
			x += xStep;
		}
		y += yStep;
	}

}

//************************************* D to A Options *********************************

void buildDtoAOptionsDialogButtons(Sheet* pg, int i) {
	finishDialog = finishDtoAOptionsDialog;

	int x, y, w, h, n, line; // j
	int lGap = 60;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int values = 1;

	// build the dialog frame
	int mdlWidth = lGap + wid + 30;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	int columns = 1;
	int valuesPerColumn = values;

	if (mdlHeight > pg->workspaceHeight - 50) {
		valuesPerColumn = floor((pg->workspaceHeight - 50 - tGap - 50) / yStep);
		columns = ceil(values / valuesPerColumn);
		mdlWidth = lGap + (wid + lGap) * columns;
		mdlHeight = tGap + valuesPerColumn * yStep + 50;
	}

	mdlHeight = tGap + 4 * yStep + 50;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "D to A Options: ";
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Number of Inputs:")); // line label
	double value = pg->dgrm.cL[i]->setting[0];
	modalDialogButtonList.push_back(ModalDialogButton(x + 130, y, 30, h, MODAL_CODE_NumericalValue, line, value, "")); // value
	lineIndex++;
	y += yStep;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, "Number of Logic Lines:")); // line label
	value = pg->dgrm.cL[i]->element.size();
	modalDialogButtonList.push_back(ModalDialogButton(x + 130, y, 30, h, MODAL_CODE_NumericalValue, line, value, "")); // value
	lineIndex++;
	y += yStep;
}

//************************************* D to A Logics *********************************

void buildDtoALogicsDialogButtons(Sheet* pg, int i) {
	finishDialog = finishDtoALogicsDialog;

	int x, y, w, h, n, j, line;
	int lGap = 50;
	int xStep = 42;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int lines = pg->dgrm.cL[i]->literal.size();
	int inputs = pg->dgrm.cL[i]->setting[0];
	int outputs = 1;
	int elements = pg->dgrm.cL[i]->element.size();

	int mdlWidth = lGap + (inputs + outputs) * xStep + 50;
	int mdlHeight = tGap + (lines - inputs) * yStep + 80;
	int mdlLeft = 30;
	int mdlTop = 30;

	// check the size of the window vs this dialog
	if (mdlWidth > pg->workspaceWidth - 40) {
		MessageBox(NULL, L"There are too many inputs to fit horizontally on the screen.", L"Logic Component", MB_OK);
		return;
	}
	if (mdlHeight > pg->workspaceHeight - 30) {
		MessageBox(NULL, L"There are too many logics to fit vertically on the screen.", L"Logic Component", MB_OK);
		return;
	}
	//modalDialogComponent = i;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = "D to A Logic Lines: ";
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	// build the Input names
	w = xStep;
	h = 14;
	int k;
	string lit;
	x = mdlLeft + lGap;
	for (j = 0; j < inputs; j++) {
		lit = pg->dgrm.cL[i]->literal[j];
		modalDialogButtonList.push_back(ModalDialogButton(x, mdlTop + 20, 0, 0, MODAL_CODE_TagNumber, 0, j, "In ")); // input column label
		modalDialogButtonList.push_back(ModalDialogButton(x, mdlTop + 40, w, h, MODAL_CODE_InputLabel, 0, j, lit)); // input column name
		x += xStep;
	}
	// build the output names
	x += 20;
	for (j = 0; j < outputs; j++) {
		lit = pg->dgrm.cL[i]->literal[inputs + j];
		modalDialogButtonList.push_back(ModalDialogButton(x - 2, mdlTop + 20, 0, 0, MODAL_CODE_TagNumber, 0, j, "Out ")); // output column label
		modalDialogButtonList.push_back(ModalDialogButton(x, mdlTop + 40, w, h, MODAL_CODE_OutputLabel, 0, j, lit)); // output column name
		x += xStep;
	}

	// build the Logic lines
	y = mdlTop + tGap + w + 2;
	for (line = 0; line < elements; line++) {
		// Linde ID
		modalDialogButtonList.push_back(ModalDialogButton(mdlLeft + 8, y, 0, 0, MODAL_CODE_TagNumber, 0, line, "Line ")); // line label
		// Input logic buttons
		x = mdlLeft + lGap;
		k = 0;
		lit = pg->dgrm.cL[i]->literal[inputs + line];
		for (j = 0; j < inputs; j++) {
			modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_InputButton, line, j, lit.substr(k++, 1))); // input button
			x += xStep;
		}
		// output logic buttons
		x += 20;
		for (j = 0; j < outputs; j++) {
			modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_NumericalValue, line, pg->dgrm.cL[i]->element[line], "")); // output value
			x += xStep;
		}
		y += yStep;
	}

}



// **************************** Universal Pin ***************************************


void buildUniversalPinDialogButtons(Sheet* pg, int i) {
	finishDialog = finishUniversalPinDialog;

	int x, y, w, h, n, line;
	int lGap = 30;
	int tGap = 30;
	int yStep = 30;
	n = 0;
	
	vector<int> localPinGroups = pg->dgrm.cL[i]->GetPinGroups();
	vector<string> localPinLabels = pg->dgrm.cL[i]->GetPinLabels();
	int groups = localPinGroups.size();

	int maxItems = 0;
	for (int j = 0; j < groups; j++)
		if (localPinGroups[j] > maxItems)
			maxItems = localPinGroups[j];
	
	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = 90;
	int mdlHeight = tGap + groups * yStep + 50;
	int mdlWidth = 40 + maxItems * (wid+10);


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Pin options: " , 200); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));


	int pinMask = (int)pg->dgrm.cL[i]->setting[pg->dgrm.cL[i]->pinMaskIndex];
	int bit = 1;

	h = 14;
	int lineIndex = 0;
	
	y = mdlTop + tGap;
	int k = 0;
	for (line = 0; line < groups; line++) {
		x = mdlLeft + 10;
		
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 20, 20, MODAL_CODE_Checkbox, (pinMask & bit), line, ""));

		for (int j = 0; j < localPinGroups[line]; j++) {

			string pinLabel = localPinLabels[k++];
			modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y + 3, wid, h, MODAL_CODE_PinName, line, 0, pinLabel));
			x += wid + 10;
		}
		bit <<= 1;
		lineIndex++;
		y += yStep;
		
	}
	pg->modalDialogComponent = i;
}




// **************************** Details about each component  ***************************************


void buildDetailsDialogButtons(Sheet* pg, int i) {
	finishDialog = NULL;
	
	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 30;
	n = 0;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = 70;
	int mdlHeight = tGap + yStep + 150;
	int mdlWidth = 480;

	LPCTSTR componentType = pg->dgrm.cL[i]->type;
	string text2 = convertLPCTSTR_toString(lookupDetail(componentType));


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Component Details: ", 200); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x + 35, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	//modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int line = 0;

	y = mdlTop + tGap;
	int k = 0;

	x = mdlLeft + 30;
	
	modalDialogButtonList.push_back(ModalDialogButton(x, y, mdlWidth - 55, 120, MODAL_CODE_PinName, line, 0, text2));


	pg->modalDialogComponent = i;
}


void buildDetailsDialogButtonsFromComponentType(Sheet* pg, LPCTSTR componentType) {
	finishDialog = NULL;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 30;
	n = 0;



	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = 70;
	int mdlHeight = tGap + yStep + 150;
	int mdlWidth = 480;

	string text2 = convertLPCTSTR_toString(lookupDetail(componentType));

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Component Details: ", 200); // +pg->dgrm.cL[i]->userLabel, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x + 35, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	//modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int line = 0;

	y = mdlTop + tGap;
	int k = 0;

	x = mdlLeft + 30;

	modalDialogButtonList.push_back(ModalDialogButton(x, y, mdlWidth - 55, 120, MODAL_CODE_PinName, line, 0, text2));


	pg->modalDialogComponent = 22222;
}



// **************************** SCREEN DIALOG ********************************

void finishLogic2Dialog(Sheet* pg, int i) {}
void finishDtoADialog(Sheet* pg, int i) {}

void finishTableDialog(Sheet* pg, int i) {

	int k;
	int values = pg->dgrm.cL[i]->setting.size() - 2;

	pg->dgrm.cL[i]->setting.resize(2 + values);
	pg->dgrm.cL[i]->output[0].value.resize(values);

	k = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
				modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->setting[2 + k] = modalDialogButtonList[but].index;
			pg->dgrm.cL[i]->output[0].value[k] = modalDialogButtonList[but].index;
			k++;
			break;
		}
	}
	//setSimpleOutputDimensions(pg->dgrm.cL[i]);
	setVisibleScreenDirty("Table");
}
void finishMatrixDialog(Sheet* pg, int i) {

	double values = pg->dgrm.cL[i]->setting[2] * pg->dgrm.cL[i]->setting[3];
	pg->dgrm.cL[i]->setting.resize(4 + values);
	pg->dgrm.cL[i]->output[0].value.resize( values);

	int k = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
				modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->setting[4 + k] = modalDialogButtonList[but].index;
			pg->dgrm.cL[i]->output[0].value[k] = modalDialogButtonList[but].index;
			k++;
			break;
		}
	}
	setVisibleScreenDirty("Matrix");
}
void finishLiteralDialog(Sheet* pg, int i) {

	pg->dgrm.cL[i]->literal.clear();
	pg->dgrm.cL[i]->output[0].stringValue.clear();

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			if (modalDialogButtonList[but].state.size()) { //  skip blank lines
				pg->dgrm.cL[i]->literal.push_back(modalDialogButtonList[but].state);
				pg->dgrm.cL[i]->output[0].stringValue.push_back(modalDialogButtonList[but].state);
			}
			break;
		}
	}
	pg->dgrm.cL[i]->output[0].dimensions[0] = pg->dgrm.cL[i]->literal.size();
	setVisibleScreenDirty("Literal");
}
void finishUserLabelDialog(Sheet* pg, int i) {

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->userLabel = modalDialogButtonList[but].state;
			break;
		}
	}
	if ((pg->dgrm.cL[i]->type == L"Buss2In") ||
		(pg->dgrm.cL[i]->type == L"Buss2Out"))
		updateAllBuss2Outs(pg);

	setVisibleScreenDirty("User Label");
}
void finishValueFontDialog(Sheet* pg, int i) {

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			if (pg->dgrm.cL[i]->literal.size() == 0)
				pg->dgrm.cL[i]->setLiteral(0, modalDialogButtonList[but].state);
			else
				pg->dgrm.cL[i]->literal[0] = modalDialogButtonList[but].state;
			break;
		}
	}
	setVisibleScreenDirty("Value Font");
}
void finishBussInDialog(Sheet* pg, int i) {

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[0] = modalDialogButtonList[but].state;
			break;
		}
	}
	pg->lastBussName = pg->dgrm.cL[i]->literal[0];

	// reset the width
	int newWidth = 10 + calcStringWidthFont(pg->hDC, pg->dgrm.cL[i]->literal[0]) / 2;
	if (newWidth < 30)
		newWidth = 30;
	int left = pg->dgrm.cL[i]->x - pg->dgrm.cL[i]->width;
	pg->dgrm.cL[i]->x = left + newWidth;
	pg->dgrm.cL[i]->width = newWidth;

	setVisibleScreenDirty("Bus In");
}

void finishBussOutDialog(Sheet* pg, int i) {

	string firstName;
	string radioName;
	int radioValue = 0;
	int k_rad = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			firstName = modalDialogButtonList[but].state;
			break;
		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			if (modalDialogButtonList[but].line) {
				radioValue = k_rad;
				radioName = modalDialogButtonList[but].state;
			}
			k_rad++;
			break;
		}
	}
	if (radioValue == 0) {
		pg->dgrm.cL[i]->literal[0] = firstName;
		pg->lastBussName = firstName;
	}
	else {
		pg->dgrm.cL[i]->literal[0] = radioName;
		pg->lastBussName = radioName;
	}

	// reset the width
	int newWidth = 10 + calcStringWidthFont(pg->hDC, pg->dgrm.cL[i]->literal[0]) / 2;
	if (newWidth < 30)
		newWidth = 30;
	int right = pg->dgrm.cL[i]->x + pg->dgrm.cL[i]->width;
	pg->dgrm.cL[i]->x = right - newWidth;
	pg->dgrm.cL[i]->width = newWidth;

	setVisibleScreenDirty("Bus Out");
}

void finishOutputDialog(Sheet* pg, int i) {

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[0] = modalDialogButtonList[but].state;
			break;
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			if(modalDialogButtonList[but].index >= 0 )
				pg->dgrm.cL[i]->setting[0] = modalDialogButtonList[but].index;
			break;
		}
	}
	setVisibleScreenDirty("Output");
}
void finishInputDialog(Sheet* pg, int i) {

	int k = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[0] = modalDialogButtonList[but].state;
			break;
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			if (k == 0) {
				if (modalDialogButtonList[but].index >= 0)
					pg->dgrm.cL[i]->setting[0] = (int)modalDialogButtonList[but].index;
				k++;
			}
			else {
				pg->dgrm.cL[i]->setting[1] = modalDialogButtonList[but].index;
			}
			break;
		}
	}
	setVisibleScreenDirty("Input");
}
void finishTestDialog(Sheet* pg, int i) {}
void finishPresetDialog(Sheet* pg, int i) {}
void finishRemoteDialog(Sheet* pg, int i) {}




void finishFsmDialog(Sheet* pg, int i) {

	bool bools[3] = { false, false, false };
	string groupName;
	int k = 0;

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			groupName = modalDialogButtonList[but].state;
			break;
		case MODAL_CODE_Checkbox:
			if (modalDialogButtonList[but].line)
				bools[k] = true;
			k++;
			break;
		}
	}

	pg->dgrm.cL[i]->literal[0] = groupName;
	pg->dgrm.cL[i]->setting[0] = bools[0];
	pg->dgrm.cL[i]->setting[1] = ! bools[1];

	if( pg->dgrm.cL[i]->setting[1] == 0)
		pg->dgrm.cL[i]->timeBased = true;
	else
		pg->dgrm.cL[i]->timeBased = false;

	if (pg->dgrm.cL[i]->setting[0]) { // this guy is Initial - clear the others in its new group
		for (int j = 0; j < pg->dgrm.cL.size(); j++)
			if (pg->dgrm.cL[j]->type == L"FSM")
				if (j != i)
					if (pg->dgrm.cL[j]->literal[0] == groupName)
						pg->dgrm.cL[j]->setting[0] = 0;
	}

	if (bools[2]) { // clear all members in this group

		for (int j = 0; j < pg->dgrm.cL.size(); j++)
			if (pg->dgrm.cL[j]->type == L"FSM")
				if (pg->dgrm.cL[j]->literal[0] == groupName) {
					pg->dgrm.cL[j]->output[0].value[0] = 0;
					pg->dgrm.cL[j]->memory[0] = pg->systemTime;
				}
		// set this one's output true
		pg->dgrm.cL[i]->output[0].value[0] = 1;
	}
	pg->dgrm.cL[i]->onLoad(pg);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();

	setVisibleScreenDirty("Fsm");
}




void finishLedDialog(Sheet* pg, int i) {

	int intValue = 0;

	int radioCounter[2] = { 0 }; // two radio groups
	int litRadio[2] = { 0 };
	int index;

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			intValue = modalDialogButtonList[but].index;
			break;
		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			index = modalDialogButtonList[but].index;
			if (modalDialogButtonList[but].line)
				litRadio[index] = radioCounter[index];
			radioCounter[index]++;
			break;
		}
	}
	if ((intValue > 0) && (intValue <= 32))
		pg->dgrm.cL[i]->setting[0] = intValue; // the number of bulbs

	if ((litRadio[0] >= 0) && (litRadio[0] <= 3))
		pg->dgrm.cL[i]->setting[1] = litRadio[0]; // the direction

	if ((litRadio[1] >= 0) && (litRadio[1] <= 7))
		pg->dgrm.cL[i]->setting[2] = litRadio[1]; // the color

	pg->dgrm.cL[i]->onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	setVisibleScreenDirty("Led");
}



void finishRotationDialog(Sheet* pg, int i) {

	int radioCounter[3] = { 0 }; // three radio groups
	int litRadio[3] = { 0 };
	int index;

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			index = modalDialogButtonList[but].index;
			if (modalDialogButtonList[but].line)
				litRadio[index] = radioCounter[index];
			radioCounter[index]++;
			break;
		}
	}

	//                         cw/ccw,			 N,E,S,W        rad,degree
	pg->dgrm.cL[i]->rotation = litRadio[0] * 4 + litRadio[1] + litRadio[2] * 8;
	propagateValues(&pg->dgrm);
	drawCanvas();
	setVisibleScreenDirty("Rotation");
}

void finishRotation3dDialog(Sheet* pg, int i) {

	int radioCounter[6] = { 0 }; // two radio groups
	int litRadio[6] = { 0 };
	int index;

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			index = modalDialogButtonList[but].index;
			if (modalDialogButtonList[but].line)
				litRadio[index] = radioCounter[index];
			radioCounter[index]++;
			break;
		}
	}

	int xRot = (litRadio[0] << 2) + litRadio[3];  
	int yRot = (litRadio[1] << 2) + litRadio[4];
	int zRot = (litRadio[2] << 2) + litRadio[5];
	pg->dgrm.cL[i]->rotation = xRot + (yRot << 3) + (zRot << 6);
	propagateValues(&pg->dgrm);
	drawCanvas();

	setVisibleScreenDirty("Rotation 3d");
}


void finishLogicOptionsDialog(Sheet* pg, int i) {

	int k = 0;
	int array[3] = { 0 };

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			array[k++] = modalDialogButtonList[but].index;
			break;
		}
	}
	logicSetInputCount(pg, i, array[0]);
	logicSetOutputCount(pg, i, array[1]);
	logicSetLogicsCount(pg, i, array[2]);

	setVisibleScreenDirty("Logic Options");
}


void finishLogicLogicsDialog(Sheet* pg, int i) {
	string state;
	int line;
	int lines = pg->dgrm.cL[i]->literal.size();
	int inputs = pg->dgrm.cL[i]->setting[0];
	int outputs = pg->dgrm.cL[i]->setting[1];

	for (int line = 0; line < lines; line++)
		pg->dgrm.cL[i]->literal[line] = "";
	int k = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_InputButton: // input
		case MODAL_CODE_OutputButton: // output
			line = modalDialogButtonList[but].line;
			state = modalDialogButtonList[but].state;
			pg->dgrm.cL[i]->literal[line + inputs + outputs] += state;
			break;
		case MODAL_CODE_InputLabel:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[k] = modalDialogButtonList[but].state;
			pg->dgrm.cL[i]->input[k++].label = modalDialogButtonList[but].state;
			break;
		case MODAL_CODE_OutputLabel:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[k] = modalDialogButtonList[but].state;
			pg->dgrm.cL[i]->output[k - inputs].label = modalDialogButtonList[but].state;
			k++;
			break;
		}
	}
	setVisibleScreenDirty("Logic Logics");
}

void finishDtoAOptionsDialog(Sheet* pg, int i) {

	int k = 0;
	int array[3] = { 0 };

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			array[k++] = modalDialogButtonList[but].index;
			break;
		}
	}
	dtoASetInputCount(pg, i, array[0]);
	dtoASetLogicsCount(pg, i, array[1]);
	setVisibleScreenDirty("D to A Options");
}

void finishDtoALogicsDialog(Sheet* pg, int i) {
	string state;
	int line;
	int literals = pg->dgrm.cL[i]->literal.size();
	int elements = pg->dgrm.cL[i]->element.size();
	int inputs = pg->dgrm.cL[i]->setting[0];
	int outputs = 1;

	for (int line = 0; line < literals; line++)
		pg->dgrm.cL[i]->literal[line] = "";
	int k = 0;
	int z = 0;
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_InputButton: // input
		//case MODAL_CODE_OutputButton: // output
			line = modalDialogButtonList[but].line;
			state = modalDialogButtonList[but].state;
			pg->dgrm.cL[i]->literal[line + inputs] += state;
			break;
		case MODAL_CODE_InputLabel:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->literal[k] = modalDialogButtonList[but].state;
			pg->dgrm.cL[i]->input[k++].label = modalDialogButtonList[but].state;
			break;
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].state);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			pg->dgrm.cL[i]->element[z++] = modalDialogButtonList[but].index;
			break;
		}
	}
	setVisibleScreenDirty("D to A Logics");
}


void finishUniversalPinDialog(Sheet* pg, int i) {

	int groups = pg->dgrm.cL[i]->GetPinGroups().size();
	int oldPinMask = pg->dgrm.cL[i]->setting[pg->dgrm.cL[i]->pinMaskIndex];

	int pinMask = 0;
	int bit = 1;

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		if (modalDialogButtonList[but].code == MODAL_CODE_Checkbox) {
			if (modalDialogButtonList[but].line)
				pinMask |= bit;
			bit <<= 1;
		}
	}
	if (oldPinMask != pinMask) {

		if (page->stickyComponentsButton->isSelected)
			captureConnections(&pg->dgrm, i);

		// get the top and left position
		int left = pg->dgrm.cL[i]->x - pg->dgrm.cL[i]->width;
		int top = pg->dgrm.cL[i]->y - pg->dgrm.cL[i]->height;

		// then update the inputs
		pg->dgrm.cL[i]->setting[pg->dgrm.cL[i]->pinMaskIndex] = pinMask;
		pg->dgrm.cL[i]->buildPinLabels();

		//  while holding the top and left position
		pg->dgrm.cL[i]->x = left + pg->dgrm.cL[i]->width;
		pg->dgrm.cL[i]->y = top + pg->dgrm.cL[i]->height;

		if (pg->stickyComponentsButton->isSelected)
			updateConnections(pg, i);

		analyzeConnections(&pg->dgrm);

		setVisibleScreenDirty("Input Pin");
	}
}




int findCursorInText(HDC hDC, int mouseX, int left, int width, std::string text) {
	int delta = mouseX - left;
	for (int j = 0; j <= text.size(); j++) {
		if( calcStringWidthFont(hDC, text.substr(0, j)) > delta)
			return j - 1;
	}
	return text.size();
}

string findKeyword; // passed when -100 is used
bool buildHelpDialogButtons(Sheet*, string);

void modalDialogAcceptValues(Sheet* pg ) {
	if (finishDialog != NULL) 
		finishDialog(pg, pg->modalDialogComponent);
	
	// update the changes for Logic and Logic2 in particular
	if (pg->modalDialogComponent >= 0)
		if (pg->modalDialogComponent < 22222) {
			pg->dgrm.cL[pg->modalDialogComponent]->method2(pg);
			pg->isDirty = true; // put here so that Help and Find dont set dirty
		}
	if (pg->modalDialogComponent == -100)
		buildHelpDialogButtons(pg, findKeyword);
	else
		pg->modalDialogComponent = -1;

	pg->activeDialogComponent = -1;	
	modalDialogKeyboardEntry = -1;
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();

}

void mouseUpInModalDialog(Sheet* pg) {
	//if(modalDialogKeyboardEntry >= 0)
	//	modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = 0;
}
void mouseMoveInModalDialog(Sheet* pg, int x, int y)
{
	for (int j = 0; j < modalDialogButtonList.size(); j++) {
		if (!modalDialogButtonList[j].isHit(x, y))
			continue;

		switch (modalDialogButtonList[j].code) {
		case MODAL_CODE_NumericalValue:
		case MODAL_CODE_InputLabel:
		case MODAL_CODE_OutputLabel:
		case MODAL_CODE_LiteralValue:
			if (modalDialogKeyboardEntry == -1)
				break;
			int mouseX = x; // / pg->cs) - pg->cXo;
			int width = modalDialogButtonList[modalDialogKeyboardEntry].w;
			int left = modalDialogButtonList[modalDialogKeyboardEntry].x;
			std::string text = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
			int newCursorPos = findCursorInText(pg->hDC, mouseX, left, width, text);
			int oldCursorPos = modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos;
			if (newCursorPos == oldCursorPos)
				break;
			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			
			modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = newCursorPos - oldCursorPos;
			drawCanvas();
			break;
		}
	}
}

bool processModalDialogComponent(Sheet* pg, int x, int y){
	if (pg->modalDialogComponent == -1)
		return false;

	 // Preset, Test, Logic, Table, Literal have modal dialogs
	
		// find the button that was hit
	std::string newState;
	//var mouseMemory;
	bool modalDialog_mouseDragEnable = false;
	int modalDialog_mouseDragX = 0;
	int modalDialog_mouseDragY = 0;
//	int modalDialog_mouseDragButton;
	int inputs;
	int outputs;
	int group;
	


	for (int j = 0; j < modalDialogButtonList.size(); j++) {
		if (!modalDialogButtonList[j].isHit(x, y))
			continue;

		switch (modalDialogButtonList[j].code) {
		case MODAL_CODE_Ok: // ok -- put the dialog results back into the component
			modalDialogAcceptValues(pg);
			return true;
		case MODAL_CODE_Cancel: // cancel
			pg->modalDialogComponent = -1;
			modalDialogKeyboardEntry = -1;
			drawCanvas();
			return true;
		case MODAL_CODE_CheckAll:
			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			for (j = 0; j < modalDialogButtonList.size(); j++) {
				if (modalDialogButtonList[j].code == MODAL_CODE_Checkbox) {
					modalDialogButtonList[j].line = 1;
					modalDialogButtonList[j].draw(pg->hDC);
				}
			}
			//ctx.scale(pg->cs, pg->cs);
			break;
		case MODAL_CODE_InputButton: // input
			switch (modalDialogButtonList[j].state[0]) {
			case 'T': modalDialogButtonList[j].state = 'F'; break;
			case 'F': modalDialogButtonList[j].state = 'P'; break;
			case 'P': modalDialogButtonList[j].state = 'N'; break;
			case 'N': modalDialogButtonList[j].state = 'X'; break;
			case 'X': modalDialogButtonList[j].state = 'T'; break;
			}
			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			modalDialogButtonList[j].draw(pg->hDC);
			//ctx.scale(pg->cs, pg->cs);
			break;
		case MODAL_CODE_FeedbackButton: // input
			switch (modalDialogButtonList[j].state[0]) {
			case 'T': modalDialogButtonList[j].state = 'F'; break;
			case 'F': modalDialogButtonList[j].state = 'X'; break;
			case 'X': modalDialogButtonList[j].state = 'T'; break;
			}
			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			modalDialogButtonList[j].draw(pg->hDC);
			//ctx.scale(pg->cs, pg->cs);
			break;
		case MODAL_CODE_OutputButton: // output
			inputs = pg->dgrm.cL[pg->modalDialogComponent]->setting[0];
			outputs = pg->dgrm.cL[pg->modalDialogComponent]->setting[1];
			switch (modalDialogButtonList[j].state[0]) {
			case 'T': newState = 'F'; break;
			case 'F': newState = '0'; break;
			case '0': if (inputs > 1) newState = '1'; else newState = 'a'; break;
			case '1': if (inputs > 2) newState = '2'; else newState = 'a'; break;
			case '2': if (inputs > 3) newState = '3'; else newState = 'a'; break;
			case '3': if (inputs > 4) newState = '4'; else newState = 'a'; break;
			case '4': if (inputs > 5) newState = '5'; else newState = 'a'; break;
			case '5': if (inputs > 6) newState = '6'; else newState = 'a'; break;
			case '6': if (inputs > 7) newState = '7'; else newState = 'a'; break;
			case '7': if (inputs > 8) newState = '8'; else newState = 'a'; break;
			case '8': if (inputs > 9) newState = '9'; else newState = 'a'; break;
			case '9': newState = 'a'; break;
			case 'a': if (outputs > 1) newState = 'b'; else newState = 'X'; break;
			case 'b': if (outputs > 2) newState = 'c'; else newState = 'X'; break;
			case 'c': if (outputs > 3) newState = 'd'; else newState = 'X'; break;
			case 'd': if (outputs > 4) newState = 'e'; else newState = 'X'; break;
			case 'e': if (outputs > 5) newState = 'f'; else newState = 'X'; break;
			case 'f': if (outputs > 6) newState = 'g'; else newState = 'X'; break;
			case 'g': if (outputs > 7) newState = 'h'; else newState = 'X'; break;
			case 'h': if (outputs > 8) newState = 'i'; else newState = 'X'; break;
			case 'i': if (outputs > 9) newState = 'j'; else newState = 'X'; break;
			case 'j': newState = 'X'; break;
			case 'X': newState = 'T'; break;
			}
			modalDialogButtonList[j].state = newState;
			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			modalDialogButtonList[j].draw(pg->hDC);
			//ctx.scale(pg->cs, pg->cs);
			break;

		case MODAL_CODE_NumericalValue: // not sure what this is **********************************************
			//modalDialog_mouseDragEnable = true;
			//rawpos = getMousePos(canvas, evt);
			//modalDialog_mouseDragX = rawpos.x;
			//modalDialog_mouseDragY = rawpos.y;

		case MODAL_CODE_InputLabel:
		case MODAL_CODE_OutputLabel:
		case MODAL_CODE_LiteralValue:
			if (modalDialogKeyboardEntry >= 0)
					modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = 0;
			if (modalDialogKeyboardEntry != -1) {
				if (modalDialogButtonList[j].enableKeyboard) { // continue keyboard edit
					int posX = x; // / pg->cs) - pg->cXo;
					int width = modalDialogButtonList[modalDialogKeyboardEntry].w;
					int left = modalDialogButtonList[modalDialogKeyboardEntry].x;
					std::string text = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
					modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = findCursorInText(pg->hDC, posX, left, width, text);
					//ctx.scale(1 / pg->cs, 1 / pg->cs);
					modalDialogButtonList[modalDialogKeyboardEntry].draw(pg->hDC);
					//ctx.scale(pg->cs, pg->cs);
				}
				else { // cancel keyboard edit
					if (modalDialogButtonList[j].code == MODAL_CODE_NumericalValue) {
						if (modalDialogButtonList[modalDialogKeyboardEntry].keyboardText.size())
							modalDialogButtonList[modalDialogKeyboardEntry].index = tomNumber(modalDialogButtonList[modalDialogKeyboardEntry].keyboardText);
						else
							modalDialogButtonList[modalDialogKeyboardEntry].index = 0.0;
					}
					else
						modalDialogButtonList[modalDialogKeyboardEntry].state = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
					modalDialogButtonList[modalDialogKeyboardEntry].enableKeyboard = false;
					int tempI = modalDialogKeyboardEntry;
					modalDialogKeyboardEntry = -1;
					//ctx.scale(1 / pg->cs, 1 / pg->cs);
					modalDialogButtonList[tempI].draw(pg->hDC);
					//ctx.scale(pg->cs, pg->cs);
				}
			}
			// start a new keyboard edit
			if (!modalDialogButtonList[j].enableKeyboard) {
				std::string text;
				modalDialogKeyboardEntry = j; // enable edit
				if (modalDialogButtonList[j].code == MODAL_CODE_NumericalValue)
					text = tomString(modalDialogButtonList[j].index);
				else
					text = modalDialogButtonList[j].state;
				modalDialogButtonList[j].keyboardText = text;
				//	modalDialog_mouseDragButton = modalDialogButtonList[j];//NEW vertical drag
				int posX = x;// / pg->cs) - pg->cXo;
				int left = modalDialogButtonList[j].x;
				int width = modalDialogButtonList[j].w;
				modalDialogButtonList[j].keyboardCursorPos = findCursorInText(pg->hDC, posX, left, width, text);
				modalDialogButtonList[j].enableKeyboard = true;
				//ctx.scale(1 / pg->cs, 1 / pg->cs);
				modalDialogButtonList[j].draw(pg->hDC);
				//ctx.scale(pg->cs, pg->cs);
			}
			//   drawCanvas();
			break;

		case MODAL_CODE_Checkbox:
			if (modalDialogButtonList[j].line)
				modalDialogButtonList[j].line = 0;
			else
				modalDialogButtonList[j].line = 1;
		
			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			modalDialogButtonList[j].draw(pg->hDC);
			//ctx.scale(pg->cs, pg->cs);
			break;
		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group

			// turn all the radios off
			group = modalDialogButtonList[j].index;
			for (int z = 0; z < modalDialogButtonList.size(); z++)
				if(modalDialogButtonList[z].code == MODAL_CODE_RadioGroup)
					if(modalDialogButtonList[z].index == group)
						modalDialogButtonList[z].line = 0;

			// turn the selected on
			if (modalDialogButtonList[j].line)
				modalDialogButtonList[j].line = 0;
			else
				modalDialogButtonList[j].line = 1;

			//ctx.scale(1 / pg->cs, 1 / pg->cs);
			modalDialogButtonList[j].draw(pg->hDC);
			//ctx.scale(pg->cs, pg->cs);
			break;
		case MODAL_CODE_PlaceComponent:
			if (pg->modalDialogComponent == -101) {
				pg->modalDialogComponent = 0;
				drawCanvas();
				break;
			}
			{
				pg->modalDialogComponent = -1;
				int line = modalDialogButtonList[j].index;
				LPCTSTR name = detailList[line]->name;
				POS pos;
				pos.x = x;
				pos.y = y;
				pg->mouseOffsetX = x;
				pg->mouseOffsetY = y;
				AddComponent(pg, name, pos, pg->visibleSheet);
				//	modalDialogComponent = -1;
				drawCanvas();
				return true;
			}
			break;
		case MODAL_CODE_ScanForLabels:
		case MODAL_CODE_ScanForUserLabels:
		case MODAL_CODE_ScanForNotes:
			//	findComponentList = modalDialogButtonList[j].line;
			//	findSheet*List = modalDialogButtonList[j].index;
			//	findIndex = 0;
			//	modalDialogComponent = -1;
			//	handleFindComponent();
			break;
		}
	}
	//console.log("Finished mousedown");
	return true;
}

bool selectDialogField(int jj) {

	if (modalDialogButtonList[jj].code == MODAL_CODE_LiteralValue) {
		modalDialogKeyboardEntry = jj;
		int width = modalDialogButtonList[modalDialogKeyboardEntry].state.size();
		modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = -width;
		modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = width;
		modalDialogButtonList[modalDialogKeyboardEntry].enableKeyboard = true;
		modalDialogButtonList[modalDialogKeyboardEntry].keyboardText = modalDialogButtonList[modalDialogKeyboardEntry].state;
		return true;
	}
	if (modalDialogButtonList[jj].code == MODAL_CODE_NumericalValue) {
		modalDialogKeyboardEntry = jj;
		modalDialogButtonList[modalDialogKeyboardEntry].keyboardText = tomString(modalDialogButtonList[modalDialogKeyboardEntry].index);
		int width = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText.size();
		modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = -width;
		modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = width;
		modalDialogButtonList[modalDialogKeyboardEntry].enableKeyboard = true;
		return true;
	}
	return false;
}

bool unselectDialogField(int jj) {
	if (!modalDialogButtonList[jj].enableKeyboard)
		return false;

	if (modalDialogButtonList[jj].code == MODAL_CODE_LiteralValue) {
		modalDialogButtonList[modalDialogKeyboardEntry].enableKeyboard = false;
		modalDialogButtonList[modalDialogKeyboardEntry].state = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
		modalDialogButtonList[jj].enableKeyboard = false;
		return true;
	}
	if (modalDialogButtonList[jj].code == MODAL_CODE_NumericalValue) {
		if (modalDialogButtonList[jj].keyboardText.size())
			modalDialogButtonList[jj].index = tomNumber(modalDialogButtonList[jj].keyboardText);
		modalDialogButtonList[jj].enableKeyboard = false;
		return true;
	}
	return false;
}

// draw the dialog
bool ModalNeedsInitialization = true;
void drawModalScreenDialog(Sheet* pg, HDC hDC) {

	if (pg->modalDialogComponent < 0) {
		ModalNeedsInitialization = true;
		return;
	}
	if (ModalNeedsInitialization) {
		// find first literal
		for (int jj = 0; jj < modalDialogButtonList.size(); jj++) {
			if ((modalDialogButtonList[jj].code == MODAL_CODE_LiteralValue) ||
			    (modalDialogButtonList[jj].code == MODAL_CODE_NumericalValue)) {
				selectDialogField(jj);
				break;
			}
		}
		ModalNeedsInitialization = false;
	}

	//ctx.scale(1 / pg->cs, 1 / pg->cs);
	for (int k = 0; k < modalDialogButtonList.size(); k++)
		modalDialogButtonList[k].draw(hDC);
	//ctx.scale(pg->cs, pg->cs);
}

std::string keyUniversalText(std::string text, int *cursorPosPtr, int keyCode) {

	if (*cursorPosPtr < 0)
		return text;

	// all other characters can just be inserted at cursor position
	std::string outText;
	outText = text.substr(0, *cursorPosPtr);
	outText += keyCode;
	outText.append(text.substr(*cursorPosPtr));
	return outText;
}

void keyboardDialogCancel(Sheet* pg) {
	pg->modalDialogComponent = -1;
	modalDialogKeyboardEntry = -1;
	drawCanvas();
}

bool keyboardDialogKeydown(Sheet* pg, int keyCode) {

	int cursorPos;
	std::string text;
	int selWidth;
	bool thisFunctionHandledIt = false;

	switch (keyCode)
	{
	case VK_LEFT: 
		cursorPos = modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos;
		if( cursorPos > 0)
			modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = cursorPos - 1;
		thisFunctionHandledIt = true;
		break;
	case VK_RIGHT: 
		cursorPos = modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos;
		text = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
		if( cursorPos < text.size())
			modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = cursorPos+1;
		thisFunctionHandledIt = true;
		break;
	case VK_UP: 	
		thisFunctionHandledIt = true;
		for (int jj = modalDialogKeyboardEntry - 1; jj > 0; jj--) {
			if ((modalDialogButtonList[jj].code == MODAL_CODE_LiteralValue) ||
				(modalDialogButtonList[jj].code == MODAL_CODE_NumericalValue)) {
				unselectDialogField(modalDialogKeyboardEntry);
				selectDialogField(jj);
				break;
			}
		}
		break;
	case VK_DOWN: 
		thisFunctionHandledIt = true;
		for (int jj = modalDialogKeyboardEntry + 1; jj < modalDialogButtonList.size(); jj++) {
			if ((modalDialogButtonList[jj].code == MODAL_CODE_LiteralValue) ||
				(modalDialogButtonList[jj].code == MODAL_CODE_NumericalValue)) {
				unselectDialogField(modalDialogKeyboardEntry);
				selectDialogField(jj);
				break;
			}
		}
		break;
	case VK_HOME:  
		//modalDialogKeyboardEntry = 0;
		//text = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
		//modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = text.size();
		thisFunctionHandledIt = true;
		break;
	case VK_DELETE:  
	case VK_BACK:  
		if (modalDialogKeyboardEntry < 0)
			break;
		text = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
		cursorPos = modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos;
		selWidth = modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth;

		if (selWidth == 0) {
			if (text.size() > 0) {
				std::string outText;
				if (cursorPos < 0)
					outText = text.substr(0, text.size() - 1); // delete last char
				else if (cursorPos > 0) {
					outText = text.substr(0, cursorPos - 1);
					outText.append(text.substr(cursorPos));
					cursorPos--;
				}
				modalDialogButtonList[modalDialogKeyboardEntry].keyboardText = outText;
				modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = cursorPos;
			}
		}
		if (selWidth < 0) {
			// delete chars before the cursor
			int start = cursorPos + selWidth;
			string text3 = text.substr(0, start);
			text3.append(text.substr(cursorPos));
			cursorPos = start;
			modalDialogButtonList[modalDialogKeyboardEntry].keyboardText = text3;
			modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = cursorPos;
		}
		else if (selWidth > 0) {
			// delete chars after the cursor
			string text4 = text.substr(0, cursorPos);
			text4.append(text.substr(cursorPos + selWidth));
			modalDialogButtonList[modalDialogKeyboardEntry].keyboardText = text4;
			modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = cursorPos;
		}
		modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = 0;
		thisFunctionHandledIt = true;
		break;
	case VK_TAB:
	case VK_NEXT:
		cursorPos = 0;
		thisFunctionHandledIt = true;
		break;
	case VK_CLEAR:
		cursorPos = 0;
		thisFunctionHandledIt = true;
		break;
	case VK_ESCAPE:
		keyboardDialogCancel(pg);
		thisFunctionHandledIt = true;
		break;
	}
	drawCanvas();
	return thisFunctionHandledIt;
}

bool keyboardDialogChar(Sheet* pg, int keyCode) {

	bool thisFunctionHandledIt = false;

	if (keyCode == 13) {
		modalDialogAcceptValues(pg);
		thisFunctionHandledIt = true;
	}
	else if (keyCode == 8) {
		// handled above
	}else{
		string text = modalDialogButtonList[modalDialogKeyboardEntry].keyboardText;
		int cursorPos = modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos;
		int selWidth = modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth;

		if (selWidth < 0) {
			// delete chars before the cursor
			int start = cursorPos + selWidth;
			string text3 = text.substr(0, start);
			text3.append(text.substr(cursorPos));
			cursorPos = start;
			text = text3;
		}
		else if (selWidth > 0) {
			// delete chars after the cursor
			string text4 = text.substr(0, cursorPos);
			text4.append(text.substr(cursorPos+selWidth));
			text = text4;
		}
		
		std::string result = keyUniversalText(text, &cursorPos, keyCode);
		
		modalDialogButtonList[modalDialogKeyboardEntry].keyboardText = result;
		modalDialogButtonList[modalDialogKeyboardEntry].keyboardCursorPos = cursorPos+1;
		modalDialogButtonList[modalDialogKeyboardEntry].selectionWidth = 0;
		thisFunctionHandledIt = true;
	}
	drawCanvas();
	return thisFunctionHandledIt;
}

// ****************************** lets try some generics ***********************************



void(*returnMethod)(Sheet*, int i, double*) = returnMethod;
/*
void finishNumericalValuesDialog(Sheet* pg, int i) {

	int k = 0;
	double values[10];
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			values[k++] = modalDialogButtonList[but].index;
			break;
		}
		if (k >= 10)
			break;
	}

	returnMethod(pg, i, values);
}
*/
/*
void buildNumericalValuesDialog(Sheet* pg, int i, vector<string> labels, vector<int> settingIndex, int fieldWidth, void(*_returnMethod)(Sheet*, int, double*)) {
	returnMethod = _returnMethod;
	finishDialog = finishNumericalValuesDialog;

	int x, y, w, h, n, line;
	int lGap = 75;
	int wid = 150;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int values = labels.size();
	int labelWidth = 0;
	for (int z = 0; z < values; z++) {
		int width = calcStringWidthFont(pg->hDC, labels[z]);
		if (width > labelWidth)
			labelWidth = width;
		if (labelWidth < 40)
			n = 0;
	}
	
	lGap = labelWidth + 5;

	// build the dialog frame
	int mdlWidth = lGap + wid + 30;
	int mdlHeight = tGap + values * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = convertLPCTSTR_toString(pg->dgrm.cL[i]->label);
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;

	for (int z = 0; z < values; z++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, labels[z])); // line label
		double value = pg->dgrm.cL[i]->setting[settingIndex[z]];
		modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, fieldWidth, h, MODAL_CODE_NumericalValue, line, value, "")); // value
		lineIndex++;
		y += yStep;
	}
}
*/


// **************** N Numerical Values, N Checkboxes, N Radio buttons in 1 group **************************


void(*returnMethod2)(Sheet* pg, int i, double* values, bool* checks, int radio) = returnMethod2;

void finishMultipleValuesDialog(Sheet* pg, int i) {

	int k_dbl = 0;
	double doubleValues[10];
	int k_ck = 0;
	bool boolValues[12];
	int k_rad = 0;
	int radioValue = 0;
	
	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_NumericalValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				if (modalDialogButtonList[but].keyboardText.size())
					modalDialogButtonList[but].index = tomNumber(modalDialogButtonList[but].keyboardText);
				modalDialogButtonList[but].enableKeyboard = false;
			}
			doubleValues[k_dbl++] = modalDialogButtonList[but].index;
			break;

		case MODAL_CODE_Checkbox:
			if (modalDialogButtonList[but].line)
				boolValues[k_ck++] = true;
			else
				boolValues[k_ck++] = false;
			break;

		case MODAL_CODE_RadioGroup:
			//line -- for radio group this is the on/off for the line
			//index -- for the radio group this is the group
			int group = modalDialogButtonList[but].index;
			if (group >= 5)
				break;
			if (modalDialogButtonList[but].line)
				radioValue = k_rad;
			k_rad++;
			break;
		}
	
		if (k_dbl >= 10)
			break;
		if (k_ck >= 12)
			break;
	}

	if( returnMethod2 )
		returnMethod2(pg, i, doubleValues, boolValues, radioValue);
	else
		pg->dgrm.cL[i]->completion(pg, doubleValues, boolValues, radioValue);
}

void buildMultipleValuesDialog(Sheet* pg, int i, 
	vector<string> doubleLabels, vector<double> doubleValue, int doubleFieldWidth,
	vector<string> checkLabels, vector<bool> checkValues,
	vector<string> radioLabels, int radioIndex,
	void(*_returnMethod2)(Sheet*, int, double*, bool*, int)) 
{
	returnMethod2 = _returnMethod2;
	finishDialog = finishMultipleValuesDialog;

	int x, y, w, h, n, line;
	int lGap = 75;
	int wid = 150;
	int tGap = 30;
	int yStep_double = 30;
	int yStep_check = 30;
	int yStep_radio = 20;
	n = 0;

	int doubles = doubleLabels.size();
	int checks = checkLabels.size();
	int radios = radioLabels.size();
	int values = doubles + checks + radios;

	// build the dialog frame
	int mdlWidth = lGap + wid + 30;
	int mdlHeight = tGap + doubles * yStep_double + checks * yStep_check + radios * yStep_radio + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = convertLPCTSTR_toString(pg->dgrm.cL[i]->label);
	//text.append(pg->dgrm.cL[i]->userLabel);
	text = trimTextToFit(pg->hDC, text, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;
	line = 0;

	// ------------- first the double values -----------------
	int doubleLabelWidth = 0;
	for (int z = 0; z < doubles; z++) {
		int width = calcStringWidthFont(pg->hDC, doubleLabels[z]);
		if (width > doubleLabelWidth)
			doubleLabelWidth = width;
	}
	lGap = doubleLabelWidth + 5;

	for (int z = 0; z < doubles; z++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 0, 0, MODAL_CODE_InputLabel, 0, line, doubleLabels[z])); // line label
		modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, doubleFieldWidth, h, MODAL_CODE_NumericalValue, line, doubleValue[z], "")); // value
		y += yStep_double;
	}

	// --------------- then the checkboxes --------------------
	for (int z = 0; z < checks; z++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 20, 20, MODAL_CODE_Checkbox, checkValues[z], 0, checkLabels[z]));
		y += yStep_check;
	}

	// --------------- then the radio buttons --------------------
	for (int z = 0; z < radios; z++) {
		modalDialogButtonList.push_back(ModalDialogButton(x, y, 15, 15, MODAL_CODE_RadioGroup, (z == radioIndex), 1, radioLabels[z]));
		y += yStep_radio;
	}
}


void buildMultipleValuesDialog(Sheet* pg, int i,
	vector<string> doubleLabels, vector<double> doubleSettingIndex, int doubleFieldWidth,
	void(*_returnMethod2)(Sheet*, int, double*, bool*, int)){
	
	vector<string> checkLabels;
	vector<bool> checkValues;
	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleSettingIndex, doubleFieldWidth,
		checkLabels, checkValues,
		radioLabels, radioIndex,
		_returnMethod2);
}
// ***************************** FIND Component ********************************************************



void toUpper(basic_string<char>& s) {
	for (basic_string<char>::iterator p = s.begin();
		p != s.end(); ++p) {
		*p = toupper(*p); // toupper is for char
	}
}

void skipWhitespace(LPCTSTR text, int* startPtr, int length) {
	for (int k = 0; k < length; k++) {
		int theChar = text[*startPtr + k];
		if ((theChar != ' ') &&
			(theChar != '.') &&
			(theChar != '(') &&
			(theChar != ')') &&
			(theChar != '/') &&
			(theChar != ',')) {
			*startPtr += k;
			break;
		}
	}
}

int getWordLength(LPCTSTR text, int start, int length) {
	
	for (int k = 0; k < length; k++) {
		int theChar = text[start + k];
		if ((theChar == ' ') ||
			(theChar == '.') ||
			(theChar == '(') ||
			(theChar == ')') ||
			(theChar == '/') ||
			(theChar == ',')) {
			return k;
		}
	}
	return length; // so we get the last word too
}

bool buildHelpDialogButtons(Sheet* pg, string keyPhrase) {
	pg->modalDialogComponent = 0;
	int x, y, w, h, n, line;
	int lGap = 20;
	int wid = pg->workspaceWidth - 90; // nice and wide
	int tGap = 30;
	int yStep = 20;
	n = 0;
	
	toUpper(keyPhrase);
	vector<int> keyWordLen;
	vector<string> keyWord;
	int j = 0;
	int stringLen = keyPhrase.size();
	string temp;
	int tempLen = 0;
	while (j < stringLen) {
		
		if ((keyPhrase[j] == ' ') || (keyPhrase[j] == ',')) {
			if (tempLen == 0)
				continue;
			keyWordLen.push_back(tempLen);
			keyWord.push_back(temp);
			temp = "";
			tempLen = 0;
		}
		else {
			tempLen++;
			temp += keyPhrase[j];
		}
		j++;
	}
	if (tempLen > 0) {
		keyWordLen.push_back(tempLen);
		keyWord.push_back(temp);
	}

	// first search for components - later may also search for comments and user entered strings
	vector<int> keyMatches;
	loadDetails();

	// now search for details that contain the keyword
	int detailListSize = detailList.size();
	for (int j = 0; j < detailListSize; j++ ) {
		
		if (!detailList[j]->isComponent)
			continue;

		// search the text for words that match keyword
		int anchor = 0;
		int detailChars = _tcslen(detailList[j]->text);

		while (anchor < detailChars) { // scan each detail text
			// skip until a character is reached
			int remaining = detailChars - anchor;
			skipWhitespace(detailList[j]->text, &anchor, remaining);
			
			// parse the next word
			remaining = detailChars - anchor;
			int nextWordLen = getWordLength(detailList[j]->text, anchor, remaining);
			if (nextWordLen <= 0)
				break;

			// test each keyword for a match
			int goNextDetail = false;
			for (int m = 0; m < keyWordLen.size(); m++) {
				if (nextWordLen == keyWordLen[m]) { // can only match if the word length is the same as the keyword

					bool match = true;
					for (int k = 0; k < keyWordLen[m]; k++) {
						int a = toupper(detailList[j]->text[anchor + k]);
						int b = keyWord[m][k];
						if (a != b) {
							match = false; // no match stop the comparison 
							break;
						}
					}
					if (match) {
						keyMatches.push_back(j);
						goNextDetail = true;
						break; 
					}
				}
			}
			if (goNextDetail)
				break;
			// move to next word start
			anchor += nextWordLen + 1;
		}
	}
/*
	// now search the components for labels, comments and notes
	// record the component index and the sheet where each was found
	var label_componentIndex = [];
	var label_sheetsNumber = [];
	var userLabel_componentIndex = [];
	var userLabel_sheetsNumber = [];
	var note_componentIndex = [];
	var note_sheetsNumber = [];
	for (var s = 0; s < visibleSheets; s++) {
		for (var i = 0; i < pgL[s].cL.length; i++) {
			if (pgL[s].cL[i].label.toUpperCase().indexOf(UpperKeyword) != -1) {
				label_componentIndex[label_componentIndex.length] = i;
				label_sheetsNumber[label_sheetsNumber.length] = s;
			}
			if (pgL[s].cL[i].userLabel.toUpperCase().indexOf(UpperKeyword) != -1) {
				userLabel_componentIndex[userLabel_componentIndex.length] = i;
				userLabel_sheetsNumber[userLabel_sheetsNumber.length] = s;
			}
			if (pgL[s].cL[i].userNote.toUpperCase().indexOf(UpperKeyword) != -1) {
				note_componentIndex[note_componentIndex.length] = i;
				note_sheetsNumber[note_sheetsNumber.length] = s;
			}
		}
	}
*/
	int lines = keyMatches.size();
	//if (label_componentIndex.length)
	//	lines++;
	//if (userLabel_componentIndex.length)
	//	lines++;
	//if (note_componentIndex.length)
	//	lines++;

	int maxLines = floor((pg->workspaceHeight - 150 - tGap) / yStep);

	if (lines == 0) { //&&
		//(label_componentIndex.length == 0) &&
		//(userLabel_componentIndex.length == 0) &&
		//(note_componentIndex.length == 0)) {
		MessageBox(NULL, L"Could not find any components, labels, or notes containing the keyword.", L"Find", MB_OK);
		return false;
	}
	// build the dialog frame
	int mdlWidth = lGap + wid + lGap;
	int mdlHeight = tGap + min(lines, maxLines) * yStep + 50;
	int mdlLeft = 30;
	int mdlTop = 30;

	mdlHeight += 10;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	string text = "Find: ";// +keyword, mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the cancel button
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	// headings
	x = mdlLeft + lGap;
	y = mdlTop + tGap;

	// list the components found
	for (line = 0; line < keyMatches.size(); line++) {
		//                                                 x, y,  w,  h,    code,                    line, index,     state
		modalDialogButtonList.push_back(ModalDialogButton(x, y, wid, 20, MODAL_CODE_PlaceComponent, line, keyMatches[line],
			convertLPCTSTR_toString(detailList[keyMatches[line]]->text) ));
		y += yStep;
		if (line >= maxLines)
			return true;
	}
/*
	if (label_componentIndex.length) {
		if (label_componentIndex.length == 1)
			var stateText = "Found 1 component with names matching this text.";
		else
			stateText = "Found " + label_componentIndex.length + " components with names matching this text. Use the 'Esc' key to advance to the next.";
		modalDialogButtonList[n++] = new ModalDialogButton(x, y, wid, 20, MODAL_CODE_ScanForLabels, label_componentIndex, label_sheetsNumber, stateText);
		y += yStep;
		line++;
		if (line >= maxLines)
			return true;
	}
	if (userLabel_componentIndex.length) {
		if (userLabel_componentIndex.length == 1)
			stateText = "Found 1 component with labels matching this text.";
		else
			stateText = "Found " + userLabel_componentIndex.length + " components with labels matching this text. Use the 'Esc' key to advance to the next.";
		modalDialogButtonList[n++] = new ModalDialogButton(x, y, wid, 20, MODAL_CODE_ScanForUserLabels, userLabel_componentIndex, userLabel_sheetsNumber, stateText);
		y += yStep;
		line++;
		if (line >= maxLines)
			return true;
	}
	if (note_componentIndex.length) {
		if (note_componentIndex.length == 1)
			stateText = "Found 1 component with notes matching this text.";
		else
			stateText = "Found " + note_componentIndex.length + " components with notes matching this text. Use the 'Esc' key to advance to the next.";
		modalDialogButtonList[n++] = new ModalDialogButton(x, y, wid, 20, MODAL_CODE_ScanForNotes, note_componentIndex, note_sheetsNumber, stateText);
		//y += yStep;
		line++;
		if (line >= maxLines)
			return true;
	}
*/
	
	return true;
}


void finishFindComponentDialog(Sheet* pg, int i) {

	for (int but = 0; but < modalDialogButtonList.size(); but++) {
		switch (modalDialogButtonList[but].code) {
		case MODAL_CODE_LiteralValue:
			if (modalDialogButtonList[but].enableKeyboard) {
				modalDialogButtonList[but].state = modalDialogButtonList[but].keyboardText;
				modalDialogButtonList[but].enableKeyboard = false;
			}
			findKeyword = modalDialogButtonList[but].state;
			break;
		}
	}
	pg->modalDialogComponent = -100;
}



void buildFindComponentDialogButtons(Sheet* pg) {
	pg->modalDialogComponent = 0; // not really a component just to get this dialog to appear
	finishDialog = finishFindComponentDialog;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 20;
	n = 0;

	int columns = 1;
	int litValues = 1;
	int mdlWidth = 250;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	int wid = (mdlWidth - (columns + 1) * (lGap + 10)) / columns;
	// var mdlWidth = lGap + (wid + lGap) * columns;
	int mdlHeight = tGap + litValues * yStep + 50;
	if (mdlHeight > tGap + yStep + 50)
		mdlHeight = tGap + yStep + 50;


	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Search component details for a word: ", mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - w - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	modalDialogButtonList.push_back(ModalDialogButton(x + 70, y, w + 9, h, MODAL_CODE_Cancel, 0, 0, ""));

	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y, wid, h, MODAL_CODE_LiteralValue, 0, 0, "")); // value

}



void buildHelpComponentDialogButtons(Sheet* pg) {
	pg->modalDialogComponent = 22222; // not really a component just to get this dialog to appear
	finishDialog = NULL;

	int x, y, w, h, n;
	int lGap = 30;
	int tGap = 30;
	int yStep = 16;
	n = 0;


	int columns = 1;
	int litValues = 1;
	int mdlWidth = 550;

	// build the dialog frame

	int mdlLeft = 30;
	int mdlTop = 30;
	
	int mdlHeight = tGap + 20 * yStep + 50;

	modalDialogButtonList.clear();
	// first build the outer frame - use just the button coordinates
	std::string text = trimTextToFit(pg->hDC, "Help: ", mdlWidth);
	modalDialogButtonList.push_back(ModalDialogButton(mdlLeft, mdlTop, mdlWidth, mdlHeight, MODAL_CODE_Frame, 0, 0, text));

	// build the ok / cancel buttons
	w = 42;
	h = 20;
	x = mdlLeft + mdlWidth / 2 - 10;
	y = mdlTop + mdlHeight - 30;
	modalDialogButtonList.push_back(ModalDialogButton(x, y, w, h, MODAL_CODE_Ok, 0, 0, ""));
	
	h = 14;
	int lineIndex = 0;
	x = mdlLeft + 10;
	y = mdlTop + tGap;

	modalDialogButtonList.push_back(ModalDialogButton(x + 150,  y,          0, 0, MODAL_CODE_StaticText, 0, 0, " Math Science and Technology Workshop"));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += 2*yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "This application lets you create programs by connecting components. There are hundreds of "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "components available. The menus on the top left group components into catagories. Select "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "a menu and look for the component you need. You can place the component anywhere on this "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "workspace(window). Start with a component that generates a value, for example the Slider "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "in the Controls menu. Then place some function to the right of the Slider. For example the "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "Square component in the Functions menu. Now you can connect the two components by letting "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "their terminals touch. Alternatively, you can click on one of the terminals and drag a wire "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "to the other terminal. Now we need to display the output of the Square function. Use the "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "Display List component in the Controls menu. Connect it to the output of the Square function. "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "Finally, you can drag the white circle in the Slider and view the square of the number selected. "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "An easy way to search for a component is to start typing the component's name. In the example  "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "above, to find the slider type SLIDE - all the components will dim and only the ones with SLIDE "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "in their name or description will be bright. To clear your search use the Enter key. You can "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "Also search by clicking the Find button in the upper right. Type a word and hit OK and all the "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "matching components will be listed. You can click on any of the lines in that dialog to create "));
	modalDialogButtonList.push_back(ModalDialogButton(x + lGap, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "that component. "));
	y += yStep;
	modalDialogButtonList.push_back(ModalDialogButton(x + 160, y += yStep, 0, 0, MODAL_CODE_StaticText, 0, 0, "More at: https://tpandolfi.wixsite.com/main"));
}