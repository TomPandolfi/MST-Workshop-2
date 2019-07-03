#include "stdafx.h"
#include <cmath>
#include "structs.h"
#include "Sheet.h"
#include "FunctionComponents.h"
#include "buildComponent.h"
#include "ArrayComponents.h"
#include <string>
#include "Text.h"
#include <ctime>
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include "Time.h"
#include "color.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)


double reasonableValue(double in) { return in; } // ****************************** for now





double funcDetent(double value, double detent) {
	if (detent)
		return floor((value + detent / 2) / detent) * detent;
	else
		return value;
}

double funcReasonable(double value) {
	return 0.0;//reasonableValue(value);
}





//************************************************************************
//******************************************************************88

void dialNull_(Sheet* pg, int i) {
}
double detentValue(double inputValue, double detentValue) {
	if (detentValue) {
		double temp = floor((inputValue + detentValue / 2.0) / detentValue);
		return reasonableValue(temp * detentValue);
	}
	else
		return inputValue;
}


void drawResizeBox(Sheet* pg, Component *comp) {

	HBRUSH fillStyle = GetColorBrush(0xFFFFFF); //=CreateSolidBrush '#FFFFFF';

	int x = pg->cXo + comp->x + comp->width;
	int y = pg->cYo + comp->y + comp->height;
	BeginPath(pg->hDC);
	MoveToEx(pg->hDC, x, y, NULL);
	LineTo(pg->hDC, x - RESIZE_HANDLE_SIZE, y);
	LineTo(pg->hDC, x, y - RESIZE_HANDLE_SIZE);
	LineTo(pg->hDC, x, y);
	EndPath(pg->hDC);
	StrokeAndFillPath(pg->hDC);
}


void commonDialCon(Sheet* pg, Component *comp) {
	int x = comp->x - comp->width + 5;
	int y = comp->y - comp->height + 5;

	//if (pg->dgrm.cL[i]->memory[0] >= 0) {
	//	dialUniversalText(ctx, pg->cXo + x - 5, pg->cYo + y - 5, 2 * pg->dgrm.cL[i]->width, 2 * pg->dgrm.cL[i]->height, pg->dgrm.cL[i]->memory[1], pg->dgrm.cL[i]->memory[0]);
	//	return;
	//}

	drawResizeBox(pg, comp);
	std::string text = tomString(comp->setting[0]);
	//text = tomString( pg->dgrm.cL[i]->setting[0]);
	if (comp->type == L"ConOff") {
		if (comp->setting[0] > 0)
			text.insert(0, "+ ");
	}else{
		if(comp->type == L"ConFac")
			text.insert(0, "* ");
	}
	int k = text.size();
	//HBRUSH fillStyle = GetColorBrush(0x000000);//CreateSolidBrush
	//DeleteObject(fillStyle);

	// single line display
	std::string textout = trimTextToFit(pg->hDC, text, 2 * comp->width - 12);
	drawTextString(pg->hDC, pg->cXo + x, pg->cYo + y - 3, 0, textout);
	y += 10;
}

void dialGenericLogic_(Sheet* pg, int i) {

	// output indicator
	if (pg->dgrm.cL[i]->output[0].value.size() != 1)
		return;

	HBRUSH fillStyle;
	if (pg->dgrm.cL[i]->output[0].value[0])
		fillStyle = GetColorBrush(0xFFFFFF); // CreateSolidBrush
	else
		fillStyle = GetColorBrush(0x000000); // CreateSolidBrush

	int x = pg->cXo + pg->dgrm.cL[i]->x + pg->dgrm.cL[i]->width - 16;
	int y = pg->cYo + pg->dgrm.cL[i]->y;

	RECT aRect;
	aRect.left = x - 5;
	aRect.top = y - 5;
	aRect.right = x + 5;
	aRect.bottom = y + 5;

	FillRect(pg->hDC, &aRect, fillStyle);
	//DeleteObject(fillStyle);
#if 0
	
	if (pg->dgrm.cL[i]->output[0].value[0])
		SetDCBrushColor(pg->hDC, 0xFFFFFF);
		//HBRUSH fillStyle = CreateSolidBrush(0xFFFFFF);
	else
		SetDCBrushColor(pg->hDC, 0x000000);
		//HBRUSH fillStyle = CreateSolidBrush(0x000000);

	int x = pg->cXo + pg->dgrm.cL[i]->x + pg->dgrm.cL[i]->width - 16;
	int y = pg->cYo + pg->dgrm.cL[i]->y;

	BeginPath(pg->hDC);
	Ellipse(pg->hDC, x - 5, y - 5, x + 5, y + 5);
	EndPath(pg->hDC);
	StrokeAndFillPath(pg->hDC);
#endif
}

