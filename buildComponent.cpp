
#include "stdafx.h"
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <string>
#include "structs.h"
#include "Sheet.h"
#include "Button.h"
#include "buildComponent.h"

#include "FrameButtons.h"
#include "ButtonGroups.h"
#include "Wire.h"
#include <windows.h>
#include <shobjidl.h>
#include "FunctionComponents.h"
#include "Text.h"
#include "Time.h"

#include "MathComponents.h"
#include "LogicComponents.h"
#include "MethodComponents.h"
#include "ArrayComponents.h"
#include "CanvasComponents.h"
#include "ScreenDialog.h"
#include "Fileio.h"
#include "DataComponents.h"
#include "color.h"
#include "ThreeDComponents.h"
#include <GL/glut.h>
#include "ImageComponents.h"
#include "Audio.h"
#include "Details.h"
#include "Mouse.h"
#include "Tester.h"
#include "MIDI.h"
//#include "CanvasBitmap.h"
#include "MST01.h"

#pragma warning (disable: 4267)
#pragma warning (disable: 4244)
#pragma warning (disable: 26451)

extern std::string debugString;


extern bool connected;// wires that are being dragged with a component
extern vector<int> connectedWireNumber; 




// these are the Sheets, setup the main Sheet
Sheet* page; // Sheets contain everything needed for this solution

vector< InnerSheet*> isL;


void buildSheets(HWND hWnd, int tempThreeDWindowHandle) {
	
	page = new Sheet();
	page->hWnd = hWnd;
	page->systemTime = 0.0;
	page->timeStep = 0.050;  // fifty milliseconds

	InnerSheet* inner = new InnerSheet();
	isL.push_back(inner);
	page->dgrm = inner->dgrm;
	//page->dgrm.cL = inner->dgrm.cL;
	//page->dgrm.wL = inner->dgrm.wL;
	page->cXo = inner->cXo;
	page->cYo = inner->cYo;
	page->cs = inner->cs;

	page->isDirty = inner->isDirty;
	page->threeDwindowHandle = tempThreeDWindowHandle;
}

void resetSheets() {

	page->systemTime = 0.0;
	page->timeStep = 0.050;  // fifty milliseconds

	for (int j = 0; j < isL.size(); j++) {
		isL[j]->cXo = 0; // window X offset
		isL[j]->cYo = 0; // window Y offset
		isL[j]->cs = 1; //  window scale
		isL[j]->isDirty = false;
		isL[j]->name.clear();
		isL[j]->dgrm.cL.clear();
		isL[j]->dgrm.wL.clear();
	}
}


string sheetToText(int sheetNum) {
	string text;
	int len = isL[sheetNum]->dgrm.cL.size();
	for (int ic = 0; ic < len; ic++)
		text += isL[sheetNum]->dgrm.cL[ic]->toString().c_str();

	len = isL[sheetNum]->dgrm.wL.size();
	for (int iw = 0; iw < len; iw++)
		text += isL[sheetNum]->dgrm.wL[iw]->toString().c_str();

	return text;
}


//bool suppressOnProgramChange = false;
void loadFromString(Sheet* pg, string text, bool justSheet) {
	//suppressOnProgramChange = true;
	loadSolution_(pg, ConvertToChar(text), justSheet);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	//suppressOnProgramChange = false;
	//draggingSelectionPhase = 0;
}



bool onLoadNull_(Sheet* pg, int i) {
	return true;
}

int pinToHeight(int pins) {
	if (pins < 1)
		pins = 1;
	return ((pins - 1) * TERMINAL_STEP + 2 * TERMINAL_SIZE) / 2 + 2;
}

void Component::copyDimensions(int it, int ot) {

	if (it == -1) {
		output[ot].dimensions.resize(1);
		output[ot].dimensions[0] = 1;
	}
	else {
		int size = input[it].dimensions.size();
		output[ot].dimensions.resize(size);
		for (int j = 0; j < size; j++)
			output[ot].dimensions[j] = input[it].dimensions[j];
	}
}

int calcStringWidth(HDC hDC, LPCTSTR aString) {
	int nCharWidth;
	int width = 0;
	for (int i = 0; i < _tcslen(aString); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth);
		width += nCharWidth;
	}
	return width;
}




int AddComponent(Sheet* pg, LPCTSTR name, POS pos, int sheetNum) {
	Component* temp = buildComponent(pg, name, sheetNum);
	if (temp) {
		
		pg->draggingComponent = pg->dgrm.cL.size();
		pg->mouseDownDX = 0; // fixed jump on first mouse move after placement
		pg->mouseDownDY = 0; // fixed jump on first mouse move after placement
		temp->setPosition((pos.x / pg->cs) - pg->cXo, (pos.y / pg->cs) - pg->cYo);
		temp->fillColor = GetButtonBackgroundColor(pg, name);

		pg->dgrm.cL.push_back(temp);
		
		//if(pg->dgrm.cL[pg->draggingComponent]->onLoad != NULL)
		pg->dgrm.cL[pg->draggingComponent]->onLoad(pg);
		return pg->draggingComponent;
	
	//	mouseOffsetX = 0;
	//	mouseOffsetY = 0;
	//	if (temp->method)
	//		temp->method2(pg, pg->draggingComponent); // for Components that output something when zero is input

		// tag it with the users key
	//	if (pg->currentAttributionKey == 0)
	//		setSheet*AttributionKey(pg);
	//	if (pg->currentAttributionKey != 0)F
	//		temp->attributionKey = pg->currentAttributionKey;

	//	if ((temp->type == L"Buss2In") ||
	//		(temp->type == L"Buss2Out"))
	//		updateAllBuss2Outputs(pg);
	}
	//if (!autoUpdateSheet*Button.isSelected)
	//	debug_hover_depth = 0; // stop debugging Sheet*s
	//clearFindComponent();

	//drawCanvas();
	return -1;
}

void Component::buildInputPins(int pins) {
	// left terminals are inputs
	leftTerminalCount = pins;
	input.clear();
	for (int i = 0; i < pins; i++)
	{
		InputClass* temp = new InputClass(); // each pin is an input class
		temp->value.push_back(0.0);
		temp->dimensions.push_back(0); // 0 means no input, 1 means scalar, n means a vector, n, m means a 2D array of n my m numbers
		temp->drivenByComponent = -1;
		temp->drivenByOutputTerminal = -1;
		input.push_back(*temp);
		delete temp;
	}
}
bool Component::crankResize(int mouseX, int mouseY, bool check) {
	int left = x + width - RESIZE_HANDLE_SIZE;
	int top = y + height - RESIZE_HANDLE_SIZE;

	if (check) {
		if (mouseX < left)
			return false; // missed
		if (mouseY < top)
			return false; // missed
	}

	left = x - width;
	top = y - height;

	width = floor((mouseX - left + RESIZE_HANDLE_SIZE / 2) / 2);
	height = floor((mouseY - top + RESIZE_HANDLE_SIZE / 2) / 2);

	if (width < 20)
		width = 20;
	if (height < MIN_COMPONENT_HEIGHT)
		height = MIN_COMPONENT_HEIGHT;

	return true;
}
bool Component::crankResizeHoldLeft(int mouseX, int mouseY, bool check) {

	int top;
	int left;
	if (check) {
		left = x + width - RESIZE_HANDLE_SIZE;
		if (mouseX < left)
			return false; // missed
		top = y + height - RESIZE_HANDLE_SIZE;
		if (mouseY < top)
			return false; // missed
	}

	//left = x - width;
	top = y - height;

	int delta = mouseX - (x + width - RESIZE_HANDLE_SIZE / 2);
	width = floor(width + delta / 2);
	if (width < 15)
		width = 15;
	else
		x = floor(x + delta / 2);

	height = floor((mouseY - top + RESIZE_HANDLE_SIZE / 2) / 2);

	if (height < MIN_COMPONENT_HEIGHT)
		height = MIN_COMPONENT_HEIGHT;

	return true;
}
void Component::buildOutputPins(int pins) {
	// right terminals are outputs
	rightTerminalCount = pins;
	output.clear();
	for (int i = 0; i < pins; i++) {
		OutputClass* temp = new OutputClass(); // each pin is an otput class
		temp->value.push_back(0.0);
		temp->dimensions.push_back(0); // 0 means no input, 1 means scalar, n means a vector, n, m means a 2D array of n my m numbers
		output.push_back(*temp);
		delete temp;
	}
}

void Component::setSetting(int index, double value) {
	while (setting.size() <= index)// 12-22
		setting.push_back(0.0);
	setting[index] = value; // 12-22
}
void Component::setOutput(int pin, int index, double value) {
	while (output[pin].value.size() <= index) // 12-22
		output[pin].value.push_back(0.0);
	output[pin].value[index] = value;
}
void Component::setMemory(int index, double value) {
	while (memory.size() <= index)// 12-22
		memory.push_back(0.0);
	memory[index] = value;// 12-22
}
void Component::setElement(int index, double value) {
	while (element.size() <= index)// 12-22
		element.push_back(0.0);
	element[index] = value;// 12-22
}
void Component::setLiteral(int index, std::string value) {
	while (literal.size() <= index)// 12-22
		literal.push_back("");
	literal[index] = value;// 12-22
}

//void Component::setInputDimension(int pin, int dim, int size) {
//	while (input[pin].dimensions.capacity() <= dim)
//		input[pin].dimensions.push_back(0.0);
//	input[pin].dimensions[dim] = size;
//}
void Component::setOutputDimension(int pin, int dim, int size){
	while (output[pin].dimensions.capacity() <= dim)
		output[pin].dimensions.push_back(0.0);
	output[pin].dimensions[dim] = size;
}

void Component::setSize(int wid, int hgt) {
	width = wid;
	height = hgt;
}

double Component::getDoubleInputOrZero(int pin, int j) {
	if (input[pin].value.size() <= j)
		return 0;
	if (input[pin].stringType)
		return 0;
	return input[pin].value[j];
}



// This is the Component, it is used for all computational elements
Component::Component(LPCTSTR _type, LPCTSTR _label, int _shapePath, int LTC, int LTS, int RTC, int RTS) {

	x = 0;
	y = 0;
	width = COMPONENT_WIDTH;
	height = COMPONENT_HEIGHT;
	if( _shapePath == squareShape )
		height = pinToHeight(max(LTC, RTC));

	ghost = NORMAL_GHOST; // this dims the Component or makes it not work - see above

	type = _type;
	label = _label;
	userLabel = "";

	selected = false;
//	attributionKey = 0;
	displayMode = 0; // 0=on canvas; 1=pixels on glass

	shapePath = _shapePath;

	//innerMethod = _innerMethod;
	timeBased = false;
	
	// left terminals are inputs
	leftTerminalStep = LTS;
	buildInputPins(LTC);

	// right terminals are outputs
	rightTerminalStep = RTS;
	buildOutputPins(RTC);

	pinMaskIndex = -1;
}
Component::Component(LPCTSTR _type, LPCTSTR _label, int LTC, int RTC) {
	x = 0;
	y = 0;
	width = COMPONENT_WIDTH;
	height = pinToHeight(max(LTC, RTC));

	ghost = NORMAL_GHOST; // this dims the Component or makes it not work - see above

	type = _type;
	label = _label;
	userLabel = "";

	selected = false;
//	attributionKey = 0;
	displayMode = 0; // 0=on canvas; 1=pixels on glass

	shapePath = squareShape;

	timeBased = false;

	// left terminals are inputs
	leftTerminalStep = 1;
	buildInputPins(LTC);

	// right terminals are outputs
	rightTerminalStep = 1;
	buildOutputPins(RTC);

	pinMaskIndex = -1;
}

InputClass::~InputClass() {
	//std::vector<double> value;
	//std::vector<int> dimensions;
	//std::vector<std::string> stringValue;
}

OutputClass::~OutputClass() {
	//std::vector<double> value;
	//std::vector<int> dimensions;
	//std::vector<std::string> stringValue;
};

Component::~Component() {
	input.clear();
	output.clear();
	setting.clear();
	memory.clear();
	literal.clear();
	element.clear();
	initialMemoryX.clear();
	initialMemoryY.clear();
}

Component::Component(Component* src) {

	x = src->x;
	y = src->y;
	width = src->width;
	height = src->height;
	ghost = src->ghost; 

	type = src->type;
	label = src->label;
	userLabel = src->userLabel;

	selected = src->selected;
	displayMode = src->displayMode; 
	shapePath = src->shapePath;

	timeBased = src->timeBased;

	leftTerminalStep = src->leftTerminalStep;
	leftTerminalCount = src->leftTerminalCount;
	for (int i = 0; i < leftTerminalCount; i++) //  MAYBE i dont need this
	{
		InputClass* temp = new InputClass(); 
		temp->value.push_back(0.0);
		temp->dimensions.push_back(0); 
		temp->drivenByComponent = -1;
		temp->drivenByOutputTerminal = -1;
		input.push_back(*temp);
		delete temp;
	}

	// right terminals are outputs
	rightTerminalStep = src->rightTerminalStep;
	rightTerminalCount = src->rightTerminalCount;
	for (int i = 0; i < rightTerminalCount; i++) {  //  MAYBE i dont need this
		OutputClass* temp = new OutputClass(); 
		temp->value.push_back(0.0);
		temp->dimensions.push_back(0); 
		output.push_back(*temp);
		delete temp;
	}
	pinMaskIndex = src->pinMaskIndex;
}

/*
void DEEP_COPY(Component* src, Component* dst) {

	
	
	dst->x = src->x;
	dst->y = src->y;
	dst->width = src->width;
	dst->height = src->height;
	dst->ghost = src->ghost;

	dst->type = src->type;
	dst->label = src->label;
	dst->userLabel = src->userLabel;

	dst->selected = src->selected;
	dst->displayMode = src->displayMode;
	dst->shapePath = src->shapePath;

	dst->timeBased = src->timeBased;

	//dst->leftTerminalStep = src->leftTerminalStep;
	//dst->leftTerminalCount = src->leftTerminalCount;
	dst->input.clear();
	for (int i = 0; i < dst->leftTerminalCount; i++) 
	{
		InputClass* temp = new InputClass();
		temp->value.push_back(0.0);
		temp->dimensions.push_back(0);
		temp->drivenByComponent = -1;
		temp->drivenByOutputTerminal = -1;
		dst->input.push_back(*temp);
		delete temp;
	}

	// right terminals are outputs
	//dst->rightTerminalStep = src->rightTerminalStep;
	//dst->rightTerminalCount = src->rightTerminalCount;
	dst->output.clear();
	for (int i = 0; i < dst->rightTerminalCount; i++) {
		OutputClass* temp = new OutputClass();
		temp->value.push_back(0.0);
		temp->dimensions.push_back(0);
		dst->output.push_back(*temp);
		delete temp;
	}
	dst->pinMaskIndex = src->pinMaskIndex;
}
*/

void Component::setPosition(int xin, int yin) {
	x = xin;
	y = yin;
}
void Component::shiftPosition(int dx, int dy) {
	x += dx;
	y += dy;
}
// allow terminal Labels
//this.setLeftLabels = setLeftLabels;
//this.setRightLabels = setRightLabels;

void Component::setLeftLabels(int count, ...) {
	va_list ap;
	va_start(ap, count);
	if (count < 0) {
		for (int i = 0; i < leftTerminalCount; i++)
			input[i].label = va_arg(ap, string);
	}
	else {
		for (int i = 0; i < leftTerminalCount; i++) {
			char *temp = va_arg(ap, char*);
			input[i].label = "";
			for (int j = 0; j < strlen(temp); j++)
				input[i].label += temp[j];
		}
	}
	va_end(ap);
}

void Component::setRightLabels(int count, ...) {
	va_list ap;
	va_start(ap, count);
	if (count < 0) {
		for (int i = 0; i < rightTerminalCount; i++)
			output[i].label = va_arg(ap, string);
	}
	else
	{
		for (int i = 0; i < rightTerminalCount; i++) {
			char *temp = va_arg(ap, char*);
			output[i].label = "";
			for (int j = 0; j < strlen(temp); j++)
				output[i].label += temp[j];
		}
	}
	va_end(ap);
}

vector<int> Component::GetPinGroups() { vector<int> asd; return asd; }
vector<string> Component::GetPinLabels() { vector<string> asd; return asd; }


void Component::buildPinLabels() {
	if (pinMaskIndex == -1) {
		MessageBox(NULL, L"pinMaskIndex is -1", L"buildPinLabels", MB_OK);
		return;
	}
	int pinMask = (int)setting[pinMaskIndex];
	input.clear();
	vector<int> localPinGroups = GetPinGroups();
	vector<string> localPinLabels = GetPinLabels();
	int groups = localPinGroups.size();
	int bitMask = 1;
	int k = 0;
	for (int i = 0; i < groups; i++) {
		if (bitMask & pinMask) {
			// this bit is set, build some inputs
			for (int j = 0; j < localPinGroups[i]; j++) {
				InputClass* temp = new InputClass(); // each pin is an input class
				temp->value.push_back(0.0);
				temp->dimensions.push_back(0);
				temp->drivenByComponent = -1;
				temp->drivenByOutputTerminal = -1;
				temp->label = localPinLabels[k++];
				input.push_back(*temp);
				delete temp;
			}
		}
		else { // bit is not set, skip these labels
			k += localPinGroups[i];
		}
		bitMask <<= 1;
	}
	leftTerminalCount = input.size();
	int pins = max(leftTerminalCount, rightTerminalCount);
	if (pins < 1)
		pins = 1;
	height = pinToHeight(pins);
	setComponentWidth();
}

int Component::inputsToLoopsN(int n) {
	int loops = 0;
	if (n > input.size())
		n = input.size();
	for (int j = 0; j < n; j++) {
		if (!input[j].stringType) {
			if (input[j].value.size() != 1) {
				if (loops == 0)
					loops = input[j].value.size();
				else if (input[j].value.size() != loops)
					return 0; // no match
			}
		}
		else {
			if (input[j].stringValue.size() != 1) {
				if (loops == 0)
					loops = input[j].stringValue.size();
				else if (input[j].stringValue.size() != loops)
					return 0; // no match
			}
		}
	}
	if (loops == 0)
		loops = 1;
	return loops;
}

int Component::inputsToLoops() {
	int loops = 0;
	for (int j = 0; j < input.size(); j++) {
		if (!input[j].stringType) {
			if (input[j].value.size() != 1) {
				if (loops == 0)
					loops = input[j].value.size();
				else if (input[j].value.size() != loops)
					return 0; // no match
			}
		}
		else {
			if (input[j].stringValue.size() != 1) {
				if (loops == 0)
					loops = input[j].stringValue.size();
				else if (input[j].stringValue.size() != loops)
					return 0; // no match
			}
		}
	}
	if (loops == 0)
		loops = 1;
	return loops;
}

int Component::inputsToLoopsSN(int start, int n) {
	int loops = 0;
	for (int j = 0; j < n; j++) {
		if (input[start + j].value.size() != 1) {
			if (loops == 0)
				loops = input[start + j].value.size();
			else if (input[start + j].value.size() != loops)
				return 0; // no match
		}
	}
	if (loops == 0)
		loops = 1;
	return loops;
}

double Component::getLoopingValueD(int pin, int loop, double defValue) {
	if (pin >= input.size())
		return defValue;
	if (input[pin].stringType)
		return defValue;
	if (input[pin].drivenByComponent == -1)
		return defValue;
	if (input[pin].value.size() == 0)
		return defValue;
	if (input[pin].value.size() == 1)
		return input[pin].value[0];
	if (input[pin].value.size() <= loop)
		return input[pin].value[0];
	return input[pin].value[loop];
}
double Component::getLoopingValueD(int group, int *pin, int loop, double defValue) {
	if (pinMaskIndex == -1) {
		MessageBox(NULL, L"pinMaskIndex is -1", L"getLoopingValueD", MB_OK);
		return 0;
	}
	if ((int)setting[pinMaskIndex] & bitSetArray[group]) {
		int localPin = (*pin)++; // so we can use it and return with it incremented
		if (localPin >= input.size())
			return defValue;
		if (input[localPin].stringType)
			return defValue;
		if (input[localPin].drivenByComponent == -1)
			return defValue;
		if (input[localPin].value.size() == 0)
			return defValue;
		if (input[localPin].value.size() == 1)
			return input[localPin].value[0];
		if (input[localPin].value.size() <= loop)
			return input[localPin].value[0];
		return input[localPin].value[loop];
	}
	return defValue;
}

int Component::getLoopingValueIntPositiveLimit(int pin, int loop, int defValue, int limit) {
	
	if (input[pin].stringType)
		return defValue;
	if (input[pin].drivenByComponent == -1)
		return defValue;
	if (input[pin].value.size() == 0)
		return defValue;
	if (input[pin].value.size() == 1) {
		if (input[pin].value[0] < 0)
			return 0;
		if (input[pin].value[0] > limit)
			return limit;
		return input[pin].value[0];
	}
	if (input[pin].value[loop] < 0)
		return 0;
	if (input[pin].value[loop] > limit)
		return limit;
	return input[pin].value[loop];
}
int Component::getLoopingValueIntPositiveLimit(int group, int* pin, int loop, int defValue, int limit) {
	if (pinMaskIndex == -1) {
		MessageBox(NULL, L"pinMaskIndex is -1", L"getLoopingValueIntPositiveLimit", MB_OK);
		return 0;
	}
	if ((int)setting[pinMaskIndex] & bitSetArray[group]) {
		int localPin = (*pin)++; // so we can use it and return with it incremented
		if (input[localPin].stringType)
			return defValue;
		if (input[localPin].drivenByComponent == -1)
			return defValue;
		if (input[localPin].value.size() == 0)
			return defValue;
		if (input[localPin].value.size() == 1) {
			if (input[localPin].value[0] < 0)
				return 0;
			if (input[localPin].value[0] > limit)
				return limit;
			return input[localPin].value[0];
		}
		if (input[localPin].value.size() <= loop)
			return input[localPin].value[0];
		if (input[localPin].value[loop] < 0)
			return 0;
		if (input[localPin].value[loop] > limit)
			return limit;
		return input[localPin].value[loop];
	}
	return defValue;
}



int Component::getLoopingValueColor(int group, int* pin, int loop) {
	if (pinMaskIndex == -1) {
		MessageBox(NULL, L"pinMaskIndex is -1", L"getLoopingValueColor", MB_OK);
		return 0;
	}

	if ((int)setting[pinMaskIndex] & bitSetArray[group]) {
		int localPin = (*pin)++; // so we can use it and return with it incremented
		if (input[localPin].stringType)
			return 0;
		if (input[localPin].drivenByComponent == -1)
			return 0;
		if (input[localPin].value.size() == 0)
			return 0;
		if (input[localPin].value.size() == 1) {
			if (input[localPin].value[0] < 0)
				return 0;
			if (input[localPin].value[0] > 255)
				return 255;
			return input[localPin].value[0];
		}
		if (input[localPin].value.size() <= loop)
			return input[localPin].value[0];
		if (input[localPin].value[loop] < 0)
			return 0;
		if (input[localPin].value[loop] > 255)
			return 255;
		return input[localPin].value[loop];
	}
	return 0;
}


int Component::getLoopingValueColorSplit(int group, int* pin, int loop) {
	if (pinMaskIndex == -1) {
		MessageBox(NULL, L"pinMaskIndex is -1", L"getLoopingValueColor", MB_OK);
		return 0;
	}
	// if it is no pin, set it to 255, if there is a pin, set it to zero
	if ((int)setting[pinMaskIndex] & bitSetArray[group]) {
		int localPin = (*pin)++; // so we can use it and return with it incremented
		if (input[localPin].stringType)
			return 0;
		if (input[localPin].drivenByComponent == -1)
			return 0;
		if (input[localPin].value.size() == 0)
			return 0;
		if (input[localPin].value.size() == 1) {
			if (input[localPin].value[0] < 0)
				return 0;
			if (input[localPin].value[0] > 255)
				return 255;
			return input[localPin].value[0];
		}
		if (input[localPin].value[loop] < 0)
			return 0;
		if (input[localPin].value[loop] > 255)
			return 255;
		return input[localPin].value[loop];
	}
	return 255;
}




std::string Component::getLoopingStringValueD(int pin, int loop, std::string defValue) {
	if (!input[pin].stringType)
		return defValue;
	if (input[pin].drivenByComponent == -1)
		return defValue;
	if (input[pin].stringValue.size() == 0)
		return defValue;
	if (input[pin].stringValue.size() == 1)
		return input[pin].stringValue[0];
	return input[pin].stringValue[loop];
}

double Component::rotateInputAngle(double inAngle, bool inNotEquals, bool inPlus) {
	if (rotation & 8)
		inAngle *= DEGREES_TO_RADIANS;

	if (inNotEquals) {
		if ((rotation & 4) != 0) // clockwise/counterclockwise & 4
			inAngle = -inAngle;
	}
	else {
		if ((rotation & 4) == 0) // clockwise/counterclockwise & 4
			inAngle = -inAngle;
	}

	if (inPlus) {
		if (rotation & 3)
			inAngle += PI_OVER_2 * (rotation & 3); // bottom 2 bits: 0=north, 1=east, 2=south, 3=west
	}
	else {
		if (rotation & 3)
			inAngle -= PI_OVER_2 * (rotation & 3); // bottom 2 bits: 0=north, 1=east, 2=south, 3=west
	}
	return inAngle;
}

double Component::rotateOutputAngle(double outAngle, bool inNotEquals, bool inPlus) {

	if (inNotEquals) {
		if ((rotation & 4) != 0) // clockwise/counterclockwise & 4
			outAngle = -outAngle;
	}
	else {
		if ((rotation & 4) == 0) // clockwise/counterclockwise & 4
			outAngle = -outAngle;
	}

	if (inPlus) {
		if (rotation & 3)
			outAngle += PI_OVER_2 * (rotation & 3); // bottom 2 bits: 0=north, 1=east, 2=south, 3=west
	}
	else {
		if (rotation & 3)
			outAngle -= PI_OVER_2 * (rotation & 3); // bottom 2 bits: 0=north, 1=east, 2=south, 3=west
	}
	if (rotation & 8)
		outAngle *= RADIANS_TO_DEGREES;
	return outAngle;
}
void Component::setComponentWidth() {

	// need to calculate the width from the input and output pin labels
	int avarageCharacterWidth = 4;
	int characters;
	int maxLeftLabelCharacters = 0;
	for (int pin = 0; pin < leftTerminalCount; pin++) {
		if (input[pin].label.size()) {
			characters = input[pin].label.size();
			if (characters > maxLeftLabelCharacters)
				maxLeftLabelCharacters = characters;
		}
	}
	int maxRightLabelCharacters = 0;
	for (int pin = 0; pin < rightTerminalCount; pin++) {
		if (output[pin].label.size()) {
			characters = output[pin].label.size();
			if (characters > maxRightLabelCharacters)
				maxRightLabelCharacters = characters;
		}
	}
	int compWidth = floor((maxLeftLabelCharacters + maxRightLabelCharacters) * 3.5);
	if (compWidth < 30)
		compWidth = 30;
	width = floor(compWidth);
}

void Component::setSimpleOutputDimensions() {
	for (int pin = 0; pin < output.size(); pin++) {
		setOutputDimension(pin, 0, output[pin].value.size());
	}
}

void Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {}

bool Component::clearOutputs() {
	bool changed = false;
	for (int j = 0; j < output.size(); j++) { // for each output pin
		bool reset = false;
		if (output[j].stringType) {
			if (output[j].stringValue.size() != 1) // if its not a scalar
				reset = true;
			else if (output[j].stringValue[0] != "") //if its not empty
				reset = true;
		}
		else
		{
			if (output[j].value.size() != 1)  // if its not a scalar
				reset = true;
			else if (output[j].value[0] != 0)  //if its not zero
				reset = true;
		}
		if (reset) {
			output[j].value.resize(1);
			output[j].value[0] = 0;
			output[j].stringValue.resize(1);
			output[j].stringValue[0] = "";
			output[j].dimensions.resize(1);
			output[j].dimensions[0] = 0;
			changed = true;
		}
	}
	return changed;
}
bool Component::checkStringInputs() {
	for (int j = 0; j < input.size(); j++) {
		if (input[j].stringType) {
			if (input[j].stringValue.size() == 0)
				return false;
		}
		else
			return false;
	}
	return true;
}
bool Component::checkValueInputs() {
	for (int j = 0; j < input.size(); j++) {
		if (!input[j].stringType) {
			if (input[j].value.size() == 0)
				return false;
		}
		else
			return false;
	}
	return true;
}
bool Component::checkMatrixInput(int pin) {
	if (pin < 0)
		return false;
	if (pin >= input.size())
		return false;
	if (input[pin].drivenByComponent == -1)
		return true;
	if ((input[pin].value.size() == 1)|| (input[pin].value.size() == 16))
		return true;
	return false;
}
bool Component::checkValueAndStringInputs() {
	for (int j = 0; j < input.size(); j++) {
		if (input[j].stringType) {
			if (input[j].stringValue.size() == 0)
				return false;
		}
		else {
			if (input[j].value.size() == 0)
				return false;
		}
	}
	return true;
}

void Component::pushInToOut(int out_pin, int in_pin, int in_index) {
	if (input[in_pin].stringType)
		output[out_pin].stringValue.push_back(input[in_pin].stringValue[in_index]);
	else
		output[out_pin].value.push_back(input[in_pin].value[in_index]);
}
int Component::getInputSize(int in_pin) {
	if (input[in_pin].stringType)
		return input[in_pin].stringValue.size();
	else
		return input[in_pin].value.size();
}

// virtuals
//bool Component::method2(Sheet* pg) { 
//	return false;
//}
int Component::method2(Sheet*) { 
	return 0; 
}
void Component::dial(Sheet*) {}
void Component::onMouseUp(Sheet*) {}
bool Component::crank(Sheet*, int, int, bool) { return false; }
bool Component::onLoad(Sheet*) { return false; }

double Component::innerMethodN(double*) { return 0.0; }
double Component::innerMethodCN(Component, double*) { return 0.0; }
double Component::innerMethodNMO(Sheet*, double*, double*, double) { return 0.0; }

void Component::new3dOjbect() {}
bool Component::crank2d(Sheet*, int, int, bool) { return false; }
void Component::onMouseUp2d(Sheet*) {}
bool Component::dial2d(Sheet*) { return false; }
//bool Component::method2d(Sheet*) { return false; }
bool Component::canvasMouseMoved(Sheet* pg, int x, int y, bool *mouseDowns, int zDelta) { return false; }


bool z(LPCTSTR text1, LPCTSTR text2)
{
	if (wcslen(text1) != wcslen(text2))
		return false;

	for (int j = 0; j < wcslen(text1); j++)
		if (text1[j] != text2[j])
			return false;

	return true;
}

