
#include "stdafx.h"
#include "MethodComponents.h"
#include "FunctionComponents.h"
#include "CanvasComponents.h"
#include "Sheet.h"
#include "structs.h"
#include "Text.h"
#include <string>
#include "Windowsx.h"
#include "ScreenDialog.h"
#include "color.h"
#include "MST01.h"
#include "ButtonGroups.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)


using namespace std;
extern Sheet* page;

int bitSetArray[16] = { 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000 };
int bitClearArray[16] = { 0xFFFE, 0xFFFD, 0xFFFB, 0xFFF7, 0xFFEF, 0xFFDF, 0xFFBF, 0xFF7F, 0xFEFF, 0xFDFF, 0xFBFF, 0xF7FF, 0xEFFF, 0xDFFF, 0xBFFF, 0x7FFF };



void rotationActionGeneral(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildRotationDialogButtons(pg, i);
}

extern HDC magic_hDC;

// ***************** Canvas_Component **************************************

int Canvas_Component::method2(Sheet* pg) {
	return 2;
}

void Canvas_Component::getPositionGroup(int group, int* pinptr, int k) {
	inputX = getLoopingValueD(group, pinptr, k, 0);
	inputY = getLoopingValueD(group, pinptr, k, 0);
}

void Canvas_Component::getSizeGroup(int group, int* pinptr, int k, double def) {
	inputSizeX = getLoopingValueD(group, pinptr, k, def);
	inputSizeY = getLoopingValueD(group, pinptr, k, def);
}
void Canvas_Component::getColorLineGroup(int group, int* pinptr, int k) {
	inputLineRed = getLoopingValueColor(group, pinptr, k);
	inputLineGreen = getLoopingValueColor(group, pinptr, k);
	inputLineBlue = getLoopingValueColor(group, pinptr, k);
}
void Canvas_Component::getColorFillGroup(int group, int* pinptr, int k) {
	inputFillRed = getLoopingValueColorSplit(group, pinptr, k);
	inputFillGreen = getLoopingValueColorSplit(group, pinptr, k);
	inputFillBlue = getLoopingValueColorSplit(group, pinptr, k);
}
Canvas_Component::Canvas_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS) {
	
}


//******************************

double canvasToSize(Sheet* pg, double canSize, int displayMode) {
	switch (displayMode) {
	case 0:
		return canSize / pg->canvas2Scale;
	case 1:
		return canSize;
	}
	return 0.0;
}

double canvasToX(Sheet* pg, double canx) {
	double out = (canx - pg->canvas2Width / 2) / pg->canvas2Scale + pg->canvas2xOffset;
	return out;
}

double canvasToY(Sheet* pg, double cany) {
	double out = (pg->canvas2Height / 2 - cany) / pg->canvas2Scale + pg->canvas2yOffset;
	return out;
}


double canvasToX2(Sheet* pg, double canx, int displayMode) {
	switch (displayMode) {
	case 0:
		return canvasToX(pg, canx);
	case 1:
		return canx - pg->canvas2Width / 2;
	}
	return 0;
}

double canvasToY2(Sheet* pg, double cany, int displayMode) {
	switch (displayMode) {
	case 0:
		return canvasToY(pg, cany);
	case 1:
		return pg->canvas2Height / 2 - cany;
	}
	return 0;
}




double xTo2dCanvas(Sheet* pg, double x) {
	double out = (x - pg->canvas2xOffset) * pg->canvas2Scale + pg->canvas2Width / 2;
	if (out > 4E8)
		out = 4E8;
	else if (out < -4E8)
		out = -4E8;
	return out;
}

double yTo2dCanvas(Sheet* pg, double y) {
	double out = pg->canvas2Height / 2 - (y - pg->canvas2yOffset) * pg->canvas2Scale;
	if (out > 4E8)
		out = 4E8;
	else if (out < -4E8)
		out = -4E8;
	return out;
}


double sizeTo2dCanvas(Sheet* pg, double inputSize, int displayMode) {
	switch (displayMode) {
	case 0:
		return inputSize * pg->canvas2Scale;
	case 1:
		return inputSize;
	}
	return 0.0;
}
double xTo2dCanvas2(Sheet* pg, double x, int displayMode) {
	switch (displayMode) {
	case 0:
		return xTo2dCanvas(pg, x);
	case 1:
		return x + pg->canvas2Width / 2;
	}
	return 0;
}
double yTo2dCanvas2(Sheet* pg, double y, int displayMode) {
	switch (displayMode) {
	case 0:
		return yTo2dCanvas(pg, y);
	case 1:
		return pg->canvas2Height / 2 - y;
	}
	return 0;
}





std::wstring decodeUTF8(std::string inText ) { // doesnt work
	
	int len = inText.size();
	wstring outText;
	int number;
	for (int j = 0; j < len; j++) {
		if ((inText[j] == '&') || (inText[j] == '~')) {
			if ((j < len - 5) && (inText[j + 5] == ';')) {
				number = std::stoi(inText.substr(j + 1, 4));
				outText += number;
				j += 5;
			}
			else if ((j < len - 4) && (inText[j + 4] == ';')) {
				number = std::stoi(inText.substr(j + 1, 3));
				outText += number;
				j += 4;
			}
			else
				outText += inText[j];
		}
		else
			outText += inText[j];
	}
	return outText;
}



int canvas2MouseX = 0;
int canvas2MouseY = 0;

//             left, center, right, extra, extra
bool canvas2MouseButtons[MOUSE_BUTTON_COUNT] = { false, false, false, false, false };
bool draggingCanvas2 = false;
int canvas2MouseDownX = 0;
int canvas2MouseDownY = 0;
int cranking2dComponent = -1;

bool send2dMouseUpdate(Sheet* pg, int mouseX, int mouseY, bool* mouseButtons, int zDelta) {
	bool change = false;
	for (int i = 0; i < pg->dgrm.cL.size(); i++)
		if (pg->dgrm.cL[i]->canvasMouseMoved(pg, mouseX, mouseY, mouseButtons, zDelta))
			change = true;
	return change;
}

void MouseDownCanvasListener(Sheet* pg, LPARAM lParam) {
	if (pg->hDC == NULL)
		return;
	canvas2MouseButtons[MOUSE_BUTTON_LEFT] = true;
	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);


	// search for 2D cranks
	bool somethingWasHit = false;
	for (int i = pg->dgrm.cL.size() - 1; i >= 0; i--) {
		if (pg->dgrm.cL[i]->ghost)
			continue;

		// check for a hit ------------------
		if (pg->dgrm.cL[i]->crank2d(pg, rawpos.x, rawpos.y, true)) {
			//pg->crank2dComp[touch] = i;
			cranking2dComponent = i;
			somethingWasHit = true;
			break;
		}
		if (send2dMouseUpdate(pg, rawpos.x, rawpos.y, canvas2MouseButtons, 0))
			somethingWasHit = true;
	}
	if (somethingWasHit) {
		//orbitControls ? orbitControls.noPan = true : undefined;
		propagateValues(&pg->dgrm);
		drawCanvas();
		drawCanvas2();
	}
	else {
		//if (okToDragCanvas2()) {
		//	draggingCanvas2 = true;
		//	canvas2MouseDownX = pos.x;
		//	canvas2MouseDownY = pos.y;
		//}
	}

}



void MouseMoveCanvasListener(Sheet* pg, LPARAM lParam)
{
	if (pg->hDC2 == NULL)
		return;

	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);

	if (cranking2dComponent != -1) {
		pg->dgrm.cL[cranking2dComponent]->crank2d(pg, rawpos.x, rawpos.y, false);
		propagateValues(&pg->dgrm);
		drawCanvas();
		drawCanvas2();
	}
	else
	{
		// send all components a message that the mouse moved
		canvas2MouseX = rawpos.x;
		canvas2MouseY = rawpos.y;
		if( send2dMouseUpdate(pg, rawpos.x, rawpos.y, canvas2MouseButtons, 0)) {
			propagateValues(&pg->dgrm);
			drawCanvas();
			drawCanvas2();
		}

	}
}

void MouseUpCanvasListener(Sheet* pg, LPARAM lParam) {
	if (pg->hDC2 == NULL)
		return;
	canvas2MouseButtons[MOUSE_BUTTON_LEFT] = false;
	cranking2dComponent = -1;

	bool change = false;
	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		if (pg->dgrm.cL[i]->ghost)
			continue;

		if (send2dMouseUpdate(pg, canvas2MouseX, canvas2MouseY, canvas2MouseButtons, 0))
			change = true;

		//if (pg->dgrm.cL[i]->onMouseUp2d != NULL) {
			pg->dgrm.cL[i]->onMouseUp2d(pg);
			change = true;
		//}

		if (pg->dgrm.cL[i]->type == L"Button") {
			if (pg->dgrm.cL[i]->setting[2] == 0) { // momentary
				if (pg->dgrm.cL[i]->output[0].value[0]) {
					pg->dgrm.cL[i]->output[0].value[0] = 0;
					change = true;
				}
			}
		}
	}
	if (change) {
		propagateValues(&pg->dgrm);
		drawCanvas();
		drawCanvas2();
	}

}


void MouseUpDownCanvasButtonListener(Sheet* pg, int buttonIndex, bool down) {
	canvas2MouseButtons[buttonIndex] = down;
	if (send2dMouseUpdate(pg, canvas2MouseX, canvas2MouseY, canvas2MouseButtons, 0)) {
		propagateValues(&pg->dgrm);
		drawCanvas();
		drawCanvas2();
	}
}


void MouseWheelCanvasButtonListener(Sheet* pg, int zDelta) {

	if (send2dMouseUpdate(pg, canvas2MouseX, canvas2MouseY, canvas2MouseButtons, zDelta)) {
		propagateValues(&pg->dgrm);
		drawCanvas();
		drawCanvas2();
	}

}


// ************************** Display Mode - Option to Ignore Display Scale and Offsets ****************

void button2dDisplayMode_Completion( Sheet* pg, int i, double *doubleValues, bool *boolValues, int radioValue) {
	pg->dgrm.cL[i]->displayMode = boolValues[0];
}
void doubleClick2dDisplayMode(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> checkLabels{ "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		checkLabels, checkValues, {}, 0, button2dDisplayMode_Completion);
}

// *************** Display2 Mode and Dash - Option to Ignore Display Scale and Offsets AND set the dashing mode ****************

void button2dDisplayModeAndDash_Completion(Sheet* pg, int i, double *doubleValues, bool *boolValues, int radioValue) {
	pg->dgrm.cL[i]->displayMode = boolValues[0];
	switch (radioValue) {
	case 0:	pg->dgrm.cL[i]->setting[1] = PS_SOLID; break;
	case 1:	pg->dgrm.cL[i]->setting[1] = PS_DASH; break;
	case 2:	pg->dgrm.cL[i]->setting[1] = PS_DOT; break;
	case 3:	pg->dgrm.cL[i]->setting[1] = PS_DASHDOT; break;
	case 4:	pg->dgrm.cL[i]->setting[1] = PS_DASHDOTDOT; break;
	}
}
void doubleClick2dDisplayModeAndDash(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> checkLabels{ "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels = { "Solid", "Dash", "Dot", "Dash Dot", "Dash Dot Dot" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		button2dDisplayModeAndDash_Completion);
}

// *************** Display2 Mode, Dash, and Offset ****************

void button2dDisplayModeAndDashAndOffset_Completion(Sheet* pg, int i, double* doubleValues, bool* boolValues, int radioValue) {

	pg->dgrm.cL[i]->setting[2] = doubleValues[0]; // x offset
	pg->dgrm.cL[i]->setting[3] = doubleValues[1]; // y offset

	pg->dgrm.cL[i]->displayMode = boolValues[0];
	switch (radioValue) {
	case 0:	pg->dgrm.cL[i]->setting[1] = PS_SOLID; break;
	case 1:	pg->dgrm.cL[i]->setting[1] = PS_DASH; break;
	case 2:	pg->dgrm.cL[i]->setting[1] = PS_DOT; break;
	case 3:	pg->dgrm.cL[i]->setting[1] = PS_DASHDOT; break;
	case 4:	pg->dgrm.cL[i]->setting[1] = PS_DASHDOTDOT; break;
	}
}
void doubleClick2dDisplayModeAndDashAndOffset(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "X Offset:",					"Y Offset:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels{ "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels = { "Solid", "Dash", "Dot", "Dash Dot", "Dash Dot Dot" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		button2dDisplayModeAndDashAndOffset_Completion);
}


// *********************** Box *******************************

bool Canvas_BOX_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops;
	//int gradient = null;
	//if setting[0] & 64) { // check for gradient
	//	loops = inputsToLoopsN(pg, i, leftTerminalCount - 2);
	//	if (loops == 0)
	//		return false;
	//	gradient = getGradient(pg, i, leftTerminalCount - 2);
	//}
	//else {
		loops = inputsToLoops();
		if (loops == 0)
			return false;
	//}

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getSizeGroup(group++, &pin, k, 10);
		if ((inputSizeX <= 0) || (inputSizeY <= 0))
			continue;
		double inputLineWidth = getLoopingValueD(group++, &pin, k, 1);
		if (inputLineWidth <= 0)
			continue;
		getColorLineGroup(group++, &pin, k);
		getColorFillGroup(group, &pin, k);
		bool enablefill = false;
		if ((int)setting[0] & bitSetArray[group])
			enablefill = true;
		group++;
		double inputAlpha = getLoopingValueD(group++, &pin, k, 1);
		if ((inputAlpha <= 0) ) //&& (gradient == null)) *******************
			continue;
	
		double inputWidth = sizeTo2dCanvas(pg, inputSizeX, displayMode);
		if ((inputWidth <= 0) || (inputWidth > 10000))
			continue;
		double inputHeight = sizeTo2dCanvas(pg, inputSizeY, displayMode);
		if ((inputHeight <= 0) || (inputHeight > 10000))
			continue;


		int xx = xTo2dCanvas2(pg, inputX + setting[2] * inputWidth/2, displayMode);
		int yy = yTo2dCanvas2(pg, inputY + setting[3] * inputHeight/2, displayMode);

		BeginPath(pg->hDC2);
		Rectangle(pg->hDC2, xx - inputWidth / 2, yy - inputHeight / 2, xx + inputWidth / 2, yy + inputHeight / 2);
		EndPath(pg->hDC2);

		 // fill a box with color
		if (enablefill || inputLineWidth) {
			HBRUSH fillColor = NULL;
			HPEN hPen = NULL;

			if (enablefill) {
				fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));// inputAlpha);
				SelectObject(pg->hDC2, fillColor);
			}
			if (inputLineWidth) {
				double lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				hPen = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
				SelectObject(pg->hDC2, hPen);
			}
			if (enablefill && inputLineWidth)
				StrokeAndFillPath(pg->hDC2);
			else if (enablefill)
				FillPath(pg->hDC2);
			else if((inputLineWidth))
				StrokePath(pg->hDC2);

			if (inputLineWidth)
				DeleteObject(hPen);
			if (enablefill)
				DeleteObject(fillColor);
		}
		
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}


std::vector<Button*>boxButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDashAndOffset) };

bool Canvas_BOX_Component::onLoad(Sheet* pg) {
	if (setting.size() < 4) {
		setSetting(1, 0); // dashing
		setSetting(2, 0); // x offset
		setSetting(3, 0); // y offset
	}
	buildPinLabels();
	return true;
}
vector<int> Canvas_BOX_Component::GetPinGroups() { return vector<int>{2, 2, 1, 3, 3, 1 }; }
vector<string> Canvas_BOX_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", 			// 0
		"Width", "Height",						// 1
		"Line Width",							// 2
		"Line Red", "Line Green", "Line Blue",	// 3
		"Fill Red", "Fill Green", "Fill Blue",	// 4
		"Alpha" };								// 5
}
Canvas_BOX_Component::Canvas_BOX_Component()
	: Canvas_Component(L"BOX", L"2D Box", squareShape, 5, 1, 0, 0)
{
	dialogButtonList = boxButtonList;
	setSetting(0, 1+2); // input pins
	setSetting(1, 0); // dashing
	setSetting(2, 0); // x offset
	setSetting(3, 0); // y offset
	pinMaskIndex = 0;
	width = 35;
}


// *********************** Circle *******************************

bool Canvas_CIRCLE_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops;
	//int gradient = null;
	//if setting[0] & 128) { // check for gradient
	//	loops = inputsToLoopsN(pg, i, leftTerminalCount - 2);
	//	if (loops == 0)
	//		return false;
	//	gradient = getGradient(pg, i, leftTerminalCount - 2);
	//}
	//else {
	loops = inputsToLoops();
	if (loops == 0)
		return false;
	//}

	int dashMode = setting[1];

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		double inputRadius = getLoopingValueD(group++, &pin, k, 10);
		
		double inputArcBegin = getLoopingValueD(group, &pin, k, 0);
		double inputArcEnd = getLoopingValueD(group++, &pin, k, TWO_PI);
		
		double inputLineWidth = getLoopingValueD(group++, &pin, k, 1);
		if (inputLineWidth <= 0)
			continue;
	
		double inputArrowSize = getLoopingValueD(group++, &pin, k, 0);
	
		getColorLineGroup(group++, &pin, k);
		getColorFillGroup(group, &pin, k);
		bool enableFill = false;
		if ((int)setting[0] & bitSetArray[group])
			enableFill = true;
		group++;
		double inputAlpha = getLoopingValueD(group++, &pin, k, 1);
		if (inputAlpha <= 0) // && (gradient == null))
			continue;

		double xx = xTo2dCanvas2(pg, inputX + setting[2] * inputRadius, displayMode);
		double yy = yTo2dCanvas2(pg, inputY + setting[3] * inputRadius, displayMode);
		double radiusValue = sizeTo2dCanvas(pg, inputRadius, displayMode);
		if (radiusValue <= 0)
			continue;

		BeginPath(pg->hDC2);
		if ((int)setting[0] & bitSetArray[2]) {
			double arcBeginX = xx + radiusValue * sin(inputArcBegin);
			double arcBeginY = yy + radiusValue * cos(inputArcBegin);
			double arcEndX = xx + radiusValue * sin(inputArcEnd);
			double arcEndY = yy + radiusValue * cos(inputArcEnd);

			Arc(pg->hDC2, xx - radiusValue, yy - radiusValue, xx + radiusValue, yy + radiusValue,
				arcEndX, arcEndY, arcBeginX, arcBeginY);
		}
		else {
			Ellipse(pg->hDC2, xx - radiusValue, yy - radiusValue, xx + radiusValue, yy + radiusValue);
		}
		EndPath(pg->hDC2);

		if (enableFill || inputLineWidth) {
			HBRUSH fillColor = NULL;
			HPEN hPen = NULL;

			if (enableFill) {
				fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));// inputAlpha);
				SelectObject(pg->hDC2, fillColor);
			}
			if (inputLineWidth) {
				int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				hPen = CreatePen(dashMode, lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
				SelectObject(pg->hDC2, hPen);
			}
			if (enableFill && inputLineWidth)
				StrokeAndFillPath(pg->hDC2);
			else if (enableFill)
				FillPath(pg->hDC2);
			else if (inputLineWidth)
				StrokePath(pg->hDC2);
		
			double a15 = 15 * radiusValue / 50;
			double a05 = 5 * radiusValue / 50;

			if(inputLineWidth && (inputArrowSize > 0)) { // draw arrowhead at start of arc
				a15 *= inputArrowSize;
				a05 *= inputArrowSize;

				double ang = inputArcBegin;
				double sinAng = sin(ang);
				double cosAng = cos(ang);
				int tipX = xx + radiusValue * sinAng;
				int tipY = yy + radiusValue * cosAng;

				int tipXa = tipX - a15 * cosAng - a05 * sinAng;
				int tipXb = tipX - a15 * cosAng + a05 * sinAng;
				int tipYa = tipY - a05 * cosAng + a15 * sinAng;
				int tipYb = tipY + a05 * cosAng + a15 * sinAng;

				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, tipXa, tipYa, NULL);
				LineTo(pg->hDC2, tipX, tipY);
				LineTo(pg->hDC2, tipXb, tipYb);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
			if (inputLineWidth && (inputArrowSize < 0)) { // draw arrowhead at end of arc
				a15 *= -inputArrowSize;
				a05 *= -inputArrowSize;

				double ang = inputArcEnd;
				double sinAng = sin(ang);
				double cosAng = cos(ang);
				int tipX = xx + radiusValue * sinAng;
				int tipY = yy + radiusValue * cosAng;

				int tipXa = tipX + a15 * cosAng + a05 * sinAng;
				int tipXb = tipX + a15 * cosAng - a05 * sinAng;
				int tipYa = tipY + a05 * cosAng - a15 * sinAng;
				int tipYb = tipY - a05 * cosAng - a15 * sinAng;

				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, tipXa, tipYa, NULL);
				LineTo(pg->hDC2, tipX, tipY);
				LineTo(pg->hDC2, tipXb, tipYb);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}

			if (enableFill)
				DeleteObject(fillColor);
			if (inputLineWidth)
				DeleteObject(hPen);

			//ctx2.setLineDash([]);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

std::vector<Button*>circleButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDashAndOffset) };

bool Canvas_CIRCLE_Component::onLoad(Sheet* pg) {
	if (setting.size() < 4) {
		setSetting(1, 0); // no dashing
		setSetting(2, 0); // x offset
		setSetting(3, 0); // y offset
	}
	buildPinLabels();
	return false;
}
vector<int> Canvas_CIRCLE_Component::GetPinGroups() { return vector<int>{2, 1, 2, 1, 1, 3, 3, 1 }; }
vector<string> Canvas_CIRCLE_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position",	// Group 0
		"Radius",					// Group 1
		"Arc Start", "Arc End",		// Group 2
		"Line Width",				// Group 3
		"Arrow Size",				// Group 4
		"Line Red", "Line Green", "Line Blue",	// Group 5
		"Fill Red", "Fill Green", "Fill Blue",	// Group 6
		"Alpha"};					// Group 7
}
Canvas_CIRCLE_Component::Canvas_CIRCLE_Component()
	: Canvas_Component(L"CIRCLE", L"2D Circle", squareShape, 4, 1, 0, 0)
{
	dialogButtonList = circleButtonList;
	setSetting(0, 3);
	setSetting(1, 0); // no dashing
	setSetting(2, 0); // x offset
	setSetting(3, 0); // y offset
	pinMaskIndex = 0;
}


// *********************** Lines *******************************

bool Canvas_LINES_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX0 = 0;
		double inputY0 = 0;
		double inputX1 = 0;
		double inputY1 = 0;
		if ((int)setting[0] & 1) {
			inputX0 = getLoopingValueD(pin++, k, 0);
			inputY0 = getLoopingValueD(pin++, k, 0);
			inputX1 = getLoopingValueD(pin++, k, 0);
			inputY1 = getLoopingValueD(pin++, k, 0);
		}

		double inputLineWidth = 1;
		if ((int)setting[0] & 2) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
		}
		if (inputLineWidth <= 0)
			continue;
		int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);

		getColorLineGroup(2, &pin, k);
		
		double inputAlpha = 1.0;
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}
		
		int corner = 0;
		if ((int)setting[0] & 16) {
			corner = getLoopingValueD(pin++, k, 0);
		}
	
		HPEN frameColor = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
		SelectObject(pg->hDC2, frameColor);

		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, xTo2dCanvas2(pg, inputX0, displayMode), yTo2dCanvas2(pg, inputY0, displayMode), NULL);

		if (corner > 0)
			LineTo(pg->hDC2, xTo2dCanvas2(pg, inputX1, displayMode), yTo2dCanvas2(pg, inputY0, displayMode));
		else if (corner < 0)
			LineTo(pg->hDC2, xTo2dCanvas2(pg, inputX0, displayMode), yTo2dCanvas2(pg, inputY1, displayMode));

		LineTo(pg->hDC2, xTo2dCanvas2(pg, inputX1, displayMode), yTo2dCanvas2(pg, inputY1, displayMode));
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);
		
		DeleteObject(frameColor);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

std::vector<Button*>linesButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash) };

bool Canvas_LINES_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}
vector<int> Canvas_LINES_Component::GetPinGroups() { return vector<int>{4, 1, 3, 1}; }
vector<string> Canvas_LINES_Component::GetPinLabels() {
	return vector<string>{
		"X0", "Y0", "X1", "Y1",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Alpha"};
}
Canvas_LINES_Component::Canvas_LINES_Component()
	: Canvas_Component(L"LINES", L"2D Line Segment", squareShape, 5, 1, 0, 0)
{
	dialogButtonList = linesButtonList;
	setSetting(0, 3);
	setSetting(1, 0);
	pinMaskIndex = 0;
}

// *********************** Line *******************************

bool Canvas_LINE_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	if (((int)setting[0] & 1) == 0)
		return false; // need line X's and Y's

	// curve X's and Y's have to be the same size
	int len = input[0].value.size();
	if (len != input[1].value.size())
		return false;
	if (len < 2)
		return false;

	int pen = 0;
	for (int j = 0; j < len; j++) {
		int pin = 2;

		double inputLineWidth = 1;
		if ((int)setting[0] & 2) { // 2=Line Width: [["Line Width"]]
			inputLineWidth = getLoopingValueD(pin++, j, 1);
			if (inputLineWidth <= 0)
				pen = 0;
		}
		int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);

		getColorLineGroup(2, &pin, j);
		
		double alpha = 1.0;
		if ((int)setting[0] & 8) { // 8=Alpha: ["Alpha"],
			alpha = getLoopingValueD(pin++, j, 1);
			if (alpha == 0)
				pen = 0;
		}	

		int corners = 0;
		if ((int)setting[0] & 16)  // 16=Corners: 0-none, + up/right, - down/left
			corners = getLoopingValueD(pin++, j, 0);
		else
			corners = 0;

		BeginPath(pg->hDC2);
		if (pen == 0) {
			MoveToEx(pg->hDC2, 
				xTo2dCanvas2(pg, input[0].value[j], displayMode),
				yTo2dCanvas2(pg, input[1].value[j], displayMode), NULL);
			pen = 1;
		}
		else {
			if (corners > 0) {
				LineTo(pg->hDC2, 
					xTo2dCanvas2(pg, input[0].value[j], displayMode),
					yTo2dCanvas2(pg, input[1].value[j - 1], displayMode));
			}
			else if (corners < 0) {
				LineTo(pg->hDC2, 
					xTo2dCanvas2(pg, input[0].value[j - 1], displayMode),
					yTo2dCanvas2(pg, input[1].value[j], displayMode));
			}
			LineTo(pg->hDC2, 
				xTo2dCanvas2(pg, input[0].value[j], displayMode),
				yTo2dCanvas2(pg, input[1].value[j], displayMode));
		}
		EndPath(pg->hDC2);

		HPEN frameColor = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
		SelectObject(pg->hDC2, frameColor);
		StrokePath(pg->hDC2);
		DeleteObject(frameColor);
	}
	
	//ctx2.setLineDash([]);
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

std::vector<Button*>lineButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash) };

bool Canvas_LINE_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}

vector<int> Canvas_LINE_Component::GetPinGroups() { return vector<int>{2, 1, 3, 1, 1 }; }
vector<string> Canvas_LINE_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Alpha",
		"Corners" };
}
Canvas_LINE_Component::Canvas_LINE_Component()
	: Canvas_Component(L"LINE", L"2D Poly Line", squareShape, 3, 1, 0, 0)
{
	dialogButtonList = lineButtonList;
	setSetting(0, 3);
	setSetting(1, 0); // dash
	pinMaskIndex = 0;
}



// *********************** Cross *******************************

bool Canvas_CROSS_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		getPositionGroup(0, &pin, k);

		double inputSize = 10;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 10);
		}
		if (inputSize <= 0)
			continue;
		int size = sizeTo2dCanvas(pg, inputSize, displayMode);

		double inputLineWidth = 1;
		if ((int)setting[0] & 4) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}
		int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);

		getColorLineGroup(3, &pin, k);

		double inputAlpha = 1.0;
		if ((int)setting[0] & 16) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		int x = xTo2dCanvas2(pg, inputX, displayMode);
		int y = yTo2dCanvas2(pg, inputY, displayMode);

		//ctx2.lineWidth = sizeTo2dCanvas(lineWidth.value, displayMode);
		//HBRUSH frameColor = CreateSolidBrush(RGB(inputLineRed, inputLineGreen, inputLineBlue));//inputAlpha);
		HPEN frameColor = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
		SelectObject(pg->hDC2, frameColor);
		//setDashingContext2(pg, i, 0);

		int scaledSize = sizeTo2dCanvas(pg, inputSize, displayMode);
		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, x, y + scaledSize, NULL);
		LineTo(pg->hDC2, x, y - scaledSize);
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);

		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, x + scaledSize, y, NULL);
		LineTo(pg->hDC2, x - scaledSize, y);
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);

		//ctx2.setLineDash([]);
		DeleteObject(frameColor);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

std::vector<Button*>crossButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash) };

bool Canvas_CROSS_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}

vector<int> Canvas_CROSS_Component::GetPinGroups() { return vector<int>{2, 1, 1, 3, 1  }; }
vector<string> Canvas_CROSS_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Size",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Alpha"};
}
Canvas_CROSS_Component::Canvas_CROSS_Component()
	: Canvas_Component(L"CROSS", L"2D Cross", squareShape, 2, 1, 0, 0)
{
	dialogButtonList = crossButtonList;
	setSetting(0, 1);
	setSetting(1, 0); // dash
	pinMaskIndex = 0;
}


// *********************** Marker *******************************

bool Canvas_MARKER_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		getPositionGroup(0, &pin, k);

		double inputSize = 10;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 10);
		}
		if (inputSize <= 0)
			continue;
		int scaledSize = sizeTo2dCanvas(pg, inputSize, displayMode);

		double inputLineWidth = 1;
		if ((int)setting[0] & 4) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}
		int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);

		getColorLineGroup(3, &pin, k);

		double inputAlpha = 1.0;
		if ((int)setting[0] & 16) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}
		
		int x = xTo2dCanvas2(pg, inputX, displayMode);
		int y = yTo2dCanvas2(pg, inputY, displayMode);

		//ctx2.lineWidth = sizeTo2dCanvas(lineWidth.value, displayMode);
		//HBRUSH frameColor = CreateSolidBrush(RGB(inputLineRed, inputLineGreen, inputLineBlue));//inputAlpha);
		HPEN frameColor = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
		SelectObject(pg->hDC2, frameColor);
		//setDashingContext2(pg, i, 0);

		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, x + scaledSize, y + scaledSize, NULL);
		LineTo(pg->hDC2, x - scaledSize, y - scaledSize);
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);

		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, x + scaledSize, y - scaledSize, NULL);
		LineTo(pg->hDC2, x - scaledSize, y + scaledSize);
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);

		//ctx2.setLineDash([]);
		DeleteObject(frameColor);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}
bool Canvas_MARKER_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}
vector<int> Canvas_MARKER_Component::GetPinGroups() { return vector<int>{2, 1, 1, 3, 1 }; }
vector<string> Canvas_MARKER_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Size",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Alpha" };
}
Canvas_MARKER_Component::Canvas_MARKER_Component()
	: Canvas_Component(L"MARKER", L"2D Marker", squareShape, 2, 1, 0, 0)
{
	dialogButtonList = crossButtonList; // shares
	setSetting(0, 1);
	setSetting(1, 0); // dash
	pinMaskIndex = 0;
}

// *********************** Arrow *******************************


bool Canvas_ARROW_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	int dashMode = setting[1];

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX0 = 0;
		double inputY0 = 0;
		double inputX1 = 0;
		double inputY1 = 0;
		if ((int)setting[0] & 1) { // "X0", "Y0", "X1", "Y1"
			inputX0 = getLoopingValueD(pin++, k, 0);
			inputY0 = getLoopingValueD(pin++, k, 0);
			inputX1 = getLoopingValueD(pin++, k, 0);
			inputY1 = getLoopingValueD(pin++, k, 0);
		}
		if ((inputX0 == inputX1) && (inputY0 == inputY1))
			continue;

		double inputLineWidth = 1;
		if ((int)setting[0] & 2) { // "Line Width"
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}
		int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);

		int inputArrowMode = 1;
		double	inputArrowSize = 15;
		if ((int)setting[0] & 4) { // "Arrow Heads", "Arrow Size"
			inputArrowMode = getLoopingValueD(pin++, k, 1);
			inputArrowSize = getLoopingValueD(pin++, k, 15);
		}
		int head = floor(inputArrowMode);

		int savepin = pin;
		bool enableDimensions = false;
		double headLeftDim = 15;
		double headRightDim = 15;
		double tailLeftDim = 15;
		double tailRightDim = 15;
		if ((int)setting[0] & 8) { // "Head Left Dim.", "Head Right Dim.", "Tail Left Dim.", "Tail Right Dim."
			enableDimensions = true;
			headLeftDim  = getLoopingValueD(pin++, k, 15);
			headRightDim = getLoopingValueD(pin++, k, 15);
			tailLeftDim  = getLoopingValueD(pin++, k, 15);
			tailRightDim = getLoopingValueD(pin++, k, 15);
		}

		bool enableValue = false;
		bool isWide = false;
		string narrowNumericalText;
		wstring wideStringText;
		double valuePosition = 0; // center
		if ((int)setting[0] & 16) { // "Value" - can be string or value, "Value Position"
			if (input[pin].stringType) {
				string tempString = getLoopingStringValueD(pin++, k, "");
				wideStringText = decodeUTF8(tempString);
				isWide = true;
			}
			else {
				narrowNumericalText = tomString(reasonableValue(getLoopingValueD(pin++, k, 0)));
			}
			valuePosition = getLoopingValueD(pin++, k, 0); // 0 is center, 1 is head, -1 is tail
			enableValue = true;
		}

		double valueXoffset = 0;
		double valueYoffset = 0;
		if ((int)setting[0] & 32) { // "Value X Offset", "Value Y Offset"
			valueXoffset = getLoopingValueD(pin++, k, 0);
			valueYoffset = getLoopingValueD(pin++, k, 0);
		}

		int fontSize = DES_VALUE_TEXT_SIZE;
		if ((int)setting[0] & 64) { // "Font Size"
			fontSize = getLoopingValueD(pin++, k, 10);
			if (fontSize <= 0)
				enableValue = false; // kill it
		}
		int stringWidth;
		if (enableValue) {
			if (isWide) {
				stringWidth = calcWideStringWidthFontSize(pg->hDC2, fontSize, wideStringText);
				if (stringWidth == 0)
					stringWidth = calcWideStringWidthFontSize(pg->hDC, fontSize, wideStringText);
				if (stringWidth == 0)
					stringWidth = calcWideStringWidthFontSize(magic_hDC, fontSize, wideStringText);
			}
			else {
				stringWidth = calcStringWidthFontSize(pg->hDC2, fontSize, narrowNumericalText);
				if (stringWidth == 0)
					stringWidth = calcStringWidthFontSize(pg->hDC, fontSize, narrowNumericalText);
				if (stringWidth == 0)
					stringWidth = calcStringWidthFontSize(magic_hDC, fontSize, narrowNumericalText);
			}
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int	inputLineBlue = 0;
		if ((int)setting[0] & 128) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputAlpha = 1.0;
		if ((int)setting[0] & 256) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		COLORREF theColor = RGB(inputLineRed, inputLineGreen, inputLineBlue);

		double x0 = xTo2dCanvas2(pg, inputX0, displayMode);
		double y0 = yTo2dCanvas2(pg, inputY0, displayMode);
		double x1 = xTo2dCanvas2(pg, inputX1, displayMode);
		double y1 = yTo2dCanvas2(pg, inputY1, displayMode);

		
		HPEN frameColor = CreatePen(dashMode, lineWidth, theColor);//inputAlpha);
		SelectObject(pg->hDC2, frameColor);

		// first draw the arrow's line
		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, x0, y0, NULL);
		LineTo(pg->hDC2, x1, y1);
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);

		double ax;
		double bx;
		double ay;
		double by;
		double amx;
		double bmx;
		double amy;
		double bmy;
		double deltaX;
		double deltaY;
		double fullLength;
		double dxol;
		double dyol;

		deltaX = abs(x0 - x1);
		deltaY = abs(y0 - y1);
		fullLength = sqrt(deltaX * deltaX + deltaY * deltaY);
		fullLength = canvasToSize(pg, fullLength, displayMode);
		dxol = (x0 - x1) / fullLength;
		dyol = (y0 - y1) / fullLength;

		// if head or tail setup
		if (head) {

			HBRUSH fillColor = NULL;
			if (head & 4) { // if we are going to fill it
				fillColor = CreateSolidBrush(theColor);
				SelectObject(pg->hDC2, fillColor);
			}

			double arrowHead15 = inputArrowSize;
			double arrowHead7 = 2 * inputArrowSize / 3;
			double arrowHead5 = inputArrowSize / 3;
			double arrowHead2 = inputArrowSize / 6;

			if (head & 1) { // draw arrow head
				ax = x1 + arrowHead15 * dxol + arrowHead5 * dyol;
				bx = x1 + arrowHead15 * dxol - arrowHead5 * dyol;
				ay = y1 - arrowHead5 * dxol + arrowHead15 * dyol;
				by = y1 + arrowHead5 * dxol + arrowHead15 * dyol;
				if (head & 8) { // close it
					amx = x1 + arrowHead7 * dxol + arrowHead2 * dyol;
					bmx = x1 + arrowHead7 * dxol - arrowHead2 * dyol;
					amy = y1 - arrowHead2 * dxol + arrowHead7 * dyol;
					bmy = y1 + arrowHead2 * dxol + arrowHead7 * dyol;
				}
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, ax, ay, NULL);
				if (head & 8)
					LineTo(pg->hDC2, amx, amy);
				LineTo(pg->hDC2, x1, y1);
				if (head & 8)
					LineTo(pg->hDC2, bmx, bmy);
				LineTo(pg->hDC2, bx, by);
				EndPath(pg->hDC2);
				if (head & 4) // fill it
					FillPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
			if (head & 2) { // draw arrow's tail 
				ax = x0 - arrowHead15 * dxol - arrowHead5 * dyol;
				bx = x0 - arrowHead15 * dxol + arrowHead5 * dyol;
				ay = y0 + arrowHead5 * dxol - arrowHead15 * dyol;
				by = y0 - arrowHead5 * dxol - arrowHead15 * dyol;
				if (head & 8) {
					amx = x0 - arrowHead7 * dxol - arrowHead2 * dyol;
					bmx = x0 - arrowHead7 * dxol + arrowHead2 * dyol;
					amy = y0 + arrowHead2 * dxol - arrowHead7 * dyol;
					bmy = y0 - arrowHead2 * dxol - arrowHead7 * dyol;
				}
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, ax, ay, NULL);
				if (head & 8)
					LineTo(pg->hDC2, amx, amy);
				LineTo(pg->hDC2, x0, y0);
				if (head & 8)
					LineTo(pg->hDC2, bmx, bmy);
				LineTo(pg->hDC2, bx, by);
				EndPath(pg->hDC2);
				if (head & 4) // fill it
					FillPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
			if (head & 4)
				DeleteObject(fillColor); 
		}

		// then draw the tail dimension lines
		if (enableDimensions && (tailLeftDim || tailRightDim)) { // tail dimension
			ax = x0 + tailRightDim * dyol;
			bx = x0 - tailLeftDim * dyol;
			ay = y0 - tailRightDim * dxol;
			by = y0 + tailLeftDim * dxol;
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, ax, ay, NULL);
			LineTo(pg->hDC2, bx, by);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		// then draw the head dimension lines
		if (enableDimensions && (headLeftDim || headRightDim)) { // head dimension
			ax = x1 + headRightDim * dyol;
			bx = x1 - headLeftDim * dyol;
			ay = y1 - headRightDim * dxol;
			by = y1 + headLeftDim * dxol;
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, ax, ay, NULL);
			LineTo(pg->hDC2, bx, by);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		DeleteObject(frameColor);
		
		// next the background rectangle and value string
		double dx;
		double dy;
		double arrowLength;

		if (enableValue) {
			double valX = (x0 + x1) / 2;
			double valY = (y0 + y1) / 2;
			dx = x1 - x0;
			dy = y1 - y0;
			arrowLength = sqrt(dx * dx + dy * dy);

			valX -= stringWidth / 2;
			valY -= fontSize / 2;

			// first apply the valuePosition along the arrow
			valX += valuePosition * abs(dx) / 2;
			valY += valuePosition * abs(dy) / 2;

			// then the offsets
			valX += sizeTo2dCanvas(pg, valueXoffset, displayMode);
			valY -= sizeTo2dCanvas(pg, valueYoffset, displayMode);

			// now we know enough to clear the background
			BeginPath(pg->hDC2);
			int localfontSize = fontSize;
			int frameOffsetTop = 0;
			int frameOffsetBottom = 0;
			if (localfontSize < MINIMUM_FONT_SIZE)
				localfontSize = MINIMUM_FONT_SIZE;
			if (localfontSize > MAXIMUM_FONT_SIZE)
				localfontSize = MAXIMUM_FONT_SIZE;
			if (localfontSize > 35)
				frameOffsetTop = localfontSize / 15;
			if (localfontSize > 25)
				frameOffsetBottom = localfontSize / 15;
			Rectangle(pg->hDC2, valX-1, valY+2+2*frameOffsetTop, 
				valX + stringWidth+1, valY + localfontSize +2 + frameOffsetBottom);
			EndPath(pg->hDC2);
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			FillPath(pg->hDC2);


			if (literal.size()) { // we have a specific font
				if (!isWide) // its a number
					drawTextString(pg->hDC2, valX, valY, fontSize, theColor, literal[0], narrowNumericalText);
				else
					drawWideTextString(pg->hDC2, valX, valY, fontSize, theColor, literal[0], wideStringText);
			}
			else { // use the standard vernanda font
				if (!isWide) // its a number
					drawTextString(pg->hDC2, valX, valY, fontSize, theColor, narrowNumericalText);
				else
					drawWideTextString(pg->hDC2, valX, valY, fontSize, theColor, wideStringText);
			}
		}
		
		
		
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}
void doubleClickArrowFont(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildValueFontDialogButtons(pg, i);
}
std::vector<Button*>arrowButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash),
	new Button(3, 80, 85, 20, L"Font...", doubleClickArrowFont) };

bool Canvas_ARROW_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}
vector<int> Canvas_ARROW_Component::GetPinGroups() { return vector<int>{4, 1, 2, 4, 2, 2, 1, 3, 1 }; }
vector<string> Canvas_ARROW_Component::GetPinLabels() {
	return vector<string>{
		"X0", "Y0", "X1", "Y1",
		"Line Width",
		"Arrow Heads", "Arrow Size",
		"Head Left Dim.", "Head Right Dim.", "Tail Left Dim.", "Tail Right Dim.",
		"Value", "Value Position", 
		"Value X Offset", "Value Y Offset",
		"Font Size",
		"Line Red", "Line Green", "Line Blue",
		"Alpha"};
}
Canvas_ARROW_Component::Canvas_ARROW_Component()
	: Canvas_Component(L"ARROW", L"2D Arrow", squareShape, 5, 1, 0, 0)
{
	dialogButtonList = arrowButtonList;
	width = 40;
	setSetting(0, 3);
	setSetting(1, 0); // dash
	pinMaskIndex = 0;
}


// ********************** Generic 2D Slider Options ***************************


void button2dSlider_Completion(Sheet* pg, int i, double *doubleValues, bool *boolValues, int radioValue) {

	pg->dgrm.cL[i]->setting[0] = doubleValues[0]; // Minimum
	pg->dgrm.cL[i]->setting[1] = doubleValues[1]; // Maximum
	pg->dgrm.cL[i]->setting[3] = doubleValues[2]; // Detent

	// be sure the values are consistent with the limits and detent
	int knobs = pg->dgrm.cL[i]->setting.size() - 4;
	for (int j = 0; j < knobs; j++) {
		if (pg->dgrm.cL[i]->setting[j + 4] < pg->dgrm.cL[i]->setting[0])
			pg->dgrm.cL[i]->setting[j + 4] = pg->dgrm.cL[i]->setting[0];
		if (pg->dgrm.cL[i]->setting[j + 4] > pg->dgrm.cL[i]->setting[1])
			pg->dgrm.cL[i]->setting[j + 4] = pg->dgrm.cL[i]->setting[1];
	}


	pg->dgrm.cL[i]->setting[2] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0) +
		(boolValues[2] ? 4 : 0) + (boolValues[3] ? 8 : 0) + (boolValues[4] ? 16 : 0);
	pg->dgrm.cL[i]->displayMode = boolValues[5];

	//pg->dgrm.cL[i]->onLoad(pg);
}
void doubleClick2dSlider(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Minimum:",					"Maximum:",					"Detent:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels = { "Draw the Track", "Draw the Knob", "Draw the Number", "Draw the Graph", "Spring Return", "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[2] & 1),
		(bool)((int)pg->dgrm.cL[i]->setting[2] & 2),
		(bool)((int)pg->dgrm.cL[i]->setting[2] & 4),
		(bool)((int)pg->dgrm.cL[i]->setting[2] & 8),
		(bool)((int)pg->dgrm.cL[i]->setting[2] & 16),
		(bool)(pg->dgrm.cL[i]->displayMode)};

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		button2dSlider_Completion);
}
std::vector<Button*>the2dSliderButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dSlider) };





// ************************ 2D Vertical Slider **********************************


bool Canvas_VerticalSlider_Component::dial2d(Sheet* pg) // can handle multiple inputs on X
{
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	int oldX;
	int oldY;
	for (int k = 0; k < loops; k++) {
		int inputX = getLoopingValueD(0, k, 0);
		int inputY = getLoopingValueD(1, k, 0);
		int inputH = getLoopingValueD(2, k, 100);
		int inputR = getLoopingValueD(3, k, DES_KNOB_SIZE);

		int lineColor;
		int blackColor;
		int whiteColor;
		//if (color) {
		//	int inputRed = getLoopingValueD(4, k, 0);
		//	int inputGreen = getLoopingValueD(5, k, 0);
		//	int inputBlue = getLoopingValueD(6, k, 0);
		//	int inputAlpha = getLoopingValueD(7, k, 1);
		//	int lineColor = redGreenBlueAlpha(inputRed, inputGreen, inputBlue, inputAlpha);
		//	int blackColor = blackAlpha(inputAlpha);
		//	int whiteColor = whiteAlpha(inputAlpha);
		//}
		//else {
		lineColor = 0x000000;
		blackColor = 0x000000;
		whiteColor = 0xFFFFFF;
		//}

		if (inputH <= 0)
			continue;
		if (inputR <= 0)
			continue;

		inputH = sizeTo2dCanvas(pg, inputH, displayMode);
		inputR = sizeTo2dCanvas(pg, inputR, displayMode);

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		int top = yCenter - inputH / 2;
		int bot = yCenter + inputH / 2;
		int knobY = (detent(3, 4 + k) - setting[0]) /
			(setting[1] - setting[0]) *
			(top - bot) + bot;

		//ctx2.lineWidth = 1;
		if ((int)setting[2] & 1) // line
		{
			// the track
			//ctx2.strokeStyle = lineColor;
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, xCenter, top, NULL);
			LineTo(pg->hDC2, xCenter, bot);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		if ((int)setting[2] & 2) // circle
		{
			// then the knob
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			//ctx2.strokeStyle = lineColor;
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, xCenter - inputR, knobY - inputR, xCenter + inputR, knobY + inputR);
			//ctx2.fillStyle = whiteColor;
			EndPath(pg->hDC2);
			StrokeAndFillPath(pg->hDC2);
		}

		if (k > 0)
			if ((int)setting[2] & 8) // graph
			{
				// the graph
				//ctx2.strokeStyle = lineColor;
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, oldX, oldY, NULL);
				LineTo(pg->hDC2, xCenter, knobY);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
		oldX = xCenter;
		oldY = knobY;

		//ctx2.fillStyle = blackColor;
		//ctx2.font = '9px ' + default_2D_FontFamily;
		// if there is a user label put below
		//if (userLabel) {
		//	int width = ctx2.measureText(userLabel).width;
		//	int offset = inputR + 9 + 1; // 9 is font size
		//	ctx2.fillText(userLabel, xCenter - width / 2, yCenter + inputH / 2 + offset);
		//}
		if ((int)setting[2] & 4) //number
		{
			// then the value at the top
			int offset = inputR + 12 + 3;
			std::string text = tomString(detent(3, 4 + k));
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
			drawLargeTextString(pg->hDC2, xCenter - textWidth /2, yCenter - inputH / 2 - offset, 0, text);
		}

		// the Halo
		//if ((int)memory[0] == k) {
		//	BeginPath(pg->hDC2);
		//	Ellipse(pg->hDC, xCenter - DES_HALO_SIZE, knobY - DES_HALO_SIZE, xCenter + DES_HALO_SIZE, knobY + DES_HALO_SIZE);
		//	EndPath(pg->hDC2);
		//	//ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	FillPath(pg->hDC2);
		//}

		// the touch area - DIAGNOSTIC
		if (DES_TOUCH_ALPHA) {
			int touchSize = max(DES_TOUCH_SIZE, inputR);
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, xCenter - touchSize, knobY - touchSize, xCenter + touchSize, knobY + touchSize);
			EndPath(pg->hDC2);
			FillPath(pg->hDC2);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}



int Canvas_VerticalSlider_Component::method2(Sheet* pg) {
	
	// this is the slide constant - send setting out
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	
	int oldSize = setting.size();
	setting.resize(4 + loops);
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		if (oldSize <= 4 + k)
			setting[4 + k] = setting[4 + k - 1]; // K NEVER ZERO
		output[0].value[k] = detent(3, 4 + k); //setting[4 + k];
	}

	output[0].dimensions[0] = loops;

	return 3;
}

bool Canvas_VerticalSlider_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	if (setting.size() > 4 + loops)
		setting.resize( 4 + loops);

	int inputX;
	int inputY;
	int inputH;
	int inputR;
	int xCenter;
	int yCenter;
	int top;
	int bot;
	int touchSize;

	int k;
	if (check) {
		for (k = loops - 1; k >= 0; k--) { // scan backwards
			// convert to pixels
			inputX = getLoopingValueD(0, k, 0);
			inputY = getLoopingValueD(1, k, 0);
			inputH = getLoopingValueD(2, k, 100);
			inputR = getLoopingValueD(3, k, DES_KNOB_SIZE );

			if (inputH <= 0)
				continue;
			if (inputR <= 0)
				continue;

			// alpha
			if (type == L"2dClrSldVZ")
				if (getLoopingValueD(7, k, 1) <= 0)
					continue;

			inputH = sizeTo2dCanvas(pg, inputH, displayMode);
			inputR = sizeTo2dCanvas(pg, inputR, displayMode);
			touchSize = max(DES_TOUCH_SIZE, inputR);

			xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			yCenter = yTo2dCanvas2(pg, inputY, displayMode);
			top = yCenter - inputH / 2;
			bot = yCenter + inputH / 2;

			double yScale = (top - bot) / (setting[1] - setting[0]);

			//  first find the current dial position
			double knobY = (detent(3, 4 + k) - setting[0]) * yScale + bot;

			if (mouseX > xCenter + touchSize)
				continue; // missed dial
			if (mouseX < xCenter - touchSize)
				continue; // missed dial

			if (mouseY > knobY + touchSize)
				continue; // missed dial
			if (mouseY < knobY - touchSize)
				continue; // missed dial

			memory[0] = k;
			return true;
		}
		return false;
	}

	k = memory[0];

	inputY = getLoopingValueD(1, k, 0);
	inputH = getLoopingValueD(2, k, 100);
	if (inputH <= 0)
		return false;

	yCenter = yTo2dCanvas2(pg, inputY, displayMode);
	inputH = sizeTo2dCanvas(pg, inputH, displayMode);

	top = yCenter - inputH / 2;
	bot = yCenter + inputH / 2;
	double yScale = (top - bot) / (setting[1] - setting[0]);

	if (mouseY > bot)
		mouseY = bot;

	if (mouseY < top)
		mouseY = top;

	// knob position to setting
	// setting[4 + k] = overScale(mouseY - bot, yScale) + setting[0];
	setting[4 + k] = reasonableValue(((mouseY - bot) / yScale) + setting[0]);
	
	method2(pg); // push the output
	return true;
}

Canvas_VerticalSlider_Component::~Canvas_VerticalSlider_Component() {
	drawCanvas2();// doesnt work
}

Canvas_VerticalSlider_Component::Canvas_VerticalSlider_Component()
	: Component(L"2dSldVZ", L"2D Vertical Slider", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = the2dSliderButtonList;
	setLeftLabels(3, "X", "Y", "Height", "Radius");
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 7);      // option code: 1 draw track, 2 draw knob, 4 draw number, 8 graph, 16 spring back
	setSetting(3, 0);      // detent
	setSetting(4, 0);      // output 4 and up
	setMemory(0, -1);
	height = pinToHeight(4);
	//getInternals = getInternals_4up;
	//setInternals = setInternals_4up_1out;
}

// ************************ 2D Horizontal Slider **********************************

int Canvas_HorizontalSlider_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);

	// this is the slide constant - send setting out
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	
	int oldSize = setting.size();
	setting.resize(4 + loops);
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		if (oldSize <= 4 + k)
			setting[4 + k] = setting[4 + k - 1]; // K NEVER ZERO
		output[0].value[k] = detent(3, 4 + k); //setting[4 + k];
	}
	output[0].dimensions[0] = loops;

	return 3;
}

bool Canvas_HorizontalSlider_Component::dial2d(Sheet* pg) // Horizontal version, multiple inputs on Y
{
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	int oldX = 0;
	int oldY = 0;
	for (int k = 0; k < loops; k++) {
		int inputX = getLoopingValueD(0, k, 0);
		int inputY = getLoopingValueD(1, k, 0);
		int inputH = getLoopingValueD(2, k, 100);
		int inputR = getLoopingValueD(3, k, DES_KNOB_SIZE);
		
		//int inputRed = getLoopingValueD(4, k, 0);
		//int inputGreen = getLoopingValueD(5, k, 0);
		//int inputBlue = getLoopingValueD(6, k, 0);
		//int inputAlpha = getLoopingValueD(7, k, 1);
		//int lineColor = redGreenBlueAlpha(inputRed, inputGreen, inputBlue, inputAlpha);
		//int blackColor = blackAlpha(inputAlpha);
		//int whiteColor = whiteAlpha(inputAlpha);
		//}
		//else {
		int lineColor = 0x000000;
		int blackColor = 0x000000;
		int whiteColor = 0xFFFFFF;
		//}

		if (inputH <= 0)
			continue;
		if (inputR <= 0)
			continue;

		inputH = sizeTo2dCanvas(pg, inputH, displayMode);
		inputR = sizeTo2dCanvas(pg, inputR, displayMode);

		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);
		int top = xCenter + inputH / 2;
		int bot = xCenter - inputH / 2;
		int knobX = (detent(3, 4 + k) - setting[0]) /
			(setting[1] - setting[0]) *
			(top - bot) + bot;

		//ctx2.lineWidth = 1;
		if ((int)setting[2] & 1) // line
		{
			// the track
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, top, yCenter, NULL);
			LineTo(pg->hDC2, bot, yCenter);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		if ((int)setting[2] & 2) // circle
		{
			// then the knob
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			//ctx2.strokeStyle = lineColor;
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, knobX - inputR, yCenter - inputR, knobX + inputR, yCenter + inputR);
			//ctx2.fillStyle = whiteColor;  // white
			EndPath(pg->hDC2);
			StrokeAndFillPath(pg->hDC2);
		}

		if (k > 0)
			if ((int)setting[2] & 8) // graph
			{
				// the graph
				//ctx2.strokeStyle = lineColor;
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, oldX, oldY, NULL);
				LineTo(pg->hDC2, knobX, yCenter);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
		oldX = knobX;
		oldY = yCenter;

		//ctx2.fillStyle = blackColor;
		//ctx2.font = '9px ' + default_2D_FontFamily;
		//// if there is a user label put below
		//if (userLabel) {
		//	int width = ctx2.measureText(userLabel).width;
		//	int offset = inputR + 9 + 1;// 9 is font size
		//	ctx2.fillText(userLabel, xCenter - width / 2, yCenter + offset);
		//}
		if ((int)setting[2] & 4) //number
		{
			// then the value at the top
			int offset = inputR + 12 + 3;// 12 is font size
			std::string text = tomString(detent(3, 4 + k));
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
			drawLargeTextString(pg->hDC2, xCenter - textWidth /2, yCenter - offset, 0, text);
		}

		// the Halo
		//if (memory[0] == k) {
		//	ctx2.beginPath();
		//	Ellipse(pg->hDC, knobX - DES_HALO_SIZE, yCenter - DES_HALO_SIZE, knobX + DES_HALO_SIZE, yCenter + DES_HALO_SIZE);
		//	ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	FillPath(pg->hDC2);
		//}
		// the touch area - DIAGNOSTIC
		if (DES_TOUCH_ALPHA) {
			int touchSize = max(DES_TOUCH_SIZE, inputR);
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, knobX - touchSize, yCenter - touchSize, knobX + touchSize, yCenter + touchSize);
			//ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
			EndPath(pg->hDC2);
			FillPath(pg->hDC2);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}


bool Canvas_HorizontalSlider_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) { // Horizontal version
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	if (setting.size() > 4 + loops)
		setting.resize( 4 + loops);

	int inputX;
	int inputY;
	int inputH;
	int inputR;
	int xCenter;
	int yCenter;
	int top;
	int bot;
	int touchSize;
	int k;

	if (check) {
		for (k = loops - 1; k >= 0; k--) { // scan backwards
			// convert to pixels
			inputX = getLoopingValueD(0, k, 0);
			inputY = getLoopingValueD(1, k, 0);
			inputH = getLoopingValueD(2, k, 100);
			inputR = getLoopingValueD(3, k, DES_KNOB_SIZE);

			if (inputH <= 0)
				continue;
			if (inputR <= 0)
				continue;

			// alpha
			if (type == L"2dClrSldHZ")
				if (getLoopingValueD(7, k, 1) <= 0)
					continue;

			inputH = sizeTo2dCanvas(pg, inputH, displayMode);
			inputR = sizeTo2dCanvas(pg, inputR, displayMode);
			touchSize = max(DES_TOUCH_SIZE, inputR);

			xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			top = xCenter + inputH / 2;
			bot = xCenter - inputH / 2;
			double xScale = (top - bot) / (setting[1] - setting[0]);

			//  first find the current dial position
			double knobX = (detent(3, 4 + k) - setting[0]) * xScale + bot;

			if (mouseY > yCenter + touchSize)
				continue; // missed dial
			if (mouseY < yCenter - touchSize)
				continue; // missed dial

			if (mouseX > knobX + touchSize)
				continue; // missed knob
			if (mouseX < knobX - touchSize)
				continue; // missed knob

			memory[0] = k;
			return true;
		}
		return false;
	}

	k = memory[0];

	inputX = getLoopingValueD(0, k, 0);
	inputH = getLoopingValueD(2, k, 100);
	if (inputH <= 0)
		return false;
	xCenter = xTo2dCanvas2(pg, inputX, displayMode);
	inputH = sizeTo2dCanvas(pg, inputH, displayMode);

	top = xCenter + inputH / 2;
	bot = xCenter - inputH / 2;
	double xScale = (top - bot) / (setting[1] - setting[0]);

	if (mouseX < bot)
		mouseX = bot;

	if (mouseX > top)
		mouseX = top;

	// knob position to setting
	//setting[4 + k] = overScale(mouseX - bot, xScale) + setting[0];
	setting[4 + k] = reasonableValue(((mouseX - bot) / xScale) + setting[0]);

	method2(pg);// push the output
	return true;
}

Canvas_HorizontalSlider_Component::Canvas_HorizontalSlider_Component()
	: Component(L"2dSldHZ", L"2D Horizontal Slider", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = the2dSliderButtonList;
	setLeftLabels(3, "X", "Y", "Width", "Radius");
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 7);      // option code: 1 draw track, 2 draw knob, 4 draw number, 8 graph, 16 spring back
	setSetting(3, 0);      // detent
	setSetting(4, 0);     // output 4 and up
	setMemory(0, -1);
	//getInternals = getInternals_4up;
	//setInternals = setInternals_4up_1out;
}


// ************************ 2D Checkbox and Radio Group **********************************


void draw2dText(Sheet* pg, int left, int right, int top, int bottom, wstring inText, int textSize) {

	//int maxLineWidth = right - left;
	//int firstWordIndex = 0;
	int lineX = left;
	int lineY = top + 1;
	int spaceWidth = calcStringWidthFontSize(pg->hDC2, textSize, " ");
	int startOfWord = 0;
	int endOfWord;
	wstring currentText;
	wstring newWord;
	int currentTextWidth = 0;
	int newWordWidth;
	int maxLineWidth = right - left - 2 *spaceWidth;

	//BeginPath(pg->hDC2);
	//Rectangle(pg->hDC2, left, top, right, bottom);
	//EndPath(pg->hDC2);
	//StrokePath(pg->hDC2);

	do {	
		// get another word
		endOfWord = inText.find_first_of(' ', startOfWord);
		if (endOfWord == -1) { // its that last word
			endOfWord = inText.size();
		}

		// test adding another word
		newWord = inText.substr(startOfWord, endOfWord - startOfWord);
		newWordWidth = calcWideStringWidthFontSize(pg->hDC2, textSize, newWord);
		if (currentTextWidth + newWordWidth > maxLineWidth) { // can't fit - print current line now
			drawWideTextString(pg->hDC2, lineX + 3, lineY, textSize, 0, currentText);
			currentText = newWord;
			currentTextWidth = newWordWidth;
			
			// advance to the next line
			lineY += 1.1 * textSize; // leave a little room for super and subscripts
			int g = 1.2 * textSize - 15;
			if (lineY + g > bottom)
				break;
		}
		else { // still room - add new word to line
			if (currentText.size()) {
				currentText += L" ";
				currentTextWidth += spaceWidth;
			}
			currentText += newWord;
			currentTextWidth += newWordWidth;
		}
		startOfWord = endOfWord + 1;
	}
	while (startOfWord < inText.size());

	int g = 1.2 * textSize - 15;
	if (lineY < bottom - textSize / 4) {
		if (currentText.size()) { // push out the last line
			drawWideTextString(pg->hDC2, lineX + 3, lineY, textSize, 0, currentText);
			currentText.clear();
			currentTextWidth = 0;
		}
	}
		
}



std::vector<Button*>the2dRadioCheckButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayMode) };

int Canvas_RadioButton_Component::method2(Sheet* pg) {
	// this is the radio group  - send setting out
	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = -1;
		output[0].dimensions.resize(1);
		output[1].dimensions.resize(1);
		return true;
	}

	output[0].value[0] = setting[1];
	output[1].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		if (k == setting[1])
			output[1].value[k] = 1;
		else
			output[1].value[k] = 0;
	}
	output[1].dimensions[0] = loops;
	
	return 3;
}

//int leftDiagnostic;
//int rightDiagnostic;
//int topDiagnostic;
//int bottomDiagnostic;
//void drawDiagnostic(HDC hdc) {
//
//	// diagnostic rectangle
//	BeginPath(hdc);
//	Rectangle(hdc, leftDiagnostic, topDiagnostic, rightDiagnostic, bottomDiagnostic);
//	EndPath(hdc);
//	HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(255, 0, 0));
//	SelectObject(hdc, hPen);
//	StrokePath(hdc);
//}
bool Canvas_RadioButton_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	int pin = 0;
	bool autoSpreadMode = true;
	if ((int)setting[0] & 1) {
		if (input[pin++].value.size() > 1)
			autoSpreadMode = false;
		if (input[pin++].value.size() > 1)
			autoSpreadMode = false;
	}
	if ((int)setting[0] & 2)
		if (input[pin++].value.size() > 1)
			autoSpreadMode = false;

	for (int k = 0; k < loops; k++) {

		pin = 0;
		double inputX = getLoopingValueD(0, &pin, k, 0);
		double inputY = getLoopingValueD(0, &pin, k, 0);

		int symbolSize = getLoopingValueD(1, &pin, k, DES_CHECK_SYMBOL_SIZE);
		if (symbolSize <= 0)
			continue;


		double hOffset = getLoopingValueD(2, &pin, k, 0);
		double vOffset = getLoopingValueD(2, &pin, k, 0);

		if ((int)setting[0] & 8) { // preset 3
			pin += 2;
		}
		int inputRed = getLoopingValueColor(4, &pin, k);
		int inputGreen = getLoopingValueColor(4, &pin, k);
		int inputBlue = getLoopingValueColor(4, &pin, k);

		double inputAlpha = getLoopingValueD(5, &pin, k, 1);
		if (inputAlpha <= 0)
			continue;

		int textSize = getLoopingValueD(6, &pin, k, DES_CHECK_TEXT_SIZE);

		int textLines = getLoopingValueD(7, &pin, k, 1);
		int textMaxWidth = getLoopingValueD(7, &pin, k, 1000);
		if (textLines > 1)
			textLines += 0.28;

		std::wstring theLabel;
		if ((int)setting[0] & bitSetArray[8]) { // the labels 8
			if (input[pin].stringType)
				theLabel = decodeUTF8(getLoopingStringValueD(pin, k, "UNDEFINED"));
			else
				theLabel = decodeUTF8(tomString(getLoopingValueD(pin, k, 0)));
		}
		symbolSize = sizeTo2dCanvas(pg, symbolSize, displayMode);
		int radius = symbolSize / 2;
		int dotRad = 3 * radius / 4;

		textSize = sizeTo2dCanvas(pg, textSize, displayMode);

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		if (autoSpreadMode)
			yCenter += k * max(1.4 * symbolSize, 1.1 * textLines * textSize);

		// the labels
		double slope = -0.6;
		int xx;
		int left;
		int right;
		int top;
		int bottom;
		if (theLabel.size()) {
			
			if (textLines > 1) {
				// text in a box
				xx = xCenter + 0.75 * symbolSize + hOffset;
				left = xx;
				right = left + textMaxWidth;
				top = yCenter + slope * textSize - vOffset;
				top -= ((textLines - 1) / 2.0) * 1.2 * textSize;
				bottom = top + (1.1 * textSize) * textLines + 2;

				draw2dText(pg, left, right, top, bottom, theLabel, textSize);

			//	if (DES_TOUCH_ALPHA) {
			//		ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
			//		ctx2.fillRect(left, top, textMaxWidth, 1.0 * textSize * textLines);
			//	}
			}
			else {
				// one line of text
				xx = xCenter + 0.75 * symbolSize + hOffset;
				int yy = yCenter + slope * textSize - vOffset;

				drawWideTextString(pg->hDC2, xx, yy, textSize, 0, theLabel);

				//if (DES_TOUCH_ALPHA) {
				//	ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
				//	ctx2.fillRect(xx, yy + textSize / 5, textWidth, -textSize);
				//}
			}

		}

		// then the Button
		//ctx2.strokeStyle = theColor;

		int penSize = sizeTo2dCanvas(pg, 1, displayMode);
		SelectObject(pg->hDC2, GetColorPen(0, penSize));

		// everyone gets a circle
		BeginPath(pg->hDC2);
		Ellipse(pg->hDC2, xCenter - radius, yCenter - radius, xCenter + radius, yCenter + radius);
		EndPath(pg->hDC2);
		SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
		StrokeAndFillPath(pg->hDC2);

		if (setting[1] == k) { // some get a filled dot
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, xCenter - dotRad, yCenter - dotRad, xCenter + dotRad, yCenter + dotRad);
			SelectObject(pg->hDC2, GetStockObject(BLACK_BRUSH));
			EndPath(pg->hDC2);
			FillPath(pg->hDC2);
		}
		
		//drawDiagnostic(pg->hDC2);
		//if (DES_TOUCH_ALPHA) {
			//ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
			//ctx2.fillRect(xCenter - 1.8 * radius, yCenter - 1.8 * radius, 3.6 * radius, 3.6 * radius);
		//}
	}
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	return true;
}



bool Canvas_RadioButton_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	if (check) {
		// search for the checkbox under the mouse
		int loops = inputsToLoops();
		if (loops == 0)
			return false;

		if (setting[1] >= loops)
			setting[1] = -1;

		int pin = 0;
		bool autoSpreadMode = true;
		if ((int)setting[0] & 1) {
			if(input[pin++].value.size() > 1) 
				autoSpreadMode = false;
			if (input[pin++].value.size() > 1)
				autoSpreadMode = false;
		}
		if ((int)setting[0] & 2) 
			if (input[pin++].value.size() > 1)
				autoSpreadMode = false;

		int newSetting = -1;
		for (int k = loops - 1; k >= 0; k--) { // scan backwards
			pin = 0;
			
			double inputX = getLoopingValueD(0, &pin, k, 0);
			double inputY = getLoopingValueD(0, &pin, k, 0);

			int symbolSize = getLoopingValueD(1, &pin, k, DES_CHECK_SYMBOL_SIZE);
			if (symbolSize <= 0)
				continue;
			
			double hOffset = getLoopingValueD(2, &pin, k, 0);
			double vOffset = getLoopingValueD(2, &pin, k, 0);
			
			if ((int)setting[0] & 8) // preset 3
				pin += 2; 
			if ((int)setting[0] & 16) // color 4
				pin += 3;
			double alpha = getLoopingValueD(5, &pin, k, 1);
			if (alpha <= 0)
				continue;
			
			int textSize = getLoopingValueD(6, &pin, k, DES_CHECK_TEXT_SIZE);
			
			int textLines = getLoopingValueD(7, &pin, k, 1);
			int textMaxWidth = getLoopingValueD(7, &pin, k, 1000);
			if (textLines > 1)
				textLines += 0.28;

			std::wstring theLabel;
			if ((int)setting[0] & bitSetArray[8]) {
				if (input[pin].stringType)
					theLabel = decodeUTF8(getLoopingStringValueD(pin, k, "UNDEFINED"));
				else
					theLabel = decodeUTF8(tomString(getLoopingValueD(pin, k, 0)));

				if ((theLabel.size() == 0) && (input[pin].drivenByComponent != -1))
					continue;
			}
			symbolSize = sizeTo2dCanvas(pg, symbolSize, displayMode);
			textSize = sizeTo2dCanvas(pg, textSize, displayMode);
			textMaxWidth = sizeTo2dCanvas(pg, textMaxWidth, displayMode);

			int textWidth = 40;
			int sensorRadius = 0.9 * symbolSize;

			// convert to pixels
			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			if (autoSpreadMode)
				yCenter += k * max(1.4 * symbolSize, 1.1 * textLines * textSize);

			bool missedText = true;
			if (theLabel.size()) {
				int radius = symbolSize / 2;
				double slope = -0.6;
				int xx = xCenter + 0.75 * symbolSize + hOffset;
				int yy = yCenter + slope * textSize - vOffset;
				int symbolTop = yCenter - symbolSize / 2;

				int xMin;
				int xMax;
				int yMin;
				int yMax;
				if (textLines > 1) {// multi line text - frame is exact
					xMin = xx;
					xMax = xMin + textMaxWidth;
					yMin = yy;
					yMin -= ((textLines - 1) / 2.0) * 1.2 * textSize;
					yMax = yMin + 1.1 * textSize * textLines + 4; // bottom
				}
				else { // single line
					textWidth = calcWideStringWidthFontSize(pg->hDC2, textSize, theLabel);
					xMin = xx-2;
					xMax = xMin + 4 + textWidth;
					yMin = yy-1; // top (plus is down)
					yMax = yMin + textSize+6;
				}

				// diagnostic rectangle around text
				//leftDiagnostic = xMin;
				//rightDiagnostic = xMax;
				//topDiagnostic = yMin;
				//bottomDiagnostic = yMax;
	
				missedText = false;
				if (mouseX > xMax)
					missedText = true;
				if (mouseX < xMin)
					missedText = true;
				if (mouseY > yMax)
					missedText = true;
				if (mouseY < yMin)
					missedText = true;
			}

			// for diagnostic rectangle around button symbol
			if (missedText) {
				int topRadius = sensorRadius;
				int bottomRadius = sensorRadius;

				if (autoSpreadMode) {
					if (k != 0)  // not the first - trim the top
						topRadius = max(1.4 * symbolSize, textLines * textSize) / 2;
					if (k != loops - 1)  // not the last - Trim the bottom
						bottomRadius = max(1.4 * symbolSize, textLines * textSize) / 2;
				}
				//ctx2.strokeStyle = 0x9F0000'; // the sensitive area in red
				//ctx2.strokeRect(xCenter-sensorRadius, yCenter-topRadius, 2*sensorRadius, bottomRadius+topRadius);
				// check for symbol
				if (mouseX > xCenter + sensorRadius)
					continue; // missed Button
				if (mouseX < xCenter - sensorRadius)
					continue; // missed Button
				if (mouseY > yCenter + bottomRadius)
					continue; // too low - missed bottom
				if (mouseY < yCenter - topRadius)
					continue; // too high - missed top
			}

			newSetting = k;

			if (newSetting == setting[1])
				return false; // didnt change
			// update the output
			setting[1] = newSetting;
			output[0].value[0] = newSetting;

			for (int k2 = 0; k2 < loops; k2++) {
				if (k2 == setting[1])
					output[1].value[k2] = 1;
				else
					output[1].value[k2] = 0;
			}
			output[1].dimensions[0] = loops;
			return true;
		}

		if (newSetting == -1)
			return false; // didnt find it
	}
	return true;
}

vector<int> Canvas_RadioButton_Component::GetPinGroups() { return vector<int>{2, 1, 2, 2, 3, 1, 1, 2, 1 }; }
vector<string> Canvas_RadioButton_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", 	// 0=Position	 (1)
		"Symbol Size",					// 1 size		 (2)
		"X Offset", "Y Offset",			// 2=offsets     (4)
		"Preset", "Set",				// 3=preset      (8)
		"Red", "Green", "Blue",			// 4=Color      (16)
		"Alpha",						// 5=Alpha      (32)
		"Text Size",					// 6=text size  (64)
		"Text Lines", "Text Width",		// 7=text lines (128)
		"Labels" }; 					// 8=Labels     (256)
}


bool Canvas_RadioButton_Component::onLoad(Sheet* pg) {
	//  rebuild it
	buildPinLabels();
	if ((int)setting[0] & bitSetArray[8]) {
		pinMaskIndex = 0;
		int pins = leftTerminalCount;
		int loops = inputsToLoopsN(pins - 1);
		for (int k = 0; k < loops; k++)
			output[0].value[k] = setting[1 + k]; // setting is actual value now
	}
	else {
		output[0].value[0] = setting[1];
	}
	return true;
}


Canvas_RadioButton_Component::Canvas_RadioButton_Component()
	: Component(L"2dRADIO", L"2D Radio Group", squareShape, 4, 1, 2, 1)
{
	dialogButtonList = the2dRadioCheckButtonList;
	pinMaskIndex = 0;
	setLeftLabels(3, "X", "Y", "Size", "Labels");
	setRightLabels(3, "Index", "List");
	width = 33;
	setSetting(0, 1 + 256 ); 
	setSetting(1, -1); // no button selected
	//getInternals = getInternals_1up;
	//setInternals = setInternals_1up_IndexListOut;
	setOutputDimension(0, 0, 1);
	setOutputDimension(1, 0, 1);
}


// ************* checkbox *******************************************

bool Canvas_Checkbox_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	int pin = 0;
	bool autoSpreadMode = true;
	if ((int)setting[0] & 1) {
		if (input[pin++].value.size() > 1)
			autoSpreadMode = false;
		if (input[pin++].value.size() > 1)
			autoSpreadMode = false;
	}
	if ((int)setting[0] & 2)
		if (input[pin++].value.size() > 1)
			autoSpreadMode = false;

	if (setting.size() != loops + 1) {
		setting.resize(loops + 1);
		for (int k = 0; k < loops; k++)
			setting[k + 1] = 0;
	}

	for (int k = 0; k < loops; k++) {
		pin = 0;

		double inputX = getLoopingValueD(0, &pin, k, 0);
		double inputY = getLoopingValueD(0, &pin, k, 0);

		double symbolSize = getLoopingValueD(1, &pin, k, DES_CHECK_SYMBOL_SIZE);
		if (symbolSize <= 0)
			continue;
		
		double hOffset = getLoopingValueD(2, &pin, k, 0);
		double vOffset = getLoopingValueD(2, &pin, k, 0);
		
		if ((int)setting[0] & 8) { // preset 3
			pin += 2;
		}
		int inputRed = getLoopingValueColor(4, &pin, k);
		int inputGreen = getLoopingValueColor(4, &pin, k);
		int inputBlue = getLoopingValueColor(4, &pin, k);
		
		double inputAlpha = getLoopingValueD(5, &pin, k, 1);
		
		int textSize = getLoopingValueD(6, &pin, k, DES_CHECK_TEXT_SIZE);
		
		int textLines = getLoopingValueD(7, &pin, k, 1);
		int textMaxWidth = getLoopingValueD(7, &pin, k, 1000);
		
		if (textLines > 1)
			textLines += 0.28;
		
		std::string theLabel;
		if ((int)setting[0] & bitSetArray[8]) { // the labels
			if (input[pin].stringType)
				theLabel = getLoopingStringValueD(pin, k, "UNDEFINED");
			else
				theLabel = tomString(getLoopingValueD(pin, k, 0));
		}

		symbolSize = sizeTo2dCanvas(pg, symbolSize, displayMode);
		int radius = symbolSize / 2;

		textSize = sizeTo2dCanvas(pg, textSize, displayMode);

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		if (autoSpreadMode)
			yCenter += k * max(1.4 * symbolSize, 1.1 * textLines * textSize);


		// the labels
		double slope = -0.6;
		int xx;
		int yy;
		int left;
		int right;
		int top;
		int bottom;
		if (theLabel.size()) {
			if (textLines > 1) {
				// text in a box
				xx = xCenter + 0.75 * symbolSize + hOffset;
				left = xx;
				right = left + textMaxWidth;
				top = yCenter + slope * textSize - vOffset;
				top -= ((textLines - 1) / 2.0) * 1.2 * textSize; 
				bottom = top + (1.1 * textSize) * textLines + 2;

				draw2dText(pg, left, right, top, bottom, decodeUTF8(theLabel), textSize);

				// diagnostics

				//BeginPath(pg->hDC2);
				//MoveToEx(pg->hDC2, xCenter - 100, yCenter, NULL);
				//LineTo(pg->hDC2, xCenter + 100, yCenter);
				//EndPath(pg->hDC2);
				//HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(100, 0, 0));
				//SelectObject(pg->hDC2, hPen);
				//StrokePath(pg->hDC2);

				//BeginPath(pg->hDC2);
				//MoveToEx(pg->hDC2, xCenter - 80, top, NULL);
				//LineTo(pg->hDC2, xCenter + 200, top);
				//EndPath(pg->hDC2);
				//HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(100, 200, 0));
				//SelectObject(pg->hDC2, hPen);
				//StrokePath(pg->hDC2);

				//BeginPath(pg->hDC2);
				//MoveToEx(pg->hDC2, xCenter - 80, bottom, NULL);
				//LineTo(pg->hDC2, xCenter + 200, bottom);
				//EndPath(pg->hDC2);
				//hPen = CreatePen(PS_COSMETIC, 1, RGB(255, 00, 0));
				//SelectObject(pg->hDC2, hPen);
				//StrokePath(pg->hDC2);




				//if (DES_TOUCH_ALPHA) {
					//		ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
					//		ctx2.fillRect(left, top, textMaxWidth, 1.0 * textSize * textLines);
				//}
			}
			else {
				// one line of text
				xx = xCenter + 0.75 * symbolSize + hOffset;
				yy = yCenter + slope * textSize - vOffset;

				drawWideTextString(pg->hDC2, xx, yy, textSize, 0, decodeUTF8(theLabel));

				// diagnostic box around text
				//drawDiagnostic(pg->hDC2);

				//if (DES_TOUCH_ALPHA) {
				//	ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
				//	ctx2.fillRect(xx, yy + textSize / 5, textWidth, -textSize);
				//}
			}

			
		}

		int penWidth = sizeTo2dCanvas(pg, 1, displayMode);
		SelectObject(pg->hDC2, GetColorPen(0, penWidth));

		BeginPath(pg->hDC2);
		Rectangle(pg->hDC2, xCenter - radius, yCenter - radius, xCenter + radius, yCenter + radius);
		EndPath(pg->hDC2);
		SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
		StrokeAndFillPath(pg->hDC2);

		if (setting[k + 1] != 0) {
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, xCenter - radius, yCenter - radius, NULL);
			LineTo(pg->hDC2, xCenter + radius-1, yCenter + radius-1);
			MoveToEx(pg->hDC2, xCenter - radius, yCenter + radius-1, NULL);
			LineTo(pg->hDC2, xCenter + radius-1, yCenter - radius);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		

		if (DES_TOUCH_ALPHA) {
			//ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES_TOUCH_ALPHA);
			//ctx2.fillRect(xCenter - 1.8 * radius, yCenter - 1.8 * radius, 3.6 * radius, 3.6 * radius);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

int Canvas_Checkbox_Component::method2(Sheet* pg) {

	// this is the radio group  - send setting out
	int loops = inputsToLoops();
	if (loops == 0) {
		output[0].value[0] = -1;
		output[0].dimensions.resize(1);
		return true;
	}
	
	int size = setting.size() - 1;
	if (loops < size) {
		// push into memory
		memory.resize(size+1);
		memory[0] = loops;
		for (int k = 0; k < size; k++)
			memory[1+k] = setting[1 + k];
	}
	
	output[0].value.resize(loops);
	setting.resize(1 + loops);

	if (loops > memory[0]) {
		// pull from memory
		size = min(loops, memory.size() - 1);
		for (int k = 0; k < size; k++)
			setting[1 + k] = memory[1+k];
	}

	for (int k = 0; k < loops; k++) {
		if (setting[1 + k])
			output[0].value[k] = 1;
		else
			output[0].value[k] = 0;
	}
	output[0].dimensions[0] = loops;
	
	return 3;
}


bool Canvas_Checkbox_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {

	if (check) {
		// search for the checkbox under the mouse
		int loops = inputsToLoops();
		if (loops == 0)
			return false;

		output[0].value.resize(loops);
		setting.resize(1 + loops);

		int pin = 0;
		bool autoSpreadMode = true;
		if ((int)setting[0] & 1) {
			if (input[pin++].value.size() > 1)
				autoSpreadMode = false;
			if (input[pin++].value.size() > 1)
				autoSpreadMode = false;
		}
		if ((int)setting[0] & 2)
			if (input[pin++].value.size() > 1)
				autoSpreadMode = false;

		int newSetting = -1;
		for (int k = loops - 1; k >= 0; k--) { // scan backwards

			pin = 0;
			double inputX = getLoopingValueD(0, &pin, k, 0);
			double inputY = getLoopingValueD(0, &pin, k, 0);

			double symbolSize = getLoopingValueD(1, &pin, k, DES_CHECK_SYMBOL_SIZE);;
			if (symbolSize <= 0)
				continue;
			
			double hOffset = getLoopingValueD(2, &pin, k, 0);
			double vOffset = getLoopingValueD(2, &pin, k, 0);
			
			if ((int)setting[0] & 8) // preset 3
				pin += 2;
			if ((int)setting[0] & 16) // color 4
				pin += 3;
			double alpha = getLoopingValueD(5, &pin, k, 1);
			if (alpha <= 0)
				continue;
			
			int textSize = getLoopingValueD(6, &pin, k, DES_CHECK_TEXT_SIZE);
			
			int textLines = getLoopingValueD(7, &pin, k, 1);
			int textMaxWidth = getLoopingValueD(7, &pin, k, 1000);
			if (textLines > 1)
				textLines += 0.28;

			std::wstring theLabel;
			if ((int)setting[0] & bitSetArray[8]) {
				if (input[pin].stringType)
					theLabel = decodeUTF8(getLoopingStringValueD(pin, k, "UNDEFINED"));
				else
					theLabel = decodeUTF8(tomString(getLoopingValueD(pin, k, 0)));
				if ((theLabel.size() == 0) && (input[pin].drivenByComponent != -1))
					continue;
			}

			symbolSize = sizeTo2dCanvas(pg, symbolSize, displayMode);
			textSize = sizeTo2dCanvas(pg, textSize, displayMode);
			textMaxWidth = sizeTo2dCanvas(pg, textMaxWidth, displayMode);

			int textWidth = 40;
			int sensorRadius = 0.9 * symbolSize;

			// convert to pixels
			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			if (autoSpreadMode)
				yCenter += k * max(1.4 * symbolSize, 1.1 * textLines * textSize);

			bool missedText = true;
			if (theLabel.size()) {
				int radius = symbolSize / 2;
				double slope = -0.6;
				int xx = xCenter + 0.75 * symbolSize + hOffset;
				int yy = yCenter + slope * textSize - vOffset;
				int symbolTop = yCenter - symbolSize / 2;

				int xMin;
				int xMax;
				int yMin;
				int yMax;
				if (textLines > 1) {// multi line text - frame is exact
					xMin = xx;
					xMax = xMin + textMaxWidth;
					yMin = yy;
					yMin -= ((textLines - 1) / 2.0) * 1.2 * textSize;
					yMax = yMin + 1.1 * textSize * textLines + 4; // bottom
				}
				else { // single line
					textWidth = calcWideStringWidthFontSize(pg->hDC2, textSize, theLabel);
					xMin = xx-2;
					xMax = xMin + 4 + textWidth;
					yMin = yy - 1; // top (plus is down)
					yMax = yMin + textSize + 6;
				}
				// diagnostic rectangle around text
				//leftDiagnostic = xMin;
				//rightDiagnostic = xMax;
				//topDiagnostic = yMin;
				//bottomDiagnostic = yMax;

				missedText = false;
				if (mouseX > xMax)
					missedText = true;
				if (mouseX < xMin)
					missedText = true;
				if (mouseY > yMax)
					missedText = true;
				if (mouseY < yMin)
					missedText = true;
			}

			// for diagnostic rectangle around button symbol
			if (missedText) {
				int topRadius = sensorRadius;
				int bottomRadius = sensorRadius;

				if (autoSpreadMode) {
					if (k != 0)  // not the first - trim the top
						topRadius = max(1.4 * symbolSize, textLines * textSize) / 2;
					if (k != loops - 1)  // not the last - Trim the bottom
						bottomRadius = max(1.4 * symbolSize, textLines * textSize) / 2;
				}
				//ctx2.strokeStyle = 0x9F0000'; // the sensitive area in red
				//ctx2.strokeRect(xCenter-sensorRadius, yCenter-topRadius, 2*sensorRadius, bottomRadius+topRadius);
				// check for symbol
				if (mouseX > xCenter + sensorRadius)
					continue; // missed Button
				if (mouseX < xCenter - sensorRadius)
					continue; // missed Button
				if (mouseY > yCenter + bottomRadius)
					continue; // too low - missed bottom
				if (mouseY < yCenter - topRadius)
					continue; // too high - missed top
			}

			// toggle
			if (setting[k + 1])
				output[0].value[k] = 0;
			else
				output[0].value[k] = 1;
			setting[k + 1] = output[0].value[k];
		
			return true;
		}

		if (newSetting == -1)
			return false; // didnt find it
	}
	return true;
}

vector<int> Canvas_Checkbox_Component::GetPinGroups() { return vector<int>{2, 1, 2, 2, 3, 1, 1, 2, 1 }; }
vector<string> Canvas_Checkbox_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", 		// 0=Position    (1)
		"Symbol Size",						// 1= size       (2)
		"X Offset", "Y Offset",				// 2=offsets     (4)
		"Preset", "Set",					// 3=preset      (8)
		"Red", "Green", "Blue",				// 4=Color      (16)
		"Alpha",							// 5=Alpha      (32)
		"Text Size",						// 6=text size  (64)
		"Text Lines", "Text Width",			// 7=text lines (128)
		"Labels" }; 						// 8=Labels     (256)
}

bool Canvas_Checkbox_Component::onLoad(Sheet* pg) {
	//  rebuild it
	buildPinLabels();
	pinMaskIndex = 0;
	//if ((int)setting[0] & bitSetArray[8]) {

	int loops = setting.size() - 1;
	output[0].value.resize(loops);
	memory.resize(loops+1);
	memory[0] = loops;
	for (int k = 0; k < loops; k++) {
		output[0].value[k] = setting[1 + k]; // setting is actual value now
		memory[1 + k] = setting[1 + k];
	}
	return true;
}
Canvas_Checkbox_Component::Canvas_Checkbox_Component()
	: Component(L"2dCHECK", L"2D Checkbox", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = the2dRadioCheckButtonList;
	pinMaskIndex = 0;
	setLeftLabels(3, "X", "Y", "Size", "Labels");
	setRightLabels(3, "List");
	width = 33;
	setSetting(0, 1 + 256);
	setSetting(1, 0); //  button 0 not selected
	//256=horiz out, 512=vert out, 1024=index, 2048=list 
	//getInternals = getInternals_1up;
	//setInternals = setInternals_1up_ListOut;
}

// ************************************ Polygon *********************************

bool Canvas_Polygon_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	if (((int)setting[0] & 128) == 0)
		return false; // need polygon X's and Y's

	int loopPins;
	int polyPinX;
	int polyPinY;
	int polyPinPen = 0;
	if ((int)setting[0] & 256) {  // 256 is 8=Polygon pen points
		if ((int)setting[0] & 512) {  // 512 is 9=gradient
			loopPins = leftTerminalCount - 5;
			polyPinX = leftTerminalCount - 5;
			polyPinY = leftTerminalCount - 4;
			polyPinPen = leftTerminalCount - 3;
		}
		else {
			loopPins = leftTerminalCount - 3;
			polyPinX = leftTerminalCount - 3;
			polyPinY = leftTerminalCount - 2;
			polyPinPen = leftTerminalCount - 1;
		}
	}
	else {
		if ((int)setting[0] & 512) {  // 512 is 9=gradient
			loopPins = leftTerminalCount - 4;
			polyPinX = leftTerminalCount - 4;
			polyPinY = leftTerminalCount - 3;
		}
		else {
			loopPins = leftTerminalCount - 2;
			polyPinX = leftTerminalCount - 2;
			polyPinY = leftTerminalCount - 1;
		}
	}
	int loops = inputsToLoopsN(loopPins); // everything but the poly arrays
	if (loops == 0)
		return false;

	// polygon X's and Y's have to be the same size
	int polyLen = input[polyPinX].value.size();
	if (polyLen != input[polyPinY].value.size())
		return false;
	if (polyLen < 2)
		return false;
	if ((int)setting[0] & 256) { // 256 is 8=Polygon pen points
		if (polyLen != input[polyPinPen].value.size())
			return false;
	}

	//var gradient = null;
	//if ((int)setting[0] & 512) {  // 512 is 9=gradient
	//	gradient = getGradient(pg, i, leftTerminalCount - 2);
	//}

	// start looping
	for (int j = 0; j < loops; j++) {

		int pin = 0;
		double inx = 0;
		double iny = 0;
		if ((int)setting[0] & 1) { // 1 is 0=Position: ["X", "Y"]
			inx = getLoopingValueD(pin++, j, 0);
			iny = getLoopingValueD(pin++, j, 0);
		}
		
		double xSize = 1.0;
		double ySize = 1.0;
		if ((int)setting[0] & 2) { // 2 is 1=size:["width", "height"]
			xSize = getLoopingValueD(pin++, j, 1);
			ySize = getLoopingValueD(pin++, j, 1);
		}

		double angle = 0.0;
		if ((int)setting[0] & 4) { // 4 is 2=Angle:["Angle"]
			angle = getLoopingValueD(pin++, j, 0);
			angle = rotateInputAngle(angle);
		}

		double lineWidth = 1;
		if ((int)setting[0] & 8) { // 8 is 3=Line Width: [["Line Width"]]
			lineWidth = getLoopingValueD(pin++, j, 1);
		}

		int lineRed = 0;
		int lineGreen = 0;
		int lineBlue = 0;
		if ((int)setting[0] & 16) { // 16 is 4=Line color: ["Line Red", "Line Green", "Line Blue"],
			lineRed = getLoopingValueD(pin++, j, 0);
			lineGreen = getLoopingValueD(pin++, j, 0);
			lineBlue = getLoopingValueD(pin++, j, 0);
		}

		int fillRed = 255;
		int fillGreen = 255;
		int fillBlue = 255;
		bool enableFill = false;
		if ((int)setting[0] & 32) { // 32 is 5=Fill Color: ["Fill Red", "Fill Green", "Fill Blue"]
			fillRed = getLoopingValueD(pin++, j, 0);
			fillGreen = getLoopingValueD(pin++, j, 0);
			fillBlue = getLoopingValueD(pin++, j, 0);
			enableFill = true;
		}

		double alpha;
		if ((int)setting[0] & 64) { // 64 is 6=Alpha: ["Alpha"],
			alpha = getLoopingValueD(pin++, j, 1);
			if ((alpha <= 0) )//&& (gradient == null))
				continue;
		}
		else {
			alpha = 1;
		}

		std::vector<double>polyX;
		std::vector<double>polyY;
		std::vector <int>polyPen;
		polyX.resize(polyLen);
		polyY.resize(polyLen);
		polyPen.resize(polyLen);

		if (angle != 0) {
			double sinAngle = sin(angle);
			double cosAngle = cos(angle);
			for (int k = 0; k < polyLen; k++) {
				polyX[k] = input[polyPinX].value[k] * cosAngle - input[polyPinY].value[k] * sinAngle;
				polyY[k] = input[polyPinX].value[k] * sinAngle + input[polyPinY].value[k] * cosAngle;
				if ((int)setting[0] & 256)  // 256 is 8=Polygon pen points
					polyPen[k] = input[polyPinPen].value[k];
				else
					polyPen[k] = 1;
			}
		}
		else {
			for (int k = 0; k < polyLen; k++) {
				polyX[k] = input[polyPinX].value[k];
				polyY[k] = input[polyPinY].value[k];
				if ((int)setting[0] & 256)  // 256 is 8=Polygon pen points
					polyPen[k] = input[polyPinPen].value[k];
				else
					polyPen[k] = 1;
			}
		}

		int xc = xTo2dCanvas2(pg, inx, displayMode);
		int yc = yTo2dCanvas2(pg, iny, displayMode);

		BeginPath(pg->hDC2);
		int pen = 0;
		for (int k = 0; k < polyLen; k++) {

			int x = xc + sizeTo2dCanvas(pg, polyX[k] * xSize, displayMode);
			int y = yc - sizeTo2dCanvas(pg, polyY[k] * ySize, displayMode);
			if (polyPen[k] <= 0)
				pen = 0;
			if (pen == 0) {
				MoveToEx(pg->hDC2, x, y, NULL);
				pen = 1;
			}
			else
				LineTo(pg->hDC2, x, y);
		}

		if ((polyX[0] == polyX[polyLen - 1]) &&
			(polyY[0] == polyY[polyLen - 1]))
			CloseFigure(pg->hDC2);
		EndPath(pg->hDC2);
		
		if (enableFill || lineWidth ) {
			HBRUSH fillColor = NULL;
			HPEN hPen = NULL;
			if (enableFill) {
				fillColor = CreateSolidBrush(RGB(fillRed, fillGreen, fillBlue));// inputAlpha);
				SelectObject(pg->hDC2, fillColor);
			}
			if (lineWidth) {
				lineWidth = sizeTo2dCanvas(pg, lineWidth, displayMode);
				hPen = CreatePen(setting[1], lineWidth, RGB(lineRed, lineGreen, lineBlue)); //  alpha);
				SelectObject(pg->hDC2, hPen);
			}
			if (enableFill && lineWidth)
				StrokeAndFillPath(pg->hDC2);
			else if (enableFill)
				FillPath(pg->hDC2);
			else if (lineWidth)
				StrokePath(pg->hDC2);
			
			if (enableFill)
				DeleteObject(fillColor);
			if (lineWidth)
				DeleteObject(hPen);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}



void buttonSetDashing1Action(int i){}

std::vector<Button*>polyButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral) };

bool Canvas_Polygon_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}
vector<int> Canvas_Polygon_Component::GetPinGroups() { return vector<int>{2, 2, 1, 1, 3, 3, 1, 2, 1 }; }
vector<string> Canvas_Polygon_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"X size", "Y size",
		"Rotation",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Fill Red", "Fill Green", "Fill Blue",
		"Alpha",
		"Poly Xs", "Poly Ys",
		"Poly Pens"};
}
Canvas_Polygon_Component :: Canvas_Polygon_Component()
	: Canvas_Component(L"POLY", L"2D Polygon", squareShape, 8, 1, 0, 0)
{
	dialogButtonList = polyButtonList;
	setSetting(0, 143);
	setSetting(1, 0); // dash
	pinMaskIndex = 0;
	setComponentWidth();
}


// ******************** value ******************************

int Canvas_Value_Component::method2(Sheet* pg) {

	// removed because we need to update the output even when no 2d Canvas is visible
	//if (pg->pCanvasWindow == NULL)
	//	return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	for (int k = 0; k < loops; k++) {
		int pin = 0;
		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & bitSetArray[0]) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputSize = DES_VALUE_TEXT_SIZE;
		if ((int)setting[0] & bitSetArray[1]) {
			inputSize = getLoopingValueD(pin++, k, DES_VALUE_TEXT_SIZE);
		}

		double inputValue = 0;
		std::string inputText;
		bool stringType = false;
		if ((int)setting[0] & bitSetArray[2]) {
			stringType = input[pin].stringType;
			if (!stringType)
				inputValue = getLoopingValueD(pin++, k, 0);
			else
				inputText = getLoopingStringValueD(pin++, k, "");
		}

		if ((int)setting[0] & bitSetArray[3]) 
			pin += 3;
		if ((int)setting[0] & bitSetArray[4]) 
			pin += 3;

		int inputAlpha = 1;
		if ((int)setting[0] & bitSetArray[5]) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
		}

		int precision;
		bool enablePrecision = false;
		if ((int)setting[0] & bitSetArray[11]) {
			precision = floor(getLoopingValueD(pin++, k, 1));
			if (precision < 1)
				precision = 1;
			else if (precision > 21)
				precision = 21;
			enablePrecision = true;
		}

		double xx = xTo2dCanvas2(pg, inputX, displayMode);
		double yy = yTo2dCanvas2(pg, inputY, displayMode);

		double fontSize = sizeTo2dCanvas(pg, inputSize, displayMode); // added to make the font scale with zoom effect

		double xt;
		int textWidth;
		string newText;
		wstring newWideText;
		if (((inputAlpha <= 0) /*&& (gradient == null)*/) || (inputSize <= 0)) {
			xt = xx;
			textWidth = 0;
		}
		else {

			if (!stringType) { // its a number
				newText = tomString(reasonableValue(inputValue));
				textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, newText);
				if (textWidth == 0)
					textWidth = calcStringWidthFontSize(pg->hDC, fontSize, newText);
				if (textWidth == 0)
					textWidth = calcStringWidthFontSize(magic_hDC, fontSize, newText);
			}
			else {
				newWideText = decodeUTF8(inputText);
				textWidth = calcWideStringWidthFontSize(pg->hDC2, fontSize, newWideText);
				if (textWidth == 0)
					textWidth = calcWideStringWidthFontSize(pg->hDC, fontSize, newWideText);
				if (textWidth == 0)
					textWidth = calcWideStringWidthFontSize(magic_hDC, fontSize, newWideText);
			}
			switch ((int)setting[1]) {
			case 0:// left just
				xt = xx;
				break;
			case 1: // right just
				xt = xx - textWidth;
				break;
			default: // center
				xt = xx - textWidth / 2;
				break;
			}
		}
		output[0].value[k] = canvasToX2(pg, xt + textWidth, displayMode);
	}

	output[0].dimensions[0] = loops;
	return 3;
}
bool Canvas_Value_Component::dial2d(Sheet* pg) {

	//var gradient = null;
	int loops = inputsToLoops();
	if (loops == 0)
		return false;
		
	//var valuePin = 0;  // pick out the value pin and check later
	bool autoStepY = false;
	int autoDeltaY = 0;
	
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & bitSetArray[0]) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			// check for auto increment Y
			if (k == 0)
				if ((input[0].value.size() == 1) && (input[1].value.size() == 1))
					autoStepY = true;
		}
	
		double inputSize = DES_VALUE_TEXT_SIZE;
		if ((int)setting[0] & bitSetArray[1]) {
			inputSize = getLoopingValueD(pin++, k, DES_VALUE_TEXT_SIZE);
		}

		double inputValue = 0;
		std::string inputText;
		bool stringType = false;
		if ((int)setting[0] & bitSetArray[2]) {
			stringType = input[pin].stringType;
			if (!stringType) 
				inputValue = getLoopingValueD(pin++, k, 0);
			else 
				inputText = getLoopingStringValueD(pin++, k, "");
		}
		
		int backRed = 0;
		int backGreen = 0;
		int backBlue = 0;
		bool enableBackground = false;
		if ((int)setting[0] & bitSetArray[3]) {
			backRed = getLoopingValueD(pin++, k, 0);
			backGreen = getLoopingValueD(pin++, k, 0);
			backBlue = getLoopingValueD(pin++, k, 0);
			enableBackground = true;
		}
		
		int fontRed = 0;
		int fontGreen = 0;
		int fontBlue = 0;
		if ((int)setting[0] & bitSetArray[4]) {
			fontRed = getLoopingValueD(pin++, k, 0);
			fontGreen = getLoopingValueD(pin++, k, 0);
			fontBlue = getLoopingValueD(pin++, k, 0);
		}
		
		int inputAlpha = 1;
		if ((int)setting[0] & bitSetArray[5]) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
		}
		
		int precision;
		bool enablePrecision = false;
		if ((int)setting[0] & bitSetArray[11]) {
			precision = floor(getLoopingValueD(pin++, k, 1));
			if (precision < 1)
				precision = 1;
			else if (precision > 21)
				precision = 21;
			enablePrecision = true;
		}

		double xx = xTo2dCanvas2(pg, inputX, displayMode);
		double yy = yTo2dCanvas2(pg, inputY, displayMode);

		double fontSize = sizeTo2dCanvas(pg, inputSize, displayMode); // added to make the font scale with zoom effect

		if (k > 0)
			autoDeltaY += fontSize / 2;
		if (autoStepY) {
			yy += autoDeltaY;
			autoDeltaY += fontSize / 2;
		}
		
		double xt;
		int textWidth;
		string newText;
		wstring newWideText;
		if (((inputAlpha <= 0) /*&& (gradient == null)*/) || (inputSize <= 0)) {
			xt = xx;
			textWidth = 0;
		}
		else {
		
			if (!stringType) {// its a number
				newText = tomString(reasonableValue(inputValue));
				textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, newText);
				if (textWidth == 0)
					textWidth = calcStringWidthFontSize(pg->hDC, fontSize, newText);
				if (textWidth == 0)
					textWidth = calcStringWidthFontSize(magic_hDC, fontSize, newText);
			}
			else {
				newWideText = decodeUTF8(inputText);
				textWidth = calcWideStringWidthFontSize(pg->hDC2, fontSize, newWideText);
				if (textWidth == 0)
					textWidth = calcWideStringWidthFontSize(pg->hDC, fontSize, newWideText);
				if (textWidth == 0)
					textWidth = calcWideStringWidthFontSize(magic_hDC, fontSize, newWideText);
			}
			if (enableBackground) {
				int width2 = textWidth + 2;
				int height2 = fontSize + 2;
				BeginPath(pg->hDC2);
				Rectangle(pg->hDC2, xx - width2 / 2, yy - height2 / 2, xx + width2 / 2, yy + height2 / 2);
				EndPath(pg->hDC2);

				HBRUSH fillColor = CreateSolidBrush(RGB(backRed, backGreen, backBlue));
				SelectObject(pg->hDC2, fillColor);
				FillPath(pg->hDC2);
				DeleteObject(fillColor);
			}

			switch ((int)setting[1]) {
			case 0:// left just
				xt = xx;
				break;
			case 1: // right just
				xt = xx - textWidth;
				break;
			default : // center
				xt = xx - textWidth / 2;
				break;
			}
	
			SetBkMode(pg->hDC2, TRANSPARENT);
			COLORREF textColor = RGB(fontRed, fontGreen, fontBlue);
			if( literal.size() ){ // we have a specific font
				if (!stringType) // its a number
					drawTextString(pg->hDC2, xt, yy - (5 * fontSize / 8), fontSize, textColor, literal[0], newText);
				else
					drawWideTextString(pg->hDC2, xt, yy - (5 * fontSize / 8), fontSize, textColor, literal[0], newWideText);
			}
			else { // use the standard vernanda font
				if (!stringType) // its a number
					drawTextString(pg->hDC2, xt, yy - (5 * fontSize / 8), fontSize, textColor, newText);
				else
					drawWideTextString(pg->hDC2, xt, yy - (5 * fontSize / 8), fontSize, textColor, newWideText);
			}
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

void Canvas_Value_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	displayMode = boolValues[0];
	setting[1] = radioValue; // justify
}
void doubleClickValue(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels = { "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels = { "Left Justify", "Right Justify", "Center Justify" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
void doubleClickValueFont(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildValueFontDialogButtons(pg, i);
}
std::vector<Button*>valueButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickValue),
	new Button(3, 80, 85, 20, L"Font...", doubleClickValueFont) };

bool Canvas_Value_Component::onLoad(Sheet* pg) {
	if (setting[1] > 2)
		setting[1] = 2;
	buildPinLabels();
	return false;
}

vector<int> Canvas_Value_Component::GetPinGroups() { return vector<int>{ 2, 1, 1, 3, 3, 1  }; }
vector<string> Canvas_Value_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Size",
		"Value",
		"Back Red", "Back Green", "Back Blue",
		"Text Red", "Text Green", "Text Blue",
		"Alpha" };
}

Canvas_Value_Component::Canvas_Value_Component()
	: Component(L"VALUE", L"2D Value", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = valueButtonList;
	setLiteral(0, "");
	setSetting(0, 7);
	setSetting(1, 2); // center justify
	pinMaskIndex = 0;
	setRightLabels(1, "X");
	completionTestChecks = 1;
	completionTestRadios = 3;
}

// ****************************** DRAW AXIS ******************


	// calculate tics and labels for axis factor
void calcTicsAndLabels(double input, int *ticPtr, int *labelPtr) {
	double a = log(input) / 2.302585092994046;
	double floora = floor(a);
	double b = a - floora;
	int c = 1;
	int d = 1;
	if (b > 0.63) {
		c = 10;
		d = 1;
	}
	else if (b > 0.24) {
		c = 20;
		d = 2;
	}
	else {
		c = 50;
		d = 10;
	}
	double e = pow(10, floora);
	int f = floor(c / e);
	int g = floor(d / e);

	*ticPtr = g;
	*labelPtr = f;
}


void innerTicLabel(double input, int *ticPtr, int *labelPtr) {

	int tic = 0;
	int label = 0;
	if (input == 0) {
		*ticPtr = tic;
		*labelPtr = label;
	}
	else if (input < 1.5) {

		double factor2 = 0.001;
		double test = 0.0015;
		while (input < test) {
			factor2 /= 1000;
			test /= 1000;
		}

		int innerTic, innerLabel;
		calcTicsAndLabels(input * factor2, &innerTic, &innerLabel);
		*ticPtr = innerTic * factor2;
		*labelPtr = innerLabel * factor2;
	}
	else {

		double factor2 = 1000;
		double test = 1500;
		while (input > test) {
			factor2 *= 1000;
			test *= 1000;
		}

		int innerTic, innerLabel;
		calcTicsAndLabels(input / factor2, &innerTic, &innerLabel);
		*ticPtr = innerTic / factor2;
		*labelPtr = innerLabel / factor2;
	}
	
	return;
}





// ********************* 2d Buttons **********************

int Canvas_RoundButton_Component::method2(Sheet* pg) {
	// this is the Button  - send setting out
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	if (output[0].value.size() != loops) {
		output[0].value.resize(loops);
		for (int k = 0; k < loops; k++)
			output[0].value[k] = 0;
	}
	setComponentWidth();
	
	return 3;
}





bool Canvas_RoundButton_Component::dial2d(Sheet* pg){ //, bool color) {
	if (pg->pCanvasWindow == NULL)
		return false;
	bool color = false; // ************************** PATCH

	int loops = inputsToLoops();
	if (loops == 0)
		return false;
	for (int k = 0; k < loops; k++) {
		double inputX = getLoopingValueD(0, k, 0);
		double inputY = getLoopingValueD(1, k, 0);
		double radius = getLoopingValueD(2, k, DES_PUSHBUTTON_SIZE);

		COLORREF theColor = 0xDDDDDD;
		COLORREF blackColor = 0x000000;
		COLORREF whiteColor = 0xFFFFFF;
		if (color) {
			int inputRed = getLoopingValueD(3, k, 0);
			int inputGreen = getLoopingValueD(4, k, 0);
			int inputBlue = getLoopingValueD(5, k, 0);
			int inputAlpha = 255*getLoopingValueD(6, k, 1);
			theColor = RGB(inputRed, inputGreen, inputBlue);// , inputAlpha);
			blackColor = RGB(inputAlpha, inputAlpha, inputAlpha);
			whiteColor = blackColor;
		}

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		if (radius <= 0)
			continue;
		radius = sizeTo2dCanvas(pg, radius, displayMode);

		COLORREF fillStyle = blackColor;
		//ctx2.lineWidth = 1;*************************************************

		// if there is a user label put below
		if (userLabel.size()) {
			//ctx2.font = '9px ' + default_2D_FontFamily;
			int widthValue = calcLargeStringWidthFont( pg->hDC2, userLabel);
			drawLargeTextString(pg->hDC2, xCenter - widthValue / 2, yCenter + radius + 14, fillStyle, userLabel);
		}

		// then the Button
		COLORREF strokeStyle = blackColor;
		BeginPath(pg->hDC2);
		Ellipse(pg->hDC2, xCenter - radius, yCenter - radius, xCenter + radius, yCenter + radius);
		EndPath(pg->hDC2);

		if (output[0].value[k] != 0)
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));// white
		else
			SelectObject(pg->hDC2, GetStockObject(GRAY_BRUSH));  // gray
		StrokeAndFillPath(pg->hDC2);

		// the Halo
		//if (output[0].value[k]) {
		//	ctx2.beginPath();
		//	ctx2.arc(xCenter, yCenter, DES_HALO_SIZE, 0, TWO_PI);
		//	ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	ctx2.fill();
		//}

		// the touch area - DIAGNOSTIC
		//if (DES_TOUCH_ALPHA) {
		//	int touchSize = max(DES_TOUCH_SIZE, radius);
		//	ctx2.beginPath();
		//	ctx2.arc(xCenter, yCenter, touchSize, 0, TWO_PI);
		//	ctx2.fillStyle = RGB(0, 255, 0);// , DES_TOUCH_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}




void Canvas_RoundButton_Component::onMouseUp2d(Sheet* pg){// int touch) {
	int loops = inputsToLoops();

	for (int k = 0; k < loops; k++)
		output[0].value[k] = 0;

	dial2d(pg);
}

bool Canvas_RoundButton_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	if (check) {
		// search for the button under the mouse
		int loops = inputsToLoops();
		if (loops == 0)
			return false;
		for (int k = loops - 1; k >= 0; k--) { // scan backwards
			double inputX = getLoopingValueD(0, k, 0);
			double inputY = getLoopingValueD(1, k, 0);
			double radius = getLoopingValueD(2, k, DES_PUSHBUTTON_SIZE);

			// convert to pixels
			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			if (radius <= 0)
				continue;

			if (type == L"2dClrButton") {
				double alpha = getLoopingValueD(6, k, 1);
				if (alpha <= 0)
					continue;
			}
			radius = sizeTo2dCanvas(pg, radius, displayMode);
			int touchSize = max(DES_TOUCH_SIZE, radius);

			// fast check for square
			if (mouseX > xCenter + touchSize)
				continue; // missed Button
			if (mouseX < xCenter - touchSize)
				continue; // missed Button
			if (mouseY > yCenter + touchSize)
				continue; // missed Button
			if (mouseY < yCenter - touchSize)
				continue; // missed Button

			// better check for circle
			int dx = mouseX - xCenter;
			int dy = mouseY - yCenter;
			if (dx * dx + dy * dy > touchSize * touchSize)
				continue;

			//found it
			output[0].value[k] = 1;
			return true;
		}
		return false; // didnt find it
	}

	return true;
}



std::vector<Button*>the2dButtonButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dDisplayMode)};

//void Canvas_2dButton_Component::onMouseUp2d(Sheet* pg){//, int touch) {
//	int loops = inputsToLoops();
//
//	for (int k = 0; k < loops; k++)
//		output[0].value[k] = 0;
//
//	dial2d(pg);
//}


Canvas_RoundButton_Component::Canvas_RoundButton_Component()
	:	Component(L"2dButton", L"2D Button", squareShape, 3, 1, 1, 1)
{
	output[0].dimensions[0] = 1;
	dialogButtonList = the2dButtonButtonList;
	setLeftLabels(3, "X", "Y", "Radius");
}

// ************************** Button 2 *************************


int Canvas_RoundedRectangleButton_Component::method2(Sheet* pg) {
	// this is the Button  - send setting out
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	output[0].value.resize(loops);
	for (int k = 0; k < loops; k++)
		output[0].value[k] = 0;

	setComponentWidth();
	output[0].dimensions[0] = loops;

	return 3;
}


bool Canvas_RoundedRectangleButton_Component::dial2d(Sheet* pg) {
	if (!checkValueInputs())
		return false;
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & bitSetArray[0]) { // x and y
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}
		
		double inputWidth = 80;
		double inputHeight = 25;
		if ((int)setting[0] & bitSetArray[1]) { // width and height
			inputWidth = getLoopingValueD(pin++, k, 80);
			inputHeight = getLoopingValueD(pin++, k, 25);
			if ((inputWidth <= 0) || (inputHeight <= 0))
				continue;
		}
		
		double rounding = 12;
		if ((int)setting[0] & bitSetArray[2]) { // rounding
			rounding = getLoopingValueD(pin++, k, 12);
		}
		
		double frameWidth = 1;
		if ((int)setting[0] & bitSetArray[3]) { // frame width
			frameWidth = getLoopingValueD(pin++, k, 1);
			if (frameWidth <= 0)
				frameWidth = 0;
		}
		
		int frameRed = 0;
		int frameGreen = 0;
		int frameBlue = 0;
		if ((int)setting[0] & bitSetArray[4]) { // frame color
			frameRed = getLoopingValueD(pin++, k, 0);
			frameGreen = getLoopingValueD(pin++, k, 0);
			frameBlue = getLoopingValueD(pin++, k, 0);
		}
		
		string text = "";
		if ((int)setting[0] & bitSetArray[5]) { // text
			if (input[pin].stringType)
				text = getLoopingStringValueD(pin++, k, "");
			else
				text = tomString(getLoopingValueD(pin++, k, 0));
		}
		
		int textSize = 20;
		if ((int)setting[0] & bitSetArray[6]) { // text size
			textSize = getLoopingValueD(pin++, k, 20);
		}
		
		int textOffRed = 0;
		int textOffGreen = 0;
		int textOffBlue = 0;
		if ((int)setting[0] & bitSetArray[7]) { // text off color
			textOffRed = getLoopingValueD(pin++, k, 0);
			textOffGreen = getLoopingValueD(pin++, k, 0);
			textOffBlue = getLoopingValueD(pin++, k, 0);
		}
		
		int textOnRed = 0;
		int textOnGreen = 0;
		int textOnBlue = 0;
		if ((int)setting[0] & bitSetArray[8]) { // text on color
			textOnRed = getLoopingValueD(pin++, k, 0);
			textOnGreen = getLoopingValueD(pin++, k, 0);
			textOnBlue = getLoopingValueD(pin++, k, 0);
		}
		
		int fillOffRed = 255;
		int fillOffGreen = 255;
		int fillOffBlue = 255;
		if ((int)setting[0] & bitSetArray[9]) { // fill off color
			fillOffRed = getLoopingValueD(pin++, k, 255);
			fillOffGreen = getLoopingValueD(pin++, k, 255);
			fillOffBlue = getLoopingValueD(pin++, k, 255);
		}
		
		int fillOnRed = 200;
		int fillOnGreen = 200;
		int fillOnBlue = 200;
		if ((int)setting[0] & bitSetArray[10]) { // fill on color
			fillOnRed = getLoopingValueD(pin++, k, 200);
			fillOnGreen = getLoopingValueD(pin++, k, 200);
			fillOnBlue = getLoopingValueD(pin++, k, 200);
		}
		
		double alpha = 1;
		int enable = 1;
		if ((int)setting[0] & bitSetArray[11]) { // alpha and enable
			alpha = getLoopingValueD(pin++, k, 1);
			enable = getLoopingValueD(pin++, k, 1);
			if (alpha <= 0)
				continue;
		}
		
		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		textSize = sizeTo2dCanvas(pg, textSize, displayMode);

		//if (literal[0] == undefined)
		//	ctx2.font = textSize.tomString() + 'px ' + default_2D_FontFamily;
		//else if (literal[0] == '')
		//	ctx2.font = textSize.tomString() + 'px ' + default_2D_FontFamily;
		//else
		//	ctx2.font = textSize.tomString() + 'px ' + literal[0];

		int textWidth = calcLargeStringWidthFont(pg->hDC2, text);

		inputWidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
		if ((inputWidth <= 0) || (inputWidth > 10000))
			continue;
		inputHeight = sizeTo2dCanvas(pg, inputHeight, displayMode);
		if ((inputHeight <= 0) || (inputHeight > 10000))
			continue;
		rounding = sizeTo2dCanvas(pg, rounding, displayMode);

		if (((int)setting[0] & bitSetArray[1]) == 0) { // NO width and height
			if ((int)setting[0] & bitSetArray[5]) { // BUT has text
				inputWidth = textWidth + 0.9 * textSize;
				inputHeight = 1.2 * textSize;
			}
		}

		COLORREF fillStyle = 0;
		COLORREF strokeStyle = 0;
		
		if ((int)output[0].value[k])
			fillStyle = RGB(fillOnRed, fillOnGreen, fillOnBlue);// , alpha);
		else
			fillStyle = RGB(fillOffRed, fillOffGreen, fillOffBlue);// , alpha);
		if (frameWidth) {
			frameWidth = sizeTo2dCanvas(pg, frameWidth, displayMode);
			strokeStyle = RGB(frameRed, frameGreen, frameBlue);// , alpha);
		}

		// first create the button shape
		int xMin = xCenter - inputWidth / 2;
		int xMax = xCenter + inputWidth / 2;
		int yMin = yCenter - inputHeight / 2;
		int yMax = yCenter + inputHeight / 2;

		if (rounding <= 0) {
			// first paint the button
			BeginPath(pg->hDC2);
			Rectangle( pg->hDC2, xMin, yMin, xMax, yMax);
			EndPath(pg->hDC2);
		}
		else {
			int maxRounding = min(inputWidth, inputHeight);
			if (rounding > maxRounding)
				rounding = maxRounding;
			BeginPath(pg->hDC2);
			RoundRect(pg->hDC2, xMin, yMin, xMax, yMax, rounding, rounding);
			EndPath(pg->hDC2);
		}
		HPEN hPen = CreatePen(PS_COSMETIC, frameWidth, strokeStyle);
		SelectObject(pg->hDC2, hPen);
		HBRUSH fillColor = CreateSolidBrush(fillStyle);
		SelectObject(pg->hDC2, fillColor);
		StrokeAndFillPath(pg->hDC2);
		DeleteObject(fillColor);
		DeleteObject(hPen);

		// last the text
		if (text.size() > 0) {
			COLORREF fillStyle;
			if (output[0].value[k])
				fillStyle = RGB(textOnRed, textOnGreen, textOnBlue);// , alpha);
			else
				fillStyle = RGB(textOffRed, textOffGreen, textOffBlue);// , alpha);

			HBRUSH fillColor = CreateSolidBrush(fillStyle);
			SelectObject(pg->hDC2, fillColor);
			drawLargeTextString(pg->hDC2, xCenter - textWidth / 2, yCenter -8, fillStyle, text);
			DeleteObject(fillColor);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}


bool Canvas_RoundedRectangleButton_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	if (check) {
		// search for the button under the mouse
		int loops = inputsToLoops();
		if (loops == 0)
			return false;

		for (int k = loops - 1; k >= 0; k--) { // scan backwards
			int pin = 0;

			double inputX = 0;
			double inputY = 0;
			if ((int)setting[0] & bitSetArray[0]) { // x and y
				 inputX = getLoopingValueD(pin++, k, 0);
				 inputY = getLoopingValueD(pin++, k, 0);
			}
			
			double inputWidth = 80;
			double inputHeight = 25;
			if ((int)setting[0] & bitSetArray[1]) { // width and height
				inputWidth = getLoopingValueD(pin++, k, 80);
				inputHeight = getLoopingValueD(pin++, k, 25);
				if ((inputWidth <= 0) || (inputHeight <= 0))
					continue;
			}
			
			string text;
			if ((int)setting[0] & bitSetArray[2])  // rounding
				pin++;
			if ((int)setting[0] & bitSetArray[3]) // frame width
				pin++;
			if ((int)setting[0] & bitSetArray[4])  // frame color
				pin += 3;
			if ((int)setting[0] & bitSetArray[5]) { // text
				if( input[pin].stringType)
					text = getLoopingStringValueD(pin++, k, "");
				else
					text = tomString(getLoopingValueD(pin++, k, 0));
			}
			else {
				text = "";
			}

			int textSize = 20;
			if ((int)setting[0] & bitSetArray[6]) { // text size
				textSize = getLoopingValueD(pin++, k, 20);
			}
			else {
				
			}
			if ((int)setting[0] & bitSetArray[7]) // text off color
				pin += 3;
			if ((int)setting[0] & bitSetArray[8]) // text on color
				pin += 3;
			if ((int)setting[0] & bitSetArray[9]) // fill off color
				pin += 3;
			if ((int)setting[0] & bitSetArray[10]) // fill on color
				pin += 3;

			double alpha = 1;
			int enable = 1;
			if ((int)setting[0] & bitSetArray[11]) { // alpha and enable
				alpha = getLoopingValueD(pin++, k, 1);
				enable = getLoopingValueD(pin++, k, 1);
				if (enable <= 0)
					continue;
			}
			

			// convert to pixels
			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			textSize = sizeTo2dCanvas(pg, textSize, displayMode);
			//ctx2.font = textSize.tomString() + 'px ' + default_2D_FontFamily;
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);

			inputWidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
			if ((inputWidth <= 0) || (inputWidth > 10000))
				continue;
			inputHeight = sizeTo2dCanvas(pg, inputHeight, displayMode);
			if ((inputHeight <= 0) || (inputHeight > 10000))
				continue;

			if (((int)setting[0] & bitSetArray[1]) == 0) { // NO size
				if ((int)setting[0] & bitSetArray[5]) { // BUT has text
					inputWidth = textWidth + textSize / 4;
					inputHeight = textSize;
				}
			}

			// check for square
			if (mouseX > xCenter + inputWidth / 2)
				continue; // missed Button
			if (mouseX < xCenter - inputWidth / 2)
				continue; // missed Button
			if (mouseY > yCenter + inputHeight / 2)
				continue; // missed Button
			if (mouseY < yCenter - inputHeight / 2)
				continue; // missed Button

			//found it
			output[0].value[k] = 1;
	
			return true;
		}
		return false; // didnt find it
	}
	return true;
}



//void onMouseUp2dButton2_(Sheet* pg, int i, int touch) {
//	int loops = inputsToLoops();
//
//	for (int k = 0; k < loops; k++)
//		output[0].value[k] = 0;
//
//	dial2dButton2_(pg, i);
//}



void Canvas_RoundedRectangleButton_Component::onMouseUp2d(Sheet* pg){// , touch) {
	int loops = inputsToLoops();

	for (int k = 0; k < loops; k++)
		output[0].value[k] = 0;

	dial2d(pg);
}

std::vector<Button*>the2dButton2ButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayMode) }; //share with 2dButton


bool Canvas_RoundedRectangleButton_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> Canvas_RoundedRectangleButton_Component::GetPinGroups() { return vector<int>{2, 2, 1, 1, 3, 1, 1, 3, 3, 3, 3, 2}; }
vector<string> Canvas_RoundedRectangleButton_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Width", "Height",
		"Rounding",
		"Frame Width",
		"Frame Red", "Frame Green", "Frame Blue",
		"Text",
		"Text Size",
		"Text Off Red", "Text Off Green", "Text Off Blue",
		"Text On Red", "Text On Green", "Text On Blue",
		"Fill Off Red", "Fill Off Green", "Fill Off Blue",
		"Fill On Red", "Fill On Green", "Fill On Blue",
		"Alpha", "Enable"};
			//"Out"],                                             
			//"Left", "Right"],                                   
			//"Top", "Bottom"]];  
}
Canvas_RoundedRectangleButton_Component::Canvas_RoundedRectangleButton_Component()
	: Component(L"2dButton2", L"2D Button 2", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = the2dButton2ButtonList;
	setRightLabels(3, "Out");
	setSetting(0, 1 + 2 + 32 + 4096);
	pinMaskIndex = 0;
}


// ***************************** 2d Vector ******************************

	// setting[0] = 7;      // option code: see below
	// setting[1] = 1;      // number of Vectors to place
	// setting[2] = 20;      // X[0]
	// setting[3] = 20;      // Y[0]
	// setting[4] = 20;      // dX[0]
	// setting[5] = 20;      // dY[0]
	// the line from base to tip 1
	// draw the base (circle) 2
	// base number 4
	// draw the tip (circle) 8
	// tip number 16
	// show the base coordinates 32
	// show the tip coordinates 64
	// has detent inputs 128
	// color mode 256
bool Canvas_Vector_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int colorPin = 0;
	int detentPin = 0;
	if (((int)setting[0] & 128) && ((int)setting[0] & 256))
		colorPin = 4;

	int x0, y0;
	int x1, y1;

	COLORREF strokeStyle = 0x000000;
	COLORREF lineColor = 0x000000;
	COLORREF blackColor = 0x000000;
	COLORREF whiteColor = 0xFFFFFF;
	for (int j = 0; j < setting[1]; j++) {
		if ((int)setting[0] & 256) {
			int inputRed = getLoopingValueD(colorPin + 0, j, 0);
			int inputGreen = getLoopingValueD(colorPin + 1, j, 0);
			int inputBlue = getLoopingValueD(colorPin + 2, j, 0);
			double inputAlpha = getLoopingValueD(colorPin + 3, j, 1);
			if (inputAlpha <= 0)
				continue;
			lineColor = RGB(inputRed, inputGreen, inputBlue);
			blackColor = 0x000000;
			whiteColor = 0xFFFFFF;
		}
		
		strokeStyle = lineColor;
		int lineWidth = 1;

		x0 = xTo2dCanvas2(pg, setting[4 * j + 2], displayMode );
		y0 = yTo2dCanvas2(pg, setting[4 * j + 3], displayMode);

		x1 = xTo2dCanvas2(pg, setting[4 * j + 2] + setting[4 * j + 4], displayMode);
		y1 = yTo2dCanvas2(pg, setting[4 * j + 3] + setting[4 * j + 5], displayMode);

		// draw the base (circle) 2
		if ((int)setting[0] & 2) {
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, x0 - DES_KNOB_SIZE, y0 - DES_KNOB_SIZE, x0 + DES_KNOB_SIZE, y0 + DES_KNOB_SIZE);
			EndPath(pg->hDC2);
			//if ((int)setting[0] & 4) {
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			//	FillPath(pg->hDC2);
			//}
			StrokeAndFillPath(pg->hDC2);
		}

		// draw the tip (circle) 8
		if ((int)setting[0] & 8) {
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, x1 - DES_KNOB_SIZE, y1 - DES_KNOB_SIZE, x1 + DES_KNOB_SIZE, y1 + DES_KNOB_SIZE);
			EndPath(pg->hDC2);
			//if ((int)setting[0] & 16) {
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			//	FillPath(pg->hDC2);
			//}
			StrokeAndFillPath(pg->hDC2);
		}

		// the line from base to tip 1
		if ((int)setting[0] & 1) {
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, x0, y0, NULL);
			LineTo(pg->hDC2, x1, y1);
			EndPath(pg->hDC2);
			SelectObject(pg->hDC2, GetColorPen(lineColor, 1));
			StrokePath(pg->hDC2);
			int deltaX = abs(x0 - x1);
			int deltaY = abs(y0 - y1);
			double fullLength = sqrt(deltaX * deltaX + deltaY * deltaY);
			if (fullLength > 1) {
				double dxol = (x0 - x1) / fullLength;
				double dyol = (y0 - y1) / fullLength;

				int ax = x1 + 15 * dxol + 5 * dyol;
				int bx = x1 + 15 * dxol - 5 * dyol;
				int ay = y1 - 5 * dxol + 15 * dyol;
				int by = y1 + 5 * dxol + 15 * dyol;

				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, ax, ay, NULL);
				LineTo(pg->hDC2, x1, y1);
				LineTo(pg->hDC2, bx, by);
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, GetColorPen(lineColor, 1));
				StrokePath(pg->hDC2);
			}
		}
		
		//ctx2.font = DES_KNOB_SIZE.tomString() + 'px ' + default_2D_FontFamily;
	//	COLORREF fillStyle;
		if ((int)setting[0] & 4) // base number 4
		{
			string text = tomString(j);
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
			drawLargeTextString(pg->hDC2, x0 - textWidth / 2, y0 - 8, blackColor, text);
		}
		// the base Halo
		//if (memory[0] == j) {
		//	BeginPath(pg->hDC2);
		//	Ellipse(pg->hDC2, x0 - DES_HALO_SIZE, y0 - DES_HALO_SIZE, x0 + DES_HALO_SIZE, y0 + DES_HALO_SIZE);
		//	EndPath(pg->hDC2);
		//	HBRUSH fillColor = CreateSolidBrush(RGB(DES_HALO_ALPHA, DES_HALO_ALPHA, DES_HALO_ALPHA));
		//	SelectObject(pg->hDC2, fillColor);
		//	FillPath(pg->hDC2);
		//	DeleteObject(fillColor);
		//}
		// the base touch area - DIAGNOSTIC
		if (DES_TOUCH_ALPHA) {
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, x0 - DES_TOUCH_SIZE, y0 - DES_TOUCH_SIZE, x0 + DES_TOUCH_SIZE, y0 + DES_TOUCH_SIZE);
			EndPath(pg->hDC2);
			HBRUSH fillColor = CreateSolidBrush(RGB(0, 255, 0));
			SelectObject(pg->hDC2, fillColor);
			FillPath(pg->hDC2);
			DeleteObject(fillColor);
		}

		if ((int)setting[0] & 16) { // tip number 16
			string text = tomString(j);
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
			drawLargeTextString(pg->hDC2, x1 - textWidth / 2, y1 - 8, blackColor, text);
		}
		// the tip Halo
		//if (memory[0] == j + setting[1]) {
		//	BeginPath(pg->hDC2);
		//	Ellipse(pg->hDC2, x1 - DES_HALO_SIZE, y1 - DES_HALO_SIZE, x1 + DES_HALO_SIZE, y1 + DES_HALO_SIZE);
		//	EndPath(pg->hDC2);
		//	fillStyle = RGB(DES_HALO_ALPHA, DES_HALO_ALPHA, DES_HALO_ALPHA);
		//	FillPath(pg->hDC2);
		//}
		// the tip touch area - DIAGNOSTIC
		//if (DES_TOUCH_ALPHA) {
		//	BeginPath(pg->hDC2);
		//	Ellipse(pg->hDC2, x1 - DES_TOUCH_SIZE, y1 - DES_TOUCH_SIZE, x1 + DES_TOUCH_SIZE, y1 + DES_TOUCH_SIZE);
		//	EndPath(pg->hDC2);
		//	fillStyle = RGB(0, 255, 0, DES_TOUCH_ALPHA);
		//	FillPath(pg->hDC2);
		//}

		if ((int)setting[0] & 32) { // show the base coordinates 32
			string text = "(" + tomString(setting[4 * j + 2]) + ", "
				+ tomString(setting[4 * j + 3]) + ")";
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
			drawLargeTextString(pg->hDC2, x0 - textWidth / 2, y0 + DES_KNOB_SIZE, blackColor, text);
		}

		if ((int)setting[0] & 64) { // show the tip coordinates 64
			string text = "(" + tomString(setting[4 * j + 4]) + ", "
				+ tomString(setting[4 * j + 5]) + ")";
			int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
			drawLargeTextString(pg->hDC2, x1 - textWidth / 2, y1 - DES_KNOB_SIZE - 16, blackColor, text);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

int Canvas_Vector_Component::method2(Sheet* pg) {

	int oldSize = output[0].value.size();
	int newSize = setting[1];
	if (oldSize != newSize) {
		for (int j = 0; j < 4; j++) {
			output[j].value.resize(newSize);
			output[j].dimensions[0] = newSize;
		}
		setting.resize(4 * newSize + 2);

		int newX = 0;
		int newY = 0;
		for (int j = oldSize; j < newSize; j++) {
			setting[4 * j + 2] = newX;   // x
			setting[4 * j + 3] = newY;   // y
			setting[4 * j + 4] = 40;   //  dx
			setting[4 * j + 5] = 40;   //  dy
			newX -= 20;
			newY += 20;
		}
	}

	//  send settings out
	for (int j = 0; j < setting[1]; j++) {
		output[0].value[j] = setting[4 * j + 2];   // x
		output[1].value[j] = setting[4 * j + 3];   // y
		output[2].value[j] = setting[4 * j + 4];   //  dx
		output[3].value[j] = setting[4 * j + 5];   //  dy
	}
	for (int j = 0; j < 4; j++)
		output[j].dimensions[0] = setting[1];
	return true;
}


bool Canvas_Vector_Component::onLoad(Sheet* pg) {
	int inPins = 0;
	if ((int)setting[0] & 128)
		inPins += 4; // add detent inputs
	if ((int)setting[0] & 256)
		inPins += 4;

	height = pinToHeight(max(inPins, rightTerminalCount));

	// left terminals are inputs
	buildInputPins(inPins);

	if (((int)setting[0] & 128) && ((int)setting[0] & 256)) {
		setLeftLabels(8, "Xs", "Ys", "dXs", "dYs", "Red", "Green", "Blue", "Alpha");
	}else
	if ((int)setting[0] & 128) {
		setLeftLabels( 4, "Xs", "Ys", "dXs", "dYs");
	}else
	if ((int)setting[0] & 256) {
		setLeftLabels( 4, "Red", "Green", "Blue", "Alpha");
	}
	
	return true;
}



bool Canvas_Vector_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	int n = setting[1];
	bool hitBase = false;
	bool hitTip = false;
	int j;

	int colorPin = 0;
	int detentPin = 0;
	if (((int)setting[0] & 128) && ((int)setting[0] & 256))
		colorPin = 4;

	if (!check)
		int k = 0;
	if (check) {
		//  find the current dial position
		
		// first search for a tip
		for (j = n - 1; j >= 0; j--) {

			int knobX = xTo2dCanvas2(pg, setting[4 * j + 2] + setting[4 * j + 4], displayMode);
			int knobY = yTo2dCanvas2(pg, setting[4 * j + 3] + setting[4 * j + 5], displayMode);

			if ((int)setting[0] & 256) {
				double inputAlpha = getLoopingValueD(colorPin+3, j, 1);
				if (inputAlpha <= 0)
					continue;
			}

			if (mouseX > knobX + DES_TOUCH_SIZE)
				continue; // missed dial
			if (mouseX < knobX - DES_TOUCH_SIZE)
				continue; // missed dial

			if (mouseY > knobY + DES_TOUCH_SIZE)
				continue; // missed dial
			if (mouseY < knobY - DES_TOUCH_SIZE)
				continue; // missed dial

			memory[0] = j + n;
			hitTip = true;
			break;
		}
		if (!hitTip) {
			// then search for a base
			for (j = n - 1; j >= 0; j--) {
				int knobX = xTo2dCanvas2(pg, setting[4 * j + 2], displayMode);
				int knobY = yTo2dCanvas2(pg, setting[4 * j + 3], displayMode);;

				if (type == L"2dClrVector") {
					double inputAlpha = getLoopingValueD(3, j, 1);
					if (inputAlpha <= 0)
						continue;
				}
				if (mouseX > knobX + DES_TOUCH_SIZE)
					continue; // missed dial
				if (mouseX < knobX - DES_TOUCH_SIZE)
					continue; // missed dial

				if (mouseY > knobY + DES_TOUCH_SIZE)
					continue; // missed dial
				if (mouseY < knobY - DES_TOUCH_SIZE)
					continue; // missed dial

				memory[0] = j;
				hitBase = true;
				break;
			}
		}
		if (!hitBase && !hitTip) {
			memory[0] = -1;
			return false;
		}
	}
	else {
		if (memory[0] < 0)
			return false;

		if (memory[0] < n) {
			hitBase = true;
			j = memory[0];
		}
		else {
			hitTip = true;
			j = memory[0] - n;
		}
	}

	int close = 20;
	if (hitBase) {
	
		int	testX = canvasToSize(pg, floor(mouseX) - pg->canvas2Width / 2, displayMode) + pg->canvas2xOffset;
		int testY = canvasToSize(pg, pg->canvas2Height / 2 - floor(mouseY), displayMode) + pg->canvas2yOffset;
		
			// check for position detents
		if (((int)setting[0] & 128) &&
			(input[0].drivenByComponent != -1) &&
			(input[1].drivenByComponent != -1) &&
			(input[0].value.size() == input[1].value.size())) {
			for (int z = 0; z < input[0].value.size(); z++)
				if ((abs(testX - input[0].value[z]) <= close) &&
					(abs(testY - input[1].value[z]) <= close)) {
					testX = input[0].value[z];
					testY = input[1].value[z];
					break;
				}
		}
		setting[4 * j + 2] = testX;
		setting[4 * j + 3] = testY;
	}
	if (hitTip) {
		
		int	deltaX = canvasToSize(pg, floor(mouseX) - pg->canvas2Width / 2, displayMode) + pg->canvas2xOffset - setting[4 * j + 2];
		int	deltaY = canvasToSize(pg, pg->canvas2Height / 2 - floor(mouseY), displayMode) + pg->canvas2yOffset - setting[4 * j + 3];
		
		// check for delta detents
		if (((int)setting[0] & 128) &&
			(input[2].drivenByComponent != -1) &&
			(input[3].drivenByComponent != -1) &&
			(input[2].value.size() == input[3].value.size())) {
			for (int z = 0; z < input[2].value.size(); z++)
				if ((abs(deltaX - input[2].value[z]) <= close) &&
					(abs(deltaY - input[3].value[z]) <= close)) {
					deltaX = input[2].value[z];
					deltaY = input[3].value[z];
					break;
				}
		}
		setting[4 * j + 4] = deltaX;
		setting[4 * j + 5] = deltaY;
	}

	method2(pg);

	return true;
}

void Canvas_Vector_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // vectors

	setting[0] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0) +
		(boolValues[2] ? 4 : 0) + (boolValues[3] ? 8 : 0) +
		(boolValues[4] ? 16 : 0) + (boolValues[5] ? 32 : 0) +
		(boolValues[6] ? 64 : 0) + (boolValues[7] ? 128 : 0) + (boolValues[8] ? 256 : 0);
	displayMode = boolValues[9];

	onLoad(pg);
}
void doubleClick2dVector(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Vectors:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1] };

	vector<string> checkLabels{ "Show the line", "Show the base circle", "Show the base number", 
		"Show the tip circle", "Show the tip number", "Show the base coordinates", 
		"Show the tip delta", "Enable Detent Input", "Enable Color Inputs", "Ignore Display Scale"};
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[0] & 1), (bool)((int)pg->dgrm.cL[i]->setting[0] & 2),
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 4), (bool)((int)pg->dgrm.cL[i]->setting[0] & 8),
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 16), (bool)((int)pg->dgrm.cL[i]->setting[0] & 32),
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 64), (bool)((int)pg->dgrm.cL[i]->setting[0] & 128),
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 256),
		(bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>the2dVectorButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dVector) };

Canvas_Vector_Component::Canvas_Vector_Component()
	: Component(L"2dVector", L"2D Vector", squareShape, 0, 1, 4, 1)
{
	dialogButtonList = the2dVectorButtonList;
	setRightLabels(3, "X", "Y", "dX", "dY");
	setSetting(0, 7);	// option code: 1 draw arrow, 2 draw base, 4 draw tip
	setSetting(1, 1);	// number of Vectors to place
	setSetting(2, 20);	// X[0]
	setSetting(3, 20);	// Y[0]
	setSetting(4, 100);	// dX[0]
	setSetting(5, 100);	// dY[0]
	setMemory(0, -1);
	//getInternals = getInternals_1up;
	//setInternals = setInternals_1up_n4out;
	completionTestDoubles = 1;
	completionTestChecks = 10;
	setOutputDimension(0, 0, 1);
	setOutputDimension(1, 0, 1);
	setOutputDimension(2, 0, 1);
	setOutputDimension(3, 0, 1);

}


// ************************************ 2D Text *************************************************

bool Canvas_TextBlock_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	int lineCount = 0;
	int lineX;
	int lineY;
	int spaceWidth = 0;
	string rawText;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & bitSetArray[0]) { // x and y
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}
		
		double inputWidth = 100;
		double inputHeight = 100;
		if ((int)setting[0] & bitSetArray[1]) { // width and height
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 100);
			if ((inputWidth <= 0) || (inputHeight <= 0))
				continue;
		}
		
		double hAnchor = 0;
		double vAnchor = 0;
		if ((int)setting[0] & bitSetArray[2]) { // Anchors
			hAnchor = getLoopingValueD(pin++, k, 0);
			vAnchor = getLoopingValueD(pin++, k, 0);
		}
		
		int horizJust = -1;
		int vertJust = -1;
		if ((int)setting[0] & bitSetArray[3]) { // anchors
			horizJust = getLoopingValueD(pin++, k, -1);
			vertJust = getLoopingValueD(pin++, k, -1);
		}
		
		int frameWidth = 0;
		if ((int)setting[0] & bitSetArray[4]) { // frame width
			frameWidth = getLoopingValueD(pin++, k, 0);
			if (frameWidth <= 0)
				frameWidth = 0;
		}
		
		int frameRed = 0;
		int frameGreen = 0;
		int frameBlue = 0;
		if ((int)setting[0] & bitSetArray[5]) { // frame color
			frameRed = getLoopingValueD(pin++, k, 0);
			frameGreen = getLoopingValueD(pin++, k, 0);
			frameBlue = getLoopingValueD(pin++, k, 0);
		}
		
		string rawText;
		wstring wideText;
		if ((int)setting[0] & bitSetArray[6]) { // text
			rawText = getLoopingStringValueD(pin++, k, "");
			if (rawText.size())
				wideText = decodeUTF8(rawText);
		}
		
		double inputTextSize = 15;
		if ((int)setting[0] & bitSetArray[7]) { // text size
			inputTextSize = getLoopingValueD(pin++, k, 10);
		}
		
		int textRed = 0;
		int textGreen = 0;
		int textBlue = 0;
		COLORREF textColor = 0;
		if ((int)setting[0] & bitSetArray[8]) { // text color
			textRed = getLoopingValueD(pin++, k, 0);
			textGreen = getLoopingValueD(pin++, k, 0);
			textBlue = getLoopingValueD(pin++, k, 0);
			textColor = RGB(textRed, textGreen, textBlue);
		}
		
		bool fillIt = false;
		int fillRed;
		int fillGreen;
		int fillBlue;
		if ((int)setting[0] & bitSetArray[9]) { // fill color
			if ((input[pin].drivenByComponent >= 0)     ||
				(input[pin + 1].drivenByComponent >= 0) ||
				(input[pin + 2].drivenByComponent >= 0)) {
				fillIt = true;
				fillRed = getLoopingValueD(pin++, k, 0);
				fillGreen = getLoopingValueD(pin++, k, 0);
				fillBlue = getLoopingValueD(pin++, k, 0);
			}
			else
				pin += 3;
		}

		double alpha = 1;
		if ((int)setting[0] & bitSetArray[10]) { // alpha
			alpha = getLoopingValueD(pin++, k, 1);
			if (alpha <= 0)
				continue;
		}
		
		int outputPin = 0;
		int linesOutPin = -1;
		if ((int)setting[0] & bitSetArray[11])  // lines out
			linesOutPin = outputPin++;

		int bottomLinePin = -1;
		if ((int)setting[0] & bitSetArray[12])  // bottom line out
			bottomLinePin = outputPin++;
		
		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		int textSize = sizeTo2dCanvas(pg, inputTextSize, displayMode);

		inputWidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
		if ((inputWidth <= 0) || (inputWidth > 10000))
			continue;
		inputHeight = sizeTo2dCanvas(pg, inputHeight, displayMode);
		if ((inputHeight <= 0) || (inputHeight > 10000))
			continue;

		xCenter -= hAnchor * inputWidth / 2;   // offset to horiz anchor
		yCenter += vAnchor * inputHeight / 2;  // offset to vert anchor

		// first paint the frame
		if ((frameWidth > 0) || (fillIt)) {
			BeginPath(pg->hDC2);
			Rectangle(pg->hDC2, xCenter - inputWidth / 2, yCenter - inputHeight / 2, xCenter + inputWidth / 2, yCenter + inputHeight / 2);
			EndPath(pg->hDC2);

			if (frameWidth || fillIt) {
				HPEN hPen = NULL;
				HBRUSH fillColor = NULL;

				if (frameWidth) {
					int lineWidth = sizeTo2dCanvas(pg, frameWidth, displayMode);
					hPen = CreatePen(PS_COSMETIC, lineWidth, RGB(frameRed, frameGreen, frameBlue));
					SelectObject(pg->hDC2, hPen);
				}
				if (fillIt) {
					fillColor = CreateSolidBrush(RGB(fillRed, fillGreen, fillBlue));// inputAlpha);
					SelectObject(pg->hDC2, fillColor);
				}
				if (frameWidth && fillIt)
					StrokeAndFillPath(pg->hDC2);
				else if (frameWidth)
					StrokePath(pg->hDC2);
				else if (fillIt)
					FillPath(pg->hDC2);

				if (frameWidth)
					DeleteObject(hPen);
				if (fillIt)
					DeleteObject(fillColor);
			}
		}
		
		lineX = xCenter - inputWidth / 2 + frameWidth/2;
		lineY = yCenter - inputHeight / 2 + frameWidth / 2;
		spaceWidth = 0;

		if (k == 0) {
			lineCount = 0;
			spaceWidth = calcStringWidthFontSize(pg->hDC2, textSize, " ");
			if (spaceWidth == 0)
				return false;
			if (height < textSize + 5)
				return false;
		}
		inputWidth -= 8 + frameWidth + spaceWidth;
		inputHeight -= 38 + frameWidth;

		if (!wideText.empty()) {
			bool hasFont = false;
			if (literal.size())
				hasFont = true;
				
			// ok put in line by line - whole words only
			int startOfWord = 0;
			int endOfWord;
			int currentTextWidth = 0;
			int newWordWidth;
			wstring currentText;
			wstring newWord;
			do {
				endOfWord = wideText.find_first_of(' ', startOfWord);
				if (endOfWord == -1) {
					//if ((startOfWord == 0) && (wideText.size() > 0))
					//	currentText = wideText;
					//break;
					endOfWord = wideText.size();
				}
				newWord = wideText.substr(startOfWord, endOfWord - startOfWord);
				newWordWidth = calcWideStringWidthFontSize(pg->hDC2, textSize, newWord);
				if (currentTextWidth + newWordWidth > inputWidth) { // can't fit - print current line now

					int xxx;
					if (horizJust < 0) // left just
						xxx = lineX + 3;
					if (horizJust == 0)// center
						xxx = xCenter - currentTextWidth / 2;
					if (horizJust > 0) // right just
						xxx = xCenter + inputWidth / 2 - currentTextWidth - 4;
					if( hasFont )
						drawWideTextString(pg->hDC2, xxx, lineY, textSize, textColor, literal[0], currentText);
					else
						drawWideTextString(pg->hDC2, xxx, lineY, textSize, textColor, currentText);
					currentText = newWord;
					currentTextWidth = newWordWidth;
					
					// advance to the next line
					lineCount++;
					lineY += 1.1 * textSize; // leave a little room for super and subscripts
					int g = 1.2 * textSize - 15;
					if (lineY + g > yCenter + inputHeight / 2 - frameWidth / 2)
						break;
				}
				else { // still room - add new word to line
					if (currentText.size()) {
						currentText += L" ";
						currentTextWidth += spaceWidth;
					}
					currentText += newWord;
					currentTextWidth += newWordWidth;
				}
				
				startOfWord = endOfWord + 1;
			}
			while (startOfWord < wideText.size());

			int g = 1.2 * textSize - 15;
			if (lineY + g <= yCenter + inputHeight / 2 - frameWidth / 2) {
				if (currentText.size()) { // push out the last line
					int xxx;
					if (horizJust < 0) // left just
						xxx = lineX + 3;
					if (horizJust == 0)// center
						xxx = xCenter - currentTextWidth / 2;
					if (horizJust > 0) // right just
						xxx = xCenter + inputWidth / 2 - currentTextWidth - 4;
					SetBkMode(pg->hDC2, TRANSPARENT);
					if (hasFont)
						drawWideTextString(pg->hDC2, lineX + 3, lineY, textSize, textColor, literal[0], currentText);
					else
						drawWideTextString(pg->hDC2, lineX + 3, lineY, textSize, textColor, currentText);
					currentText.clear();
					currentTextWidth = 0;
				}
			}
		}
		else
			lineY += textSize * 1.1; // no text just feed the line
	
		if (linesOutPin != -1)
			output[linesOutPin].value[k] = lineCount;
		if (bottomLinePin != -1)
			output[bottomLinePin].value[k] = canvasToY2(pg, lineY - 0.75 * textSize, displayMode);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

void doubleClick2dDisplayFont(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildValueFontDialogButtons(pg, i);
}
std::vector<Button*>the2dTextButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayMode),
	new Button(3, 80, 85, 20, L"Font...", doubleClick2dDisplayFont) };

bool Canvas_TextBlock_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
int Canvas_TextBlock_Component::method2(Sheet* pg) {
	
	return 3;
}
vector<int> Canvas_TextBlock_Component::GetPinGroups() { return vector<int>{2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 1 }; }
vector<string> Canvas_TextBlock_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",									// 0=Position    (1)
		"Width", "Height",							// 1=size        (2)
		"Horiz Anchor", "Vert Anchor",				// 3=Anchors     (4)
		"Horiz Justify", "Vert Justify",			// 3=justify     (8)
		"Frame Width",								// 4=frame width (16)
		"Frame Red", "Frame Green", "Frame Blue",	// 5=frame color (32)
		"Text",										// 6=text        (64)
		"Text Size",								// 7=text size   (128)
		"Text Red", "Text Green", "Text Blue",		// 8=text color  (256)
		"Fill Red", "Fill Green", "Fill Blue",		// 9=fill color  (512)
		"Alpha" };
}
Canvas_TextBlock_Component::Canvas_TextBlock_Component()
	: Component(L"2dText", L"2D Text", squareShape, 5, 1, 0, 1)
{
	dialogButtonList = the2dTextButtonList;
	setLeftLabels(3, "X", "Y", "Width", "Height", "Text");
	setSetting(0, 1 + 2 + 64);
	pinMaskIndex = 0;
};

// ********************************** Parabola ********************************************

bool Canvas_Parabola_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputWidth = 50;
		double inputFocus = 10;
		if ((int)setting[0] & 2) {
			inputWidth = getLoopingValueD(pin++, k, 50);
			inputFocus = getLoopingValueD(pin++, k, 10);
			if (inputFocus == 0)
				continue;
		}

		double angle = 0;
		if ((int)setting[0] & 4) {
			angle = getLoopingValueD(pin++, k, 0);
			angle = rotateInputAngle(angle);
		}

		double inputLineWidth = 1;
		if ((int)setting[0] & 8) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 16) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		bool enableFill = false;
		if ((int)setting[0] & 32) {
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
			enableFill = true;
		}

		double inputAlpha = 1.0;
		if ((int)setting[0] & 64) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if ((inputAlpha <= 0)) // && (gradient == null))
				continue;
		}

		int x = xTo2dCanvas2(pg, inputX, displayMode);
		int y = yTo2dCanvas2(pg, inputY, displayMode);

		double sinA = sin(angle);
		double cosA = cos(angle);


		BeginPath(pg->hDC2);
		if (inputWidth > 5000)
			continue;
		for (int px = -inputWidth; px < inputWidth; px += 1) {

			double py = px * px / (4.0 * inputFocus);

			int rx = px * cosA - py * sinA;
			int ry = px * sinA + py * cosA;

			int xx = x + rx;
			int yy = y - ry;

			if (px == -inputWidth)
				MoveToEx(pg->hDC2, xx, yy, NULL);
			else
				LineTo(pg->hDC2, xx, yy);
		}
		EndPath(pg->hDC2);

		if (enableFill || inputLineWidth) {
			HBRUSH fillColor = NULL;
			HPEN hPen = NULL;

			if (enableFill) {
				fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));// inputAlpha);
				SelectObject(pg->hDC2, fillColor);
			}
			if (inputLineWidth) {
				int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				hPen = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
				SelectObject(pg->hDC2, hPen);
			}
			if (enableFill && inputLineWidth)
				StrokeAndFillPath(pg->hDC2);
			else if (enableFill)
				FillPath(pg->hDC2);
			else if (inputLineWidth)
				StrokePath(pg->hDC2);

			if (enableFill)
				DeleteObject(fillColor);
			if (inputLineWidth)
				DeleteObject(hPen);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}


std::vector<Button*>parabButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral) };

bool Canvas_Parabola_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}


vector<int> Canvas_Parabola_Component::GetPinGroups() { return vector<int>{2, 2, 1, 1, 3, 3, 1 }; }
vector<string> Canvas_Parabola_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Width", "Focus",
		"Rotation",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Fill Red", "Fill Green", "Fill Blue",
		"Alpha"};
}
Canvas_Parabola_Component::Canvas_Parabola_Component()
	: Canvas_Component(L"PARAB", L"2D Parabola", squareShape, 6, 1, 0, 0)
{
	dialogButtonList = parabButtonList;
	setSetting(0, 15); // 1 + 2 + 4 + 8 = 15
	setSetting(1, 0); // dash
	pinMaskIndex = 0;
}

// ****************************** Ellipse ****************************************

std::vector<Button*>ellipseButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayModeAndDash),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral) };

bool Canvas_EllipseOnCenter_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	//var gradient = null;
	//if (setting[0] & 256) { // check for gradient
	//	var loops = inputsToLoopsN(pg, i, leftTerminalCount - 2);
	//	if (loops == 0)
	//		return false;
	//	gradient = getGradient(pg, i, leftTerminalCount - 2);
	//}
	//else {
		int loops = inputsToLoops();
		if (loops == 0)
			return false;
	//}

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}
		
		double inputMajor = 20;
		double inputMinor = 10;
		if ((int)setting[0] & 2) {
			inputMajor = getLoopingValueD(pin++, k, 20);
			inputMinor = getLoopingValueD(pin++, k, 10);
		}
		
		double inputArcBegin = 0;
		double inputArcEnd = TWO_PI;
		if ((int)setting[0] & 4) {
			inputArcBegin = getLoopingValueD(pin++, k, 0);
			inputArcEnd = getLoopingValueD(pin++, k, TWO_PI);
			if (inputArcBegin > inputArcEnd)
				continue;
		}
		
		double inputAngle = 0;
		if ((int)setting[0] & 8) {
			inputAngle = getLoopingValueD(pin++, k, 0);
			inputAngle = rotateInputAngle(inputAngle);
		}

		double inputLineWidth = 1;
		if ((int)setting[0] & 16) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
		}
		
		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 32) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}
		
		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		bool enableFill = false;
		if ((int)setting[0] & 64) {
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
			enableFill = true;
		}

		double inputAlpha = 1;
		if ((int)setting[0] & 128) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0) //&& (gradient == null))
				continue;
		}
		
		int x = xTo2dCanvas2(pg, inputX, displayMode);
		int y = yTo2dCanvas2(pg, inputY, displayMode);
		double major = inputMajor;// sizeTo2dCanvas(pg, inputMajor, displayMode);
		double minor = inputMinor;// sizeTo2dCanvas(pg, inputMinor, displayMode);

		double sinA = sin(inputAngle);
		double cosA = cos(inputAngle);

		bool closePath = false;
		if ((inputArcBegin == 0) &&
			(inputArcEnd == TWO_PI))
			closePath = true;
		int startX = 0;
		int startY = 0;

		if (inputArcEnd > 20)
			continue;
		if (inputArcEnd < -20)
			continue;
		if (inputArcBegin > 20)
			continue;
		if (inputArcBegin < -20)
			continue;
		// generate the ellipse
		BeginPath(pg->hDC2);
		for (double n = inputArcBegin; n <= inputArcEnd; n += 0.1) {
			double px = minor * cos(n);
			double py = major * sin(n);

			double rx = px * cosA - py * sinA;
			double ry = px * sinA + py * cosA;

			int xx = x + sizeTo2dCanvas(pg, rx, displayMode);
			int yy = y - sizeTo2dCanvas(pg, ry, displayMode);

			if (n == 0) {
				MoveToEx(pg->hDC2, xx, yy, NULL);
				startX = xx;
				startY = yy;
			}
			else
				LineTo(pg->hDC2, xx, yy);
		}
		if( closePath )
			LineTo(pg->hDC2, startX, startY);
		EndPath(pg->hDC2);

		if (enableFill || inputLineWidth) {
			HBRUSH fillColor = NULL;
			HPEN hPen = NULL;

			if (enableFill) {
				fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));// inputAlpha);
				SelectObject(pg->hDC2, fillColor);
			}
			if (inputLineWidth) {
				int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				hPen = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
				SelectObject(pg->hDC2, hPen);
			}
			if (enableFill && inputLineWidth)
				StrokeAndFillPath(pg->hDC2);
			else if (enableFill)
				FillPath(pg->hDC2);
			else if (inputLineWidth)
				StrokePath(pg->hDC2);

			if (enableFill)
				DeleteObject(fillColor);
			if (inputLineWidth)
				DeleteObject(hPen);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

bool Canvas_EllipseOnCenter_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}
vector<int> Canvas_EllipseOnCenter_Component::GetPinGroups() { return vector<int>{2, 2, 2, 1, 1, 3, 3, 1}; }
vector<string> Canvas_EllipseOnCenter_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Major", "Minor",
		"Arc Start", "Arc End",
		"Rotation",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Fill Red", "Fill Green", "Fill Blue",
		"Alpha"};
}
Canvas_EllipseOnCenter_Component::Canvas_EllipseOnCenter_Component()
	: Canvas_Component(L"ELIPS", L"2D Ellipse on Center", squareShape, 6, 1, 0, 0)
{
	dialogButtonList = ellipseButtonList;
	setSetting(0, 27); // 1 + 2 + 8 + 16 = 27
	setSetting(1, 0);
	pinMaskIndex = 0;
};

// **************** Ellipse 2 *******************

bool Canvas_EllipseOnFocus_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	//var gradient = null;
	//if (setting[0] & 256) { // check for gradient
	//	var loops = inputsToLoopsN(pg, i, leftTerminalCount - 2);
	//	if (loops == 0)
	//		return false;
	//	gradient = getGradient(pg, i, leftTerminalCount - 2);
	//}
	//else {
		int loops = inputsToLoops();
		if (loops == 0)
			return false;
	//}

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}
		
		double inputMajor = 20;
		double inputMinor = 10;
		if ((int)setting[0] & 2) {
			inputMajor = getLoopingValueD(pin++, k, 20);
			inputMinor = getLoopingValueD(pin++, k, 10);
		}
		if (inputMajor < inputMinor)
			continue;

		double inputArcBegin = 0;
		double inputArcEnd = TWO_PI;
		if ((int)setting[0] & 4) {
			inputArcBegin = getLoopingValueD(pin++, k, 0);
			inputArcEnd = getLoopingValueD(pin++, k, TWO_PI);
			if (inputArcBegin > inputArcEnd)
				continue;
		}

		double inputAngle = 0;
		if ((int)setting[0] & 8) {
			inputAngle = getLoopingValueD(pin++, k, 0);
			inputAngle = rotateInputAngle(inputAngle);
		}

		double inputLineWidth = 1;
		if ((int)setting[0] & 16) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 32) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		bool enableFill = false;
		if ((int)setting[0] & 64) {
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
			enableFill = true;
		}

		double inputAlpha = 1;
		if ((int)setting[0] & 128) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0) //  && (gradient == null))
				continue;
		}

		//if (inputMajor < inputMinor)
		//	continue;

		double major = inputMajor;// * canvas2dScale;
		double minor = inputMinor;// * canvas2dScale;
		double c = sqrt(inputMajor * inputMajor - inputMinor * inputMinor);
		double cx = inputX + c * sin(-inputAngle);
		double cy = inputY + c * cos(-inputAngle);
		int x = xTo2dCanvas2(pg, cx, displayMode);
		int y = yTo2dCanvas2(pg, cy, displayMode);

		double sinA = sin(inputAngle);
		double cosA = cos(inputAngle);

		bool closePath = false;
		if ((inputArcBegin == 0) &&
			(inputArcEnd == TWO_PI))
			closePath = true;
		int startX = 0;
		int startY = 0;

		if (inputArcEnd > 20)
			continue;
		if (inputArcEnd < -20)
			continue;
		if (inputArcBegin > 20)
			continue;
		if (inputArcBegin < -20)
			continue;

		// generate the ellipse
		BeginPath(pg->hDC2);
		for (double n = inputArcBegin; n <= inputArcEnd; n += 0.1) {
			double px = minor * cos(n);
			double py = major * sin(n);

			double rx = px * cosA - py * sinA;
			double ry = px * sinA + py * cosA;

			int xx = x + sizeTo2dCanvas(pg, rx, displayMode);
			int yy = y - sizeTo2dCanvas(pg, ry, displayMode);

			if (n == 0) {
				MoveToEx(pg->hDC2, xx, yy, NULL);
				startX = xx;
				startY = yy;
			}
			else
				LineTo(pg->hDC2, xx, yy);
		}
		if (closePath)
			LineTo(pg->hDC2, startX, startY);
		EndPath(pg->hDC2);

		if (enableFill || inputLineWidth) {
			HBRUSH fillColor = NULL;
			HPEN hPen = NULL;

			if (enableFill) {
				fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));// inputAlpha);
				SelectObject(pg->hDC2, fillColor);
			}
			if (inputLineWidth) {
				int lineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				hPen = CreatePen(setting[1], lineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
				SelectObject(pg->hDC2, hPen);
			}
			if (enableFill && inputLineWidth)
				StrokeAndFillPath(pg->hDC2);
			else if (enableFill)
				FillPath(pg->hDC2);
			else if (inputLineWidth)
				StrokePath(pg->hDC2);

			if (enableFill)
				DeleteObject(fillColor);
			if (inputLineWidth)
				DeleteObject(hPen);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}
bool Canvas_EllipseOnFocus_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	buildPinLabels();
	return false;
}
vector<int> Canvas_EllipseOnFocus_Component::GetPinGroups() { return vector<int>{2, 2, 2, 1, 1, 3, 3, 1 }; }
vector<string> Canvas_EllipseOnFocus_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Major", "Minor",
		"Arc Start", "Arc End",
		"Rotation",
		"Line Width",
		"Line Red", "Line Green", "Line Blue",
		"Fill Red", "Fill Green", "Fill Blue",
		"Alpha"};
}
Canvas_EllipseOnFocus_Component::Canvas_EllipseOnFocus_Component()
	: Canvas_Component(L"ELIPS2", L"2D Ellipse on Focus", squareShape, 6, 1, 0, 0)
{
	dialogButtonList = ellipseButtonList;
	setSetting(0, 27); // 1 + 2 + 8 + 16 = 27
	setSetting(1, 0);
	pinMaskIndex = 0;
}

// ******************************** 2d Sliders ********************************************




	// ANGLED SLIDER --------------------------------

int Canvas_AngledSlider_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);

	// this is the slide constant - send setting out
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	
	int oldSize = setting.size();
	setting.resize(4 + loops);
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		if (oldSize <= 4 + k)
			setting[4 + k] = setting[4 + k - 1]; // K NEVER ZERO
		output[0].value[k] = detent(3, 4 + k); //setting[4 + k];
	}

	output[0].dimensions[0] = loops;
	return 3;
}

bool Canvas_AngledSlider_Component::dial2d(Sheet* pg) // Angled version,
{	
	if (pg->pCanvasWindow == NULL)
		return false;

	bool color = false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;
	for (int k = 0; k < loops; k++) {
		double inputX = getLoopingValueD(0, k, 0);
		double inputY = getLoopingValueD(1, k, 0);
		double inputL = getLoopingValueD(2, k, 100);
		double inputA = getLoopingValueD(3, k, 0);
		double inputR = getLoopingValueD(4, k, DES_KNOB_SIZE);
		if (inputL <= 0)
			continue;
		if (inputR <= 0)
			continue;

		COLORREF lineColor = 0x000000;
		COLORREF blackColor = 0x000000;
		COLORREF whiteColor = 0xFFFFFF;
		if (color) {
			int inputRed = getLoopingValueD(5, k, 0);
			int inputGreen = getLoopingValueD(6, k, 0);
			int inputBlue = getLoopingValueD(7, k, 0);
			int inputAlpha = getLoopingValueD(8, k, 1);
			lineColor = RGB(inputRed, inputGreen, inputBlue);// , inputAlpha);
			blackColor = RGB(0, 0, 0);
			whiteColor = RGB(255, 255, 255);
		}

		inputL = sizeTo2dCanvas(pg, inputL, displayMode);
		inputR = sizeTo2dCanvas(pg, inputR, displayMode);

		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		int topX = xCenter + inputL / 2 * sin(inputA);
		int topY = yCenter - inputL / 2 * cos(inputA);
		int botX = xCenter - inputL / 2 * sin(inputA);
		int botY = yCenter + inputL / 2 * cos(inputA);

		double detVal = (detent(3, 4 + k) - setting[0]) /
			(setting[1] - setting[0]);
		int knobX = detVal * (topX - botX) + botX;
		int knobY = detVal * (topY - botY) + botY;

		if ((int)setting[2] & 1) // line
		{
			// the track
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, topX, topY, NULL);
			LineTo(pg->hDC2, botX, botY);
			EndPath(pg->hDC2);

			HPEN hPen = CreatePen(PS_COSMETIC, 1, lineColor);
			SelectObject(pg->hDC2, hPen);
			StrokePath(pg->hDC2);
			DeleteObject(hPen);
		}
		if ((int)setting[2] & 2) // circle
		{
			// then the knob
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, knobX - inputR, knobY - inputR, knobX + inputR, knobY + inputR);
			EndPath(pg->hDC2);
			
			//ctx2.fillStyle = whiteColor;  // white
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			StrokeAndFillPath(pg->hDC2);
		}
		int oldX;
		int oldY;
		if (k > 0)
			if ((int)setting[2] & 8) // graph
			{
				// the graph
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, oldX, oldY, NULL);
				LineTo(pg->hDC2, knobX, knobY);
				EndPath(pg->hDC2);
				HPEN hPen = CreatePen(PS_COSMETIC, 1, lineColor);
				SelectObject(pg->hDC2, hPen);
				StrokePath(pg->hDC2);
			}
		oldX = knobX;
		oldY = knobY;
		//ctx2.font = '9px ' + default_2D_FontFamily;
		// if there is a user label put below
		if ((int)userLabel.size()) {
			int textWidth = calcLargeStringWidthFont(pg->hDC2, userLabel);
			drawLargeTextString(pg->hDC2, xCenter - textWidth / 2, max(botY, topY) + inputR + 2, blackColor, userLabel );
		}
		if ((int)setting[2] & 4) //number
		{
			// then the value at the top
			string value = tomString(detent(3, 4 + k));
			int textWidth = calcLargeStringWidthFont(pg->hDC2, value);
			drawLargeTextString(pg->hDC2, xCenter- textWidth /2, min(topY, botY) - inputR - 12 - 3, blackColor, value);
		}

		// the Halo
		//if ((int)memory[0] == k) {
		//	ctx2.beginPath();
		//	ctx2.arc(knobX, knobY, DES_HALO_SIZE, 0, TWO_PI);
		//	ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	ctx2.fill();
		//}
		//// the touch area - DIAGNOSTIC
		//if (DES_TOUCH_ALPHA) {
		//	int touchSize = max(DES_TOUCH_SIZE, inputR);
		//	ctx2.beginPath();
		//	ctx2.arc(knobX, knobY, touchSize, 0, TWO_PI);
		//	ctx2.fillStyle = RGB(0, 255, 0);// , DES_TOUCH_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}


bool Canvas_AngledSlider_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) { // Angled version
	int loops = inputsToLoops();
	if (loops == 0)
		return false;
	if ((int)setting.size() > 4 + loops)
		setting.resize( 4 + loops);

	int k;
	double inputX = 0;
	double inputY = 0;
	double inputL = 100;
	double inputA = 0;
	double inputR = DES_KNOB_SIZE;
	double alpha = 1;
	int xCenter;
	int yCenter;

	int topX;
	int topY;
	int botX;
	int botY;

	if (check) {
		for (k = loops - 1; k >= 0; k--) { // scan backwards
			// convert to pixels
			inputX = getLoopingValueD(0, k, 0);
			inputY = getLoopingValueD(1, k, 0);
			inputL = getLoopingValueD(2, k, 100);
			inputA = getLoopingValueD(3, k, 0);
			inputR = getLoopingValueD(4, k, DES_KNOB_SIZE);

			if (inputL <= 0)
				continue;
			if (inputR <= 0)
				continue;

			alpha = 1;
			if (type == L"2dClrSldAZ") {
				alpha = getLoopingValueD(8, k, 1);
				if (alpha <= 0)
					continue;
			}

			inputL = sizeTo2dCanvas(pg, inputL, displayMode);
			inputR = sizeTo2dCanvas(pg, inputR, displayMode);
			int touchSize = max(DES_TOUCH_SIZE, inputR);

			xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			topX = xCenter + inputL / 2 * sin(inputA);
			topY = yCenter - inputL / 2 * cos(inputA);
			botX = xCenter - inputL / 2 * sin(inputA);
			botY = yCenter + inputL / 2 * cos(inputA);

			double detVal = (detent(3, 4 + k) - setting[0]) /
				(setting[1] - setting[0]);
			int knobX = detVal * (topX - botX) + botX;
			int knobY = detVal * (topY - botY) + botY;

			if (mouseY > knobY + touchSize)
				continue; // missed dial
			if (mouseY < knobY - touchSize)
				continue; // missed dial

			if (mouseX > knobX + touchSize)
				continue; // missed knob
			if (mouseX < knobX - touchSize)
				continue; // missed knob

			memory[0] = k;
			return true;
		}
		return false;
	}

	k = memory[0];

	inputX = getLoopingValueD(0, k, 0);
	inputY = getLoopingValueD(1, k, 0);
	inputL = getLoopingValueD(2, k, 100);
	inputA = getLoopingValueD(3, k, 0);
	if (inputL <= 0)
		return false;
	xCenter = xTo2dCanvas2(pg, inputX, displayMode);
	yCenter = yTo2dCanvas2(pg, inputY, displayMode);
	inputL = sizeTo2dCanvas(pg, inputL, displayMode);

	topX = xCenter + inputL / 2 * sin(inputA);
	topY = yCenter - inputL / 2 * cos(inputA);
	botX = xCenter - inputL / 2 * sin(inputA);
	botY = yCenter + inputL / 2 * cos(inputA);

	//   var scale = inputL / (setting[1] - setting[0]);

	if (abs(topX - botX) > abs(topY - botY)) {   // use the X direction
		double scale = (topX - botX) / (setting[1] - setting[0]);
		// knob position to setting
		//setting[4 + k] = overScale(mouseX - botX, scale) + setting[0];
		setting[4 + k] = ((mouseX - botX) / scale) + setting[0];
	}
	else {  // use the Y direction
		double scale = (topY - botY) / (setting[1] - setting[0]);
		// knob position to setting
		//setting[4 + k] = overScale(mouseY - botY, scale) + setting[0];
		setting[4 + k] = ((mouseY - botY) / scale) + setting[0];
	}

	if ((int)setting[0] < setting[1]) {
		if ((int)setting[4 + k] < setting[0])
			setting[4 + k] = setting[0];
		if ((int)setting[4 + k] > setting[1])
			setting[4 + k] = setting[1];
	}
	else {
		if ((int)setting[4 + k] > setting[0])
			setting[4 + k] = setting[0];
		if ((int)setting[4 + k] < setting[1])
			setting[4 + k] = setting[1];
	}
	
	method2(pg);
	return true;
}




void Canvas_AngledSlider_Component::onMouseUp2d(Sheet* pg) {

	memory[0] = -1; // clear the halo

	if (((int)setting[2] & 16) != 0)
		if (setting[0] <= 0)
			if (setting[1] >= 0) {
				int loops = inputsToLoops();
				for (int k = 0; k < loops; k++) {
					output[0].value[k] = 0;
					setting[4 + k] = 0;
				}
				// method2d(pg,i); // dial it
			}
	method2(pg); // dial it
	return;
}

Canvas_AngledSlider_Component::Canvas_AngledSlider_Component()
	: Component(L"2dSldAZ", L"2D Angled Slider", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = the2dSliderButtonList;
	setLeftLabels(3, "X", "Y", "Length", "Angle", "Radius");
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 7);      // option code: 1 draw track, 2 draw knob, 4 draw number, 8 graph, 16 spring back
	setSetting(3, 0);      // detent
	setSetting(4, 0);     // output 4 and up
	setMemory(0, 0);
	//getInternals = getInternals_4up;
	//setInternals = setInternals_4up_1out;
}


// ===========  ARC SLIDER =============================

// inputs ("X", "Y", "Radius", "Start", "End");
// setting[0] = 0;      // Minimum
// setting[1] = 100;    // Maximum
// setting[2] = 7;      // option code: 1 draw track, 2 draw knob, 4 draw number
// setting[3] = 0;     // output 3 and up

int Canvas_ArcSlider_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);

	// this is the slide constant - send setting out
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	
	int oldSize = setting.size();
	setting.resize(4 + loops);
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		double inputS = getLoopingValueD(3, k, 0);            // Start
		double inputE = getLoopingValueD(4, k, PI_OVER_2);    // End

		if (oldSize <= 4 + k)
			setting[4 + k] = setting[4 + k - 1]; // k NEVER ZERO
		if (inputS > inputE)
			output[0].value[k] = setting[0] + setting[1] - detent(3, 4 + k);//setting[4 + k];
		else
			output[0].value[k] = detent(3, 4 + k);//setting[4 + k];
	}
	output[0].dimensions[0] = loops;

	return 3;
}

bool Canvas_ArcSlider_Component::dial2d(Sheet* pg) // Arc version,
{
	if (pg->pCanvasWindow == NULL)
		return false;

	bool color = false;
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		double inputX = getLoopingValueD(0, k, 0);
		double inputY = getLoopingValueD(1, k, 0);
		double inputA = getLoopingValueD(2, k, 100);        // Arc
		double inputS = getLoopingValueD(3, k, 0);          // Start
		double inputE = getLoopingValueD(4, k, PI_OVER_2);  // End
		int inputR = getLoopingValueD(5, k, DES_KNOB_SIZE);          // Radius

		COLORREF lineColor = 0x000000;
		COLORREF blackColor = 0x000000;
		COLORREF whiteColor = 0xFFFFFF;
		if (color) {
			int inputRed = getLoopingValueD(6, k, 0);
			int inputGreen = getLoopingValueD(7, k, 0);
			int inputBlue = getLoopingValueD(8, k, 0);
			double inputAlpha = getLoopingValueD(9, k, 1);
			lineColor = RGB(inputRed, inputGreen, inputBlue);// , inputAlpha);
			blackColor = 0x000000;
			whiteColor = 0xFFFFFF;
		}
		

		// check for CCW operation
		bool CCW = false;
		if (inputS > inputE) {
			CCW = true;
			double temp = inputE;
			inputE = inputS;
			inputS = temp;
		}

		double range = abs(inputE - inputS);
		if (range > TWO_PI)
			continue;
		if (range <= 0)
			continue;

		if (inputA <= 0)
			continue;
		if (inputR <= 0)
			continue;

		inputA = sizeTo2dCanvas(pg, inputA, displayMode);
		inputR = sizeTo2dCanvas(pg, inputR, displayMode);

		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		double knobA = (detent(3, 4 + k) - setting[0]) * (inputE - inputS) /
			(setting[1] - setting[0]) + inputS;
		double cosAngle = cos(knobA);
		double sinAngle = sin(knobA);
		int knobX = xCenter + inputA * cosAngle;
		int knobY = yCenter + inputA * sinAngle;

		if ((int)setting[2] & 1) // track
		{
			int arcBeginX = xCenter + inputA * cos(inputS);
			int arcBeginY = yCenter + inputA * sin(inputS);
			int arcEndX = xCenter + inputA * cos(inputE);
			int arcEndY = yCenter + inputA * sin(inputE);

			BeginPath(pg->hDC2);
			Arc(pg->hDC2, xCenter - inputA, yCenter - inputA, xCenter + inputA, yCenter + inputA,
				arcEndX, arcEndY, arcBeginX, arcBeginY);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		if ((int)setting[2] & 2) // knob
		{
			//ctx2.strokeStyle = lineColor;
			//ctx2.lineWidth = 1;
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, knobX - inputR, knobY - inputR, knobX + inputR, knobY + inputR);
			
			//ctx2.fillStyle = whiteColor;  // white
			EndPath(pg->hDC2);
			StrokeAndFillPath(pg->hDC2);
		}
		int oldX;
		int oldY;
		if (k > 0)
			if ((int)setting[2] & 8) // graph
			{
				// the graph
				//ctx2.strokeStyle = lineColor;
				//ctx2.lineWidth = 1;
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, oldX, oldY, NULL);
				LineTo(pg->hDC2, knobX, knobY);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
		oldX = knobX;
		oldY = knobY;
		//ctx2.fillStyle = blackColor;
		//ctx2.font = '9px ' + default_2D_FontFamily;
		// if there is a user label put on center
		if (userLabel.size()) {
			int textWidth = calcLargeStringWidthFont(pg->hDC2, userLabel);
			drawLargeTextString(pg->hDC2, xCenter - textWidth / 2, yCenter, blackColor, userLabel );
		}
		if ((int)setting[2] & 4) // number
		{
			// then the value near the crank
			string text;
			if (CCW) {
				//draw2dLimitedWidthText(setting[0] + setting[1] - detent(3, 4 + k),
				//	knobX, knobY + 5, 40);
				text = tomString(setting[0] + setting[1] - detent(3, 4 + k));
				
			}
			else {
				//draw2dLimitedWidthText(detent(3, 4 + k),
				//	knobX, knobY + 5, 40);
				text = tomString(detent(3, 4 + k));
			}
			
			if (cosAngle > 0) { // text on the right
				knobX = xCenter + (inputA + inputR + 2) * cosAngle; 
			}else{ // text on the left
				int stringLength = calcLargeStringWidthFont(pg->hDC2, text);
				knobX = xCenter + (inputA + inputR + 2) * cosAngle - stringLength;
			}
			if (sinAngle > 0) { // text below
				knobY = yCenter + (inputA + inputR + 2) * sinAngle + 6;
			}else { // text above
				knobY = yCenter + (inputA + inputR + 2) * sinAngle - 5;
			}
			drawLargeTextString(pg->hDC2, knobX, knobY-8, blackColor, text);
			
		}

		// the Halo
		//if ((int)memory[0] == k) {
		//	ctx2.beginPath();
		//	ctx2.arc(oldX, oldY, DES_HALO_SIZE, 0, TWO_PI);
		//	ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	ctx2.fill();
		//}
		//// the touch area - DIAGNOSTIC
		//if (DES_TOUCH_ALPHA) {
		//	int touchSize = max(DES_TOUCH_SIZE, inputR);
		//	ctx2.beginPath();
		//	ctx2.arc(oldX, oldY, touchSize, 0, TWO_PI);
		//	ctx2.fillStyle = RGB(0, 255, 0);// , DES_TOUCH_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

bool Canvas_ArcSlider_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) { // Arc version
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	if ((int)setting.size() > 4 + loops)
		setting.resize(4 + loops);

	int k;
	double inputX;
	double inputY;
	double inputA;
	double inputS;
	double inputE;
	double inputR;
	double xCenter;
	double yCenter;
	double alpha;

	if (check) {
		for (k = loops - 1; k >= 0; k--) { // scan backwards
			// convert to pixels
			inputX = getLoopingValueD(0, k, 0);
			inputY = getLoopingValueD(1, k, 0);
			inputA = getLoopingValueD(2, k, 100);        // Arc
			inputS = getLoopingValueD(3, k, 0);          // Start
			inputE = getLoopingValueD(4, k, PI_OVER_2);  // End
			inputR = getLoopingValueD(5, k, DES_KNOB_SIZE);          // Radius

			// check for CCW operation
			bool CCW = false;
			if (inputS > inputE) {
				CCW = true;
				double temp = inputE;
				inputE = inputS;
				inputS = temp;
			}

			double range = abs(inputE - inputS);
			if (range > TWO_PI)
				continue;
			if (range <= 0)
				continue;

			if (inputA <= 0)
				continue;
			if (inputR <= 0)
				continue;

			alpha = 1;
			if (type == L"2dArcClrSldZ") {
				double alpha = getLoopingValueD(9, k, 1);
				if (alpha <= 0)
					continue;
			}

			inputA = sizeTo2dCanvas(pg, inputA, displayMode);
			inputR = sizeTo2dCanvas(pg, inputR, displayMode);
			double touchSize = max(DES_TOUCH_SIZE, inputR);

			xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			double knobA = (detent(3, 4 + k) - setting[0]) * (inputE - inputS) /
				(setting[1] - setting[0]) + inputS;
			double knobX = xCenter + inputA * cos(knobA);
			double knobY = yCenter + inputA * sin(knobA);

			if (mouseY > knobY + touchSize)
				continue; // missed dial
			if (mouseY < knobY - touchSize)
				continue; // missed dial

			if (mouseX > knobX + touchSize)
				continue; // missed knob
			if (mouseX < knobX - touchSize)
				continue; // missed knob

			memory[0] = k;
			return true;
		}
		return false;
	}

	k = memory[0];

	inputX = getLoopingValueD(0, k, 0);
	inputY = getLoopingValueD(1, k, 0);
	//inputA = getLoopingValueD(2, k, 100);          // Arc
	inputS = getLoopingValueD(3, k, 0);            // Start
	inputE = getLoopingValueD(4, k, PI_OVER_2);    // End

	// check for CCW operation
	//CCW = false;
	if (inputS > inputE) {
		//CCW = true;
		double temp = inputE;
		inputE = inputS;
		inputS = temp;
	}

	double range = abs(inputE - inputS);
	if (range > TWO_PI)
		return false;
	if (range <= 0)
		return false;

	xCenter = xTo2dCanvas2(pg, inputX, displayMode);
	yCenter = yTo2dCanvas2(pg, inputY, displayMode);

	double oldAngle = (setting[4 + k] - setting[0]) * (inputE - inputS) /
		(setting[1] - setting[0]) + inputS;

	double dx = mouseX - xCenter;
	double dy = mouseY - yCenter;
	double newAngle = atan2(dy, dx);

	double deltaAngle = newAngle - oldAngle;
	if (deltaAngle > PI)
		deltaAngle -= TWO_PI;
	else if (deltaAngle < -PI)
		deltaAngle += TWO_PI;

	double deltaFraction = deltaAngle / TWO_PI;
	double revolution = (setting[1] - setting[0]) * (TWO_PI / range); // value changed in one revolution
	deltaFraction *= revolution;

	setting[4 + k] += deltaFraction;

	// limit the range
	if (setting[0] < setting[1]) {
		if (setting[4 + k] < setting[0])
			setting[4 + k] = setting[0];
		if (setting[4 + k] > setting[1])
			setting[4 + k] = setting[1];
	}
	else {
		if (setting[4 + k] > setting[0])
			setting[4 + k] = setting[0];
		if (setting[4 + k] < setting[1])
			setting[4 + k] = setting[1];
	}
	
	method2(pg);
	return true;
}



Canvas_ArcSlider_Component::Canvas_ArcSlider_Component()
	: Component(L"2dArcSldZ", L"2D Arc Slider", squareShape, 6, 1, 1, 1)
{
	dialogButtonList = the2dSliderButtonList;
	setLeftLabels(3, "X", "Y", "Arc", "Start", "End", "Radius");
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 7);      // option code: 1 draw track, 2 draw knob, 4 draw number, 8 graph, 16 spring back
	setSetting(3, 0);      // detent
	setSetting(4, 0);     // output 4 and up
	setMemory(0, 0);
	//getInternals = getInternals_4up;
	//setInternals = setInternals_4up_1out;
}




// 2D KNOB --------------------------------------

//.setting[0] = 1.0;    // amount per revolution.
//.setting[1] = 7;      // option code: 1 draw track, 2 draw knob, 4 draw number
//.setting[2] = 0.0;    // output 2 +
bool Canvas_Knob_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	bool color = false;
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		double inputX = getLoopingValueD(0, k, 0);
		double inputY = getLoopingValueD(1, k, 0);
		double inputR = getLoopingValueD(2, k, 50);
		double inputK = getLoopingValueD(3, k, DES_KNOB_SIZE);

		COLORREF lineColor = 0x000000;
		COLORREF blackColor = 0x000000;
		COLORREF whiteColor = 0xFFFFFF;
		if (color) {
			int inputRed = getLoopingValueD(4, k, 0);
			int inputGreen = getLoopingValueD(5, k, 0);
			int inputBlue = getLoopingValueD(6, k, 0);
			double inputAlpha = getLoopingValueD(7, k, 1);
			lineColor = RGB(inputRed, inputGreen, inputBlue);// , inputAlpha);
			//blackColor = blackAlpha(inputAlpha);
			//whiteColor = whiteAlpha(inputAlpha);
		}

		if (inputR <= 0)
			continue;
		if (inputK <= 0)
			continue;
		inputR = sizeTo2dCanvas(pg, inputR, displayMode);
		inputK = sizeTo2dCanvas(pg, inputK, displayMode);

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);
		//ctx2.lineWidth = 1;
		if ((int)setting[1] & 1) // track
		{
			// draw the track
			//ctx2.strokeStyle = lineColor;
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, xCenter - inputR, yCenter - inputR, xCenter + inputR, yCenter + inputR);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}

		double revolution = setting[0];
		double angle = (setting[3 + k] / revolution); // % 1
		angle *= TWO_PI;

		int knobX = xCenter + inputR * sin(angle);
		int knobY = yCenter - inputR * cos(angle);

		//ctx2.fillStyle = blackColor;
		//ctx2.font = '9px ' + default_2D_FontFamily;
		int originalDecimalPoint = -1;
		int kk = 0;
		int k0 = 0;
		string text;
		if ((int)setting[1] & 4) // number
		{
			// write the value
			text = tomString(detent(2, 3 + k));
			originalDecimalPoint = text.find('.', 0);
			kk = text.size();
			k0 = kk;
			while ((calcLargeStringWidthFont(pg->hDC2, text.substr(0, kk)) > 2 * inputR - inputK) &&
				(kk > 0))
				kk--;
			text = text.substr(0, kk);
		}

		if ((int)setting[1] & 4) // number continued
		{
			if ((originalDecimalPoint == -1) && (kk != k0))
				text = "****";
			int wid = calcLargeStringWidthFont(pg->hDC2, text.substr(0, kk));
			drawLargeTextString(pg->hDC2, xCenter - wid / 2, yCenter - 8, blackColor, text);
		}

		// if there is a user label put below
		if (userLabel.size()) {
			int textWidth = calcLargeStringWidthFont(pg->hDC2, userLabel);
			drawLargeTextString(pg->hDC2, xCenter - textWidth / 2, yCenter + inputR + 14, blackColor, userLabel);
		}

		if ((int)setting[1] & 2) // knob
		{
			// then the knob
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, knobX - inputK, knobY - inputK, knobX + inputK, knobY + inputK);
			EndPath(pg->hDC2);
			//ctx2.fillStyle = whiteColor;  // white
			StrokeAndFillPath(pg->hDC2);
		}
		// the Halo
		//if ((int)memory[0] == k) {
		//	ctx2.beginPath();
		//	ctx2.arc(knobX, knobY, DES_HALO_SIZE, 0, TWO_PI);
		//	ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}



//.setting[0] = 1.0;    // amount per revolution.
//.setting[1] = 7;    // option code: 1 draw track, 2 draw knob, 4 draw number
//.setting[2] = 0;    // detent or zero for disable
//.setting[3] = 0.0;    // output 3 +
bool Canvas_Knob_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	if (setting.size() > 3 + loops)
		setting.resize(3 + loops);

	int k;
	double inputX;
	double inputY;
	double inputR;
	double inputK;
	int xCenter;
	int yCenter;
	double revolution;
	double oldAngle;

	if (check) {
		for (k = loops - 1; k >= 0; k--) { // scan backwards
			// convert to pixels
			inputX = getLoopingValueD(0, k, 0);
			inputY = getLoopingValueD(1, k, 0);
			inputR = getLoopingValueD(2, k, 50);
			inputK = getLoopingValueD(3, k, DES_KNOB_SIZE);

			if (inputR <= 0)
				continue;
			if (inputK <= 0)
				continue;

			// alpha
			if (type == L"2dClrKnobZ")
				if (getLoopingValueD(7, k, 1) <= 0)
					continue;

			inputR = sizeTo2dCanvas(pg, inputR, displayMode);
			inputK = sizeTo2dCanvas(pg, inputK, displayMode);

			xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			yCenter = yTo2dCanvas2(pg, inputY, displayMode);
			revolution = setting[0];     // value changed in one revolution
			oldAngle = (setting[3 + k] / revolution);// % 1.0;   // value of output
			oldAngle *= TWO_PI;

			int knobX = xCenter + inputR * sin(oldAngle);
			int knobY = yCenter - inputR * cos(oldAngle);

			if (mouseX > knobX + inputK)
				continue; // missed knob
			if (mouseX < knobX - inputK)
				continue; // missed knob
			if (mouseY > knobY + inputK)
				continue; // missed knob
			if (mouseY < knobY - inputK)
				continue; // missed knob

			memory[0] = k;
			return true;
		}
		return false;
	}

	k = memory[0];

	// rep start
	inputX = getLoopingValueD(0, k, 0);
	inputY = getLoopingValueD(1, k, 0);
	inputR = getLoopingValueD(2, k, 25);

	xCenter = xTo2dCanvas2(pg, inputX, displayMode);
	yCenter = yTo2dCanvas2(pg, inputY, displayMode);
	if (inputR <= 0)
		return false;
	//inputR *= canvas2dScale;

	revolution = setting[0];     // value changed in one revolution
	oldAngle = (setting[3 + k] / revolution);// % 1.0;   // value of output
	oldAngle *= TWO_PI;

	int dx = mouseX - xCenter;
	int dy = mouseY - yCenter;
	double newAngle = atan2(dy, dx) + PI_OVER_2;

	double deltaAngle = newAngle - oldAngle;
	if (deltaAngle > PI)
		deltaAngle -= TWO_PI;
	else if (deltaAngle < -PI)
		deltaAngle += TWO_PI;

	double deltaFraction = deltaAngle / TWO_PI;
	deltaFraction *= revolution;

	setting[3 + k] += deltaFraction;
	setting[3 + k] = floor(setting[3 + k] * 100 / revolution + 0.5)
		/ (100 / revolution);

	method2(pg);

	return true;
}


int Canvas_Knob_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);

	// this is the Knob constant - send setting out
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	
	int oldSize = setting.size();
	setting.resize(3 + loops);
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		if (oldSize <= 3 + k)
			setting[3 + k] = setting[3 + k - 1]; // k NEVER ZERO
		output[0].value[k] = detent(2, 3 + k);
	}
	
	output[0].dimensions[0] = loops;

	return 3;
}



void Canvas_Knob_Component::onMouseUp2d(Sheet* pg) {
	memory[0] = -1;
}

std::vector<Button*>the2dKnobButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dDisplayMode) };

Canvas_Knob_Component::Canvas_Knob_Component()
	: Component(L"2dKnobZ", L"2D Knob", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = the2dKnobButtonList;
	setLeftLabels(3, "X", "Y", "Circle", "Radius");
	setSetting(0, 1.0);    // amount per revolution.
	setSetting(1, 7);    // option code: 1 draw track, 2 draw knob, 4 draw number
	setSetting(2, 0);    // detent
	setSetting(3, 0.0);    // output 3 +
	setMemory(0, 0);
	//getInternals = getInternals_3up;
	//setInternals = setInternals_3up_1out;
}




// *********************** 2D Linear Slider ***************************

int Canvas_LinearSlider_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	if ((int)output[0].value.size() != loops) {
		output[0].value.resize(loops);
		output[1].value.resize(loops);
		output[2].value.resize(loops);
		element.resize(loops);
	}
	int pin = 6; // always get x, y, value, x, y, value
	if ((int)setting[0] & bitSetArray[1]) // skip track Line Width
		pin++;
	if ((int)setting[0] & bitSetArray[2]) // skip Line color
		pin += 3;
	if ((int)setting[0] & bitSetArray[3]) // skip track Alpha
		pin++;
	if ((int)setting[0] & bitSetArray[4])  // skip knob size
		pin++;
	if ((int)setting[0] & bitSetArray[5])  // skip knob line size
		pin++;
	if ((int)setting[0] & bitSetArray[6])  // skip knob line color
		pin += 3;
	if ((int)setting[0] & bitSetArray[7])  // skip knob fill color
		pin += 3;
	if ((int)setting[0] & bitSetArray[8])  // skip knob alpha
		pin++;
	int savePin = pin;

	for (int j = 0; j < loops; j++) {
		// always X0, Y0, Value0, X1, Y1, Value1
		double inputX0 = getLoopingValueD(0, j, 0);
		double inputY0 = getLoopingValueD(1, j, 0);
		double inputValue0 = getLoopingValueD(2, j, 0);
		double inputX1 = getLoopingValueD(3, j, 0);
		double inputY1 = getLoopingValueD(4, j, 100); // defaults to a vertical slider
		double inputValue1 = getLoopingValueD(5, j, 100); // defaults to a vertical slider

		pin = savePin;
		double trackValue = 0;
		int trackNow = 0;
		int mouseTracking = 1;
		if ((int)setting[0] & bitSetArray[9]) {  // get knob tracking
			trackValue = getLoopingValueD(pin++, j, 0);
			trackNow = getLoopingValueD(pin++, j, 0);
			mouseTracking = getLoopingValueD(pin++, j, 1);
		}

		bool initialEnabled = false;
		double initialValue = 0;
		if ((int)setting[0] & bitSetArray[10]) {  // get initial settings
			initialEnabled = true;
			initialValue = getLoopingValueD(pin++, j, 0);
		}
		
		if (trackNow) {
			if ((trackValue >= min(inputValue0, inputValue1)) &&
				(trackValue <= max(inputValue0, inputValue1)))
				element[j] = trackValue;
		}
		else {
			//f (initialEnabled) {
			//	trackValue = initialValue;
			//	if ((int)initialMemory[j] != trackValue) {
			//		initialMemory[j] = initialValue;
			//		if ((initialValue >= min(inputValue0, inputValue1)) &&
			//			(initialValue <= max(inputValue0, inputValue1)))
			//			element[j] = initialValue;
			//	}
			//
		}

		if (inputValue1 == inputValue0) {
			output[0].value[j] = inputX0;
			output[1].value[j] = inputY0;
			output[2].value[j] = inputValue0;
			element[j] = inputValue0;
		}
		else {
			double newValue = funcDetent(element[j], setting[2]);
			double max = max(inputValue0, inputValue1);
			if (newValue > max)
				newValue = max;
			double min = min(inputValue0, inputValue1);
			if (newValue < min)
				newValue = min;
			element[j] = newValue;
			double temp = (newValue - inputValue0) / (inputValue1 - inputValue0);
			output[0].value[j] = (inputX1 - inputX0) * temp + inputX0;
			output[1].value[j] = (inputY1 - inputY0) * temp + inputY0;
			output[2].value[j] = newValue;
		}
	}
	for( int j = 0; j < 3; j++)
		output[j].dimensions[0] = loops;

	return 3;
}

bool Canvas_LinearSlider_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	for (int j = 0; j < loops; j++) {

		int pin = 0;
		double inputX0 = getLoopingValueD(pin++, j, 0);
		double inputY0 = getLoopingValueD(pin++, j, 0);
		double inputValue0 = getLoopingValueD(pin++, j, 0);
		double inputX1 = getLoopingValueD(pin++, j, 0);
		double inputY1 = getLoopingValueD(pin++, j, 100); // defaults to a vertical slider
		double inputValue1 = getLoopingValueD(pin++, j, 100); // defaults to a vertical slider

		// 1=track Line Width: [["track Line Width"]]
		double trackLineWidth = 1;
		if ((int)setting[0] & bitSetArray[1])
			trackLineWidth = getLoopingValueD(pin++, j, 1);
		
		double lineWidth = sizeTo2dCanvas(pg, trackLineWidth, displayMode);
		int trackLineRed = 0;
		int trackLineGreen = 0;
		int trackLineBlue = 0;
		// 2=Line color: ["Line Red", "Line Green", "Line Blue"],
		if ((int)setting[0] & bitSetArray[2]) {
			trackLineRed = getLoopingValueD(pin++, j, 0);
			trackLineGreen = getLoopingValueD(pin++, j, 0);
			trackLineBlue = getLoopingValueD(pin++, j, 0);
		}
		
		// 3=Alpha: ["Track Alpha"],
		double trackAlpha = 1;
		if ((int)setting[0] & bitSetArray[3])
			trackAlpha = getLoopingValueD(pin++, j, 1);
			
		// 4-get knob Size
		double knobSize = DES_KNOB_SIZE;
		if ((int)setting[0] & bitSetArray[4])
			knobSize = getLoopingValueD(pin++, j, DES_KNOB_SIZE);
			
		// 5-get knob line Width
		double knobLineWidth = 1;
		if ((int)setting[0] & bitSetArray[5])
			knobLineWidth = getLoopingValueD(pin++, j, 1);
			
		// 6-Line color: ["Knob Line Red", "Knob Line Green", "Knob Line Blue"],
		int knobLineRed = 0;
		int knobLineGreen = 0;
		int knobLineBlue = 0;
		if ((int)setting[0] & bitSetArray[6]) {
			knobLineRed = getLoopingValueD(pin++, j, 0);
			knobLineGreen = getLoopingValueD(pin++, j, 0);
			knobLineBlue = getLoopingValueD(pin++, j, 0);
		}
		
		// 7-Line color: ["Knob Fill Red", "Knob Fill Green", "Knob Fill Blue"],
		int knobFillRed = 255;
		int knobFillGreen = 255;
		int knobFillBlue = 255;
		if ((int)setting[0] & bitSetArray[7]) {
			int knobFillRed = getLoopingValueD(pin++, j, 0);
			int knobFillGreen = getLoopingValueD(pin++, j, 0);
			int knobFillBlue = getLoopingValueD(pin++, j, 0);
		}
		
		// 8=knob Alpha: ["Knob Alpha"],
		double knobAlpha = 1;
		if ((int)setting[0] & bitSetArray[8])
			knobAlpha = getLoopingValueD(pin++, j, 1);

		// convert the end points
		int x0 = xTo2dCanvas2(pg, inputX0, displayMode);
		int y0 = yTo2dCanvas2(pg, inputY0, displayMode);
		int x1 = xTo2dCanvas2(pg, inputX1, displayMode);
		int y1 = yTo2dCanvas2(pg, inputY1, displayMode);

		if ((trackAlpha > 0) && (trackLineWidth > 0)) {
			// draw the track
			//ctx2.strokeStyle = RGB(trackLineRed, trackLineGreen, trackLineBlue);// , trackAlpha);
			//setDashingContext2(pg, i, 0);
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, x0, y0, NULL);
			LineTo(pg->hDC2, x1, y1);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		//ctx2.setLineDash([]);


		if ((knobSize > 0) && (knobLineWidth > 0) && (knobAlpha > 0)) {
			// draw the knobs
			knobSize = sizeTo2dCanvas(pg, knobSize, displayMode);
			knobLineWidth = sizeTo2dCanvas(pg, knobLineWidth, displayMode);

			COLORREF fillStyle = RGB(knobFillRed, knobFillGreen, knobFillBlue);// , knobAlpha);
			COLORREF strokeStyle = RGB(knobLineRed, knobLineGreen, knobLineBlue);// , knobAlpha);
			int lineWidth = knobLineWidth;

			int xx;
			int yy;
			if (inputValue1 == inputValue0) {
				xx = x0;
				yy = y0;
			}
			else {
				double value = element[j];
				double temp = (value - inputValue0) / (inputValue1 - inputValue0);
				xx = (x1 - x0) * temp + x0;
				yy = (y1 - y0) * temp + y0;
			}
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, xx - knobSize, yy - knobSize, xx + knobSize, yy + knobSize);
			EndPath(pg->hDC2);
			StrokeAndFillPath(pg->hDC2);
		}

		// needed for either halo or touch diagnostic
		//if (inputValue1 == inputValue0) {
		//	xx = x0;
		//	yy = y0;
		//}
		//else {
		//	value = element[j];
		//	temp = (value - inputValue0) / (inputValue1 - inputValue0);
		//	xx = (x1 - x0) * temp + x0;
		//	yy = (y1 - y0) * temp + y0;
		//}
		//// the Halo
		//if ((int)memory[0] == j) {
		//
		//	ctx2.beginPath();
		//	ctx2.arc(xx, yy, DES_HALO_SIZE, 0, TWO_PI);
		//	ctx2.fillStyle = blackAlpha(DES_HALO_ALPHA);
		//	ctx2.fill();
		//}
		//// the touch area - DIAGNOSTIC
		//if (DES_TOUCH_ALPHA) {
		//	int touchSize = max(DES_TOUCH_SIZE, knobSize);
		//	ctx2.beginPath();
		//	ctx2.arc(xx, yy, touchSize, 0, TWO_PI);
		//	ctx2.fillStyle = RGB(0, 255, 0);// , DES_TOUCH_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

bool Canvas_LinearSlider_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	int j;
	double inputX0;
	double inputY0;
	double inputValue0;
	double inputX1;
	double inputY1;
	double inputValue1;

	if (check) {
		bool hitOne = false;
		for (j = loops - 1; j >= 0; j--) {

			inputX0 = getLoopingValueD(0, j, 0);
			inputY0 = getLoopingValueD(1, j, 0);
			inputValue0 = getLoopingValueD(2, j, 0);
			inputX1 = getLoopingValueD(3, j, 0);
			inputY1 = getLoopingValueD(4, j, 100); // defaults to a vertical slider
			inputValue1 = getLoopingValueD(5, j, 100); // defaults to a vertical slider
			int pin = 6;
			if ((int)setting[0] & bitSetArray[1])  // skip 1=track Line Width [Track Width]
				pin++;
			if ((int)setting[0] & bitSetArray[2])  // skip 2=Line color: ["track Red", "track Green", "track Blue"],
				pin += 3;
			if ((int)setting[0] & bitSetArray[3])// skip 3=Alpha: ["Track Alpha"],
				pin++;

			// 4-get knob Size
			double knobSize = DES_KNOB_SIZE;
			if ((int)setting[0] & bitSetArray[4]) // 4 knob size
				knobSize = getLoopingValueD(pin++, j, DES_KNOB_SIZE);
				
			if ((int)setting[0] & bitSetArray[5])// skip 5 knob line Width
				pin++;
			if ((int)setting[0] & bitSetArray[6]) // skip 6-knob line width
				pin += 3;
			if ((int)setting[0] & bitSetArray[7]) // skip 7-knob fill color:
				pin += 3;
			if ((int)setting[0] & bitSetArray[8]) // skip 8=knob Alpha: ["Knob Alpha"],
				pin++;

			int trackIndex = 0;
			int trackNow = 0;
			int mouseTracking = 1;
			if ((int)setting[0] & bitSetArray[9]) {  // get knob tracking
				trackIndex = getLoopingValueD(pin++, j, 0);
				trackNow = getLoopingValueD(pin++, j, 0);
				mouseTracking = getLoopingValueD(pin++, j, 1);
				if (mouseTracking <= 0)
					continue;
			}
			
			// convert the end points
			int x0 = xTo2dCanvas2(pg, inputX0, displayMode);
			int y0 = yTo2dCanvas2(pg, inputY0, displayMode);
			int x1 = xTo2dCanvas2(pg, inputX1, displayMode);
			int y1 = yTo2dCanvas2(pg, inputY1, displayMode);

			knobSize = sizeTo2dCanvas(pg, knobSize, displayMode);
			double touchSize = max(DES_TOUCH_SIZE, knobSize);

			double value = element[j];
			double temp = (value - inputValue0) / (inputValue1 - inputValue0);
			int xx = (x1 - x0) * temp + x0;
			int yy = (y1 - y0) * temp + y0;

			if (mouseX > xx + touchSize)
				continue; // missed dial
			if (mouseX < xx - touchSize)
				continue; // missed dial

			if (mouseY > yy + touchSize)
				continue; // missed dial
			if (mouseY < yy - touchSize)
				continue; // missed dial

			memory[0] = j;
			hitOne = true;
			return true;
		}
		if (!hitOne) {
			memory[0] = -1;
			return false;
		}
	}
	else {
		if ((int)memory[0] < 0)
			return false;
		j = memory[0];
	}

	// ok current selection is j,
	double value = element[j];

	inputX0 = getLoopingValueD(0, j, 0);
	inputY0 = getLoopingValueD(1, j, 0);
	inputValue0 = getLoopingValueD(2, j, 0);
	inputX1 = getLoopingValueD(3, j, 0);
	inputY1 = getLoopingValueD(4, j, 100); // defaults to a vertical slider
	inputValue1 = getLoopingValueD(5, j, 100); // defaults to a vertical slider

	// convert the end points
	int x0 = xTo2dCanvas2(pg, inputX0, displayMode);
	int y0 = yTo2dCanvas2(pg, inputY0, displayMode);
	int x1 = xTo2dCanvas2(pg, inputX1, displayMode);
	int y1 = yTo2dCanvas2(pg, inputY1, displayMode);

	double scale;
	double newValue;
	if (abs(x0 - x1) > abs(y0 - y1)) {
		// horizontal mode
		scale = (x1 - x0) / (inputValue1 - inputValue0);
		newValue = ((mouseX - x0) / scale) + inputValue0;
	}
	else {
		// vertical mode
		scale = (y1 - y0) / (inputValue1 - inputValue0);
		newValue = ((mouseY - y0) / scale) + inputValue0;
	}
	double max = max(inputValue0, inputValue1);
	if (newValue > max)
		newValue = max;
	double min = min(inputValue0, inputValue1);
	if (newValue < min)
		newValue = min;

	if (newValue != value) {
		element[j] = funcDetent(newValue, setting[2]);
		
		method2(pg);
		return true;
	}
	return false;
}


void Canvas_LinearSlider_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	setting[2] = doubleValues[0];  // Detent

	setting[0] = 1 + (boolValues[0] ? 2 : 0) + (boolValues[1] ? 4 : 0) +
		(boolValues[2] ? 8 : 0) + (boolValues[3] ? 16 : 0) + (boolValues[4] ? 32 : 0) +
		(boolValues[5] ? 64 : 0) + (boolValues[6] ? 128 : 0) + (boolValues[7] ? 256 : 0) +
		(boolValues[8] ? 512 : 0) + (boolValues[9] ? 1024 : 0) + 4096;
	displayMode = boolValues[10];

	onLoad(pg);
}
void doubleClick2dLinSld(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Detent:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2] };

	vector<string> checkLabels = { "Track Width", "Track Color", "Track Alpha", 
		 "Knob Size", "Knob Line Width", "Knob Line Color", "Knob Fill Color", "Knob Alpha",
		 "Tracking", "Toggle Initial", "Ignore Display Scale" };
	vector<bool> checkValues = {
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 2),	// 1 track Width
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 4),	// 2 track color: ["Line Red", "Line Green", "Line Blue"],
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 8),	// 3 track Alpha: ["Track Alpha"],
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 16),	// 4 knob Size
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 32),	// 5 knob line Width
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 64),	// 6 knob Line color
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 128),	// 7 knob Fill color
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 256),	// 8 knob alpha
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 512),  // 9 tracking [track value, track now, mouse tracking]
		(bool)((int)pg->dgrm.cL[i]->setting[0] & 1024),	// 10 initial value
		(bool)pg->dgrm.cL[i]->displayMode };
	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>the2dLinSldButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dLinSld) };

bool Canvas_LinearSlider_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Canvas_LinearSlider_Component::GetPinGroups() { return vector<int>{6, 1, 3, 1, 1, 1, 3, 3, 1, 3, 1 }; }
vector<string> Canvas_LinearSlider_Component::GetPinLabels() {
	return vector<string>{
		"X0", "Y0", "Value0", "X1", "Y1", "Value1",
		"Track Width", 
		"Track Red", "Track Green", "Track Blue",
		"Track Alpha",
		"Knob Size",
		"Knob Line Width",
		"Knob Line Red", "Knob Line Green", "Knob Line Blue",
		"Knob Fill Red", "Knob Fill Green", "Knob Fill Blue",
		"Knob Alpha",
		"Track Value", "Track Now", "Mouse Tracking",
		"Initial Value"};
}

Canvas_LinearSlider_Component::Canvas_LinearSlider_Component()
	: Component(L"2dLinSld", L"2D Linear Slider", squareShape, 6, 1, 3, 1)
{
	dialogButtonList = the2dLinSldButtonList;
	pinMaskIndex = 0;
	setMemory(0, -1); // knob in motion
	setSetting(0, 1 + 4096); // 0-X,Y values (1), 12-outputs (4096)
	setSetting(1, 1); // WAS the number of knobs
	setSetting(2, 0); // detent
	setElement(0, 0); // knob zero is at element 0
	//setLeftLabels(3, "X0", "Y0", "Value0", "X1", "Y1", "Value1");
	setRightLabels(3, "X", "Y", "Value");
	//    nC->getInternals = getInternals_2dGeom; 
	//    nC->setInternals = setInternals_2dGeom; 
	width = 50;
	completionTestDoubles = 1;
	completionTestChecks = 11;
}


bool pointInRectangle(int pointX, int pointY, int rectX, int rectY, int rectWidth, int rectHeight) {
	if (pointX > rectX + rectWidth)
		return false;
	if (pointX < rectX - rectWidth)
		return false;
	if (pointY > rectY + rectHeight)
		return false;
	if (pointY < rectY - rectHeight)
		return false;
	return true;
}
//bool pointInSquare(int pointX, int pointY, int squareX, int squareY, int squareSize) {
//	if (pointX > squareX + squareSize)
//		return false;
//	if (pointX < squareX - squareSize)
//		return false;
//	if (pointY > squareY + squareSize)
//		return false;
//	if (pointY < squareY - squareSize)
//		return false;
//	return true;
//}
// *************************** Snap Point 2 *************************************
	//
	// inputs: 0-Initial X, 1-Initial Y, 2-Symbol count,
	//         3-Symbol Width, 4-Symbol Height,
	//         5-Initialize,
	//         6-Target X, 7-Target Y, 8-Target Width, 9-Target Height,
	// outputs: 0-Symbol X, 1-Symbol Y, 2-Symbol ID,
	//          3-Active Point, 4-Symbol Placed On, 5-Target Filled By

#define MAX_SYMBOLS 20
#define MAX_TARGETS 20
bool SnapPoint2_Component::snapPoint2Test(Sheet* pg) {
	int symbols = inputsToLoopsSN(0, 6);
	int targets = inputsToLoopsSN(6, 4);
	if ((symbols == 0) || (targets == 0))
		return clearOutputs();

	if (targets > MAX_TARGETS)
		return clearOutputs();

	// symbols can be multiple, so count them all
	int totalSymbolCount = 0;
	for (int s = 0; s < symbols; s++)
		totalSymbolCount += getLoopingValueD(2, s, 1);
	if (totalSymbolCount > MAX_SYMBOLS) 
		return clearOutputs();

	int symbolX[MAX_SYMBOLS] = { 0 };
	int symbolY[MAX_SYMBOLS] = { 0 };
	int symbolID[MAX_SYMBOLS] = { 0 };

	int targetRawX[MAX_TARGETS] = { 0 };
	int targetRawY[MAX_TARGETS] = { 0 };
	int targetX[MAX_TARGETS] = { 0 };
	int targetY[MAX_TARGETS] = { 0 };
	int targetWidth[MAX_TARGETS] = { 0 };
	int targetHeight[MAX_TARGETS] = { 0 };

	output[0].value.resize(totalSymbolCount);
	output[1].value.resize(totalSymbolCount);
	output[2].value.resize(totalSymbolCount);
	output[3].value.resize(totalSymbolCount);
	output[4].value.resize(totalSymbolCount);
		
	// load the symbol arrays
	int index = 0; // count the symbols in play
	for (int s = 0; s < symbols; s++) {
		int symCount = getLoopingValueD(2, s, 1);

		for (int sc = 0; sc < symCount; sc++) {
			symbolX[index] = xTo2dCanvas2(pg, output[0].value[index], displayMode);
			symbolY[index] = yTo2dCanvas2(pg, output[1].value[index], displayMode);
			symbolID[index] = s;

			output[4].value[index] = -1; // no point placed yet
			index++;
		}
	}
	

	// load the target arrays
	output[5].value.resize(targets);
	for (int t = 0; t < targets; t++) {
		targetRawX[t] = getLoopingValueD(6, t, 0);
		targetRawY[t] = getLoopingValueD(7, t, 0);
		targetWidth[t] = sizeTo2dCanvas( pg, getLoopingValueD(8, t, 30) / 2, displayMode);
		targetHeight[t] = sizeTo2dCanvas(pg, getLoopingValueD(9, t, 30) / 2, displayMode);

		targetX[t] = xTo2dCanvas2(pg, targetRawX[t], displayMode);
		targetY[t] = yTo2dCanvas2(pg, targetRawY[t], displayMode);

		output[5].value[t] = -1; // no target filled yet
	}

	int symbolInTarget[MAX_SYMBOLS];
	bool targetIsFilled[MAX_TARGETS];
//	int pointOrder[MAX_SYMBOLS];
	bool last = false;
	int k = 0;

	for (int t = 0; t < targets; t++) {
		symbolInTarget[t] = -1;
		targetIsFilled[t] = false;
	}

	// scan the works
	for (int s = 0; s < totalSymbolCount; s++) { // here we scan all the symbols
		//.op = pointOrder[p];
		for (int t = 0; t < targets; t++) {  //  for a match to this target
			if (targetIsFilled[t])
				continue;
			if (targetWidth[t] <= 0)
				continue;
			if (targetHeight[t] <= 0)
				continue;
			if (pointInRectangle(symbolX[s], symbolY[s], targetX[t], targetY[t], targetWidth[t], targetHeight[t])) {
				symbolInTarget[t] = s;
				targetIsFilled[t] = true;
				break;
			}
		}
	}

	// first just deal with the test results (snapping doesn't matter here)
	for (int t = 0; t < targets; t++) { // scan the targets
		if (symbolInTarget[t] != -1) {
			int s = symbolInTarget[t];
			//op = pointOrder[p]; // this may be wrong
			output[4].value[s] = t; // point placed on
			output[5].value[t] = symbolID[s]; // target filled by
		}
	}

	// snap in any order
	for (int t = 0; t < targets; t++) { // scan the targets
		if (symbolInTarget[t] != -1) {
			int s = symbolInTarget[t];
			//op = pointOrder[p];
			output[0].value[s] = targetRawX[t];
			output[1].value[s] = targetRawY[t];
		}
	}
	return true;
}

// outputs: 0-Symbol X, 1-Symbol Y, 2-Symbol ID,
//          3-Active Point, 4-Symbol Placed On, 5-Target Filled By
int SnapPoint2_Component::method2(Sheet* pg) {
	int symbols = inputsToLoopsSN(0, 6);
	int targets = inputsToLoopsSN(6, 4);
	if ((symbols == 0) || (targets == 0))
		return clearOutputs();

	// copy the initial inputs directly to outputs (drivers) if they change
	int k = 0;
	bool anyForce = false;

	// symbols can be multiple, so count them all
	int totalSymbolCount = 0;
	int symcnt;
	for (int s = 0; s < symbols; s++) {
		symcnt = getLoopingValueD(2, s, 1);
		if (symcnt < 1)
			return clearOutputs();
		totalSymbolCount += symcnt;
	}
	if (totalSymbolCount < 1)
		return clearOutputs();
	if (totalSymbolCount > MAX_SYMBOLS)
		return clearOutputs();

	initialMemoryX.resize(totalSymbolCount);
	initialMemoryY.resize(totalSymbolCount);
	output[0].value.resize(totalSymbolCount); // symbol x position
	output[1].value.resize(totalSymbolCount); // symbol y position
	output[2].value.resize(totalSymbolCount); // symbol's ID
	output[3].value.resize(totalSymbolCount); // is symbol active

	output[5].value.resize(targets);

	for (int s = 0; s < symbols; s++) {

		int initialX = getLoopingValueD(0, s, 0);
		int initialY = getLoopingValueD(1, s, 0);
		int symCount = getLoopingValueD(2, s, 1);

		int forceInit = getLoopingValueD(5, s, 0);
		if (!forceInit) {
			if ((initialMemoryX[s] != initialX) ||
				(initialMemoryY[s] != initialY))
				forceInit = true;
		}
		if (forceInit) {
			initialMemoryX[s] = initialX;
			initialMemoryY[s] = initialY;
			for (int sc = 0; sc < symCount; sc++) {
				output[0].value[k] = initialX; // x
				output[1].value[k] = initialY; // y
				output[2].value[k] = s;        // ID
				output[3].value[k] = 0;        // Active Point
				k++;
			}
			anyForce = true;
		}
		else
		{
			k += symCount;
		}
	}
	if (anyForce) {
		for (int n = 0; n < targets; n++)
			output[5].value[n] = -1;

		for (int n = 0; n < 5; n++) {
			output[n].value.resize(k);
			output[n].dimensions.resize(1);
			output[n].dimensions[0] = k;
		}

		output[5].value.resize(targets);
		output[5].dimensions.resize(1);
		output[5].dimensions[0] = targets;
	}
	
	return snapPoint2Test(pg);
}

bool SnapPoint2_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	int symbols = inputsToLoopsSN(0, 6);
	int targets = inputsToLoopsSN(6, 4);
	if ((symbols == 0) || (targets == 0))
		return false;

	if ((symbols > MAX_SYMBOLS) || (targets > MAX_TARGETS))
		return clearOutputs();

	int k;
	if (check) {
		int loops = output[3].value.size();
		for (k = 0; k < loops; k++)
			output[3].value[k] = 0; // not active

		double targetX[MAX_TARGETS];
		double targetY[MAX_TARGETS];
		double targetWidth[MAX_TARGETS];
		double targetHeight[MAX_TARGETS];
		for (int t = 0; t < targets; t++) {
			targetX[t] = xTo2dCanvas2(pg, getLoopingValueD(6, t, 0), displayMode);
			targetY[t] = yTo2dCanvas2(pg, getLoopingValueD(7, t, 0), displayMode);
			targetWidth[t] = sizeTo2dCanvas(pg, getLoopingValueD(8, t, 30) / 2, displayMode);
			targetHeight[t] = sizeTo2dCanvas(pg, getLoopingValueD(9, t, 30) / 2, displayMode);
		}

		double symbolWidth[MAX_SYMBOLS];
		double symbolHeight[MAX_SYMBOLS];
		for (int s = 0; s < symbols; s++) {
			symbolWidth[s] = sizeTo2dCanvas(pg, getLoopingValueD(3, s, DES_TOUCH_SIZE) / 2, displayMode);
			symbolHeight[s] = sizeTo2dCanvas(pg, getLoopingValueD(4, s, DES_TOUCH_SIZE) / 2, displayMode);
		}

		bool hitOne = false;
		for (k = loops - 1; k >= 0; k--) { // scan backwards so we pick the last one drawn (the top visible)

			double symbolX = xTo2dCanvas2(pg, output[0].value[k], displayMode);
			double symbolY = yTo2dCanvas2(pg, output[1].value[k], displayMode);
			int symbolID = output[2].value[k];

			// is the mouse in the driver?
			if (mouseX > symbolX + symbolWidth[symbolID])
				continue; // missed
			if (mouseX < symbolX - symbolWidth[symbolID])
				continue; // missed

			if (mouseY > symbolY + symbolHeight[symbolID])
				continue; // missed
			if (mouseY < symbolY - symbolHeight[symbolID])
				continue; // missed

			memory[0] = k;
			memory[1] = mouseX - symbolX;
			memory[2] = mouseY - symbolY;

			hitOne = true;
			output[3].value[k] = 1; // active point
			break;
		}
		if (!hitOne) {
			memory[0] = -1;
			return false;
		}
	}
	else {
		if (memory[0] < 0)
			return false;
		k = memory[0];

	}

	if (memory[0] != -1) { // move it


		//if (outputValue[0][k] == undefined) {
		//	outputValue[0][k] = 0; // point x
		//	outputValue[1][k] = 0; // point y
		//}
		//else {
			output[0].value[k] = canvasToX2(pg, floor(mouseX - memory[1]), displayMode);
			output[1].value[k] = canvasToY2(pg, floor(mouseY - memory[2]), displayMode);
		//}

		snapPoint2Test(pg);
	}

	return true;
}

void SnapPoint2_Component::onMouseUp2d(Sheet* pg) {
	int loops = output[3].value.size();
	for (int k = 0; k < loops; k++)
		output[3].value[k] = 0; // clear active point
	return;
}

std::vector<Button*>snapPoint2ButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dDisplayMode) };

SnapPoint2_Component::SnapPoint2_Component()
	: Component(L"SnapPoint2", L"Snap Point", squareShape, 10, 1, 6, 1)
{
	dialogButtonList = snapPoint2ButtonList;
	setMemory(0, 0); // point in motion
	setMemory(1, 0); // offset X
	setMemory(2, 0); // offset Y
	//initialMemoryX;
	//initialMemoryY;
	setSetting(0, 0);  // spare
	setSetting(1, 1);  // spare
	setLeftLabels(3, "Initial Symbol X", "Initial Symbol Y", "Symbol Count", "Symbol Width", "Symbol Height", "Initialize", "Target X", "Target Y", "Target Width", "Target Height");
	setRightLabels(3, "Symbol X", "Symbol Y", "Symbol ID", "Active Symbol", "Symbol Placed On", "Target Filled By");
	width = 75;
	for( int k = 0; k < 6; k++)
		setOutputDimension(k, 0, 1);
}

//***************************** 2D Geometry ****************************************

// 2D Geometry code
// setting[0] = number of points
// setting[1] = code: 1 draw line, 2 draw circle, 4 draw number, 8 close polygon, 16 coordinates
// setting[2] = input pins choice //  new this version
// setting[3] = value of x[0]
// setting[4] = value of y[0]

bool Canvas_Geometry_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	bool color = false;
	int x, y;
	
	if ((int)setting[1] & 1) { // the lines
		// pass one - draw the lines
		for (int j = 0; j < setting[0]; j++) {
			int inPin = 0;

			// skip size
			if ((int)setting[2] & 1)
				inPin++;

			// get line Color
			int inputLineRed = 0;
			int inputLineGreen = 0;
			int inputLineBlue = 0;
			if ((int)setting[2] & 2) {
				inputLineRed = getLoopingValueD(inPin++, j, 0);
				inputLineGreen = getLoopingValueD(inPin++, j, 0);
				inputLineBlue = getLoopingValueD(inPin++, j, 0);
			}
			
			// skip fill Color
			if ((int)setting[2] & 4)
				inPin += 3;

			// Alpha
			double inputAlpha = 1;
			if ((int)setting[2] & 8) {
				int savePin = inPin;
				inputAlpha = getLoopingValueD(inPin++, j, 1);
				if (inputAlpha <= 0)
					continue;
				if (j + 1 < setting[0]) // only draw this line if the next point has a good alpha
					if (getLoopingValueD(savePin, j+1, 1) <= 0)
						continue;
			}
			
			if ((int)setting[2] & 16)  // skip preset
				inPin += 3;

			// get detent
			double xDetent = 0;
			double yDetent = 0;
			if ((int)setting[2] & 32) {
				xDetent = getLoopingValueD(inPin++, j, 0);
				yDetent = getLoopingValueD(inPin++, j, 0);
			}
			
			COLORREF lineColor = RGB(inputLineRed, inputLineGreen, inputLineBlue);// , inputAlpha);

			// draw the lines
			//ctx2.strokeStyle = lineColor;
			if (j < setting[0] - 1) {
				BeginPath(pg->hDC2);
				x = xTo2dCanvas2(pg, funcDetent(setting[2 * j + 3], xDetent), displayMode);
				y = yTo2dCanvas2(pg, funcDetent(setting[2 * j + 4], yDetent), displayMode);
				MoveToEx(pg->hDC2, x, y, NULL);
				x = xTo2dCanvas2(pg, funcDetent(setting[2 * (j + 1) + 3], xDetent), displayMode);
				y = yTo2dCanvas2(pg, funcDetent(setting[2 * (j + 1) + 4], yDetent), displayMode);
				LineTo(pg->hDC2, x, y);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
			else if ((int)setting[1] & 8) { // close the shape

				BeginPath(pg->hDC2);
				x = xTo2dCanvas2(pg, funcDetent(setting[2 * j + 3], xDetent), displayMode);
				y = yTo2dCanvas2(pg, funcDetent(setting[2 * j + 4], yDetent), displayMode);
				MoveToEx(pg->hDC2, x, y, NULL);

				x = xTo2dCanvas2(pg, funcDetent(setting[3], xDetent), displayMode);
				y = yTo2dCanvas2(pg, funcDetent(setting[4], yDetent), displayMode);
				LineTo(pg->hDC2, x, y);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
			}
		}
	}
	// pass two the knobs numbers and coordinates
	if ((int)setting[1] & (2 + 4 + 16)) { // knob OR number OR coordinates
		for (int j = 0; j < setting[0]; j++) {
			int inPin = 0;

			// get size
			double inputSize = DES_KNOB_SIZE;
			if ((int)setting[2] & 1) {
				inputSize = getLoopingValueD(inPin++, j, DES_KNOB_SIZE);
				if (inputSize < 0)
					inputSize = 0;
			}
			
			// line Color
			int inputLineRed = 0;
			int inputLineGreen = 0;
			int inputLineBlue = 0;
			if ((int)setting[2] & 2) {
				inputLineRed = getLoopingValueD(inPin++, j, 0);
				inputLineGreen = getLoopingValueD(inPin++, j, 0);
				inputLineBlue = getLoopingValueD(inPin++, j, 0);
			}
			
			// fill Color
			int inputFillRed = 255;// when fill pins are hidden default fill to white
			int inputFillGreen = 255;
			int inputFillBlue = 255;
			if ((int)setting[2] & 4) {
				// when fill pins are availabel default fill to black
				inputFillRed = getLoopingValueD(inPin++, j, 0);
				inputFillGreen = getLoopingValueD(inPin++, j, 0);
				inputFillBlue = getLoopingValueD(inPin++, j, 0);
			}
			
			// Alpha
			double inputAlpha = 1;
			if ((int)setting[2] & 8) {
				inputAlpha = getLoopingValueD(inPin++, j, 1);
				if (inputAlpha <= 0)
					continue;
			}
			
			if ((int)setting[2] & 16)  // skip preset
				inPin += 3;

			// get detent
			double xDetent = 0;
			double yDetent = 0;
			if ((int)setting[2] & 32) {
				xDetent = getLoopingValueD(inPin++, j, 0);
				yDetent = getLoopingValueD(inPin++, j, 0);
			}

			COLORREF lineColor = RGB(inputLineRed, inputLineGreen, inputLineBlue);// , inputAlpha);
			COLORREF fillColor = RGB(inputFillRed, inputFillGreen, inputFillBlue);// , inputAlpha);
			COLORREF blackColor = RGB(inputAlpha, inputAlpha, inputAlpha);

			x = xTo2dCanvas2(pg, funcDetent(setting[2 * j + 3], xDetent), displayMode);
			y = yTo2dCanvas2(pg, funcDetent(setting[2 * j + 4], yDetent), displayMode);

			inputSize = sizeTo2dCanvas(pg, inputSize, displayMode);

			if ((int)setting[1] & 2) // knob
			{
				//  the knobs
				BeginPath(pg->hDC2);
				Ellipse(pg->hDC2, x - inputSize, y - inputSize, x + inputSize, y + inputSize);
				EndPath(pg->hDC2);
				//ctx2.fillStyle = fillColor;  // white
				StrokeAndFillPath(pg->hDC2);
			}
			//if (inputSize == undefined)
			//	inputSize = 9;
			//if (inputSize < 0)
			//	ctx2.font = '9px ' + default_2D_FontFamily;
			//else
			//	ctx2.font = inputSize.tomString() + 'px ' + default_2D_FontFamily;
			if ((int)setting[1] & 4) // show the number
			{
				string text = tomString(j);
				int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
				drawLargeTextString(pg->hDC2, x - textWidth / 2, y -9/*+ inputSize / 2.6*/, blackColor, text );
			}

			if ((int)setting[1] & 16) { // show the coordinates
				string text = "(" + tomString(setting[2 * j + 3]) + ", "
					+ tomString(setting[2 * j + 4]) + ")";
				int textWidth = calcLargeStringWidthFont(pg->hDC2, text);
		
				if ((int)setting[1] & (2 + 4))// change y when knob or number present
					drawLargeTextString(pg->hDC2, x - textWidth / 2, y + inputSize, blackColor, text);
				else
					drawLargeTextString(pg->hDC2, x - textWidth / 2, y + 6, blackColor, text);
			}

			// the Halo
			//if ((int)memory[0] == j) {
			//	ctx2.beginPath();
			//	ctx2.arc(x, y, DES_HALO_SIZE, 0, TWO_PI);
			//	ctx2.fillStyle = blackAlpha(DES.HALO_ALPHA);
			//	ctx2.fill();
			//}
			//// the touch area - DIAGNOSTIC
			//if (DES_TOUCH_ALPHA) {
			//	var touchSize = Math.max(DES.TOUCH_SIZE, inputSize);
			//	ctx2.beginPath();
			//	ctx2.arc(x, y, touchSize, 0, TWO_PI);
			//	ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES.TOUCH_ALPHA);
			//	ctx2.fill();
			//}
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}




int Canvas_Geometry_Component::method2(Sheet* pg) {

	// check for x y tracking input higher than current size setting
	if ((int)setting[2] & 16) { // check for preset input active
		int inPin = 0;
		// skip size
		if ((int)setting[2] & 1)
			inPin++;
		// skip line Color
		if ((int)setting[2] & 2)
			inPin += 3;
		// skip fill Color
		if ((int)setting[2] & 4)
			inPin += 3;
		// skip Alpha
		if ((int)setting[2] & 8)
			inPin++;
		// we know bit 9 is set, so...
		int xSize = input[inPin++].value.size();
		int ySize = input[inPin++].value.size();
		int yxyTrackingSize = input[inPin].value.size();

		bool tracingActive = false;
		for (int j = 0; j < yxyTrackingSize; j++)
			if (input[inPin].value[j])
				tracingActive = true;
		if (tracingActive) {
			setting[0] = max(max(xSize, ySize), yxyTrackingSize);
			if (setting[0] < 1)
				setting[0] = 1;
			if (setting[0] > 99)
				setting[0] = 99;
		}
	}

	// process as usual
	output[0].value.clear();
	output[1].value.clear();
	output[2].value.clear();
	output[0].value.resize(setting[0]);
	output[1].value.resize(setting[0]);
	output[2].value.resize(setting[0]);

	for (int j = 0; j < setting[0]; j++) {
		int inPin = 0;
		// skip size
		if ((int)setting[2] & 1)
			inPin++;
		// skip line Color
		if ((int)setting[2] & 2)
			inPin += 3;
		// skip fill Color
		if ((int)setting[2] & 4)
			inPin += 3;
		// skip Alpha
		if ((int)setting[2] & 8)
			inPin++;
		double inputX = 0;
		double inputY = 0;
		int xyTrackingPreset = 0;
		if ((int)setting[2] & 16) {
			if (j < input[inPin].value.size())
				inputX = getLoopingValueD(inPin, j, 0);
			else
				inputX = 0;
			inPin++;

			if (j < input[inPin].value.size())
				inputY = getLoopingValueD(inPin, j, 0);
			else
				inputY = 0;
			inPin++;

			if (j < input[inPin].value.size())
				xyTrackingPreset = getLoopingValueD(inPin, j, 0);
			else
				xyTrackingPreset = 0;
			inPin++;

			if (xyTrackingPreset) {
				// preset this knob
				setting[2 * j + 3] = inputX;
				setting[2 * j + 4] = inputY;
			}
		}

		// get detent
		double xDetent = 0;
		double yDetent = 0;
		if ((int)setting[2] & 32) {
			xDetent = getLoopingValueD(inPin++, j, 0);
			yDetent = getLoopingValueD(inPin++, j, 0);
		}

		//  send settings out
		output[0].value[j] = funcDetent(setting[2 * j + 3], xDetent);
		output[1].value[j] = funcDetent(setting[2 * j + 4], yDetent);
		//if (xyTrackingPreset)
		//    outputValue[2][j] = 1;
		//else {
		if (memory[0] == j)
			output[2].value[j] = 1;
		else
			output[2].value[j] = 0;
		//}
	}
	output[0].dimensions[0] = setting[0];
	output[1].dimensions[0] = setting[0];
	output[2].dimensions[0] = setting[0];

	return 3;
}


bool Canvas_Geometry_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {

	int j;
	if (check) {
		//  find the current dial position
		bool hitOne = false;
		int n = setting[0];
		for (j = n - 1; j >= 0; j--) { // scan backwards so we pick the last one drawn (the top visible)

			int pin = 0;
			double inputSize = DES_KNOB_SIZE;
			if ((int)setting[2] & 1) { // get size
				inputSize = getLoopingValueD(pin++, j, DES_KNOB_SIZE);
				if (inputSize <= 0)
					continue;
			}
			int touchSize = max(DES_TOUCH_SIZE, inputSize);

			if ((int)setting[2] & 2) // line color
				pin += 3;
			if ((int)setting[2] & 4) // fill color
				pin += 3;
			double alpha = 1;
			if ((int)setting[2] & 8) { // alpha
				alpha = getLoopingValueD(pin++, j, 1);
				if (alpha <= 0)
					continue;
			}

			// skip tracking
			if ((int)setting[2] & 16)
				pin += 3;

			// get detent
			double xDetent = 0;
			double yDetent = 0;
			if ((int)setting[2] & 32) {
				xDetent = getLoopingValueD(pin++, j, 0);
				yDetent = getLoopingValueD(pin++, j, 0);
			}
			
			int knobX = xTo2dCanvas2(pg, funcDetent(setting[2 * j + 3], xDetent), displayMode);
			int knobY = yTo2dCanvas2(pg, funcDetent(setting[2 * j + 4], yDetent), displayMode);

			if (mouseX > knobX + touchSize)
				continue; // missed dial
			if (mouseX < knobX - touchSize)
				continue; // missed dial

			if (mouseY > knobY + touchSize)
				continue; // missed dial
			if (mouseY < knobY - touchSize)
				continue; // missed dial

			memory[0] = j;
			memory[1] = mouseX - knobX;
			memory[2] = mouseY - knobY;
			hitOne = true;
			break;
		}
		if (!hitOne) {
			memory[0] = -1;
			return false;
		}
	}
	else {
		if (memory[0] < 0)
			return false;
		j = memory[0];
	}

	setting[2 * j + 3] = canvasToSize(pg, floor(mouseX - memory[1]) - pg->canvas2Width / 2, displayMode) + pg->canvas2xOffset;
	setting[2 * j + 4] = canvasToSize(pg, pg->canvas2Height / 2 - floor(mouseY - memory[2]), displayMode) + pg->canvas2yOffset;

	method2(pg);

	return true;
}

void Canvas_Geometry_Component::onMouseUp2d(Sheet* pg) {
	if (memory[0] != -1) {
		// var j = memory[0];
		memory[0] = -1;
		//  outputValue[2][j] = 0;

		method2(pg);
	}
}


void button2dGeom_Copy_Action(Sheet* pg, int i) {
	string aString;

	int points = pg->dgrm.cL[i]->setting[0];

	for (int p = 0; p < points; p++) {
		if (p)
			aString += "\r\n";
		aString += tomString(pg->dgrm.cL[i]->output[0].value[p]);
		aString += ", ";
		aString += tomString(pg->dgrm.cL[i]->output[1].value[p]);
	}
	copyStringToClipboard(aString);
	pg->activeDialogComponent = -1;
}
void button2dGeom_Paste_Action(Sheet* pg, int i) {

	HANDLE theData = getDataFromClipboard();

	char* text = (char*)theData;
	int jt = 0;
	char buffer[50];
	int jb = 0;

	pg->dgrm.cL[i]->output[0].value.clear();
	pg->dgrm.cL[i]->output[1].value.clear();
	pg->dgrm.cL[i]->output[2].value.clear();
	pg->dgrm.cL[i]->setting.resize(3); // points, drawing mode, pin code 

	bool readingX = true;
	int points = 0;

	while (text[jt] != 0) {
		if ((jb == 0) && (text[jt] == ' ')) {
			jt++;
			continue;
		}
		if ((text[jt] == ' ') || (text[jt] == ',') || (text[jt] == '\t') || (text[jt] == '\n') || (text[jt] == '\r')) {
			buffer[jb] = 0;
			double value = atof(buffer);
			if (readingX) {
				pg->dgrm.cL[i]->output[0].value.push_back(value);
				pg->dgrm.cL[i]->setting.push_back(value);
			}
			else {
				pg->dgrm.cL[i]->output[1].value.push_back(value);
				pg->dgrm.cL[i]->setting.push_back(value);
				points++;
				pg->dgrm.cL[i]->output[2].value.push_back(0);
			}
			readingX = !readingX;
			jb = 0;
			if (((text[jt] == '\n') && (text[jt + 1] == '\r')) ||
				((text[jt] == '\r') && (text[jt + 1] == '\n'))) {
				jt++;
			}
			jt++;
		}
		else
			buffer[jb++] = text[jt++];
	}
	if (jb) {
		buffer[jb] = 0;
		double value = atof(buffer);
		if (readingX) {
			pg->dgrm.cL[i]->output[0].value.push_back(value);
			pg->dgrm.cL[i]->setting.push_back(value);
		}
		else {
			pg->dgrm.cL[i]->output[1].value.push_back(value);
			pg->dgrm.cL[i]->setting.push_back(value);
			points++;
			pg->dgrm.cL[i]->output[2].value.push_back(0);
		}
	}

	pg->dgrm.cL[i]->setting[0] = points;
	pg->dgrm.cL[i]->output[0].dimensions[0] = points;
	pg->dgrm.cL[i]->output[1].dimensions[0] = points;
	pg->dgrm.cL[i]->output[2].dimensions[0] = points;

	pg->activeDialogComponent = -1;

	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}

void Canvas_Geometry_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	int oldPoints = setting[0];
	int newPoints = doubleValues[0];

	setting[1] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0) +
		(boolValues[2] ? 4 : 0) + (boolValues[3] ? 8 : 0) +
		(boolValues[4] ? 16 : 0);
	displayMode = boolValues[5];

	if (oldPoints != newPoints) {
		if ((newPoints >= 1) && (newPoints < 99)) {

			// reset the settings
			setting.resize(2 * newPoints + 3);

			// reset the x y outputs
			output[0].value.resize(newPoints);
			output[1].value.resize(newPoints);
			output[2].value.resize(newPoints);

			if (oldPoints < newPoints) {
				for (int j = oldPoints; j < newPoints; j++) {
					output[0].value[j] = 0;
					output[1].value[j] = 0;
					output[2].value[j] = 0;
					setting[2 * j + 3] = 0;
					setting[2 * j + 4] = 0;
				}
			}

			output[0].dimensions[0] = setting[0];
			output[1].dimensions[0] = setting[0];
			output[2].dimensions[0] = setting[0];

			setting[0] = newPoints;
		}
	}
}
void doubleClick2dGeom(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Points:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	vector<string> checkLabels{ "Show the lines", "Show the knobs", "Show the numbers", "Close the Shape", "Show the Coordinates",
		"Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 1), (bool)((int)pg->dgrm.cL[i]->setting[1] & 2),
		(bool)((int)pg->dgrm.cL[i]->setting[1] & 4), (bool)((int)pg->dgrm.cL[i]->setting[1] & 8),
		(bool)((int)pg->dgrm.cL[i]->setting[1] & 16),
		(bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>the2dGeomButtonList = {
	new Button(3, 40, 100, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 100, 20, L"Options...", doubleClick2dGeom),
		
	new Button(-73, 40, 70, 20, L"Copy", button2dGeom_Copy_Action),
	new Button(-73, 60, 70, 20, L"Paste", button2dGeom_Paste_Action) };

bool Canvas_Geometry_Component::onLoad(Sheet* pg) {
	// check for old Geom
	int points = setting[0];
	int settingCount = setting.size();
	if (settingCount != points * 2 + 3) {
		setting.resize(points * 2 + 3);
		for (int j = settingCount; j >= 2; j--)
			setting[j] = setting[j-1];
		setting[2] = 0; // could pull some bits from [1]
	}

	buildPinLabels();
	return false;
}
vector<int> Canvas_Geometry_Component::GetPinGroups() { return vector<int>{ 1, 3, 3, 1, 3, 2  }; }
vector<string> Canvas_Geometry_Component::GetPinLabels() {
	return vector<string>{
		"Size",
		"Line Red", "Line Green", "Line Blue",
		"Fill Red", "Fill Green", "Fill Blue",
		"Alpha",
		"X Position", "Y Position", "Track",   // 9=Track
		"X Detent", "Y Detent" };
}
Canvas_Geometry_Component::Canvas_Geometry_Component()
	: Component(L"2dGeom", L"2D Geometry", squareShape, 0, 1, 3, 1)
{
	dialogButtonList = the2dGeomButtonList;
	setMemory(0, -1); // used to show recent changes
	setMemory(1, 0); // used to hold mouse x offset
	setMemory(2, 0); // used to hold mouse Y offset
	setSetting(0, 3);  // number of points
	setSetting(1, 7);  // drawing (code)
	setSetting(2, 0); // new pin code
	pinMaskIndex = 2;
	setSetting(3, -65);  // value of x[0]
	setSetting(4, 65);  // value of y[0]
	setSetting(5, 45);  // value of x[1]
	setSetting(6, 55);  // value of y[1]
	setSetting(7, 15);  // value of x[2]
	setSetting(8, -25);  // value of y[2]
	setOutput(0, 0, setting[3]);
	setOutput(1, 0, setting[4]);
	setOutput(2, 0, 0);
	setOutput(0, 1, setting[5]);
	setOutput(1, 1, setting[6]);
	setOutput(2, 1, 0);
	setOutput(0, 2, setting[7]);
	setOutput(1, 2, setting[8]);
	setOutput(2, 2, 0);
	output[0].dimensions[0] = setting[0];
	output[1].dimensions[0] = setting[0];
	output[2].dimensions[0] = setting[0];
	width = 45;
	setRightLabels(3, "X", "Y", "Update");
	//getInternals = getInternals_2dGeom;
	//setInternals = setInternals_2dGeom;
	completionTestDoubles = 1;
	completionTestChecks = 6;
}

// ************************************ Text Spinner ************************************

int Canvas_TextSpinner_Component::method2(Sheet* pg) { 
	dial2d(pg); 
	return true; 
}

Canvas_TextSpinner_Component::Canvas_TextSpinner_Component()
	: Component(L"2dTxSpin", L"2D Text Spinner", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialog2dTxSpinButtonList;
//	method2d = dial2dTxSpin_;
//	crank2d = crank2dTxSpin_;
//	onLoad = onLoad2dTxSpin_;
//	onMouseUp2d = onMouseUp2dNumSpin_; // this clears the Halo circle
//	setLeftLabels(3, "X", "Y", "Size", "Text");
//	setRightLabels(3, "Index");
//	width = 30;
//	swipping = -1;            // needed for new Swipping
//	setSetting(0, 1 + 128 + 1024); // options: 1=position & Size, 2=Anchor, 4=Frame, 8=preset, 16=color, 32=alpha, 64=text, 128=H outs, 256=V outs, 512=standard out
//	setSetting(1, 0); // initial text index
//	setMemory(0, 0); // edge detector
	//getInternals = getInternals_1up;
	//setInternals = setInternals_1up_1out;
}



// ************************************ Number Spinner ************************************

string Canvas_NumberSpinner_Component::genNumberString(Sheet* pg, int j, int decimal, bool stripSign, bool trimZeros) {

	decimal = floor(decimal);
	if (decimal < 0)
		decimal = 0;
	double scaleFactor = pow(10, decimal);
	string stringValue;
	if (stripSign)
		stringValue = tomString(floor(0.5 + abs(setting[1 + j] * scaleFactor)));
	else
		stringValue = tomString(floor(0.5 + setting[1 + j] * scaleFactor));
	bool negative = false;
	if (decimal > 0) {
		if (stringValue[0] == '-') {
			negative = true;
			stringValue = stringValue.substr(1, string::npos);
		}
		if (decimal >= stringValue.size()) {
			int leadingZerosNeeded = decimal - stringValue.size() + 1;
			for (int g = 0; g < leadingZerosNeeded; g++)
				stringValue = '0' + stringValue;
		}
		int decPosition = stringValue.size() - decimal;
		
		if (negative)
			stringValue = '-' + stringValue.substr(0, decPosition) + '.' + stringValue.substr(decPosition, string::npos);
		else
			stringValue = stringValue.substr(0, decPosition) + '.' + stringValue.substr(decPosition, string::npos);
		
		if (trimZeros) {
			int last = stringValue.size() - 1;
			while (stringValue[last] == '0' )
				stringValue = stringValue.substr(0, last--);
			if (stringValue[last] == '.')
				stringValue = stringValue.substr(0, last--);
		}
	}
	return stringValue;
}



int Canvas_NumberSpinner_Component::method2(Sheet* pg) {
	
	// this is the spinner  - send setting out
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return 0;

	setting.resize(loops + 1);
	output[0].value.resize(loops);
	output[1].value.resize(loops);

	// NOTE: this causes the values to all be the same as the first
	if (loops > output[0].value.size()) {
		for (int k = 0; k < loops; k++) {
			setting[k + 1] = setting[1];
			output[0].value[k] = setting[k + 1];
		}
	}

	bool checkPreset = false;
	bool hasDecimal = false;
	bool hasLimits = false;

	for (int k = 0; k < loops; k++) {

		int pin = 0;
		double inputX = 0;
		double inputY = 0;
		double size = 20;
		if ((int)setting[0] & bitSetArray[0]) { // position and size: ["X", "Y", "Size"]
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			size = getLoopingValueD(pin++, k, 20);
		}

		int hAnchor = 0;
		int vAnchor = 0;
		if ((int)setting[0] & bitSetArray[1]) { // anchors: ["Horiz Anchor", "Vert Anchor"]
			hAnchor = getLoopingValueD(pin++, k, 0);
			vAnchor = getLoopingValueD(pin++, k, 0);
		}

		if ((int)setting[0] & bitSetArray[2]) // skip chevron: ["Chev Position", "Chev Size", "Chev Count"]
			pin += 3;
		if ((int)setting[0] & bitSetArray[3]) // skip frame: ["Frame"]
			pin++;

		int presetPin;
		if ((int)setting[0] & bitSetArray[4]) { // preset: ["Preset", "Set"]
			presetPin = pin;
			checkPreset = true;
			pin += 2;
		}

		int decimals = 0;
		int decimalPin = pin;
		if ((int)setting[0] & bitSetArray[5]) { // decimal: ["Decimal"]
			hasDecimal = true;
			decimals = floor(getLoopingValueD(decimalPin, k, 0));
		}
		
		int limitPin;
		if ((int)setting[0] & bitSetArray[6]) { // limits: ["Minimum", "Maximum"]
			hasLimits = true;
			limitPin = pin;
			pin += 2;
		}

		if ((int)setting[0] & bitSetArray[7]) // skip color: ["Red", "Green", "Blue"]
			pin += 3;
		if ((int)setting[0] & bitSetArray[8]) {// skip alpha: ["Alpha"]
			if (getLoopingValueD(pin++, k, 0) <= 0) {
				output[0].value[k] = 0;
				output[1].value[k] = inputX;
				continue;
			}
		}
		bool stripSign = false;
		bool trimZeros = false;
		if ((int)setting[0] & bitSetArray[9]) { // sign and zeros
			stripSign = getLoopingValueD(pin++, k, 0);
			trimZeros = getLoopingValueD(pin++, k, 0);
		}

		bool initialEnabled = false;
		bool initialValue = 0;
		//if ((int)setting[0] & bitSetArray[13]) {  // get initial settings: ["Initial Value"]
		//	initialEnabled = true;
		//	initialValue = getLoopingValueD(pin++, k, 0);
		//}

		double value = 0;
		double minimum = 0;
		double maximum = 0;
		if (checkPreset) {
			double scaleFactor = 1;
			if (hasDecimal)
				scaleFactor = pow(10, decimals);

			double presetValue = getLoopingValueD(presetPin, k, 0);
			double setValue = getLoopingValueD(presetPin + 1, k, 0);
		
			if (hasLimits) {
				minimum = getLoopingValueD(limitPin, k, -1e10);
				minimum = round(minimum * scaleFactor) / scaleFactor;
				maximum = getLoopingValueD(limitPin + 1, k, 1e10);
				maximum = round(maximum * scaleFactor) / scaleFactor;
			}
			if (setValue) { // preset
				value = round(presetValue * scaleFactor) / scaleFactor; // clean it up
				if (hasLimits) {
					if (value < minimum)
						value = minimum;
					if (value > maximum)
						value = maximum;
				}
				setting[1 + k] = value;
			}
		}
		else {
			if (initialEnabled) {
				value = initialValue;
				if (initialMemory[k] != value) {
					initialMemory[k] = value;
					if (hasLimits) {
						if (value < minimum)
							value = minimum;
						if (value > maximum)
							value = maximum;
					}
					setting[1 + k] = value;
				}
			}
		}

		int xx;
		int textWidth;
		int fontSize;
		//if ((int)setting[0] & (bitSetArray[9] + bitSetArray[10])) { // either output
			// convert to pixels
			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		//	int yCenter = yTo2dCanvas2(pg, inputY, displayMode);
			fontSize = sizeTo2dCanvas(pg, size, displayMode);
			
			string stringValue = genNumberString(pg, k, decimals, stripSign, trimZeros);
			textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, stringValue);
			if( textWidth == 0 )
				textWidth = calcStringWidthFontSize(pg->hDC, fontSize, stringValue);
			if (textWidth == 0)
				textWidth = calcStringWidthFontSize(magic_hDC, fontSize, stringValue);

			xx = xCenter - textWidth / 2; // first center the text horizontaly
			//yy = yCenter - fontSize / 2;      // first center the text vertically
			xx -= hAnchor * textWidth / 2;  // then offset to horiz anchor
			//yy += vAnchor * fontSize / 2;       // then offset to vert anchor
		//}

		output[0].value[k] = setting[1 + k]; // value
		output[1].value[k] = inputX - (hAnchor * textWidth / 2) + textWidth/2; // right
	}
	//dial2d(pg); // think about it
	return 3;
}

bool Canvas_NumberSpinner_Component::dial2d(Sheet* pg) {
	//if (pg->pCanvasWindow == NULL) --- need to calculate x output even when not displayed
	//	return false;

	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return false;

	bool checkPreset = false;
	bool hasDecimal = false;
	bool hasLimits = false;

	chevronCenterX.resize(loops); // set by the dial routine
	chevronStepX.resize(loops);
	chevronMinY.resize(loops);
	chevronMaxY.resize(loops);
	chevronCount.resize(loops);
	swippingMinimum.resize(loops);
	swippingMaximum.resize(loops);

	for (int k = 0; k < loops; k++) {

		int pin = 0;
		double inputX = 0;
		double inputY = 0;
		double size = 20;
		if ((int)setting[0] & bitSetArray[0]) { // position and size: ["X", "Y", "Size"]
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			size = getLoopingValueD(pin++, k, 20);
			if (size <= 0)
				continue;
		}

		int hAnchor = 0;
		int vAnchor = 0;
		if ((int)setting[0] & bitSetArray[1]) { // anchors: ["Horiz Anchor", "Vert Anchor"]
			hAnchor = getLoopingValueD(pin++, k, 0);
			vAnchor = getLoopingValueD(pin++, k, 0);
		}

		int inChevronPosition = 0;
		int	inChevronSize = 2;
		int	inChevronCount = 1;
		if ((int)setting[0] & bitSetArray[2]) { // chevron: ["Chev Position", "Chev Size", "Chev Count"]
			inChevronPosition = getLoopingValueD(pin++, k, 0);
			inChevronSize = getLoopingValueD(pin++, k, 2);
			inChevronCount = getLoopingValueD(pin++, k, 1);
		}

		int frame = 0;
		if ((int)setting[0] & bitSetArray[3]) // frame: ["Frame"]
			frame = getLoopingValueD(pin++, k, 0);

		int presetPin = pin;
		if ((int)setting[0] & bitSetArray[4]) { // preset: ["Preset", "Set"]
			checkPreset = true;
			pin += 2;
		}
	
		int decimals = 0;
		if ((int)setting[0] & bitSetArray[5]) { // decimal: ["Decimal"]
			hasDecimal = true;
			decimals = floor(getLoopingValueD(pin++, k, 0));
		}

		swippingMinimum[k] = -1e10;
		swippingMaximum[k] = 1e10;
		if ((int)setting[0] & bitSetArray[6]) { // limits: ["Minimum", "Maximum"]
			hasLimits = true;
			swippingMinimum[k] = getLoopingValueD(pin++, k, -1e10);
			swippingMaximum[k] = getLoopingValueD(pin++, k, 1e10);
		}

		// if no color enabled use white
		int red = 255;
		int green = 255;
		int blue = 255;
		bool haveColor = false;
		if ((int)setting[0] & bitSetArray[7]) { //  color: ["Red", "Green", "Blue"]
			red = getLoopingValueD(pin++, k, 1);
			green = getLoopingValueD(pin++, k, 1);
			blue = getLoopingValueD(pin++, k, 1);
			haveColor = true;
		}

		double alpha = 1;
		if ((int)setting[0] & bitSetArray[8]) // alpha: ["Alpha"]
			alpha = getLoopingValueD(pin++, k, 1);
		if (alpha <= 0) {
			output[0].value[k] = 0;
			output[1].value[k] = inputX;
			continue;
		}

		bool stripSign = false;
		bool trimZeros = false;
		if ((int)setting[0] & bitSetArray[9]) { // sign and zeros
			stripSign = getLoopingValueD(pin++, k, 0);
			trimZeros = getLoopingValueD(pin++, k, 0);
		}

		// pick a complementry text color - most visible text
		COLORREF textColor;
		if ((0.64 * red + 0.77 * green + 0.5 * blue) > 230)
			textColor = RGB(0, 0, 0);// , alpha.value);
		else
			textColor = RGB(255, 255, 255);// , alpha.value);

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		int fontSize = sizeTo2dCanvas(pg, size, displayMode);
		string stringValue = genNumberString(pg, k, decimals, stripSign, trimZeros);
		int textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, stringValue);
		if( textWidth == 0 )
			textWidth = calcStringWidthFontSize(pg->hDC, fontSize, stringValue);
		if (textWidth == 0)
			textWidth = calcStringWidthFontSize(magic_hDC, fontSize, stringValue);
		int textY = yCenter - (11*fontSize/16);
		textY += vAnchor * fontSize / 2;
	
		int xx = xCenter - textWidth / 2; // first center the text horizontaly
		int yy = yCenter - fontSize / 2;      // first center the text vertically
		xx -= hAnchor * textWidth / 2;  // then offset to horiz anchor
		yy += vAnchor * fontSize / 2;       // then offset to vert anchor

		COLORREF theColor = RGB(red, green, blue);// , alpha.value);
		COLORREF whiteColor = RGB(255, 255, 255);
		COLORREF blackColor = RGB(0, 0, 0);

		// pick a complementry text color - most visible text
		if ((0.64 * red + 0.77 * green + 0.5 * blue) > 230)
			textColor = blackColor;
		else
			textColor = whiteColor;

		// fill the background. Hide if color is not enabled and Frame is set to 0
		if ((frame > 0) || haveColor) { // color
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, xx - 2,			yy - 2, NULL);
			LineTo(pg->hDC2, xx - 2,				yy + fontSize + 4);
			LineTo(pg->hDC2, xx + textWidth + 4,	yy + fontSize + 4);
			LineTo(pg->hDC2, xx + textWidth + 4,	yy - 2);
			LineTo(pg->hDC2, xx - 2,				yy - 2);
			EndPath(pg->hDC2);
		
			if ((frame > 0) && haveColor) { //frame and fill it
				HPEN hPen = CreatePen(PS_COSMETIC, frame, blackColor);
				SelectObject(pg->hDC2, hPen);
				HBRUSH fillColor = CreateSolidBrush(theColor);
				SelectObject(pg->hDC2, fillColor);
				StrokeAndFillPath(pg->hDC2);
				DeleteObject(hPen);
				DeleteObject(fillColor);
			}
			if ((frame > 0) && ! haveColor) { // just frame it
				HPEN hPen = CreatePen(PS_COSMETIC, frame, blackColor);
				SelectObject(pg->hDC2, hPen);
				StrokePath(pg->hDC2);
				DeleteObject(hPen);
			}
			if ((frame <= 0) && haveColor) { // just fill it
				HBRUSH fillColor = CreateSolidBrush(theColor);
				SelectObject(pg->hDC2, fillColor);
				StrokePath(pg->hDC2);
				DeleteObject(fillColor);
			}
		}

		if (inChevronSize > 0) { // 1 is chevron size
			// draw the chevrons
			int chevCenterX = xx + textWidth / 2;
			chevCenterX += hAnchor * textWidth / 2; // chevron position??
			chevCenterX += inChevronPosition * fontSize;

			inChevronSize /= 2;
			yCenter += vAnchor * fontSize / 2;
			int chevTopY = yCenter - fontSize / 2 - fontSize * inChevronSize / 2;
			int chevBotY = yCenter + fontSize / 2 + 4 * fontSize * inChevronSize / 10;
			int chevHorizStep = fontSize * inChevronSize / 4;
			int chevVertStep = fontSize * inChevronSize / 3;
			int chevCount = floor(inChevronCount);
			chevCenterX += (chevCount - 1) * 2.5 * chevHorizStep; // shift the start for multiple chevrons

			// save for the crank function
			chevronCenterX[k] = chevCenterX;
			chevronStepX[k] = chevHorizStep;
			chevronMinY[k] = yCenter - DES_TOUCH_SIZE;
			chevronMaxY[k] = yCenter + DES_TOUCH_SIZE;
			chevronCount[k] = chevCount;

			COLORREF strokeStyle = RGB(128, 128, 128);
			for (int ch = 0; ch < chevCount; ch++) {

				if (setting[k + 1] < swippingMaximum[k]) {
					BeginPath(pg->hDC2);
					MoveToEx(pg->hDC2, chevCenterX - chevHorizStep, chevTopY + chevVertStep, NULL);
					LineTo(pg->hDC2, chevCenterX, chevTopY); // tip of top
					LineTo(pg->hDC2, chevCenterX + chevHorizStep, chevTopY + chevVertStep);
					EndPath(pg->hDC2);
					StrokePath(pg->hDC2);
					//strokeStyle = RGB(128, 128, 128)
				}
				if (setting[k + 1] > swippingMinimum[k]) {
		
					BeginPath(pg->hDC2);
					MoveToEx(pg->hDC2, chevCenterX - chevHorizStep, chevBotY - chevVertStep, NULL);
					LineTo(pg->hDC2, chevCenterX, chevBotY); // dip on bottom
					LineTo(pg->hDC2, chevCenterX + chevHorizStep, chevBotY - chevVertStep);
					EndPath(pg->hDC2);
					StrokePath(pg->hDC2);
				}
				// the Halo
				//if (swipping == k) {
				//	if (swippingChevron == ch) {
				//
				//		int delta = swippingInitialValue - setting[1 + k];
				//		delta *= swippingSize / swippingEffect;
				//		delta *= swippingScaleFactor;
				//		delta *= pg->canvas2Scale;
				//		BeginPath(pg->hDC2);
				//		Ellipse(pg->hDC2, chevCenterX - DES_HALO_SIZE, swippingMouseDownY + delta - DES_HALO_SIZE, 
				//			                chevCenterX + DES_HALO_SIZE, swippingMouseDownY + delta + DES_HALO_SIZE);
				//		//ctx2.fillStyle = blackAlpha(DES.HALO_ALPHA);
				//		EndPath(pg->hDC2);
				//		FillPath(pg->hDC2);
				//	}
				//}
				// the touch area - DIAGNOSTIC
				//if (DES.TOUCH_ALPHA) {
				//	var touchSize = Math.max(DES.TOUCH_SIZE, (chevBotY - chevTopY) / 2);
				//	//  ctx2.beginPath();
				//	//    ctx2.arc(chevCenterX, yCenter, touchSize, 0, TWO_PI);
				//	ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES.TOUCH_ALPHA);
				//	ctx2.fillRect(chevCenterX - DES.TOUCH_SIZE, yCenter - touchSize, 2 * DES.TOUCH_SIZE, 2 * touchSize);
				//}
				chevCenterX -= 5 * chevHorizStep;
			}
		}

		//  display the value
		drawTextString(pg->hDC2, xx, textY, fontSize, 0, stringValue);
		output[1].value[k] = inputX - (hAnchor * textWidth / 2) + textWidth / 2; // right
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

bool Canvas_NumberSpinner_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	
	double scaleFactor = 1;

	if (check) {
		// search for the spinner under the mouse
		swipping = -1; // clear it
		
		for (int k = loops - 1; k >= 0; k--) { // scan backwards
			int pin = 0;
			double inputX = 0;
			double	inputY = 0;
			double	size = DES_SPINNER_TEXT_SIZE;
			if ((int)setting[0] & bitSetArray[0]) { // position and size
				inputX = getLoopingValueD(pin++, k, 0);
				inputY = getLoopingValueD(pin++, k, 0);
				size = getLoopingValueD(pin++, k, DES_SPINNER_TEXT_SIZE);
			}
			
			if ((int)setting[0] & bitSetArray[1]) // anchors
				pin += 2;
			if ((int)setting[0] & bitSetArray[2]) // chevrons
				pin += 3;

			// convert to pixels
			//var xCenter = xTo2dCanvas(inputX);
			//var yCenter = yTo2dCanvas(inputY);

			// gather the limits
			if ((int)setting[0] & bitSetArray[3]) // frame
				pin++;
			if ((int)setting[0] & bitSetArray[4]) // preset
				pin += 2;
			if ((int)setting[0] & bitSetArray[5]) { // decimal
				scaleFactor = pow(10, floor(getLoopingValueD(pin, k, 0)));
				pin++;
			}
			if ((int)setting[0] & bitSetArray[6]) { // limits
				swippingMinimum[k] = getLoopingValueD(pin++, k, -1e10);
				swippingMinimum[k] = round(swippingMinimum[k] * scaleFactor) / scaleFactor;
				swippingMaximum[k] = getLoopingValueD(pin++, k, 1e10);
				swippingMaximum[k] = round(swippingMaximum[k] * scaleFactor) / scaleFactor;
			}
			if ((int)setting[0] & bitSetArray[7]) { // color
				pin += 3;
			}

			double alpha = 1;
			if ((int)setting[0] & bitSetArray[8]) { // alpha
				alpha = getLoopingValueD(pin++, k, 1);
				if (alpha == 0) {
					output[0].value[k] = 0;
					output[1].value[k] = inputX;
					continue;
				}
			}

			int centerX = chevronCenterX[k];
			int effect = 1;
			for (int ch = 0; ch < chevronCount[k]; ch++) {

				//ctx2.strokeStyle = '#FF0000'; // the sensitive area in red
				//ctx2.strokeRect(xCenter-DES.TOUCH_SIZE, chevronMinY[k], 2*DES.TOUCH_SIZE, chevronMaxY[k]-chevronMinY[k]);

				if (ch) {
					// shift to next chevron
					centerX -= 5 * chevronStepX[k];
					effect *= 10;
				}

				int multiChevronLeftEdgeCorrection = 0;
				if (ch == chevronCount[k] - 1) // last one
					multiChevronLeftEdgeCorrection = 0;
				else
					multiChevronLeftEdgeCorrection = 3 * chevronStepX[k];

				if (mouseX > centerX + DES_TOUCH_SIZE)
					continue; // missed
				if (mouseX < centerX - DES_TOUCH_SIZE + multiChevronLeftEdgeCorrection)
					continue; // missed

				if (mouseY > chevronMaxY[k])
					continue; // missed
				if (mouseY < chevronMinY[k])
					continue; // missed

				// mouse is down in the zone !
				swippingMouseDownY = mouseY;
				swippingInitialValue = setting[1 + k];
				swippingSize = sizeTo2dCanvas(pg, size/4, displayMode);
				swippingScaleFactor = scaleFactor;
				swippingEffect = effect;
				swippingChevron = ch;
				swipping = k;
				return true;
			}
		}
		return false;
	}

	if (swipping != -1) {
		int k = swipping;
		int stepSize = swippingSize;
		double delta = -(mouseY - swippingMouseDownY) / stepSize;
		delta *= swippingEffect / swippingScaleFactor;
		double newValue = swippingInitialValue + delta;
		if (newValue > swippingMaximum[k])
			newValue = swippingMaximum[k];
		if (newValue < swippingMinimum[k])
			newValue = swippingMinimum[k];

		setting[1 + k] = newValue;
		setting[1 + k] = round(setting[1 + k] * swippingScaleFactor) / swippingScaleFactor;
		output[0].value[k] = setting[1 + k];

		// save delta so we can hilight one of the chevrons
		swippingDelta = delta;
		return true;
	}

	return false;
}
std::vector<Button*>the2dNumSpinButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayMode) };

bool Canvas_NumberSpinner_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return true;
}

vector<int> Canvas_NumberSpinner_Component::GetPinGroups() { return vector<int>{3, 2, 3, 1, 2, 1, 2, 3, 1, 2 }; }
vector<string> Canvas_NumberSpinner_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Size",              // 0=Position and size (1)
		"Horiz Anchor", "Vert Anchor", // 1=anchors       (2)
		"Chev Position", "Chev Size", "Chev Count",  // 2=chevron       (4)
		"Frame",                       // 3=frame         (8)
		"Preset", "Set",               // 4=preset       (16)
		"Decimal",                     // 5=decimal      (32)
		"Minimum", "Maximum",          // 6=limits       (64)
		"Red", "Green", "Blue",        // 7= Color      (128)
		"Alpha",                       // 8=alpha       (256)
		"Strip Sign", "Trim Zeros"};    // 9=additional	(512)
}

void Canvas_NumberSpinner_Component::onMouseUp2d(Sheet* pg) {
	
	swipping = -1;
	drawCanvas2();
}

Canvas_NumberSpinner_Component::Canvas_NumberSpinner_Component()
	: Component(L"2dNumSpin", L"2D Number Spinner", squareShape, 3, 1, 2, 1)
{
	dialogButtonList = the2dNumSpinButtonList;
	setRightLabels(3, "Value", "X");
	setSetting(0, 1); // 1=position & size
	setSetting(1, 0); // array of values[0]
	pinMaskIndex = 0;
	width = 50;
	//getInternals = getInternals_1up;
	//setInternals = setInternals_1up_1out;
	setOutputDimension(0, 0, 1);
	setOutputDimension(1, 0, 1);
}

// ************************************ Spinner ************************************

bool Canvas_Spinner_Component::dial2d(Sheet* pg) {
	//if (pg->pCanvasWindow == NULL)
	//	return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputWidth = 100;
		double inputHeight = 20;
		if ((int)setting[0] & 1) {
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 20);
		}
		if ((inputWidth <= 0) || (inputHeight <= 0))
			continue;

		double inputFrameWidth = 1;
		if ((int)setting[0] & 2) {
			inputFrameWidth = getLoopingValueD(pin++, k, 1);
		}

		int inputDecimal = 0;
		if ((int)setting[0] & 4) {
			inputDecimal = getLoopingValueD(pin++, k, 0);
		}

		int inputPreset = 0;
		int inputSet = 0;
		if ((int)setting[0] & 8) {
			inputPreset = getLoopingValueD(pin++, k, 0);
			inputSet = getLoopingValueD(pin++, k, 0);
		}

		string prepend;
		string append;
		if ((int)setting[0] & 16) {
			if (input[pin].drivenByComponent != -1)
				if (input[pin].stringType)
					prepend = input[pin].stringValue[0];
			pin++;
			if (input[pin].drivenByComponent != -1)
				if (input[pin].stringType)
					append = input[pin].stringValue[0];
			pin++;
		}

		double inputLimitLow = -1e10;
		double inputLimitHigh = 1e10;
		if ((int)setting[0] & 32) {
			inputLimitLow = getLoopingValueD(pin++, k, -1e10);
			inputLimitHigh = getLoopingValueD(pin++, k, 1e10);
		}

		int inputRed = 255;
		int inputGreen = 255;
		int inputBlue = 255;
		if ((int)setting[0] & 64) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputAlpha = 1.0;
		if ((int)setting[0] & 128) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		double scaleFactor = pow(10, floor(inputDecimal));

		// pick a complementry text color - most visible text
		COLORREF textColor;
		if ((0.64 * inputRed + 0.77 * inputGreen + 0.5 * inputBlue) > 230)
			textColor = RGB(0, 0, 0);// , alpha.value);
		else
			textColor = RGB(255, 255, 255);// , alpha.value);

		// convert to pixels
		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);


		inputWidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
		if ((inputWidth <= 0) || (inputWidth > 10000))
			continue;
		inputHeight = sizeTo2dCanvas(pg, inputHeight, displayMode);
		if ((inputHeight <= 0) || (inputHeight > 10000))
			continue;
		int knobWidth = inputHeight / 2;

		int xx = xCenter - inputWidth /2;
		int yy = yCenter - inputHeight /2;

		COLORREF theColor = RGB(inputRed, inputGreen, inputBlue); //, alpha.value);
		COLORREF blackColor = 0x000000;

		// fill the background. Hide if color is not enabled and Frame is set to 0
		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, xCenter - inputWidth / 2, yCenter - inputHeight / 2, NULL);
		LineTo(pg->hDC2, xCenter - inputWidth / 2, yCenter + inputHeight / 2);
		LineTo(pg->hDC2, xCenter + inputWidth / 2, yCenter + inputHeight / 2);
		LineTo(pg->hDC2, xCenter + inputWidth / 2, yCenter - inputHeight / 2);
		LineTo(pg->hDC2, xCenter - inputWidth / 2, yCenter - inputHeight / 2);
		EndPath(pg->hDC2);

		bool frameIt = false;
		bool fillIt = false;
		if (((int)setting[0] & 64) == 64)
			fillIt = true;
		if (inputFrameWidth > 0)
			frameIt = true;

		HBRUSH fillColor = NULL;
		HPEN hPen = NULL;
		if (fillIt) {
			fillColor = CreateSolidBrush(theColor);
			SelectObject(pg->hDC2, fillColor);
		}
		if (frameIt) {
			hPen = CreatePen(PS_COSMETIC, inputFrameWidth, blackColor);
			SelectObject(pg->hDC2, hPen);
		}

		if (fillIt && frameIt)
			StrokeAndFillPath(pg->hDC2);
		else
		if (fillIt)
			FillPath(pg->hDC2);
		else
		if (frameIt)
			StrokePath(pg->hDC2);
	
		if (fillIt)
			DeleteObject(fillColor);
		if (frameIt)
			DeleteObject(hPen);

		
		//  display the value
		//ctx2.fillStyle = textColor;
		int size2 = height * 1.3724 - 6.85;
		if (size2 > 5) {
			//ctx2.font = size.tomString() + 'px ' + default_2D_FontFamily;
			string stringValue = tomString(floor(0.5 + setting[1 + k] * scaleFactor));
			int decimal2 = floor(inputDecimal);
			bool negative = false;
			if (decimal2 > 0) {
				if (stringValue[0] == '-') {
					negative = true;
					stringValue = stringValue.substr(1);
				}
				if (decimal2 >= stringValue.size()) {
					int zerosNeeded = inputDecimal - stringValue.size() + 1;
					for (int g = 0; g < zerosNeeded; g++)
						stringValue = '0' + stringValue;
				}
				int decPosition = stringValue.size() - inputDecimal;
				if (negative)
					stringValue = '-' + stringValue.substr(0, decPosition) + '.' + stringValue.substr(decPosition);
				else
					stringValue = stringValue.substr(0, decPosition) + '.' + stringValue.substr(decPosition);
			}
			stringValue = prepend + stringValue + append;
			string trimText = stringValue;
			int fontSize = sizeTo2dCanvas(pg, 14, displayMode);
			int lift = sizeTo2dCanvas(pg, 18, displayMode);
			int textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, trimText);
			if (textWidth < inputWidth - knobWidth - 1)
				drawTextString(pg->hDC2, xx + inputWidth - textWidth - knobWidth - 2, yy + inputHeight - lift, fontSize, textColor, trimText);
		}

		int knobMaxX = xCenter + inputWidth / 2;
		int knobMinX = knobMaxX - knobWidth;

		int knobMaxY = yCenter - inputHeight / 2;
		int knobMinY = yCenter + inputHeight / 2;

		if (inputFrameWidth > 0) {
			// 2 lines create the boxes
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, knobMinX, knobMinY, NULL);
			LineTo(pg->hDC2, knobMinX, knobMaxY);
			
			MoveToEx(pg->hDC2, knobMaxX, yCenter, NULL);
			LineTo(pg->hDC2, knobMinX, yCenter);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}

		double chevCenterX = (knobMaxX + knobMinX) / 2;
		double chevHorizStep = (knobMaxX - knobMinX) / 4;
		double chevVertStep = (knobMaxY - knobMinY) / 6;

		double minimum = round(inputLimitLow * scaleFactor) / scaleFactor;
		double maximum = round(inputLimitHigh * scaleFactor) / scaleFactor;

		if (setting[1 + k] > minimum) {
			// upward pointing chevron
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, chevCenterX - chevHorizStep, knobMinY + 2 * chevVertStep, NULL);
			LineTo(pg->hDC2, chevCenterX, knobMinY + chevVertStep);
			LineTo(pg->hDC2, chevCenterX + chevHorizStep, knobMinY + 2 * chevVertStep);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		if (setting[1 + k] < maximum) {
			// downward pointing chevron
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, chevCenterX - chevHorizStep, knobMaxY - 2 * chevVertStep, NULL);
			LineTo(pg->hDC2, chevCenterX, knobMaxY - chevVertStep);
			LineTo(pg->hDC2, chevCenterX + chevHorizStep, knobMaxY - 2 * chevVertStep);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}

		// if there is a user label put below
		if (userLabel.size()) {
			int fontSize = sizeTo2dCanvas( pg, 12, displayMode);
			int textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, userLabel);
			drawTextString(pg->hDC2, xCenter - textWidth / 2, yCenter + height / 2 + 4, fontSize, blackColor, userLabel);
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

int Canvas_Spinner_Component::method2(Sheet* pg) {

	// this is the spinner  - send setting out
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	//int changed = 0;
	int oldSize = output[0].value.size();
	if (loops != oldSize) {

		setting.resize(1 + loops);
		output[0].value.resize(loops);

		for (int k = oldSize; k < loops; k++) {
			setting[1 + k] = setting[k];
			output[0].value[k] = setting[k];
		}
	}
	int presetPin = 4; // position and size
	if ((int)setting[0] & 2) // frame
		presetPin++;

	bool hasDecimal = false;
	int decimalPin;
	if ((int)setting[0] & 4) { // decimal
		hasDecimal = true;
		decimalPin = presetPin;
		presetPin++;
	}

	if ((int)setting[0] & 8) { // preset
		for (int k = 0; k < loops; k++) {
			double scaleFactor = 1;
			if (hasDecimal)
				scaleFactor = pow(10, floor(getLoopingValueD(decimalPin, k, 0)));

			double presetValue = getLoopingValueD(presetPin, k, 0);
			double setValue = getLoopingValueD(presetPin + 1, k, 0);
			if (setValue) { // preset
				double value = round(presetValue * scaleFactor) / scaleFactor; // clean it up
				if ((int)setting[0] & 32) { // limits
					double minimum = getLoopingValueD(presetPin + 2, k, -1e10);
					minimum = round(minimum * scaleFactor) / scaleFactor;
					double maximum = getLoopingValueD(presetPin + 3, k, 1e10);
					maximum = round(maximum * scaleFactor) / scaleFactor;

					if (value < minimum)
						value = minimum;
					if (value > maximum)
						value = maximum;
				}

				setting[1 + k] = value;
				output[0].value[k] = value;
				//changed = 3;
			}
		}
	}
	
	output[0].dimensions[0] = loops;
	return 3;
}
bool Canvas_Spinner_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {

	if (check) {
		// search for the spinner buttons under the mouse
		int loops = inputsToLoops();
		if (loops == 0)
			return false;

		bool hit = false;
		for (int k = loops - 1; k >= 0; k--) { // scan backwards

			double inputX = getLoopingValueD(0, k, 0);
			double inputY = getLoopingValueD(1, k, 0);
			double inputWidth = getLoopingValueD(2, k, 100);
			double inputHeight = getLoopingValueD(3, k, 20);
			double knobSize = 8;

			if ((inputWidth <= 0) || (inputHeight <= 0))
				continue;
			// convert to pixels
			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			inputWidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
			inputHeight = sizeTo2dCanvas(pg, inputHeight, displayMode);
			knobSize = sizeTo2dCanvas(pg, knobSize, displayMode);

			// gather the limits
			double minimum = -1e10;
			double maximum = 1e10;
			double scaleFactor = 1;
			int pin = 2 + 2; // position and size
			if ((int)setting[0] & 2) // frame
				pin++;
			if ((int)setting[0] & 4) { // decimal
				scaleFactor = pow(10, floor(getLoopingValueD(pin, k, 0)));
				pin++;
			}
			if ((int)setting[0] & 8) // preset
				pin += 2;
			if ((int)setting[0] & 16) // text
				pin += 2;
			if ((int)setting[0] & 32) { // if limits
				minimum = getLoopingValueD(pin++, k, -1e10);
				minimum = round(minimum * scaleFactor) / scaleFactor;
				maximum = getLoopingValueD(pin++, k, 1e10);
				maximum = round(maximum * scaleFactor) / scaleFactor;
			}
			if ((int)setting[0] & 64) // color
				pin += 3;
			double alpha = 1;
			if ((int)setting[0] & 128) { // alpha
				alpha = getLoopingValueD(pin, k, 1);
				if (alpha <= 0)
					continue;
			}

			// check for overall hit
			double maxX = xCenter + inputWidth / 2; // right side
			double minX = maxX - knobSize;
			if (mouseX > maxX)
				continue; // missed
			if (mouseX < minX)
				continue; // missed

			int minY;
			if (setting[1 + k] < maximum)
				minY = yCenter - inputHeight / 2;
			else
				minY = yCenter;

			int maxY;
			if (setting[1 + k] > minimum)
				maxY = yCenter + inputHeight / 2;
			else
				maxY = yCenter;

			if (mouseY > maxY)
				continue; // missed
			if (mouseY < minY)
				continue; // missed

			// find the specific button
			if (mouseY < yCenter) {
				setting[1 + k] += 1 / scaleFactor; // top button
				if (setting[1 + k] > maximum)
					setting[1 + k] = maximum;
			}
			else {
				setting[1 + k] -= 1 / scaleFactor; // bottom button
				if (setting[1 + k] < minimum)
					setting[1 + k] = minimum;
			}
			setting[1 + k] = round(setting[1 + k] * scaleFactor) / scaleFactor;
			output[0].value[k] = setting[1 + k];
			hit = true;
			break;
		}
		if (!hit)
			return false;
		
		output[0].dimensions[0] = setting[1];
		//  dial2dSpin_(pg, i, k, a);
		return true;
	}
	return false;

}

std::vector<Button*>the2dSpinButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayMode) };

bool Canvas_Spinner_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

void Canvas_Spinner_Component::onMouseUp2d(Sheet* pg) {
	for (int k = 0; k < memory.size(); k++)
		memory[k] = 0;
}
vector<int> Canvas_Spinner_Component::GetPinGroups() { return vector<int>{4, 1, 1, 2, 2, 2, 3, 1}; }
vector<string> Canvas_Spinner_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Width", "Height",			// 1
		"Frame Width",							// 2
		"Decima",								// 4
		"Preset", "Set",						// 8
		"Prepend", "Append",					// 16
		"Minimum", "Maximum",					// 32
		"Fill Red", "Fill Green", "Fill Blue",	// 64
		"Alpha" };
}
Canvas_Spinner_Component::Canvas_Spinner_Component()
	: Component(L"2dSpin", L"2D Spinner", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = the2dSpinButtonList;
	setSetting(0, 1); 
	setSetting(1, 0); // array of values[0]
	pinMaskIndex = 0;
	//getInternals = getInternals_1up;
	//setInternals = setInternals_1up_1out;
}

// ************************* 2D Mouse *************************************
bool Canvas_Mouse_Component::canvasMouseMoved(Sheet* pg, int inx, int iny, bool *mouseDown, int zDelta){

	if (setting.size() < 1)
		setting[0] = 0;

	switch ((int)setting[0]) {
	case 0: // normal
		output[0].value[0] = canvasToX2(pg, inx, 0);
		output[1].value[0] = canvasToY2(pg, iny, 0);
		break;
	case 1: // ignore scale
		output[0].value[0] = canvasToX2(pg, inx, 1);
		output[1].value[0] = canvasToY2(pg, iny, 1);
		break;
	case 2: // raw
		output[0].value[0] = inx;
		output[1].value[0] = iny;
		break;
	}

	output[2].value[0] += zDelta; // the mouse wheel

	for( int k = 0; k < MOUSE_BUTTON_COUNT; k++ )
		output[3].value[k] = mouseDown[k];

	return true;
}
void Canvas_Mouse_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	setting[0] = radioValue;
}
void doubleClick2dMouse(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> radioLabels = { "Normal", "Ignore Display Scale", "Raw Window" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, {}, {}, 80, {}, {}, radioLabels, radioIndex, NULL);
}
std::vector<Button*>the2dMouseButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dMouse) };

Canvas_Mouse_Component::Canvas_Mouse_Component()
	: Component(L"Mouse", L"2D Mouse", squareShape, 0, 0, 4, 1)
{
	dialogButtonList = the2dMouseButtonList;
	output[0].value[0] = 0;
	output[1].value[0] = 0;
	output[2].value[0] = 0;
	output[3].value.resize(MOUSE_BUTTON_COUNT);
	for (int k = 0; k < MOUSE_BUTTON_COUNT; k++)
		output[3].value[k] = 0;
	setSetting(0, 0); // 0 - use 2d view coordinates, 1 - ignore scale, 2 - use raw screen
	setRightLabels(3, "X", "Y", "Z", "Button");
	setOutputDimension(0, 0, 1);
	setOutputDimension(1, 0, 1);
	setOutputDimension(2, 0, 1);
	setOutputDimension(3, 0, MOUSE_BUTTON_COUNT);
}



//**************************** 2d Sin Cos **************************

int Canvas_SinCosKnob_Component::method2(Sheet* pg) {
	// this is the sine cosine constant - send setting out
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	if (loops != memory.size()) 
		memory.resize(loops);
	if (4+loops != setting.size())
		setting.resize(4 + loops);
	if (loops != output[0].value.size())
		output[0].value.resize(loops);
	if (loops != output[1].value.size())
		output[1].value.resize(loops);
	if (loops != output[2].value.size())
		output[2].value.resize(loops);


	double ang;
	for (int k = 0; k < loops; k++) {
		if (setting.size() <= 4 + k)
			setting[4 + k] = setting[4 + k - 1]; // k NEVER ZERO
		if (setting[3]) { // detent is active
			double step = TWO_PI / setting[3];
			double temp = floor((setting[4 + k] + step / 2) / step);
			ang = temp * step;
		}
		else
			ang = setting[4 + k];
		output[0].value[k] = sin(ang);
		output[1].value[k] = cos(ang);
		output[2].value[k] = ang;
	}
	for (int j = 0; j < 3; j++)
		output[j].dimensions[0] = loops;

	return 3;
}


bool Canvas_SinCosKnob_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	bool color = false;

	double inputX = 0;
	double inputY = 0;
	double inputRadius = 50;
	double inputKnob = DES_KNOB_SIZE;
	for (int k = 0; k < loops; k++) {

		inputX = getLoopingValueD(0, k, 0);
		inputY = getLoopingValueD(1, k, 0);
		inputRadius = getLoopingValueD(2, k, 50);        // Radius
		inputKnob = getLoopingValueD(3, k, DES_KNOB_SIZE); // Knob

		COLORREF lineColor;
		COLORREF blackColor;
		COLORREF whiteColor;
		if (color) {
			int inputRed = getLoopingValueD(4, k, 0);
			int inputGreen = getLoopingValueD(5, k, 0);
			int inputBlue = getLoopingValueD(6, k, 0);
			int inputAlpha = getLoopingValueD(7, k, 1);
			//int lineColor = redGreenBlueAlpha(inputRed, inputGreen, inputBlue, inputAlpha);
			//int blackColor = blackAlpha(inputAlpha);
			//int whiteColor = whiteAlpha(inputAlpha);
		}
		else {
			lineColor = 0x000000;
			blackColor = 0x000000;
			whiteColor = 0xFFFFFF;
		}

		if (inputRadius <= 0)
			continue;
		if (inputKnob <= 0)
			continue;

		inputRadius = sizeTo2dCanvas(pg, inputRadius, displayMode);
		inputKnob = sizeTo2dCanvas(pg, inputKnob, displayMode);

		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

		double knobA = setting[4 + k];

		if (setting[3]) { // detent is active
			double step = TWO_PI / setting[3];
			double temp = floor((setting[4 + k] + step / 2) / step);
			knobA = temp * step;
		}
		else
			knobA = setting[4 + k];

		int knobX = xCenter + inputRadius * cos(knobA);
		int knobY = yCenter + inputRadius * sin(knobA);

		if ((int)setting[2] & 1) // track
		{
			//ctx2.strokeStyle = lineColor;
			//ctx2.lineWidth = 1;
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, xCenter - inputRadius, yCenter - inputRadius, xCenter + inputRadius, yCenter + inputRadius);
			EndPath(pg->hDC2);
			StrokePath(pg->hDC2);
		}
		if ((int)setting[2] & 2) // knob
		{
			//ctx2.strokeStyle = lineColor;
			SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
			//ctx2.lineWidth = 1;
			BeginPath(pg->hDC2);
			Ellipse(pg->hDC2, knobX - inputKnob, knobY - inputKnob, knobX + inputKnob, knobY + inputKnob);
			EndPath(pg->hDC2);
			//ctx2.fillStyle = whiteColor;  // white
			StrokeAndFillPath(pg->hDC2);
		}

		if ((int)setting[2] & 4) // show user text
		{
			// if there is a user label put in the center
			if (userLabel.size()) {
				//ctx2.fillStyle = blackColor;
				//ctx2.font = '9px ' + default_2D_FontFamily;
				int textWidth = calcLargeStringWidthFont(pg->hDC2, userLabel);
				drawLargeTextString(pg->hDC2, xCenter - textWidth / 2, yCenter-8, blackColor, userLabel);
			}
		}
		// the Halo
		//if (pg->dgrm.cL[i].memory[0] == k) {
		//	ctx2.beginPath();
		//	ctx2.arc(knobX, knobY, DES.HALO_SIZE, 0, TWO_PI);
		//	ctx2.fillStyle = blackAlpha(DES.HALO_ALPHA);
		//	ctx2.fill();
		//}
		// the touch area - DIAGNOSTIC
		//if (DES.TOUCH_ALPHA) {
		//	var touchSize = Math.max(DES.TOUCH_SIZE, inputKnob);
		//	ctx2.beginPath();
		//	ctx2.arc(knobX, knobY, touchSize, 0, TWO_PI);
		//	ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES.TOUCH_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}



bool Canvas_SinCosKnob_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	if (setting.size() > 4 + loops)
		setting.resize( 4 + loops);
	int k;
	if (check) {
		for (k = loops - 1; k >= 0; k--) { // scan backwards
			// convert to pixels
			double inputX = getLoopingValueD(0, k, 0);
			double inputY = getLoopingValueD(1, k, 0);
			double inputRadius = getLoopingValueD(2, k, 50);  // radius
			double inputKnob = getLoopingValueD(3, k, DES_KNOB_SIZE);    // knob

			if (inputRadius <= 0)
				continue;
			if (inputKnob <= 0)
				continue;

			double alpha = 1;
			//if (pg->dgrm.cL[i].type == '2dClrSinCosZ') {
			//	alpha = getLoopingValueD(pg, i, 7, k, 1);
			//	if (alpha <= 0)
			//		continue;
			//}

			inputRadius = sizeTo2dCanvas(pg, inputRadius, displayMode);
			inputKnob = sizeTo2dCanvas(pg, inputKnob, displayMode);
			int touchSize = max(DES_TOUCH_SIZE, inputKnob);

			int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
			int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

			double knobA;
			if (setting[3]) { // detent is active
				double step = TWO_PI / setting[3];
				double temp = floor((setting[4 + k] + step / 2) / step);
				knobA = temp * step;
			}
			else
				knobA = setting[4 + k];
			int knobX = xCenter + inputRadius *cos(knobA);
			int knobY = yCenter + inputRadius * sin(knobA);

			if (mouseY > knobY + touchSize)
				continue; // missed dial
			if (mouseY < knobY - touchSize)
				continue; // missed dial

			if (mouseX > knobX + touchSize)
				continue; // missed knob
			if (mouseX < knobX - touchSize)
				continue; // missed knob

			memory[0] = k;
			return true;
		}
		return false;
	}

	k = memory[0];

	int inputX = getLoopingValueD(0, k, 0);
	int inputY = getLoopingValueD(1, k, 0);

	int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
	int yCenter = yTo2dCanvas2(pg, inputY, displayMode);

	int dx = mouseX - xCenter;
	int dy = mouseY - yCenter;
	double newAngle = atan2(dy, dx);
	
	if (setting[3]) { // detent is active
		double step = TWO_PI / setting[3];
		double temp = floor((newAngle + step / 2) / step);
		newAngle = temp * step;
	}

	setting[4 + k] = newAngle;

	method2(pg);

	return true;
}

bool Canvas_SinCosKnob_Component::onLoad(Sheet* pg) {
	//buildPinLabels();
	return method2(pg);
}
void Canvas_SinCosKnob_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	setting[3] = doubleValues[0]; // detent
	setting[2] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0) + (boolValues[2] ? 4 : 0);
	displayMode = boolValues[3];
}
void doubleClick2dSinCos(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Detent (steps in circle):" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels = { "Show Track", "Show Knob", "Show User Text", "Ignore Display Scale" };
	vector<bool> checkValues{ (bool)((int)pg->dgrm.cL[i]->setting[2] & 1),
		(bool)((int)pg->dgrm.cL[i]->setting[2] & 2),
		(bool)((int)pg->dgrm.cL[i]->setting[2] & 4),
		(bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>the2dSinCosButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dSinCos) };

Canvas_SinCosKnob_Component::Canvas_SinCosKnob_Component()
	: Component(L"2dSinCosZ", L"2D Sin/Cos", squareShape, 4, 1, 3, 1)
{
	dialogButtonList = the2dSinCosButtonList;
	setLeftLabels(3, "X", "Y", "Radius", "Knob");
	setRightLabels(3, "Sin", "Cos", "Angle");
	setSetting(0, -6);    // range needed for remote
	setSetting(1, 6);    // range needed for remote
	setSetting(2, 3);    // option code: 1 draw track, 2 draw knob, 4 draw number, 
	setSetting(3, 0);    // detent: 0-off, 4-90deg, 360-1 deg etc
	setSetting(4, 0);    // output 4 and up
	//getInternals = getInternals_4up;
	//setInternals = setInternals_2dSinCos;
	width = 40;
	completionTestDoubles = 1;
	completionTestChecks = 4;
}


// ********************** Vertical Axis *************************************

void innerVertAxis(Sheet* pg, double inputX, double inputY, double inputSize,
	double inputStart, double inputEnd, double inputTic, double inputLabel,
	int inputLineRed, int inputLineGreen, int inputLineBlue, double inputLineWidth,
	int inputTicRed, int inputTicGreen, int inputTicBlue, double inputTicWidth,
	int inputLabelRed, int inputLabelGreen, int inputLabelBlue,
	int optionBits, double inputArrowSize, double inputSmallTicSize, double inputLargeTicSize, int displayMode )
{
	// 1 = labels on top; 2 = show first value; 4 = arrow head
	bool labelsOnRight = optionBits & 1;
	bool showFirstLabel = optionBits & 2;
	bool arrowHead = optionBits & 4;

	int xx = xTo2dCanvas2(pg, inputX, displayMode);
	int yy = yTo2dCanvas2(pg, inputY, displayMode);

	// the line
	if (inputLineWidth > 0) {
		int y2 = yTo2dCanvas2(pg, inputY + inputSize, displayMode);
		if( arrowHead ){
			int arrowSizeX = inputArrowSize;// just a guess
			int arrowSizeY = 2*inputArrowSize;// just a guess

			if (inputEnd > inputStart) { // arrow on the top
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, xx, yy, NULL);
				LineTo(pg->hDC2, xx, y2 - 2*arrowSizeY);

				MoveToEx(pg->hDC2, xx - arrowSizeX, y2 - arrowSizeY, NULL);
				LineTo(pg->hDC2, xx, y2 - 2 * arrowSizeY);
				LineTo(pg->hDC2, xx + arrowSizeX, y2 - arrowSizeY);
			}
			else { // arrow on the bottom
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, xx, yy + 2*arrowSizeY, NULL);
				LineTo(pg->hDC2, xx, y2);

				MoveToEx(pg->hDC2, xx - arrowSizeX, yy + arrowSizeY, NULL);
				LineTo(pg->hDC2, xx, yy + 2 * arrowSizeY);
				LineTo(pg->hDC2, xx + arrowSizeX, yy + arrowSizeY);
			}
		}
		else { // no arrow head
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, xx, yy, NULL);
			LineTo(pg->hDC2, xx, y2);
		}
		EndPath(pg->hDC2);

		HPEN hPen = CreatePen(PS_COSMETIC, inputLineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
		SelectObject(pg->hDC2, hPen);
		StrokePath(pg->hDC2);
		DeleteObject(hPen);
	}

	double tic = abs(inputTic);
	double label = abs(inputLabel);
	double start = inputStart;
	double end = inputEnd;
	vector<int> labelPosition;
	int lastLabelIndex = 0;
	HPEN hTicPen = CreatePen(PS_COSMETIC, inputTicWidth, RGB(inputTicRed, inputTicGreen, inputTicBlue));

	if (label != 0) {
		if ((abs(end - start) / label) < 100) {
			//  put in the labels
			int x0 = xx;
			int y0 = yy;

			int labelSize = inputLargeTicSize;
			int labelXoffset;
			
			if (end > start) {
				// tic with label -- first the lines
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue <= end; labelValue += label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
	
					int yyy = y0 - (labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode);

					if (inputTicWidth > 0 ) {
						MoveToEx(pg->hDC2, x0 + labelSize, yyy, NULL);
						LineTo(pg->hDC2, x0 - labelSize, yyy);
					}
					labelPosition.push_back(yyy);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
				// then the labels
				int lab = 0;
				for (double labelValue = start; labelValue <= end; labelValue += label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
				
					int yyy = labelPosition[lab++];
					string text = tomString(labelValue);
					int labelWidth = calcStringWidthFont(pg->hDC2, text);
					labelXoffset = -labelSize - labelWidth - 0;
					if (labelsOnRight)
						labelXoffset = labelSize + 0;
					drawTextString(pg->hDC2, x0 + labelXoffset, yyy - 7, RGB(inputLabelRed, inputLabelGreen, inputLabelBlue), text);
				}
			}
			else {
				// tic lablel, first the tic marks
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue >= end; labelValue -= label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
					int yyy = y0 -(labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode);
					if (inputTicWidth > 0) {
						MoveToEx(pg->hDC2, x0 + labelSize, yyy, NULL);
						LineTo(pg->hDC2, x0 - labelSize, yyy);
					}
					labelPosition.push_back(yyy);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
				// then the label
				int lab = 0;
				for (double labelValue = start; labelValue >= end; labelValue -= label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
					int yyy = labelPosition[lab++];
					string text = tomString(labelValue);// labelConverter(labelValue);
					int labelWidth = calcStringWidthFont(pg->hDC2, text);
					labelXoffset = -labelSize - labelWidth - 0;
					if (labelsOnRight)
						labelXoffset = labelSize + 0;
					drawTextString(pg->hDC2, x0 + labelXoffset, yyy - 7, RGB(inputLabelRed, inputLabelGreen, inputLabelBlue), text);
				}
			}
		}
		lastLabelIndex = labelPosition.size() - 1;
	}

	if ((tic != 0) && (inputTicWidth > 0)){
		if ((abs(end - start) / tic) < 1000) {
			//  put in the tics
		
			int x0 = xx;
			int y0 = yy;
			int ticSize = inputSmallTicSize;
			int labelIndex = 0;
			if (end > start) {
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue <= end; labelValue += tic) {
					int yyy = y0 - (labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode);
					if (label != 0) {
						if ((labelIndex < lastLabelIndex) && (labelIndex < labelPosition.size())) {
							while ((labelPosition[labelIndex] > yyy) && (labelIndex < lastLabelIndex))
								labelIndex++;
							if (yyy == labelPosition[labelIndex])
								continue;
						}
					}
				
					MoveToEx(pg->hDC2, x0 + ticSize, yyy, NULL);
					LineTo(pg->hDC2, x0 - ticSize, yyy);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
			}
			else {
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue >= end; labelValue -= tic) {
					int yyy = y0 - (labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode);
					if (label != 0) {
						while (yyy < labelPosition[labelIndex]) {
							if (labelIndex >= labelPosition.size())
								labelIndex++;
							else
								break;
						}
						if (yyy == labelPosition[labelIndex])
							continue;
					}
					MoveToEx(pg->hDC2, x0 + ticSize, yyy, NULL);
					LineTo(pg->hDC2, x0 - ticSize, yyy);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
			}
		}
	}
	DeleteObject(hTicPen);
}

bool Canvas_VerticalAxis_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	
	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;
	
		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputSize = 100;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 100);
			if (inputSize <= 0)
				continue;
		}

		double inputStart = 0;
		double inputEnd = 100;
		if ((int)setting[0] & 4) {
			inputStart = getLoopingValueD(pin++, k, 0);
			inputEnd = getLoopingValueD(pin++, k, 100);
		}

		double inputTic = 5;
		double inputLabel = 10;
		if ((int)setting[0] & 8) {
			inputTic = getLoopingValueD(pin++, k, 5);
			inputLabel = getLoopingValueD(pin++, k, 10);
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 16) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputTicRed = 0;
		int inputTicGreen = 0;
		int inputTicBlue = 0;
		if ((int)setting[0] & 64) {
			inputTicRed = getLoopingValueD(pin++, k, 0);
			inputTicGreen = getLoopingValueD(pin++, k, 0);
			inputTicBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputLineWidth = 1;
		double inputTicWidth = 1;
		if ((int)setting[0] & 32) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			inputTicWidth = getLoopingValueD(pin++, k, 1);
		}

		int inputLabelRed = 0;
		int inputLabelGreen = 0;
		int inputLabelBlue = 0;
		if ((int)setting[0] & 128) {
			inputLabelRed = getLoopingValueD(pin++, k, 0);
			inputLabelGreen = getLoopingValueD(pin++, k, 0);
			inputLabelBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputAlpha = 1;
		if ((int)setting[0] & 256) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		innerVertAxis(pg, inputX, inputY, inputSize, inputStart, inputEnd, inputTic, inputLabel,
		inputLineRed, inputLineGreen, inputLineBlue, inputLineWidth,
		inputTicRed,  inputTicGreen, inputTicBlue, inputTicWidth, 
			inputLabelRed, inputLabelGreen, inputLabelBlue,
		setting[1], setting[2], setting[3], setting[4], displayMode);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

bool Canvas_VerticalAxis_Component::onLoad(Sheet* pg) {
	if (setting.size() < 5) {
		setSetting(2, 5); // arrow Size
		setSetting(3, 5); // small tic Size
		setSetting(4, 10); // large tic Size
	}
	buildPinLabels();
	return false;
}

void Canvas_VerticalAxis_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	
	setting[2] = doubleValues[0]; // arrow size
	setting[3] = doubleValues[1]; // small tic size
	setting[4] = doubleValues[2]; // large tic size

	//				Labels on right,		 Show First Label,			 Arrow Head
	setting[1] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0) + (boolValues[2] ? 4 : 0);
	displayMode = boolValues[3];
}

void setVertAxisOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	
	vector<string> doubleLabels = { "Arrow Size:", "Small Tic Size:", "Large Tic Size:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4] };

	// 1 = labels on top; 2 = show first value; 4 = arrow head
	vector<string> checkLabels = { "Labels on Right", "Show First Label", "Arrow Head", "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 1), 
		(bool)((int)pg->dgrm.cL[i]->setting[1] & 2), (bool)((int)pg->dgrm.cL[i]->setting[1] & 4),
		(bool)(pg->dgrm.cL[i]->displayMode) };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 40,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>vertAxisButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", setVertAxisOptions_Action) };


vector<int> Canvas_VerticalAxis_Component::GetPinGroups() { return vector<int>{2, 1, 2, 2, 3, 3, 2, 3, 1}; }
vector<string> Canvas_VerticalAxis_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Size",
		"Start", "End",
		"Tic", "Label",
		"Line Red", "Line Green", "Line Blue",
		"Tic Red", "Tic Green", "Tic Blue",
		"Line Width", "Tic Width",
		"Label Red", "Label Green", "Label Blue",
		"Alpha" };
}
Canvas_VerticalAxis_Component::Canvas_VerticalAxis_Component()
	: Canvas_Component(L"VAXIS", L"2D Vertical Axis", squareShape, 7, 1, 0, 0)
{
	dialogButtonList = vertAxisButtonList;
	setSetting(0, 1+2+4+8);
	setSetting(1, 2+4); // 1 = labels on right; 2 = show first value; 4 = arrow head
	setSetting(2, 5); // Arrow Size
	setSetting(3, 5); // small Tic Size
	setSetting(4, 10); // large Tic Size
	pinMaskIndex = 0;
	completionTestDoubles = 2;
	completionTestChecks = 4;
};

// ********************** Horizontal Axis *************************************


void innerHorizAxis(Sheet* pg, double inputX, double inputY, double inputSize,
	double inputStart, double inputEnd, double inputTic, double inputLabel,
	int inputLineRed, int inputLineGreen, int inputLineBlue, double inputLineWidth,
	int inputTicRed, int inputTicGreen, int inputTicBlue, double inputTicWidth,
	int inputLabelRed, int inputLabelGreen, int inputLabelBlue,
	int optionBits, double inputArrowSize, double inputSmallTicSize, double inputLargeTicSize, int displayMode)
{
	// 1 = labels on top; 2 = show first value; 4 = arrow head
	bool labelsOnTop = optionBits & 1;
	bool showFirstLabel = optionBits & 2;
	bool arrowHead = optionBits & 4;

	int xx = xTo2dCanvas2(pg, inputX, displayMode);
	int yy = yTo2dCanvas2(pg, inputY, displayMode);

	// the line and optional arrow head
	if (inputLineWidth > 0) {
		int x2 = xTo2dCanvas2(pg, inputX + inputSize, displayMode);
		if (arrowHead) {
			int arrowSizeX = 2*inputArrowSize;// just a guess
			int arrowSizeY = inputArrowSize;// just a guess

			if (inputEnd > inputStart) { // arrow on the right
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, xx, yy, NULL);
				LineTo(pg->hDC2, x2 + 2*arrowSizeX, yy);

				MoveToEx(pg->hDC2, x2 + arrowSizeX, yy - arrowSizeY, NULL);
				LineTo(pg->hDC2, x2 + 2*arrowSizeX, yy);
				LineTo(pg->hDC2, x2 + arrowSizeX, yy + arrowSizeY);
			}
			else { // arrow on the left
				BeginPath(pg->hDC2);
				MoveToEx(pg->hDC2, xx - 2*arrowSizeX, yy, NULL);
				LineTo(pg->hDC2, x2, yy);

				MoveToEx(pg->hDC2, xx - arrowSizeX, yy - arrowSizeY, NULL);
				LineTo(pg->hDC2, xx - 2*arrowSizeX, yy);
				LineTo(pg->hDC2, xx - arrowSizeX, yy + arrowSizeY);
			}
		}
		else { // no arrow head
			BeginPath(pg->hDC2);
			MoveToEx(pg->hDC2, xx, yy, NULL);
			LineTo(pg->hDC2, x2, yy);
		}
		EndPath(pg->hDC2);

		HPEN hPen = CreatePen(PS_COSMETIC, inputLineWidth, RGB(inputLineRed, inputLineGreen, inputLineBlue));
		SelectObject(pg->hDC2, hPen);
		StrokePath(pg->hDC2);
		DeleteObject(hPen);
	}

	double tic = abs(inputTic);
	double label = abs(inputLabel);
	double start = inputStart;
	double end = inputEnd;
	vector<int> labelPosition;
	int lastLabelIndex = 0;
	HPEN hTicPen = CreatePen(PS_COSMETIC, inputTicWidth, RGB(inputTicRed, inputTicGreen, inputTicBlue));

	if (label != 0) {
		if ((abs(end - start) / label) < 100) {
			//  put in the labels
			int x0 = xx;
			int y0 = yy;

			int labelYoffset = inputLargeTicSize;
			if (labelsOnTop)
				labelYoffset = -inputLargeTicSize - 10;
			int labelSize = inputLargeTicSize;

			if (end > start) {
				// first the tics
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue <= end; labelValue += label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
				
					int xxx = (labelValue - start) / (end - start) * sizeTo2dCanvas( pg, inputSize, displayMode) + x0;
					if (inputTicWidth > 0) {
						MoveToEx(pg->hDC2, xxx, y0 + labelSize, NULL);
						LineTo(pg->hDC2, xxx, y0 - labelSize);
					}
					labelPosition.push_back(xxx);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
				// then the text
				int lab = 0;
				for (double labelValue = start; labelValue <= end; labelValue += label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
				
					int xxx = labelPosition[lab++];
					string text = tomString(labelValue);// labelConverter(labelValue);
					int labelWidth = calcStringWidthFont(pg->hDC2, text);
					drawTextString(pg->hDC2, xxx - labelWidth / 2, y0 + labelYoffset, RGB(inputLabelRed, inputLabelGreen, inputLabelBlue), text);
				}
			}
			else {
				// first the tics
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue >= end; labelValue -= label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
					int xxx = (labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode) + x0;
					if (inputTicWidth > 0) {
						MoveToEx(pg->hDC2, xxx, y0 + labelSize, NULL);
						LineTo(pg->hDC2, xxx, y0 - labelSize);
					}
					labelPosition.push_back(xxx);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);

				int lab = 0;
				for (double labelValue = start; labelValue >= end; labelValue -= label) {
					if ((labelValue == 0) && !showFirstLabel)
						continue;
					int xxx = labelPosition[lab++];
					string text = tomString(labelValue);// labelConverter(labelValue);
					int labelWidth = calcStringWidthFont(pg->hDC2, text);
					drawTextString(pg->hDC2, xxx - labelWidth / 2, y0 + labelYoffset, RGB(inputLabelRed, inputLabelGreen, inputLabelBlue), text);
				}
			}
		}
		lastLabelIndex = labelPosition.size() - 1;
	}

	if ((tic != 0) && (inputTicWidth > 0)) {
		if ((abs(end - start) / tic) < 1000) {
			//  put in the tics
			int x0 = xx;
			int y0 = yy;
			int ticSize = inputSmallTicSize;
			int labelIndex = 0;
			if (end > start) {
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue <= end; labelValue += tic) {
					int xxx = (labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode) + x0;
					if (label != 0) {
						if ((labelIndex < lastLabelIndex) && (labelIndex < labelPosition.size())){
							while ((labelPosition[labelIndex] < xxx) && (labelIndex < lastLabelIndex))
								labelIndex++;
							if (xxx == labelPosition[labelIndex])
								continue;
						}
					}
				
					MoveToEx(pg->hDC2, xxx, y0 + ticSize, NULL);
					LineTo(pg->hDC2, xxx, y0 - ticSize);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
			}
			else {
				BeginPath(pg->hDC2);
				for (double labelValue = start; labelValue >= end; labelValue -= tic) {
					int xxx = (labelValue - start) / (end - start) * sizeTo2dCanvas(pg, inputSize, displayMode) + x0;
					if (label != 0) {
						while (xxx < labelPosition[labelIndex]) {
							if (labelIndex >= labelPosition.size())
								labelIndex++;
							else
								break;
						}
						if (xxx == labelPosition[labelIndex])
							continue;
					}
					MoveToEx(pg->hDC2, xxx, y0 + ticSize, NULL);
					LineTo(pg->hDC2, xxx, y0 - ticSize);
				}
				EndPath(pg->hDC2);
				SelectObject(pg->hDC2, hTicPen);
				StrokePath(pg->hDC2);
			}
		}
	}
	DeleteObject(hTicPen);
}

bool Canvas_HorizontalAxis_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputSize = 100;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 100);
			if (inputSize <= 0)
				continue;
		}

		double inputStart = 0;
		double inputEnd = 100;
		if ((int)setting[0] & 4) {
			inputStart = getLoopingValueD(pin++, k, 0);
			inputEnd = getLoopingValueD(pin++, k, 100);
		}

		double inputTic = 5;
		double inputLabel = 10;
		if ((int)setting[0] & 8) {
			inputTic = getLoopingValueD(pin++, k, 5);
			inputLabel = getLoopingValueD(pin++, k, 10);
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 16) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputTicRed = 0;
		int inputTicGreen = 0;
		int inputTicBlue = 0;
		if ((int)setting[0] & 64) {
			inputTicRed = getLoopingValueD(pin++, k, 0);
			inputTicGreen = getLoopingValueD(pin++, k, 0);
			inputTicBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputLineWidth = 1;
		double inputTicWidth = 1;
		if ((int)setting[0] & 32) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			inputTicWidth = getLoopingValueD(pin++, k, 1);
		}

		int inputLabelRed = 0;
		int inputLabelGreen = 0;
		int inputLabelBlue = 0;
		if ((int)setting[0] & 128) {
			inputLabelRed = getLoopingValueD(pin++, k, 0);
			inputLabelGreen = getLoopingValueD(pin++, k, 0);
			inputLabelBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputAlpha = 1;
		if ((int)setting[0] & 256) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		innerHorizAxis(pg, inputX, inputY, inputSize, inputStart, inputEnd, inputTic, inputLabel,
			inputLineRed, inputLineGreen, inputLineBlue, inputLineWidth,
			inputTicRed, inputTicGreen, inputTicBlue, inputTicWidth,
			inputLabelRed, inputLabelGreen, inputLabelBlue,
			setting[1], setting[2], setting[3], setting[4], displayMode);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

bool Canvas_HorizontalAxis_Component::onLoad(Sheet* pg) {
	if (setting.size() < 5) {
		setSetting(2, 5); // Arrow Size
		setSetting(3, 5); // Small Tic Size
		setSetting(4, 10); // Large Tic Size
	}
	buildPinLabels();
	return false;
}

void Canvas_HorizontalAxis_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[2] = doubleValues[0]; // arrow size
	setting[3] = doubleValues[1]; // small tic size
	setting[4] = doubleValues[2]; // large tic size

	//				Labels on Top,			 Show First Label,			 Arrow Head
	setting[1] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0) + (boolValues[2] ? 4 : 0);
	displayMode = boolValues[3];
}

void setHorAxisOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Arrow Size:", "Small Tic Size:", "Large Tic Size:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4] };

	// 1 = labels on top; 2 = show first value; 4 = arrow head
	vector<string> checkLabels = { "Labels on Top", "Show First Label", "Arrow Head", "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 1),
		(bool)((int)pg->dgrm.cL[i]->setting[1] & 2), (bool)((int)pg->dgrm.cL[i]->setting[1] & 4),
		(bool)(pg->dgrm.cL[i]->displayMode) };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>horizAxisButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", setHorAxisOptions_Action) };

vector<int> Canvas_HorizontalAxis_Component::GetPinGroups() { return vector<int>{2, 1, 2, 2, 3, 3, 2, 3, 1}; }
vector<string> Canvas_HorizontalAxis_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Size",
		"Start", "End",
		"Tic", "Label",
		"Line Red", "Line Green", "Line Blue",
		"Tic Red", "Tic Green", "Tic Blue",
		"Line Width", "Tic Width",
		"Label Red", "Label Green", "Label Blue",
		"Alpha" };
}
Canvas_HorizontalAxis_Component::Canvas_HorizontalAxis_Component()
	: Canvas_Component(L"HAXIS", L"2D Horizontal Axis", squareShape, 7, 1, 0, 0)
{
	dialogButtonList = horizAxisButtonList;
	setSetting(0, 1 + 2 + 4 + 8);
	setSetting(1, 2+4); // 1 = labels on top; 2 = show first value; 4 = arrow head
	setSetting(2, 5); // Arrow Size
	setSetting(3, 5); // Small Tic Size
	setSetting(4, 10); // Large Tic Size
	pinMaskIndex = 0;
	completionTestDoubles = 2;
	completionTestChecks = 4;
};


// ******************************** X Y Axis *************************************************

bool Canvas_XyAxis_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputSize = 100;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 100);
			if (inputSize <= 0)
				continue;
		}

		double inputRange = 100;
		if ((int)setting[0] & 4) {
			inputRange = getLoopingValueD(pin++, k, 100);
		}
		if (inputRange <= 0)
			continue;

		double inputTic = 5;
		double inputLabel = 10;
		if ((int)setting[0] & 8) {
			inputTic = getLoopingValueD(pin++, k, 5);
			inputLabel = getLoopingValueD(pin++, k, 10);
		}
		if (inputTic < 0)
			inputTic = 0;
		if (inputLabel < 0)
			inputLabel = 0;

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 16) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputTicRed = 0;
		int inputTicGreen = 0;
		int inputTicBlue = 0;
		if ((int)setting[0] & 64) {
			inputTicRed = getLoopingValueD(pin++, k, 0);
			inputTicGreen = getLoopingValueD(pin++, k, 0);
			inputTicBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputLineWidth = 1;
		double inputTicWidth = 1;
		if ((int)setting[0] & 32) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			inputTicWidth = getLoopingValueD(pin++, k, 1);
		}

		int inputLabelRed = 0;
		int inputLabelGreen = 0;
		int inputLabelBlue = 0;
		if ((int)setting[0] & 128) {
			inputLabelRed = getLoopingValueD(pin++, k, 0);
			inputLabelGreen = getLoopingValueD(pin++, k, 0);
			inputLabelBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputAlpha = 1;
		if ((int)setting[0] & 256) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		// positive X - with arrow and first value
		innerHorizAxis(pg, inputX, inputY, inputSize, 0, inputRange, inputTic, inputLabel,
			inputLineRed, inputLineGreen, inputLineBlue, inputLineWidth,
			inputTicRed, inputTicGreen, inputTicBlue, inputTicWidth,
			inputLabelRed, inputLabelGreen, inputLabelBlue,
			6, setting[2], setting[3], setting[4], displayMode);

		// negative X - without arrow
		innerHorizAxis(pg, inputX-inputSize, inputY, inputSize, -inputRange, 0, inputTic, inputLabel,
			inputLineRed, inputLineGreen, inputLineBlue, inputLineWidth,
			inputTicRed, inputTicGreen, inputTicBlue, inputTicWidth,
			inputLabelRed, inputLabelGreen, inputLabelBlue,
			2, setting[2], setting[3], setting[4], displayMode);

		// positive Y - with arrow, without first value
		innerVertAxis(pg, inputX, inputY, inputSize, 0, inputRange, inputTic, inputLabel,
			inputLineRed, inputLineGreen, inputLineBlue, inputLineWidth,
			inputTicRed, inputTicGreen, inputTicBlue, inputTicWidth,
			inputLabelRed, inputLabelGreen, inputLabelBlue,
			4, setting[2], setting[3], setting[4], displayMode);

		// negative Y - without arrow, without first value
		innerVertAxis(pg, inputX, inputY-inputSize, inputSize, -inputRange, 0, inputTic, inputLabel,
			inputLineRed, inputLineGreen, inputLineBlue, inputLineWidth,
			inputTicRed, inputTicGreen, inputTicBlue, inputTicWidth,
			inputLabelRed, inputLabelGreen, inputLabelBlue,
			0, setting[2], setting[3], setting[4], displayMode);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

bool Canvas_XyAxis_Component::onLoad(Sheet* pg) {
	if (setting.size() < 5) {
		setSetting(2, 5); // Arrow Size
		setSetting(3, 5); // small Tic Size
		setSetting(4, 10); // large Tic Size
	}
	buildPinLabels();
	return false;
}

void Canvas_XyAxis_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	setting[2] = doubleValues[0]; // Arrow size
	setting[3] = doubleValues[1]; // small tic size
	setting[4] = doubleValues[2]; // large tic size

	//					 Arrow Head
	setting[1] = (boolValues[2] ? 4 : 0);
	displayMode = boolValues[3];
}

void setXyAxisOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Arrow Size:", "Small Tic Size:", "Large Tic Size:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4] };

	// 1 = labels on top; 2 = show first value; 4 = arrow head
	vector<string> checkLabels = { "Arrow Head", "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 4),
		(bool)(pg->dgrm.cL[i]->displayMode) };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>xyAxisButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", setXyAxisOptions_Action) };

vector<int> Canvas_XyAxis_Component::GetPinGroups() { return vector<int>{2, 1, 1, 2, 3, 3, 2, 3, 1}; }
vector<string> Canvas_XyAxis_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",
		"Size",
		"Range",
		"Tic", "Label",
		"Line Red", "Line Green", "Line Blue",
		"Tic Red", "Tic Green", "Tic Blue",
		"Line Width", "Tic Width",
		"Label Red", "Label Green", "Label Blue",
		"Alpha" };
}

Canvas_XyAxis_Component::Canvas_XyAxis_Component()
	: Canvas_Component(L"XyAxis", L"2D XY Axis", squareShape, 7, 1, 0, 0)
{
	dialogButtonList = horizAxisButtonList;
	setSetting(0, 1 + 2 + 4 + 8);
	setSetting(1, 4); // 1 = labels on top; 2 = show first value; 4 = arrow head
	setSetting(2, 5); // Arrow Size
	setSetting(3, 5); // Small Tic Size
	setSetting(4, 10); // Large Tic Size
	pinMaskIndex = 0;
	completionTestDoubles = 2;
	completionTestChecks = 2;
};

// ***************************** Draw Axis on the 2D canvas ************************************
void drawAxis(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return;

	if (pg->canvas2Scale > 30000000)
		return;

	int halfHeight = pg->canvas2Height / 2;
	int halfWidth = pg->canvas2Width / 2;
	
	int tic, label;
	innerTicLabel(pg->canvas2Scale, &tic, &label);
	
	int hSize = halfWidth / pg->canvas2Scale;
	int vSize = halfHeight / pg->canvas2Scale;

	int size;
	int start;
	int end;
	if (hSize + pg->canvas2xOffset > 0) {
		size = hSize + pg->canvas2xOffset;
		start = 0;
		end = hSize + pg->canvas2xOffset;
		innerHorizAxis(pg, 0, 0, size, start, end, tic, label, 0,0,0,1, 0,0,0,1, 0,0,0, 0, 5, 5, 10, 0);
	}
	
	if (pg->canvas2xOffset - hSize < 0) {
		size = pg->canvas2xOffset - hSize;
		start = 0;
		end = pg->canvas2xOffset - hSize;
		innerHorizAxis(pg, 0, 0, size, start, end, tic, label, 0,0,0,1, 0,0,0,1, 0,0,0, 0, 5, 5, 10, 0);
	}
	
	if (vSize + pg->canvas2yOffset > 0) {
		size = vSize + pg->canvas2yOffset;
		start = 0;
		end = vSize + pg->canvas2yOffset;
		innerVertAxis(pg, 0, 0, size, start, end, tic, label, 0,0,0,1, 0,0,0,1, 0,0,0, 0, 5, 5, 10, 0);
	}
	
	if (pg->canvas2yOffset - vSize < 0) {
		size = pg->canvas2yOffset - vSize;
		start = 0;
		end = pg->canvas2yOffset - vSize;
		innerVertAxis(pg, 0, 0, size, start, end, tic, label, 0,0,0,1, 0,0,0,1, 0,0,0, 0, 5, 5, 10, 0);
	}
}

// ********************** 2D Display ***************************
int Canvas_ViewControl_Component::method2(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	if (!checkValueInputs())
		return false;

	int result = 0;
	int pin = 0;


	double inputScale = 1;
	if ((int)setting[0] & 1) {
		inputScale = getLoopingValueD(pin++, 0, 1);
		if (inputScale < 0.000000001)
			inputScale = 0.000000001;
	}
	if (pg->canvas2Scale != inputScale) {
		pg->canvas2Scale = inputScale;
		result |= 2;
	}
		

	double inputOffsetX = 0;
	double inputOffsetY = 0;
	if ((int)setting[0] & 2) {
		inputOffsetX = getLoopingValueD(pin++, 0, 0);
		inputOffsetY = getLoopingValueD(pin++, 0, 0);
	}
	if (pg->canvas2xOffset != inputOffsetX) {
		pg->canvas2xOffset = inputOffsetX;
		result |= 2;
	}
	if (pg->canvas2yOffset != inputOffsetY) {
		pg->canvas2yOffset = inputOffsetY;
		result |= 2;
	}
	
	int inputRed = 255;
	int inputGreen = 255;
	int inputBlue = 255;
	if ((int)setting[0] & 4) {
		inputRed = getLoopingValueD(pin++, 0, 0);
		inputGreen = getLoopingValueD(pin++, 0, 0);
		inputBlue = getLoopingValueD(pin++, 0, 0);
	}
	if (pg->canvas2Red != inputRed) {
		pg->canvas2Red = inputRed;
		result |= 2;
	}
	if (pg->canvas2Green != inputGreen) {
		pg->canvas2Green = inputGreen;
		result |= 2;
	}
	if (pg->canvas2Blue != inputBlue) {
		pg->canvas2Blue = inputBlue;
		result |= 2;
	}

	if (output[0].value[0] != pg->canvas2Width) {
		output[0].value[0] = pg->canvas2Width;
		result |= 1;
	}
	if (output[1].value[0] != pg->canvas2Height) {
		output[1].value[0] = pg->canvas2Height;
		result |= 1;
	}

	return result;
}
bool Canvas_ViewControl_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	return method2(pg);
}
Canvas_ViewControl_Component::~Canvas_ViewControl_Component() {
	if ((page->canvas2Scale == 1)		&&
		(page->canvas2xOffset == 0)	&&
		(page->canvas2yOffset == 0)	&&
		(page->canvas2Red == 255)		&&
		(page->canvas2Green == 255)	&&
		(page->canvas2Blue == 255)	)
		return;

	page->canvas2Scale = 1;
	page->canvas2xOffset = 0;
	page->canvas2yOffset = 0;

	page->canvas2Red = 255;
	page->canvas2Green = 255;
	page->canvas2Blue = 255;

	// for some reason method will be called next - override the input values
	int pin = 0;
	if ((int)setting[0] & 1) 
		if( input[pin].value.size())
			input[pin++].value[0] = 1;
		
	if ((int)setting[0] & 2) {
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
	}
	
	if ((int)setting[0] & 4) {
		if (input[pin].value.size())
			input[pin++].value[0] = 255;
		if (input[pin].value.size())
			input[pin++].value[0] = 255;
		if (input[pin].value.size())
			input[pin++].value[0] = 255;
	}
	
	drawCanvas2();
	return;
}
bool Canvas_ViewControl_Component::onLoad(Sheet* pg) {
	if (setting.size() < 1)
		setSetting(0, 7);
	buildPinLabels();
	return false;
}
std::vector<Button*>view2dButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

vector<int> Canvas_ViewControl_Component::GetPinGroups() { return vector<int>{1, 2, 3}; }
vector<string> Canvas_ViewControl_Component::GetPinLabels() {
	return vector<string>{
		"Scale Factor",
		"X Offset", "Y Offset",
		"Red", "Green", "Blue" };
}
Canvas_ViewControl_Component::Canvas_ViewControl_Component()
	: Component(L"2dDisp", L"2D View Control", squareShape, 6, 1, 2, 1)
{
	dialogButtonList = view2dButtonList;
	pinMaskIndex = 0;
	setRightLabels(3, "Width", "Height");
	setSetting(0, 3);
	width = 50;
}



// ************************** Debug ********************************

int drawMainWindowCounter = 0;
int drawCanvasWindowCounter = 0;
int analyzeCounter = 0;
int updateCounter = 0;

int Debug_Component::method2(Sheet* pg) {

	bool change = false;
	int delta;

	if (old_drawMainWindowCounter != drawMainWindowCounter) {
		delta = drawMainWindowCounter - old_drawMainWindowCounter;
		old_drawMainWindowCounter = drawMainWindowCounter;
		output[0].value[0] = drawMainWindowCounter;
		output[1].value[0] = delta;
		change = true;
	}
	else
		output[1].value[0] = 0;

	if (old_drawCanvasWindowCounter != drawCanvasWindowCounter) {
		delta = drawCanvasWindowCounter - old_drawCanvasWindowCounter;
		old_drawCanvasWindowCounter = drawCanvasWindowCounter;
		output[2].value[0] = drawCanvasWindowCounter;
		output[3].value[0] = delta;
		change = true;
	}
	else
		output[3].value[0] = 0;

	if (old_analyzeCounter != analyzeCounter) {
		delta = analyzeCounter - old_analyzeCounter;
		old_analyzeCounter = analyzeCounter;
		output[4].value[0] = analyzeCounter;
		output[5].value[0] = delta;
		change = true;
	}
	else
		output[5].value[0] = 0;

	if (old_updateCounter != updateCounter) {
		delta = updateCounter - old_updateCounter;
		old_updateCounter = updateCounter;
		output[6].value[0] = updateCounter;
		output[7].value[0] = delta;
		change = true;
	}
	else
		output[7].value[0] = 0;

	return change;
}

Debug_Component::Debug_Component()
	: Component(L"Debug", L"Debug", squareShape, 1, 1, 8, 1)
{
	setRightLabels(3, "Draw", "delta", "Draw 2", "delta", "Analyze", "delta", "Update", "delta" );
	width = 50;
}



// ************************** 2d Array Slider *********************************************

int Canvas_ArraySlider_Component::method2(Sheet* pg) {

	if (((int)setting[0] & bitSetArray[0]) == 0)
		return 0;

	// curve X's and Y's have to be the same size
	int arrayLength = input[0].value.size();
	if (arrayLength != input[1].value.size())
		return 0;
	if (arrayLength < 2)
		return 0;

	// match elements to number of knobs
	if (setting[1] < element.size()) {
		element.resize(setting[1]);
	}else if (setting[1] > element.size()) {
		int oldSize = element.size();
		element.resize(setting[1]);
		for (int j = oldSize; j < setting[1]; j++)
			element[j] = 0;
	}

	// match outputs to number of knobs
	if (setting[1] < output[0].value.size()) {
		output[0].value.resize(setting[1]);
		output[1].value.resize(setting[1]);
		output[2].value.resize(setting[1]);
	}
	else if (setting[1] > output[0].value.size()) {
		int oldSize = output[0].value.size();
		output[0].value.resize(setting[1]);
		output[1].value.resize(setting[1]);
		output[2].value.resize(setting[1]);
		for (int j = oldSize; j < setting[1]; j++) {
			element[j] = 0;
			output[0].value[j] = 0;
			output[1].value[j] = 0;
			output[2].value[j] = 0;
		}
	}


	int pin = 2;
	if ((int)setting[0] & bitSetArray[1]) // skip track Line Width
		pin++;
	if ((int)setting[0] & bitSetArray[2]) // skip Line color
		pin += 3;
	if ((int)setting[0] & bitSetArray[3]) // skip track Alpha
		pin++;
	if ((int)setting[0] & bitSetArray[4])  // skip knob size
		pin++;
	if ((int)setting[0] & bitSetArray[5])  // skip knob line size
		pin++;
	if ((int)setting[0] & bitSetArray[6])  // skip knob line color
		pin += 3;
	if ((int)setting[0] & bitSetArray[7])  // skip knob fill color
		pin += 3;
	if ((int)setting[0] & bitSetArray[8])  // skip knob alpha
		pin++;
	int savePin = pin;

	for (int j = 0; j < setting[1]; j++) {
		pin = savePin;

		int trackValue = 0;
		bool trackNow = false;
		bool mouseTracking = 1;
		if ((int)setting[0] & bitSetArray[9]) {  // get knob tracking
			trackValue    = getLoopingValueD(pin++, j, 0);
			trackNow      = getLoopingValueD(pin++, j, 0);
			mouseTracking = getLoopingValueD(pin++, j, 1);
		}
		
		bool initialEnabled = false;
		double initialValue = 0;
		if ((int)setting[0] & bitSetArray[10]) {  // get initial settings
			initialEnabled = true;
			initialValue = getLoopingValueD(pin++, j, 0);
		}
		

		if (trackNow) {
			trackValue = floor(trackValue);
			if ((trackValue >= 0) && (trackValue < arrayLength))
				element[j] = trackValue;
		}
		else {
			if (initialEnabled) {
				trackValue = floor(initialValue);
				if (initialMemoryX[j] != trackValue) {
					initialMemoryX[j] = initialValue;
					if ((initialValue >= 0) && (initialValue < arrayLength))
						element[j] = initialValue;
				}
			}
		}
		int k = element[j];
		if (k >= arrayLength) {
			// this knob no longer has a point
			element[j] = 0; // put it on zero
			k = 0;
		}
		// output its values
		output[0].value[j] = input[0].value[k];
		output[1].value[j] = input[1].value[k];
		output[2].value[j] = k;
	}

	output[0].dimensions[0] = setting[1];
	output[1].dimensions[0] = setting[1];
	output[2].dimensions[0] = setting[1];

	return 3;
}

bool Canvas_ArraySlider_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	// curve X's and Y's have to be the same size
	int arrayLength = input[0].value.size();
	if (arrayLength != input[1].value.size())
		return false;
	if (arrayLength < 2)
		return false;

	int pin = 2;
	double trackLineWidth = 1;// 1=track Line Width: [["track Line Width"]]
	if ((int)setting[0] & bitSetArray[1]) {
		trackLineWidth = getLoopingValueD(pin++, 0, 1);
	}
	
	//ctx2.lineWidth = trackLineWidth * pg->canvas2Scale;
	// 2=Line color: ["Line Red", "Line Green", "Line Blue"],
	int trackLineRed = 0;
	int trackLineGreen = 0;
	int trackLineBlue = 0;
	if ((int)setting[0] & bitSetArray[2]) {
		trackLineRed = getLoopingValueD(pin++, 0, 0);
		trackLineGreen = getLoopingValueD(pin++, 0, 0);
		trackLineBlue = getLoopingValueD(pin++, 0, 0);
	}
	
	// 3=Alpha: ["Track Alpha"],
	double trackAlpha = true;
	if ((int)setting[0] & bitSetArray[3]) {
		trackAlpha = getLoopingValueD(pin++, 0, 1);
	}
	
	int savePin = pin;

	// grab up and convert the track points
	vector<double> x;
	vector<double> y;
	x.resize(arrayLength);
	y.resize(arrayLength);
	for (int j = 0; j < arrayLength; j++) {
		x[j] = xTo2dCanvas2(pg, input[0].value[j], displayMode);
		y[j] = yTo2dCanvas2(pg, input[1].value[j], displayMode);
	}

	if ((trackAlpha > 0) && (trackLineWidth > 0)) {
		// draw the track
		
		//setDashingContext2(pg, i, 0);
		BeginPath(pg->hDC2);
		if ((int)setting[0] & bitSetArray[14]) { // FREEFORM -- can jump to any point
			for (int src = 0; src < arrayLength - 1; src++) {
				for (int dst = src + 1; dst < arrayLength; dst++) {
					MoveToEx(pg->hDC2, x[src], y[src], NULL);
					LineTo(pg->hDC2, x[dst], y[dst]);
				}
			}
		}else {
			MoveToEx(pg->hDC2, x[0], y[0], NULL);
			for (int j = 1; j < arrayLength; j++)
				LineTo(pg->hDC2, x[j], y[j]);
			if ((int)setting[0] & bitSetArray[13])  // wraparound enabled
				LineTo(pg->hDC2, x[0], y[0]);
		}
		EndPath(pg->hDC2);
		trackLineWidth = sizeTo2dCanvas(pg, trackLineWidth, displayMode);
		COLORREF lineColor = RGB(trackLineRed, trackLineGreen, trackLineBlue);
		HPEN hPen = CreatePen(PS_COSMETIC, trackLineWidth, lineColor);
		SelectObject(pg->hDC2, hPen);
		StrokePath(pg->hDC2);
		DeleteObject(hPen);
	}
	//ctx2.setLineDash([]);

	// draw the knobs
	for (int j = 0; j < setting[1]; j++) {
		pin = savePin;

		double knobSize = DES_KNOB_SIZE;// 4-get knob Size
		if ((int)setting[0] & bitSetArray[4]) {
			knobSize = getLoopingValueD(pin++, j, DES_KNOB_SIZE);
			if (knobSize <= 0)
				continue;
		}
		knobSize = sizeTo2dCanvas(pg, knobSize, displayMode);

		double knobLineWidth = 1;// 5-get knob line Width
		if ((int)setting[0] & bitSetArray[5]) {
			knobLineWidth = getLoopingValueD(pin++, j, 1);
			if (knobLineWidth <= 0)
				continue;
		}
		knobLineWidth = sizeTo2dCanvas(pg, knobLineWidth, displayMode);
		
		int knobLineRed = 0;// 6-Line color: ["Knob Line Red", "Knob Line Green", "Knob Line Blue"],
		int knobLineGreen = 0;
		int knobLineBlue = 0;
		if ((int)setting[0] & bitSetArray[6]) {
			knobLineRed = getLoopingValueD(pin++, j, 0);
			knobLineGreen = getLoopingValueD(pin++, j, 0);
			knobLineBlue = getLoopingValueD(pin++, j, 0);
		}
		
		int knobFillRed = 255;// 7-Fill color: ["Knob Fill Red", "Knob Fill Green", "Knob Fill Blue"],
		int knobFillGreen = 255;
		int knobFillBlue = 255;
		if ((int)setting[0] & bitSetArray[7]) {
			knobFillRed = getLoopingValueD(pin++, j, 0);
			knobFillGreen = getLoopingValueD(pin++, j, 0);
			knobFillBlue = getLoopingValueD(pin++, j, 0);
		}
		
		double knobAlpha = 1;// 3=knob Alpha: ["Knob Alpha"],
		if ((int)setting[0] & bitSetArray[8]) {
			knobAlpha = getLoopingValueD(pin++, j, 1);
			if (knobAlpha <= 0)
				continue;
		}

		int k = element[j];
		BeginPath(pg->hDC2);
		Ellipse(pg->hDC2, x[k] - knobSize, y[k] - knobSize, x[k] + knobSize, y[k] + knobSize);
		EndPath(pg->hDC2);

		HBRUSH hBrush = CreateSolidBrush(RGB(knobFillRed, knobFillGreen, knobFillBlue));
		SelectObject(pg->hDC2, hBrush);
		HPEN hPen = CreatePen(PS_COSMETIC, knobLineWidth, RGB(knobLineRed, knobLineGreen, knobLineBlue));
		SelectObject(pg->hDC2, hPen);
		StrokeAndFillPath(pg->hDC2);
		DeleteObject(hPen);
		DeleteObject(hBrush);

		// the Halo
		//if (memory[0] == j) {
		//	BeginPath(pg->hDC2);
		//	Ellipse(pg->hDC2, x[k] - DES_HALO_SIZE, y[k] - DES_HALO_SIZE, x[k] + DES_HALO_SIZE, y[k] + DES_HALO_SIZE);
		//	EndPath(pg->hDC2);
		//	StrokeAndFillPath(pg->hDC2);
		//}
		// the touch area - DIAGNOSTIC
		//if (DES_TOUCH_ALPHA) {
		//	int touchSize = max(DES_TOUCH_SIZE, knobSize);
		//	ctx2.beginPath();
		//	ctx2.arc(x[k], y[k], touchSize, 0, TWO_PI);
		//	ctx2.fillStyle = redGreenBlueAlpha(0, 255, 0, DES.TOUCH_ALPHA);
		//	ctx2.fill();
		//}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return false;
}

bool Canvas_ArraySlider_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {

	// curve X's and Y's have to be the same size
	int arrayLength = input[0].value.size();
	if (arrayLength != input[1].value.size())
		return false;
	if (arrayLength < 2)
		return false;

	// grab and convert the track points
	vector<int> x;
	vector<int> y;
	x.resize(arrayLength);
	y.resize(arrayLength);
	for (int j = 0; j < arrayLength; j++) {
		x[j] = xTo2dCanvas2(pg, input[0].value[j], displayMode);
		y[j] = yTo2dCanvas2(pg, input[1].value[j], displayMode);
	}
	int knobNumber = 0;

	if (check) {

		int pin = 0;
		if ((int)setting[0] & bitSetArray[0]) // array is required
			pin += 2;
		if ((int)setting[0] & bitSetArray[1]) // skip track width
			pin++;
		if ((int)setting[0] & bitSetArray[2]) // skip track color
			pin += 3;
		if ((int)setting[0] & bitSetArray[3]) // skip track alpha
			pin++;
		int savePin = pin;


		bool hitOne = false;
		for (int knobNumber = setting[1] - 1; knobNumber >= 0; knobNumber--) {
			pin = savePin;

			int knobSize = DES_KNOB_SIZE;
			if ((int)setting[0] & bitSetArray[4]) { // get knob size
				knobSize = getLoopingValueD(pin++, knobNumber, DES_KNOB_SIZE);
				if (knobSize <= 0)
					return false;
			}
				
			int touchSize = max(DES_TOUCH_SIZE, knobSize);

			if ((int)setting[0] & bitSetArray[5])  // skip knob line size
				pin++;
			if ((int)setting[0] & bitSetArray[6])  // skip knob line color
				pin += 3;
			if ((int)setting[0] & bitSetArray[7])  // skip knob fill color
				pin += 3;
			if ((int)setting[0] & bitSetArray[8])  // skip knob alpha
				pin++;

			int trackIndex = 0;
			bool trackNow = false;
			int mouseTracking = 1;
			if ((int)setting[0] & bitSetArray[9]) {  // get knob tracking
				trackIndex = getLoopingValueD(pin++, knobNumber, 0);
				trackNow = getLoopingValueD(pin++, knobNumber, 0);
				mouseTracking = getLoopingValueD(pin++, knobNumber, 1);
			}
			if (mouseTracking <= 0)
				continue;

			int arrayIndex = element[knobNumber];
			int knobX = x[arrayIndex];
			int knobY = y[arrayIndex];

			if (mouseX > knobX + touchSize)
				continue; // missed dial
			if (mouseX < knobX - touchSize)
				continue; // missed dial

			if (mouseY > knobY + touchSize)
				continue; // missed dial
			if (mouseY < knobY - touchSize)
				continue; // missed dial

			memory[0] = knobNumber;
			hitOne = true;

			if ((int)setting[0] & bitSetArray[11]) {  // has active output pin
				for (int zz = 0; zz < setting[1]; zz++) {
					if (knobNumber == zz)
						output[3].value[zz] = 1;
					else
						output[3].value[zz] = 0;
				}
			}
			return true;
		}
		if (!hitOne) {
			memory[0] = -1;
			if ((int)setting[0] & bitSetArray[11]) {  // has active output pin
				for (int zz = 0; zz < setting[1]; zz++) {
					output[3].value[zz] = 0;
				}
			}
			return false;
		}
	}
	else {
		if (memory[0] < 0)
			return false;
		knobNumber = memory[0];
	}

	// ok current selection is knobNumber,
	// its located at knobArrayIndex
	// lets see if there is a point in the array that is closer
	int knobArrayIndex = element[knobNumber];
	int knobX = x[knobArrayIndex];
	int knobY = y[knobArrayIndex];
	int dx = mouseX - knobX;
	int dy = mouseY - knobY;
	int oldDist = dx * dx + dy * dy;
	int newDist;
	int oldKnobIndex = knobArrayIndex;
	bool change = false;
	if ((int)setting[0] & bitSetArray[14]) { // FREEFORM -- can jump to any point
		int closestDistanceSoFar = oldDist;
		for (int zz = 0; zz < arrayLength; zz++) {
			if (zz == oldKnobIndex)
				continue;
			dx = mouseX - x[zz];
			dy = mouseY - y[zz];
			newDist = dx * dx + dy * dy;
			if (newDist < closestDistanceSoFar) {
				change = true;
				closestDistanceSoFar = newDist;
				knobArrayIndex = zz;
			}
		}
	}
	else {
		// just test next higher and lower points

		if (knobArrayIndex > 0) { // ok to test lower point
			do {
				change = false;
				dx = mouseX - x[knobArrayIndex - 1];
				dy = mouseY - y[knobArrayIndex - 1];
				newDist = dx * dx + dy * dy;
				if (newDist < oldDist) {
					change = true;
					oldDist = newDist;
					knobArrayIndex--;
				}
			} while (change && (knobArrayIndex > 0));
		}
		else if ((int)setting[0] & bitSetArray[13]) { // index is at zero and wraparound enabled
			change = false;
			dx = mouseX - x[arrayLength - 1];
			dy = mouseY - y[arrayLength - 1];
			newDist = dx * dx + dy * dy;
			if (newDist < oldDist) {
				change = true;
				oldDist = newDist;
				knobArrayIndex = arrayLength - 1;
			}
		}

		if (!change && (knobArrayIndex < arrayLength - 1)) { // ok to test higher point
			do {
				change = false;
				dx = mouseX - x[knobArrayIndex + 1];
				dy = mouseY - y[knobArrayIndex + 1];
				newDist = dx * dx + dy * dy;
				if (newDist < oldDist) {
					change = true;
					oldDist = newDist;
					knobArrayIndex++;
				}
			} while (change && (knobArrayIndex < arrayLength - 1));
		}
		else if ((int)setting[0] & bitSetArray[13]) { // index is at limit and wraparound enabled
		 //change = false;
			dx = mouseX - x[0];
			dy = mouseY - y[0];
			newDist = dx * dx + dy * dy;
			if (newDist < oldDist) {
				//change = true;
				//oldDist = newDist;
				knobArrayIndex = 0;
			}
		}
	}

	if (oldKnobIndex != knobArrayIndex) {
		element[knobNumber] = knobArrayIndex;
		// move the dial
		//dial2d(pg);
		// update the outputs
		method2(pg);
		return true;
	}

	return false;
}


bool Canvas_ArraySlider_Component::onLoad(Sheet* pg) {
	//  rebuild it
	buildPinLabels();

	return true;
}
void Canvas_ArraySlider_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	if((doubleValues[0] > 0) && (doubleValues[0] < 32))
		setting[1] = floor(doubleValues[0]); // knobs

	// clear the bits we will change
	setting[0] = (int)setting[0] & ~( bitSetArray[13] | bitSetArray[14]);
	setting[0] = (int)setting[0] | ( (boolValues[0] ? bitSetArray[13] : 0) + (boolValues[1] ? bitSetArray[14] : 0));
	displayMode = boolValues[2];

	//onLoad(pg);
}
void doubleClick2dSldArray(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Knobs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1] };

	vector<string> checkLabels = {"Wraparound", "Freeform", "Ignore Display Scale"};
	vector<bool> checkValues = { 
		(bool)((int)pg->dgrm.cL[i]->setting[0] & bitSetArray[13]),
		(bool)((int)pg->dgrm.cL[i]->setting[0] & bitSetArray[14]),
		(bool)(pg->dgrm.cL[i]->displayMode) };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
vector<int> Canvas_ArraySlider_Component::GetPinGroups() { return vector<int>{2, 1, 3, 1, 1, 1, 3, 3, 1 }; }
vector<string> Canvas_ArraySlider_Component::GetPinLabels() {
	return vector<string>{
		"Xs", "Ys",								// 0=x and y (1)
		"Track Width",							// 1=track Line Width
		"Track Red", "Track Green", "Track Blue",	// 2=line color     
		"Track Alpha",								// 3=Alpha
		"Knob Size",							// 4-get knob Size
		"Knob Line Width",						// 5-get knob line Width
		"Knob Line Red", "Knob Line Green", "Knob Line Blue",	// 6-Line color: 			
		"Knob Fill Red", "Knob Fill Green", "Knob Fill Blue",	// 7-Line color: 
		"Knob Alpha" }; 						// 8=knob Alpha:
}
std::vector<Button*>the2dArrSldButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dSldArray) };


Canvas_ArraySlider_Component::Canvas_ArraySlider_Component()
	: Component(L"2dArrSld", L"2D Array Slider", squareShape, 2, 1, 3, 1)
{
	dialogButtonList = the2dArrSldButtonList;
	pinMaskIndex = 0;
	setMemory(0, -1 ); // knob in motion
	initialMemoryX.resize(1);
	initialMemoryX[0] = -12345;
	setSetting(0, 1); // 1-X,Y coords, 2-width, 4-line color, 8-alpha -- 10- outputs (1024)
	setSetting(1, 1); // the number of knobs
	setElement(0, 0); // knob zero is at element 0
	setRightLabels(3, "X", "Y", "N");
	// getInternals = getInternals_2dGeom;
	// setInternals = setInternals_2dGeom;
	width = 60;
	completionTestDoubles = 1;
	completionTestChecks = 2;
	setOutputDimension(0, 0, 1);
	setOutputDimension(1, 0, 1);
	setOutputDimension(2, 0, 1);
}


// ******************************* tic and label **************************


int TicAndLabel_Component::method2(Sheet *pg) {

	output[0].value.clear();
	output[1].value.clear();

	int loops = input[0].value.size();
	if (loops == 0)
		return 1;

	output[0].value.resize(loops);
	output[1].value.resize(loops);

	int tic;
	int label;
	for (int k = 0; k < loops; k++) {
		double value = input[0].value[k];
		if (value < -10000)
			return 1;
		if (value > 10000)
			return 1;
		innerTicLabel(value, &tic, &label);
		output[0].value[k] = tic;
		output[1].value[k] = label;
	}

	return 1;
}
TicAndLabel_Component::TicAndLabel_Component()
	: Component(L"TicLab", L"Tic and Label", squareShape, 1, 1, 2, 1)
{
	setRightLabels(3, "Tic", "Label");
}


// ***************************** 2D Meter *********************************

bool Canvas_Meter_Component::dial2d(Sheet* pg){
	if (pg->pCanvasWindow == NULL)
		return false;

	int loops = inputsToLoops();
	if (loops == 0)
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) { // "X", "Y"                      
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputSize = 100;
		if ((int)setting[0] & 2) { // "Size"
			inputSize = getLoopingValueD(pin++, k, 100);
			if (inputSize <= 0)
				continue;
		}

		double inputPlusX = 0;
		double inputPlusY = 0;
		double inputMinusX = 0;
		double inputMinusY = 0;
		if ((int)setting[0] & 4) { // "Plus X", "Plus Y", "Minus X", "Minus Y"  
			inputPlusX = getLoopingValueD(pin++, k, 0);
			inputPlusY = getLoopingValueD(pin++, k, 0);
			inputMinusX = getLoopingValueD(pin++, k, 0);
			inputMinusY = getLoopingValueD(pin++, k, 0);
		}

		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		if ((int)setting[0] & 8) { // "Fill Red", "Fill Green", "Fill Blue"        
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
		}

		string inputTopLabel = "";
		string inputBottomLabel = "";
		if ((int)setting[0] & 16) { // "Top Label", "Bottom Label"                      
			inputTopLabel = getLoopingStringValueD(pin++, k, "");
			inputBottomLabel = getLoopingStringValueD(pin++, k, "");
		}

		double inputAlpha = 1.0;
		if ((int)setting[0] & 32) { // "Alpha"
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if ((inputAlpha <= 0))
				continue;
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 64) { // "Line Red", "Line Green", "Line Blue"        
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputData = 0.0;
		if ((int)setting[0] & 128) { // "Data"
			inputData = getLoopingValueD(pin++, k, 0);
		}


		double xx = xTo2dCanvas2(pg, inputX, displayMode);
		double yy = yTo2dCanvas2(pg, inputY, displayMode);
		double fullSize = sizeTo2dCanvas(pg, inputSize, displayMode);
		double halfSize = fullSize / 2;
		double frame = halfSize - 10;
		double pointer = halfSize;
		double yOffset = 0.6 * halfSize;

		// mode
		bool drawFrame = ((int)setting[1] & 1);
		bool drawScale = ((int)setting[1] & 2);
		int fontSize = 10;

		if ((inputPlusX != 0) || (inputPlusY != 0) || (inputMinusX != 0) || (inputMinusY != 0)) {
			//ctx2.strokeStyle = RGB(0, 0, 0);
			//ctx2.fillStyle = ctx2.strokeStyle;

			// code gets a bit fancy but saves 150 lines of code
			double pmx[2] = { xTo2dCanvas(pg, inputPlusX), xTo2dCanvas(pg, inputMinusX) };
			double pmy[2] = { yTo2dCanvas(pg, inputPlusY), yTo2dCanvas(pg, inputMinusY) };

			// decide where bends should be:
			//
			//    8  |  1  |  2
			//-------+-----+-----
			//    7  |  9  |  3
			//-------+-----+-----
			//    6  |  5  |  4

			string sym[2] = { "+", "-" };
			int code[2] = { 0, 0 };

			for (int p = 0; p < 2; p++) {
				// run both plus and minus points
				if (pmx[p] > xx + halfSize) { // 2,3,4
					if (pmy[p] > yy + halfSize) code[p] = 2;
					else if (pmy[p] < yy - halfSize) code[p] = 4;
					else code[p] = 3;
				}
				else if (pmx[p] < xx - halfSize) { // 6,7,8
					if (pmy[p] > yy + halfSize) code[p] = 8;
					else if (pmy[p] < yy - halfSize) code[p] = 6;
					else code[p] = 7;
				}
				else {
					if (pmy[p] > yy + halfSize) code[p] = 1;
					else if (pmy[p] < yy - halfSize) code[p] = 5;
					else code[p] = 9;
				}

				double tx; // terminus at meter
				double ty;
				double bx; // bend on way to meter
				double by;
				double lx; // +/- label
				double ly;

				// draw the terminal if needed
				if (code[p] != 9) {
					BeginPath(pg->hDC2);
					Ellipse(pg->hDC2, pmx[p] - 4, pmy[p] - 4, pmx[p] + 4, pmy[p] + 4);
					EndPath(pg->hDC2);
					StrokePath(pg->hDC2);

					//center2dTextHV(sym[p], pmx[p]+5, pmy[p]+5);
					if (code[p] % 2 == 1) { // 1357
						// single line
						switch (code[p]) {
						case 1:
							tx = pmx[p];
							ty = yy + halfSize;
							lx = tx + 5;
							ly = ty + 6;
							break;
						case 3:
							tx = xx + halfSize;
							ty = pmy[p];
							lx = tx + 5;
							ly = ty - 6;
							break;
						case 5:
							tx = pmx[p];
							ty = yy - halfSize;
							lx = tx + 5;
							ly = ty - 6;
							break;
						case 7:
							tx = xx - halfSize;
							ty = pmy[p];
							lx = tx - 5;
							ly = ty - 6;
							break;
						}
						BeginPath(pg->hDC2);
						MoveToEx(pg->hDC2, pmx[p], pmy[p], NULL);
						LineTo(pg->hDC2, tx, ty);
						EndPath(pg->hDC2);
						StrokePath(pg->hDC2);
					}
					else {
						if (abs(pmx[0] - pmx[1]) > abs(pmy[0] - pmy[1])) { //draw to sides
							bx = pmx[p];
							by = yy;
							ty = yy;
							ly = ty - 6;
							switch (code[p]) {
							case 2:
							case 4:
								tx = xx + halfSize;
								lx = tx + 5;
								break;
							case 6:
							case 8:
								tx = xx - halfSize;
								lx = tx - 5;
								break;
							}
						}
						else { // draw to top and bottom
							bx = xx;
							by = pmy[p];
							tx = xx;
							lx = tx + 5;
							switch (code[p]) {
							case 2:
							case 8:
								ty = yy + halfSize;
								ly = ty + 6;
								break;
							case 4:
							case 6:
								ty = yy - halfSize;
								ly = ty - 6;
								break;
							}
							//  code[p] = -code[p];
						}
						BeginPath(pg->hDC2);
						MoveToEx(pg->hDC2, pmx[p], pmy[p], NULL);
						LineTo(pg->hDC2, bx, by);
						LineTo(pg->hDC2, tx, ty);
						EndPath(pg->hDC2);
						StrokePath(pg->hDC2);
					}
					// label the plus lead
					int textWidth = calcStringWidthFontSize(pg->hDC2, 10, sym[p]);
					drawTextString(pg->hDC2, lx - textWidth / 2, ly - 9, 10, 0, sym[p]);
				}
			}
		}
		
		if (drawFrame) { // no frame reduction yet
			HBRUSH fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));
			HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(0, 0, 0));
			SelectObject(pg->hDC2, fillColor);
			SelectObject(pg->hDC2, hPen);
			BeginPath(pg->hDC2);
			Rectangle(pg->hDC2, xx - halfSize, yy - halfSize, xx + halfSize, yy + halfSize);
			EndPath(pg->hDC2);
			StrokeAndFillPath(pg->hDC2);
			DeleteObject(fillColor);
			DeleteObject(hPen);
		}
		if (fullSize < 10)
			continue;
		
		// The Labels
		int labelSize = floor(fullSize / 10);
		if (labelSize > 30)
			labelSize = 30;
		if (labelSize < 10)
			labelSize = 10;
		int textWidth = calcStringWidthFontSize(pg->hDC2, labelSize, inputTopLabel);
		if (textWidth + 5 < fullSize)
			drawTextString(pg->hDC2, xx - textWidth / 2, yy + labelSize/2 - halfSize, labelSize, 0, inputTopLabel);

		textWidth = calcStringWidthFontSize(pg->hDC2, labelSize, inputBottomLabel);
		if (textWidth + 5 < fullSize)
			drawTextString(pg->hDC2, xx - textWidth / 2, yy - 2*labelSize + halfSize, labelSize, 0, inputBottomLabel);
			
		double SWEEP_ANGLE = 1.5; // 100 deg
		int count = 1;  // simple for now
		
		// draw the arc
		if (drawScale || drawFrame) {

			if (pointer > 10) {
				int arcBeginX = xx + pointer * sin(PI_OVER_2 + SWEEP_ANGLE / 2);
				int arcBeginY = yy + yOffset + pointer * cos(PI_OVER_2 + SWEEP_ANGLE / 2);
				int arcEndX = xx + pointer * sin(-PI_OVER_2 - SWEEP_ANGLE / 2);
				int arcEndY = yy + yOffset + pointer * cos(-PI_OVER_2 - SWEEP_ANGLE / 2);

				HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(170, 170, 170));
				BeginPath(pg->hDC2);
				Arc(pg->hDC2, xx - pointer, yy + yOffset - pointer, xx + pointer, yy + yOffset + pointer,
					arcBeginX, arcBeginY, arcEndX, arcEndY);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
				DeleteObject(hPen);
			}
		}
			
		double fullRange = setting[3] - setting[2];
		if (fullRange <= 0)
			return false;

		if (drawScale) {

			// draw some tics
			int ticCount = 20;
			double step = SWEEP_ANGLE / ticCount;
			double minor = pointer * 1.06;
			double tic = pointer * 1.1;
			double label = pointer * 1.12;
			int labelIndex = 0;
			double xTip;
			double yTip;
			double xBase;
			double yBase;
			int skip;
			string text;
			int textWidth;

			SetBkMode(pg->hDC2, TRANSPARENT);
			double labels = halfSize / 20;
			if ((setting[3] > 0) && (setting[2] < 0)) {

				// start from zero and work UP
				skip = floor(ticCount / labels);
				double zeroAngle = -setting[3] / fullRange * SWEEP_ANGLE + SWEEP_ANGLE / 2;
				for (double angle = zeroAngle; angle < SWEEP_ANGLE / 2; angle += step) {

					if (labelIndex % skip == 0) {
						xTip = tic * sin(angle);
						yTip = -tic * cos(angle);
					}
					else {
						xTip = minor * sin(angle);
						yTip = -minor * cos(angle);
					}
					xBase = pointer * sin(angle);
					yBase = -pointer * cos(angle);

					BeginPath(pg->hDC2);
					MoveToEx(pg->hDC2, xx + xBase, yy + yOffset + yBase, NULL);
					LineTo(pg->hDC2, xx + xTip, yy + yOffset + yTip);
					EndPath(pg->hDC2);
					StrokePath(pg->hDC2);

					// label some tics
					if (labelIndex % skip == 0) {
						xTip = label * sin(angle);
						yTip = -label * cos(angle);

						text = tomString(((double)labelIndex / ticCount * fullRange));
						if (text.size() > 5)
							text = tomString(round(((double)labelIndex / ticCount * fullRange) * 1000) / 1000);
						textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, text);
						drawTextString(pg->hDC2, xx + xTip - textWidth / 2, yy + yOffset + yTip - fontSize, fontSize, 0, text);
					}
					labelIndex++;
				}

				labelIndex = 0;
				// start from zero and work DOWN
				skip = floor(ticCount / labels);
				for (double angle = zeroAngle; angle > -SWEEP_ANGLE / 2; angle -= step) {

					if (labelIndex % skip == 0) {
						xTip = tic * sin(angle);
						yTip = -tic * cos(angle);
					}
					else {
						xTip = minor * sin(angle);
						yTip = -minor * cos(angle);
					}
					xBase = pointer * sin(angle);
					yBase = -pointer * cos(angle);

					BeginPath(pg->hDC2);
					MoveToEx(pg->hDC2, xx + xBase, yy + yOffset + yBase, NULL);
					LineTo(pg->hDC2, xx + xTip, yy + yOffset + yTip);
					EndPath(pg->hDC2);
					StrokePath(pg->hDC2);

					// label some tics
					if (labelIndex % skip == 0) {
						xTip = label * sin(angle);
						yTip = -label * cos(angle);

						text = tomString(((double)labelIndex / ticCount * fullRange));
						if (text.size() > 5)
							text = tomString((round(((double)labelIndex / ticCount * fullRange) * 1000) / 1000));
						textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, text);
						drawTextString(pg->hDC2, xx + xTip - textWidth / 2, yy + yOffset + yTip - fontSize, fontSize, 0, text);
					}
					labelIndex--;
				}
			}
			else {
				skip = floor(ticCount / labels);
				if (skip < 1)
					skip = 1;
				for (double angle = -SWEEP_ANGLE / 2; angle < SWEEP_ANGLE / 2; angle += step) {

					if (labelIndex % skip == 0) {
						xTip = tic * sin(angle);
						yTip = -tic * cos(angle);
					}
					else {
						xTip = minor * sin(angle);
						yTip = -minor * cos(angle);
					}
					xBase = pointer * sin(angle);
					yBase = -pointer * cos(angle);

					BeginPath(pg->hDC2);
					MoveToEx(pg->hDC2, xx + xBase, yy + yOffset + yBase, NULL);
					LineTo(pg->hDC2, xx + xTip, yy + yOffset + yTip);
					EndPath(pg->hDC2);
					StrokePath(pg->hDC2);

					// label some tics
					if (labelIndex % skip == 0) {
						xTip = label * sin(angle);
						yTip = -label * cos(angle);

						text = tomString(((double)labelIndex / ticCount * fullRange + setting[2]));
						if (text.size() > 5)
							text = tomString((round(((double)labelIndex / ticCount * fullRange + setting[2]) * 1000) / 1000));
						textWidth = calcStringWidthFontSize(pg->hDC2, fontSize, text);
						drawTextString(pg->hDC2, xx + xTip - textWidth / 2, yy + yOffset + yTip - fontSize, fontSize, 0, text);
					}
					labelIndex++;
				}

			}
		}
		// draw the pointer
		double value = inputData;
		if (value < setting[2])
			value = setting[2];
		if (value > setting[3])
			value = setting[3];

		double angle = (value - setting[2]) / fullRange * SWEEP_ANGLE - SWEEP_ANGLE / 2;
		double xTip = 1.05*pointer * sin(angle);
		double yTip = -1.05*pointer * cos(angle);

		COLORREF strokeStyle = RGB(inputLineRed, inputLineGreen, inputLineBlue);
		HPEN hPen = CreatePen(PS_COSMETIC, 1, strokeStyle);
		SelectObject(pg->hDC2, hPen);
		BeginPath(pg->hDC2);
		MoveToEx(pg->hDC2, xx, yy + yOffset, NULL);
		LineTo(pg->hDC2, xx + xTip, yy + yOffset + yTip);
		EndPath(pg->hDC2);
		StrokePath(pg->hDC2);
		DeleteObject(hPen);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}
void Canvas_Meter_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	setting[2] = doubleValues[0];
	setting[3] = doubleValues[1];

	setting[1] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0);
	displayMode = boolValues[2];
}
void doubleClick2dMeterMode(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels{ "Minimum", "Maximum" };
	vector<double> doubleValues{ pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels{ "Draw Frame", "Draw Scale", "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 1), (bool)((int)pg->dgrm.cL[i]->setting[1] & 2), (bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

vector<int> Canvas_Meter_Component::GetPinGroups() { return vector<int>{2, 1, 4, 3, 2, 1, 3, 1 }; }
vector<string> Canvas_Meter_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",									// 0=Position    (1)
		"Size",										// 1=size        (2)
		"Plus X", "Plus Y", "Minus X", "Minus Y", 	// 2=Anchors     (4)
		"Fill Red", "Fill Green", "Fill Blue",		// 3=fill color  (8)
		"Top Label", "Bottom Label",				// 4=text        (16)
		"Alpha",									// 5=alpha		 (32)
		"Line Red", "Line Green", "Line Blue",		// 6=line color  (64)
		"Data"										// 7=Data		 (128)
		};				
}
std::vector<Button*>the2dMeterButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dMeterMode) };

bool Canvas_Meter_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return true;
}

Canvas_Meter_Component::Canvas_Meter_Component()
	: Canvas_Component(L"2dMeter", L"2D Meter", squareShape, 4, 1, 0, 0)
{
	dialogButtonList = the2dMeterButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2 + 128); // input code: 1-x/y position, 2-size, 4-Leads, 8-fill, 16-labels, 32-alpha, 64-color, 128-data
	setSetting(1, 1 + 2);    // display mode: 1) draw frame, 2) draw scale
	setSetting(2, 0);      // min value
	setSetting(3, 100);    // max value
	completionTestDoubles = 2;
	completionTestChecks = 2;
}

// ************************************ 2D Chart ***************************************

bool Canvas_Chart_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	if (!checkValueInputs())
		return false;
	int MAX_CURVES = 3;
	int curves = 0;
	if ((int)setting[0] & 512)
		curves++;
	if ((int)setting[0] & 1024)
		curves++;
	if ((int)setting[0] & 2048)
		curves++;

	int firstCurveIndex = leftTerminalCount - curves;
	int maxCount = 0;
	for (int ci = 0; ci < curves; ci++) {
		int cix = firstCurveIndex + ci;
		if (input[cix].drivenByComponent != -1)
			if (input[cix].value.size() > maxCount)
				maxCount = input[cix].value.size();
	}

	int loops = inputsToLoopsN(leftTerminalCount - curves);
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputWidth = 100;
		double inputHeight = 100;
		if ((int)setting[0] & 2) {
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 100);
			if (inputWidth <= 10)
				continue;
			if (inputHeight <= 10)
				continue;
		}

		int inputMode = 0;// AT FRIST mode 0 = lines ; mode 1 - dots; mode 3 - labels 
		if ((int)setting[0] & 4) {
			inputMode = getLoopingValueD(pin++, k, 0);
		}

		int inputDeltaX = 1;
		if ((int)setting[0] & 8) {
			inputDeltaX = getLoopingValueD(pin++, k, 1);
		}

		int inputLineWidth = 1;
		int inputDotSize = 3;
		if ((int)setting[0] & 16) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			inputDotSize = getLoopingValueD(pin++, k, 3);
		}

		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		if ((int)setting[0] & 32) { // fill Color
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
		}

		double inputAlpha = 1.0;
		if ((int)setting[0] & 64) { // Alpha
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		double inputRangeX = 0;
		if ((int)setting[0] & 128) {
			inputRangeX = getLoopingValueD(pin++, k, 0);
		}

		double inputMinY = -1e100;
		double inputMaxY = 1e100;
		bool limitYrange = false;
		if ((int)setting[0] & 256) {
			inputMinY = getLoopingValueD(pin++, k, -1e100);
			inputMaxY = getLoopingValueD(pin++, k, 1e100);
			limitYrange = true;
		}

		int xx = xTo2dCanvas2(pg, inputX, displayMode);
		int yy = yTo2dCanvas2(pg, inputY, displayMode);
		int xwidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
		if ((xwidth <= 10) || (xwidth > 10000))
			continue;
		int yheight = -sizeTo2dCanvas(pg, inputHeight, displayMode);
		if ((-yheight <= 10) || (-yheight > 10000))
			continue;
		xx -= xwidth / 2;
		yy -= yheight / 2;

		// draw the frame
		int border = 3;
		BeginPath(pg->hDC2);
		Rectangle(pg->hDC2, xx- border, yy + border, xx + xwidth+ border, yy + yheight - border);
		EndPath(pg->hDC2);

		HBRUSH	fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));
		SelectObject(pg->hDC2, fillColor);
		
		StrokeAndFillPath(pg->hDC2);
		DeleteObject(fillColor);
		
		if (maxCount < 2)
			continue;

		double min = 1e100;
		double max = -1e100;
		for (int ci = 0; ci < curves; ci++) {
			int cix = firstCurveIndex + ci;
			if (input[cix].drivenByComponent != -1)
				for (int j = 0; j < input[cix].value.size(); j++) {
					if (input[cix].value[j] < min)
						min = input[cix].value[j];
					if (input[cix].value[j] > max)
						max = input[cix].value[j];
				}
		}
		if (max == min) {
			max += 1;
			min -= 1;
		}

		if (inputRangeX > 0) {
			if(maxCount > inputRangeX)
				maxCount = inputRangeX;
		}
		if (maxCount < 2)
			continue;

		if (limitYrange) {
			if(min < inputMinY)
				min = inputMinY;
			if(max > inputMaxY)
				max = inputMaxY;
		}
		double xScale = (double)xwidth / (maxCount - 1);
		double yScale = yheight / (max - min);

		inputMode++;// NOW mode 1 = lines ; mode 2 - dots; mode 4 - labels 
		COLORREF minorGridColor = RGB(200, 200, 255);
		HPEN hMinorPen = CreatePen(PS_COSMETIC, 1, minorGridColor);
		COLORREF majorGridColor = RGB(100, 100, 255);
		HPEN hMajorPen = CreatePen(PS_COSMETIC, 1, majorGridColor);
		//COLORREF labelColor = RGB(100, 100, 100);
		
		// draw the vertical axis grid - horizontal lines
		
		// pick nice round number for tic
		double label = pickLabelSeparation(-yScale);
		if (label != 0) {
			double start = floor(min / label) * label;
			int labelCounter;
			if (start >= 0)
				labelCounter = (int)(10 - (label * 10 - start) / label) % 10; //  because the data doesnt start at zero
			else
				labelCounter = 10 + ((int)((start - label * 10) / label) % 10);

			SelectObject(pg->hDC2, hMinorPen);
			for (int lv = start; lv <= max; lv += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC2, hMajorPen);
				BeginPath(pg->hDC2);
				int yp = (lv - min) * yScale + yy;
				MoveToEx(pg->hDC2, xx, yp, NULL);
				LineTo(pg->hDC2, xx+xwidth, yp);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);

				if (labelCounter >= 10) {
					labelCounter = 0;
					if (inputMode & 4) {
						string tex = tomString(lv);
						// on right side of graph
						drawTextString(pg->hDC2, xx + xwidth + 4, yp - 6, 10, 0, tex);
						// on left side of graph
						int wid = calcStringWidthFontSize(pg->hDC2, 10, tex);
						drawTextString(pg->hDC2, xx - wid - 4, yp - 6, 10, 0, tex);
					}
					SelectObject(pg->hDC2, hMinorPen);
				}
				labelCounter++;
			}
		}

		// draw the horizontal axis grid - vertical lines
		// pick nice round number for tic
		double otherCount = maxCount * inputDeltaX;
		double otherScale = xwidth / (otherCount - 1);
		SelectObject(pg->hDC2, hMinorPen);
		label = pickLabelSeparation(otherScale);
		int labelCounter = 10;
		if (label != 0) {
			//start = Math.ceil(0 / label) * label;
			for (double lv = 0; lv < otherCount - 1; lv += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC2, hMajorPen);
				BeginPath(pg->hDC2);
				int xp = lv * otherScale + xx;
				MoveToEx(pg->hDC2, xp, yy, NULL);
				LineTo(pg->hDC2, xp, yy + yheight);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);

				if (labelCounter >= 10) {
					labelCounter = 0;
					if (inputMode & 4) {
						string tex = tomString(lv);
						int wid = calcStringWidthFontSize(pg->hDC2, 10, tex);
						drawTextString(pg->hDC2, xp - wid/2, yy + 3, 10, 0, tex);
						drawTextString(pg->hDC2, xp - wid / 2, yy + yheight - 15, 10, 0, tex);
					}
					SelectObject(pg->hDC2, hMinorPen);
				}
				labelCounter++;
			}
		}
		DeleteObject(hMinorPen);
		DeleteObject(hMajorPen);

		// finally draw the data
		// display mode: 0=lines, 1= dots, 2= lines and dots
		int dataPin = firstCurveIndex;
		for (int ci = 0; ci < MAX_CURVES; ci++) {
			int cix;
			COLORREF penColor;
			switch (ci) {
			case 0:
				if (((int)setting[0] & 512) == 0)
					continue;
				penColor = RGB(0, 0, 0);
				cix = dataPin++;
				break;
			case 1: 
				if (((int)setting[0] & 1024) == 0)
					continue;
				penColor = RGB(255, 0, 0);
				cix = dataPin++;
				break;
			case 2: 
				if (((int)setting[0] & 2048) == 0)
					continue;
				penColor = RGB(0, 255, 0);
				cix = dataPin++;
				break;
			}

			if (input[cix].drivenByComponent == -1)
				continue;

			// Draw the lines
			if (inputMode & 1) {
				
				inputLineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				HPEN hPen = CreatePen(PS_COSMETIC, inputLineWidth, penColor);
				SelectObject(pg->hDC2, hPen);

				BeginPath(pg->hDC2);
				int limit = min(input[cix].value.size(), maxCount);
				int pen = 0; // up
				for (int j = 0; j < limit; j++) {
					if (input[cix].value[j] > max) {
						pen = 0;
						continue;
					}
					if (input[cix].value[j] < min) {
						pen = 0;
						continue;
					}

					int xp = j * xScale + xx;
					int yp = (input[cix].value[j] - min) * yScale + yy;
					if (pen == 0) {
						MoveToEx(pg->hDC2, xp, yp, NULL);
						pen = 1; // down
					}
					else
						LineTo(pg->hDC2, xp, yp);
				}
				EndPath(pg->hDC2);

				StrokePath(pg->hDC2);
				DeleteObject(hPen);
			}

			// draw the dots
			if (inputMode & 2) {
			
				HBRUSH	fillColor = CreateSolidBrush(penColor);
				SelectObject(pg->hDC2, fillColor);
				int limit = min(input[cix].value.size(), maxCount);
				BeginPath(pg->hDC2);
				for (int j = 0; j < limit; j++) {

					if (input[cix].value[j] > max)
						continue;
					if (input[cix].value[j] < min)
						continue;

					int xp = j * xScale + xx;
					int yp = (input[cix].value[j] - min) * yScale + yy;
					Ellipse(pg->hDC2, xp - inputDotSize, yp - inputDotSize, xp + inputDotSize, yp + inputDotSize);
				}
				EndPath(pg->hDC2);
				StrokeAndFillPath(pg->hDC2);
				DeleteObject(fillColor);
			}
		}

	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

vector<int> Canvas_Chart_Component::GetPinGroups() { return vector<int>{2, 2, 1, 1, 2, 3, 1, 1, 2, 1, 1, 1 }; }
vector<string> Canvas_Chart_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",								// 0=Position    (1)
		"Width", "Height",						// 1=size        (2)
		"Mode", 								// 2==			 (4)
		"Delta X", 								// 2==			 (8)
		"Line Width", "Dot Size",				// 3=			 (16)
		"Fill Red", "Fill Green", "Fill Blue",	// 4=line color  (32)
		"Alpha",								// 5=alpha		 (64)
		"X Axis Range", 						// 6=range x	 (128)
		"Minimum Y", "Maximum Y",				// 7=range Y 	 (256)
		"Data 0",								// 8=Data		 (512)
		"Data 1",								// 9=Data		 (1024)
		"Data 2"};								// 10=Data		 (2048)
}
std::vector<Button*>the2dChartButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dDisplayMode) };

bool Canvas_Chart_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return true;
}

Canvas_Chart_Component::Canvas_Chart_Component()
	: Canvas_Component(L"2dChart", L"2D Chart", squareShape, 5, 1, 0, 0)
{
	dialogButtonList = the2dChartButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1+2+512); // input code
}

// ************************************ 2D XY Chart ***************************************

bool Canvas_XyChart_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;

	int MAX_CURVES = 3;
	int curves = 0;
	if ((int)setting[0] & 256)
		curves++;
	if ((int)setting[0] & 512)
		curves++;
	if ((int)setting[0] & 1024)
		curves++;

	int firstCurveIndex = leftTerminalCount - 2*curves;
	int maxCount = 0;
	for (int ci = 0; ci < curves; ci++) {
		int cix = firstCurveIndex + 2*ci;
		int ciy = cix + 1;
		if (input[cix].drivenByComponent == -1)
			continue;
		if (input[ciy].drivenByComponent == -1)
			continue;
		int limit = min(input[cix].value.size(), input[ciy].value.size());
		if (limit > maxCount)
			maxCount = limit;
	}

	int loops = inputsToLoopsN(leftTerminalCount - 2 * curves);// different here

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputWidth = 100;
		double inputHeight = 100;
		if ((int)setting[0] & 2) {
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 100);
			if (inputWidth <= 30)
				continue;
			if (inputHeight <= 30)
				continue;
		}
	
		int inputMode = 0;// AT FRIST mode 0 = lines ; mode 1 - dots; mode 3 - labels 
		if ((int)setting[0] & 4) {
			inputMode = getLoopingValueD(pin++, k, 0);
		}

		int inputLineWidth = 1;
		int inputDotSize = 3;
		if ((int)setting[0] & 8) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			inputDotSize = getLoopingValueD(pin++, k, 3);
		}

		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		if ((int)setting[0] & 16) { // fill Color
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
		}
	
		double inputAlpha = 1.0;
		if ((int)setting[0] & 32) { // Alpha
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		double inputMinX = -1e100;
		double inputMaxX = 1e100;
		bool limitXrange = false;
		if ((int)setting[0] & 64) {
			inputMinX = getLoopingValueD(pin++, k, -1e100);
			inputMaxX = getLoopingValueD(pin++, k, 1e100);
			limitXrange = true;
		}

		double inputMinY = -1e100;
		double inputMaxY = 1e100;
		bool limitYrange = false;
		if ((int)setting[0] & 128) {
			inputMinY = getLoopingValueD(pin++, k, -1e100);
			inputMaxY = getLoopingValueD(pin++, k, 1e100);
			limitYrange = true;
		}

		int xx = xTo2dCanvas2(pg, inputX, displayMode);
		int yy = yTo2dCanvas2(pg, inputY, displayMode);
		int xwidth = sizeTo2dCanvas(pg, inputWidth, displayMode);
		if ((xwidth <= 10) || (xwidth > 10000))
			continue;
		int yheight = -sizeTo2dCanvas(pg, inputHeight, displayMode);
		if ((yheight <= 10) || (yheight > 10000))
			continue;

		xx -= xwidth / 2;
		yy -= yheight / 2;

		// draw the frame
		int border = 3;
		BeginPath(pg->hDC2);
		Rectangle(pg->hDC2, xx - border, yy + border, xx + xwidth + border, yy + yheight - border);
		EndPath(pg->hDC2);

		HBRUSH	fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));
		SelectObject(pg->hDC2, fillColor);

		StrokeAndFillPath(pg->hDC2);
		DeleteObject(fillColor);

		if (maxCount < 2)
			continue;

		double minX = 1e100;
		double maxX = -1e100;
		double minY = 1e100;
		double maxY = -1e100;
		for (int ci = 0; ci < curves; ci++) {
			int cix = firstCurveIndex + 2*ci;
			int ciy = cix + 1;
			if (input[cix].drivenByComponent == -1)
				continue;
			if (input[ciy].drivenByComponent == -1)
				continue;
			int limit = min(input[cix].value.size(), input[ciy].value.size());
			for (int j = 0; j < limit; j++) {
				if (input[cix].value[j] < minX)
					minX = input[cix].value[j];
				if (input[cix].value[j] > maxX)
					maxX = input[cix].value[j];

				if (input[ciy].value[j] < minY)
					minY = input[ciy].value[j];
				if (input[ciy].value[j] > maxY)
					maxY = input[ciy].value[j];
			}
		}

		if (limitXrange) {
			if(minX < inputMinX)
				minX = inputMinX;
			if(maxX > inputMaxX)
				maxX = inputMaxX;
		}
		if (limitYrange) {
			if(minY < inputMinY)
				minY = inputMinY;
			if(maxY > inputMaxY)
				maxY = inputMaxY;
		}
		if (maxX <= minX)
			maxX = minX + 1;
		if (maxY <= minY) 
			maxY = minY + 1;
	
		double xScale = xwidth / (maxX - minX);
		double yScale = yheight / (maxY - minY);

		inputMode++;// NOW mode 1 = lines ; mode 2 - dots; mode 4 - labels

		COLORREF minorGridColor = RGB(200, 200, 255);
		HPEN hMinorPen = CreatePen(PS_COSMETIC, 1, minorGridColor);
		COLORREF majorGridColor = RGB(100, 100, 255);
		HPEN hMajorPen = CreatePen(PS_COSMETIC, 1, majorGridColor);

		// draw the vertical axis grid - horizontal lines
		// pick nice round number for tic
		double label = pickLabelSeparation(-yScale);
		if (label != 0) {
			double start = floor(minY / label) * label;
			int labelCounter;
			if (start >= 0)
				labelCounter = (int)(10 - (label * 10 - start) / label) % 10; //  because the data doesnt start at zero
			else
				labelCounter = 10 + ((int)((start - label * 10) / label) % 10);

			SelectObject(pg->hDC2, hMinorPen);
			for (double ly = start; ly <= maxY; ly += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC2, hMajorPen);
				BeginPath(pg->hDC2);
				int yp = (ly - minY) * yScale + yy;
				MoveToEx(pg->hDC2, xx, yp, NULL);
				LineTo(pg->hDC2, xx + xwidth, yp);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);

				if (labelCounter >= 10) {
					labelCounter = 0;
					if (inputMode & 4) {
						string tex = tomString(ly);
						// on right side of graph
						drawTextString(pg->hDC2, xx + xwidth + 4, yp - 6, 10, 0, tex);
						// on left side of graph
						int wid = calcStringWidthFontSize(pg->hDC2, 10, tex);
						drawTextString(pg->hDC2, xx - wid - 4, yp - 6, 10, 0, tex);
					}
					SelectObject(pg->hDC2, hMinorPen);
				}
				labelCounter++;
			}
		}
	
		// draw the horizontal axis grid - vertical lines
		// pick nice round number for tic
		label = pickLabelSeparation(xScale);
		if (label != 0) {
			double start = floor(minX / label) * label;
			int labelCounter;
			if (start >= 0)
				labelCounter = (int)(10 - (label * 10 - start) / label) % 10; //  because the data doesnt start at zero
			else
				labelCounter = 10 + ((int)((start - label * 10) / label) % 10);

			SelectObject(pg->hDC2, hMinorPen);
			for (double lx = start; lx <= maxX; lx += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC2, hMajorPen);
				BeginPath(pg->hDC2);
				int xp = (lx - minX) * xScale + xx;
				MoveToEx(pg->hDC2, xp, yy, NULL);
				LineTo(pg->hDC2, xp, yy + yheight);
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);

				if (labelCounter >= 10) {
					labelCounter = 0;
					if (inputMode & 4) {
						string tex = tomString(lx);
						int wid = calcStringWidthFontSize(pg->hDC2, 10, tex);
						drawTextString(pg->hDC2, xp - wid / 2, yy + 3, 10, 0, tex);
						drawTextString(pg->hDC2, xp - wid / 2, yy + yheight - 15, 10, 0, tex);
					}
					SelectObject(pg->hDC2, hMinorPen);
				}
				labelCounter++;
			}
		}
		DeleteObject(hMinorPen);
		DeleteObject(hMajorPen);

		// finally draw the data
		// display mode: 0=lines, 1= dots, 2= lines and dots
		COLORREF penColor;
		if (inputMode & 1) {
			int dataPin = firstCurveIndex;
			for (int ci = 0; ci < MAX_CURVES; ci++) {
				int cix;
				int ciy;

				switch (ci) {
				case 0:
					if (((int)setting[0] & 256) == 0)
						continue;
					penColor = RGB(0, 0, 0);
					cix = dataPin;
					dataPin += 2;
					break;
				case 1:
					if (((int)setting[0] & 512) == 0)
						continue;
					penColor = RGB(255, 0, 0);
					cix = dataPin;
					dataPin += 2;
					break;
				case 2:
					if (((int)setting[0] & 1024) == 0)
						continue;
					penColor = RGB(0, 255, 0);
					cix = dataPin;
					dataPin += 2;
					break;
				}
				ciy = cix + 1;

				// Draw the lines
				int pen = 0; // up
				inputLineWidth = sizeTo2dCanvas(pg, inputLineWidth, displayMode);
				HPEN hPen = CreatePen(PS_COSMETIC, inputLineWidth, penColor);
				SelectObject(pg->hDC2, hPen);

				BeginPath(pg->hDC2);
				int limit = min(input[cix].value.size(), input[ciy].value.size());
				for (int j = 0; j < limit; j++) {

					if (input[cix].value[j] < minX) {
						pen = 0;
						continue;
					}
					if (input[cix].value[j] > maxX) {
						pen = 0;
						continue;
					}
					if (input[ciy].value[j] < minY) {
						pen = 0;
						continue;
					}
					if (input[ciy].value[j] > maxY) {
						pen = 0;
						continue;
					}

					int xp = (input[cix].value[j] - minX) * xScale + xx;
					int yp = (input[ciy].value[j] - minY) * yScale + yy;
					if (pen == 0) {
						MoveToEx(pg->hDC2, xp, yp, NULL);
						pen = 1; // down
					}
					else
						LineTo(pg->hDC2, xp, yp);
				}
				EndPath(pg->hDC2);
				StrokePath(pg->hDC2);
				DeleteObject(hPen);
			}
		}
		if (inputMode & 2) {
			int dataPin = firstCurveIndex;
			for (int ci = 0; ci < MAX_CURVES; ci++) {
				int cix;
				int ciy;

				switch (ci) {
				case 0:
					if (((int)setting[0] & 256) == 0)
						continue;
					penColor = RGB(0, 0, 0);
					cix = dataPin;
					dataPin += 2;
					break;
				case 1:
					if (((int)setting[0] & 512) == 0)
						continue;
					penColor = RGB(255, 0, 0);
					cix = dataPin;
					dataPin += 2;
					break;
				case 2:
					if (((int)setting[0] & 1024) == 0)
						continue;
					penColor = RGB(0, 255, 0);
					cix = dataPin;
					dataPin += 2;
					break;
				}
				ciy = cix + 1;

				HBRUSH	fillColor = CreateSolidBrush(penColor);
				SelectObject(pg->hDC2, fillColor);
				int limit = min(input[cix].value.size(), input[ciy].value.size());
				BeginPath(pg->hDC2);
				for (int j = 0; j < limit; j++) {
					if (input[cix].value[j] < minX) continue;
					if (input[cix].value[j] > maxX) continue;
					if (input[ciy].value[j] < minY) continue;
					if (input[ciy].value[j] > maxY) continue;

					int xp = (input[cix].value[j] - minX) * xScale + xx;
					int yp = (input[ciy].value[j] - minY) * yScale + yy;
					Ellipse(pg->hDC2, xp - inputDotSize, yp - inputDotSize, xp + inputDotSize, yp + inputDotSize);
				}
				EndPath(pg->hDC2);
				StrokeAndFillPath(pg->hDC2);
				DeleteObject(fillColor);
			}
		}
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}

void Canvas_XyChart_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	if(doubleValues[0] >= 1 )
		setting[1] = doubleValues[0];
	
	displayMode = boolValues[0];
}
void doubleClick2dXyChartMode(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels{ "Input Pairs" };
	vector<double> doubleValues{ pg->dgrm.cL[i]->setting[1] };

	vector<string> checkLabels{ "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

vector<int> Canvas_XyChart_Component::GetPinGroups() { return vector<int>{2, 2, 1, 2, 3, 1, 2, 2, 2, 2, 2 }; }
vector<string> Canvas_XyChart_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",								// 0=Position    (1)
		"Width", "Height",						// 1=size        (2)
		"Mode", 								// 2==			 (4)
		"Line Width", "Dot Size",				// 3=			 (8)
		"Fill Red", "Fill Green", "Fill Blue",	// 4=line color  (16)
		"Alpha",								// 5=alpha		 (32)
		"Minimum X", "Maximum X", 				// 6=range x	 (64)
		"Minimum Y", "Maximum Y",				// 7=range Y 	 (128)
		"Data X0", "Data Y0",					// 8=Data		 (256)
		"Data X1", "Data Y1",					// 9=Data		 (512)
		"Data X2", "Data Y2"};					// 10=Data		 (1024)
}
std::vector<Button*>the2dXyChartButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dXyChartMode) };

bool Canvas_XyChart_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return true;
}

Canvas_XyChart_Component::Canvas_XyChart_Component()
	: Canvas_Component(L"2dChart2", L"2D XY Chart", squareShape, 6, 1, 0, 0)
{
	dialogButtonList = the2dXyChartButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1+2+256); // input code
	completionTestDoubles = 1;
	completionTestChecks = 1;
}

// *************************** 2D Field ****************************************

void fieldPlot(HDC hDC_context, double xCenter, double yCenter, double xFrameSize, double yFrameSize,
	double dataWidth, double dataHeight, 
	vector<double> xData, vector<double> yData, 
	double overdrive, bool drawOval, COLORREF ovalColor, COLORREF lineColor, int lineWidth ) 
{
	int points = dataWidth * dataHeight;
	int boxWidth = xFrameSize / dataWidth;
	int boxHeight = yFrameSize / dataHeight;
	int XpStep = boxWidth;
	int YpStep = boxHeight;
	int halfBoxWidth = boxWidth/2;
	int halfBoxHeight = boxHeight/2;

	double maxAmp = 0;
	for (int j = 0; j < points; j++) {
		double amp = sqrt(xData[j] * xData[j] + yData[j] * yData[j]);
		if (amp > maxAmp)
			maxAmp = amp;
	}

	int n, Yp, Xp, x, y;

	if (drawOval) { // draw the ovals in the cells
		HPEN hPen2 = CreatePen(PS_COSMETIC, 1, ovalColor);
		SelectObject(hDC_context, hPen2);
		BeginPath(hDC_context);

		Yp = yCenter - yFrameSize / 2 + YpStep / 2;
		for (y = 0; y < dataHeight; y++) {
			Xp = xCenter - xFrameSize / 2 + XpStep / 2;
			for (x = 0; x < dataWidth; x++) {
				Ellipse(hDC_context, Xp - halfBoxWidth, Yp - halfBoxHeight, Xp + halfBoxWidth, Yp + halfBoxHeight);
				Xp += XpStep;
			}
			Yp += YpStep;
		}

		EndPath(hDC_context);
		StrokePath(hDC_context);
		DeleteObject(hPen2);
	}

	if (lineWidth > 0) {
		HPEN hPen3 = CreatePen(PS_COSMETIC, lineWidth, lineColor);
		SelectObject(hDC_context, hPen3);
		BeginPath(hDC_context);

		n = 0;
		BeginPath(hDC_context);
		Yp = yCenter - yFrameSize / 2 + YpStep / 2;
		for (y = 0; y < dataHeight; y++) {
			Xp = xCenter - xFrameSize / 2 + XpStep / 2;
			for (x = 0; x < dataWidth; x++) {

				// draw the arrows in the cells
				double amp = sqrt(xData[n] * xData[n] + yData[n] * yData[n]);
				double angle = atan2(yData[n], xData[n]);
				double arrowSize = overdrive * amp / maxAmp;
				int xArrow = arrowSize * halfBoxWidth * cos(angle);
				int yArrow = arrowSize * halfBoxHeight * sin(angle);
				MoveToEx(hDC_context, Xp, Yp, NULL);
				LineTo(hDC_context, Xp + xArrow, Yp - yArrow);

				Xp += XpStep;
				n++;
			}
			Yp += YpStep;
		}
		EndPath(hDC_context);
		StrokePath(hDC_context);
		DeleteObject(hPen3);
	}
}

bool Canvas_FieldPlot_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return false;
	if (((int)setting[0] & 128) == 0) // need data inputs
		return false;

	int xDataPin = leftTerminalCount - 2;
	int yDataPin = leftTerminalCount - 1;
	int loops = inputsToLoopsN(leftTerminalCount - 2);

	// check data
	int xDataSize = input[xDataPin].value.size();
	int yDataSize = input[yDataPin].value.size();
	if (xDataSize != yDataSize) // must be the same size
		return false;
	if ((input[xDataPin].dimensions.size() != 2) ||
		(input[yDataPin].dimensions.size() != 2)) // both must be 2 dimensional
		return false;
	if ((input[xDataPin].dimensions[0] != input[yDataPin].dimensions[0]) || // dimensions must be the same
		(input[xDataPin].dimensions[1] != input[yDataPin].dimensions[1])   )
		return false; 
	if ((input[xDataPin].dimensions[0] < 2) || 
		(input[xDataPin].dimensions[1] < 2)) // need at least 2 data points in each direction
		return false;

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		if ((int)setting[0] & 1) { //  "X", "Y",
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
		}

		double inputWidth = 100;
		double inputHeight = 100;
		if ((int)setting[0] & 2) { //  "Width", "Height"
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 100);
			if (inputWidth <= 30)
				continue;
			if (inputHeight <= 30)
				continue;
		}	

		int inputFillRed = 255;
		int inputFillGreen = 255;
		int inputFillBlue = 255;
		if ((int)setting[0] & 4) {
			inputFillRed = getLoopingValueD(pin++, k, 0);
			inputFillGreen = getLoopingValueD(pin++, k, 0);
			inputFillBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputOvalRed = 0;
		int inputOvalGreen = 0;
		int inputOvalBlue = 0;
		if ((int)setting[0] & 8) {
			inputOvalRed = getLoopingValueD(pin++, k, 0);
			inputOvalGreen = getLoopingValueD(pin++, k, 0);
			inputOvalBlue = getLoopingValueD(pin++, k, 0);
		}

		int inputLineRed = 0;
		int inputLineGreen = 0;
		int inputLineBlue = 0;
		if ((int)setting[0] & 16) {
			inputLineRed = getLoopingValueD(pin++, k, 0);
			inputLineGreen = getLoopingValueD(pin++, k, 0);
			inputLineBlue = getLoopingValueD(pin++, k, 0);
		}

		
		double inputLineWidth = 1;
		double inputOverdrive = 1;
		if ((int)setting[0] & 32) { //  "Overdrive",
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			inputOverdrive = getLoopingValueD(pin++, k, 1);
		}

		double inputAlpha = 1;
		if ((int)setting[0] & 64) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		int xCenter = xTo2dCanvas2(pg, inputX, displayMode);
		int yCenter = yTo2dCanvas2(pg, inputY, displayMode);
		int xFrameSize = sizeTo2dCanvas(pg, inputWidth, displayMode);
		if ((xFrameSize <= 10) || (xFrameSize > 10000))
			continue;
		int yFrameSize = sizeTo2dCanvas(pg, inputHeight, displayMode);
		if ((yFrameSize <= 10) || (yFrameSize > 10000))
			continue;
		int halfXframeSize = xFrameSize / 2;
		int halfYframeSize = yFrameSize / 2;

		// lets frame it
		if ((int)setting[1] & 1) {
			HBRUSH fillColor = CreateSolidBrush(RGB(inputFillRed, inputFillGreen, inputFillBlue));
			HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(0, 0, 0));
			SelectObject(pg->hDC2, fillColor);
			SelectObject(pg->hDC2, hPen);
			BeginPath(pg->hDC2);
			Rectangle(pg->hDC2, xCenter - halfXframeSize, yCenter - halfYframeSize, xCenter + halfXframeSize, yCenter + halfYframeSize);
			EndPath(pg->hDC2);
			StrokeAndFillPath(pg->hDC2);
			DeleteObject(fillColor);
			DeleteObject(hPen);
		}

		vector<double> dataXinfo;
		vector<double> dataYinfo;
		dataXinfo.resize(xDataSize);
		dataYinfo.resize(yDataSize);

		// grab the data
		for (int j = 0; j < xDataSize; j++) {
			dataXinfo[j] = input[xDataPin].value[j];
			dataYinfo[j] = input[yDataPin].value[j];
		}

		int dim0 = input[xDataPin].dimensions[0];
		int dim1 = input[xDataPin].dimensions[1];

		bool drawOval = false;
		if ((int)setting[1] & 2)
			drawOval = true;

		COLORREF ovalColor = RGB(inputOvalRed, inputOvalGreen, inputOvalBlue);
		COLORREF lineColor = RGB(inputLineRed, inputLineGreen, inputLineBlue);
		fieldPlot(pg->hDC2, xCenter, yCenter, xFrameSize, yFrameSize, 
			dim0, dim1, dataXinfo, dataYinfo, inputOverdrive, drawOval, ovalColor, lineColor, inputLineWidth);
	}
	SelectObject(pg->hDC2, GetColorPen(0, 1));
	SelectObject(pg->hDC2, GetStockObject(WHITE_BRUSH));
	return true;
}
vector<int> Canvas_FieldPlot_Component::GetPinGroups() { return vector<int>{2, 2, 3, 3, 3, 2, 1, 2}; }
vector<string> Canvas_FieldPlot_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y",								// 0=Position    (1)
		"Width", "Height",						// 1=size        (2)
		"Fill Red", "Fill Green", "Fill Blue",	// 3=line color  (4)
		"Oval Red", "Oval Green", "Oval Blue",	// 2=line color  (8)
		"Line Red", "Line Green", "Line Blue",	// 4=line color  (16)
		"Line Width", "Overdrive",				// 5=			 (32)
		"Alpha",								// 6=alpha		 (64)
		"X Data", "Y Data"};					// 7==			 (128)
}

void Canvas_FieldPlot_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	setting[1] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0);
	displayMode = boolValues[2];
}
void doubleClickFieldPlot(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	
	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels{ "Show the frame", "Show the ovals", "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 1), (bool)((int)pg->dgrm.cL[i]->setting[1] & 2),
		(bool)pg->dgrm.cL[i]->displayMode };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>theFieldPlotButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickFieldPlot) };

bool Canvas_FieldPlot_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return true;
}
Canvas_FieldPlot_Component::Canvas_FieldPlot_Component()
	: Canvas_Component(L"2dField", L"2D Field Plot", squareShape, 11, 1, 0, 0)
{
	dialogButtonList = theFieldPlotButtonList;
	pinMaskIndex = 0;
	setSetting(0, 1 + 2 + 128); // input code
	setSetting(1, 3); // 1 draw frame, 2 draw oval
	width = 40;
	completionTestChecks = 3;
}



// *********************** Bitmap (the component) *******************************

bool Canvas_Bitmap_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return clearOutputs();


	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	output[1].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		output[0].value[k] = 0;
		output[1].value[k] = 0;

		getPositionGroup(group++, &pin, k);		// "X Position", "Y Position" 
		getSizeGroup(group++, &pin, k, 10);		// "Width", "Height"
		if ((inputSizeX <= 0) || (inputSizeY <= 0))
			continue;

		string bitmapPath = "test.bmp";
		if ((int)setting[0] & bitSetArray[group++]) {
			bitmapPath = getLoopingStringValueD(pin++, k, "test.bmp");
			if (bitmapPath.size() < 4)
				bitmapPath = "test.bmp";
		}

		LPWSTR path = ConvertToLPWSTR(bitmapPath);
		if (z(path, pathLoaded) == 0) {
			if (hBitmapLoaded) {
				DeleteObject(hBitmapLoaded);
				pathLoaded = ConvertToLPWSTR("");
			}
			hBitmapLoaded = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);
			if (hBitmapLoaded == NULL) {
				//MessageBox(NULL, L"Can't find BMP file.", L"2D Bitmap", MB_OK);
				continue;
			}
		}
		HDC hdcMem = CreateCompatibleDC(pg->hDC2);
		SelectObject(hdcMem, hBitmapLoaded);

		BITMAP bm = { 0 };
		GetObject(hBitmapLoaded, sizeof(bm), &bm);
		output[0].value[k] = bm.bmWidth;
		output[1].value[k] = bm.bmHeight; //  these get out eventually

		// NOW finish loading the inputs

		double inputBitmapLeft = 0;
		double inputBitmapTop = 0;
		if ((int)setting[0] & bitSetArray[group++]) {
			inputBitmapLeft = getLoopingValueD(pin++, k, 0); //"Bitmap Left",
			if (inputBitmapLeft <= 0)
				inputBitmapLeft = 0;
			inputBitmapTop = getLoopingValueD(pin++, k, 0); //"Bitmap Top"
			if (inputBitmapTop <= 0)
				inputBitmapTop = 0;
		}

		double inputBitmapWidth = bm.bmWidth;
		double inputBitmapHeight = bm.bmHeight;
		if ((int)setting[0] & bitSetArray[group++]) {
			inputBitmapWidth = getLoopingValueD(pin++, k, bm.bmWidth); //"Bitmap width",
			if (inputBitmapWidth > bm.bmWidth)
				inputBitmapWidth = bm.bmWidth;
			inputBitmapHeight = getLoopingValueD(pin++, k, bm.bmHeight); //"Bitmap height"
			if (inputBitmapHeight > bm.bmHeight)
				inputBitmapHeight = bm.bmHeight;
		}

		double inputAlpha = getLoopingValueD(group++, &pin, k, 1);
		if ((inputAlpha <= 0))
			continue;

		double inputWidth = sizeTo2dCanvas(pg, inputSizeX, displayMode);
		if ((inputWidth <= 0) || (inputWidth > 10000))
			continue;
		double inputHeight = sizeTo2dCanvas(pg, inputSizeY, displayMode);
		if ((inputHeight <= 0) || (inputHeight > 10000))
			continue;

		int xx = xTo2dCanvas2(pg, inputX + setting[2] * inputWidth / 2, displayMode);
		int yy = yTo2dCanvas2(pg, inputY + setting[3] * inputHeight / 2, displayMode);

		StretchBlt(pg->hDC2, xx - inputWidth / 2, yy - inputHeight / 2, inputWidth, inputHeight, hdcMem,
			inputBitmapLeft, inputBitmapTop, inputBitmapWidth, inputBitmapHeight, SRCCOPY);

		DeleteDC(hdcMem);
		//DeleteObject(hBitmapLoaded);
	}

	return true;
}

void doubleClick2dBitmapDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "X Offset:",					"Y Offset:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels{ "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, {}, 0,
		button2dDisplayModeAndDashAndOffset_Completion);
}
std::vector<Button*>bitmapButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dBitmapDialog) };

Canvas_Bitmap_Component::~Canvas_Bitmap_Component() {
	if (hBitmapLoaded)
		DeleteObject(hBitmapLoaded);
	hBitmapLoaded = NULL;
}
bool Canvas_Bitmap_Component::onLoad(Sheet* pg) {
	if (setting.size() < 4) {
		setSetting(1, 0); // spare
		setSetting(2, 0); // x offset
		setSetting(3, 0); // y offset
	}
	buildPinLabels();
	return true;
}
vector<int> Canvas_Bitmap_Component::GetPinGroups() { return vector<int>{2, 2, 1, 2, 2, 1 }; }
vector<string> Canvas_Bitmap_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position",	// 0
			"Width", "Height",			// 1
			"Source Path",				// 2
			"Left", "Top",				// 3
			"Right", "Bottom",			// 4
			"Alpha" };					// 5
}
Canvas_Bitmap_Component::Canvas_Bitmap_Component()
	: Canvas_Component(L"2dBitmap", L"2D Bitmap", squareShape, 5, 1, 2, 1)
{
	dialogButtonList = bitmapButtonList;
	setRightLabels(3, "Width", "Height");
	setSetting(0, 1 + 2); // input pins
	setSetting(1, 0); // spare
	setSetting(2, 0); // x offset
	setSetting(3, 0); // y offset
	pinMaskIndex = 0;
	width = 35;
	pathLoaded = ConvertToLPWSTR("");
	hBitmapLoaded = NULL;
}