double overScale(double value, double scale) {
	double temp = value / scale;
	double absScale = abs(scale);
	double power = 1.0;
	for (int j = 0; j < 10; j++) {
		if (absScale < power)
			return round(temp * power) / power;
		power *= 10;
	}
	return temp;
}


void onMouseUpSld_(Sheet* pg, int i) {
	//if ((int)pg->dgrm.cL[i]->setting[5] & 2) { // continuous mode
	//	double semiRange = (pg->dgrm.cL[i]->setting[1] - pg->dgrm.cL[i]->setting[0]) / 2;
	//	pg->dgrm.cL[i]->setting[0] = reasonableValue(pg->dgrm.cL[i]->setting[2] - semiRange);
	//	pg->dgrm.cL[i]->setting[1] = reasonableValue(pg->dgrm.cL[i]->setting[2] + semiRange);
	//	drawCanvas();
	//}
}





//bool methodMultiplex_(Sheet* pg, int i) {
//	
//	if (pg->dgrm.cL[i]->input[2].drivenByComponent == -1) {
//		// no control input is the same as a zero
//		if (pg->dgrm.cL[i]->input[0].drivenByComponent == -1)
//			return clearOutputs_(pg, i);
//
//		int len = pg->dgrm.cL[i]->input[0].value.size();
//		pg->dgrm.cL[i]->output[0].value.resize( len);
//		for (int j = 0; j < len; j++) // copy the input to output
//			pg->dgrm.cL[i]->output[0].value[j] = pg->dgrm.cL[i]->input[0].value[j];
//		copyDimensions(pg->dgrm.cL[i], 0, 0);
//		return true;
//	}
//	int controlSize = pg->dgrm.cL[i]->input[2].value.size();
//	int sel;
//	if (controlSize == 1) { // control is a scalar
//		if (pg->dgrm.cL[i]->input[2].value[0])
//			sel = 1;
//		else
//			sel = 0;
//		if (pg->dgrm.cL[i]->input[sel].drivenByComponent == -1)
//			return clearOutputs_(pg, i);
//
//		int len = pg->dgrm.cL[i]->input[sel].value.size();
//		pg->dgrm.cL[i]->output[0].value.resize(len);
//		for (int j = 0; j < len; j++) // copy the input to output
//			pg->dgrm.cL[i]->output[0].value[j] = pg->dgrm.cL[i]->input[sel].value[j];
//		copyDimensions(pg->dgrm.cL[i], sel, 0);
//	}
//	else { // control is an array
//		int leng[2];
//		for (int j = 0; j < 2; j++)
//			if (pg->dgrm.cL[i]->input[j].drivenByComponent != -1)
//				leng[j] = pg->dgrm.cL[i]->input[j].value.size();
//			else
//				leng[j] = 0;
//
//		if (((leng[0] != 1) && (leng[0] < controlSize)) ||
//			((leng[1] != 1) && (leng[1] < controlSize)))
//			return clearOutputs_(pg, i);
//
//		pg->dgrm.cL[i]->output[0].value.resize( controlSize);
//		// pick one of the inputs for output the dimensions
//		copyDimensions(pg->dgrm.cL[i], 2, 0);
//		if (pg->dgrm.cL[i]->input[1].value.size() == controlSize)
//			copyDimensions(pg->dgrm.cL[i], 1, 0);
//		if (pg->dgrm.cL[i]->input[0].value.size() == controlSize)
//			copyDimensions(pg->dgrm.cL[i], 0, 0);
//
//		for (int j = 0; j < controlSize; j++) { // copy the inputs to output
//			if (pg->dgrm.cL[i]->input[2].value[j])
//				sel = 1;
//			else
//				sel = 0;
//			if (leng[sel] == 1)
//				pg->dgrm.cL[i]->output[0].value[j] = pg->dgrm.cL[i]->input[sel].value[0];
//			else
//				pg->dgrm.cL[i]->output[0].value[j] = pg->dgrm.cL[i]->input[sel].value[j];
//		}
//	}
//	return true;
//}