Component* buildComponent(Sheet* pg, LPCTSTR name, int sheetNum) {
	Component* nC = NULL;
	//setVisibleScreenDirty("Added a Component");// pg->isDirty = true;
	switch (name[0] ){
		case '2':
			if (z(name, L"2dSldVZ"))		{ return new Canvas_VerticalSlider_Component(); }
			if (z(name, L"2dSldHZ"))		{ return new Canvas_HorizontalSlider_Component(); }
			if (z(name, L"2dSldAZ"))		{ return new Canvas_AngledSlider_Component(); }
			if (z(name, L"2dArcSldZ"))		{ return new Canvas_ArcSlider_Component(); }
			if (z(name, L"2dKnobZ"))		{ return new Canvas_Knob_Component(); }
			if (z(name, L"2dLinSld"))		{ return new Canvas_LinearSlider_Component(); }
			if (z(name, L"2dSinCosZ"))		{ return new Canvas_SinCosKnob_Component(); }
			if (z(name, L"2dVector"))		{ return new Canvas_Vector_Component(); }
			if (z(name, L"2dButton"))		{ return new Canvas_RoundButton_Component(); }
			if (z(name, L"2dButton2"))		{ return new Canvas_RoundedRectangleButton_Component(); }
			if (z(name, L"2dText"))			{ return new Canvas_TextBlock_Component(); }
			if (z(name, L"2dRADIO"))		{ return new Canvas_RadioButton_Component(); }
			if (z(name, L"2dCHECK"))		{ return new Canvas_Checkbox_Component(); }
			if (z(name, L"2dTxSpin"))		{ return new Canvas_TextSpinner_Component(); }
			if (z(name, L"2dNumSpin"))		{ return new Canvas_NumberSpinner_Component(); }
			if (z(name, L"2dSpin"))			{ return new Canvas_Spinner_Component(); }
			if (z(name, L"2dGeom"))			{ return new Canvas_Geometry_Component(); }
			if (z(name, L"2dArrSld"))		{ return new Canvas_ArraySlider_Component(); }
			if (z(name, L"2dLength"))		{ return new Canvas_Length_Component(); }
			if (z(name, L"2dScale"))		{ return new Canvas_Scale_Component(); }
			if (z(name, L"2dDelta"))		{ return new Canvas_Delta_Component(); }
			if (z(name, L"2dDisp"))			{ return new Canvas_ViewControl_Component(); }
			if (z(name, L"2dMeter"))		{ return new Canvas_Meter_Component(); }
			if (z(name, L"2dChart"))		{ return new Canvas_Chart_Component(); }
			if (z(name, L"2dChart2"))		{ return new Canvas_XyChart_Component(); }
			if (z(name, L"2dField"))		{ return new Canvas_FieldPlot_Component(); }
			if (z(name, L"2dPosTester"))	{ return new Tester_2dPosition_Component(); }
			if (z(name, L"2dColorTester"))	{ return new Tester_2dColor_Component(); }
			if (z(name, L"2PhaseTester"))	{ return new Tester_2Phase_Component(); }
			if (z(name, L"2dBitmap"))		{ return new Canvas_Bitmap_Component(); }
			break; // 26
		case '3':
			if (z(name, L"3Dist"))			{ return new ThreeDDist_Component(); }
			if (z(name, L"3TORUS"))			{ return new Torus_Component(); }
			if (z(name, L"3SPHERE"))		{ return new Sphere_Component(); }
			if (z(name, L"3CONE"))			{ return new Cone_Component(); }
			if (z(name, L"3CUBE"))			{ return new Cube3D_Component(); }
			if (z(name, L"3CYLIND"))		{ return new Cylinder_Component(); }
		//	if (z(name, L"3Tetra"))			{ return new Tetra_Component(); }
		//	if (z(name, L"3Octa"))			{ return new Octahedron_Component(); }
		//	if (z(name, L"3Dodeca"))		{ return new Dodecahedron_Component(); }
		//	if (z(name, L"3Icosa"))			{ return new Icosahedron_Component(); }
			if (z(name, L"3XYZROT"))		{ return new Rotate3D_Component(); }
			if (z(name, L"3ROTX"))			{ return new RotateX_Component(); }
			if (z(name, L"3ROTY"))			{ return new RotateY_Component(); }
			if (z(name, L"3ROTZ"))			{ return new RotateZ_Component(); }
			if (z(name, L"3XYZSCALE"))		{ return new Scale3D_Component(); }
			if (z(name, L"3XYZSHIFT"))		{ return new Shift3D_Component(); }
			if (z(name, L"3Line"))			{ return new PolyLine3D_Component(); }
			if (z(name, L"3Lines"))			{ return new Lines3D_Component(); }
			if (z(name, L"3Polygon"))		{ return new Polygon3D_Component(); }
			if (z(name, L"3Circle"))		{ return new Circle3D_Component(); }
			if (z(name, L"3Grid"))			{ return new Grid3D_Component(); }
			if (z(name, L"3Axis"))			{ return new Axis3D_Component(); }
			if (z(name, L"3Arrows"))		{ return new Arrow3D_Component(); }
			if (z(name, L"3Lathe"))			{ return new Lathe3D_Component(); }
			if (z(name, L"3Extrude"))		{ return new Extrude3D_Component(); }
			if (z(name, L"3PlSurf"))		{ return new PlaneSurface3D_Component(); }
			if (z(name, L"3PlSurf2"))		{ return new PlaneSurface3D2_Component(); }
			if (z(name, L"3SphSurf"))		{ return new SphereSurface3D_Component(); }
			if (z(name, L"3SphSurf2"))		{ return new SphereSurface3D2_Component(); }
			if (z(name, L"3CylSurf"))		{ return new CylinderSurface3D_Component(); }
			if (z(name, L"3CylSurf2"))		{ return new CylinderSurface3D2_Component(); }
			if (z(name, L"3Surf"))			{ return new Surface3D_Component(); }
			//if (z(name, L"3View"))			{ return new View3D_Component(); }
			if (z(name, L"3Light"))			{ return new Light3D_Component(pg); }
			if (z(name, L"3UNROT"))			{ return new GetDirection3D_Component(pg); }
			if (z(name, L"3BRIDGE"))		{ return new Bridge3D_Component(pg); }
			if (z(name, L"3dDisp"))			{ return new ViewControl3D_Component(); }
			if (z(name, L"3dPosTester"))	{ return new Tester_3dPosition_Component(); }
			if (z(name, L"3PhaseTester"))	{ return new Tester_3Phase_Component(); }
			break;// 26+36=62
		case 'A':
			if (z(name, L"Add"))			{ return new Add_Component(); }
			if (z(name, L"Asin"))			{ return new ArcSin_Component(pg); }
			if (z(name, L"Acos"))			{ return new ArcCos_Component(pg); }
			if (z(name, L"Atan"))			{ return new ArcTan_Component(pg); }
			if (z(name, L"Atan2"))			{ return new ArcTan2_Component(pg); }
			if (z(name, L"Ave2"))			{ return new Ave2_Component(); }
			if (z(name, L"Abs"))			{ return new Abs_Component(); }
			if (z(name, L"And"))			{ return new And_Component(); }
			if (z(name, L"Alt"))			{ return new Alternate_Component(); }
			if (z(name, L"AnyEdge"))		{ return new AnyEdge_Component(); }
			if (z(name, L"AddFrac"))		{ return new AddFrac_Component(); }
			if (z(name, L"ARROW"))			{ return new Canvas_ARROW_Component(); }
			if (z(name, L"Ave"))			{ return new Ave_Component(); }
			if (z(name, L"ArrayRand"))		{ return new ArrayRand_Component(); }
			if (z(name, L"ASort"))			{ return new AlphaSort_Component(); }
			if (z(name, L"ARank"))			{ return new AlphaRank_Component(); }
			if (z(name, L"Arrange"))		{ return new Arrange_Component(); }
			//if (z(name, L"AudSine"))		{ return new AudSine_Component(); }
			//if (z(name, L"AudSquare"))	{ return new AudSquare_Component(); }
			//if (z(name, L"AudSawtooth"))	{ return new AudSawtooth_Component(); }
			//if (z(name, L"AudTriangle"))	{ return new AudTriangle_Component(); }
			//if (z(name, L"AudWave"))		{ return new AudWave_Component(); }
			if (z(name, L"Arr2Sound"))		{ return new ArrayToSound_Component(); }
			
			break; // 62+17=79
		case 'B':
			if (z(name, L"BitAnd"))			{ return new BitAnd_Component(); }
			if (z(name, L"BitOr"))			{ return new BitOr_Component(); }
			if (z(name, L"BitXor"))			{ return new BitXor_Component(); }
			if (z(name, L"BitNot"))			{ return new BitNot_Component(); }
			if (z(name, L"Buffer"))			{ return new Buffer_Component(); }
			if (z(name, L"Button"))			{ return new Button_Component(); }
			if (z(name, L"BOX"))			{ return new Canvas_BOX_Component(); }
			if (z(name, L"BitJoin"))		{ return new BitJoin_Component(); }
			if (z(name, L"BitSplit"))		{ return new BitSplit_Component(); }
			if (z(name, L"BitToN"))			{ return new BitsToNumber_Component(); }
			if (z(name, L"Bezier"))			{ return new Bezier_Component(); }
			if (z(name, L"BezDeriv"))		{ return new BezierDeriv_Component(); }
			if (z(name, L"BussIn"))			{ return new BusIn_Component(); }
			if (z(name, L"BussOut"))		{ return new BusOut_Component(pg->lastBussName); }
			if (z(name, L"Buss2In"))		{ return new Bus2In_Component(); }
			if (z(name, L"Buss2Out"))		{ return new Bus2Out_Component(); }
			if (z(name, L"Beep"))			{ return new Beep_Component(); }
			if (z(name, L"BitTester"))		{ return new Tester_Bit_Component(); }
			break; // 79+17=96 
		case 'C':
			if (z(name, L"Ceil"))			{ return new Ceiling_Component(); }
			if (z(name, L"Cos"))			{ return new Cosine_Component(pg); }
			if (z(name, L"Cube"))			{ return new Cube_Component(); }
			if (z(name, L"Con"))			{ return new Constant_Component(); }
			if (z(name, L"ConFac"))			{ return new ConstantFactor_Component(); }
			if (z(name, L"ConOff"))			{ return new ConstantOffset_Component(); }
			if (z(name, L"Chart"))			{ return new Chart_Component(); }
			if (z(name, L"Chart2"))			{ return new XyChart_Component(); }
			if (z(name, L"ClrPatch"))		{ return new ColorPatch_Component(); }
			if (z(name, L"CIRCLE"))			{ return new Canvas_CIRCLE_Component(); }
			if (z(name, L"Cc3pts"))			{ return new CircleCenter_Component(); }
			if (z(name, L"Center2Edge"))	{ return new Center2Edge_Component(); }
			if (z(name, L"CROSS"))			{ return new Canvas_CROSS_Component(); }
			if (z(name, L"Count"))			{ return new Count_Component(); }
			if (z(name, L"Corr"))			{ return new Correlation_Component(); }
			if (z(name, L"CrossProd"))		{ return new CrossProduct_Component(); }
			if (z(name, L"CharAt"))			{ return new CharAt_Component(); }
			if (z(name, L"CharIndex"))		{ return new CharIndex_Component(); }
			if (z(name, L"Counter"))		{ return new Counter_Component(); }
			if (z(name, L"Compare"))		{ return new Compare_Component(); }
			if (z(name, L"Clock"))			{ return new RealTimeClock_Component(); }
			if (z(name, L"ComFactor"))		{ return new CommonFactor_Component(); }
			if (z(name, L"CreateFldr"))		{ return new CreateFldr_Component(); }
			if (z(name, L"Concat"))			{ return new Concatenate_Component(); }
			break; // 96+24=120
		case 'D':
			if (z(name, L"Div"))			{ return new Divide_Component(); }
			if (z(name, L"Day"))			{ return new Day_Component(); }
			if (z(name, L"Deselect"))		{ return new Deselect_Component(); }
			if (z(name, L"Detent"))			{ return new Detent_Component(); }
			if (z(name, L"DtoA"))			{ return new DtoA_Component(); }
			if (z(name, L"DFlip"))			{ return new DFlipFlop_Component(); }
			if (z(name, L"DivFrac"))		{ return new DivideFractions_Component(); }
			if (z(name, L"Debug"))			{ return new Debug_Component(); }
			if (z(name, L"Dis"))			{ return new DisplayList_Component(); }
			if (z(name, L"Dup"))			{ return new Duplicate_Component(); }
			if (z(name, L"Dims"))			{ return new Dimensions_Component(); }
			if (z(name, L"Difs"))			{ return new Differences_Component(); }
			if (z(name, L"Dot2Prod"))		{ return new Dot2Prod_Component(); }
			if (z(name, L"Dot3Prod"))		{ return new Dot3Prod_Component(); }
			if (z(name, L"DataToImage3"))	{ return new DataToImage3_Component(); }
			if (z(name, L"DataToImage1"))	{ return new DataToImage1_Component(); }
			if (z(name, L"DayOfWeek"))		{ return new DayOfWeek_Component(); }
			if (z(name, L"DaysInMon"))		{ return new DaysInMonth_Component(); }
			if (z(name, L"Deal"))			{ return new Deal_Component(); }
			if (z(name, L"DelFile"))		{ return new DeleteFile_Component(); }
			break; // 120+20=140
		case 'E':
			if (z(name, L"Exp"))			{ return new Exp_Component(); }
			if (z(name, L"EQ"))				{ return new EQ_Component(); }
			if (z(name, L"EQZ"))			{ return new EQZ_Component(); }
			if (z(name, L"ELIPS"))			{ return new Canvas_EllipseOnCenter_Component(); }
			if (z(name, L"ELIPS2"))			{ return new Canvas_EllipseOnFocus_Component(); }
			if (z(name, L"Edge2Center"))	{ return new Edge2Center_Component(); }
			if (z(name, L"Expand"))			{ return new Expand_Component(); }
			if (z(name, L"Excise"))			{ return new Excise_Component(); }
			if (z(name, L"Ellipse"))		{ return new EllipsePath_Component(); }
			if (z(name, L"ElimValue"))		{ return new EliminateValue_Component(); }
			break; // 140+10=150
		case 'F':
			if (z(name, L"Floor"))			{ return new Floor_Component(); }
			if (z(name, L"Fade"))			{ return new Fade_Component(); }
			if (z(name, L"Fact"))			{ return new Fact_Component(); }
			if (z(name, L"FDist"))			{ return new Flat2dDist_Component(); }
			if (z(name, L"Flip"))			{ return new Flip_Component(); }
			if (z(name, L"FuncGen"))		{ return new FunctionGenerator_Component(); }
			if (z(name, L"FIFO"))			{ return new FIFO_Component(); }
			if (z(name, L"FirstSet"))		{ return new FirstSet_Component(); }
			if (z(name, L"FSM"))			{ return new FSM_Component(); }
			if (z(name, L"Factor"))			{ return new Factor_Component(); }
			if (z(name, L"FindIndex"))		{ return new FindIndex_Component(); }
			if (z(name, L"FindBool"))		{ return new FindBool_Component(); }
			if (z(name, L"Filter"))			{ return new Filter_Component(); }
			if (z(name, L"Function"))		{ return new Function_Component(); }
			break; // 150+15=165
		case 'G':
			if (z(name, L"GT"))				{ return new GT_Component(); }
			if (z(name, L"GTE"))			{ return new GTE_Component(); }
			if (z(name, L"GTZ"))			{ return new GTZ_Component(); }
			if (z(name, L"GTEZ"))			{ return new GTEZ_Component(); }
			if (z(name, L"Gather"))			{ return new Gather_Component(); }
			if (z(name, L"Geom"))			{ return new Geom_Component(); }
			if (z(name, L"Grid"))			{ return new Grid_Component(); }
			if (z(name, L"GetAt"))			{ return new GetAt_Component(); }
			if (z(name, L"GetUntil"))		{ return new GetUntil_Component(); }
			if (z(name, L"GetFileInfo"))	{ return new GetFileInfo_Component(); }
			if (z(name, L"GetDir"))			{ return new GetDir_Component(); }
			if (z(name, L"GraphBaseWords")) { return new GraphBaseWords_Component(); }
			break; // 165+12=177
		case 'H':
			if (z(name, L"Hour"))			{ return new Hour_Component(); }
			if (z(name, L"Hold"))			{ return new Hold_Component(); }
			if (z(name, L"HAXIS"))			{ return new Canvas_HorizontalAxis_Component(); }
			if (z(name, L"Histo"))			{ return new Histogram_Component(); }
			if (z(name, L"HarmGen"))		{ return new HarmonicGenerator_Component(); }
			if (z(name, L"HarmGen2"))		{ return new HarmonicGenerator2_Component(); }
			break; // 177+4=181
		case 'I':
			if (z(name, L"Inv"))			{ return new Inv_Component(); }
			if (z(name, L"InRange"))		{ return new InRange_Component(); }
			if (z(name, L"InPoly"))			{ return new InPoly_Component(); }
			if (z(name, L"Intersect"))		{ return new IntersectLines_Component(); }
			if (z(name, L"Intersect2"))		{ return new IntersectCircles_Component(); }
			if (z(name, L"Intersect3"))		{ return new IntersectLineCircle_Component(); }
			if (z(name, L"Interp"))			{ return new Interpolate_Component(); }
			if (z(name, L"Inter"))			{ return new Intersection_Component(); }
			if (z(name, L"ImCamera"))		{ return new Image_Camera_Component(); }
			if (z(name, L"ImHStripes"))		{ return new Image_HorizStripes_Component(); }
			if (z(name, L"ImVStripes"))		{ return new Image_VerticalStripes_Component(); }
			if (z(name, L"ImCheckers"))		{ return new Image_Checkers_Component(); }
			if (z(name, L"ImClrBars"))		{ return new Image_ColorBars_Component(); }
			if (z(name, L"ImSolidClr"))		{ return new Image_SolidColor_Component(); }
			if (z(name, L"ImToData3"))		{ return new ImageToData3_Component(); }
			if (z(name, L"ImToData1"))		{ return new ImageToData1_Component(); }
			if (z(name, L"ImCrop"))			{ return new Image_Crop_Component(); }
			if (z(name, L"ImInsert"))		{ return new Image_Insert_Component(); }
			if (z(name, L"ImRotate"))		{ return new Image_Rotate_Component(); }
			if (z(name, L"ImMux"))			{ return new Image_Mux_Component(); }
			if (z(name, L"ImFader"))		{ return new Image_Fader_Component(); }
			if (z(name, L"ImResize"))		{ return new Image_Resize_Component(); }
			if (z(name, L"ImGradient"))		{ return new Image_Gradient_Component(); }
			if (z(name, L"ImGrayMod"))		{ return new Image_GrayModify_Component(); }
			if (z(name, L"ImGrayColor"))	{ return new Image_GrayColorize_Component(); }
			if (z(name, L"ImColorMod"))		{ return new Image_ColorModify_Component(); }
			if (z(name, L"ImColor2Gray"))	{ return new Image_ColorToGrayscale_Component(); }
			if (z(name, L"ImRGB2HSV"))		{ return new Image_RGBtoHSV_Component(); }
			if (z(name, L"ImHSV2RGB"))		{ return new Image_HSVtoRGB_Component(); }
			if (z(name, L"ImChromaKey"))	{ return new Image_ChromaKey_Component(); }
			if (z(name, L"ImGaussBlur"))	{ return new Image_Gaussian_Component(); }
			if (z(name, L"ImMedianBlur"))	{ return new Image_MedianBlur_Component(); }
			if (z(name, L"ImBilatFilt"))	{ return new Image_BilateralFilter_Component(); }
			if (z(name, L"ImEdge"))			{ return new Image_EdgeEnhancer_Component(); }
			if (z(name, L"ImThresh"))		{ return new Image_Threshold_Component(); }
			if (z(name, L"ImErode"))		{ return new Image_Erode_Component(); }
			if (z(name, L"ImDilate"))		{ return new Image_Dilate_Component(); }
			if (z(name, L"ImBrightness"))	{ return new Image_Brightness_Component(); }
			if (z(name, L"ImContrast"))		{ return new Image_Contrast_Component(); }
			if (z(name, L"ImLine"))			{ return new Image_Line_Component(); }
			if (z(name, L"ImBox"))			{ return new Image_Box_Component(); }
			if (z(name, L"ImCircle"))		{ return new Image_Circle_Component(); }
			if (z(name, L"ImCross"))		{ return new Image_Cross_Component(); }
			if (z(name, L"ImEllipse"))		{ return new Image_Ellipse_Component(); }
			if (z(name, L"ImArrow"))		{ return new Image_Arrow_Component(); }
			if (z(name, L"ImPolygon"))		{ return new Image_Polygon_Component(); }
			if (z(name, L"ImText"))			{ return new Image_Text_Component(); }
			if (z(name, L"ImSegment"))		{ return new Image_Segment_Component(); }
			if (z(name, L"ImArea"))			{ return new Image_CalculateArea_Component(); }
			if (z(name, L"ImCent"))			{ return new Image_CalculateCentroid_Component(); }
			if (z(name, L"ImBounds"))		{ return new Image_CalculateBounds_Component(); }
			if (z(name, L"ImAngle"))		{ return new Image_CalculateAngle_Component(); }
			if (z(name, L"ImDisplay"))		{ return new Image_Display_Component(); }
			if (z(name, L"ImRead"))			{ return new Image_ReadFile_Component(); }
			if (z(name, L"ImWrite"))		{ return new Image_WriteFile_Component(); }
			if (z(name, L"Integ"))			{ return new Integrator_Component(); }
			if (z(name, L"IToPos"))			{ return new IndexToPosition_Component(); }
			if (z(name, L"ImFace"))			{ return new Image_FaceDetector_Component(); }
			if (z(name, L"Input"))			{ return new Input_Component(pg); }
			if (z(name, L"ImClrIndx"))		{ return new Image_GenerateColorIndex_Component(); }
			if (z(name, L"InstHarm"))		{ return new InstrumentHarmonics_Component(); }
			if (z(name, L"InJoin"))			{ return new InnerJoin_Component(); }
			break; // 181+60=241
		case 'J':
			if (z(name, L"Join"))			{ return new Join_Component(); }
			break; // 241+1=242
		case 'K':
			if (z(name, L"Knob"))			{ return new Knob_Component(); }
			if (z(name, L"KnobFac"))		{ return new KnobFactor_Component(); }
			if (z(name, L"KnobOff"))		{ return new KnobOffset_Component(); }
			if (z(name, L"Keep"))			{ return new Keep_Component(); }
			if (z(name, L"KeepFirst"))		{ return new KeepFirst_Component(); }
			if (z(name, L"KeepLast"))		{ return new KeepLast_Component(); }
			break; // 242+6=248
		case 'L':
			if (z(name, L"Log"))			{ return new Log_Component(); }
			if (z(name, L"LT"))				{ return new LessThan_Component(); }
			if (z(name, L"LTE"))			{ return new LessThanOrEqual_Component(); }
			if (z(name, L"LTZ"))			{ return new LessThanZero_Component(); }
			if (z(name, L"LTEZ"))			{ return new LTEZ_Component(); }
			if (z(name, L"Limit"))			{ return new Limit_Component(); }
			if (z(name, L"Logic"))			{ return new Logic_Component(); }
			if (z(name, L"LED"))			{ return new LED_Component(); }
			if (z(name, L"LINES"))			{ return new Canvas_LINES_Component(); }
			if (z(name, L"LINE"))			{ return new Canvas_LINE_Component(); }
			if (z(name, L"LlToXyz"))		{ return new LlToXyz_Component(); }
			if (z(name, L"LastSet"))		{ return new LastSet_Component(); }
			if (z(name, L"LinReg"))			{ return new LinearRegression_Component(); }
			if (z(name, L"LstSqr"))			{ return new LeastSquares_Component(); }
			if (z(name, L"LstSqrN"))		{ return new LeastSquaresN_Component(); }
			if (z(name, L"Large"))			{ return new Large_Component(); }
			if (z(name, L"Loan"))			{ return new Loan_Component(); }
			if (z(name, L"Lit"))			{ return new Literal_Component(); }
			if (z(name, L"LoCase"))			{ return new LoCase_Component(); }
			if (z(name, L"LogScale"))		{ return new LogScale_Component(); }
			if (z(name, L"LIFO")) {
				Component *temp = new FIFO_Component();
				temp->type = L"LIFO";
				temp->label = L"LIFO";
				return temp;
			} // 248+21=269
			break;
		case 'M':
			if (z(name, L"Mul"))			{ return new Mul_Component(); }
			if (z(name, L"Min2"))			{ return new Min2_Component(); }
			if (z(name, L"Max2"))			{ return new Max2_Component(); }
			if (z(name, L"Mod"))			{ return new Mod_Component(); }
			if (z(name, L"Month"))			{ return new Month_Component(); }
			if (z(name, L"Minute"))			{ return new Minute_Component(); }
			if (z(name, L"Matrix"))			{ return new Matrix_Component(); }
			if (z(name, L"Mux"))			{ return new Multiplexer_Component(); }
			if (z(name, L"Mem"))			{ return new Mem_Component(); }
			if (z(name, L"MulFrac"))		{ return new MulFrac_Component(); }
			if (z(name, L"Mouse"))			{ return new Canvas_Mouse_Component(); }
			if (z(name, L"Meter"))			{ return new Meter_Component(); }
			if (z(name, L"MARKER"))			{ return new Canvas_MARKER_Component(); }
			if (z(name, L"MJoin"))			{ return new MatrixJoin_Component(); }
			if (z(name, L"MSplit"))			{ return new MatrixSplit_Component(); }
			if (z(name, L"Mirror"))			{ return new Mirror_Component(); }
			if (z(name, L"Median"))			{ return new Median_Component(); }
			if (z(name, L"Mode"))			{ return new Mode_Component(); }
			if (z(name, L"MatMul"))			{ return new MatMul_Component(); }
			if (z(name, L"MatTran"))		{ return new MatrixTranspose_Component(); }
			if (z(name, L"MatDet"))			{ return new MatrixDeterminent_Component(); }
			if (z(name, L"Min"))			{ return new Min_Component(); }
			if (z(name, L"Max"))			{ return new Max_Component(); }
			if (z(name, L"MaxIdx"))			{ return new MaxIndex_Component(); }
			if (z(name, L"MinIdx"))			{ return new MinIndex_Component(); }
			if (z(name, L"MoonPhase"))		{ return new MoonPhase_Component(); }
			if (z(name, L"MatGetAt"))		{ return new MatrixGetAt_Component(); }
			if (z(name, L"Merge"))			{ return new Merge_Component(); }
			if (z(name, L"MatSubset"))		{ return new MatrixSubset_Component(); }
			if (z(name, L"MatExcise"))		{ return new MatrixExcise_Component(); }
			if (z(name, L"MatInv"))			{ return new MatrixInversion_Component(); }
			if (z(name, L"MatInterp"))		{ return new MatrixInterpolate_Component(); }
			if (z(name, L"Morse"))			{ return new MorseCode_Component(); }
			if (z(name, L"MidiPitch"))		{ return new MIDI_Pitch_Component(); }
			if (z(name, L"MidiPlay"))		{ return new MidiPlay_Component(); }
			if (z(name, L"MidiDrum"))		{ return new MidiPercussion_Component(); }
			break; // 269+32=301
		case 'N':
			if (z(name, L"Neg"))			{ return new Neg_Component(); }
			if (z(name, L"NEQ"))			{ return new NEQ_Component(); }
			if (z(name, L"NEQZ"))			{ return new NEQZ_Component(); }
			if (z(name, L"Nand"))			{ return new Nand_Component(); }
			if (z(name, L"Nor"))			{ return new Nor_Component(); }
			if (z(name, L"Not"))			{ return new Not_Component(); }
			if (z(name, L"NToBit"))			{ return new NToBit_Component(); }
			if (z(name, L"NSort"))			{ return new NumericalSort_Component(); }
			if (z(name, L"NRank"))			{ return new NumericalRank_Component(); }
			break; // 301+9=310
		case 'O':
			if (z(name, L"Or"))				{ return new Or_Component(); }
			if (z(name, L"OneEdge"))		{ return new OneEdge_Component(); }	// aka Radio
			if (z(name, L"OuterSum"))		{ return new OuterSum_Component(); }
			if (z(name, L"OuterProd"))		{ return new OuterProd_Component(); }
			if (z(name, L"OnlyA"))			{ return new OnlyA_Component(); }
			if (z(name, L"OnlyB"))			{ return new OnlyB_Component(); }
			if (z(name, L"OneShot"))		{ return new OneShot_Component(); }
			if (z(name, L"Output"))			{ return new Output_Component(pg); }
			break; // 310+8=318
		case 'P':
			if (z(name, L"Pow"))			{ return new Pow_Component(); }
			if (z(name, L"Precision"))		{ return new Precision_Component(); }
			if (z(name, L"Polynomial"))		{ return new Polynomial_Component(); }
			if (z(name, L"PolyInteg"))		{ return new PolyInteg_Component(); }
			if (z(name, L"PolyDeriv"))		{ return new PolyDeriv_Component(); }
			if (z(name, L"PARAB"))			{ return new Canvas_Parabola_Component(); }
			if (z(name, L"POLY"))			{ return new Canvas_Polygon_Component(); }
			if (z(name, L"PolySynth"))		{ return new PolySynth_Component(); }
			if (z(name, L"ProdA"))			{ return new ProdA_Component(); }
			if (z(name, L"Period"))			{ return new Period_Component(); }
			if (z(name, L"Planet"))			{ return new Planet_Component(); }
			if (z(name, L"Perm"))			{ return new Permute_Component(); }
			if (z(name, L"Perm2"))			{ return new Permute2_Component(); }
			if (z(name, L"Perm3"))			{ return new Permute3_Component(); }
			if (z(name, L"Primes"))			{ return new Primes_Component(); }
			if (z(name, L"Parse"))			{ return new Parse_Component(); }
			if (z(name, L"Pulse"))			{ return new Pulse_Component(); }
			if (z(name, L"PutAt"))			{ return new PutAt_Component(); }
			if (z(name, L"Parabola"))		{ return new ParabolaPath_Component(); }
			if (z(name, L"PlayMidi"))		{ return new PlayMidi_Component(); }
			if (z(name, L"PlySndFile"))		{ return new PlaySoundFile_Component(); }
			if (z(name, L"Point2Line"))		{ return new PointToLine_Component(); }
			break; // 318+22=340
		case 'Q':
			if (z(name, L"Quad"))			{ return new Quad_Component(); }
			break; // 340+1=341
		case 'R':
			if (z(name, L"Round"))			{ return new Round_Component(); }
			if (z(name, L"Reasonable"))		{ return new Reasonable_Component(); }
			if (z(name, L"RandPos"))		{ return new RandPos_Component(); }
			if (z(name, L"Reset"))			{ return new Reset_Component(); }
			if (z(name, L"Running"))		{ return new Running_Component(); }
			if (z(name, L"RackZ"))			{ return new RackZ_Component(); }
			if (z(name, L"RackFacZ"))		{ return new RackFacZ_Component(); }
			if (z(name, L"RackOffZ"))		{ return new RackOffZ_Component(); }
			if (z(name, L"Rec"))			{ return new Recorder_Component(pg->systemTime); }
			if (z(name, L"Rotate"))			{ return new Rotate_Component(); }
			if (z(name, L"Roll"))			{ return new Roll_Component(); }
			if (z(name, L"Repeat"))			{ return new Repeat_Component(); }
			if (z(name, L"Range"))			{ return new Range_Component(); }
			if (z(name, L"Reduce"))			{ return new Reduce_Component(); }
			if (z(name, L"Ravel"))			{ return new Ravel_Component(); }
			if (z(name, L"Replace"))		{ return new Replace_Component(); }
			if (z(name, L"ReadTxFile"))		{ return new ReadTextFile_Component(); }
			if (z(name, L"ReadDatFile"))	{ return new ReadDataFile_Component(); }
			if (z(name, L"RandReadFile"))	{ return new RandReadFile_Component(); }
			if (z(name, L"RandWriteFile"))	{ return new RandWriteFile_Component(); }
			break; // 341+20=361
		case 'S':
			if (z(name, L"Sub"))			{ return new Subtract_Component(); }
			if (z(name, L"Sin"))			{ return new Sine_Component(pg); }
			if (z(name, L"Sign"))			{ return new Sign_Component(); }
			if (z(name, L"Sqrt"))			{ return new SquareRoot_Component(); }
			if (z(name, L"Square"))			{ return new Square_Component(); }
			if (z(name, L"Second"))			{ return new Second_Component(); }
			if (z(name, L"Sld"))			{ return new Sld_Component(); }
			if (z(name, L"SldFac"))			{ return new SldFac_Component(); }
			if (z(name, L"SldOff"))			{ return new SldOff_Component(); }
			if (z(name, L"Sld2"))			{ return new Sld2_Component(); }
			if (z(name, L"Sld2Fac"))		{ return new Sld2Fac_Component(); }
			if (z(name, L"Sld2Off"))		{ return new Sld2Off_Component(); }
			if (z(name, L"Select"))			{ return new Select_Component(); }
			if (z(name, L"Sample"))			{ return new Sample_Component(); }
			if (z(name, L"SDist"))			{ return new SphericalDistance_Component(); }
			if (z(name, L"Slope"))			{ return new Slope_Component(); }
			if (z(name, L"Stop"))			{ return new Stop_Component(); }
			if (z(name, L"Start"))			{ return new Start_Component(); }
			if (z(name, L"Step"))			{ return new Step_Component(); }
			if (z(name, L"SpliceIn"))		{ return new SpliceIn_Component(); }
			if (z(name, L"SpliceOut"))		{ return new SpliceOut_Component(); }
			if (z(name, L"Simp"))			{ return new Simplex_Component(); }
			if (z(name, L"Switch"))			{ return new Switch_Component(); }
			if (z(name, L"SnapPoint2"))		{ return new SnapPoint2_Component(); }
			if (z(name, L"Synth"))			{ return new Synth_Component(); }
			if (z(name, L"Split"))			{ return new Split_Component(); }
			if (z(name, L"SJoin"))			{ return new SJoin_Component(); }
			if (z(name, L"SubSet"))			{ return new SubSet_Component(); }
			if (z(name, L"Sum"))			{ return new Sum_Component(); }
			if (z(name, L"Stdev"))			{ return new Stdev_Component(); }
			if (z(name, L"Sums"))			{ return new Sums_Component(); }
			if (z(name, L"Small"))			{ return new Small_Component(); }
			if (z(name, L"SetDiff"))		{ return new Difference_Component(); }
			if (z(name, L"Savings"))		{ return new Savings_Component(); }
			if (z(name, L"StrLen"))			{ return new StrLen_Component(); }
			if (z(name, L"SubStr"))			{ return new SubStr_Component(); }
			if (z(name, L"Str2dWid"))		{ return new Str2dWid_Component(); }
			if (z(name, L"Sunrise"))		{ return new Sunrise_Component(); }
			if (z(name, L"SunPos"))			{ return new SunPosition_Component(); }
			if (z(name, L"SquareWave"))		{ return new SquareWave_Component(); }
			if (z(name, L"Sawtooth"))		{ return new Sawtooth_Component(); }
			if (z(name, L"SineWave"))		{ return new SineWave_Component(); }
			if (z(name, L"Scale"))			{ return new Scale_Component(); }
			if (z(name, L"Spline"))			{ return new Spline_Component(); }
			if (z(name, L"SplineXY"))		{ return new SplineXY_Component(); }
			if (z(name, L"SCount"))			{ return new ScalarCounter_Component(); }
			if (z(name, L"Shuffle"))		{ return new Shuffle_Component(); }
			if (z(name, L"Sheet"))			{ return new Sheet_Component(sheetNum); }
			if (z(name, L"Sequ"))			{ return new Sequencer_Component(); }
			break; // 361+48=409
		case 'T':
			if (z(name, L"Tan"))			{ return new Tan_Component(pg); }
			if (z(name, L"Time"))			{ return new Time_Component(); }
			if (z(name, L"Table"))			{ return new Table_Component(); }
			if (z(name, L"TimeRand"))		{ return new TimeRand_Component(); }
			if (z(name, L"TrigRand"))		{ return new TrigRand_Component(); }
			if (z(name, L"TimeStep"))		{ return new TimeStep_Component(); }
			if (z(name, L"TrigSum"))		{ return new TriggeredSum_Component(); }
			if (z(name, L"ToPolar"))		{ return new ToPolar_Component(); }
			if (z(name, L"ToCart"))			{ return new ToCart_Component(); }
			if (z(name, L"ToNumber"))		{ return new ToNumber_Component(); }
			if (z(name, L"ToString"))		{ return new ToString_Component(); }
			if (z(name, L"ToASCII"))		{ return new ToASCII_Component(); }
			if (z(name, L"ToChar"))			{ return new ToChar_Component(); }
			if (z(name, L"ToRGB"))			{ return new ToRGB_Component(); }
			if (z(name, L"ToHSV"))			{ return new ToHSV_Component(); }
			if (z(name, L"ToHMS"))			{ return new ToHMS_Component(); }
			if (z(name, L"ToHM"))			{ return new ToHM_Component(); }
			if (z(name, L"TriWave"))		{ return new TriangleWave_Component(); }
			if (z(name, L"Timer"))			{ return new Timer_Component(); }
			if (z(name, L"Timer2"))			{ return new Timer2_Component(); }
			if (z(name, L"TicLab"))			{ return new TicAndLabel_Component(); }
			if (z(name, L"TriArea"))		{ return new TriangleArea_Component(); }
			break; // 409+22=431
		case 'U':
			if (z(name, L"Union"))			{ return new Union_Component(); }
			if (z(name, L"UsCity"))			{ return new UsCity_Component(); }
			if (z(name, L"UpCase"))			{ return new UpCase_Component(); }
			if (z(name, L"UTF"))			{ return new UTF8_Component(); }
			break; // 431+4=435
		case 'V':
			if (z(name, L"VALUE"))			{ return new Canvas_Value_Component(); }
			if (z(name, L"VAXIS"))			{ return new Canvas_VerticalAxis_Component(); }
			if (z(name, L"Var"))			{ return new Var_Component(); }
			if (z(name, L"ValueTerms"))		{ return new ValueTerms_Component(); }
			if (z(name, L"Version"))		{ return new Version_Component(); }
			break; // 435+5=440
		case 'W':
			if (z(name, L"WorldCity"))		{ return new WorldCity_Component(); }
			if (z(name, L"WriteTxFile"))	{ return new WriteTextFile_Component(); }
			if (z(name, L"WriteDatFile"))	{ return new WriteDataFile_Component(); }
			break; // 440+3=443
		case 'X':
			if (z(name, L"Xor"))			{ return new Xor_Component(); }
			if (z(name, L"XyRec"))			{ return new XYRecorder_Component(); }
			if (z(name, L"XyzToLl"))		{ return new XyzToLl_Component(); }
			if (z(name, L"XyAxis"))			{ return new Canvas_XyAxis_Component(); }
			break; // 443+4=447
		case 'Y':
			if (z(name, L"Year"))			{ return new Year_Component(); }
			break; // 447+1=448
		case 'Z':
			break;
	}
	// 5/7/2019 448 components
	
/*
	//if(z(name, L"Diff")) {
	//	nC = new Component(L"Diff", L"Differen.", inputSpecialRepeater_, triangleShape, 1, 1, 1, 1);
	//	nC->SetInnerMethod2(funcDiff);
	//	nC->timeBased = true;
	//	return nC; }

//	if (z(name, L"Logic2"))		{ return new Logic2_Component(); }

	if(z(name, L"TrigInteg"))  {
		nC = new Component(L"TrigInteg", L"Triggered Integrator", inputSpecialRepeater_, funcTriggeredIntegrator, squareShape, 6, 1, 1, 1);
		//  nC->dialogList = dialogIntegButtonList;
		//  nC->timeBased = true;
		//  nC->dial = dialInteg_;
		nC->setLeftLabels(3, L"Initial", L"+ In", L"- In", L"dx", L"Step", L"Reset");
		return nC; } 

	if(z(name, L"2dClick"))  {
		nC = new Component(L"2dClick", L"2D Click", dial2dClick_, null, squareShape, 4, 1, 1, 1);
		nC->dialogList = dialog2dClickButtonList;
		nC->method2d = dial2dClick_;
		nC->crank2d = crank2dClick_;
		nC->onLoad = onLoad2dClick_;
		nC->onMouseUp2d = onMouseUp2dClick_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height");
		nC->setSetting(0, 1 + 2); // options: 1=position, 2=Size, 4=Enable, 8=alpha
		nC->setMemory(0, 0; // edge detector
		return nC; } 
	if(z(name, L"2dHover"))  {
		nC = new Component(L"2dHover", L"2D Hover", method2dClick_, null, squareShape, 4, 1, 1, 1);
		nC->dialogList = dialog2dClickButtonList;
		nC->method2d = dial2dClick_;
		// nC->crank2d = crank2dClick_;
		nC->onLoad = onLoad2dClick_;
		// nC->onMouseUp2d = onMouseUp2dClick_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height");
		nC->setSetting(0, 1 + 2); // options: 1=position, 2=Size, 4=Enable, 8=alpha
		nC->setMemory(0, 0; // just to match 2dClick above
		return nC; } 

	if(z(name, L"Midi"))  {
		nC = new Component(L"Midi", L"MIDI Send", inputSpecialRepeater_, funcMidi, squareShape, 4, 1, 0, 0); // maybe output the instrument name
		nC->setLeftLabels(3, L"Trigger", L"Note", L"Velocity", L"Instrument");// duration??
		return nC; } 
	
	if(z(name, L"ChainModel"))  {
		nC = new Component(L"ChainModel", L"Chain Model", inputSpecialRepeater_, funcChainModel, squareShape, 3, 1, 0, 0);
		nC->setLeftLabels(3, L"UUID", L"Params", L"Trigger");
		nC->setMemory(0, 0;
		return nC; } 
	if(z(name, L"Prod"))  {
		nC = new Component(L"Prod", L"Product", inputSpecialRepeater_, funcProd, 3, 1);
		nC->setLeftLabels(3, L"Input", L"Trigger", L"Reset");
		nC->output[0].value[0] = 1;
		return nC; }
	
	//if(z(name, L"ListMem"))		{ return new ListMem_Component(); 

	if(z(name, L"Path2"))  {
		nC = new Component(L"Path2", L"Path in 2D", methodPath2_, null, squareShape, 4, 1, 1, 1);
		nC->setLeftLabels(3, L"Matrix", L"Path X", L"Path Y", L"Time");
		return nC; } 
	if(z(name, L"Path3"))  {
		nC = new Component(L"Path3", L"Path in 3D", methodPath3_, null, squareShape, 5, 1, 1, 1);
		nC->setLeftLabels(3, L"Matrix", L"Path X", L"Path Y", L"Path Z", L"Time");
		return nC; } 

	if(z(name, L"Save"))  {
		nC = new Component(L"Save", L"Save to Disk", methodSaveToDisk_, null, squareShape, 3, 1, 0, 1);
		nC->setLeftLabels(3, L"Name", L"Value", L"Trigger");
		nC->setMemory(0, false; //  edge memory
		return nC; }
	if(z(name, L"Load"))  {
		nC = new Component(L"Load", L"Load from Disk", methodLoadFromDisk_, null, 2, 1);
		nC->setLeftLabels(3, L"Name", L"Trigger");
		nC->setRightLabels(3, L"Value");
		nC->setMemory(0, false; //  edge memory
		return nC; }
	
		//case 'Store"))  {
		//    nC = new Component(L"Store", L"Store", methodStore_, null, 3, 1);
		//    nC->dialogList = dialogStoreButtonList;
		//    nC->setLeftLabels(3, L"Input 0", L"Push", L"Pull");
		//    nC->setRightLabels(3, L"0");
		//    nC->setMemory(0, false); // Push edge memory
		//    nC->setMemory(1, false); // Pull edge memory
		//    nC->setSetting(0, 1); // number of data inputs
		//    nC->setLiteral(0, '');  // the key to the cloud
		//    nC->outputValue[0, []; // start empty
		//    nC->onLoad = onLoadStore_;
		//    //  nC->width = 50;
		//    return nC; }
	
	if(z(name, L"Eval"))  {
		nC = new Component(L"Eval", L"Evaluate", methodEvaluate_, null, squareShape, 4, 1, 1, 1);
		nC->setLeftLabels(3, L"A", L"B", L"C", L"Program");
		nC->dialogList = dialogEvalButtonList;
		nC->onLoad = onLoadEval_;
		nC->setSetting(0, 3);
		return nC; }
	
	if(z(name, L"Rast"))  {
		nC = new Component(L"Rast", L"Raster Solver", methodRaster_, null, squareShape, 4, 1, 4, 1);
		nC->dialogList = dialogRasterButtonList;
		nC->setLeftLabels(3, L"Feedback 0", L"Trigger", L"Param 0", L"Param 1");
		nC->setRightLabels(3, L"Result 0", L"Done", L"Param 0", L"Param 1");
		nC->width = 55;
		nC->timeBased = true;
		nC->onLoad = onLoadRaster_;
		nC->setSetting(0, 1); // number of feedbacks
		nC->setSetting(1, 2); // number of parameters
		nC->setMemory(0, 0); // trigger input memory
		nC->setMemory(1, 0); // running memory goes hi on trigger input low when done
		nC->setMemory(2, 0); // the number of outputs calculated
		nC->setMemory(3, 0); // the system time
		return nC; }

//if (name == L"Second")) { return new Second_Component(); }
		
	if(z(name, L"2dList"))  {
		nC = new Component(L"2dList", L"2D List", method2dList_, null, squareShape, 5, 1, 2, 1);
		nC->dialogList = dialog2dListButtonList;
		nC->method2d = dial2dList_;
		nC->crank2d = crank2dList_;
		nC->onLoad = method2dList_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"List");
		nC->setRightLabels(3, L"Index", L"List");
		nC->width = 40;
		nC->setSetting(0, 3); // put label above
		nC->getInternals = getInternals_1up;
		nC->setInternals = setInternals_1up_IndexListOut;
		return nC; }
	
	if(z(name, L"2dPopup"))  {
		nC = new Component(L"2dPopup", L"2D Popup List", method2dPopup_, null, squareShape, 5, 1, 1, 1);
		nC->dialogList = dialog2dListButtonList;
		nC->method2d = dial2dPopup_;
		nC->crank2d = crank2dPopup_;
		nC->onLoad = method2dPopup_;
		nC->onMouseUp2d = onMouseUp2dPopup_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"List");
		// nC->setRightLabels(3, L"Index", L"List");
		nC->width = 40;
		nC->setSetting(0, 3); // put label above
		nC->setSetting(1, -1); // no selection yet
		nC->setMemory(0, -1; // no list is expanded
		nC->setMemory(1, -1; // not hovering over an item
		nC->getInternals = getInternals_1up;
		nC->setInternals = setInternals_1up_1out;
		return nC; }
	
	if(z(name, L"2dString"))  {
		nC = new Component(L"2dString", L"2D String", method2dString_, null, squareShape, 5, 1, 1, 1);
		nC->method2d = dial2dString_;
		nC->crank2d = crank2dString_;
		nC->onLoad = method2dString_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Clear");
		return nC; }
	if(z(name, L"2dThumb"))  {
		nC = new Component(L"2dThumb", L"2D Thumb Wheel", method2dThumb_, null, squareShape, 6, 1, 1, 1);
		nC->dialogList = dialog2dThumbButtonList; // works for all
		nC->method2d = dial2dThumb_;
		nC->crank2d = crank2dThumb_;
		nC->onLoad = method2dThumb_;
		nC->onMouseUp2d = onMouseUp2dTxSpin_;// more hope
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Button Size", L"Clear");
		nC->setSetting(0, 3); // number of digits
		nC->setSetting(1, 0); // initial value
		nC->getInternals = getInternals_1up;
		nC->setInternals = setInternals_1up_1out;
		return nC; }
	
	if(z(name, L"2dEnThumb"))  {
		nC = new Component(L"2dEnThumb", L"2D Thumb Wheel", method2dThumb_, null, squareShape, 8, 1, 1, 1);
		nC->dialogList = dialog2dThumbButtonList; // works for all
		nC->method2d = dial2dThumb_;
		nC->crank2d = crank2dThumb_;
		nC->onLoad = method2dThumb_;
		nC->onMouseUp2d = onMouseUp2dTxSpin_;// more hope
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Button Size", L"Clear", L"Maximum", L"Minimum");
		nC->setSetting(0, 3); // number of digits
		nC->setSetting(1, 0); // initial value
		nC->getInternals = getInternals_1up;
		nC->setInternals = setInternals_1up_1out;
		return nC; }
	
	
	if(z(name, L"2dToggle"))  {
		nC = new Component(L"2dToggle", L"2D Toggle Text", method2dToggle_, null, squareShape, 7, 1, 1, 1);
		nC->dialogList = dialog2dButtonButtonList; // works for both
		nC->method2d = dial2dToggle_;
		nC->crank2d = crank2dToggle_;
		nC->onLoad = method2dToggle_;
		nC->onMouseUp2d = onMouseUp2dToggle_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Frame", L"Clear", L"Text");
		nC->setSetting(0, 0); // initial value
		nC->setMemory(0, 0); // edge detector
		nC->getInternals = getInternals_0up;
		nC->setInternals = setInternals_0up_1out;
		return nC; }


		if(z(name, L"2dDragBox"))  {
		nC = new Component(L"2dDragBox", L"2D Drag Box", method2dDragBox_, null, squareShape, 9, 1, 6, 1);
		nC->dialogList = dialog2dDragBoxButtonList;
		nC->method2d = dial2dDragBox_;
		nC->setMemory(0, -1); // used to hold crank moving element index
		nC->setMemory(1, 0); // used to hold mouse x offset
		nC->setMemory(2, 0); // used to hold mouse Y offset
		nC->setMemory(3, 0); // append edge memory
		nC->setMemory(4, 0); // delete edge memory
		nC->setMemory(5, -1); // used to hold crank sizing element index
		nC->setMemory(6, 0); // used to hold crank sizing position: 1=top, 2=bottom, 4=left, 8=right
		nC->setMemory(7, 0); // horiz anchor
		nC->setMemory(8, 0); // vert anchor
		nC->setMemory(9, 0); // clear edge memory
		nC->setMemory(10, -1); // we stole the update, fix it now
		nC->initialMemoryX = [];
		nC->initialMemoryY = [];
		nC->initialMemoryW = [];
		nC->initialMemoryH = [];
		nC->crank2d = crank2dDragBox_;
		nC->onLoad = onLoad2dDragBox_;
		nC->onMouseUp2d = onMouseUp2dDragBox_;
		nC->onDoubleClick2d = onDoubleClick2dDragBox_;
		nC->onDoubleClear2d = onDoubleClear2dDragBox_;
		nC->width = 42;
		nC->setSetting(0, 1);  // number of boxes
		nC->setSetting(1, 1 + bitSetArray[10]);  // drawing (code): 0=(X, Y, Width, Height, Append, Boolean, Delete, Track, Clear),
		//  (1) 1=Anchor(2), 2=LineWidth(4), 3=LineColor(8), 4=FillColor(16), 5=alpha(32), 6=Mouse(64 -- 10=outputs(1024)
		nC->setSetting(2, 0); // X detent
		nC->setSetting(3, 0); // Y detent
		nC->setElement(0, -65);  // value of x[0]
		nC->setElement(1, 65);   // value of y[0]
		nC->setElement(2, 100);  // value of width[0]
		nC->setElement(3, 100);  // value of height[0]
		nC->output[0].value[0] = nC->element[0];
		nC->output[1].value[0] = nC->element[1];
		nC->outputValue[2][0] = nC->element[2];
		nC->outputValue[3][0] = nC->element[3];
		nC->outputValue[4][0] = 0;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Append", L"Index", L"Track", L"Delete", L"Clear");
		nC->setRightLabels(3, L"X", L"Y", L"Width", L"Height", L"Active", L"Selected");
		nC->getInternals = getInternals_2dDragBox;
		nC->setInternals = setInternals_2dDragBox;
		nC->pushElementsOut = push2dDragBox_;
		return nC; }
	if(z(name, L"2dDragLine"))  {
		nC = new Component(L"2dDragLine", L"2D Drag Line", method2dDragLine_, null, squareShape, 9, 1, 8, 1);
		nC->dialogList = dialog2dDragLineButtonList;
		nC->method2d = dial2dDragLine_;
		nC->setMemory(0, -1); // used to hold crank moving element index
		nC->setMemory(1, 0); // used to hold mouse x0 offset
		nC->setMemory(2, 0); // used to hold mouse Y0 offset
		nC->setMemory(3, 0); // append edge memory
		nC->setMemory(4, 0); // delete edge memory
		nC->setMemory(5, -1); // used to hold crank sizing element index
		nC->setMemory(6, 0); // used to hold crank sizing position: 1=point_0, 2=point_1
		nC->setMemory(7, 0); // set to 1 if we know the mouse starting point
		nC->setMemory(9, 0); // clear edge memory
		nC->setMemory(10, -1); // we stole the update, fix it now
		nC->initialMemoryX = [];
		nC->initialMemoryY = [];
		nC->crank2d = crank2dDragLine_;
		nC->onLoad = onLoad2dDragLine_;
		nC->onMouseUp2d = onMouseUp2dDragLine_;
		nC->onDoubleClick2d = onDoubleClick2dDragLine_;
		nC->onDoubleClear2d = onDoubleClear2dDragLine_;
		nC->width = 42;
		nC->setSetting(0, 1);  // number of points
		nC->setSetting(1, 1 + bitSetArray[10]);  // drawing (code): 0=(X, Y, Width, Height, Append, Boolean, Delete, Track, Clear),(1) 1=Anchor(2), 2=alpha(4), 3=Mouse(8) -- 10=outputs 
		nC->setSetting(2, 2);  // number of points in each line
		nC->setSetting(3, 0);  // x Detent
		nC->setSetting(4, 0);  // y Detent  
		nC->setElement(0, -50); // value of x[0][0]
		nC->setElement(1, -50); // value of y[0][0]
		nC->setElement(2, 50);  // value of x[0][1]
		nC->setElement(3, 50);  // value of y[0][1]
		nC->output[0].value[0] = nC->element[0]; // x0
		nC->output[1].value[0] = nC->element[1]; // y0
		nC->outputValue[2][0] = nC->element[2]; // x1
		nC->outputValue[3][0] = nC->element[3]; // y1
		nC->outputValue[4][0] = sqrt(2) * 100; // len
		nC->outputValue[5][0] = PI / 4;   // angle
		nC->outputValue[6][0] = 0; // active
		nC->outputValue[7][0] = 0; // selected
		nC->setLeftLabels(3, L"X0", L"Y0", L"X1", L"Y1", L"Append", L"Index", L"Track", L"Delete", L"Clear");
		nC->setRightLabels(3, L"X0", L"Y0", L"X1", L"Y1", L"Length", L"Angle", L"Active", L"Selected");
		nC->getInternals = getInternals_2dDragLine;
		nC->setInternals = setInternals_2dDragLine;
		return nC; }
	
	if(z(name, L"2dVButton"))  {
		nC = new Component(L"2dVButton", L"2D Virtual Button", method2dVirtualButton_, null, 2, 1);
		nC->dialogList = dialog2dVirtualButtonButtonList;
		nC->method2d = dial2dVirtualButton_;
		nC->timeBased = true;
		nC->setMemory(0, -1); // knob being pressed
		nC->setMemory(1, 0); // spare
		nC->setMemory(2, 0); // spare
		nC->setMemory(3, 0); // time mouse started hovering
		nC->setMemory(4, 0); // fraction of ring to show
		nC->setMemory(5, 0); // phase: 0-waiting, 1-ramping up, 2-tracking, 3-ramping down
		nC->setMemory(6, 0); // spare
		nC->setMemory(7, 0); // spare
		nC->setMemory(8, 0); // clear release timer;
		nC->onLoad = onLoad2dVirtualButton_; // this is where we read the Initial Position inputs
		nC->onMouseUp2d = onMouseUp2dVirtualButton_;
		// nC->width = 45;
		nC->setSetting(0, 1 + 64); // 1-initial X/Y/Radius inputs, (64)-X/Y/Active outputs
		nC->setSetting(1, 1); // the number of buttons
		nC->setLeftLabels(3, L"X", L"Y");
		nC->setRightLabels(3, L"Out");
		//    nC->getInternals = getInternals_2dVPoint;
		//    nC->setInternals = setInternals_2dVPoint;
		return nC; }
	if(z(name, L"2dVPoint"))  {
		nC = new Component(L"2dVPoint", L"2D Virtual Point", method2dVirtualPoint_, null, squareShape, 2, 1, 3, 1);
		nC->dialogList = dialog2dVirtualPointButtonList;
		nC->method2d = dial2dVirtualPoint_;
		nC->timeBased = true;
		nC->setMemory(0, -1); // knob in motion
		nC->setMemory(1, 0); // X offset for drag
		nC->setMemory(2, 0); // Y offset for drag
		nC->setMemory(3, 0); // time mouse started hovering
		nC->setMemory(4, 0); // fraction of ring to show
		nC->setMemory(5, 0); // phase: 0-waiting, 1-ramping up, 2-tracking, 3-ramping down
		nC->setMemory(6, 0); // last mouse X - test for movement to release
		nC->setMemory(7, 0); // last mouse Y - test for movement to release
		nC->setMemory(8, 0); // clear release timer;
		nC->onLoad = onLoad2dVirtualPoint_; // this is where we read the Initial Position inputs
		nC->onMouseUp2d = onMouseUp2dVirtualPoint_;
		nC->width = 45;
		nC->setSetting(0, 1 + 64); // 1-initial X/Y inputs, (64)-X/Y/Active outputs
		nC->setSetting(1, 1); // the number of points
		nC->setElement(0, 0); // point zero X is at element 0
		nC->setElement(1, 0); // point zero Y is at element 1
		nC->setLeftLabels(3, L"Initial X", L"Initial Y");
		nC->setRightLabels(3, L"X", L"Y", L"Active");
		//    nC->getInternals = getInternals_2dVPoint;
		//    nC->setInternals = setInternals_2dVPoint;
		return nC; }
	
	if(z(name, L"Orientation"))  {
		nC = new Component(L"Orientation", L"Orientation", methodOrientation_, null, squareShape, 1, 1, 8, 1);
		for (k = 0; k < 8; k++) {
			nC->outputValueDimensions[k] = [1];
			nC->outputValue[k, 0.0;
		}
		nC->setRightLabels(3, L"Absolute", L"Alpha", L"Beta", L"Gamma", L"Absolute*", L"Alpha*", L"Beta*", L"Gamma*");
		nC->setLeftLabels(3, L"Tare");
		nC->timeBased = true; // just for Sheet*s
		nC->onLoad = onLoadOrientation_;
		nC->setMemory(0, 0); // tare edge detection
		nC->width = 50;
		return nC; }

	if(z(name, L"Wheel"))  {
		nC = new Component(L"Wheel", L"2D Wheel", methodWheel_, null, squareShape, 5, 1, 1, 1);
		nC->setMemory(0, 0); // starting X
		nC->setMemory(1, 0); // starting Y
		nC->setMemory(2, 0); // button edge
		nC->setMemory(3, []; // where to keep the recent matrix
		nC->setMemory(4, []; // where to keep the previous matrix
		nC->setMemory(5, []; // where to keep the previous [4x4] input
		for (var k = 0; k < 16; k++) {
			nC->setMemory(3][k, originMatrix.elements[k];
			nC->setMemory(4][k, originMatrix.elements[k];
			nC->setMemory(5][k, originMatrix.elements[k];
		}
		nC->setLeftLabels(3, L"X", L"Y", L"Button", L"Reset", L"[4x4]");
		nC->setRightLabels(3, L"[4x4]");
		nC->outputValueDimensions[0] = [];
		return nC; }
	if(z(name, L"Pan"))  {
		nC = new Component(L"Pan", L"2D Pan", methodPan_, null, squareShape, 5, 1, 1, 1);
		nC->setMemory(0, 0); // starting X
		nC->setMemory(1, 0); // starting Y
		nC->setMemory(2, 0); // button edge
		nC->setMemory(3] = []; // where to keep the recent matrix
		nC->setMemory(4] = []; // where to keep the previous matrix
		nC->setMemory(5] = []; // where to keep the previous [4x4] input
		for (k = 0; k < 16; k++) {
			nC->setMemory(3][k] = originMatrix.elements[k];
			nC->setMemory(4][k] = originMatrix.elements[k];
			nC->setMemory(5][k] = originMatrix.elements[k];
		}
		nC->setLeftLabels(3, L"X", L"Y", L"Button", L"Reset", L"[4x4]");
		nC->setRightLabels(3, L"[4x4]");
		nC->outputValueDimensions[0] = [];
		return nC; }
	
	if(z(name, L"Mouse3"))  {
		nC = new Component(L"Mouse3", L"3D Mouse", methodMouse3_, null, squareShape, 0, 0, 3, 1);
		nC->setSetting(0, 0.0);
		nC->setSetting(1, 0.0);
		nC->setSetting(2, false);
		nC->setRightLabels(3, L"X", L"Y", L"Button");
		nC->timeBased = true;
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }
	
	if(z(name, L"Compass"))  {
		nC = new Component(L"Compass", L"Compass", methodNull_, null, squareShape, 1, 1, 0, 0);
		nC->dialogList = dialogDisButtonList;
		nC->dial = dialCompass_;
		nC->crank = crankCompass_;
		nC->hover = hoverMeter;
		nC->setSize(60, 60);
		nC->setSetting(0, 60);     // x size
		nC->setSetting(1, 60);     // y size
		nC->onLoad = onLoadDis_; // same for now
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }
	if(z(name, L"Level"))  {
		nC = new Component(L"Level", L"Level", methodNull_, null, squareShape, 1, 1, 0, 0);
		nC->dialogList = dialogLevelButtonList;
		nC->dial = dialLevel_;
		nC->crank = crankLevel_;
		nC->setSize(20, 60);
		nC->setSetting(0, 20);     // x size
		nC->setSetting(1, 60);     // y size
		nC->setSetting(2, 100);    // max value
		nC->setSetting(3, 80);    // percent to change to yellow
		nC->setSetting(4, 90);    // percent to change to red
		nC->onLoad = onLoadDis_; // same for now
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }

	if(z(name, L"BarChart"))  {
		nC = new Component(L"BarChart", L"Bar Chart", methodNull_, null, squareShape, 1, 1, 0, 0);
		nC->dialogList = dialogChartButtonList; // works for both
		nC->dial = dialBarChart_;
		nC->crank = crankBarChart_;
		nC->hover = hoverBarChart;
		nC->setSize(60, 60);
		nC->setSetting(0, 60);
		nC->setSetting(1, 60);
		nC->setSetting(2, 1); // number of input pins
		nC->setSetting(3, 0); // display mode: 0=lines, 1= dots, 2= lines and dots
		nC->onLoad = onLoadChart_;
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }
	
	if(z(name, L"2dContour"))  {
		nC = new Component(L"2dContour", L"2D Contour Plotter", methodNull_, null, squareShape, 10, 1, 0, 0);
		nC->method2d = func2dContour_;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Data Values", L"Plot Interval", L"Red", L"Green", L"Blue", L"Alpha");
		nC->width = 40;
		return nC; }
	
	if(z(name, L"Contour"))  {
		nC = new Component(L"Contour", L"Contour Plot", methodNull_, null, squareShape, 2, 2, 0, 0);
		nC->dialogList = dialogContourButtonList;
		nC->dial = dialContour_;
		nC->crank = crankChart2_;
		nC->hover = hoverContour;
		nC->setSize(60, 60);
		nC->setSetting(0, 60);
		nC->setSetting(1, 60);
		nC->onLoad = onLoadContour_;
		nC->setLeftLabels(3, L"Mat", L"Int");
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }
	if(z(name, L"Rec"))  {
		nC = new Component(L"Rec", L"Recorder", methodNull_, null, squareShape, 1, 1, 0, 0);
		nC->timeBased = true;
		nC->dialogList = dialogRecButtonList;
		nC->dial = dialRec_;
		nC->crank = crankRec_;
		nC->hover = hoverRec;
		nC->setSize(60, 60);
		nC->setSetting(0, 60); // width
		nC->setSetting(1, 60); // height
		nC->setSetting(2, 100); // limit
		nC->setSetting(3, systemTime;
		nC->onLoad = onLoadDis_;
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }
	if(z(name, L"XyRec"))  {
		nC = new Component(L"XyRec", L"XY Recorder", methodXyRec_, null, squareShape, 3, 1, 0, 0);
		nC->dialogList = dialogXyRecButtonList;
		nC->dial = dialXyRec_;
		nC->crank = crankXyRec_;
		nC->hover = hoverXyRec;
		nC->setSize(60, 60);
		nC->setSetting(0, 60); // width
		nC->setSetting(1, 60); // height
		nC->memory2 = [];
		nC->onLoad = onLoadDis_;
		nC->setLeftLabels(3, L"X", L"Y", L"C");
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }

	if(z(name, L"Speak"))  {
		nC = new Component(L"Speak", L"Speak", methodSpeak_, null, squareShape, 7, 1, 0, 1);
		nC->setLeftLabels(3, L"Message", L"Volume (0-1)", L"Rate (0.1-10)", L"Pitch (0-2)", L"Voice", L"Language", L"Trigger");
		nC->width = 40;
		nC->memory = [0]; // for trigger memory
		nC->setMemory(0, 0);
		return nC; }
			if(z(name, L"ARC"))  {
			nC = new Component(L"ARC", L"Arc", method2dARC_, null, squareShape, 6, 1, 0, 0);
			nC->dialogList = dialogARCButtonList;
			//             1=Position, 2=Size,  4=Start/End, 8=Line Width
			nC->setLeftLabels(3, L"X", L"Y", L"Radius", L"Start", L"End", L"Line Width");
			nC->method2d = method2dARC_;
			nC->onLoad = onLoadARC_;
			nC->setSetting(0, 15);
			return nC; }
	
	if((z(name, L"CURVE"))  ||
	   (z(name, L"Curve")) ) {
		nC = new Component(L"CURVE", L"Curve", methodNull_, null, squareShape, 8, 1, 0, 0);
		nC->dialogList = dialogCURVEButtonList;
		nC->setLeftLabels(3, L"X", L"Y", L"X size", L"Y size", L"Orientation", L"Line Width", L"Curve Xs", L"Curve Ys");
		nC->method2d = methodCURVE_;
		nC->onLoad = onLoadCURVE_;
		nC->setSetting(0, 143);
		nC->width = calcComponentWidth(nC);
		return nC; }
	

	if(z(name, L"Vector"))  {
		nC = new Component(L"Vector", L"X/Y Vector", methodVector_, null, squareShape, 2, 1, 2, 1);
		nC->setLeftLabels(3, L"X", L"Y");
		nC->setRightLabels(3, L"X", L"Y");
		return nC; }

	if(z(name, L"Get2dPixel"))  {
		nC = new Component(L"Get2dPixel", L"Get Pixel", methodGet2dPixel_, null, squareShape, 2, 1, 4, 1);
		nC->timeBased = true;
		nC->setLeftLabels(3, L"X", L"Y");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue", L"Alpha");
		return nC; }
	
	if(z(name, L"LineAnal"))  {
		nC = new Component(L"LineAnal", L"Line Analysis", methodLineAnalysis_, null, squareShape, 5, 1, 4, 1);
		//nC->dialogList = dialogLineAnalButtonList;
		//nC->onLoad = onLoadLineAnal_;
		nC->width = 42;
		nC->setSetting(0, 0);  // future
		nC->setLeftLabels(3, L"X0", L"Y0", L"X1", L"Y1", L"Tolerance");
		nC->setRightLabels(3, L"Sides", L"Xs", L"Ys", L"Closed");
		return nC; }
	if(z(name, L"CircAnal"))  {
		nC = new Component(L"CircAnal", L"Circuit Analysis", methodCircuitAnalysis_, null, squareShape, 4, 1, 7, 1);
		nC->dialogList = dialogCircAnalButtonList;
		nC->onLoad = onLoadCircAnal_;
		nC->width = 55;
		nC->setSetting(0, 2);  // number of Component inputs
		nC->setSetting(1, 1);  // number of connector inputs
		nC->setSetting(2, 1);  // number of sequence inputs
		nC->setLeftLabels(3, L"Comp_0", L"Comp_1", L"Conn_0", L"Seq_0");
		nC->setRightLabels(3, L"Comp_0", L"Comp_1", L"Conn_0", L"Xs", L"Ys", L"Seq Index", L"Closed");
		return nC; }
	if(z(name, L"PatAnal"))  {
		nC = new Component(L"PatAnal", L"Pattern Analysis", methodPatternAnalysis_, null, squareShape, 4, 1, 6, 1);
		nC->dialogList = dialogPatAnalButtonList;
		nC->onLoad = onLoadPatAnal_;
		nC->setSetting(0, 2);  // number of Component inputs
		nC->setSetting(1, 1);  // code for matching: 0-position(1), 1-size(2), 2-angle(4)
		nC->setSetting(2, 1);  // allowShiftX
		nC->setSetting(3, 1);  // allowShiftY
		nC->setSetting(4, 1);  // allowScaleX
		nC->setSetting(5, 1);  // allowScaleY
		nC->setSetting(6, 1);  // combineXYScales
		nC->setLeftLabels(3, L"Comp_0", L"Comp_1", L"Pattern_0", L"Pattern_1");
		nC->setRightLabels(3, L"Comp_0", L"Comp_1", L"Position_0", L"Position_1", L"Matches", L"Complete");
		nC->width = calcComponentWidth(nC);
		return nC; }
	if(z(name, L"BoundBox"))  {
		nC = new Component(L"BoundBox", L"Bounding Box", methodBoundingBox_, null, squareShape, 4, 1, 4, 1);
		nC->dialogList = dialogBoundingBoxButtonList;
		nC->onLoad = onLoadBoundingBox_;
		nC->setLeftLabels(3, L"Xs", L"Ys", L"Widths", L"Heights");
		nC->setRightLabels(3, L"X", L"Y", L"Width", L"Height");
		nC->setSetting(0, 0); // no extra inputs
		nC->width = 40;
		return nC; }
	if(z(name, L"SnapPoint"))  {
		nC = new Component(L"SnapPoint", L"Snap Point", methodSnapPoint_, null, squareShape, 12, 1, 7, 1);
		nC->crank2d = crank2dSnapPoint_;
		nC->onMouseUp2d = onMouseUp2dSnapPoint_;
		nC->setMemory(0, 0); // point in motion
		nC->setMemory(1, 0); // offset X
		nC->setMemory(2, 0); // offset Y
		nC->initialMemoryX = [];
		nC->initialMemoryY = [];
		nC->setSetting(0, 0);  // spare
		nC->setSetting(1, 1);  // spare
		nC->setLeftLabels(3, L"Initial X", L"Initial Y", L"Drag Width", L"Drag Height", L"Target X", L"Target Y", L"Target Width", L"Target Height", L"Initialize", L"Enable/1", L"Order/0", L"Snap/1");
		nC->setRightLabels(3, L"Point X", L"Point Y", L"Active Point", L"Point Placed On", L"Target Filled By", L"Matches", L"Complete");
		nC->width = 75;
		return nC; }
	
	if(z(name, L"SnapLine"))  {
		nC = new Component(L"SnapLine", L"Snap Line", methodSnapLine_, null, squareShape, 15, 1, 11, 1);
		nC->dialogList = dialogSnapLineButtonList;
		nC->onLoad = onLoadSnapLine_;
		nC->crank2d = crank2dSnapLine_;
		nC->onMouseUp2d = onMouseUp2dSnapLine_;
		nC->setMemory(0, 0); // polyline in motion
		nC->setMemory(1, 0); // point in motion on that polyline
		nC->setMemory(2, 0); // offset X
		nC->setMemory(3, 0); // offset Y
		nC->initialMemoryX = [];
		nC->initialMemoryY = [];
		nC->setSetting(0, 2);  // number of points in each
		nC->setSetting(1, 1);  // spare
		nC->setLeftLabels(3, L"Initial X0", L"Initial Y0", L"Initial X1", L"Initial Y1", L"Drag Radius", L"Target X0", L"Target Y0", L"Target X1", L"Target Y1", L"Target Radius", L"Initialize", L"Enable Points/1", L"Enable Lines/1", L"Snap/1", L"Order/0");
		nC->setRightLabels(3, L"Point X0", L"Point Y0", L"Point X1", L"Point Y1", L"Active", L"0 Placed", L"1 Placed", L"0 Filled", L"1 Filled", L"Matches", L"Complete");
		nC->width = 65;
		return nC; }
	if(z(name, L"Set2dPixel"))  {
		nC = new Component(L"Set2dPixel", L"Set Pixel", methodSet2dPixel_, null, squareShape, 6, 1, 0, 0);
		nC->setLeftLabels(3, L"X", L"Y", L"Red", L"Green", L"Blue", L"Alpha");
		return nC; }
	
	if(z(name, L"2dFixed"))  {
		nC = new Component(L"2dFixed", L"2D Fixed", methodFixed_, null, squareShape, 4, 1, 4, 1);
		nC->setLeftLabels(3, L"X%", L"Y%", L"Width%", L"Height%");
		nC->setRightLabels(3, L"X", L"Y", L"Width", L"Height");
		nC->width = 40;
		nC->method2d = methodFixed_;
		return nC; }
	if(z(name, L"AlignOnGlass"))  {
		nC = new Component(L"AlignOnGlass", L"Align on Glass", methodAlignOnGlass_, null, squareShape, 8, 1, 4, 1);
		nC->setLeftLabels(3, L"Left", L"Right", L"Top", L"Bottom", L"Width", L"Height", L"Horizontal Mode", L"Vertical Mode");
		nC->setRightLabels(3, L"X", L"Y", L"Width", L"Height");
		nC->width = 45;
		return nC; }
	if(z(name, L"2dInfo"))  {
		nC = new Component(L"2dInfo", L"2d Info", method2dInfo_, null, squareShape, 0, 1, 5, 1);
		nC->setRightLabels(3, L"Scale Factor", L"X Offset", L"Y Offset", L"Width", L"Height");
		//  nC->width = 40;
		nC->method2d = method2dInfo_;
		return nC; }
	if(z(name, L"3Origin"))  {
		nC = new Component(L"3Origin", L"3D Origin", method3Origin_, null, squareShape, 0, 0, 1, 1);
		nC->setRightLabels(3, L"[4x4]");
		nC->height = 15;
		nC->onLoad = method3Origin_;
		// special initialization
		nC->output[0].value = originMatrix;
		nC->outputValueDimensions[0] = [4, 4];
		nC->height = COMPONENT_HEIGHT; // added because it was original size
		return nC; }

	if(z(name, L"3UNSCALE"))  {
		nC = new Component(L"3UNSCALE", L"3D Get Size", input3dRepeater_, func3UNSCALE_, squareShape, 1, 1, 4, 1);
		//    nC->dialogList = dialog3UNSHIFTButtonList; // shift and scale are the same
		nC->setLeftLabels(3, L"[4x4]");
		nC->setRightLabels(3, L"X size", L"Y size", L"Z size", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		//   nC->onLoad = onLoad3UNSHIFT_;// shift and scale are the same
		//   nC->setSetting(0, 15);
		return nC; }
	if(z(name, L"3UNSHIFT"))  {
		nC = new Component(L"3UNSHIFT", L"3D Get Position", input3dRepeater_, func3UNSHIFT_, squareShape, 1, 1, 4, 1);
		//    nC->dialogList = dialog3UNSHIFTButtonList; // shift and scale are the same
		nC->setLeftLabels(3, L"[4x4]");
		nC->setRightLabels(3, L"X position", L"Y position", L"Z position", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		//   nC->onLoad = onLoad3UNSHIFT_;// shift and scale are the same
		//   nC->setSetting(0, 15);
		return nC; }
	
	if(z(name, L"3REF"))  {
		nC = new Component(L"3REF", L"3D Reference", input3dRepeater_, func3REF_, squareShape, 1, 1, 6, 1);
		//    nC->dialogList = dialog3UNROTButtonList;
		nC->setLeftLabels(3, L"[4x4]");
		nC->setRightLabels(3, L"X position", L"Y position", L"Z position", L"X direction", L"Y direction", L"Z direction");
		nC->width = calcComponentWidth(nC);
		//   nC->onLoad = onLoad3UNROT_;
		//   nC->setSetting(0, 15);
		return nC; }
	

	if(z(name, L"4CUBE"))  {
		nC = new Component(L"4CUBE", L"3D Physics Box", input3dRepeater_, func4CUBE_, squareShape, 11, 1, 0, 1);
		nC->dialogList = dialog4CUBEButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X size", L"Y size", L"Z size", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4CUBE');
		nC->shape3D[0].scale.x = 40;
		nC->shape3D[0].scale.y = 40;
		nC->shape3D[0].scale.z = 40;
		nC->memory = []; // to record the mass
		nC->setMemory(0, 0;
		nC->needsRebuild = [];
		nC->timeBased = true;
		//   if (os == 0)
		//scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4CUBE_;
		nC->setSetting(0, 1 + 2 + 4 + 8 + 128);
		return nC; }
	if(z(name, L"4TOPO"))  {
		nC = new Component(L"4TOPO", L"3D Physics Topography", input3dRepeater_, func4TOPO_, squareShape, 12, 1, 0, 1);
		nC->dialogList = dialog4TOPOButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X size", L"Y size", L"Z matrix", L"Z width", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4TOPO');
		nC->shape3D[0].scale.x = 40;
		nC->shape3D[0].scale.y = 40;
		nC->shape3D[0].scale.z = 40;
		nC->memory = []; // to record the mass
		nC->setMemory(0, 0;
		nC->needsRebuild = [];
		nC->timeBased = true;
		//   if (os == 0)
		//scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4TOPO_;
		nC->setSetting(0, 1 + 2 + 4 + 8 + 128);
		return nC; }
	if(z(name, L"4GRND"))  {
		nC = new Component(L"4GRND", L"3D Ground", input3dRepeater_, func4GRND_, squareShape, 3, 1, 0, 1);
		nC->dialogList = dialog4GRNDButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4GRND');
		nC->shape3D[0].scale.x = 40;
		nC->shape3D[0].scale.y = 40;
		nC->shape3D[0].scale.z = 40;
		//   if (os == 0)
		//scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4GRND_;
		nC->setSetting(0, 1);
		return nC; }
	if(z(name, L"4HINGE"))  {
		nC = new Component(L"4HINGE", L"3D Hinge", input3dRepeater_, func4HINGE_, squareShape, 8, 1, 0, 1);
		nC->dialogList = dialog4HINGEButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X direction", L"Y direction", L"Z direction", L"ID0", L"ID1");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->constraint3D = [];
		nC->obj0_X = [];
		nC->obj0_Y = [];
		nC->obj0_Z = [];
		nC->obj0_dX = [];
		nC->obj0_dY = [];
		nC->obj0_dZ = [];
		nC->obj0 = [];
		nC->vis_width = [];
		nC->vis_red = [];
		nC->vis_green = [];
		nC->vis_blue = [];
		nC->vis_alpha = [];
		nC->shape3D[0] = make3dObject('4HINGE');
		nC->timeBased = true;
		//   if (os == 0)
		//       scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4HINGE_;
		nC->setSetting(0, 1 + 2 + 32);
		nC->memory = []; // store the id of the first object here
		return nC; }
	if(z(name, L"4PNT2PNT"))  {
		nC = new Component(L"4PNT2PNT", L"3D Point to Point", input3dRepeater_, func4PNT2PNT_, squareShape, 5, 1, 0, 1);
		nC->dialogList = dialog4PNT2PNTButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"ID0", L"ID1");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0, make3dObject('4PNT2PNT');
		nC->timeBased = true;
		//   if (os == 0)
		//       scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4PNT2PNT_;
		nC->setSetting(0, 1 + 4);
		return nC; }
	if(z(name, L"4SLIDE"))  {
		nC = new Component(L"4SLIDE", L"3D Slide", input3dRepeater_, func4SLIDE_, squareShape, 8, 1, 0, 1);
		nC->dialogList = dialog4SLIDEButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X direction", L"Y direction", L"Z direction", L"ID0", L"ID1");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->constraint3D = [];
		nC->obj0_X = [];
		nC->obj0_Y = [];
		nC->obj0_Z = [];
		nC->obj0_dX = [];
		nC->obj0_dY = [];
		nC->obj0_dZ = [];
		nC->obj0 = [];
		nC->vis_width = [];
		nC->vis_red = [];
		nC->vis_green = [];
		nC->vis_blue = [];
		nC->vis_alpha = [];
		nC->shape3D[0] = make3dObject('4SLIDE');
		nC->timeBased = true;
		//   if (os == 0)
		//       scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4SLIDE_;
		nC->setSetting(0, 1 + 2 + 1024);
		return nC; }
	if(z(name, L"4TWIST"))  {
		nC = new Component(L"4TWIST", L"3D Twist", input3dRepeater_, func4TWIST_, squareShape, 5, 1, 0, 1);
		nC->dialogList = dialog4TWISTButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"ID0", L"ID1");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4TWIST');
		nC->timeBased = true;
		//   if (os == 0)
		//       scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4TWIST_;
		nC->setSetting(0, 1 + 32);
		return nC; }
	if(z(name, L"4DOFC"))  {
		nC = new Component(L"4DOFC", L"3D Constraint", input3dRepeater_, func4DOFC_, squareShape, 5, 1, 0, 1);
		nC->dialogList = dialog4DOFCButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"ID0", L"ID1");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4DOFC');
		nC->timeBased = true;
		//   if (os == 0)
		//       scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4DOFC_;
		nC->setSetting(0, 1 + 256);
		return nC; }

	
	if(z(name, L"4SPHERE"))  {
		nC = new Component(L"4SPHERE", L"3D Physics Sphere", input3dRepeater_, func4SPHERE_, squareShape, 9, 1, 0, 1);
		nC->dialogList = dialog4SPHEREButtonList; // shift and scale are the same
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"Radius", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4SPHERE');
		nC->shape3D[0].scale.x = 20;
		nC->shape3D[0].scale.y = 20;
		nC->shape3D[0].scale.z = 20;
		nC->setMemory(0, 0;
		nC->needsRebuild = [];
		nC->timeBased = true;
		//     if (os == 0)
		//      scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4SPHERE_;
		nC->setSetting(0, 1 + 2 + 8 + 16 + 256);
		return nC; }
	
	if(z(name, L"4CYLIND"))  {
		nC = new Component(L"4CYLIND", L"3D Physics Cylinder", input3dRepeater_, func4CYLIND_, squareShape, 10, 1, 0, 1);
		nC->dialogList = dialog4CYLINDButtonList; // cylinder and cone are the same
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"Radius", L"Length", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4CYLIND');
		nC->shape3D[0].scale.x = 20;
		nC->shape3D[0].scale.y = 20;
		nC->shape3D[0].scale.z = 20;
		nC->setMemory(0, 0;
		nC->needsRebuild = [];
		nC->timeBased = true;
		//   if (os == 0)
		//      scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4CYLIND_; // cylinder and cone are the same
		nC->setSetting(0, 1 + 2 + 8 + 16 + 256);
		return nC; }
	if(z(name, L"4CONE"))  {
		nC = new Component(L"4CONE", L"3D Physics Cone", input3dRepeater_, func4CYLIND_, squareShape, 10, 1, 0, 1);
		nC->dialogList = dialog4CYLINDButtonList; // cylinder and cone are the same
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"Radius", L"Length", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('4CONE');
		nC->shape3D[0].scale.x = 20;
		nC->shape3D[0].scale.y = 20;
		nC->shape3D[0].scale.z = 20;
		nC->setMemory(0, 0;
		nC->needsRebuild = [];
		nC->timeBased = true;
		//   if (os == 0)
		//      scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad4CYLIND_; // cylinder and cone are the same
		nC->setSetting(0, 1 + 2 + 8 + 16 + 256);
		return nC; }
	if(z(name, L"3Polyline"))  {
		nC = new Component(L"3Polyline", L"3D Polyline", input3dRepeater_, func3dPolyline_, squareShape, 14, 1, 0, 0);
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X size", L"Y size", L"Z size", L"X poly", L"Y poly", L"Z poly", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('3Polyline');
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if(z(name, L"3Spline"))  {
		nC = new Component(L"3Spline", L"3D Spline", input3dRepeater_, func3dSpline_, squareShape, 14, 1, 0, 0);
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X size", L"Y size", L"Z size", L"X poly", L"Y poly", L"Z poly", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('3Spline');
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if((z(name, L"3Plane"))  ||
	   (z(name, L"3PLANE")) ) {
		nC = new Component(L"3PLANE", L"3D Plane", input3dRepeater_, func3PLANE_, squareShape, 10, 1, 0, 1);
		nC->dialogList = dialog3PLANEButtonList;
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X size", L"Y size", L"Red", L"Green", L"Blue", L"Alpha", L"Mass", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('3PLANE');
		nC->shape3D[0].scale.x = 40;
		nC->shape3D[0].scale.y = 40;
		nC->shape3D[0].scale.z = 40;
		if (os == 0)
			scene.add(nC->shape3D[0]);
		nC->onLoad = onLoad3PLANE_;
		nC->setSetting(0, 27);
		return nC; }
	if(z(name, L"3Surf"))  {
		nC = new Component(L"3Surf", L"3D Surface", method3dSurface_, null, squareShape, 8, 1, 0, 0);
		nC->setLeftLabels(3, L"X Matrix", L"Y Matrix", L"Z Matrix", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('3Surf');
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if(z(name, L"3Contour"))  {
		nC = new Component(L"3Contour", L"3D Contour Plot", method3dContour_, null, squareShape, 13, 1, 0, 0);
		nC->setLeftLabels(3, L"X", L"Y", L"Z", L"Width", L"Height", L"Depth", L"Data Values", L"Plot Interval", L"Red", L"Green", L"Blue", L"Alpha", L"[4x4]");
		nC->width = 40;
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('3Contour');
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if(z(name, L"3Cam"))  {
		nC = new Component(L"3Cam", L"3D Camera Dolly", method3dCam_, null, squareShape, 6, 1, 0, 1);
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"X LookAt", L"Y LookAt", L"Z LookAt");
		nC->width = calcComponentWidth(nC);
		return nC; }
	if(z(name, L"3Text"))  {
		nC = new Component(L"3Text", L"3D Text", input3dRepeater_, func3dText_, squareShape, 10, 1, 0, 1);
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"Size", L"Red", L"Green", L"Blue", L"Font ID", L"String", L"[4x4]");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = make3dObject('3Text');
		//    nC->shape3D[0].scale.x = 40;
		//    nC->shape3D[0].scale.y = 40;
		//    nC->shape3D[0].scale.z = 40;
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if(z(name, L"3Point"))  {
		nC = new Component(L"3Point", L"3D Point", method3Point, null, squareShape, 7, 1, 0, 0);
		nC->setLeftLabels(3, L"X", L"Y", L"Z", L"Red", L"Green", L"Blue", L"[4x4]");
		nC->width = calcComponentWidth(nC);
#if 0
		nC->shape3D = [];
		var particles = 500000;

		var geometry = new THREE.BufferGeometry();

		geometry.addAttribute('position', double32Array, particles, 3);
		geometry.addAttribute('color', double32Array, particles, 3);

		var positions = geometry.attributes.position.array;
		var colors = geometry.attributes.color.array;

		var color = new THREE.Color();

		var n = 1000, n2 = n / 2; // particles spread in the cube

		for (var i = 0; i < positions.length; i += 3) {

			// positions

			var x = random() * n - n2;
			var y = random() * n - n2;
			var z = random() * n - n2;

			positions[i] = x;
			positions[i + 1] = y;
			positions[i + 2] = z;

			// colors

			var vx = (x / n) + 0.5;
			var vy = (y / n) + 0.5;
			var vz = (z / n) + 0.5;

			color.setRGB(vx, vy, vz);

			colors[i] = color.r;
			colors[i + 1] = color.g;
			colors[i + 2] = color.b;

		}

		geometry.computeBoundingSphere();

		//

		var material = new THREE.ParticleSystemMaterial({ size: 15, vertexColors : true });

		particleSystem = new THREE.ParticleSystem(geometry, material);
		if (os == 0)
			scene.add(particleSystem);
		if (os == 0)
			scene.add(nC->shape3D[0]);
#endif
		return nC; }
	

	if(z(name, L"3AmbLight"))  {
		if (ComponentExists('3AmbLight')) {
			alert("You can only have 1 Ambient Light.");
			return null;
		}
		nC = new Component(L"3AmbLight", L"3D Ambient Light", method3dAmbLight_, null, squareShape, 3, 1, 0, 0);
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = new THREE.AmbientLight(0x404040); // soft white light
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if(z(name, L"3DirLight"))  {
		nC = new Component(L"3DirLight", L"3D Directional Light", method3dDirLight_, null, squareShape, 7, 1, 0, 0);
		nC->setLeftLabels(3, L"X direction", L"Y direction", L"Z direction", L"Intensity", L"Red", L"Green", L"Blue");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = new THREE.DirectionalLight(0x404040, 1); // soft white light
		nC->shape3D[0].position.set(1, 1, 1).normalize();
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if(z(name, L"3PointLight"))  {
		nC = new Component(L"3PointLight", L"3D Point Light", method3dPointLight_, null, squareShape, 8, 1, 0, 0);
		nC->setLeftLabels(3, L"X position", L"Y position", L"Z position", L"Range", L"Intensity", L"Red", L"Green", L"Blue");
		nC->width = calcComponentWidth(nC);
		nC->shape3D = [];
		nC->shape3D[0] = new THREE.PointLight(0xffffff, 1, 0); // soft white light
		nC->shape3D[0].position.set(50, 50, 50).normalize();
		if (os == 0)
			scene.add(nC->shape3D[0]);
		return nC; }
	if((z(name, L"Coord"))  ||
	   (z(name, L"COORD")) ) {
		nC = new Component(L"COORD", L"Coordinates", methodNull_, func2dCOORD, squareShape, 2, 1, 0, 0);
		nC->dialogList = dialogCOORDButtonList;
		nC->setLeftLabels(3, L"X", L"Y");
		nC->method2d = input2dRepeater_;
		nC->onLoad = onLoadCOORD_;
		nC->setSetting(0, 1);
		return nC; }

	if(z(name, L"Parse2"))  {
		nC = new Component(L"Parse2", L"Array Parser", methodParse2_, null, squareShape, 2, 1, 5, 1);
		// nC->crank = crankParse2_;
		// nC->dial = dialParse2_;
		nC->setSetting(0, 4);
		nC->width = 45;
		nC->dialogList = dialogParse2ButtonList;
		nC->setLeftLabels(3, L"Array", L"Separator");
		nC->setRightLabels(3, L"Group0", L"Group1", L"Group2", L"Group3", L"Groups");
		nC->onLoad = onLoadParse2_;
		return nC; }
	
	if(z(name, L"PointCharge"))  {
		nC = new Component(L"PointCharge", L"Point Charge", methodPointCharge_, null, squareShape, 5, 1, 10, 1);
		nC->setLeftLabels(3, L"X", L"Y", L"Charge", L"Constant", L"Scale");
		nC->setRightLabels(3, L"X Force Sum", L"Y Force Sum", L"Vector Sum X0", L"Vector Sum Y0", L"Vector Sum X1", L"Vector Sum Y1", L"Component X0", L"Component Y0", L"Component X1", L"Component Y1");
		nC->width = 60;
		return nC; }
	if(z(name, L"Gravity"))  {
		nC = new Component(L"Gravity", L"Point Charge", methodGravity_, null, squareShape, 5, 1, 10, 1);
		nC->setLeftLabels(3, L"X", L"Y", L"Mass", L"Constant", L"Scale");
		nC->setRightLabels(3, L"X Force Sum", L"Y Force Sum", L"Vector Sum X0", L"Vector Sum Y0", L"Vector Sum X1", L"Vector Sum Y1", L"Component X0", L"Component Y0", L"Component X1", L"Component Y1");
		nC->width = 60;
		return nC; }
	
	if(z(name, L"BellJar"))  {
		nC = new Component(L"BellJar", L"Bell Jar", methodBellJar_, null, squareShape, 17, 1, 7, 1);
		nC->timeBased = true;
		// INPUTS: 0-X0, 1-Y0, 2-Angle0, 3-dX, 4-dY, 5-dAngle, 6-Radius, 7-Shell, 8-Filled, 9-Bonds, //// 10-Left, 11-Top, 12-Right, 13-bottom, 14-Groups, 15-Run, 16-Energy
		// OUTPUTS:  0-X0, 1-Y0, 2-Angle, 3-dX, 4-dY, 5-dAngle, 6-Bonds
		nC->setLeftLabels(3, L"X0", L"Y0", L"Angle0", L"dX", L"dY", L"dAngle", L"Radius", L"Shell", L"Filled", L"Bonds", L"Left", L"Top", L"Right", L"Bottom", L"Groups", L"Run", L"Energy");
		nC->setRightLabels(3, L"X", L"Y", L"Angle", L"dX", L"dY", L"dAngle", L"Bonds");
		nC->width = 50;
		nC->setMemory(0, false); // to detect Run edge
		nC->setMemory(1, 0); // last clock for time based
		nC->setSetting(0, 0); // maybe auto/manual someday
		return nC; }
	if(z(name, L"BellJar2"))  {
		nC = new Component(L"BellJar2", L"Bell Jar 2", methodBellJar2_, null, squareShape, 14, 1, 7, 1);
		nC->timeBased = true;
		nC->dialogList = dialogBellJar2ButtonList;
		// INPUTS: 0-X0, 1-Y0, 2-Angle0, 3-dX, 4-dY, 5-dAngle, 6-Radius, 7-Bonds, 8-Bond Type, //// 9-Left, 10-Top, 11-Right, 12-bottom, 13-Run
		// OUTPUTS:  0-X0, 1-Y0, 2-Angle, 3-dX, 4-dY, 5-dAngle, 6-Bond Status
		nC->setLeftLabels(3, L"X0", L"Y0", L"Angle0", L"dX", L"dY", L"dAngle", L"Radius", L"Bonds", L"Bond Type", L"Left", L"Top", L"Right", L"Bottom", L"Run");
		nC->setRightLabels(3, L"X", L"Y", L"Angle", L"dX", L"dY", L"dAngle", L"Bond Status");
		nC->width = 50;
		nC->setMemory(0, 0); // to detect Reset edge
		nC->setMemory(1, 0); // last clock for time base
		nC->setMemory(2, 1); // first time
		nC->setSetting(0, 10); // distanceTolerance
		nC->setSetting(1, 0.1); // angleTolerance
		nC->setSetting(2, 0); // A_bond_a
		return nC; }

	if(z(name, L"MatSums"))  {
		nC = new Component(L"MatSums", L"Matrix Sums", methodMatRowColumnSums_, null, squareShape, 1, 1, 2, 1);
		//nC->crank = crankSplit_;// same because it only affects right terminals
		//nC->dial = dialSplit_;// same because it only affects right terminals
		nC->setSetting(0, 2); // outputs
		//nC->setSetting(1, 0); // mode: 0) Split on X columns, 1) split on Y rows,  2) split on Z layers someday >
		//nC->dialogList = dialogMatSplitButtonList;
		nC->setLeftLabels(3, L"Matrix");
		nC->setRightLabels(3, L"Column Sum", L"Row Sum");
		nC->width = 50;
		//nC->onLoad = onLoadMatrixSplit_;
		return nC; }

	
	if(z(name, L"Forge"))  {
		nC = new Component(L"Forge", L"Surface Forge", methodSurfaceForge_, null, squareShape, 9, 1, 3, 1);
		nC->setLeftLabels(3, L"Surface X Size", L"Surface Y Size", L"Initialize Surface", L"Tool X Position", L"Tool Y Position", L"Tool Radius", L"Tool Sharpness", L"Tool Force", L"Trigger Tool");
		nC->setRightLabels(3, L"Matrix", L"X size", L"Y size");
		nC->width = 65;
		nC->setMemory(0, false);  // initialize edge
		nC->setMemory(1, false);  // trigger tool edge
		nC->setMemory(2, 0);  // xSize
		nC->setMemory(3, 0);  // ySize
		nC->output[1].value[0] = 0; // xSize
		nC->outputValue[2][0] = 0; // ySize
		return nC; }
	if(z(name, L"SurfLight"))  {
		nC = new Component(L"SurfLight", L"Surface Light", methodSurfaceLight_, null, squareShape, 11, 1, 5, 1);
		nC->setLeftLabels(3, L"Surface X size", L"Surface Y size", L"Initialize surface", L"X position", L"Y position", L"Radius", L"Sharpness", L"Red", L"Green", L"Blue", L"Trigger Light");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue", L"X size", L"Y size");
		nC->width = 65;
		nC->setMemory(0, false);  // initialize edge
		nC->setMemory(1, false);  // trigger tool edge
		nC->setMemory(2, 0);  // xSize
		nC->setMemory(3, 0);  // ySize
		nC->outputValue[3][0] = 0; // xSize
		nC->outputValue[4][0] = 0; // ySize
		return nC; }
	if(z(name, L"SurfPaint"))  {
		nC = new Component(L"SurfPaint", L"Surface Paint", methodSurfacePaint_, null, squareShape, 11, 1, 5, 1);
		nC->setLeftLabels(3, L"Surface X size", L"Surface Y size", L"Initialize surface", L"X position", L"Y position", L"Radius", L"Sharpness", L"Red", L"Green", L"Blue", L"Trigger");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue", L"X size", L"Y size");
		nC->width = 65;
		nC->setMemory(0, false);  // initialize edge
		nC->setMemory(1, false);  // trigger tool edge
		nC->setMemory(2, 0);  // xSize
		nC->setMemory(3, 0);  // ySize
		nC->outputValue[3][0] = 0; // xSize
		nC->outputValue[4][0] = 0; // ySize
		return nC; }
	
		if(z(name, L"StringSim"))  {
		nC = new Component(L"StringSim", L"String Simulation", methodStringSimulation_, null, squareShape, 7, 1, 1, 1);
		nC->dialogList = dialogStringSimButtonList;
		nC->onLoad = onLoadStringSim_;
		nC->setLeftLabels(3, L"Size", L"Preset A", L"Preset B", L"Load Now", L"Start Mode", L"End Mode", L"Enable");
		nC->bufferA = [0];
		nC->bufferB = [0];
		nC->setMemory(0, 0); // for single step trigger
		nC->setMemory(1, 0); // last clock for time based
		nC->setMemory(2, 0); // for load trigger
		nC->setSetting(0, 1); // time based is true
		nC->timeBased = true; // pin seven changes: from Enable when true, to Trigger when false
		return nC; }
	//if(z(name, L"Match"))		{return new Match_Component(); }

	if(z(name, L"MatDiag"))  {
		nC = new Component(L"MatDiag", L"Diagonal Matrix", methodDiagonalMatrix_, null, 1, 1);
		nC->setLeftLabels(3, L"Size");
		return nC; }

	if(z(name, L"Attr"))  {
		nC = new Component(L"Attr", L"Attribution", methodAttribution_, null, 1, 1);
		nC->setLeftLabels(3, L"Trigger");
		nC->setMemory(0, 0;
		return nC; }
	if(z(name, L"SplitValue"))  {
		nC = new Component(L"SplitValue", L"Split On Value", methodSplitOnValue_, null, squareShape, 2, 1, 3, 1);
		nC->setLeftLabels(3, L"Array", L"Value");
		nC->setRightLabels(3, L"Group 0", L"Value 1", L"Group 1");
		nC->width = 35;
		nC->setSetting(0, 2); // number of groups
		return nC; }
	
	if(z(name, L"MatchPatt"))  {
		nC = new Component(L"MatchPatt", L"Match Pattern", methodMatchPattern_, null, squareShape, 2, 1, 2, 1);
		nC->setLeftLabels(3, L"Array", L"Goal");
		nC->setRightLabels(3, L"Match", L"spare");
		nC->width = 35;
		return nC; }
	if(z(name, L"ValueMerge"))  {
		nC = new Component(L"ValueMerge", L"Value Merge", methodValueMerge_, null, squareShape, 3, 1, 3, 1);
		nC->dialogList = dialogValueMergeButtonList;
		nC->onLoad = onLoadValueMerge_;
		nC->setLeftLabels(3, L"Array", L"0", L"1");
		nC->setRightLabels(3, L"Array", L"0", L"1");
		nC->setSetting(0, 2); // function and 1 parameter
		return nC; }
	if(z(name, L"ValueUnmerge"))  {
		nC = new Component(L"ValueUnmerge", L"Value Unmerge", methodValueUnmerge_, null, squareShape, 1, 1, 2, 1);
		nC->dialogList = dialogValueUnmergeButtonList;
		nC->onLoad = onLoadValueUnmerge_;
		nC->setLeftLabels(3, L"Array");
		nC->setRightLabels(3, L"0", L"1");
		nC->setSetting(0, 2); // number of outputs
		return nC; }
	
	if(z(name, L"ValueEnclosed"))  {
		nC = new Component(L"ValueEnclosed", L"Value Enclosed", methodValueEnclosed_, null, squareShape, 3, 1, 5, 1);
		nC->setLeftLabels(3, L"Array", L"Left Value", L"Right Value");
		nC->setRightLabels(3, L"Left Array", L"Left Value", L"Enclosed Array", L"Right Value", L"Right Array");
		nC->width = 65;
		return nC; }
	if(z(name, L"Life"))  {
		nC = new Component(L"Life", L"Life", methodLife_, null, squareShape, 4, 1, 1, 1);
		nC->setLeftLabels(3, L"Array", L"Load", L"Clear", L"Step");
		nC->setMemory(0, 0;
		nC->setMemory(1, 0;
		return nC; }

	if(z(name, L"ValueTerms2")) { return new ValueTerms2_Component(); }
	if(z(name, L"Replace"))  {
		nC = new Component(L"Replace", L"Replace String", methodReplace_, null, 3, 1);
		nC->dialogList = dialogReplaceButtonList;
		nC->setLeftLabels(3, L"Array", L"Find", L"Replace");
		nC->setSetting(0, 0); // Regex Find enable
		nC->setSetting(1, 0); // replace all
		return nC; }
	if(z(name, L"Note"))  {
		nC = new Component(L"Note", L"Note", methodNull_, null, squareShape, 0, 0, 0, 0);
		nC->dialogList = dialogNoteButtonList;
		nC->setSetting(0, nC->width);
		nC->setSetting(1, nC->height);
		nC->setSetting(2, parseInt('FFEEEE', 16));
		nC->dial = dialNote_;
		nC->crank = crankNote_;
		nC->onLoad = onLoadNote_;
		nC->annotation = true;
		nC->width = COMPONENT_WIDTH;
		return nC; }
	if(z(name, L"LessonNote"))  {
		nC = new Component(L"LessonNote", L"", methodNull_, null, squareShape, 0, 0, 0, 0);
		nC->dialogList = dialogNoteButtonList;
		nC->setSetting(0, nC->width);
		nC->setSetting(1, nC->height);
		nC->setSetting(2, parseInt('FFEEEE', 16));
		nC->setSetting(3, -1; // the only step to show this note, -1 until assigned a step
		nC->dial = dialNote_;
		nC->crank = crankNote_;
		nC->onLoad = onLoadNote_;
		nC->lessonStep = true;
		nC->width = COMPONENT_WIDTH;
		return nC; }
	if(z(name, L"LessonArrow"))  {
		nC = new Component(L"LessonArrow", L"", methodNull_, null, squareShape, 0, 0, 0, 0);
		nC->dialogList = dialogNoteButtonList;
		nC->setSetting(0, nC->width);
		nC->setSetting(1, nC->height);
		nC->setSetting(2, parseInt('FFEEEE', 16));
		nC->setSetting(3, -1); // the only step to show this note, -1 until assigned a step
		nC->setSetting(4, 50); // x offset to arrow head
		nC->setSetting(5, 60); // y offset to arrow head
		nC->dial = dialLessonArrow_;
		nC->crank = crankLessonArrow_;
		nC->onLoad = onLoadNote_;
		nC->lessonStep = true;
		nC->setMemory(0, 0;
		nC->width = COMPONENT_WIDTH;
		return nC; }

		// Image Components ----------------------
	if(z(name, L"ImClTest"))  {
		nC = new Component(L"ImClTest", L"Test Image", methodImageColorTest_, null, squareShape, 0, 1, 3, 1);
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		return nC; }
	if(z(name, L"ImClProj"))  {
		nC = new Component(L"ImClProj", L"Color Projector", methodNull_, null, squareShape, 6, 1, 0, 1);
		nC->method2d = methodImageColorProj_;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue", L"Alpha", L"X", L"Y");
		return nC; }
	if(z(name, L"ImGrProj"))  {
		nC = new Component(L"ImGrProj", L"Gray Projector", methodNull_, null, squareShape, 4, 1, 0, 1);
		nC->method2d = methodImageGrayProj_;
		nC->setLeftLabels(3, L"Gray", L"Alpha", L"X", L"Y");
		return nC; }
	if(z(name, L"ImIdxProj"))  {
		nC = new Component(L"ImIdxProj", L"Index Projector", methodNull_, null, squareShape, 3, 1, 0, 1);
		nC->method2d = methodImageIndexProj_;
		nC->setLeftLabels(3, L"Index", L"X", L"Y");
		return nC; }

	if(z(name, L"ImClLumen"))  {
		nC = new Component(L"ImClLumen", L"Color Lumenosity", methodImageColorLumen_, null, 3, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue");
		nC->setRightLabels(3, L"Gray");
		return nC; }
	if(z(name, L"ImClAvg"))  {
		nC = new Component(L"ImClAvg", L"Color Average", methodImageColorAvg_, null, 3, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue");
		nC->setRightLabels(3, L"Gray");
		return nC; }
	if(z(name, L"ImClLight"))  {
		nC = new Component(L"ImClAvg", L"Color Lightness", methodImageColorLight_, null, 3, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue");
		nC->setRightLabels(3, L"Gray");
		return nC; }
	if(z(name, L"ImClInvert"))  {
		nC = new Component(L"ImClInvert", L"Color Invert", methodImageColorInvert_, null, squareShape, 3, 1, 3, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		return nC; }
	if(z(name, L"ImClColor"))  {
		nC = new Component(L"ImClColor", L"Colorize", methodImColorColorize_, null, squareShape, 6, 1, 3, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue", L"f(red)", L"f(green)", L"f(blue)");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		return nC; }
	
	if(z(name, L"ImClFlip"))  {
		nC = new Component(L"ImClFlip", L"Color Flip", methodImageColorFlip_, null, squareShape, 5, 1, 3, 1);
		nC->width = 50;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue", L"Horizontal", L"Vertical");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		return nC; }
	if(z(name, L"ImGetClPix"))  {
		nC = new Component(L"ImGetClPix", L"Get Color Pixel", methodImageGetColorPixel_, null, squareShape, 5, 1, 3, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue", L"X", L"Y");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		return nC; }

	if(z(name, L"ImPosSync"))  {
		nC = new Component(L"ImPosSync", L"Position Sync Cam to Canvas", methodImPosSyncCam2Can_, null, squareShape, 3, 1, 2, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"X", L"Y", L"Composite");
		nC->setRightLabels(3, L"X", L"Y");
		nC->method2d = methodImPosSyncCam2Can_; // so it will update when the canvas 2 size changes
		return nC; }
	if(z(name, L"ImSizSync"))  {
		nC = new Component(L"ImSizSync", L"Size Sync Cam to Canvas", methodImSizSyncCam2Can_, null, squareShape, 3, 1, 2, 1);
		nC->width = 50;
		nC->setLeftLabels(3, L"Width", L"Height", L"Composite");
		nC->setRightLabels(3, L"Width", L"Height");
		nC->method2d = methodImSizSyncCam2Can_; // so it will update when the canvas 2 size changes
		return nC; }
	if(z(name, L"CanPosSync"))  {
		nC = new Component(L"CanPosSync", L"Position Sync Canvas to Cam", methodImPosSyncCan2Cam_, null, squareShape, 3, 1, 2, 1);
		nC->width = 40;
		nC->setLeftLabels(3, L"X", L"Y", L"Composite");
		nC->setRightLabels(3, L"X", L"Y");
		nC->method2d = methodImPosSyncCan2Cam_; // so it will update when the canvas 2 size changes
		return nC; }
	if(z(name, L"CanSizSync"))  {
		nC = new Component(L"CanSizSync", L"Size Sync Canvas to Cam", methodImSizSyncCan2Cam_, null, squareShape, 3, 1, 2, 1);
		nC->width = 50;
		nC->setLeftLabels(3, L"Width", L"Height", L"Composite");
		nC->setRightLabels(3, L"Width", L"Height");
		nC->method2d = methodImSizSyncCan2Cam_; // so it will update when the canvas 2 size changes
		return nC; }
	if(z(name, L"Capture"))  {
		nC = new Component(L"Capture", L"Screen Capture", methodCapture_, null, squareShape, 5, 1, 4, 1);
		nC->width = 40;
		nC->setMemory(0, 0;
		nC->setLeftLabels(3, L"X", L"Y", L"Width", L"Height", L"Enable");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue", L"Alpha");
		return nC; }
	if(z(name, L"Xerox"))  {
		nC = new Component(L"Xerox", L"Xerox", methodXerox_, null, squareShape, 8, 1, 0, 1);
		nC->method2d = methodXerox_;
		nC->width = 40;
		nC->setMemory(0, 0;
		nC->setLeftLabels(3, L"Source X", L"Source Y", L"Width", L"Height", L"Destination X", L"Destination Y", L"Magnification", L"Enable");
		return nC; }
	
	if(z(name, L"ImGrAxis"))  {
		nC = new Component(L"ImGrAxis", L"Gray Axis", methodImageGrayAxis_, null, squareShape, 1, 1, 3, 1);
		nC->setLeftLabels(3, L"Gray");
		nC->setRightLabels(3, L"Major", L"Minor", L"Eccentricity");
		nC->width = 50;
		return nC; }

	if(z(name, L"ImClFade"))  {
		nC = new Component(L"ImClFade", L"Color Fader", methodImageColorMixer_, funcImageColorFade_, squareShape, 7, 1, 3, 1);
		nC->setLeftLabels(3, L"Red 0", L"Green 0", L"Blue 0", L"Red 1", L"Green 1", L"Blue 1", L"Control");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		nC->width = 40;
		return nC; }
	
	if(z(name, L"ImClFilter"))  {
		nC = new Component(L"ImClFilter", L"Color Filter", methodImageColorFilter_, null, squareShape, 4, 1, 3, 1);
		nC->setLeftLabels(3, L"Red", L"Green", L"Blue", L"Filter");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		nC->width = 40;
		return nC; }
	if(z(name, L"GetText"))  {
		nC = new Component(L"GetText", L"Get Text", methodGetText_, null, 1, 1);
		nC->dialogList = dialogGetTextButtonList;
		nC->onLoad = onLoadGetText_;
		nC->setLiteral(0, '?';
		nC->setLeftLabels('?');
		// nC->width = calcBuss2Width(nC);
		nC->setSetting(0, nC->width);
		return nC; }
	if(z(name, L"GetValue"))  {
		nC = new Component(L"GetValue", L"Get Value", methodCommonGet_, null, 1, 1);
		nC->dialogList = dialogGetValueButtonList;
		nC->onLoad = onLoadGet_;
		nC->setLiteral(0, '';
		nC->setRightLabels('?');
		nC->setLeftLabels('?');
		nC->width = calcBuss2Width(nC);
		nC->setSetting(0, nC->width);
		nC->setSetting(1, 0); // detent
		nC->setElement(0, 0);// default value[0]
		return nC; }
	if(z(name, L"GetValueGrp"))  {
		nC = new Component(L"GetValueGrp", L"Get Value Group", methodGetValueGroup_, null, squareShape, 3, 1, 3, 1);
		nC->dialogList = dialogGetValueGroupButtonList;
		nC->onLoad = onLoadGetValueGroup_;
		nC->setLiteral(0, "X";
		nC->setLiteral(1, "Y";
		nC->setRightLabels(3, L"Count", L"X", L"Y");
		nC->setLeftLabels(3, L"Names", L"?", L"?");
		nC->width = calcBuss2Width(nC);
		nC->setSetting(0, nC->width);
		nC->setSetting(1, 0); // detent
		return nC; }
	if(z(name, L"GetBool"))  {
		nC = new Component(L"GetBool", L"Get Bool", methodCommonGet_, null, 1, 1);
		nC->dialogList = dialogGetBoolButtonList;
		nC->onLoad = onLoadGet_;
		nC->setLiteral(0, '';
		nC->setRightLabels('?');
		nC->setLeftLabels('?');
		nC->width = calcBuss2Width(nC);
		nC->setSetting(0, nC->width);
		nC->setElement(0, 0);// default value[0]
		return nC; }
	if(z(name, L"Submit"))  {
		nC = new Component(L"Submit", L"Submit", methodSubmit_, null, squareShape, 2, 1, 0, 1);
		nC->setMemory(0, 0; // trigger edge
		nC->setLeftLabels(3, L"Score", L"Trigger");
		return nC; }
	if(z(name, L"ProgQuiz"))  {
		nC = new Component(L"ProgQuiz", L"Program Quiz", methodNull_, null, squareShape, 0, 1, 1, 1);
		if (programQuizButton.isSelected) // always starts with current setting
			nC->output[0].value[0] = 1;
		else
			nC->output[0].value[0] = 0;
		return nC; }
	if(z(name, L"GetConst"))  {
		nC = new Component(L"GetConst", L"Get Constant", methodGetConstant_, null, 1, 1);
		nC->dialogList = dialogGetConstantButtonList;
		nC->onLoad = onLoadGetConstant_;
		nC->setLiteral(0, '';
		nC->setRightLabels('');
		nC->width = calcBuss2Width(nC);
		nC->setSetting(0, nC->width);
		return nC; }
	
	if(z(name, L"Transmit"))  {
		nC = new Component(L"Transmit", L"Transmitter", methodTransmit_, null, squareShape, 3, 1, 0, 0);
		nC->dialogList = dialogTransmitButtonList;
		nC->onLoad = onLoadTransmit_;
		nC->setLeftLabels(3, L"Channel", L"Data", L"Inhibit");
		nC->setSetting(0, 0); // inhibit
		nC->setMemory(0, 0);   // false trigger memory
		return nC; }
	if(z(name, L"Receive"))  {
		nC = new Component(L"Receive", L"Receiver", methodReceive_, null, 1, 1);
		nC->setLeftLabels(3, L"Channel");
		nC->timeBased = true;
		return nC; }
	if(z(name, L"Publish"))  {
		nC = new Component(L"Publish", L"Publish", methodPublish_, null, squareShape, 3, 1, 0, 0);
		nC->dialogList = dialogTransmitButtonList; // same as transmitter
		nC->onLoad = onLoadTransmit_; // same as transmitter
		nC->setLeftLabels(3, L"Channel", L"Data", L"Inhibit");
		nC->timeBased = true;
		nC->setSetting(0, 0); // inhibit
		nC->setMemory(0, 0);   // trigger memory
		nC->setMemory(0, 1);   // system time
		return nC; }
	if(z(name, L"Subscribe"))  {
		nC = new Component(L"Subscribe", L"Subscribe", methodSubscribe_, null, 1, 1);
		nC->setLeftLabels(3, L"Channel");
		nC->timeBased = true;
		nC->setMemory(0, 0); // system time
		return nC; }
	if(z(name, L"Preset"))  {
		nC = new Component(L"Preset", L"Preset", methodPreset_, null, squareShape, 1, 1, 0, 0);
		nC->dialogList = dialogPresetButtonList;
		nC->dial = dialPreset_;
		nC->crank = crankPreset_;
		nC->onLoad = onLoadPreset_;
		nC->setSetting(0, 0); // number of presets - saved in literal list
		nC->setSetting(1, nC->width);
		nC->setSetting(2, nC->height);
		nC->setSetting(3, 0);  // lock
		nC->width = COMPONENT_WIDTH;
		return nC; }
	if(z(name, L"Remote"))  {
		nC = new Component(L"Remote", L"Remote", methodRemote_, null, squareShape, 1, 1, 0, 0);
		nC->dialogList = dialogRemoteButtonList;
		nC->dial = dialRemote_;
		nC->crank = crankRemote_;
		nC->onLoad = onLoadRemote_;
		nC->setSetting(0, 0); // number of remotes - saved in literal list
		nC->setSetting(1, nC->width);
		nC->setSetting(2, nC->height); // not used
		nC->setSetting(3, 0); // not used
		nC->width = COMPONENT_WIDTH;
		return nC; }
	if(z(name, L"Key"))  {
		nC = new Component(L"Key", L"Key", methodNull_, null, squareShape, 1, 1, 5, 1);
		nC->setSetting(0, 0); // key value
		nC->setLeftLabels(3, L"Code");
		nC->setRightLabels(3, L"Key down", L"Shift", L"Alt", L"Ctrl", L"Meta");
		return nC; }

	

	if(z(name, L"RtFilter"))  {
		nC = new Component(L"RtFilter", L"RT Filter", methodRtFilter_, null, 3, 1);
		nC->timeBased = true;
		nC->setLeftLabels(3, L"Data Stream", L"Input Coefficients", L"Output Coefficients");
		nC->outputValueDimensions[0][0] = 1;
		nC->setMemory(0, 0;
		nC->memory2[0, 0;
		nC->lastClock = -1;
		nC->width = 50;
		return nC; }
	if(z(name, L"KalFilter"))  {
		nC = new Component(L"KalFilter", L"Kalman Filter", methodKalmanFilter_, null, squareShape, 7, 1, 1, 1);
		nC->setLeftLabels(3, L"Process Noise (R)", L"Measurement Noise (Q)", L"State Vector (A)", L"Control Vector (B)", L"Measurement Vector (C)", L"Measurement (z)", L"Control (u)");
		//nC->setRightLabels(3, L"Out");
		nC->outputValueDimensions[0][0] = 1;
		nC->width = 60;
		return nC; }
	if(z(name, L"Delay"))  {
		nC = new Component(L"Delay", L"Delay", methodDelay_, null, squareShape, 4, 1, 1, 1);
		nC->setLeftLabels(3, L"Data", L"Trigger", L"Delay", L"Reset");
		nC->setRightLabels(3, L"Out");
		nC->memory = []; // clear it
		nC->oldTrigger = 0; // trigger memory
		nC->outputValueDimensions[0][0] = 1;
		return nC; }

	if(z(name, L"FrChCo"))  {
		nC = new Component(L"FrChCo", L"From UTF-8", methodFromCharCode_, null, 1, 1);
		nC->height = MIN_COMPONENT_HEIGHT;
		return nC; }
	if(z(name, L"ToChCo"))  {
		nC = new Component(L"ToChCo", L"To UTF-8", methodToCharCode_, null, 1, 1);
		nC->height = MIN_COMPONENT_HEIGHT;
		return nC; }
	if(z(name, L"DecodeUTF-8"))  {
		nC = new Component(L"DecodeUTF-8", L"Decode UTF-8", methodDecodeUTF8_, null, 1, 1);
		nC->height = MIN_COMPONENT_HEIGHT;
		return nC; }
	if(z(name, L"BinSear"))  {
		nC = new Component(L"BinSear", L"Binary Search", methodBinarySearch_, null, squareShape, 6, 1, 3, 1);
		nC->setLeftLabels(3, L"Y", L"Y Tolerance", L"X Minimum", L"X Maximum", L"Step", L"Reset");
		nC->setRightLabels(3, L"X", L"Done", L"Error");
		nC->width = 50;
		nC->setMemory(0, 0); // step input memory
		nC->setMemory(1, 0); // reset input memory
		nC->setMemory(2, 0); // phase memory
		return nC; }
	
	
	if(z(name, L"DayOfWeek"))  {
		nC = new Component(L"DayOfWeek", L"Day Of Week", methodDayOfWeek_, null, 3, 1);
		nC->setLeftLabels(3, L"Year", L"Month", L"Day");
		return nC; }
	if(z(name, L"DaysInMon"))  {
		nC = new Component(L"DaysInMon", L"Days In Month", methodDaysInMonth_, null, 2, 1);
		nC->setLeftLabels(3, L"Year", L"Month");
		return nC; }

	
	if(z(name, L"Test"))  {
		nC = new Component(L"Test", L"Test", methodTest_, null, squareShape, 1, 0, 1, 0);
		nC->dialogList = dialogTestButtonList;
		nC->dial = dialTest_;  // the Pass/Fail light
		nC->crank = crankTest_; // the test button
		nC->setSetting(0, 0); // controls - saved in literal list
		nC->setSetting(1, 0);  // displays - saved after controls in literal list
		nC->setSetting(2, 0);  // tolerance
		nC->setSetting(3, 0);  // lock
		nC->setMemory(0, 0);  // button state (for edge detection)
		nC->setMemory(1, 0);  // 0=untested, 1=fail, 2=Pass
		nC->setMemory(2, 0);  // input state (for edge detection)
		nC->output[0].value[0] = 0;
		nC->outputValueDimensions[0] = [1];
		nC->width = COMPONENT_WIDTH;
		return nC; }

	
	if(z(name, L"Warning"))  {
		nC = new Component(L"Warning", L"Warning", methodNull_, null, squareShape, 0, 0, 4, 1);
		nC->setRightLabels(3, L"Zero", L"undefined", L"infinity", L"NaN");
		nC->output[0].value[0] = 0;
		nC->output[1].value[0] = undefined;
		nC->outputValue[2][0] = 1 / 0;
		nC->outputValue[3][0] = 'j' / 12;
		for (var kk = 0; kk < 4; kk++)
			nC->outputValueDimensions[kk][0] = 1;
		return nC; }

	if(z(name, L"ToRGB"))  {
		nC = new Component(L"ToRGB", L"To RGB", methodToRGB_, null, squareShape, 3, 1, 3, 1);
		nC->setLeftLabels(3, L"Hue", L"Saturation", L"Value");
		nC->setRightLabels(3, L"Red", L"Green", L"Blue");
		nC->width = 50;
		return nC; }

	default:
		return new Component(L"null', name, methodNull_, null, squareShape, 0, 0, 0, 0);

*/
	
	return NULL;

}