bool commonCrankTable(Sheet* pg, Component *comp, int x, int y, bool check) {

	if (!comp->crankResize(x, y, check))
		return false;

	comp->setting[0] = comp->width;
	comp->setting[1] = comp->height;

	// because the output terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//drawCanvas2();

	// move the dial
	comp->dial(pg);

	return true;
}

int getGrayInfo(Component *comp, int pin, int *widthPtr, int *heightPtr) {
	if (comp->input[pin].drivenByComponent == -1) {
		*widthPtr = 0;
		*heightPtr = 0;
		return 0;
	}

	int pixels;
	if(comp->input[pin].stringType )
		pixels = comp->input[pin].stringValue.size();
	else
		pixels = comp->input[pin].value.size();

	int dims = comp->input[pin].dimensions.size();
	if (dims > 2) {
		*widthPtr = 0;
		*heightPtr = 0;
		return 0;
	}

	int width;
	int height;
	if (dims == 2) {
		width = comp->input[pin].dimensions[0];
		height = comp->input[pin].dimensions[1];
		if (pixels != width * height) {
			*widthPtr = 0;
			*heightPtr = 0;
			return 0;
		}
	}
	else {
		width = comp->input[pin].dimensions[0];
		height = 1;
		if (pixels != width) {
			*widthPtr = 0;
			*heightPtr = 0;
			return 0;
		}
	}

	*widthPtr = width;
	*heightPtr = height;
	return pixels;
}


void commonDialGrid(Sheet* pg, Component *comp) {

	// check for debugging a display
	//pg = getDebugInstancePage(pg, i); ***************** someday

	int x = comp->x - comp->width + 5;
	int y = comp->y - comp->height + 5;
	int w = 2 * comp->width - 10;
	int h = 2 * comp->height - 10;

	drawResizeBox(pg, comp);

	int xWidth = 0;
	int yHeight = 0;
	if (comp->type == L"Grid") {
		if( getGrayInfo(comp, 0, &xWidth, &yHeight) == 0 )
		return;
	}
	if (comp->type == L"Matrix") {
		xWidth = comp->setting[2];
		yHeight = comp->setting[3];
		int values = xWidth * yHeight;
		if (values == 0)
			return;
	}

	int xx = pg->cXo + x;
	int top = pg->cYo + y;

	if ((comp->width / xWidth < 15) ||
		(comp->height / yHeight < 10)) {
		//ctx.fillStyle = '#000000';
		if ((w < 60) || (h < 16)) {
			//ctx.fillText("***", xx + 4, top + 12);
			drawTextString(pg->hDC, xx + 4, top + 0, 0, "***");
			return;
		}
		//text = '[' + xWidth + 'x' + yHeight + ']';
		//ctx.fillText(text, xx + 4, top + 12);
		std::string text = "[";
		text.append(tomString(xWidth));
		text.append("x");
		text.append(tomString(yHeight));
		text.append("]");
		drawTextString(pg->hDC, xx + 4, top +0, 0, text);
		return;
	}

	//ctx.strokeStyle = '#000000';

	// draw the vertical grid
	int bot = top + h;
	for (int ix = 0; ix <= xWidth; ix++) {
		BeginPath(pg->hDC);
		MoveToEx(pg->hDC, xx, top, NULL);
		LineTo(pg->hDC, xx, bot);
		EndPath(pg->hDC);
		StrokePath(pg->hDC);
		xx += w / xWidth;
	}

	// draw the horizontal grid
	int yy = pg->cYo + y;
	int lef = pg->cXo + x;
	int rig = lef + w;
	for (int iy = 0; iy <= yHeight; iy++) {
		BeginPath(pg->hDC);
		MoveToEx(pg->hDC, lef, yy, NULL);
		LineTo(pg->hDC, rig, yy);
		EndPath(pg->hDC);
		StrokePath(pg->hDC);
		yy += h / yHeight;
	}

	// put in the values
	bool isString = false;
	if(comp->input.size() )
		isString = comp->input[0].stringType;

	int maxWid = w / xWidth - 12;
	yy = pg->cYo + y;
	int j = 0;
	std::string text;
	for (int iy = 0; iy < yHeight; iy++) {
		xx = pg->cXo + x;
		for (int ix = 0; ix < xWidth; ix++) {
			if (comp->type == L"Grid") {
				if( isString )
					text = comp->input[0].stringValue[j];
				else
					text = tomString(comp->input[0].value[j]);
			}
			if (comp->type == L"Matrix")
				text = tomString(comp->setting[j + 4]);
			int k = text.size();
			bool showLineEllipsis = false;
			//while ((ctx.measureText(text.substr(0, k)).width > maxWid) && (k > 1)) {
			//	k--;
			//	showLineEllipsis = true;
			//	if (text.indexOf('e') != -1)
			//		text = '';
			//}
			//text = text.substr(0, k);
			//if (showLineEllipsis)
			//	text += "...";
			std::string textout = trimTextToFit(pg->hDC, text, maxWid);
			drawTextString(pg->hDC, xx + 4, yy + 2, 0, textout);
			xx += w / xWidth;
			j++;
		}
		yy += h / yHeight;
	}

}