void Component::draw(Sheet* pg, HDC hDC) {

	// draw body
	switch (shapePath) {
	case triangleShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x - width, pg->cYo + y - height, NULL);
		LineTo(hDC, pg->cXo + x + width, pg->cYo + y);
		LineTo(hDC, pg->cXo + x - width, pg->cYo + y + height);
		LineTo(hDC, pg->cXo + x - width, pg->cYo + y - height);
		EndPath(hDC);
		break;
	case squareShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x - width, pg->cYo + y - height, NULL);
		LineTo(hDC, pg->cXo + x + width, pg->cYo + y - height);
		LineTo(hDC, pg->cXo + x + width, pg->cYo + y + height);
		LineTo(hDC, pg->cXo + x - width, pg->cYo + y + height);
		LineTo(hDC, pg->cXo + x - width, pg->cYo + y - height);
		EndPath(hDC);
		break;
	case roundShape:
		BeginPath(hDC);
		Ellipse(hDC, pg->cXo + x - width, pg->cYo + y - width,
			pg->cXo + x + width, pg->cYo + y + width);
		EndPath(hDC);
		break;
	case bussInShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x - width, pg->cYo + y, NULL);
		LineTo(hDC, pg->cXo + x - width + 7, pg->cYo + y + height);
		LineTo(hDC, pg->cXo + x + width, pg->cYo + y + height);
		LineTo(hDC, pg->cXo + x + width, pg->cYo + y - height);
		LineTo(hDC, pg->cXo + x - width + 7, pg->cYo + y - height);
		LineTo(hDC, pg->cXo + x - width, pg->cYo + y);
		EndPath(hDC);
		break;

	case bussOutShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x + width,      pg->cYo + y, NULL);
		LineTo(hDC,   pg->cXo + x + width - 7, pg->cYo + y + height);
		LineTo(hDC,   pg->cXo + x - width,      pg->cYo + y + height);
		LineTo(hDC,   pg->cXo + x - width,      pg->cYo + y - height);
		LineTo(hDC,   pg->cXo + x + width - 7, pg->cYo + y - height);
		LineTo(hDC,   pg->cXo + x + width,      pg->cYo + y);
		EndPath(hDC);
		break;

	case buss2OutShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x + width,     pg->cYo + y, NULL);
		LineTo(hDC,   pg->cXo + x + width,     pg->cYo + y + height - 7);
		LineTo(hDC,   pg->cXo + x + width - 7, pg->cYo + y + height);
		LineTo(hDC,   pg->cXo + x - width,     pg->cYo + y + height);
		LineTo(hDC,   pg->cXo + x - width,     pg->cYo + y - height);
		LineTo(hDC,   pg->cXo + x + width - 7, pg->cYo + y - height);
		LineTo(hDC,   pg->cXo + x + width,     pg->cYo + y - height + 7);
		LineTo(hDC,   pg->cXo + x + width,     pg->cYo + y);
		EndPath(hDC);
		break;
	case buss2InShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x - width,     pg->cYo + y, NULL);
		LineTo(hDC,   pg->cXo + x - width,     pg->cYo + y + height - 7);
		LineTo(hDC,   pg->cXo + x - width + 7, pg->cYo + y + height);
		LineTo(hDC,   pg->cXo + x + width,     pg->cYo + y + height);
		LineTo(hDC,   pg->cXo + x + width,     pg->cYo + y - height);
		LineTo(hDC,   pg->cXo + x - width + 7, pg->cYo + y - height);
		LineTo(hDC,   pg->cXo + x - width,     pg->cYo + y - height + 7);
		LineTo(hDC,   pg->cXo + x - width,     pg->cYo + y);
		EndPath(hDC);
		break;

	case diamondShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x, pg->cYo + y - height, NULL);
		LineTo(hDC, pg->cXo + x - width, pg->cYo + y);
		LineTo(hDC, pg->cXo + x, pg->cYo + y + height);
		LineTo(hDC, pg->cXo + x + width, pg->cYo + y);
		LineTo(hDC, pg->cXo + x, pg->cYo + y - height);
		EndPath(hDC);
		break;

	case sheetShape:
		BeginPath(hDC);
		MoveToEx(hDC, pg->cXo + x + width,      pg->cYo + y - height + FOLD, NULL);
		LineTo(hDC, pg->cXo + x + width - FOLD, pg->cYo + y - height); // diagonal
		LineTo(hDC, pg->cXo + x + width - FOLD, pg->cYo + y - height + FOLD); // inner corner
		LineTo(hDC, pg->cXo + x + width,		pg->cYo + y - height + FOLD);
		LineTo(hDC, pg->cXo + x + width,		pg->cYo + y + height); // lower right
		LineTo(hDC, pg->cXo + x - width,		pg->cYo + y + height); // lower left
		LineTo(hDC, pg->cXo + x - width,		pg->cYo + y - height); // upper left
		LineTo(hDC, pg->cXo + x + width - FOLD, pg->cYo + y - height);
		EndPath(hDC);
		break;
	}

	bool fillIt = true;

	if (pg->timerID == 0) {
		if (timeBased)
			fillIt = false;
	}

	HBRUSH coloredBrush = NULL;
	if (fillIt) {
		coloredBrush = GetColorBrush(fillColor); //CreateSolidBrush
		SelectObject(hDC, coloredBrush);
	}
	else
		SelectObject(hDC, GetStockObject(LTGRAY_BRUSH));

	if( selected )
		SelectObject(hDC, GetColorPen(0, 2));
	else
		SelectObject(hDC, GetColorPen(0, 1));

	StrokeAndFillPath(hDC);

	// draw inputs
	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	int xx = x - width - 1 - TERMINAL_SIZE;
	int yy = y - (TERMINAL_STEP * (leftTerminalCount - 1) * leftTerminalStep) / 2;
	for (int i = 0; i < leftTerminalCount; i++) {
		
		Ellipse(hDC, pg->cXo + xx - TERMINAL_SIZE, pg->cYo + yy - TERMINAL_SIZE,
			         pg->cXo + xx + TERMINAL_SIZE, pg->cYo + yy + TERMINAL_SIZE);

		StrokePath(hDC);
		if (input[i].label.size()) 
			drawTextString(hDC, pg->cXo + xx + 10, pg->cYo + yy -5, 0, input[i].label);

		yy += TERMINAL_STEP * leftTerminalStep;
	}
	
	// draw outputs
	xx = x + width + 1 + TERMINAL_SIZE;
	yy = y - (TERMINAL_STEP * (rightTerminalCount - 1) * rightTerminalStep) / 2;
	for (int i = 0; i < rightTerminalCount; i++) {

		if (selected) {
			SelectObject(hDC, GetColorPen(0, 2));
		}
		else {
			// output line width thickness = scalar or array
			if (output[i].stringType) {
				if (output[i].stringValue.size() > 1)
					SelectObject(hDC, GetColorPen(0, ARRAY_THICK_LINE));
			}
			else {
				if (output[i].value.size() > 1)
					SelectObject(hDC, GetColorPen(0, ARRAY_THICK_LINE));
			}
		}
		Ellipse(hDC, pg->cXo + xx - TERMINAL_SIZE, pg->cYo + yy - TERMINAL_SIZE,
			pg->cXo + xx + TERMINAL_SIZE, pg->cYo + yy + TERMINAL_SIZE);

		StrokePath(hDC);
		SelectObject(hDC, GetColorPen(0, 1));

		if (output[i].label.size()) {
			int wid = calcStringWidthFont(hDC, output[i].label);
			drawTextString(hDC, pg->cXo + xx - 10 - wid, pg->cYo + yy - 5, 0, output[i].label);
		}

		yy += TERMINAL_STEP * rightTerminalStep;
	}
	
	if (selected)
		SelectObject(hDC, GetColorPen(0, 1));

	// label it
	//if (that.ghost)
	//	ctx.fillStyle = ghostColor; // gray lines
	//else
	//	ctx.fillStyle = '#000000';
	if (shapePath == triangleShape) {
		//int indexOfSpace = label.indexOf(' ');
		//if (indexOfSpace == -1) {
			//TextOut(hDC,        pg->cXo + x - width + 5, pg->cYo + y - 8, label, _tcslen(label) );
			drawTextLptStr(hDC, pg->cXo + x - width + 3, pg->cYo + y - 6, 0, label);
		//}
		//else {
		//	ctx.fillText(that.label.substr(0, indexOfSpace), pg->cXo + that.x - that.width + 5,
		//		pg->cYo + that.y - 2);
		//	ctx.fillText(that.label.substr(indexOfSpace + 1), pg->cXo + that.x - that.width + 5,
		//		pg->cYo + that.y + 8);
		//
		//}
		// add trig mode
		//if (that.onLoad == onLoadTrig_) {
		//	if (that.setting[0])
		//		ctx.fillText('Deg', pg->cXo + that.x + 8, pg->cYo + that.y + 25);
		//	else
		//		ctx.fillText('Rad', pg->cXo + that.x + 8, pg->cYo + that.y + 25);
		//}
	}
	else {
		int wid = calcStringWidthFont(hDC, label);
		drawTextLptStr(hDC, pg->cXo + x - wid/2, pg->cYo + y + height + 1, 0, label);
	}

	
	if (userLabel.size() > 0) {
		int deltaY = 4;
	
		if( z(type, L"Sld")       || 
			z(type, L"SldFac")    ||
			z(type, L"SldOff")    ||
			z(type, L"Sld2")      ||
			z(type, L"Sld2Fac")   ||
			z(type, L"Sld2Off")   ||
			z(type, L"Decade")    ||
			z(type, L"DecadeFac") ||
			z(type, L"DecadeOff") ||
			z(type, L"RackZ")     ||
			z(type, L"RackFacZ")  ||
			z(type, L"RackOffZ")  ||
			z(type, L"Geom")        ) 
			deltaY += 10;
		
		int wid = calcStringWidthFont(pg->hDC, userLabel);
		drawTextString(pg->hDC, pg->cXo + x - wid / 2,
			pg->cYo + y - height -9 - deltaY, 0, userLabel);
		
	}

	// draw the dial
	if (!ghost)
		//DialFunc(pg);
		dial(pg);

}



FunctionN_Component::FunctionN_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS)
{
	int method2(Sheet*); //inputRepeaterNew_
}
int FunctionN_Component::method2(Sheet*) {
	int len[12];
	int loops = 0;
	int j, k;
	double a[12];
	double out;
	bool changed = false;
	int inputCount = leftTerminalCount;

	if (inputCount > 12)
		return 0.0;


	// check inputs for matches and calculate dimensions
	int dimensionsPin = -1;
	for (j = 0; j < inputCount; j++) {
		len[j] = input[j].value.size();
		if (len[j] != 1) {
			if (loops == 0) {
				loops = len[j];
				dimensionsPin = j;
			}
			else if (len[j] != loops)
				return false; // need them to match
		}
	}
	if (loops == 0)
		loops = 1;

	for (k = 0; k < inputCount; k++) {
		if (input[k].stringType)
			a[k] = 0;
		else {
			if (input[k].value.size() == 0)
				a[k] = 0;
			else {
				a[k] = input[k].value[0];
				if (input[k].drivenByOutputTerminal == -1)
					a[k] = 0.0; // was undefined;
			}
		}
	}

	// remove extra output elements if necessary - WHAT ABOUT CHANGING THE DIMENSIONS TOO?
	if (output.size() > 0)
		if (output[0].value.size() > loops) {
			output[0].value.resize(loops);
			changed = true;
		}

	for (j = 0; j < loops; j++) {
		if (j > 0) {
			for (k = 0; k < inputCount; k++) {
				if (len[k] != 1) {
					if (input[k].stringType)
						a[k] = 0;
					else if (len[k])
						a[k] = input[k].value[j];
					else
						a[k] = 0;
				}
			}
		}

		out = innerMethodN(a);// <------------ here is where it happens

		if (j < output.size()) {
			if (out != output[0].value[j]) {
				output[0].value[j] = out;
				changed = true;
			}
		}
		else { // output is non existant - create it and set it
			setOutput(0, j, out);
			changed = true;
		}
	}

	copyDimensions(dimensionsPin, 0);
	return changed;
}