// *************** Round *******************

double Round_Component::innerMethodN(double *a) {
	double factor = pow(10, round(a[1]));
	return round(factor * a[0]) / factor;
}
Round_Component::Round_Component()
	: FunctionN_Component(L"Round", L"Round", squareShape, 2, 1, 1, 1)
{
	setLeftLabels(3, "In", "Places");
}

// *************** Precision *******************

double Precision_Component::innerMethodN(double *a) {
	int prec = floor(a[1]);
	if (prec < 1)
		return a[0];
	if (prec > 21)
		return a[0];

	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << a[0];
	std::string s = stream.str();

	return tomNumber(s);
}
Precision_Component::Precision_Component()
	: FunctionN_Component(L"Precision", L"Precision", squareShape, 2, 1, 1, 1)
{
	setLeftLabels(3, "In", "Precision");
}

// *************** detent *******************

double Detent_Component::innerMethodN(double *value) {
	if (value[1] != 0.0)
		return reasonableValue(floor((value[0] + value[1] / 2) / value[1]) * value[1]);
	else
		return value[0];
}
Detent_Component::Detent_Component()
	: FunctionN_Component(L"Detent", L"Detent", squareShape, 2, 1, 1, 1)
{
	setLeftLabels(3, "In", "Detent");
}

// *************** reasonable *******************

double Reasonable_Component::innerMethodN(double *value) {
	return reasonableValue(value[0]);
}
Reasonable_Component::Reasonable_Component()
	: FunctionN_Component(L"Reasonable", L"Reasonable", squareShape, 1, 1, 1, 1)
{
}

// *************** Limit *******************

double Limit_Component::innerMethodN(double *value) { // value[0] is MAX,  value[1] is input,  value[2] is MIN, 
	if (value[0] < value[2])
		return 0.0; // invalid min and max inputs
	if (value[1] > value[0])
		 return value[0];	// clip at max
	if (value[1] < value[2])
		 return value[2]; // clip at min
	return value[1];
}
Limit_Component::Limit_Component()
	: FunctionN_Component(L"Limit", L"Limit", squareShape, 3, 1, 1, 1)
{
	setLeftLabels(1, "Max", "Input", "Min");
}


// *************************** Flip Flop ***********************************