FunctionCN_Component::FunctionCN_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS)
{
	int method2(Sheet*); //inputRepeaterWithComp_
	void dial(Sheet*);
}

int FunctionCN_Component::method2(Sheet* pg) {
	int len[12];
	int loops = 0;
	int j, k;
	double a[12];
	double out;
	//  var mem;
	bool changed = false;
	int inputCount = leftTerminalCount;
	//var func = pg->dgrm.cL[i]->innerMethod;


	// check inputs for matches and calculate dimensions
	int dimensionsPin = -1;
	for (j = 0; j < inputCount; j++) {
		len[j] = input[j].value.size();
		if (len[j] == 0)
			return false;
		if (len[j] != 1) {
			if (loops == 0) {
				loops = len[j];
				dimensionsPin = j;
			}
			else if (len[j] != loops)
				return false; // need them to match
		}
	}
	if (loops == 0)
		loops = 1;

	for (k = 0; k < inputCount; k++) {
		if (input[k].stringType)
			a[k] = 0;
		else if (input[k].value.size() == 0)
			a[k] = 0;
		else
			a[k] = input[k].value[0];
	}

	if (output.size() > 0)
		if (output[0].value.size() > loops) {
			output[0].value.resize(loops);
			changed = true;
		}

	for (j = 0; j < loops; j++) {
		if (j > 0) {
			for (k = 0; k < inputCount; k++) {
				if (len[k] != 1) {
					if (input[k].stringType)
						a[k] = 0;
					else
						a[k] = input[k].value[j];
				}
			}
		}

		out = innerMethodCN(*this, a); // <++++++++++++++++++++++++

		if (rightTerminalCount) {
			if (j < output.size()) {
				if (out != output[0].value[j]) {
					output[0].value[j] = out;
					changed = true;
				}
			}
			else { // output is non existant - create it and set it
				setOutput(0, j, out);
				changed = true;
			}
		}
	}

	copyDimensions(dimensionsPin, 0);

	return changed;
}
void FunctionCN_Component::dial(Sheet* pg) {

	if (setting[0])
		drawTextLptStr(pg->hDC, pg->cXo + x + 10, pg->cYo + y + 15, 0, L"Deg");
	else
		drawTextLptStr(pg->hDC, pg->cXo + x + 10, pg->cYo + y + 15, 0, L"Rad");
}


FunctionNMO_Component::FunctionNMO_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS)
{
	int method2(Sheet*); //inputRepeaterWithOutput_
}

// same as above but with an output array
int FunctionNMO_Component::method2(Sheet* pg) { //inputRepeaterWithOutput_
	int len[12];
	int loops = 0;
	int j, k;
	double a[12];
	double out;
	bool changed = false;
	int inputCount = leftTerminalCount;

	if (inputCount > 12)
		return false;

	// check inputs for matches and calculate dimensions
	int dimensionsPin = -1;
	for (j = 0; j < inputCount; j++) {
		len[j] = input[j].value.size();
		if (len[j] == 0)
			return false;
		if (len[j] != 1) {
			if (loops == 0) {
				loops = len[j];
				dimensionsPin = j;
			}
			else if (len[j] != loops)
				return false; // need them to match
		}
	}
	if (loops == 0)
		loops = 1;

	for (k = 0; k < inputCount; k++) {
		if (input[k].stringType)
			a[k] = 0;
		else if (input[k].value.size() == 0)
			a[k] = 0;
		else{
			a[k] = input[k].value[0];
			if (input[k].drivenByOutputTerminal == -1)
				a[k] = 0.0; // was undefined;
		}
	}

	// remove extra output elements if necessary - WHAT ABOUT CHANGING THE DIMENSIONS TOO?
	if (output.size() > 0)
		if (output[0].value.size() > loops) {
			output[0].value.resize(loops);
			changed = true;
		}

	double oldOut;
	double memoryValue;

	for (j = 0; j < loops; j++) {
		if (j > 0) {
			for (k = 0; k < inputCount; k++) {
				if (len[k] != 1) {
					if (input[k].stringType)
						a[k] = 0;
					else
						a[k] = input[k].value[j];
				}
			}
		}

		if (j < output[0].value.size()) {
			oldOut = output[0].value[j];
		}
		else { // output is non existant - create it and set it
			output[0].value.resize(j+1);
			output[0].value[j] = 0.0;
			oldOut = 0.0;
			changed = true;
		}

		if (j < memory.size()) {
			memoryValue = memory[j];
		}
		else { // output is non existant - create it and set it
			setMemory(j, 0.0);
			memoryValue = 0.0;
		}
		out = innerMethodNMO(pg, a, &memoryValue, oldOut); // <------------ here is where it happens
		setMemory(j, memoryValue);

		if (j < output.size()) {
			if (out != output[0].value[j]) {
				output[0].value[j] = out;
				changed = true;
			}
		}
		else { // output is non existant - create it and set it
			setOutput(0, j, out);
			changed = true;
		}
	}

	copyDimensions(dimensionsPin, 0);
	return changed;
}


void drawAllComponents(Sheet* pg, HDC hDC) {
	int size = pg->dgrm.cL.size();
	for (int j = 0; j < size; j++)
		pg->dgrm.cL[j]->draw(pg, hDC);
}

void drawAllWires(Sheet* pg, HDC hDC) {
	int size = pg->dgrm.wL.size();
	for (int j = 0; j < size; j++)
		pg->dgrm.wL[j]->draw(pg);
}

int draggingSelectionPhase = 0; // 0 = not dragging, 1 = objects selected, 2 = ready for movement, 3 = ready to move more

int draggingSelectionStartX = 0;
int draggingSelectionStartY = 0;

void dragSelection() {
#if 0
	saveState();
	var i, j, k, w;

	var cSSX = (canvasSelectStartX / pg->cs) - pg->cXo;
	var cSSY = (canvasSelectStartY / pg->cs) - pg->cYo;

	var cSEX = (canvasSelectEndX / pg->cs) - pg->cXo;
	var cSEY = (canvasSelectEndY / pg->cs) - pg->cYo;

	// first make a list of the components to be dragged
	j = 0;
	componentsBeingDragged = [];
	for (i = 0; i < pg->dgrm.cL.length; i++) {
		if (pg->dgrm.cL[i]->ghost)
			continue;
		if (lockAnnotationButton.isSelected && pg->dgrm.cL[i]->annotation)
			continue;
		if ((pg->dgrm.cL[i]->x > cSSX) &&
			(pg->dgrm.cL[i]->x < cSEX) &&
			(pg->dgrm.cL[i]->y > cSSY) &&
			(pg->dgrm.cL[i]->y < cSEY)) {
			componentsBeingDragged[j++] = i;
			draggingSelectionPhase = 3; // objects selected
			//console.log( '33136 Phase = ' + draggingSelectionPhase);
		}
	}
	// next make a list of the wire nodes to be dragged
	j = 0;
	wiresBeingDragged = [];
	nodesBeingDragged = [];
	for (w = 0; w < pg->dgrm.wL.length; w++) {// this will include bends from wires with terminals outside the rectangle
		if (pg->dgrm.wL[w].ghost)
			continue;
		for (k = 0; k < pg->dgrm.wL[w].x.length; k++)
			if ((pg->dgrm.wL[w].x[k] > cSSX) &&
				(pg->dgrm.wL[w].x[k] < cSEX) &&
				(pg->dgrm.wL[w].y[k] > cSSY) &&
				(pg->dgrm.wL[w].y[k] < cSEY)) {
				wiresBeingDragged[j] = w;
				nodesBeingDragged[j++] = k;
				draggingSelectionPhase = 1; // objects selected
				//console.log( '33154 Phase = ' + draggingSelectionPhase);
			}
	}
#endif
}


void unselectAllItems(Sheet* pg) {

	int clen = pg->dgrm.cL.size();
	for (int i = 0; i < clen; i++)
		pg->dgrm.cL[i]->selected = false;
		
	int wlen = pg->dgrm.wL.size();
	for (int i = 0; i < wlen; i++)
		pg->dgrm.wL[i]->selected = false;
}


void innerDeleteComponent_(Sheet* pg, int i) {
	
	// is it a Sheet icon?
//	if (z(pg->dgrm.cL[i]->type, L"Sheet"))
	//	for (int j = 0; j < pg->dgrm.cL[i]->pg->dgrm.cL.size(); j++)
	//		innerDeleteComponent_(pg->dgrm.cL[i]->pg, j);  // dont forget to delete it ************************

}


void deleteComponent(Sheet* pg, int i) {
	innerDeleteComponent_(pg, i);

	//bool rebuild3dScene = false;
	//rebuild3dScene = true;
	delete pg->dgrm.cL[i];
	pg->dgrm.cL.erase(pg->dgrm.cL.begin() + i);

	//for (int j = i; j < size-1; j++)
	//	pg->dgrm.cL[j] = pg->dgrm.cL[j + 1];
	//pg->dgrm.cL.resize(size-1);
	//removeComponentFromLesson(i);
	//clearFindComponent();
	//if (!autoUpdateSheet*Button.isSelected)
		//debug_hover_depth = 0; // stop debugging Sheet*s

//	if(pg->threeDwindowHandle)
	//	pg->update3Dview = true; //should move this to the components that need it

	if (pg->dgrm.cL.size() == 0)
		pg->isDirty = false;
	else
		setVisibleScreenDirty("Deleted Component");// pg->isDirty = true;

	drawCanvas2();
}

void buttonBase_Delete_Action(Sheet* pg, int i) {

	bool isBuss = (pg->dgrm.cL[i]->type == L"Buss2In");
	deleteComponent(pg, i);
	//if (isBuss)
		//updateAllBuss2Outputs(pg);


	pg->activeDialogComponent = -1;
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//drawCanvas2();
}

void buttonBase_Details_Action(Sheet* pg, int i) {
	buildDetailsDialogButtons(pg, i);
}

// Duplicate selected items
void FrameBottomRightDuplicateGroup(Sheet* pg) {
	// duplicate the Components
	int clen = pg->dgrm.cL.size();
	for (int i = 0; i < clen; i++)
		if (pg->dgrm.cL[i]->selected) {
			buttonBase_Duplicate_Action(pg, i);
			pg->dgrm.cL[i]->selected = false;
		}
	// now select the new ones
	int clen2 = pg->dgrm.cL.size();
	for (int i = clen; i < clen2; i++)
		pg->dgrm.cL[i]->selected = true;

	// duplicate the Wires
	int wlen = pg->dgrm.wL.size();
	for (int i = 0; i < wlen; i++)
		if (pg->dgrm.wL[i]->selected) {
			Wire* newWire = new Wire(pg->dgrm.wL[i]);
			newWire->offset(COMPONENT_WIDTH, COMPONENT_HEIGHT);
			pg->dgrm.wL[i]->selected = false;
			pg->dgrm.wL.push_back(newWire);
		}

	drawCanvas();
}

// Delete Selected items
void FrameBottomRightDeleteGroup(Sheet* pg) {
	int len = pg->dgrm.cL.size();
	for (int ic = len - 1; ic >= 0; ic--) {
		if (pg->dgrm.cL[ic]->selected) {

			// delete all wires connected to the right terminals
			for (int it = 0; it < pg->dgrm.cL[ic]->rightTerminalCount; it++) {
				for (int wn = pg->dgrm.wL.size() - 1; wn >= 0; wn--) {
					for (int wt = 0; wt < 2; wt++) {
						if (checkComponentOutputToWire(&pg->dgrm, ic, it, wn, wt)) {
							pg->dgrm.wL.erase(pg->dgrm.wL.begin() + wn);
							break;
						}
					}
				}
			}
			// delete all wires connected to the left terminals
			for (int it = 0; it < pg->dgrm.cL[ic]->leftTerminalCount; it++) {
				for (int wn = pg->dgrm.wL.size() - 1; wn >= 0; wn--) {
					for (int wt = 0; wt < 2; wt++) {
						if (checkComponentInputToWire(&pg->dgrm, ic, it, wn, wt)) {
							pg->dgrm.wL.erase(pg->dgrm.wL.begin() + wn);
							break;
						}
					}
				}
			}

			// then the component
			buttonBase_Delete_Action(pg, ic);
		}
	}

	// delete wires with both ends in the selection rectangle
	len = pg->dgrm.wL.size();
	for (int iw = len - 1; iw >= 0; iw--) {
		if (pg->dgrm.wL[iw]->pointSelected[0] &&
			pg->dgrm.wL[iw]->pointSelected[1]) {
			pg->dgrm.wL.erase(pg->dgrm.wL.begin() + iw);
		}
	}
	drawCanvas();
}


void innerComponentCopy(Diagram* src_dgrm, Diagram* dst_dgrm, int isrc, int jdst, int dx, int dy) {

	dst_dgrm->cL[jdst] = buildComponent(page, src_dgrm->cL[isrc]->type, 0);

	dst_dgrm->cL[jdst]->onLoad(page);

	dst_dgrm->cL[jdst]->x = src_dgrm->cL[isrc]->x + dx;
	dst_dgrm->cL[jdst]->y = src_dgrm->cL[isrc]->y + dy;
	dst_dgrm->cL[jdst]->fillColor = src_dgrm->cL[isrc]->fillColor;
	dst_dgrm->cL[jdst]->rotation = src_dgrm->cL[isrc]->rotation;

	int n = src_dgrm->cL[isrc]->setting.size();
	dst_dgrm->cL[jdst]->setting.resize(n);
	for (int k = 0; k < n; k++)
		dst_dgrm->cL[jdst]->setting[k] = src_dgrm->cL[isrc]->setting[k];

	n = src_dgrm->cL[isrc]->literal.size();
	dst_dgrm->cL[jdst]->literal.resize(n);
	for (int k = 0; k < n; k++)
		dst_dgrm->cL[jdst]->literal[k] = src_dgrm->cL[isrc]->literal[k];

	n = src_dgrm->cL[isrc]->memory.size();
	dst_dgrm->cL[jdst]->memory.resize(n);
	for (int k = 0; k < n; k++)
		dst_dgrm->cL[jdst]->memory[k] = src_dgrm->cL[isrc]->memory[k];

	n = src_dgrm->cL[isrc]->element.size();
	dst_dgrm->cL[jdst]->element.resize(n);
	for (int k = 0; k < n; k++)
		dst_dgrm->cL[jdst]->element[k] = src_dgrm->cL[isrc]->element[k];
}
void buttonBase_Duplicate_Action(Sheet* pg, int i) {

	//if (pg->dgrm.cL[i]->type == L"3AmbLight")
	//	if (componentExists('3AmbLight')) {
	//		alert("You can only have 1 Ambient Light.");
	//		return;
	//	}

	int j = pg->dgrm.cL.size();
	pg->dgrm.cL.resize(j+1);

	innerComponentCopy(&pg->dgrm, &pg->dgrm, i, j, COMPONENT_WIDTH, COMPONENT_HEIGHT);

	pg->dgrm.cL[j]->onLoad(pg);

	pg->activeDialogComponent = -1;
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//drawCanvas2();
	
	setVisibleScreenDirty("Duplicated");// pg->isDirty = true;
}

void FrameBottomRightSubroutineGroup(Sheet* pg) {

	// find an empty subroutine
	int newSubroutine = -1;
	for (int i = pg->visibleSheet+1; i < isL.size(); i++)
		if ((isL[i]->dgrm.cL.size() == 0) && (isL[i]->dgrm.wL.size() == 0)) {
			newSubroutine = i;
			break;
		}
	if (newSubroutine == -1) { // not found, so create one
		InnerSheet* inner = new InnerSheet();
		isL.push_back(inner);
		newSubroutine = isL.size() - 1;
	}

	// copy selected components to the new subroutine
	int csize = 0;
	int sumX = 0;
	int sumY = 0;
	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		if (!pg->dgrm.cL[i]->selected) 
			continue;
		isL[newSubroutine]->dgrm.cL.resize(csize+1);
		innerComponentCopy(&pg->dgrm, &isL[newSubroutine]->dgrm, i, csize, 0, 0);
		csize++;
		sumX += pg->dgrm.cL[i]->x;
		sumY += pg->dgrm.cL[i]->y;
	}
	// calc the subroutine's icon position
	POS iconPosition;
	iconPosition.x = sumX / csize;
	iconPosition.y = sumY / csize;
	iconPosition.x += pg->cXo;
	iconPosition.y += pg->cYo;

	// copy wires with both ends selected to the subroutine
	int wlen = pg->dgrm.wL.size();
	int wsize = 0;
	for (int i = 0; i < wlen; i++) {
		if (!pg->dgrm.wL[i]->pointSelected[0])
			continue;
		if (!pg->dgrm.wL[i]->pointSelected[1])
			continue;
		isL[newSubroutine]->dgrm.wL.resize(wsize+1);
		
		Wire* newWire = new Wire(pg->dgrm.wL[i]);
		isL[newSubroutine]->dgrm.wL[wsize] = newWire;
		wsize++;
	}
	

	// scan all the output terminals first
	vector<int>outComponentX;
	vector<int>outComponentY;
	vector<vector<int>> outWireNumber;  // for each output, list the wire numbers connected to it
	vector<vector<int>> outWireTerminal;// for each output, list the wire terminals is connected
	for (int ic = 0; ic < pg->dgrm.cL.size(); ic++) {
		if (!pg->dgrm.cL[ic]->selected)// is the component selected
			continue; 
		for (int it = 0; it < pg->dgrm.cL[ic]->rightTerminalCount; it++) {
			bool newOutput = true;

			// find all the ingoing wires that are connected to this terminal
			for (int wn = 0; wn < wlen; wn++) {
				// wires with [0] outside, [1] inside
				if ((!pg->dgrm.wL[wn]->pointSelected[0]) && (pg->dgrm.wL[wn]->pointSelected[1])) {
					if (checkComponentOutputToWire(&pg->dgrm, ic, it, wn, 1)) {
						if (newOutput) {
							outWireNumber.resize(outWireNumber.size() + 1);
							outWireTerminal.resize(outWireTerminal.size() + 1);
							outComponentX.push_back(pg->dgrm.cL[ic]->rightTerminalX(it));
							outComponentY.push_back(pg->dgrm.cL[ic]->rightTerminalY(it));
							newOutput = false;
						}
						outWireNumber[outWireNumber.size()-1].push_back(wn);
						outWireTerminal[outWireTerminal.size()-1].push_back(1);
					}
				}

				// wires with [1] outside, [0] inside
				if ((!pg->dgrm.wL[wn]->pointSelected[1]) && (pg->dgrm.wL[wn]->pointSelected[0])) {
					if (checkComponentOutputToWire(&pg->dgrm, ic, it, wn, 0)) {
						if (newOutput) {
							outWireNumber.resize(outWireNumber.size() + 1);
							outWireTerminal.resize(outWireTerminal.size() + 1);
							outComponentX.push_back(pg->dgrm.cL[ic]->rightTerminalX(it));
							outComponentY.push_back(pg->dgrm.cL[ic]->rightTerminalY(it));
							newOutput = false;
						}
						outWireNumber[outWireNumber.size() - 1].push_back(wn);
						outWireTerminal[outWireTerminal.size() - 1].push_back(0);
					}
				}
			}
		}
	}


	// scan all the input terminals - second (skipping output wires)
	vector<int>inComponentX;
	vector<int>inComponentY;
	vector<vector<int>> inWireNumber;  // for each input, list the wire numbers connected to it
	vector<vector<int>> inWireTerminal;// for each input, list the wire terminals is connected
	for (int ic = 0; ic < pg->dgrm.cL.size(); ic++) {
		if (!pg->dgrm.cL[ic]->selected)
			continue;
		for (int it = 0; it < pg->dgrm.cL[ic]->leftTerminalCount; it++) {
			bool newInput = true;

			// find all the incomming wires that are connected to this terminal
			for (int wn = 0; wn < wlen; wn++) {
				bool skipIt = false;
				for (int k = 0; k < outWireNumber.size(); k++) {
					if (wn == outWireNumber[k][0]) {
						skipIt = true;
						break;
					}
				}
				if (skipIt)
					continue;
				// wires with zero outside, one inside
				if ((!pg->dgrm.wL[wn]->pointSelected[0]) && (pg->dgrm.wL[wn]->pointSelected[1])) {
					if (checkComponentInputToWire(&pg->dgrm, ic, it, wn, 1)) {
						if (newInput) {
							inWireNumber.resize(inWireNumber.size() + 1);
							inWireTerminal.resize(inWireTerminal.size() + 1);
							inComponentX.push_back(pg->dgrm.cL[ic]->leftTerminalX(it));
							inComponentY.push_back(pg->dgrm.cL[ic]->leftTerminalY(it));
							newInput = false;
						}
						inWireNumber[inWireNumber.size() - 1].push_back(wn);
						inWireTerminal[inWireTerminal.size() - 1].push_back(1);
					}
				}

				// wires with one outside, zero inside
				if ((!pg->dgrm.wL[wn]->pointSelected[1]) && (pg->dgrm.wL[wn]->pointSelected[0])) {
					if (checkComponentInputToWire(&pg->dgrm, ic, it, wn, 0)) {
						if (newInput) {
							inWireNumber.resize(inWireNumber.size() + 1);
							inWireTerminal.resize(inWireTerminal.size() + 1);
							inComponentX.push_back(pg->dgrm.cL[ic]->leftTerminalX(it));
							inComponentY.push_back(pg->dgrm.cL[ic]->leftTerminalY(it));
							newInput = false;
						}
						inWireNumber[inWireNumber.size() - 1].push_back(wn);
						inWireTerminal[inWireTerminal.size() - 1].push_back(0);
					}
				}
			}
		}
	}

	int originalSheet = pg->visibleSheet;
	switchToSheet(pg, newSubroutine);

	// build Input components in the subroutine
	int inputs = inComponentX.size();
	for (int k = 0; k < inputs; k++) {
		POS pos;
		pos.x = inComponentX[k] - 37;
		pos.y = inComponentY[k];
		AddComponent(pg, L"Input", pos, newSubroutine);
		pg->draggingComponent = -1;
	}
	// build Output components in the subroutine
	int outputs = outComponentX.size();
	for (int k = 0; k < outputs; k++) {
		POS pos;
		pos.x = outComponentX[k] + 37;
		pos.y = outComponentY[k];
		AddComponent(pg, L"Output", pos, newSubroutine);
		pg->draggingComponent = -1;
	}

	switchToSheet(pg, originalSheet);

	// delete the copied components from the source sheet
	int len = pg->dgrm.cL.size();
	for (int ic = len - 1; ic >= 0; ic--)
		if (pg->dgrm.cL[ic]->selected)
			buttonBase_Delete_Action(pg, ic);

	// delete the copied wires from the source sheet --  DELAY TO AFTER REWIRING
	//len = pg->dgrm.wL.size();
	//for (int iw = len - 1; iw >= 0; iw--) {
	//	if (!pg->dgrm.wL[iw]->pointSelected[0])
	//		continue;
	//	if (!pg->dgrm.wL[iw]->pointSelected[1])
	//		continue;
	//	pg->dgrm.wL.erase(pg->dgrm.wL.begin() + iw);
	//}

	// build the new subroutine icon
	AddComponent(pg, L"Sheet", iconPosition, newSubroutine);
	// hook up the inputs
	for (int ja = 0; ja < inputs; ja++) {
		int terms = inWireNumber[ja].size();
		for (int jb = 0; jb < terms; jb++) {
			int wn = inWireNumber[ja][jb];
			int wt = inWireTerminal[ja][jb];
			pg->dgrm.wL[wn]->x[wt] = pg->dgrm.cL[pg->draggingComponent]->leftTerminalX(ja);
			pg->dgrm.wL[wn]->y[wt] = pg->dgrm.cL[pg->draggingComponent]->leftTerminalY(ja);
		}
	}
	// hook up the outputs
	for (int ja = 0; ja < outputs; ja++) {
		int terms = outWireNumber[ja].size();
		for (int jb = 0; jb < terms; jb++) {
			int wn = outWireNumber[ja][jb];
			int wt = outWireTerminal[ja][jb];
			pg->dgrm.wL[wn]->x[wt] = pg->dgrm.cL[pg->draggingComponent]->rightTerminalX(ja);
			pg->dgrm.wL[wn]->y[wt] = pg->dgrm.cL[pg->draggingComponent]->rightTerminalY(ja);
		}
	}

	// delete the copied wires from the source sheet --  DELAYED TO AFTER REWIRING
	len = pg->dgrm.wL.size();
	for (int iw = len - 1; iw >= 0; iw--) {
		if (!pg->dgrm.wL[iw]->pointSelected[0])
			continue;
		if (!pg->dgrm.wL[iw]->pointSelected[1])
			continue;
		pg->dgrm.wL.erase(pg->dgrm.wL.begin() + iw);
	}

	pg->draggingComponent = -1;

	isL[newSubroutine]->isDirty = true;
	setVisibleScreenDirty("Dragged");// pg->isDirty = true;

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();

	pg->activeDialogComponent = -1;
}
void buttonBase_EditLabel_Action(Sheet* pg, int i) {

	pg->modalDialogComponent = i;
	buildUserLabelDialogButtons(pg, i);

}

void buttonBase_MoveToTop_Action(Sheet* pg, int i) { // last one drawn is on top. move this component to the end of the list
	
	pg->activeDialogComponent = -1;
	if (i == pg->dgrm.cL.size() - 1)
		return;

	Component *temp = pg->dgrm.cL[i];
	for (int j = i; j < pg->dgrm.cL.size()-1; j++)
		pg->dgrm.cL[j] = pg->dgrm.cL[j + 1];
	pg->dgrm.cL[pg->dgrm.cL.size() - 1] = temp;
	
	setVisibleScreenDirty("Move to Top");
	
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}

void buttonBase_MoveUp_Action(Sheet* pg, int i) { // move toward end of list
	
	pg->activeDialogComponent = -1;
	if (i == pg->dgrm.cL.size() - 1)
		return;

	Component *temp = pg->dgrm.cL[i];
	pg->dgrm.cL[i] = pg->dgrm.cL[i + 1];
	pg->dgrm.cL[i + 1] = temp;
	
	setVisibleScreenDirty("Move Up");

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}
void buttonBase_MoveDown_Action(Sheet* pg, int i) {// move toward beginning of list
	
	pg->activeDialogComponent = -1;
	if (i == 0)
		return;

	Component *temp = pg->dgrm.cL[i];
	pg->dgrm.cL[i] = pg->dgrm.cL[i - 1];
	pg->dgrm.cL[i - 1] = temp;
	
	setVisibleScreenDirty("Move Down");

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}

void buttonBase_MoveToBottom_Action(Sheet* pg, int i) { // first one drawn is on the bottom. move this to the beginning of the list
	
	pg->activeDialogComponent = -1;
	if (i == 0)
		return;

	Component *temp = pg->dgrm.cL[i];
	int k = 0;
	for (int j = i; j > 0; j--) 
		pg->dgrm.cL[j] = pg->dgrm.cL[j - 1];
	pg->dgrm.cL[0] = temp; // pg->dgrm.cL[k] = temp;
	
	setVisibleScreenDirty("Move to Bottom");

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}



void drawComponentButtons(Sheet* pg, HDC hDC) {

	if (pg->activeDialogComponent == -1)
		return;

	//if (activeDialogButtonList != null) {
	//ctx.scale(1 / pg->cs, 1 / pg->cs);

	// lets frame the popup buttons
	int minX = 1000000;
	int minY = 1000000;
	int maxX = -1000000;
	int maxY = -1000000;
		
	for (int j = 0; j < 8; j++) {
		if (pg->componentButtonList[j]->x < minX)
			minX = pg->componentButtonList[j]->x;
		if (pg->componentButtonList[j]->x + pg->componentButtonList[j]->width > maxX)
			maxX = pg->componentButtonList[j]->x + pg->componentButtonList[j]->width;
		if (pg->componentButtonList[j]->y < minY)
			minY = pg->componentButtonList[j]->y;
		if (pg->componentButtonList[j]->y + pg->componentButtonList[j]->height > maxY)
			maxY = pg->componentButtonList[j]->y + pg->componentButtonList[j]->height;
	}

	int dialogButtonCount = pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList.size();
	for( int j = 0; j < dialogButtonCount; j++){
		if (pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->x < minX)
			minX = pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->x;
		if (pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->x + pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->width > maxX)
			maxX = pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->x + pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->width;
		if (pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->y < minY)
			minY = pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->y;
		if (pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->y + pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->height > maxY)
			maxY = pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->y + pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->height;
	}

	// center it around the active component
	int cx = (pg->dgrm.cL[pg->activeDialogComponent]->x + pg->cXo) * pg->cs;
	int cy = (pg->dgrm.cL[pg->activeDialogComponent]->y + pg->cYo) * pg->cs;

	// don't let them go off screen *** repeated in DialogButton.draw()
	if (cx < 10-minX)
		cx = 10-minX;
	else if (cx > pg->workspaceWidth - maxX - 10)
		cx = pg->workspaceWidth - maxX - 10;

	if (cy < 40 - minY)
		cy = 40 - minY;
	else if (cy > pg->workspaceHeight - maxY - 60)
		cy = pg->workspaceHeight - maxY - 60;

	pg->activeDialogOffsetX = cx;
	pg->activeDialogOffsetY = cy;

	HBRUSH hbrush = GetColorBrush(RGB(240, 240, 240)); // CreateSolidBrush'#F0F0F0';
	RECT aRect;
	aRect.left = cx + minX - 5;
	aRect.top = cy + minY - 5;
	aRect.right = cx + maxX + 5;
	aRect.bottom = cy + maxY + 5;
	FillRect(hDC, &aRect, hbrush);
	//DeleteObject(hbrush);

	hbrush = GetColorBrush(RGB(0, 0, 0)); // CreateSolidBrush
	FrameRect(hDC, &aRect, hbrush);
	//DeleteObject(hbrush);

	// now draw the popup buttons
	for (int j = 0; j < 8; j++) 
		pg->componentButtonList[j]->draw(hDC, cx, cy);

	for (int j = 0; j < dialogButtonCount; j++)
		pg->dgrm.cL[pg->activeDialogComponent]->dialogButtonList[j]->draw(hDC, cx, cy);

	//	activeDialogComponent = -1;
	//ctx.scale(pg->cs, pg->cs);

}

int oldCanvasBottom = 0;
int oldCanvasRight = 0;

// offscreen mode
HBITMAP offScreenBuffer = NULL;
HDC hdcOffScreen = NULL;
void BuildOffScreenBuffer(Sheet* pg, HWND hWnd, HDC hDC, bool printing) {

	// setup off screen buffer
	RECT    clientRect;
	GetClientRect(hWnd, &clientRect);

	if (hdcOffScreen == NULL)
		hdcOffScreen = CreateCompatibleDC(hDC);
	if (offScreenBuffer == NULL)
		offScreenBuffer = CreateCompatibleBitmap(hDC, clientRect.right, clientRect.bottom);

	if ((oldCanvasRight != 0) && (oldCanvasBottom != 0)) {
		if ((oldCanvasRight < clientRect.right) || (oldCanvasBottom < clientRect.bottom)) {
				DeleteDC(hdcOffScreen);
				hdcOffScreen = CreateCompatibleDC(hDC);
				DeleteObject( offScreenBuffer);
				offScreenBuffer = CreateCompatibleBitmap(hDC, clientRect.right, clientRect.bottom);
		}
	}

	SelectObject(hdcOffScreen, offScreenBuffer);

	pg->hWnd = hWnd;
	pg->hDC = hdcOffScreen;
	SetBkMode(hdcOffScreen, TRANSPARENT);

	if ((oldCanvasRight != clientRect.right) || (oldCanvasBottom != clientRect.bottom)) {
		buildBottomFrameButtons(pg, hWnd);
		buildTopFrameButtons(pg, hWnd);
		oldCanvasRight = clientRect.right;
		oldCanvasBottom = clientRect.bottom;
	}

	// background color
	BeginPath(hdcOffScreen);
	MoveToEx(hdcOffScreen, 0, 0, NULL);
	LineTo(hdcOffScreen, 0, clientRect.bottom);
	LineTo(hdcOffScreen, clientRect.right, clientRect.bottom);
	LineTo(hdcOffScreen, clientRect.right, 0);
	LineTo(hdcOffScreen, 0, 0);
	EndPath(hdcOffScreen);
	SelectObject(hdcOffScreen, GetColorBrush(0xFFFFFF));
	FillPath(hdcOffScreen);
	
	// graph paper
	if (showGraphPaperButton->isSelected) {
		
		int gridSpacing = 10;
		BeginPath(hdcOffScreen);
		int xStart = pg->cXo % gridSpacing;
		for (int xx = xStart; xx < clientRect.right; xx += gridSpacing) {
			MoveToEx(hdcOffScreen, xx, 0, NULL);
			LineTo(hdcOffScreen, xx, clientRect.bottom);
		}
		int yStart = pg->cYo % gridSpacing;
		for (int yy = yStart; yy < clientRect.bottom; yy += gridSpacing) {
			MoveToEx(hdcOffScreen, 0, yy, NULL);
			LineTo(hdcOffScreen, clientRect.right, yy);
		}
		EndPath(hdcOffScreen);
		
		SelectObject(hdcOffScreen, GetColorPen(RGB(220, 240, 255), 1));
		StrokePath(hdcOffScreen);
	}
	SelectObject(hdcOffScreen, GetColorPen(0, 1));

	drawAllComponents(pg, hdcOffScreen);
	drawAllWires(pg, hdcOffScreen);
	if (!printing) {
		drawFrameButtons(pg, hdcOffScreen);
		drawGroupButtons(pg, hdcOffScreen);
		drawComponentButtons(pg, hdcOffScreen);

		// top edge of the workspace
		BeginPath(hdcOffScreen);
		MoveToEx(hdcOffScreen, 0, 0, NULL);
		LineTo(hdcOffScreen, clientRect.right, 0);
		EndPath(hdcOffScreen);
		SelectObject(hdcOffScreen, GetColorPen(RGB(0, 0, 0), 1));
		StrokePath(hdcOffScreen);
	}

	// show the name of this sheet
	string title;
	if (printing)
		if(pg->filePath != NULL)
		title += convertLPCTSTR_toString(pg->filePath) + "     ";

	if (pg->visibleSheet == 0) {
		if( pg->isDirty)
			title += "*Main*";
		else
			title += "Main";
	}
	else {
		if (isL[pg->visibleSheet]->isDirty) 
			title = "*";
		title += "Sub " + tomString(pg->visibleSheet) + ": ";
		if (isL[pg->visibleSheet]->name.size())
			title += isL[pg->visibleSheet]->name;
		if (isL[pg->visibleSheet]->isDirty)
			title += "*";
	}

	// diagnostic to watch the main screen and the inner sheets dirty bit
	//title += "    (";
	//if (pg->isDirty)
	//	title += "pg* ";
	//else
	//	title += "pg  ";
	//
	//for (int s = 0; s < isL.size(); s++) {
	//	title += tomString(s);
	//	if (isL[s]->isDirty)
	//		title += "* ";
	//	else
	//		title += "  ";
	//}
	//title += ")";

	drawLargeTextString(hdcOffScreen, 7 * BUTTON_WIDTH + 25, 3, 0, title);

	displaySystemTime(pg);
	showDebugString();

	// draw modal dialog last
	if (!printing) {
		drawModalScreenDialog(pg, hdcOffScreen);

		if (pg->selectionRectangleInProgress) {

			BeginPath(hdcOffScreen);
			MoveToEx(hdcOffScreen, pg->selectionRectangleStartX, pg->selectionRectangleStartY, NULL);
			LineTo(hdcOffScreen, pg->selectionRectangleStartX, pg->selectionRectangleEndY);
			LineTo(hdcOffScreen, pg->selectionRectangleEndX, pg->selectionRectangleEndY);
			LineTo(hdcOffScreen, pg->selectionRectangleEndX, pg->selectionRectangleStartY);
			LineTo(hdcOffScreen, pg->selectionRectangleStartX, pg->selectionRectangleStartY);
			EndPath(hdcOffScreen);

			HPEN hPen = CreatePen(PS_COSMETIC, 1, RGB(200, 0, 0));
			SelectObject(hdcOffScreen, hPen);
			StrokePath(hdcOffScreen);
			DeleteObject(hPen);
		}
	}
}

HBITMAP backBuffer = NULL;
HDC hdcBack = NULL;