// mem bit 0 with value 1 is the toggle memory
// mem bit 1 with value 2 is set edge memory
// mem bit 2 with value 4 is reset edge memory
double Flip_Component::innerMethodNMO(Sheet* pg, double *input, double *memory, double oldOutput ){//(oldOut, mem, j, set, trigset, toggle, trigres, res) {
	// input:  [0]Set, [1]Edge Set, [2]Edge Toggle, [3]Edge Reset, [4]Reset
	// First check the edge triggered inputs - acknowledge them but dont use them yet
	int mem = floor(*memory);
	int k;
	bool ToggleEdgeDetected = false;
	bool TrigSetEdgeDetected = false;
	bool TrigResetEdgeDetected = false;

	if (input[1]) { // trigger was high
		k = mem & 2;
		if (k == 0) // trigset was false
		{
			mem |= 2;    // set trigset memory true
			TrigSetEdgeDetected = true;
		}
	}
	else {
		mem &= 1 | 4; // clear the trigset memory
		TrigSetEdgeDetected = false;
	}

	if (input[3]) {
		k = mem & 4;
		if (k == 0) // trigres was false
		{
			mem |= 4;    // set trigres memory true
			TrigResetEdgeDetected = true;
		}
	}
	else {
		mem &= 1 | 2; // clear the toggle memory
		TrigResetEdgeDetected = false;
	}

	if (input[2]) {
		k = mem & 1;
		if (k == 0) // toggle was false
		{
			mem |= 1;    // set toggle memory true
			ToggleEdgeDetected = true;
		}
	}
	else {
		mem &= 2 | 4; // clear the toggle memory
		ToggleEdgeDetected = false;
	}
	*memory = mem; // update the memory

	// now check the non edge inputs and do what they say

	if (input[0] && input[4]) // Deadlock - no change
		return oldOutput;

	if (input[0]) // set
		return 1;

	if (input[4])  // reset
		return 0;

	// finally - we can do what the edge triggered inputs say.

	if (TrigSetEdgeDetected && TrigResetEdgeDetected) // Deadlock - no change
		return oldOutput;

	if (TrigSetEdgeDetected) // so we set
	{
		if (ToggleEdgeDetected)
			return 0; // the toggle reverses it
		else
			return 1;
	}
	if (TrigResetEdgeDetected) // so we reset
	{
		if (ToggleEdgeDetected)
			return 1; // the toggle reverses it
		else
			return 0;
	}
	if (ToggleEdgeDetected)
		if (oldOutput) // finally a simple toggle
			return 0;
		else
			return 1;

	return oldOutput;
}
Flip_Component::Flip_Component()
	: FunctionNMO_Component(L"Flip", L"Flip Flop", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "Set", "Edge Set", "Edge Toggle", "Edge Reset", "Reset");
	width = 35;
	output[0].value[0] = 0;
}


// *************** D Flip Flop *************************

// mem bit 0 with value 1 is the trigger memory
double DFlipFlop_Component::innerMethodNMO(Sheet* pg, double *input, double *memory, double oldOutput){//(oldOut, mem, j, set, data, trigger, res) {
	// input is [0]Set, [1]Data, [2]Trigger, [3]Reset
	// First check the edge triggered input - acknowledge it but dont use it yet
	for( int j = 0; j < 4; j++)
		if (isinf(input[j]))
			return 0;
	bool TriggerEdgeDetected = false;
	int mem = floor(*memory);

	if (input[2]) {
		if (mem == 0) // trigset was false
		{
			mem = 1;    // set trigset memory true
			TriggerEdgeDetected = true;
		}
	}
	else {
		mem = 0; // clear the trigger memory
		TriggerEdgeDetected = false;
	}

	*memory = mem; // update the memory

	// now check the non edge inputs and do what they say

	if (input[0] && input[3]) // Deadlock - no change
		return oldOutput;

	if (input[0]) // set
		return 1;

	if (input[3])  // reset
		return 0;

	// finally - we can do what the edge triggered inputs say.

	if (TriggerEdgeDetected) { // so we read the input data
		if (input[1])
			return 1;
		else
			return 0;
	}

	return oldOutput; // no change
}
DFlipFlop_Component::DFlipFlop_Component()
	: FunctionNMO_Component(L"DFlip", L"D Flip Flop", squareShape, 4, 1, 1, 1)
{
	setLeftLabels(3, "Set", "Data", "Trigger", "Reset");
	width = 35;
	output[0].value[0] = 0;
}

// ******************** trigger Sum *****************************


double TriggeredSum_Component::innerMethodNMO(Sheet* pg, double *input, double *memory, double oldOutput) {
	// inputs are [0]Input, [1]Trigger, [2]Reset
	
	double output = oldOutput;
	if (input[2] > 0.0) {
		output = 0.0;
	}
	else {
		// look for rising edge
		if ((input[1] > 0) && (*memory == 0.0)) 
			output += input[0];
	}

	if (input[1] <= 0)
		*memory = 0.0;
	else
		*memory = 1.0;
	return output; 
}
TriggeredSum_Component::TriggeredSum_Component()
	: FunctionNMO_Component(L"TrigSum", L"Triggered Sum", squareShape, 3, 1, 1, 1)
{
	setLeftLabels(3, "Input", "Trigger", "Reset");
}