int doubleBufferPhase = 0; //  0 ignore it, 1 build it, 2 use it, 3 destory it (on mouse up)
void deleteDoubleBuffer() {
	DeleteDC(hdcBack);
	hdcBack = NULL;
	DeleteObject(backBuffer);
	backBuffer = NULL;
	doubleBufferPhase = 0;
}
void drawCanvasForReal(Sheet* pg, HWND hWnd, HDC hDC, bool printing) {

	BuildOffScreenBuffer(pg, hWnd, hDC, printing);

	RECT    clientRect;
	RECT    windowRect;
	GetClientRect(hWnd, &clientRect);
	GetWindowRect(hWnd, &windowRect);
	pg->hWnd2 = hWnd;
	pg->hDC2 = hDC;
	pg->workspaceLeft = windowRect.left;
	pg->workspaceTop = windowRect.top;

	pg->workspaceWidth = clientRect.right;
	pg->workspaceHeight = clientRect.bottom;

	BitBlt(hDC, 0, 0, clientRect.right, clientRect.bottom, hdcOffScreen, 0, 0, SRCCOPY);

}



bool checkForTimeBased(Sheet* pg) {
	
	// update all inner sheet's time base attribute
	for (int k = isL.size()-1; k >= 0; k--) {
		bool dgrmIsTimeBased = false;
		for (int j = 0; j < isL[k]->dgrm.cL.size(); j++) {
			if (isL[k]->dgrm.cL[j]->timeBased) {
				dgrmIsTimeBased = true;
				break;
			}
			if (isL[k]->dgrm.cL[j]->type == L"Sheet") {
				int subNumber = isL[k]->dgrm.cL[j]->setting[0];
				if (isL[subNumber]->isTimeBased) {
					dgrmIsTimeBased = true;
					break;
				}
			}
		}
		isL[k]->isTimeBased = dgrmIsTimeBased;
	}

	// second pass to update all the sheet components that call the subroutines
	for (int k = isL.size() - 1; k >= 0; k--) {
		for (int j = 0; j < isL[k]->dgrm.cL.size(); j++) {
			if (isL[k]->dgrm.cL[j]->type == L"Sheet") {
				int subNumber = isL[k]->dgrm.cL[j]->setting[0];
				isL[k]->dgrm.cL[j]->timeBased = isL[subNumber]->isTimeBased;
			}
		}
	}

	// finally check the main screen
	bool someTimeBased = false;
	if (pg->visibleSheet == 0) {
		for (int j = 0; j < pg->dgrm.cL.size(); j++) {
			if (pg->dgrm.cL[j]->type == L"Sheet") {
				int subNumber = pg->dgrm.cL[j]->setting[0];
				pg->dgrm.cL[j]->timeBased = isL[subNumber]->isTimeBased;
				if (isL[subNumber]->isTimeBased)
					someTimeBased = true;
			}
			if (pg->dgrm.cL[j]->timeBased)
				someTimeBased = true;
		}
		if (!someTimeBased)
			if (pg->timerID)
				stopClock(pg);
	}
	return someTimeBased;
}


int convertToLocalX(Sheet* pg, int x) {
	return (x / pg->cs) - pg->cXo;
}
int convertToLocalY(Sheet* pg, int y) {
	return (y / pg->cs) - pg->cYo;
}


int findObject(Sheet* pg, int x, int y, bool canSeeGhosts) {
	// Search for a component under the  mouse -- check most recent first
	int x1;
	int y1;
	int dist;
	for (int i = pg->dgrm.cL.size() - 1; i >= 0; i--) {
		if (pg->dgrm.cL[i]->ghost && !canSeeGhosts)
			continue;
		//if (lockAnnotationButton.isSelected && pg->dgrm.cL[i]->annotation)
		//	continue;
		if ((abs(pg->dgrm.cL[i]->x - x) < pg->dgrm.cL[i]->width) &&
			(abs(pg->dgrm.cL[i]->y - y) < pg->dgrm.cL[i]->height)) {
			switch (pg->dgrm.cL[i]->shapePath) {
			case triangleShape:
				/* trim off the upper edge */
				y1 = pg->dgrm.cL[i]->y - (pg->dgrm.cL[i]->width + pg->dgrm.cL[i]->x - x) / 2;
				if (y < y1)
					continue;

				/* trim off the lower edge */
				y1 = pg->dgrm.cL[i]->y + (pg->dgrm.cL[i]->width + pg->dgrm.cL[i]->x - x) / 2;
				if (y > y1)
					continue;
				return i;

			case roundShape:
				x1 = pg->dgrm.cL[i]->x - x;
				y1 = pg->dgrm.cL[i]->y - y;
				dist = sqrt(x1 * x1 + y1 * y1);
				if (dist > pg->dgrm.cL[i]->width)
					continue;
				return i;

			case bussInShape:   // close but not perfect
			case bussOutShape:  // close but not perfect
			case squareShape:
			case diamondShape:  // TODO needs edge trim
			default:
				return i;
			}
		}
	}
	return -1;
}


void dragComponent(Sheet* pg, int component, int dx, int dy) {
	pg->dgrm.cL[component]->x += dx;
	pg->dgrm.cL[component]->y += dy;
	setVisibleScreenDirty("Dragged Component");// pg->isDirty = true;
//	pg->dgrm.cL[component]->setPosition(dx, dy);
}

int getComponentX(Sheet* pg, int i) {
	return pg->dgrm.cL[i]->x;
}
int getComponentY(Sheet* pg, int i) {
	return pg->dgrm.cL[i]->y;
}


int AddWire(Sheet* pg, int startX, int startY, int endX, int endY){

	Wire* temp = new Wire(startX, startY, endX, endY);
	if (temp) {
		pg->dgrm.wL.push_back(temp);
		setVisibleScreenDirty("Added Wire");// pg->isDirty = true;
		return pg->dgrm.wL.size() - 1;
	}
	return -1;
}


void dragWire(Sheet* pg, int inx, int iny) {
	int inwire = pg->draggingWire;
	int interminal = pg->dragTerminal;

	pg->dgrm.wL[inwire]->neaten(interminal, &inx, &iny);

	pg->dgrm.wL[inwire]->x[interminal] = inx;
	pg->dgrm.wL[inwire]->y[interminal] = iny;
}



void finishWire(Sheet* pg, int x, int y) {
	int wireIndex = pg->draggingWire;
	int wireEnd = pg->dragTerminal;
	pg->dgrm.wL[wireIndex]->x[wireEnd] = x;
	pg->dgrm.wL[wireIndex]->y[wireEnd] = y;
}
bool destroyShortWire(Sheet* pg) {
	int inw = pg->draggingWire;
	if (abs(pg->dgrm.wL[inw]->x[0] - pg->dgrm.wL[inw]->x[1]) < 10 &&
		abs(pg->dgrm.wL[inw]->y[0] - pg->dgrm.wL[inw]->y[1]) < 10) {
		// delete it
		delete pg->dgrm.wL[inw];
		pg->dgrm.wL.erase(pg->dgrm.wL.begin()+inw);
		return true;
	}
	return false;
}
void destroyWire(Sheet* pg) {
	int inw = pg->draggingWire;
	delete pg->dgrm.wL[inw];
	pg->dgrm.wL.erase(pg->dgrm.wL.begin() + inw);
	setVisibleScreenDirty("Deleted Wire");// pg->isDirty = true;
	pg->draggingWire = -1;
}

#include <iostream>
using std::cout;
using std::cin;
using std::ios;
using std::cerr;
using std::endl;
#include <fstream>
using std::wofstream;



std::string ConvertLPCTSTR(LPCTSTR input) {
	std::string result;
	int len = _tcslen(input);
	for (int i = 0; i < len; i++)
		result += input[i];
	return result;
}

std::string Component::toString() {

	// write the Component details
	std::string result;

	result += "<C> <t>";
	result += ConvertLPCTSTR(type) + "</t> ";
	result += "<x>" + tomString(x) + "</x> ";
	result += "<y>" + tomString(y) + "</y> ";

	if (ghost)
		result += "<g>1</g> ";

	if (element.size()) {
		result += "<e>";
		for (int j = 0; j < element.size(); j++) {
			result += tomString(element[j]);
			result += " ";
		}
		result += "</e> ";
	}

	if (setting.size()) {
		result += " <s>";
		for (int j = 0; j < setting.size(); j++) {
			result += tomString(setting[j]);
			result += " ";
		}
		result += "</s> ";
	}

	if (literal.size()) {
		for (int j = 0; j < literal.size(); j++) {
			result += "<l>";
			result += literal[j];
			result += "</l> ";
		}	
	}

	if (userLabel.size()) {
		result += "<u>";
		result += userLabel;  // hideLTGT(userLabel)
		result += "</u> ";
	}

	if (displayMode)
		result += "<d>1</d> ";

	if (rotation) {
		result += "<r>";
		result += tomString(rotation);
		result += " </r> ";
	}

	result += "</C> \n"; 
	
	return result;
}



std::string setupVariablesToString(Sheet* pg) {

	// write the Component details
	std::string result;


	result += "<version>" + tomString(THIS_PROGRAM_VERSION) + " </version>\n ";

	result += "<magwire>";
	result += magnetWiresButton->isSelected ? "1" : "0";
	result += " </magwire >\n ";

	result += "<magcomp>";
	result += magnetComponentsButton->isSelected ? "1" : "0";
	result += " </magcomp>\n ";

	result += "<sticky>";
	result += pg->stickyComponentsButton->isSelected ? "1" : "0";
	result += " </sticky>\n ";

	//if (startWiresFromTerminalsButton->isSelected)
	//	result += "<termwire>1 </termwire>\n ";

	result += "<bend>";
	result += allowWiresToBendButton->isSelected ? "1" : "0";
	result += " </bend>\n ";

	result += "<kbdsearch>";
	result += keyboardSearch->isSelected ? "1" : "0";
	result += " </kbdsearch>\n ";

	result += "<graphwork>";
	result += showGraphPaperButton->isSelected ? "1" : "0";
	result += " </graphwork>\n ";

	result += "<graph2d>";
	result += pg->showGridButton->isSelected ? "1" : "0";
	result += " </graph2d>\n ";

	result += "<axis>";
	result += pg->showAxisButton->isSelected ? "1" : "0";
	result += " </axis>\n ";

	result += "<3dlight>";
	result += enable3dDefaultLightingButton->isSelected ? "1" : "0";
	result += " </3dlight>\n ";

	if (pg->timerID != NULL)
		result += "<start>1 </start>\n ";

	result += "<resizeWork>" + tomString(pg->workspaceLeft) + " " + tomString(pg->workspaceTop) + " " + tomString(pg->workspaceWidth + 16) + " " + tomString(pg->workspaceHeight + 39) + " </resizeWork>\n ";

	if (pg->pCanvasWindow != NULL) {
		result += "<open2d>" + tomString(pg->canvas2Left) + " " + tomString(pg->canvas2Top) + " " + tomString(pg->canvas2Width+16) + " " + tomString(pg->canvas2Height+39) + " </open2d>\n ";
	}
	if (pg->GLFW_window != NULL) {
		int posX = 40;
		int posY = 50;
		int width = 400;
		int height = 450;
		glfwGetWindowPos(pg->GLFW_window, &posX, &posY);
		glfwGetWindowSize(pg->GLFW_window, &width, &height);
		result += "<open3d>" + tomString(posX) + " " + tomString(posY) + " " + tomString(width) + " " + tomString(height) + " </open3d>\n ";
	}

	return result;
}

void setTitleFromPathName(Sheet* pg) {

	if (pg->filePath == NULL) {
		SetWindowTextA(pg->hWnd, "MST Workshop");
		pg->buttonList[pg->mainNameIndex]->visibleText = L"Main: (untitled)";
		return;
	}
	// find the last slash
	int slashIndex = -1;
	int q = 0;
	while (pg->filePath[q]) {
		if (pg->filePath[q] == '\\')
			slashIndex = q;
		q++;
	}

	// set the title
	CHAR windowName[100];
	q = slashIndex + 1;
	int j = 0;
	while( (pg->filePath[q] != 0) && (pg->filePath[q] != '.'))
		windowName[j++] = pg->filePath[q++];

	windowName[j] = 0;
	SetWindowTextA(pg->hWnd, windowName);
	String buttonText = "Main: ";
	buttonText += windowName;
	pg->buttonList[pg->mainNameIndex]->visibleText = ConvertToLPWSTR(buttonText);
}



COMDLG_FILTERSPEC rgSpec[] =
{
	{ L"MST Workshop files", L"*.mstw" },
	{ L"MST text files", L"*.txt" },
	{ L"MST XML files", L"*.xml" },
	{ L"All Files", L"*.*" },
};

void AddFileExtension(WCHAR *fileName, WCHAR* fileNameWithExtension, int extensionIndex) {

	string exten;
	switch (extensionIndex) {
	case 1: exten = ".mstw"; break;
	case 2: exten = ".txt"; break;
	case 3: exten = ".xml"; break;
	case 4: exten = ""; break;
	}
	WCHAR extension[6];
	for (int k = 0; k < exten.size(); k++)
		extension[k] = exten[k];
	extension[exten.size()] = 0;


	// determine the length and last period
	int len = 0;
	int period = -1;
	while (fileName[len] != 0) {
		if (fileName[len] == '.')
			period = len;
		len++;
	}
	//WCHAR extension[] = L".mstw";
	if( period > 0){
		if (len > 6) {
			bool goodExtension = true;
			int i = len - 5;
			for (int j = 0; j < 5; j++)
				if (fileName[len - 6 + j] != extension[j]) {
					goodExtension = false;
					break;
				}
			if (goodExtension) {
				for (int j = 0; j <= len; j++)
					fileNameWithExtension[j] = fileName[j];
				return;
			}
		}
	}

	int nameLength = period;
	if (nameLength < 0)
		nameLength = len;

	// copy left of extension
	int j = 0;
	for (; j < nameLength; j++)
		fileNameWithExtension[j] = fileName[j];
	// add the new one
	for (int k = 0; k <= 5; k++)
		fileNameWithExtension[j++] = extension[k];
}


string cleanFileName(LPTSTR filePath) {
	if (filePath == NULL)
		return "";
	string fullPath = convertLPCTSTR_toString(filePath);
	int dot = fullPath.find_last_of('.');
	if (dot == -1)
		dot = fullPath.size();
	int slash = fullPath.find_last_of('\\');
	if (slash == -1)
		slash = 0;
	else
		slash++;
	return fullPath.substr(slash, dot - slash);
}



// returns false when user cancels the save
bool writeComponentsAndWiresAs(Sheet* pg) {

	syncVisibleSheet(pg);
	if (isL[0]->dgrm.cL.size() == 0) { // need something in the main routine
		MessageBox(NULL, L"Nothing to save.", L"Save File", MB_OK);
		return true;
	}

	bool fileWasSaved = false;
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog *pFileSave;

		// Create the FileSaveDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, //CLSCTX_ALL,
			CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER | CLSCTX_REMOTE_SERVER,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{
			pFileSave->SetFileTypes(4, rgSpec); // shows the list but doesn't change the extension

			// Show the Save dialog box.
			LPWSTR justName = ConvertToLPWSTR(cleanFileName(pg->filePath));
			pFileSave->SetFileName(justName);
			hr = pFileSave->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileSave->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						unsigned int fileTypeIndex;
						hr = pFileSave->GetFileTypeIndex(&fileTypeIndex);
						wchar_t newFileName[100];
						AddFileExtension(pszFilePath, newFileName, fileTypeIndex);
						CoTaskMemFree(pszFilePath);
						
						// got the name now open a file
						wofstream cout(newFileName, ios::out);
						if (cout) {
							
							for (int s = isL.size() - 1; s >= 0; s--) {
								cout << "<setSheet>";
								cout << s ;
								cout << " </setSheet>\n";
								if ((s > 0) && (isL[s]->name.size() > 0)) {
									cout << "<sheetName>";
									for (int z = 0; z < isL[s]->name.size(); z++)
										cout << isL[s]->name[z];
									cout << "</sheetName>\n";
								}
								// write the Component details
								int len = isL[s]->dgrm.cL.size();
								for (int i = 0; i < len; i++)
									cout << isL[s]->dgrm.cL[i]->toString().c_str();

								// write the wire details
								len = isL[s]->dgrm.wL.size();
								for (int i = 0; i < len; i++)
									cout << isL[s]->dgrm.wL[i]->toString().c_str();
							}
							cout << "<setSheet>0 </setSheet>\n";

							// write the setup
							cout << setupVariablesToString(pg).c_str();

							clearAllDirty();

							// copy the new name back
							int q = 0;
							pg->filePath = SysAllocStringLen(0, 100);
							while (newFileName[q] && (q < 95)) {
								pg->filePath[q] = newFileName[q];
								q++;
							}
							pg->filePath[q] = 0;

							setTitleFromPathName(pg);
						}
						fileWasSaved = true;
						//MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
						//CoTaskMemFree(pszFilePath2);
					}
					pItem->Release();
				}
			}
			pFileSave->Release();
		}
		CoUninitialize();
	}
	return fileWasSaved;
}
void writeComponentsAndWiresAs_void(Sheet* pg) {
	writeComponentsAndWiresAs(pg);
}
bool writeComponentsAndWires(Sheet* pg) {

	syncVisibleSheet(pg);
	if (isL[0]->dgrm.cL.size() == 0) {
		MessageBox(NULL, L"Nothing to save.", L"Save File", MB_OK);
		return true;
	}
	if (pg->filePath == NULL)
		return writeComponentsAndWiresAs(pg);

	bool fileWasSaved = false;
	
	//  open the file
	wofstream cout(pg->filePath, ios::out);
	if (cout) {
		
		for (int s = isL.size()-1; s >= 0; s--) {
			cout << "<setSheet>";
			cout << s;
			cout << " </setSheet>\n";
			if ((s > 0) && (isL[s]->name.size() > 0)) {
				cout << "<sheetName>";
				for( int z = 0; z < isL[s]->name.size(); z++)
					cout << isL[s]->name[z];
				cout << "</sheetName>\n";
			}
			// write the Component details
			int len = isL[s]->dgrm.cL.size();
			for (int i = 0; i < len; i++)
				cout << isL[s]->dgrm.cL[i]->toString().c_str();

			// write the wire details
			len = isL[s]->dgrm.wL.size();
			for (int i = 0; i < len; i++)
				cout << isL[s]->dgrm.wL[i]->toString().c_str();
		}
		cout << "<setSheet>0 </setSheet>\n";

		// write the setup
		cout << setupVariablesToString(pg).c_str();

		clearAllDirty();
		fileWasSaved = true;
	}
	return fileWasSaved;
}
void writeComponentsAndWires_void(Sheet* pg) {
	writeComponentsAndWires(pg);
}

string componentsAndWiresToString(Sheet* pg) {

	syncVisibleSheet(pg);
	string result;

	for (int s = isL.size() - 1; s >= 0; s--) {
		result += "<setSheet>";
		result += tomString(s);
		result += " </setSheet>\n";
		if ((s > 0) && (isL[s]->name.size() > 0)) {
			result += "<sheetName>";
			for (int z = 0; z < isL[s]->name.size(); z++)
				result += isL[s]->name[z];
			result += "</sheetName>\n";
		}
		// write the Component details
		int len = isL[s]->dgrm.cL.size();
		for (int i = 0; i < len; i++)
			result += isL[s]->dgrm.cL[i]->toString().c_str();

		// write the wire details
		len = isL[s]->dgrm.wL.size();
		for (int i = 0; i < len; i++)
			result += isL[s]->dgrm.wL[i]->toString().c_str();
	}
	return result;
}


int Component::leftTerminalX(int it) {
	return x - width - 1 - TERMINAL_SIZE;
}

int Component::leftTerminalY(int it) {
	return y
		- (TERMINAL_STEP * (leftTerminalCount - 1) * leftTerminalStep) / 2
		+ it * TERMINAL_STEP * leftTerminalStep;
}

int Component::rightTerminalX(int it) {
	return x + width + 1 + TERMINAL_SIZE;
}

int Component::rightTerminalY(int it) {
	return y
		- (TERMINAL_STEP * (rightTerminalCount - 1) * rightTerminalStep) / 2
		+ it * TERMINAL_STEP * rightTerminalStep;
}


bool checkComponentOutputToWire(Diagram* dgrm, int ic, int it, int wn, int wt) {

	int xc = dgrm->cL[ic]->rightTerminalX(it);
	int xw = dgrm->wL[wn]->x[wt];
	//alert('xc = ' + xc.tomString() + ' xw = ' + xw.tomString() );
	if (abs(xc - xw) > TERMINAL_SIZE)
		return false;

	int yc = dgrm->cL[ic]->rightTerminalY(it);
	int yw = dgrm->wL[wn]->y[wt];
	//alert('yc = ' + yc.tomString() + ' yw = ' + yw.tomString() );
	if( abs(yc - yw) > TERMINAL_SIZE)
		return false;

	// lets be exact
	return ((xc - xw)*(xc - xw) + (yc - yw)*(yc - yw)) <= TERMINAL_SIZE_SQUARED;
}

bool checkOverlap(Diagram* dgrm, int ic, int it, int oc, int ot) {
	//int dist = TERMINAL_SIZE * 2;

	int x0 = dgrm->cL[ic]->leftTerminalX(it);
	int x1 = dgrm->cL[oc]->rightTerminalX(it);
	//alert('x0 = ' + x0.tomString() + ' x1 = ' + x1.tomString() );
	if (abs(x0 - x1) > TERMINAL_SIZE)
		return false;

	int y0 = dgrm->cL[ic]->leftTerminalY(it);
	int y1 = dgrm->cL[oc]->rightTerminalY(ot);
	//alert('y0 = ' + y0.tomString() + ' y1 = ' + y1.tomString() );
	if( abs(y0 - y1) > TERMINAL_SIZE)
		return false;

	// lets be exact
	return ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1)) <= TERMINAL_SIZE_SQUARED;
}
bool checkWireTerminalOverlap(Diagram* dgrm, int w1, int t1, int w2, int t2) {
	//int dist = TERMINAL_SIZE * 2;

	int x1 = dgrm->wL[w1]->x[t1];
	int x2 = dgrm->wL[w2]->x[t2];

	//alert('x0 = ' + x0.tomString() + ' x1 = ' + x1.tomString() );
	if (abs(x1 - x2) > TERMINAL_SIZE)
		return false;

	int y1 = dgrm->wL[w1]->y[t1];
	int y2 = dgrm->wL[w2]->y[t2];
	//alert('y0 = ' + y0.tomString() + ' y1 = ' + y1.tomString() );
	if (abs(y1 - y2) > TERMINAL_SIZE)
		return false;

	// lets be exact
	return ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) <= TERMINAL_SIZE_SQUARED;
}

bool checkWireOverlap(Diagram* dgrm, int w1, int w2) {
	if (checkWireTerminalOverlap(dgrm, w1, 0, w2, 0))
		return true;
	if (checkWireTerminalOverlap(dgrm, w1, 0, w2, 1))
		return true;
	if (checkWireTerminalOverlap(dgrm, w1, 1, w2, 0))
		return true;
	if (checkWireTerminalOverlap(dgrm, w1, 1, w2, 1))
		return true;
	return false;
}

bool checkComponentInputToWire(Diagram* dgrm, int ic, int it, int wn, int wt) {

	int xc = dgrm->cL[ic]->leftTerminalX(it);
	int xw = dgrm->wL[wn]->x[wt];
	//alert('xc = ' + xc.tomString() + ' xw = ' + xw.tomString() );
	if (abs(xc - xw) > TERMINAL_SIZE)
		return false;

	int yc = dgrm->cL[ic]->leftTerminalY(it);
	int yw = dgrm->wL[wn]->y[wt];
	//alert('yc = ' + yc.tomString() + ' yw = ' + yw.tomString() );
	if (abs(yc - yw) > TERMINAL_SIZE)
		return false;

	// lets be exact
	return ((xc - xw)*(xc - xw) + (yc - yw)*(yc - yw)) <= TERMINAL_SIZE_SQUARED;
}


void propagateSheetValues(Diagram* dgrm) {
	int oc, ot, j;
	bool outputChanged = true; // just to get started
	bool updateCanvas2 = false;
	bool inputChanged;
	bool timeBasedChange = false;
	int ic;
	int pass = 0;

	//  debugShowTime( 'Propagate start');
	// first push the sources once
	for (ic = 0; ic < dgrm->cL.size(); ic++) {
	    if (!dgrm->cL[ic]->ghost)
	        if (dgrm->cL[ic]->timeBased)
	            if (dgrm->cL[ic]->method2(page))
	                timeBasedChange = true;
	}

	// then run the rest until they are up to date
	//debugTrace(' ');
	//debugTrace('------------- Propagate start ---------------------');

	while (outputChanged) {
		outputChanged = false; // lets hope
		if (timeBasedChange) {
			outputChanged = true;
			timeBasedChange = false;
		}
		//debugTrace('Pass: ' + pass + ', called by: ' + debugCalledBy);

		// Update all the components
		for (ic = 0; ic < dgrm->cL.size(); ic++) {
			if (dgrm->cL[ic]->ghost)
				continue;
			//debugTrace('  Component ' + ic + '  ' + pg->dgrm.cL[ic].type );

			// scan this component's input for drivers
			bool copyIt = false;
			inputChanged = false;
			for (int it = 0; it < dgrm->cL[ic]->leftTerminalCount; it++) {
				//debugTrace('    Terminal ' + it);
				// first see if its driven
				oc = dgrm->cL[ic]->input[it].drivenByComponent;
				if (oc == -1) { // its not driven
					if (dgrm->cL[ic]->input[it].dimensions.size() != 0) {
						dgrm->cL[ic]->input[it].value.clear();
						dgrm->cL[ic]->input[it].stringValue.clear();
						dgrm->cL[ic]->input[it].value.push_back(0);
						dgrm->cL[ic]->input[it].stringType = false;
						dgrm->cL[ic]->input[it].dimensions.clear();
						inputChanged = true;
					}
					//debugTrace('    Terminal ' + it + ' Not driven.' );
					continue;
				}

				ot = dgrm->cL[ic]->input[it].drivenByOutputTerminal;
				//debugTrace('    Terminal ' + it + ' Driven by component ' + oc + ' terminal ' + ot );
				if (!copyIt) { // this will save time. If any input pin is updated, just update the rest without checking for change

					// Its driven, so check the dimensions of the input and output for a change
					copyIt = false;
					if (dgrm->cL[ic]->input[it].dimensions.size() != dgrm->cL[oc]->output[ot].dimensions.size()) {
						copyIt = true; // number of dimensions is different
						//debugTrace('      number of dimensions is different: ' + pg->dgrm.cL[ic].inputValueDimensions[it].length + ' / ' + pg->dgrm.cL[oc].outputValueDimensions[ot].length );
					}
					else { // same number of dimensions, check each dimension
						for (int dim = 0; dim < dgrm->cL[oc]->output[ot].dimensions.size(); dim++) {
							if (dgrm->cL[ic]->input[it].dimensions[dim] != dgrm->cL[oc]->output[ot].dimensions[dim]) {
								copyIt = true; // one of the dimensions is different
								//debugTrace('      one of the dimensions is different: ' + pg->dgrm.cL[ic].inputValueDimensions[it][dim] + ' / ' + pg->dgrm.cL[oc].outputValueDimensions[ot][dim]  );
								break;
							}
						}
					}

					if (!copyIt) { // next check the values of the input and output for a change
						if (!dgrm->cL[oc]->output[ot].stringType) {
							if (dgrm->cL[ic]->input[it].value.size() != dgrm->cL[oc]->output[ot].value.size()) { // not really necessary, they should be the same at this point
								copyIt = true; // size of array is different
								//debugTrace('      size of array is different: '  +  pg->dgrm.cL[ic].inputValue[it].length + ' / ' + pg->dgrm.cL[oc].outputValue[ot].length );
							}
							else { // check each value in the list
								for (j = 0; j < dgrm->cL[oc]->output[ot].value.size(); j++) { 
									// *************** this nan code is important
									if ( isnan(dgrm->cL[ic]->input[it].value[j]) && isnan(dgrm->cL[oc]->output[ot].value[j]))
										continue; // no need to copy if both are NaN
									if (dgrm->cL[ic]->input[it].value[j] != dgrm->cL[oc]->output[ot].value[j]) {
										copyIt = true; // one of the values of the output doesnt match the input
										//debugTrace('      one value of output doesnt match: ' + pg->dgrm.cL[ic].inputValue[it][j] + ' / ' + pg->dgrm.cL[oc].outputValue[ot][j] );
										break;
									}
								}
							}
						}
						else // its a string type
						{
							if (dgrm->cL[ic]->input[it].stringValue.size() != dgrm->cL[oc]->output[ot].stringValue.size()) { // not really necessary, they should be the same at this point
								copyIt = true; // size of array is different
								//debugTrace('      size of array is different: '  +  pg->dgrm.cL[ic].inputValue[it].length + ' / ' + pg->dgrm.cL[oc].outputValue[ot].length );
							}
							else { // check each value in the list
								for (j = 0; j < dgrm->cL[oc]->output[ot].stringValue.size(); j++) {  
									if (dgrm->cL[ic]->input[it].stringValue[j] != dgrm->cL[oc]->output[ot].stringValue[j]) {
										copyIt = true; // one of the values of the output doesnt match the input
										//debugTrace('      one value of output doesnt match: ' + pg->dgrm.cL[ic].inputValue[it][j] + ' / ' + pg->dgrm.cL[oc].outputValue[ot][j] );
										break;
									}
								}
							}
						}
					}
				}
				int size;
				if (copyIt) { // finally we copy all the dimensions and data from the output (driver) to the input (driven component)
					//debugTrace('      finally we copy, inputChanged = ' + inputChanged + ' copyIt = ' + copyIt );
					if (!dgrm->cL[oc]->output[ot].stringType) {
						size = dgrm->cL[oc]->output[ot].value.size();
						dgrm->cL[ic]->input[it].value.resize(size);
						dgrm->cL[ic]->input[it].stringValue.clear();
						for (j = 0; j < size; j++)
							dgrm->cL[ic]->input[it].value[j] = dgrm->cL[oc]->output[ot].value[j]; // copy values
					}
					else // its a string type
					{
						size = dgrm->cL[oc]->output[ot].stringValue.size();
						dgrm->cL[ic]->input[it].stringValue.resize(size);
						dgrm->cL[ic]->input[it].value.clear();
						for (j = 0; j < size; j++) 
							dgrm->cL[ic]->input[it].stringValue[j] = dgrm->cL[oc]->output[ot].stringValue[j]; // copy string values
					}
					dgrm->cL[ic]->input[it].stringType = dgrm->cL[oc]->output[ot].stringType;

					dgrm->cL[ic]->input[it].dimensions.clear();// = pg->dgrm.cL[oc].outputValueDimensions[ot].length;
					for (j = 0; j < dgrm->cL[oc]->output[ot].dimensions.size(); j++)
						dgrm->cL[ic]->input[it].dimensions.push_back(dgrm->cL[oc]->output[ot].dimensions[j]); // copy dimensions
					if (!dgrm->cL[ic]->timeBased)
						inputChanged = true;
				}

			}

			//  if any have changed re run it also on first pass run all time based components
			if (inputChanged || ((pass == 0) && dgrm->cL[ic]->timeBased)) {
				//if (pg->dgrm.cL[ic]->method != NULL) {
				int result = dgrm->cL[ic]->method2(page);
				if (result & 1) {
					outputChanged = true;
					//debugTrace('      component ' + pg->dgrm.cL[ic].type + ' was run output changed = ' + outputChanged  );
				}
				if (result & 2) {
					updateCanvas2 = true;
					//debugTrace('      component ' + pg->dgrm.cL[ic].type + ' was run output changed = ' + outputChanged  );
				}
				//}
			}
		}
		pass++;

		if (pass > 5 + 3 * dgrm->cL.size()) {
			//console.log(" propagate " + pg->name + " took " + pass + " passes. limit is " + (5 + 3 * pg->dgrm.cL.length) + " passes. ");
			return;
		}

	}

	if (updateCanvas2)
		drawCanvas2();
	//debugTrace( " propagate took " + pass + " passes. Limit is " + pg->dgrm.cL.length );
}

void propagateValues(Diagram* dgrm) {
	//   if( isNaN( lineNumber))
	//       debugCalledBy = 0;
	//   debugCalledBy = lineNumber;
	propagateSheetValues(dgrm);// always propagate the Sheet* zero
	//drawCanvas2();
	updateCounter++;
}




void showDebugString() {

	if (debugString.size() == 0)
		return;

	int x = page->workspaceWidth / 2;
	int y = page->workspaceHeight - 40;
	int width = 500;
	int height = 12;

	BeginPath(page->hDC);
	Rectangle(page->hDC, x-2, y-2, x+width+4, y+height+4);
	EndPath(  page->hDC);

	HBRUSH fillColor = CreateSolidBrush(RGB(205, 255, 255));
	SelectObject(page->hDC, fillColor);
	StrokeAndFillPath(page->hDC);
	DeleteObject(fillColor);

	drawTextString(page->hDC, x, y, 0, debugString);

	RECT aRect;
	aRect.left = x-2;
	aRect.right = x + width+4;
	aRect.top = y-2;
	aRect.bottom = y + height+4;

	InvalidateRect(page->hWnd, &aRect, true);
	UpdateWindow(page->hWnd);
}


void updateThisBuss2Output(Component* buss2InComp, Component* buss2OutComp) {

	if (buss2InComp->type != L"Buss2In")
		return;
	if (buss2OutComp->type != L"Buss2Out")
		return;
	if (buss2OutComp->userLabel != buss2InComp->userLabel)
		return;

	buss2OutComp->buildOutputPins(buss2InComp->literal.size());
	buss2OutComp->height = buss2InComp->height;

//	var right = buss2OutComp.x + buss2OutComp.setting[0];
	buss2OutComp->width = buss2InComp->width;
	buss2OutComp->setting[0] = buss2InComp->width;
//	buss2OutComp.x = right - buss2OutComp.width;

	for (int k = 0; k < buss2InComp->literal.size(); k++) {
		buss2OutComp->output[k].label = buss2InComp->literal[k];
	}
}

int saveWireLength = -1;  // page->dgrm.wL length before adding buss wires
void convertBussesToWires(Diagram* dgrm) {
	saveWireLength = dgrm->wL.size(); // save for reset

	// ************** first do Buss ****************

	// first scan for duplicate inputs, we can have only one with each name
	vector<int> inputList;
	for (int ic = 0; ic < dgrm->cL.size(); ic++) {
		if (dgrm->cL[ic]->type != L"BussIn")
			continue;
		if (dgrm->cL[ic]->ghost)
			continue;

		// be sure we dont already have an input with this name
		bool found = false;
		for (int k = 0; k < inputList.size(); k++) {
			if (dgrm->cL[ic]->literal[0] == dgrm->cL[inputList[k]]->literal[0]) {
				found = true;
				break;
			}
		}
		if (!found)
			inputList.push_back(ic);
	}
	if (inputList.size() > 0) { // pointless without inputs

		// convert each bussOut to a wire
		for (int oc = 0; oc < dgrm->cL.size(); oc++) {
			if (dgrm->cL[oc]->type != L"BussOut")
				continue;
			if (dgrm->cL[oc]->ghost)
				continue;

			// search for the matching buss input
			for (int k = 0; k < inputList.size(); k++) {
				int ic = inputList[k];
				if (dgrm->cL[ic]->literal[0] != dgrm->cL[oc]->literal[0])
					continue;

				// make a wire
				Wire *bussWire = new Wire();
				// start at the buss out
				bussWire->x.push_back(dgrm->cL[oc]->rightTerminalX(0));
				bussWire->y.push_back(dgrm->cL[oc]->rightTerminalY(0));
				// run to the buss input
				bussWire->x.push_back(dgrm->cL[ic]->leftTerminalX(0));
				bussWire->y.push_back(dgrm->cL[ic]->leftTerminalY(0));

				dgrm->wL.push_back(bussWire);
			}
		}
	}

	// ************** second do Buss 2 ****************

	// first scan for duplicate inputs, we can have only one with each name
	inputList.clear();
	for (int ic = 0; ic < dgrm->cL.size(); ic++) {
		if (dgrm->cL[ic]->type != L"Buss2In")
			continue;
		if (dgrm->cL[ic]->ghost)
			continue;

		// be sure we dont already have an input with this name
		bool found = false;
		for (int k = 0; k < inputList.size(); k++) {
			if (dgrm->cL[ic]->userLabel == dgrm->cL[inputList[k]]->userLabel) {
				found = true;
				break;
			}
		}
		if (!found)
			inputList.push_back(ic);
	}
	if (inputList.size() > 0) { // pointless without inputs

		// convert each buss2Out to a wire
		for (int oc = 0; oc < dgrm->cL.size(); oc++) {
			if (dgrm->cL[oc]->type != L"Buss2Out")
				continue;
			if (dgrm->cL[oc]->ghost)
				continue;


			// search for the matching buss2 input
			for (int k = 0; k < inputList.size(); k++) {
				int ic = inputList[k];
				if (dgrm->cL[oc]->userLabel != dgrm->cL[ic]->userLabel)
					continue;

				if(dgrm->cL[oc]->output.size() != dgrm->cL[ic]->input.size())
					updateThisBuss2Output(page->dgrm.cL[ic], page->dgrm.cL[oc]);

				int conductors = dgrm->cL[ic]->input.size(); //min(dgrm->cL[oc]->output.size(), dgrm->cL[ic]->input.size());
				for (int w = 0; w < conductors; w++) {
					// make a wire
					Wire *bussWire = new Wire();
					// start at the buss out
					bussWire->x.push_back(dgrm->cL[oc]->rightTerminalX(w));
					bussWire->y.push_back(dgrm->cL[oc]->rightTerminalY(w));
					// run to the buss input
					bussWire->x.push_back(dgrm->cL[ic]->leftTerminalX(w));
					bussWire->y.push_back(dgrm->cL[ic]->leftTerminalY(w));

					dgrm->wL.push_back( bussWire);
				}
			}
		}
	}

}

void resetBusses(Diagram* dgrm) {

	int size = dgrm->wL.size();
	for (int k = size - 1; k >= saveWireLength; k--)
		delete dgrm->wL[k];
	dgrm->wL.resize(saveWireLength);
}


void analyzeWires(Diagram* dgrm) {
	int w, oc, ot;

	// first find wires that are connected directly to component outputs
	for (w = 0; w < dgrm->wL.size(); w++) {
		if (dgrm->wL[w]->ghost)
			continue;

		dgrm->wL[w]->drivenByComponent = -1;
		dgrm->wL[w]->drivenByOutputTerminal = -1;

		for (oc = 0; oc < dgrm->cL.size(); oc++) {
			if ((dgrm->cL[oc]->type == L"BussOut") || (dgrm->cL[oc]->type == L"Buss2Out"))
				continue; // these will be connected by temporary wires later

			for (ot = 0; ot < dgrm->cL[oc]->rightTerminalCount; ot++) {
				if (checkComponentOutputToWire(dgrm, oc, ot, w, 0)) {
					dgrm->wL[w]->drivenByComponent = oc;
					dgrm->wL[w]->drivenByOutputTerminal = ot;
					break;
				}
				if (checkComponentOutputToWire(dgrm, oc, ot, w, 1)) {
					dgrm->wL[w]->drivenByComponent = oc;
					dgrm->wL[w]->drivenByOutputTerminal = ot;
					break;
				}
			}
			if (dgrm->wL[w]->drivenByComponent != -1)
				break;
		}
	}

	// now scan the undriven wires to see if any are connected to wires that are driven
	bool newConnection = true; // just to get things rolling
	int w1, w2;
	while (newConnection) {
		newConnection = false; // repeat if any wire was proven to be driven

		// scan all the unconnected wires
		for (w1 = 0; w1 < dgrm->wL.size(); w1++) // w1 is an unconnected wire
		{
			if (dgrm->wL[w1]->ghost)
				continue;
			if (dgrm->wL[w1]->drivenByComponent != -1)
				continue;

			for (w2 = 0; w2 < dgrm->wL.size(); w2++) // w2 is a connected wire
			{
				if (w1 == w2)
					continue;
				if (dgrm->wL[w2]->drivenByComponent == -1)
					continue;

				if (checkWireOverlap(dgrm, w1, w2)) {
					dgrm->wL[w1]->drivenByComponent = dgrm->wL[w2]->drivenByComponent;
					dgrm->wL[w1]->drivenByOutputTerminal = dgrm->wL[w2]->drivenByOutputTerminal;
					newConnection = true;
					break;
				}
			}
		}
	}
}
void testanalyze() {
	analyzeConnections(&page->dgrm);
}
void remoteAnalyzeConnections() {
	analyzeConnections(&page->dgrm);
}
void analyzeConnections(Diagram* dgrm) {
	int oc, ic;
	int ot, it;
	int j;

	// pre test for null wires
	for (int w = 0; w < dgrm->wL.size(); w++) {
		if (abs(dgrm->wL[w]->x[0] - dgrm->wL[w]->x[1]) < 10 &&
			abs(dgrm->wL[w]->y[0] - dgrm->wL[w]->y[1]) < 10) {
			// delete it
			dgrm->wL.erase(dgrm->wL.begin() + w);
			w--; // the wire[w] is replaced by a different wire, so test that one too
		}
	}

	convertBussesToWires(dgrm);
	analyzeWires(dgrm);

	// scan all the inputs to see if we can find who drives it
	for (ic = 0; ic < dgrm->cL.size(); ic++) {

		if (dgrm->cL[ic]->ghost)
			continue;

		// clear this component's input drivers
		for (j = 0; j < dgrm->cL[ic]->leftTerminalCount; j++) {
			dgrm->cL[ic]->input[j].drivenByComponent = -1;
			dgrm->cL[ic]->input[j].drivenByOutputTerminal = -1;
		}

		// check each terminal for a driver component
		for (it = 0; it < dgrm->cL[ic]->leftTerminalCount; it++) {

			// check all the other components
			for (oc = 0; oc < dgrm->cL.size(); oc++) {
				if (oc == ic)
					continue;
				if (dgrm->cL[oc]->ghost)
					continue;
				if ((dgrm->cL[oc]->type == L"BussOut") || (dgrm->cL[oc]->type == L"Buss2Out"))
					continue; // these are connected by temporary wires later

				for (ot = 0; ot < dgrm->cL[oc]->rightTerminalCount; ot++) {
					if (checkOverlap(dgrm, ic, it, oc, ot)) {
						dgrm->cL[ic]->input[it].drivenByComponent = oc;
						dgrm->cL[ic]->input[it].drivenByOutputTerminal = ot;
						break; // done checking right terminals -- speed improvement
					}
				}

				if (dgrm->cL[ic]->input[it].drivenByComponent != -1)
					break; // done checking other components -- speed improvement
			}
			if (dgrm->cL[ic]->input[it].drivenByComponent == -1) {
				// no components are connected - search for a wire to drive this input
				for (int w = 0; w < dgrm->wL.size(); w++) {
					if (dgrm->wL[w]->ghost)
						continue;
					if (dgrm->wL[w]->drivenByComponent != -1)
						if (!dgrm->cL[dgrm->wL[w]->drivenByComponent]->ghost) {
							if (checkComponentInputToWire(dgrm, ic, it, w, 0)) {
								dgrm->cL[ic]->input[it].drivenByComponent = dgrm->wL[w]->drivenByComponent;
								dgrm->cL[ic]->input[it].drivenByOutputTerminal = dgrm->wL[w]->drivenByOutputTerminal;
							}
							else if (checkComponentInputToWire(dgrm, ic, it, w, 1)) {
								dgrm->cL[ic]->input[it].drivenByComponent = dgrm->wL[w]->drivenByComponent;
								dgrm->cL[ic]->input[it].drivenByOutputTerminal = dgrm->wL[w]->drivenByOutputTerminal;
							}
							if (dgrm->cL[ic]->input[it].drivenByComponent != -1)
								break;
						}
				}
			}
		}
	}
	resetBusses(dgrm);
	analyzeCounter++;

	propagateValues(dgrm);
//	drawCanvas();
//	drawCanvas2();
//	if (theTimer != null) {
//		//if timer is running, save the program if user made changes. This is necessary here because we suppress checkProgramChanged in drawcanvas when the timer is running for performance reasons. We put this here to catch most cases where user made substantive changes to the program.
//	//    console.log( 'analyzeConnections'); // debug6162016
//		checkProgramChanged(true);
//	}
}

// Find a wire under the mouse - check most recent first
PATH findWireTerminal(int x, int y) {
	PATH combo;
	combo.w = -1;
	combo.t = -1;
	for (int i = page->dgrm.wL.size() - 1; i >= 0; i--) {
		if (page->dgrm.wL[i]->ghost)
			continue;
		for (int j = 0; j < page->dgrm.wL[i]->x.size(); j++) {
			if ((abs(page->dgrm.wL[i]->x[j] - x) < 10) &&
				(abs(page->dgrm.wL[i]->y[j] - y) < 10)) {
				combo.w = i;
				combo.t = j;
				return combo;
			}
		}
	}
	return combo;
}

bool componentIsGhost(int i) {
	return page->dgrm.cL[i]->ghost;
}
bool componentHasCrank(HDC hDC, int i, int posX, int posY, bool check) {
	//if (page->dgrm.cL[i]->crank == NULL)
	//	return false;
	return page->dgrm.cL[i]->crank(page, posX, posY, check);
}
void doCrank(HDC hDC, int i, int posx, int posy, bool check) {
	page->dgrm.cL[i]->crank(page, posx, posy, check);
}
void doMouseUp(int i) {
	//if (page->dgrm.cL[i]->onMouseUp != NULL)
		page->dgrm.cL[i]->onMouseUp(page);
}



double Component::detent(int detentIndex, double valueIndex) {
	// detent upgrade
	if (setting[detentIndex] == NULL)
		setting[detentIndex] = 0;

	return detentValue(setting[valueIndex], setting[detentIndex]);
}

POS findComponentTerminal(int posX, int posY) {
	POS result;

	for (int i = 0; i < page->dgrm.cL.size(); i++) {
		// check inputs
		for (int j = 0; j < page->dgrm.cL[i]->leftTerminalCount; j++) {
			if (abs(posX - page->dgrm.cL[i]->leftTerminalX(j)) > TERMINAL_SIZE)
				continue;
			if (abs(posY - page->dgrm.cL[i]->leftTerminalY(j)) > TERMINAL_SIZE)
				continue;
			result.x = page->dgrm.cL[i]->leftTerminalX(j);
			result.y = page->dgrm.cL[i]->leftTerminalY(j);
				return result;
		}

		// check outputs
		for (int j = 0; j < page->dgrm.cL[i]->rightTerminalCount; j++) {
			if (abs(posX - page->dgrm.cL[i]->rightTerminalX(j)) > TERMINAL_SIZE)
				continue;
			if (abs(posY - page->dgrm.cL[i]->rightTerminalY(j)) > TERMINAL_SIZE)
				continue;
			result.x = page->dgrm.cL[i]->rightTerminalX(j);
			result.y = page->dgrm.cL[i]->rightTerminalY(j);
			return result;
		}
	}

	// not found
	result.x = -1;
	result.y = -1;
	return result;
}

// **************************** Canvas 2 drawing ***********************************

int oldCanvas2Bottom = 0;
int oldCanvas2Right = 0;

// offscreen mode
HBITMAP offScreenBuffer2 = NULL;
HDC hdcOffScreen2 = NULL;
void BuildOffScreenBuffer2(HWND hWnd, HDC hDC) {

	// setup off screen buffer
	RECT    clientRect;
	GetClientRect(hWnd, &clientRect);

	if (hdcOffScreen2 == NULL)
		hdcOffScreen2 = CreateCompatibleDC(hDC);
	if (offScreenBuffer2 == NULL)
		offScreenBuffer2 = CreateCompatibleBitmap(hDC, clientRect.right, clientRect.bottom);

	if ((oldCanvas2Right != 0) && (oldCanvas2Bottom != 0)) {
		if ((oldCanvas2Right < clientRect.right) || (oldCanvas2Bottom < clientRect.bottom)) {
			DeleteDC(hdcOffScreen2);
			hdcOffScreen2 = CreateCompatibleDC(hDC);
			DeleteObject(offScreenBuffer2);
			offScreenBuffer2 = CreateCompatibleBitmap(hDC, clientRect.right, clientRect.bottom);
		}
	}

	SelectObject(hdcOffScreen2, offScreenBuffer2);

	page->hWnd2 = hWnd;
	page->hDC2 = hdcOffScreen2;

	if ((oldCanvas2Right != clientRect.right) || (oldCanvas2Bottom != clientRect.bottom)) {
		oldCanvas2Right = clientRect.right;
		oldCanvas2Bottom = clientRect.bottom;
	}

	int canvasWidth = clientRect.right - clientRect.left;
	int canvasHeight = clientRect.bottom - clientRect.top;

	page->canvas2Width = canvasWidth;
	page->canvas2Height = canvasHeight;

	BeginPath(hdcOffScreen2);
	Rectangle(hdcOffScreen2, 0, 0, clientRect.right, clientRect.bottom);
	EndPath(hdcOffScreen2);

	HBRUSH fillColor = GetColorBrush(RGB(page->canvas2Red, page->canvas2Green, page->canvas2Blue));
	SelectObject(hdcOffScreen2, fillColor);
	StrokeAndFillPath(hdcOffScreen2);

	if (page->showGridButton->isSelected) {
		int gridSpacing = 10;
		BeginPath(hdcOffScreen2);
		//int xStart = ((-page->canvas2xOffset) % gridSpacing) ;
		//for (int xx = xStart; xx < clientRect.right; xx += gridSpacing) {
		//	MoveToEx(hdcOffScreen2, xx, 0, NULL);
		//	LineTo(hdcOffScreen2, xx, clientRect.bottom);
		//}
		// right half
		int gridX = xTo2dCanvas(page, 0);
		while (gridX < clientRect.right) {
			MoveToEx(hdcOffScreen2, gridX, 0, NULL);
			LineTo(hdcOffScreen2, gridX, clientRect.bottom);
			gridX += gridSpacing;
		}

		// left half
		gridX = xTo2dCanvas(page, 0) - gridSpacing;
		while (gridX > 0) {
			MoveToEx(hdcOffScreen2, gridX, 0, NULL);
			LineTo(hdcOffScreen2, gridX, clientRect.bottom);
			gridX -= gridSpacing;
		}

		// bottom half
		int gridY = yTo2dCanvas(page, 0);
		while (gridY < clientRect.bottom) {
			MoveToEx(hdcOffScreen2, 0, gridY, NULL);
			LineTo(hdcOffScreen2, clientRect.right, gridY);
			gridY += gridSpacing;
		}
		// top half
		gridY = yTo2dCanvas(page, 0) - gridSpacing;
		while (gridY > 0) {
			MoveToEx(hdcOffScreen2, 0, gridY, NULL);
			LineTo(hdcOffScreen2, clientRect.right, gridY);
			gridY -= gridSpacing;
		}


		EndPath(hdcOffScreen2);

		SelectObject(hdcOffScreen2, GetColorPen(RGB(220, 240, 255), 1));
		StrokePath(hdcOffScreen2);
	}

	if (page->showAxisButton->isSelected)
		drawAxis(page);

	if (page->visibleSheet == 0) {
		for (int i = 0; i < page->dgrm.cL.size(); i++)
			if (page->dgrm.cL[i]->ghost == NORMAL_GHOST)
				page->dgrm.cL[i]->dial2d(page);
	}
	else {
		int tempSheet = page->visibleSheet;
		switchToSheet(page, 0);
		for (int i = 0; i < page->dgrm.cL.size(); i++)
			if (page->dgrm.cL[i]->ghost == NORMAL_GHOST)
				page->dgrm.cL[i]->dial2d(page);
		switchToSheet(page, tempSheet);
	}

}

HBITMAP backBuffer2 = NULL;
HDC hdcBack2 = NULL;

int doubleBuffer2Phase = 0; //  0 ignore it, 1 build it, 2 use it, 3 destory it (on mouse up)
void deleteDoubleBuffer2() {
	DeleteDC(hdcBack2);
	hdcBack2 = NULL;
	DeleteObject(backBuffer2);
	backBuffer2 = NULL;
	doubleBuffer2Phase = 0;
}
void drawCanvas2ForReal(HWND hWnd, HDC hDC) {

	BuildOffScreenBuffer2(hWnd, hDC);

	RECT clientRect;
	RECT windowRect;
	GetClientRect(hWnd, &clientRect);
	GetWindowRect(hWnd, &windowRect);
	page->hWnd2 = hWnd;
	page->hDC2 = hDC;
	page->canvas2Left = windowRect.left;
	page->canvas2Top = windowRect.top;
	page->canvas2Width = clientRect.right;
	page->canvas2Height = clientRect.bottom;

	BitBlt(hDC, 0, 0, clientRect.right, clientRect.bottom, hdcOffScreen2, 0, 0, SRCCOPY);

}


void setScreenSizeYoutube(Sheet* pg) {
	SetWindowPos(pg->hWnd, NULL, 200, 200, 1294, 726, 0);
}

// ************************************

void doubleClickDialog(int i) {

	page->activeDialogComponent = i;
	page->activeDialogButtonList = page->dgrm.cL[i]->dialogButtonList;

	drawCanvas();
}



void readComponentsAndWires(Sheet* pg) {
	
	if (pg->dgrm.cL.size() > 0) {

		if (checkIsDirty(pg)) {
			int quitResult = MessageBox(NULL, L"You have unsaved changes. Do you want to save them now?", 
				L"MST Workshop", MB_YESNOCANCEL | MB_ICONWARNING);
			if (quitResult == IDCANCEL)
				return;
			if (quitResult == IDYES)
				writeComponentsAndWires(pg);
			else 
				clearAllDirty();
		}
	}

	


	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			pFileOpen->SetFileTypes(4, rgSpec);
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						// got the name now open the file
						const int size = 5000;
						char buffer[size];
						std::ifstream cin(pszFilePath, ios::in);
						if (cin) {
							deleteAll(pg);

							while (!cin.eof()) {
								cin.getline(buffer, size);
								if( strlen(buffer) > 10)	
									loadSolution_(pg, buffer, false);
							}
							
							analyzeConnections(&pg->dgrm);
							//propagateValues(pg);
							drawCanvas();

							clearAllSaveBuffers();
							setVisibleScreenDirty("file loaded");
							clearAllDirty();
						}
						
						int q = 0;
						pg->filePath = SysAllocStringLen(0, 100);
						while (pszFilePath[q] && (q < 95)) {
							pg->filePath[q] = pszFilePath[q];
							q++;
						}
						pg->filePath[q] = 0;

						setTitleFromPathName(pg);
					
						//MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}	
}



void readComponentsAndWiresFromString(Sheet* pg, string theString) {

	const int size = 500;
	char buffer[size];

	innerDeleteAll(pg);

	bool done = false;
	size_t end;
	while (!done) {

		end = theString.find('\n');
		strncpy_s(buffer, size, theString.c_str(), end);
		theString = theString.substr(end+1, string::npos);
	
		if (strlen(buffer) > 10)
			loadSolution_(pg, buffer, false);
		if( theString.size() < 5 )
			done = true;
	}
		
}


PWSTR get_a_file_Path() { // using the Save dialog

	PWSTR pszFilePath = NULL;
	bool success = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem *pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						success = true;
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}
	if (success)
		return pszFilePath;
	else
		return NULL;
}

// **************************** Test all the components ************************

void componentMemoryTest(Sheet* pg) {

	OutputDebugString(L"\n\nStarting Test ");
	
	
	innerDeleteAll(pg);
	int q = 0;

	POS testComp;
	testComp.x = 300;
	testComp.y = 300;

	for (int j = 0; j < pg->buttonList.size(); j++) {

		if (!loolupDetailIsComponent(pg->buttonList[j]->componentName))
			continue;

		OutputDebugString(L"\n");
		string deb = tomString(q++);
		OutputDebugString(ConvertToLPWSTR(deb));
		OutputDebugString(L" ");
		OutputDebugString(pg->buttonList[j]->visibleText);
		OutputDebugString(L"\n");

#if MEMORY_CHECK
		startMemCheck();
#endif
		// place the component
		AddComponent(pg, pg->buttonList[j]->componentName, testComp, 0);
		// clean up
		deleteComponent(pg, 0);
		pg->dgrm.cL.clear();
		pg->draggingComponent = -1;
#if MEMORY_CHECK
		endMemCheck();
#endif
	}
	OutputDebugString(L"\nDONE.\n");
	MessageBox(NULL, L"All components tested.", L"Memory Test", MB_OK);
	

}




// ************************** random test **********************************************

int allLowBits[17] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F, 0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };
int configBits[17] = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };


bool pickComponent(Sheet* pg, int* compj) {

	do {
		(*compj)++;
		if (*compj >= pg->buttonList.size())
			return false; //  we are done

		if (!loolupDetailIsComponent(pg->buttonList[*compj]->componentName))
			continue; // not a component

		if (z(pg->buttonList[*compj]->componentName, L"ImCamera"))
			continue; // cant analze the camera component

		return true;

	} while (true);
}

bool pickTerminalConfiguration(Sheet* pg, int TEST, int* termj) {
	
	(*termj)++;

	if (pg->dgrm.cL[TEST]->pinMaskIndex == -1) 
		return (*termj == 0); // run just one test

	vector<int> localPinGroups = pg->dgrm.cL[TEST]->GetPinGroups();
	int groups = localPinGroups.size();
	if (*termj >= configBits[groups])
		return false;

	if (*termj == 0)
		(*termj)++;
	pg->dgrm.cL[TEST]->setting[pg->dgrm.cL[TEST]->pinMaskIndex] = *termj;
	pg->dgrm.cL[TEST]->onLoad(pg);
	return true;
}

bool pickInputConfiguration(int* list, int listSize, int listRange ) {

	int j = 0;
	do {
		list[j]++;

		if (list[j] < listRange)
			return true;

		list[j++] = 0;
		if (j >= listSize)
			return false;
		
	} while (true);

}

void componentRandomTest(Sheet* pg) {
	OutputDebugString(L"\n\nStarting Random Test ");

	innerDeleteAll(pg);
	int q = 0;
	int n = 0;

	// ************************ build sources ***********************
	const int SOURCES = 7;
	POS testSource[SOURCES];
	for (int j = 0; j < SOURCES; j++) {
		testSource[j].x = 100;
		testSource[j].y = 100+100*j;
		if (j == 6) {
			testSource[j].x = testSource[j-1].x + 2*(COMPONENT_WIDTH + TERMINAL_SIZE + 1);
			testSource[j].y = testSource[j-1].y;
		}
	}
													// 0) no connection
	AddComponent(pg, L"Con",    testSource[1], 0);	// 1) simple test
	AddComponent(pg, L"Table",  testSource[2], 0);	// 2) test value array
	AddComponent(pg, L"Lit",    testSource[3], 0);	// 3) test string array
	AddComponent(pg, L"Gather", testSource[4], 0);	// 4) test null 
	AddComponent(pg, L"Inv",    testSource[5], 0);	// 5) test infinity

	AddComponent(pg, L"Neg", testSource[6], 0);	// 5) test infinity

	deactivateAllButtons(pg);
	drawCanvas();

	string testName[] = { "N/C  ", "0    ", "Tab  ", "Lit  ", "void ", "+Inf ", "-Inf " };

	int testSourceOutputX[SOURCES];
	int testSourceOutputY[SOURCES];
	testSourceOutputX[0] = testSource[0].x;
	testSourceOutputY[0] = testSource[0].y;
	for (int j = 1; j < SOURCES; j++) {
		testSourceOutputX[j] = pg->dgrm.cL[j-1]->rightTerminalX(0);
		testSourceOutputY[j] = pg->dgrm.cL[j-1]->rightTerminalY(0);
	}

	const int TEST = SOURCES-1;
	const int TESTS = 12;

	POS testComp;
	testComp.x = 500;
	testComp.y = 300;

	int testCompInputX;
	int testCompInputY;

	int compj = -1; // sequence through all components in the menus
	while( pickComponent(pg, &compj) ){ 
		
		// place the component under test
		AddComponent(pg, pg->buttonList[compj]->componentName, testComp, 0);

		if (pg->dgrm.cL[TEST]->leftTerminalCount) { // we need input terminals to test it
			
			int termj = -1; // if there are pin options, sequence through them all
			while (pickTerminalConfiguration(pg, TEST, &termj)) {

				OutputDebugString(L"\n");
				q++;
				OutputDebugString(ConvertToLPWSTR(tomString(compj)));
				OutputDebugString(L" ");
				OutputDebugString(pg->buttonList[compj]->visibleText);
				OutputDebugString(L"   ");

				int inputs = pg->dgrm.cL[TEST]->leftTerminalCount;
				OutputDebugString(L" Config: ");
				OutputDebugString(ConvertToLPWSTR(tomString(termj)));
				OutputDebugString(L" Inputs: ");
				OutputDebugString(ConvertToLPWSTR(tomString(inputs)));
				OutputDebugString(L"\n");

				int list[20];
				for (int j = 0; j < inputs; j++)
					list[j] = 0;
				list[0] = -1;

				while( pickInputConfiguration(list, inputs, SOURCES)) {

					// connect wires to all input terminals
					OutputDebugString(L"     ");
					for (int inPin = 0; inPin < inputs; inPin++) {
						// wire each input in turn
						testCompInputX = pg->dgrm.cL[TEST]->leftTerminalX(inPin);
						testCompInputY = pg->dgrm.cL[TEST]->leftTerminalY(inPin);

						OutputDebugString(ConvertToLPWSTR(testName[list[inPin]]));

						AddWire(pg, testSourceOutputX[list[inPin]], testSourceOutputY[list[inPin]], testCompInputX, testCompInputY);
					}

					// test it
					analyzeConnections(&pg->dgrm);
					propagateValues(&pg->dgrm);
					drawCanvas();
					OutputDebugString(L" -- Passed\n");
					n++;

					// clean up wires
					for (int i = 0; i < pg->dgrm.wL.size(); i++)
						delete pg->dgrm.wL[i];
					pg->dgrm.wL.clear();
				}
			}
		}

		// clean up component
		deleteComponent(pg, TEST); 
	}
	innerDeleteAll(pg);
	pg->draggingComponent = -1;
	drawCanvas();

	OutputDebugString(L"\nA total of ");
	string count = tomString(n);
	OutputDebugString(ConvertToLPWSTR(count));
	OutputDebugString(L" tests were run.");

	MessageBox(NULL, L"All components passed random test.", L"Random Test", MB_OK);
}




#include <time.h>


void componentFastRandomTest(Sheet* pg) {
	OutputDebugString(L"\n\nStarting Fast Random Test ");

	Open2DView();
	Open3DView();
	BringWindowToTop(pg->hWnd);// pCanvasWindow->GetHWND());

	innerDeleteAll(pg);
	int q = 0;
	int n = 0;

	// ************************ build sources ***********************

	const int SOURCES = 8;
	POS testSource[SOURCES];
	for (int j = 0; j < SOURCES; j++) {
		testSource[j].x = 100;
		testSource[j].y = 100+80*j;
		if (j == 7) {
			testSource[j].x = testSource[j-1].x + 2*(COMPONENT_WIDTH + TERMINAL_SIZE + 1);
			testSource[j].y = testSource[j-1].y;
		}
	}
													// 0) no connection
	AddComponent(pg, L"Time",   testSource[1], 0);	// 1) simple test
	AddComponent(pg, L"Table",  testSource[2], 0);	// 2) test value array
	AddComponent(pg, L"3ROTX",  testSource[3], 0);	// 3) test matrix
	AddComponent(pg, L"Lit",    testSource[4], 0);	// 4) test string array
	AddComponent(pg, L"Gather", testSource[5], 0);	// 5) test null
	AddComponent(pg, L"Inv",    testSource[6], 0);	// 6) test infinity
	AddComponent(pg, L"Neg",	testSource[7], 0);	// 7) test infinity
	
	deactivateAllButtons(pg);
	drawCanvas();

	string testName[] = { "N/C  ", "Time ", "Tab  ", "3Rot ", "Lit  ", "void ", "+Inf ", "-Inf " };


	int testSourceOutputX[SOURCES];
	int testSourceOutputY[SOURCES];
	testSourceOutputX[0] = testSource[0].x;
	testSourceOutputY[0] = testSource[0].y;
	for (int j = 1; j < SOURCES; j++) {
		testSourceOutputX[j] = pg->dgrm.cL[j-1]->rightTerminalX(0);
		testSourceOutputY[j] = pg->dgrm.cL[j-1]->rightTerminalY(0);
	}

	const int TEST = SOURCES-1;
	const int TESTS = 2;

	POS testComp;
	testComp.x = 500;
	testComp.y = 300;

	int testCompInputX;
	int testCompInputY;
     
	srand(time(NULL));

	// ************** open each component for random test ********************
	//startClock(pg);

	for (int j = 0; j < pg->buttonList.size(); j++) {

		if (!loolupDetailIsComponent(pg->buttonList[j]->componentName))
			continue;
		if (z(pg->buttonList[j]->componentName, L"ImCamera"))
			continue;

		// place the component under test
		AddComponent(pg, pg->buttonList[j]->componentName, testComp, 0);

		if (pg->dgrm.cL[TEST]->leftTerminalCount) {
			OutputDebugString(L"\n");
			string deb = tomString(j);
			q++;
			OutputDebugString(ConvertToLPWSTR(deb));
			OutputDebugString(L" ");
			OutputDebugString(pg->buttonList[j]->visibleText);
			OutputDebugString(L"\n");

			// check for hidden inputs
			if (pg->dgrm.cL[TEST]->pinMaskIndex != -1) {
				vector<int> localPinGroups = pg->dgrm.cL[TEST]->GetPinGroups();
				int groups = localPinGroups.size();
				int maxPinBits = allLowBits[groups];
				pg->dgrm.cL[TEST]->setting[pg->dgrm.cL[TEST]->pinMaskIndex] = maxPinBits;
				pg->dgrm.cL[TEST]->onLoad(pg);
			}

			int inputs = pg->dgrm.cL[TEST]->leftTerminalCount;

			for (int t = 0; t < TESTS * inputs; t++) {

				// connect wires to all input terminals
				OutputDebugString(L"     ");
				for (int inPin = 0; inPin < pg->dgrm.cL[TEST]->leftTerminalCount; inPin++) {
					// wire each input in turn
					testCompInputX = pg->dgrm.cL[TEST]->leftTerminalX(inPin);
					testCompInputY = pg->dgrm.cL[TEST]->leftTerminalY(inPin);

					// randomly choose a source
					int source = rand() % SOURCES;
					//string src = tomString(source);
					OutputDebugString(ConvertToLPWSTR(testName[source]));

					AddWire(pg, testSourceOutputX[source], testSourceOutputY[source], testCompInputX, testCompInputY);
				}

				// test it
				analyzeConnections(&pg->dgrm);
				propagateValues(&pg->dgrm);
				drawCanvas();
				OutputDebugString(L" -- Passed\n");
				n++;

				// clean up wires
				for (int i = 0; i < pg->dgrm.wL.size(); i++)
					delete pg->dgrm.wL[i];
				pg->dgrm.wL.clear();
			}
		
		}

		// clean up component
		deleteComponent(pg, TEST);
	}
	innerDeleteAll(pg);
	pg->draggingComponent = -1;
	drawCanvas();

	OutputDebugString(L"\nA total of ");
	string count = tomString(n);
	OutputDebugString(ConvertToLPWSTR(count));
	OutputDebugString(L" tests were run.");

	MessageBox(NULL, L"All components passed random test.", L"Random Test", MB_OK);
}

