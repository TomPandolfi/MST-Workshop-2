#include "stdafx.h"
#include "MethodComponents.h"
#include "ArrayComponents.h"
#include "FunctionComponents.h"
#include "Sheet.h"
#include <ctime>
#include "Time.h"
#include "Text.h"
#include <string>
#include <locale>
#include "ScreenDialog.h"
#include "color.h"
#include <algorithm>    // std::sort
#include "Mouse.h"
#include "ButtonGroups.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

extern Sheet* page;
extern vector< InnerSheet*> isL;

extern void rotationActionGeneral(Sheet* pg, int i);

bool commonCrankHorizontalResizeHoldLeft(Sheet* pg, Component *comp, int ax, int ay, bool check, int minimumWidth) {

	if (check) {
		int left = comp->x + comp->width - RESIZE_HANDLE_SIZE;
		if (ax < left)
			return false; // missed
		int top = comp->y + comp->height - RESIZE_HANDLE_SIZE;
		if (ay < top)
			return false; // missed
	}

	int delta = ax - (comp->x + comp->width - RESIZE_HANDLE_SIZE / 2);
	comp->width = floor(comp->width + delta / 2);
	if (comp->width < minimumWidth)
		comp->width = minimumWidth;
	else
		comp->x = floor(comp->x + delta / 2);

	return true;
}





// ************************* Time ***************************************

int Time_Component::method2(Sheet* pg) {
	if (output[0].value[0] != pg->systemTime) {
		output[0].value[0] = pg->systemTime;
		return true;
	}
	return false;
}
Time_Component::Time_Component()
	: Component(L"Time", L"Time", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ************************* Year ***************************************

int Year_Component::method2(Sheet* pg) {
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);

	double year = buf.tm_year + 1900.0;
	if (output[0].value[0] != year) {
		output[0].value[0] = year;
		return true;
	}
	return false;
}
Year_Component::Year_Component()
	: Component(L"Year", L"Year", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ************************* Month ***************************************

int Month_Component::method2(Sheet* pg) {
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);

	double month = buf.tm_mon + 1;
	if (output[0].value[0] != month) {
		output[0].value[0] = month;
		return true;
	}
	return false;
}
Month_Component::Month_Component()
	: Component(L"Month", L"Month", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ************************* Day ***************************************

int Day_Component::method2(Sheet* pg) {
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);

	double day = buf.tm_mday;
	if (output[0].value[0] != day) {
		output[0].value[0] = day;
		return true;
	}
	return false;
}
Day_Component::Day_Component()
	: Component(L"Day", L"Day", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ************************* Hour ***************************************

int Hour_Component::method2(Sheet* pg) {
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);

	double hour = buf.tm_hour;
	if (output[0].value[0] != hour) {
		output[0].value[0] = hour;
		return true;
	}
	return false;
}
Hour_Component::Hour_Component()
	: Component(L"Hour", L"Hour", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ************************* Minute ***************************************

int Minute_Component::method2(Sheet* pg) {
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);

	double minute = buf.tm_min;
	if (output[0].value[0] != minute) {
		output[0].value[0] = minute;
		return true;
	}
	return false;
}
Minute_Component::Minute_Component()
	: Component(L"Minute", L"Minute", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ************************* Second ***************************************

int Second_Component::method2(Sheet* pg) {
	time_t t = time(NULL);
	struct tm buf;
	localtime_s(&buf, &t);

	double second = buf.tm_sec;
	if (output[0].value[0] != second) {
		output[0].value[0] = second;
		return true;
	}
	return false;
}

Second_Component::Second_Component()
	: Component(L"Second", L"Second", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
}

// ********************************** Constant ***********************************************

int Constant_Component::method2(Sheet* pg) {
	output[0].value[0] = setting[0];
	return true;
}


void Constant_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = doubleValues[0];
	output[0].value[0] = doubleValues[0];
}
void doubleClickConstant(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Value:"  };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0]};

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

void setConstant(Sheet* pg, int i, double value) {
	pg->dgrm.cL[i]->setting[0] = value;
	pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[0];
	pg->dgrm.cL[i]->method2(pg);

	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}

void doubleClickConFt2M(Sheet* pg, int i)		{ setConstant(pg, i, 0.3048);}
void doubleClickConM2Ft(Sheet* pg, int i)		{ setConstant(pg, i, 3.28083989);}
void doubleClickConMi2Km(Sheet* pg, int i)		{ setConstant(pg, i, 1.60934);}
void doubleClickConKm2Mi(Sheet* pg, int i)		{ setConstant(pg, i, 0.621371);}
void doubleClickConLb2Kg(Sheet* pg, int i)		{ setConstant(pg, i, 0.453592);}
void doubleClickConKg2Lb(Sheet* pg, int i)		{ setConstant(pg, i, 2.204624);}
void doubleClickConDeg2Rad(Sheet* pg, int i)	{ setConstant(pg, i, DEGREES_TO_RADIANS);}
void doubleClickConRad2Deg(Sheet* pg, int i)	{ setConstant(pg, i, RADIANS_TO_DEGREES);}
void doubleClickConPi(Sheet* pg, int i)			{ setConstant(pg, i, PI);}
void doubleClickCon2Pi(Sheet* pg, int i)		{ setConstant(pg, i, TWO_PI);}
void doubleClickConPiOver2(Sheet* pg, int i)	{ setConstant(pg, i, PI_OVER_2);}
void doubleClickConE(Sheet* pg, int i)			{ setConstant(pg, i, 2.718281828459);}
void doubleClickConLog10(Sheet* pg, int i)		{ setConstant(pg, i, 2.302585092994);}
void doubleClickConERadMi(Sheet* pg, int i)		{ setConstant(pg, i, 3959);}
void doubleClickConGrav1(Sheet* pg, int i)		{ setConstant(pg, i, 9.80665);}
void doubleClickConLight(Sheet* pg, int i)		{ setConstant(pg, i, 299792458);}
void doubleClickConProt(Sheet* pg, int i)		{ setConstant(pg, i, 1.672621777E-27);}
void doubleClickConElect(Sheet* pg, int i)		{ setConstant(pg, i, 9.10938291E-31);}
void doubleClickConCoul(Sheet* pg, int i)		{ setConstant(pg, i, 8.9875517873681764e9);}
void doubleClickConGrav2(Sheet* pg, int i)		{ setConstant(pg, i, 6.67408e-11);}

void buttonCon_SetFromMouseX_Action(int i){}
void buttonCon_SetFromMouseY_Action(int i){}


// Display dialog and actions for the Constant popup buttons
std::vector<Button*> conButtonList = { new Button(3, 40, 120, 20, L"Enter Constant...", doubleClickConstant),
	new Button(3, 70, 165, 20, L"Pi", doubleClickConPi),
	new Button(3, 90, 165, 20, L"2 * Pi", doubleClickCon2Pi),
	new Button(3, 110, 165, 20, L"Pi / 2", doubleClickConPiOver2),
	new Button(3, 130, 165, 20, L"e", doubleClickConE),
	new Button(3, 150, 165, 20, L"log(10)", doubleClickConLog10),
	new Button(3, 170, 165, 20, L"Earth Radius (mi)", doubleClickConERadMi),
	new Button(3, 190, 165, 20, L"Speed of Light (m/sec)", doubleClickConLight),
	new Button(3, 210, 165, 20, L"Gravity (m/sec/sec)", doubleClickConGrav1),
	new Button(3, 230, 165, 20, L"Mass of Proton (kg)", doubleClickConProt),
	new Button(3, 250, 165, 20, L"Mass of Electron (kg)", doubleClickConElect),
	new Button(3, 270, 165, 20, L"Coulomb's (m/F)", doubleClickConCoul),
	new Button(3, 290, 165, 20, L"Gravitational (N-m2/kg2)", doubleClickConGrav2),

	new Button(-122, 40, 120, 20, L"Feet to Meters", doubleClickConFt2M),
	new Button(-122, 60, 120, 20, L"Meters to Feet", doubleClickConM2Ft),
	new Button(-122, 80, 120, 20, L"Miles to Kilom.", doubleClickConMi2Km),
	new Button(-122, 100, 120, 20, L"Kilom. to Miles", doubleClickConKm2Mi),
	new Button(-122, 120, 120, 20, L"Pound to Kilo", doubleClickConLb2Kg),
	new Button(-122, 140, 120, 20, L"Kilo to Pound", doubleClickConKg2Lb),
	new Button(-122, 160, 120, 20, L"Degree to Radian", doubleClickConDeg2Rad),
	new Button(-122, 180, 120, 20, L"Radian to Degree", doubleClickConRad2Deg),
	new Button(-122, 200, 120, 20, L"Use 2D mouse X", buttonCon_SetFromMouseX_Action),
	new Button(-122, 220, 120, 20, L"Use 2D mouse Y", buttonCon_SetFromMouseY_Action) };


void Constant_Component::dial(Sheet* pg) {
	commonDialCon(pg, this);
}
bool commonCrankCon(Sheet* pg, Component *comp, int ax, int ay, bool check) {

	if (!commonCrankHorizontalResizeHoldLeft(pg, comp, ax, ay, check, 15))
		return false;

	comp->setting[1] = comp->width;

	// because the output terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();

	// move the dial
	comp->dial(pg);

	return true;
}
bool Constant_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	return commonCrankCon(pg, this, ax, ay, check);
}

bool Constant_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 35);
	width = setting[1];
	output[0].value[0] = setting[0];
	return true;
}

Constant_Component::Constant_Component()
	: Component(L"Con", L"Constant", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = conButtonList;
	output[0].dimensions[0] = 1;
	setSetting(0, 0.0);
	height = MIN_COMPONENT_HEIGHT;
	width = 35;
	setSetting(1, width);
	output[0].value[0] = 0.0; 
}

// ************* Constant Factor **********************************

bool ConstantFactor_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	return commonCrankCon(pg, this, ax, ay, check);
}
double ConstantFactor_Component::innerMethodN( double *a) {
	return setting[0] * a[0];
}
void ConstantFactor_Component::dial(Sheet* pg) {
	commonDialCon(pg, this);
}
bool ConstantFactor_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 35);
	width = setting[1];
	output[0].value[0] = setting[0];
	return true;
}
void ConstantFactor_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = doubleValues[0];
	output[0].value[0] = doubleValues[0];
}
ConstantFactor_Component::ConstantFactor_Component()
	: FunctionN_Component(L"ConFac", L"Constant Factor", squareShape, 1, 1, 1, 1)
{
	dialogButtonList = conButtonList;
	setSetting(0, 1);   // constant factor

	height = MIN_COMPONENT_HEIGHT;
	width = 35;
	setSetting(1, width);
	setMemory(0, -1); // disable keyboard edit
	//setMemory(1, ''; // the temp text
}

// ************* Constant offset ******************************

bool ConstantOffset_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	return commonCrankCon(pg, this, ax, ay, check);
}
double ConstantOffset_Component::innerMethodN(double *a) {
	return setting[0] + a[0];
}
void ConstantOffset_Component::dial(Sheet* pg) {
	commonDialCon(pg, this);
}
bool ConstantOffset_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 35);
	width = setting[1];
	output[0].value[0] = setting[0];
	return true;
}
void ConstantOffset_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = doubleValues[0];
	output[0].value[0] = doubleValues[0];
}
ConstantOffset_Component::ConstantOffset_Component()
	: FunctionN_Component(L"ConOff", L"Constant Offset", squareShape, 1, 1, 1, 1)
{
	dialogButtonList = conButtonList;
	setSetting(0, 0);   // constant offset
	height = MIN_COMPONENT_HEIGHT;
	width = 35;
	setSetting(1, width);
	setMemory(0, -1); // disable keyboard edit
	//setMemory(1, ''; // the temp text
}


// ************************************* Slider ****************************************

void buttonSld_Completion(Sheet* pg, int i, double *values) {

	pg->dgrm.cL[i]->setting[2] = values[0];
	pg->dgrm.cL[i]->setting[0] = values[1];
	pg->dgrm.cL[i]->setting[1] = values[2];
	pg->dgrm.cL[i]->setting[4] = values[3];

	// check for bad limits
	if (pg->dgrm.cL[i]->setting[1] < pg->dgrm.cL[i]->setting[0]) {
		pg->dgrm.cL[i]->setting[0] = 0;
		pg->dgrm.cL[i]->setting[1] = 100;
		pg->dgrm.cL[i]->setting[2] = 0;
		if (pg->dgrm.cL[i]->type == L"Sld")// only output for slider not offset or factor
			pg->dgrm.cL[i]->output[0].value[0] = 0;
	}
	else // be sure setting is in limits
	{
		if (pg->dgrm.cL[i]->setting[2] < pg->dgrm.cL[i]->setting[0]) {
			pg->dgrm.cL[i]->setting[2] = pg->dgrm.cL[i]->setting[0];
			if (pg->dgrm.cL[i]->type == L"Sld")
				pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[0];
		}
		else if (pg->dgrm.cL[i]->setting[2] > pg->dgrm.cL[i]->setting[1]) {
			pg->dgrm.cL[i]->setting[2] = pg->dgrm.cL[i]->setting[1];
			if (pg->dgrm.cL[i]->type == L"Sld")
				pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[0];
		}
		else
			if (pg->dgrm.cL[i]->type == L"Sld")
				pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[2];
	}
}

void buttonSld_Completion(Sheet* pg, int i, double* doubleValues, bool* boolValues, int radioValue) {

	if (doubleValues[1] > doubleValues[2]) {
		double temp = doubleValues[1];
		doubleValues[1] = doubleValues[2];
		doubleValues[2] = temp;
	}
	if (doubleValues[0] < doubleValues[1])
		doubleValues[0] = doubleValues[1];
	if (doubleValues[0] > doubleValues[2])
		doubleValues[0] = doubleValues[2];

	pg->dgrm.cL[i]->setting[2] = doubleValues[0];
	pg->dgrm.cL[i]->setting[0] = doubleValues[1];
	pg->dgrm.cL[i]->setting[1] = doubleValues[2];

	pg->dgrm.cL[i]->setting[4] = doubleValues[3];

	pg->dgrm.cL[i]->setting[5] = boolValues[0];
}
void buttonSld_SetOptionsAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Value:", "Minimum:", "Maximum:", "Detent:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[1],	pg->dgrm.cL[i]->setting[4] };

	vector<string> checkLabels{ "Continuous Action" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->setting[5] };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		buttonSld_Completion);
}
void buttonSld_SetRangeAction(Sheet* pg, int i, double minimum, double maximum)  {

	pg->dgrm.cL[i]->setSetting(0, minimum);      // Minimum
	pg->dgrm.cL[i]->setSetting(1, maximum);    // Maximum
	if (pg->dgrm.cL[i]->setting[2] < minimum)
		pg->dgrm.cL[i]->setting[2] = minimum;
	if (pg->dgrm.cL[i]->setting[2] > maximum)
		pg->dgrm.cL[i]->setting[2] = maximum;
	pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[2];

	pg->dgrm.cL[i]->method2(pg);
	propagateValues(&pg->dgrm);

	pg->activeDialogComponent = -1;
	drawCanvas();
	drawCanvas2();
}
void buttonSld_SetRange0Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,    0, 255); }
void buttonSld_SetRange1Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i, -100, 100); }
void buttonSld_SetRange2Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,    0, 100); }
void buttonSld_SetRange3Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,  -10,  10); }
void buttonSld_SetRange4Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,    0,  10); }
void buttonSld_SetRange5Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,    0, TWO_PI); }
void buttonSld_SetRange6Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,  -PI,  PI); }
void buttonSld_SetRange7Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,   -1,   1); }
void buttonSld_SetRange8Action(Sheet* pg, int i) { buttonSld_SetRangeAction(pg, i,    0,   1); }


std::vector<Button*> sldButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonSld_SetOptionsAction),
	new Button(-117, 40, 115, 20, L"Range: 0, 255", buttonSld_SetRange0Action),
	new Button(-117, 60, 115, 20, L"Range: -100, 100", buttonSld_SetRange1Action),
	new Button(-117, 80, 115, 20, L"Range: 0, 100", buttonSld_SetRange2Action),
	new Button(-117, 100, 115, 20, L"Range: -10, 10", buttonSld_SetRange3Action),
	new Button(-117, 120, 115, 20, L"Range: 0, 10", buttonSld_SetRange4Action),
	new Button(-117, 140, 115, 20, L"Range: 0, 2*Pi", buttonSld_SetRange5Action),
	new Button(-117, 160, 115, 20, L"Range: -Pi, Pi", buttonSld_SetRange6Action),
	new Button(-117, 180, 115, 20, L"Range: -1, 1", buttonSld_SetRange7Action),
	new Button(-117, 200, 115, 20, L"Range: 0, 1", buttonSld_SetRange8Action) };

bool commonCrankVerticalResize(Sheet* pg, Component *comp, int mouseX, int mouseY, bool check) {
	int left = comp->x + comp->width - RESIZE_HANDLE_SIZE;
	int top = comp->y + comp->height - RESIZE_HANDLE_SIZE;

	if (check) {
		if (mouseX < left)
			return false; // missed
		if (mouseY < top)
			return false; // missed
	}

	//left = x - width;
	top = comp->y - comp->height;

	comp->height = round((mouseY - top + RESIZE_HANDLE_SIZE / 2) / 2);

	if (comp->height < MIN_COMPONENT_HEIGHT)
		comp->height = MIN_COMPONENT_HEIGHT;

	return true;
}

// handle all the sliders, constant, factor, and offset
bool commonCrankSld_(Sheet* pg, Component *comp, int mouseX, int mouseY, bool check) {

	int top = comp->y - comp->height + TERMINAL_SIZE;
	int bot = comp->y + comp->height - TERMINAL_SIZE;
	double yScale = (top - bot) / (comp->setting[1] - comp->setting[0]);

	if (check) {
		bool hitIt = true; // optimism
		comp->memory[0] = 0;

		// must be near center
		if (mouseX > comp->x + TERMINAL_SIZE)
			hitIt = false; // missed dial
		if (mouseX < comp->x - TERMINAL_SIZE)
			hitIt = false; // missed dial

		//  find the current dial position
		int currentY = (comp->detent(4, 2) - comp->setting[0]) * yScale + bot;

		if (mouseY > currentY + TERMINAL_SIZE)
			hitIt = false; // missed dial
		if (mouseY < currentY - TERMINAL_SIZE)
			hitIt = false; // missed dial

		if (!hitIt) {
			// check for hit on resize box
			if (commonCrankVerticalResize(pg, comp, mouseX, mouseY, true)) {
				comp->memory[0] = -9; // rember it was the resize not a geom point

				if (comp->height < MIN_SLIDER_HEIGHT)
					comp->height = MIN_SLIDER_HEIGHT;
				comp->setting[3] = comp->height;

				// move the dial
				comp->dial(pg);
				return true;
			}
			return false;
		}
	}
	if (comp->memory[0] == -9) { // still resizeing
		commonCrankVerticalResize(pg, comp, mouseX, mouseY, false);

		if (comp->height < MIN_SLIDER_HEIGHT)
			comp->height = MIN_SLIDER_HEIGHT;
		comp->setting[3] = comp->height;

		// move the dial
		comp->dial(pg);
		return true;
	}
	if (mouseY > bot)
		mouseY = bot;

	if (mouseY < top)
		mouseY = top;

	// knob position to setting
	comp->setting[2] = reasonableValue(overScale(mouseY - bot, yScale) + comp->setting[0]);

	// move the dial
	comp->dial(pg);

	return true;
}

void commonDialSld(Sheet* pg, Component *comp) {
	int top = comp->y - comp->height + TERMINAL_SIZE;
	int bot = comp->y + comp->height - TERMINAL_SIZE;
	int ay = (comp->detent(4, 2) - comp->setting[0]) /
		(comp->setting[1] - comp->setting[0]) *
		(top - bot) + bot;


	// first the track
	SelectObject(pg->hDC, GetStockObject(BLACK_PEN)); // '#000000';
	BeginPath(pg->hDC);
	MoveToEx(pg->hDC, pg->cXo + comp->x, pg->cYo + top, NULL);
	LineTo(pg->hDC, pg->cXo + comp->x, pg->cYo + bot);
	EndPath(pg->hDC);
	StrokePath(pg->hDC);

	// then the knob
	BeginPath(pg->hDC);
	Ellipse(pg->hDC, pg->cXo + comp->x - SLIDER_HANDLE_SIZE, pg->cYo + ay - SLIDER_HANDLE_SIZE,
		            pg->cXo + comp->x + SLIDER_HANDLE_SIZE, pg->cYo + ay + SLIDER_HANDLE_SIZE);
	EndPath(pg->hDC);
	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH)); // '#FFFFFF';  // white
	StrokeAndFillPath(pg->hDC);

	drawResizeBox(pg, comp);

	std::string text = tomString(comp->detent(4, 2));
	int textWidth = calcStringWidthFont(pg->hDC, text);
	drawTextString(pg->hDC, pg->cXo + comp->x - textWidth / 2, pg->cYo + comp->y - comp->height - 12, 0, text);
}

void commonOnMouseUpSld_(Sheet* pg, Component* comp) {
	if ((int)comp->setting[5]) { // continuous mode
	
		double semiRange = (comp->setting[1] - comp->setting[0]) / 2;
		comp->setting[0] = reasonableValue(comp->setting[2] - semiRange);
		comp->setting[1] = reasonableValue(comp->setting[2] + semiRange);

		drawCanvas();
	}
}

void Sld_Component::dial(Sheet* pg) {
	commonDialSld(pg, this);
}

int Sld_Component::method2(Sheet* pg) {
	output[0].value[0] = reasonableValue(detent(4, 2));
	return true;
}
bool Sld_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankSld_(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
bool Sld_Component::onLoad(Sheet* pg) {
	if (setting.size() < 6)
		setSetting(5, 0);
	height = setting[3];
	return method2(pg);
}
void Sld_Component::onMouseUp(Sheet* pg) {
	commonOnMouseUpSld_(pg, this);
}
Sld_Component::Sld_Component()
	: Component(L"Sld", L"Slider", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = sldButtonList;
	//onMouseUp = onMouseUpSld_;
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 0);      // output
	output[0].value[0] = 0;
	width = 15;
	height = 50;
	setSetting(3, height);
	setSetting(4, 0);  // detent, 0 = disable
	setSetting(5, 0); // bit 1 for continuous - value is 2 - MAYBE NOT
	setMemory(0, 0);
	setOutputDimension(0, 0, 1);
}

// ********* Slider factor *****************************

int SldFac_Component::method2(Sheet* pg) {
	output[0].value.clear();
	copyDimensions(0, 0);
	for (int j = 0; j < input[0].value.size(); j++)
		output[0].value.push_back(reasonableValue(detent(4, 2) * input[0].value[j]));
	return true;
}
void SldFac_Component::dial(Sheet* pg) {
	commonDialSld(pg, this);
}
bool SldFac_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankSld_(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
bool SldFac_Component::onLoad(Sheet* pg) {
	if (setting.size() < 6)
		setSetting(5, 0);
	height = setting[3];
	return method2(pg);
}
void SldFac_Component::onMouseUp(Sheet* pg) {
	commonOnMouseUpSld_(pg, this);
}
SldFac_Component::SldFac_Component()
	: Component(L"SldFac", L"Slide Factor", 1, 1)
{
	dialogButtonList = sldButtonList;
	setSetting(0, 0);    // Minimum
	setSetting(1, 100);  // Maximum
	setSetting(2, 0);    // output
	width = 15;
	height = 50;
	setSetting(3, height);
	setSetting(4, 0);  // detent, 0 = disable
	setSetting(5, 0);  //  bit 1 for continuous - value 2
	setMemory(0, 0);
}

// ************* Slider offset ****************************

int SldOff_Component::method2(Sheet* pg) {
	output[0].value.clear();//= input[0].value.size();
	copyDimensions(0, 0);

	for (int j = 0; j < input[0].value.size(); j++)
		output[0].value.push_back(reasonableValue(detent(4, 2) + input[0].value[j]));
	return true;
}
void SldOff_Component::dial(Sheet* pg) {
	commonDialSld(pg, this);
}
bool SldOff_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankSld_(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
bool SldOff_Component::onLoad(Sheet* pg) {
	if (setting.size() < 6)
		setSetting(5, 0);
	height = setting[3];
	return method2(pg);
}
void SldOff_Component::onMouseUp(Sheet* pg) {
	commonOnMouseUpSld_(pg, this);
}
SldOff_Component::SldOff_Component()
	: Component(L"SldOff", L"Slide Offset", 1, 1)
{
	dialogButtonList = sldButtonList;
	setSetting(0, 0);    // Minimum
	setSetting(1, 100);  // Maximum
	setSetting(2, 0);    // output
	width = 15;
	height = 50;
	setSetting(3, height);
	setSetting(4, 0);  // detent, 0 = disable
	setSetting(5, 0);  //  bit 1 for continuous - value 2
	setMemory(0, 0);
}


// ********************************* X Y Slider *******************************************

void buttonSld2_Completion(Sheet* pg, int i, double *values) {
	for (int j = 0; j < 4; j++)
		pg->dgrm.cL[i]->setting[j] = values[j];

	// check for bad X limits
	if (pg->dgrm.cL[i]->setting[1] < pg->dgrm.cL[i]->setting[0]) {
		pg->dgrm.cL[i]->setting[0] = -100;
		pg->dgrm.cL[i]->setting[1] = 100;
		pg->dgrm.cL[i]->setting[4] = 0;
		if (pg->dgrm.cL[i]->type == L"Sld2")// only output for slider not offset or factor
			pg->dgrm.cL[i]->output[0].value[0] = 0;
	}
	else // be sure X setting is in limits
	{
		if (pg->dgrm.cL[i]->setting[4] < pg->dgrm.cL[i]->setting[0]) {
			pg->dgrm.cL[i]->setting[4] = pg->dgrm.cL[i]->setting[0];
			if (pg->dgrm.cL[i]->type == L"Sld2")
				pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[0];
		}
		else if (pg->dgrm.cL[i]->setting[4] > pg->dgrm.cL[i]->setting[1]) {
			pg->dgrm.cL[i]->setting[4] = pg->dgrm.cL[i]->setting[1];
			if (pg->dgrm.cL[i]->type == L"Sld2")
				pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[0];
		}
		else
			if (pg->dgrm.cL[i]->type == L"Sld2")
				pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[4];
	}

	// check for bad Y limits
	if (pg->dgrm.cL[i]->setting[3] < pg->dgrm.cL[i]->setting[2]) {
		pg->dgrm.cL[i]->setting[2] = -100;
		pg->dgrm.cL[i]->setting[3] = 100;
		pg->dgrm.cL[i]->setting[5] = 0;
		if (pg->dgrm.cL[i]->type == L"Sld2")
			pg->dgrm.cL[i]->output[0].value[0] = 0;
	}
	else // be sure Y output is in limits
	{
		if (pg->dgrm.cL[i]->setting[5] < pg->dgrm.cL[i]->setting[2]) {
			pg->dgrm.cL[i]->setting[5] = pg->dgrm.cL[i]->setting[2];
			if (pg->dgrm.cL[i]->type == L"Sld2")
				pg->dgrm.cL[i]->output[1].value[0] = pg->dgrm.cL[i]->setting[2];
		}
		else if (pg->dgrm.cL[i]->setting[5] > pg->dgrm.cL[i]->setting[3]) {
			pg->dgrm.cL[i]->setting[5] = pg->dgrm.cL[i]->setting[3];
			if (pg->dgrm.cL[i]->type == L"Sld2")
				pg->dgrm.cL[i]->output[1].value[0] = pg->dgrm.cL[i]->setting[3];
		}
		else
			if (pg->dgrm.cL[i]->type == L"Sld2")
				pg->dgrm.cL[i]->output[1].value[0] = pg->dgrm.cL[i]->setting[5];
	}

}

void buttonSld2_Completion(Sheet* pg, int i, double* doubleValues, bool* boolValues, int radioValue) {

	// for x
	if (doubleValues[0] > doubleValues[1]) {
		double temp = doubleValues[0];
		doubleValues[0] = doubleValues[1];
		doubleValues[1] = temp;
	}
	if (pg->dgrm.cL[i]->setting[4] < doubleValues[0])
		pg->dgrm.cL[i]->setting[4] = doubleValues[0];
	if (pg->dgrm.cL[i]->setting[4] > doubleValues[1])
		pg->dgrm.cL[i]->setting[4] = doubleValues[1];

	// for Y
	if (doubleValues[2] > doubleValues[3]) {
		double temp = doubleValues[2];
		doubleValues[2] = doubleValues[3];
		doubleValues[3] = temp;
	}
	if (pg->dgrm.cL[i]->setting[5] < doubleValues[2])
		pg->dgrm.cL[i]->setting[5] = doubleValues[2];
	if (pg->dgrm.cL[i]->setting[5] > doubleValues[3])
		pg->dgrm.cL[i]->setting[5] = doubleValues[3];

	pg->dgrm.cL[i]->setting[0] = doubleValues[0];
	pg->dgrm.cL[i]->setting[1] = doubleValues[1];
	pg->dgrm.cL[i]->setting[2] = doubleValues[2];
	pg->dgrm.cL[i]->setting[3] = doubleValues[3];

	pg->dgrm.cL[i]->setting[8] = boolValues[0];
}
void buttonSld2_Options_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Minimum X:", "Maximum X:", "Minimum Y:", "Maximum Y:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2],	pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels{ "Continuous Action" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->setting[8] };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		buttonSld2_Completion);
}

void buttonSld2_SetRangeAction(Sheet* pg, int i, double minimum, double maximum) {

	pg->dgrm.cL[i]->setSetting(0, minimum);    // min X range
	pg->dgrm.cL[i]->setSetting(1, maximum);    // max X range

	pg->dgrm.cL[i]->setSetting(2, minimum);    // min Y range
	pg->dgrm.cL[i]->setSetting(3, maximum);    // max Y range

	if (pg->dgrm.cL[i]->setting[4] < minimum)
		pg->dgrm.cL[i]->setting[4] = minimum;
	if (pg->dgrm.cL[i]->setting[4] > maximum)
		pg->dgrm.cL[i]->setting[4] = maximum;

	if (pg->dgrm.cL[i]->setting[5] < minimum)
		pg->dgrm.cL[i]->setting[5] = minimum;
	if (pg->dgrm.cL[i]->setting[5] > maximum)
		pg->dgrm.cL[i]->setting[5] = maximum;

	pg->dgrm.cL[i]->output[0].value[0] = pg->dgrm.cL[i]->setting[4];
	pg->dgrm.cL[i]->output[1].value[0] = pg->dgrm.cL[i]->setting[5];

	pg->dgrm.cL[i]->method2(pg);
	propagateValues(&pg->dgrm);

	pg->activeDialogComponent = -1;
	drawCanvas();
	drawCanvas2();
}
void buttonSld2_SetRange0Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, 0, 255); }
void buttonSld2_SetRange1Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, -100, 100); }
void buttonSld2_SetRange2Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, 0, 100); }
void buttonSld2_SetRange3Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, -10, 10); }
void buttonSld2_SetRange4Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, 0, 10); }
void buttonSld2_SetRange5Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, 0, TWO_PI); }
void buttonSld2_SetRange6Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, -PI, PI); }
void buttonSld2_SetRange7Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, -1, 1); }
void buttonSld2_SetRange8Action(Sheet* pg, int i) { buttonSld2_SetRangeAction(pg, i, 0, 1); }


std::vector<Button*> sld2ButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonSld2_Options_Action),
	new Button(-117, 40, 115, 20, L"Range: 0, 255",    buttonSld2_SetRange0Action),
	new Button(-117, 60, 115, 20, L"Range: -100, 100", buttonSld2_SetRange1Action),
	new Button(-117, 80, 115, 20, L"Range: 0, 100",    buttonSld2_SetRange2Action),
	new Button(-117, 100, 115, 20, L"Range: -10, 10",  buttonSld2_SetRange3Action),
	new Button(-117, 120, 115, 20, L"Range: 0, 10",    buttonSld2_SetRange4Action),
	new Button(-117, 140, 115, 20, L"Range: 0, 2*Pi",  buttonSld2_SetRange5Action),
	new Button(-117, 160, 115, 20, L"Range: -Pi, Pi",  buttonSld2_SetRange6Action),
	new Button(-117, 180, 115, 20, L"Range: -1, 1",    buttonSld2_SetRange7Action),
	new Button(-117, 200, 115, 20, L"Range: 0, 1",     buttonSld2_SetRange8Action) };

void commonDialSld2(Sheet* pg, Component *comp) {

	drawResizeBox(pg, comp);

	int lef = comp->x - comp->width + 3 * TERMINAL_SIZE;
	int rig = comp->x + comp->width - 3 * TERMINAL_SIZE;
	double ax = (comp->setting[4] - comp->setting[0]) /
		(comp->setting[1] - comp->setting[0]) *
		(rig - lef) + lef;

	int top = comp->y - comp->height + 2 * TERMINAL_SIZE;
	int bot = comp->y + comp->height - 2 * TERMINAL_SIZE;
	double ay = (comp->setting[5] - comp->setting[2]) /
		(comp->setting[3] - comp->setting[2]) *
		(top - bot) + bot;

	double limitedX = ax;
	double limitedY = ay;
	bool stretchMode = false;
	if (comp->setting[8] != 0) { // bit 1 for continuous
		if (limitedX > rig) {
			limitedX = rig;
			stretchMode = true;
		}
		if (limitedX < lef) {
			limitedX = lef;
			stretchMode = true;
		}
		if (limitedY < top) {
			limitedY = top;
			stretchMode = true;
		}
		if (limitedY > bot) {
			limitedY = bot;
			stretchMode = true;
		}
	}

	// first the vertical track
	SelectObject(pg->hDC, GetStockObject(BLACK_PEN)); // '#000000';
	BeginPath(pg->hDC);
	MoveToEx(pg->hDC, pg->cXo + limitedX, pg->cYo + top, NULL);
	LineTo(pg->hDC, pg->cXo + limitedX, pg->cYo + bot);
	EndPath(pg->hDC);
	StrokePath(pg->hDC);

	// then the horizontal
	BeginPath(pg->hDC);
	MoveToEx(pg->hDC, pg->cXo + lef, pg->cYo + limitedY, NULL);
	LineTo(pg->hDC, pg->cXo + rig, pg->cYo + limitedY);
	EndPath(pg->hDC);
	StrokePath(pg->hDC);

	// then the knob
	BeginPath(pg->hDC);
	Ellipse(pg->hDC, pg->cXo + ax - SLIDER_HANDLE_SIZE, pg->cYo + ay - SLIDER_HANDLE_SIZE,
		pg->cXo + ax + SLIDER_HANDLE_SIZE, pg->cYo + ay + SLIDER_HANDLE_SIZE);
	EndPath(pg->hDC);
	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH)); // '#FFFFFF';  // white
	StrokeAndFillPath(pg->hDC);

	if (stretchMode) { // connect the knob back to the body
		BeginPath(pg->hDC);
		MoveToEx(pg->hDC, pg->cXo + limitedX, pg->cYo + limitedY, NULL);
		LineTo(pg->hDC, pg->cXo + ax, pg->cYo + ay);
		EndPath(pg->hDC);
		StrokePath(pg->hDC);
	}

	// some text over the component
	//ctx.fillStyle = '#000000';
	//var fullText = '(';
	//var text = reasonableValue(comp->setting[4]).tomString();
	//int k = text.size();
	//while (ctx.measureText(text.substr(0, k)).width > comp->width)
	//	k--;
	//fullText += text.substr(0, k) + ', ';
	//
	//text = reasonableValue(comp->setting[5]).tomString();
	//k = text.size();
	//while (ctx.measureText(text.substr(0, k)).width > comp->width)
	//	k--;
	//fullText += text + ')';
	//int width = ctx.measureText(fullText).width;
	//ctx.fillText(fullText, comp->x + pg->cXo - width / 2, comp->y - comp->height + pg->cYo - 4);

	std::string text1 = tomString(reasonableValue(comp->setting[4]));
	std::string text2 = tomString(reasonableValue(comp->setting[5]));
	std::string text = "(" + text1 + ", " + text2 + ")";
	int textWidth = calcStringWidthFont(pg->hDC, text);
	drawTextString(pg->hDC, pg->cXo + comp->x - textWidth / 2, pg->cYo + comp->y - comp->height - 12, 0, text);
}


bool commonCrankSld2_(Sheet* pg, Component *comp, int ax, int ay, bool check) {

	int lef = comp->x - comp->width + 3 * TERMINAL_SIZE;
	int rig = comp->x + comp->width - 3 * TERMINAL_SIZE;
	double xScale = (rig - lef) / (comp->setting[1] - comp->setting[0]);

	int top = comp->y - comp->height + 2 * TERMINAL_SIZE;
	int bot = comp->y + comp->height - 2 * TERMINAL_SIZE;
	double yScale = (top - bot) / (comp->setting[3] - comp->setting[2]);

	if (check) {
		bool foundit = true; // just hoping
		comp->memory[0] = 0;

		//  first find the current dial position
		int currentX = (comp->setting[4] - comp->setting[0]) * xScale + lef;
		int currentY = (comp->setting[5] - comp->setting[2]) * yScale + bot;

		if (ax > currentX + TERMINAL_SIZE)
			foundit = false; // missed dial
		if (ax < currentX - TERMINAL_SIZE)
			foundit = false; // missed dial

		if (ay > currentY + TERMINAL_SIZE)
			foundit = false; // missed dial
		if (ay < currentY - TERMINAL_SIZE)
			foundit = false; // missed dial

		if (!foundit) {
			// check for hit on resize box
			if (comp->crankResize(ax, ay, true)) {
				comp->memory[0] = -9; // rember it was the resize not a geom point

				if (comp->width < 30)
					comp->width = 30;
				comp->setting[6] = comp->width;

				if (comp->height < 30)
					comp->height = 30;
				comp->setting[7] = comp->height;

				// because the output terminal moved
				analyzeConnections(&pg->dgrm);
				propagateValues(&pg->dgrm);
				drawCanvas();
				//drawCanvas2();

				// move the dial
				comp->dial(pg);
				return true;
			}
			comp->memory[0] = 0;
			return false;
		}
	}
	else {
		if (comp->memory[0] == -9) {  // still resizeing
			comp->crankResize(ax, ay, false);

			if (comp->width < 30)
				comp->width = 30;
			comp->setting[6] = comp->width;

			if (comp->height < 30)
				comp->height = 30;
			comp->setting[7] = comp->height;

			// because the input terminal moved
			analyzeConnections(&pg->dgrm);
			propagateValues(&pg->dgrm);
			drawCanvas();
			//drawCanvas2();

			// move the dial
			comp->dial(pg);
			return true;
		}
	}

	if (comp->setting[8] == 0) { // bit 1 for continuous - value 2
		if (ax > rig) ax = rig;
		if (ax < lef) ax = lef;
		if (ay > bot) ay = bot;
		if (ay < top) ay = top;
	}
	comp->setting[4] = reasonableValue(overScale(ax - lef, xScale) + comp->setting[0]);
	comp->setting[5] = reasonableValue(overScale(ay - bot, yScale) + comp->setting[2]);

	// move the dial
	comp->dial(pg);

	return true;
}


void commonOnMouseUpSld2_(Sheet* pg, Component *comp) {
	if ((int)comp->setting[8]) { // continuous mode
		// horizontal
		double semiRange = (comp->setting[1] - comp->setting[0]) / 2;
		comp->setting[0] = reasonableValue(comp->setting[4] - semiRange);
		comp->setting[1] = reasonableValue(comp->setting[4] + semiRange);

		// vertical
		semiRange = (comp->setting[3] - comp->setting[2]) / 2;
		comp->setting[2] = reasonableValue(comp->setting[5] - semiRange);
		comp->setting[3] = reasonableValue(comp->setting[5] + semiRange);

		drawCanvas();
	}
}

int Sld2_Component::method2(Sheet* pg) {
	// this is the slide constant - send setting out
	output[0].value[0] = setting[4];  // X output
	output[1].value[0] = setting[5];  // Y output

	//if (setting[8] == undefined)
	//	setting[8] = 0;    // continuous bit - off - 2 on

	width = setting[6];
	height = setting[7];
	return true;
}

void Sld2_Component::dial(Sheet* pg) {
	commonDialSld2(pg, this);
}
bool Sld2_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankSld2_(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
bool Sld2_Component::onLoad(Sheet* pg) {
	if( setting.size() < 9 )
		setSetting(8, 0); // bit 1 for continuous - value 2
	return method2(pg);
}
void Sld2_Component::onMouseUp(Sheet* pg) {
	commonOnMouseUpSld2_(pg, this);
}
Sld2_Component::Sld2_Component()
	: Component(L"Sld2", L"XY Slider", squareShape, 0, 0, 2, 2)
{
	dialogButtonList = sld2ButtonList;
	setSetting(0, -100);   // min X range
	setSetting(1, 100);   // max X range
	setSetting(2, -100);   // min Y range
	setSetting(3, 100);   // max Y range
	output[0].value[0] = -65;
	output[1].value[0] = 65;
	setSetting(4, -65);   // X output
	setSetting(5, 65);  //  Y output
	height = width;
	setSetting(6, width);
	setSetting(7, height);
	setSetting(8, 0); // bit 1 for continuous - value 2
	setMemory(0, 0);
	setRightLabels(3, "X", "Y");
	setOutputDimension(0, 0, 1);
	setOutputDimension(1, 0, 1);
}

// ************* factor


int Sld2Fac_Component::method2(Sheet* pg) {
	output[0].value.resize(input[0].value.size());
	output[1].value.resize(input[1].value.size());

	if (setting[8] == NULL)
		setting[8] = 0;    // continuous bit - off - 2 on

	copyDimensions(0, 0);
	copyDimensions(1, 1);

	// this is the slide factor - send setting out
	for (int j = 0; j < input[0].value.size(); j++)
		output[0].value[j] = reasonableValue(setting[4] * input[0].value[j]);
	for (int j = 0; j < input[1].value.size(); j++)
		output[1].value[j] = reasonableValue(setting[5] * input[1].value[j]);

	width = setting[6];
	height = setting[7];
	return true;
}


void Sld2Fac_Component::dial(Sheet* pg) {
	commonDialSld2(pg, this);
}
bool Sld2Fac_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankSld2_(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
bool Sld2Fac_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
void Sld2Fac_Component::onMouseUp(Sheet* pg) {
	commonOnMouseUpSld2_(pg, this);
}
Sld2Fac_Component::Sld2Fac_Component()
	: Component(L"Sld2Fac", L"XY Slide Factor", squareShape, 2, 2, 2, 2)
{
	dialogButtonList = sld2ButtonList;
	setSetting(0, -100);   // min X range
	setSetting(1, 100);   // max X range
	setSetting(2, -100);   // min Y range
	setSetting(3, 100);   // max Y range
	output[0].value[0] = -65;
	output[1].value[0] = 65;
	setSetting(4, -65);   // X output
	setSetting(5, 65);  //  Y output
	height = width;
	setSetting(6, width);
	setSetting(7, height);
	setSetting(8, 0); // bit 1 for continuous - value 2
	setMemory(0, 0);
	setRightLabels(3, "X", "Y");
	setLeftLabels(3, "X", "Y");
}

// **************** offset


int Sld2Off_Component::method2(Sheet* pg) {
	output[0].value.resize(input[0].value.size());
	output[1].value.resize(input[1].value.size());

	//if (setting[8] == NULL)
	//	setting[8] = 0;    // continuous bit - off - 2 on

	copyDimensions(0, 0);
	copyDimensions(1, 1);

	// this is the slide offset - send setting out
	for (int j = 0; j < input[0].value.size(); j++)
		output[0].value[j] = reasonableValue(setting[4] + input[0].value[j]);
	for (int j = 0; j < input[1].value.size(); j++)
		output[1].value[j] = reasonableValue(setting[5] + input[1].value[j]);

	width = setting[6];
	height = setting[7];
	return true;
}

bool Sld2Off_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankSld2_(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
void Sld2Off_Component::dial(Sheet* pg) {
	commonDialSld2(pg, this);
}
bool Sld2Off_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
void Sld2Off_Component::onMouseUp(Sheet* pg) {
	commonOnMouseUpSld2_(pg, this);
}
Sld2Off_Component::Sld2Off_Component()
	: Component(L"Sld2Off", L"XY Slide Offset", squareShape, 2, 2, 2, 2)
{
	dialogButtonList = sld2ButtonList;
	setSetting(0, -100);   // min X range
	setSetting(1, 100);   // max X range
	setSetting(2, -100);   // min Y range
	setSetting(3, 100);   // max Y range
	output[0].value[0] = -65;
	output[1].value[0] = 65;
	setSetting(4, -65);   // X output
	setSetting(5, 65);  //  Y output
	height = width;
	setSetting(6, width);
	setSetting(7, height);
	setSetting(8, 0); // bit 1 for continuous - value 2
	setMemory(0, 0);
	setRightLabels(3, "X", "Y");
	setLeftLabels(3, "X", "Y");
}


// *********************************** Display **********************************


void DisplayList_Component::dial(Sheet* pg) {

	// check for debugging a display
	// pg = getDebugInstancepg(pg, i); allow this later *********************************

	int totalLineHeight = 10;
	bool showHeightEllipsis = false;
	int count = 0;
	if (input[0].drivenByComponent != -1) {
		if (!input[0].stringType)
			count = input[0].value.size();
		else
			count = input[0].stringValue.size();
		if (count * totalLineHeight > 2 * height - 4) {
			showHeightEllipsis = true;
			count = floor((2 * height - 4 - 3) / totalLineHeight);
		}
	}

	int ax = x - width + 5;
	int ay = y - height + 5;
	int aw = 2 * width - 10;
	int ah = 2 * height - 10;

	drawResizeBox(pg, this);

	HBRUSH fillStyle = GetColorBrush(0x000000); // CreateSolidBrush
	std::string text;

	for (int j = 0; j < count; j++) {

		if (!input[0].stringType)
			text = tomString(input[0].value[j]);
		else
			text = input[0].stringValue[j];

		//if (setting[2]) { // 5.7.2016 was pg
		//	// add commas
		//	var parts = text.split(".");
		//	parts[0] = parts[0].replace(/ \B(? = (\d{ 3 }) + (? !\d)) / g, ",");
		//	text = parts.join(".");
		//}

		if (count > 4)
			text.insert(0, tomString(j) + ") ");

		drawTextString(pg->hDC, pg->cXo + ax + 2, pg->cYo + ay - 3, 0, trimTextToFit(pg->hDC, text, aw));
		ay += totalLineHeight;
	}

	if (showHeightEllipsis) {
		drawTextString(pg->hDC, pg->cXo + ax + 5, pg->cYo + ay - 7, 0, "...");
	}
	//DeleteObject(fillStyle);
}

bool DisplayList_Component::crank(Sheet* pg, int mouseX, int mouseY, bool check) {

	if (!crankResizeHoldLeft(mouseX, mouseY, check))
		return false;

	setting[0] = width;
	setting[1] = height;

	// because the input terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//  drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}

bool DisplayList_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	return true;
}

void doubleCopyDisplay(Sheet* pg, int i) {
	string aString;

	if (pg->dgrm.cL[i]->input[0].stringType) {
		int len = pg->dgrm.cL[i]->input[0].stringValue.size();
		for (int j = 0; j < len; j++) {
			if( j )
				aString += ", ";
			aString += pg->dgrm.cL[i]->input[0].stringValue[j];
		}
	}
	else {
		int len = pg->dgrm.cL[i]->input[0].value.size();
		for (int j = 0; j < len; j++) {
			if (j)
				aString += ", ";
			aString += tomString(pg->dgrm.cL[i]->input[0].value[j]);
		}
	}
	copyStringToClipboard(aString);
	pg->activeDialogComponent = -1;
}
void doubleCopyDisplayLines(Sheet* pg, int i) {
	string aString;

	if (pg->dgrm.cL[i]->input[0].stringType) {
		int len = pg->dgrm.cL[i]->input[0].stringValue.size();
		for (int j = 0; j < len; j++) {
			if (j)
				aString += "\r\n";
			aString += pg->dgrm.cL[i]->input[0].stringValue[j];
		}
	}
	else {
		int len = pg->dgrm.cL[i]->input[0].value.size();
		for (int j = 0; j < len; j++) {
			if (j)
				aString += "\r\n";
			aString += tomString(pg->dgrm.cL[i]->input[0].value[j]);
		}
	}
	copyStringToClipboard(aString);
	pg->activeDialogComponent = -1;
}

std::vector<Button*>displayButtonList = {
	new Button(3, 40, 85, 20, L"Copy", doubleCopyDisplay) ,
	new Button(3, 60, 85, 20, L"Copy Lines", doubleCopyDisplayLines) };

DisplayList_Component::DisplayList_Component()
	: Component(L"Dis", L"Display List", squareShape, 1, 1, 0, 0)
{
	dialogButtonList = displayButtonList;
	width = 30;
	height = MIN_COMPONENT_HEIGHT;
	setSetting(0, width);
	setSetting(1, height);
	setSetting(2, 0); // commas
	setSetting(3, 10); // base
}

// ****************************** Grid ***************************

bool Grid_Component::crank(Sheet* pg, int mouseX, int mouseY, bool check) {

	if (!crankResizeHoldLeft(mouseX, mouseY, check))
		return false;

	setting[0] = width;
	setting[1] = height;

	// because the input terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//  drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}

void Grid_Component::dial(Sheet* pg) {
	commonDialGrid(pg, this);
}
bool Grid_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	return true;
}
void doubleClickGrid(Sheet* pg, int i) {
	string aString;
	if (pg->dgrm.cL[i]->input[0].dimensions.size() != 2)
		return;

	int columns = pg->dgrm.cL[i]->input[0].dimensions[0];
	int rows = pg->dgrm.cL[i]->input[0].dimensions[1];

	bool isString = pg->dgrm.cL[i]->input[0].stringType;
	int j = 0;
	for (int r = 0; r < rows; r++) {
		if (r)
			aString += "\r\n";
		for (int c = 0; c < columns; c++) {
			if (c)
				aString += ", ";
			if( isString )
				aString += pg->dgrm.cL[i]->input[0].stringValue[j++];
			else
				aString += tomString(pg->dgrm.cL[i]->input[0].value[j++]);
		}
	}
	copyStringToClipboard(aString);
	pg->activeDialogComponent = -1;
}

std::vector<Button*>gridButtonList = { new Button(-80, 40, 125, 20, L"Copy", doubleClickGrid) };

Grid_Component::Grid_Component()
	: Component(L"Grid", L"Display Grid", squareShape, 1, 1, 0, 0)
{
	dialogButtonList = gridButtonList;
	setSize(60, 60);
	setSetting(0, 60);
	setSetting(1, 60);
}



// ************** Select ********************

bool Select_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n); //((n - 1) * TERMINAL_STEP + 2 * TERMINAL_SIZE) / 2;

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n-1; j++)
		input[j].label = tomString(j);
	input[n-1].label = "Control";
	return true;
}

int Select_Component::method2(Sheet* pg) {

	int n = setting[0];
	int controlInput = n - 1;
	if (input[controlInput].stringType || (input[controlInput].value.size() == 0))
		return clearOutputs();

	if (input[controlInput].value.size() == 1) { // single control input

		int control = input[controlInput].value[0];// simple selection
		if (control < 0)
			control = 0;
		else if (control >= controlInput)
			control = controlInput-1;

		if (input[control].stringType) {
			int size = input[control].stringValue.size();
			output[0].stringValue.resize(size);
			output[0].value.clear();
			output[0].stringType = true;
			for (int j = 0; j < size; j++)
				output[0].stringValue[j] = input[control].stringValue[j];
		}
		else {
			int size = input[control].value.size();
			output[0].value.resize(size);
			output[0].stringValue.clear();
			output[0].stringType = false;
			for (int j = 0; j < size; j++)
				output[0].value[j] = input[control].value[j];
		}
		copyDimensions(control, 0);
		return 1;
	}

	// more complex if the control is also an array
	// need to choose an output type
	// for now lets use the more common type
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	bool outputIsString = input[0].stringType;
	for( int j = 1; j < controlInput; j++ )
		if(outputIsString != input[j].stringType)
			return clearOutputs();

	if (outputIsString) {
		output[0].stringValue.resize(loops);
		output[0].value.clear();
		output[0].stringType = true;

		for (int j = 0; j < loops; j++) {
			int control = getLoopingValueD(controlInput, j, 0);
			if (control < 0)
				control = 0;
			else if (control >= controlInput)
				control = controlInput - 1;

			output[0].stringValue[j] = getLoopingStringValueD(control, j, "");
		}
	}
	else // output is numbers
	{
		output[0].value.resize(loops);
		output[0].stringValue.clear();
		output[0].stringType = false;

		for (int j = 0; j < loops; j++) {
			int control = getLoopingValueD(controlInput, j, 0);
			if (control < 0)
				control = 0;
			else if (control >= controlInput)
				control = controlInput - 1;

			output[0].value[j] = getLoopingValueD(control, j, 0);
		}
	}
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = loops;
	return 1;
}
bool Select_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	
		// check for hit on resize box
	if( ! commonCrankVerticalResize(pg, this, ax, ay, check))
		return false;

	int pins = round((2 * (height - 2) - 2 * TERMINAL_SIZE) / TERMINAL_STEP + 1);
	if (pins < 3)
		pins = 3;
	if (pins > 12)
		pins = 12;

	height = pinToHeight(pins);
	setting[0] = pins;

	onLoad(pg);
	method2(pg);
	propagateValues(&pg->dgrm);
	drawCanvas();

	drawResizeBox(pg, this);

	return true;
}

void Select_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}



Select_Component::Select_Component()
	: Component(L"Select", L"Selector", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialogSelectButtonList;
	setSetting(0, 4);
	setLeftLabels(3, "0", "1", "2", "Control");
}




// ************** Deselect************************

bool Deselect_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);

	return true;
}
int Deselect_Component::method2(Sheet* pg) {
	// scan the n left inputs for the first true

	if (!checkValueInputs())
		return clearOutputs();

	int inputs = setting[0];
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);

	int mode = setting[1]; // mode: 0 first true, 1 last true, 2 first false, 3 last false
	for (int j = 0; j < loops; j++) {
		output[0].value[j] = -1; // no true inputs yet

		switch (mode) {
		case 0: // first true
			for (int k = 0; k < inputs; k++) {
				if (input[k].drivenByComponent == -1)
					continue;
				if (getLoopingValueD(k, j, 0) != 0) {
					output[0].value[j] = k;
					break;
				}
			}
			break;
		case 1: // last true
			for (int k = 0; k < inputs; k++) {
				if (input[k].drivenByComponent == -1)
					continue;
				if (getLoopingValueD(k, j, 0) != 0)
					output[0].value[j] = k;
				else
					break;
			}
			break;
		case 2: // first false
			for (int k = 0; k < inputs; k++) {
				if (input[k].drivenByComponent == -1)
					continue;
				if (getLoopingValueD(k, j, 0) == 0) {
					output[0].value[j] = k;
					break;
				}
			}
			break;
		case 3: // last false
			for (int k = 0; k < inputs; k++) {
				if (input[k].drivenByComponent == -1)
					continue;
				if (getLoopingValueD(k, j, 0) != 0)
					output[0].value[j] = k;
				else
					break;
			}
			break;
		}
	}
	setSimpleOutputDimensions();
	return true;
}
void Deselect_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
	//ctx.fillStyle = '#000000';
	int mode = setting[1]; // mode: 0 first true, 1 last true, 2 first false, 3 last false
	int ax = pg->cXo + x;
	int ay = pg->cYo + y;
	switch (mode) {
	case 0: // first true
		drawTextLptStr(pg->hDC, ax, ay - 2, 0, L"First");
		drawTextLptStr(pg->hDC, ax, ay + 10, 0, L"True");
		//ctx.fillText("First", x, y - 2);
		//ctx.fillText("True", x, y + 10);
		break;
	case 1: // last true
		drawTextLptStr(pg->hDC, ax, ay - 2, 0, L"Last");
		drawTextLptStr(pg->hDC, ax, ay + 10, 0, L"True");
		//ctx.fillText("Last", x, y - 2);
		//ctx.fillText("True", x, y + 10);
		break;
	case 2: // first false
		drawTextLptStr(pg->hDC, ax, ay - 2, 0, L"First");
		drawTextLptStr(pg->hDC, ax, ay + 10, 0, L"False");
		//ctx.fillText("First", x, y - 2);
		//ctx.fillText("False", x, y + 10);
		break;
	case 3: // last false
		drawTextLptStr(pg->hDC, ax, ay - 2, 0, L"Last");
		drawTextLptStr(pg->hDC, ax, ay + 10, 0, L"False");
		//ctx.fillText("Last", x, y - 2);
		//ctx.fillText("False", x, y + 10);
		break;
	}
}


bool Deselect_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	
	// check for hit on resize box
	if( ! commonCrankVerticalResize(pg, this, ax, ay, check))
		return false;

	int pins = round((2 * (height - 2) - 2 * TERMINAL_SIZE) / TERMINAL_STEP + 1);
	if (pins < 2)
		pins = 2;
	if (pins > 10)
		pins = 10;

	height = pinToHeight(pins);
	setting[0] = pins;

	onLoad(pg);
	method2(pg);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}

Deselect_Component::Deselect_Component()
	: Component(L"Deselect", L"Deselector", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialogDeselectButtonList;
	setSetting(0, 4); // inputs
	setSetting(1, 0); // mode: 0 first true, 1 last true, 2 first false, 3 last false
	
} 

// ************************* Sample ******************************

int Sample_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	bool isString = input[0].stringType;

	output[0].stringType = isString;
	if (isString) {
		output[0].stringValue.resize(loops);
		output[0].value.clear();
	}
	else {
		output[0].value.resize(loops);
		output[0].stringValue.clear();
	}

	for (int j = 0; j < loops; j++) {
		double con = getLoopingValueD(1, j, 0);

		if (con > 0) {
			if( isString )
				output[0].stringValue[j] = getLoopingStringValueD(0, j, "");
			else
				output[0].value[j] = getLoopingValueD(0, j, 0);
		}
	}	
	output[0].dimensions[0] = loops;
	return 1;
}

Sample_Component::Sample_Component()
	: Component(L"Sample", L"Sample", triangleShape, 2, 2, 1, 1)
{
	setLeftLabels(3, "In", "Con");
}

// *********************** Hold *********************************

int Hold_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	bool isString = input[0].stringType;

	output[0].stringType = isString;
	if (isString) {
		output[0].stringValue.resize(loops);
		output[0].value.clear();
	}
	else {
		output[0].value.resize(loops);
		output[0].stringValue.clear();
	}

	for (int j = 0; j < loops; j++) {
		double con = getLoopingValueD(1, j, 0);

		if (con <= 0) {
			if (isString)
				output[0].stringValue[j] = getLoopingStringValueD(0, j, "");
			else
				output[0].value[j] = getLoopingValueD(0, j, 0);
		}
	}
	output[0].dimensions[0] = loops;
	return 1;
}


Hold_Component::Hold_Component()
	: Component(L"Hold", L"Hold", triangleShape, 2, 2, 1, 1)
{
	setLeftLabels(3, "In", "Con");
}




int Quad_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	output[1].value.resize(loops);
	output[2].value.resize(loops);
	output[3].value.resize(loops);
	copyDimensions(0, 0);
	copyDimensions(0, 1);
	copyDimensions(0, 2);
	copyDimensions(0, 3);

	for (int j = 0; j < loops; j++) {
		double a = getLoopingValueD(0, j, 0);
		double b = getLoopingValueD(1, j, 0);
		double c = getLoopingValueD(2, j, 0);

		double numerator = b * b - 4 * a * c;
		if (numerator == 0) {
			output[0].value[j] = -b / (2 * a);
			output[1].value[j] = 1;
			output[2].value[j] = 0;
			output[3].value[j] = 0;
		}
		else
			if (numerator < 0) {
				output[0].value[j] = 0;
				output[1].value[j] = 0;
				output[2].value[j] = 0;
				output[3].value[j] = 0;
			}
			else {
				double root = sqrt(numerator);
				output[0].value[j] = (-b + root) / (2 * a);
				output[1].value[j] = 1;
				output[2].value[j] = (-b - root) / (2 * a);
				output[3].value[j] = 1;
			}
	}
	return true;
}


Quad_Component::Quad_Component()
	: Component(L"Quad", L"Quadratic", squareShape, 3, 1, 4, 1)
{
	setLeftLabels(3, "a", "b", "c");
	setRightLabels(3, "X+", "+IsReal", "X-", "-IsReal");
}

int Alternate_Component::method2(Sheet* pg) {
	if( ! checkValueInputs())
		return clearOutputs();
	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();

	output[0].value.clear();

	int sign = 1;
	int dataSize = input[0].value.size();
	int reps;
	if (input[1].drivenByComponent >= 0) {   // reps
		reps = input[1].value[0];
		if (reps <= 0)
			reps = 1;
	}
	else
		reps = 1;
	int k = 0;
	for (int m = 0; m < reps; m++) {
		for (int n = 0; n < dataSize; n++) {
			output[0].value.push_back(sign * input[0].value[n]);
			sign *= -1;
		}
	}
	output[0].dimensions[0] = output[0].value.size();
	return true;
}


Alternate_Component::Alternate_Component()
	: Component(L"Alt", L"Alternate", 2, 1)
{
	setLeftLabels(3, "In", "Reps.");
}


// **************************************** Light LED **********************************

int LED_Component::method2(Sheet* pg) {
	// draw in the dial method
	return false;
}

void LED_Component::dial(Sheet* pg) {

	// check for debugging a display
	//pg = getDebugInstancepg(pg, i);

	// no output, just display it/them
	COLORREF aColorRef;
	switch ((int)setting[2]) {
	case 1: aColorRef = RGB(255, 0, 0); break;		// Red
	case 2: aColorRef = RGB(255, 136, 0); 	break;  // Orange
	case 3: aColorRef = RGB(255, 255, 0); break;	// Yellow
	case 4: aColorRef = RGB(0, 255, 0); break;		// Green
	case 5: aColorRef = RGB(100, 100, 255); break;	// Blue
	case 6: aColorRef = RGB(255, 0, 255); break;	// Violet
	case 7: aColorRef = RGB(190, 190, 190); break;  // Gray
	}
	int n = setting[0];

	// set it up
	int ax;
	int ay;
	int size;
	switch ((int)setting[1]) {
	case 0: // spread them horizontally Left to right
		ax = pg->cXo + x - (n / 2.0) * 30 + 15;
		ay = pg->cYo + y;
		size = height;
		break;
	case 1: // stack them vertically
		ax = pg->cXo + x;
		ay = pg->cYo + y - (n / 2.0) * 30 + 15;
		size = width;
		break;
	case 2: // spread them horizontally
		ax = pg->cXo + x + (n / 2.0) * 30 - 15;
		ay = pg->cYo + y;
		size = height;
		break;
	case 3: // stack them vertically
		ax = pg->cXo + x;
		ay = pg->cYo + y + (n / 2.0) * 30 - 15;
		size = width;
		break;
	}

	for (int j = 0; j < n; j++) {

		// draw the LED and number it
		HBRUSH	fillColor = NULL;

		BeginPath(pg->hDC);
		Ellipse(pg->hDC, ax - size, ay - size, ax + size, ay + size);
		EndPath(pg->hDC);
		if( j >= input[0].value.size())
			SelectObject(pg->hDC, GetStockObject(GRAY_BRUSH));
		else if (input[0].value[j]) {
			if (setting[2] == 0) {
				SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
			}
			else {
				fillColor = GetColorBrush(aColorRef); // CreateSolidBrush
				SelectObject(pg->hDC, fillColor);
			}
		}
		else
			SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
		StrokeAndFillPath(pg->hDC);
		//if (fillColor != NULL) {
		//	DeleteObject(fillColor);
		//	fillColor = NULL;
		//}
		if (n > 1) {
			// number it
			if (j < input[0].value.size()) {
				if (input[0].value[j])
					SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
				else
					SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));

				std::string text = tomString(j);
				int wid = calcStringWidthFont(pg->hDC, text);
				drawTextString(pg->hDC, ax - wid/2, ay - 7, 0, text);
			}
		}

		// index to next LED
		switch ((int)setting[1]) {
		case 0: // spread them horizontally Left to right
			ax += 30;
			break;
		case 1: // stack them vertically
			ay += 30;
			break;
		case 2: // spread them horizontally
			ax -= 30;
			break;
		case 3: // stack them vertically
			ay -= 30;
			break;
		}
	}
	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
}



bool LED_Component::onLoad( Sheet* pg) {
	// number of lights
	while (setting.size() < 3)
		setting.push_back(0.0);

	switch ((int)setting[1]) {
	case 0:
	case 2:
		width = setting[0] * 15;
		height = 15;
		break;
	case 1:
	case 3:
		height = setting[0] * 15;
		width = 15;
		break;
	}
	return true;
}

void doubleClickLed(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildLedDialogButtons(pg, i);
}

std::vector<Button*>ledButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickLed) };

LED_Component::LED_Component()
	: Component(L"LED", L"Light", squareShape, 1, 1, 0, 0)
{
	dialogButtonList = ledButtonList;
	setSetting(0, 1); // number of lights - default to 1 if missing
	setSetting(1, 0); // 0 = horizontal ; 1 = vertical
	setSetting(2, 0); // color
	setSize(15, 15);
}

// ********************************** Switch *****************************************

// handle the switch
void Switch_Component::dial(Sheet* pg) {
	int knobWidth = 7;

	int n = setting.size();
	int ax = pg->cXo + x - (n / 2.0) * 20 + 10;
	int ay = pg->cYo + y;
	
	for (int j = 0; j < n; j++) {

		// first the track
		BeginPath(pg->hDC);
		MoveToEx(pg->hDC, ax, ay - knobWidth, NULL);
		LineTo(pg->hDC, ax, ay + knobWidth);
		EndPath(pg->hDC);
		StrokePath(pg->hDC);

		BeginPath(pg->hDC);
		int yy = ay;
		if (setting[j])
			yy -= knobWidth;
		else
			yy += knobWidth;
		Ellipse(pg->hDC, ax - knobWidth, yy - knobWidth, ax + knobWidth, yy + knobWidth);
		EndPath(pg->hDC);
		SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
		StrokeAndFillPath(pg->hDC);

		if (n > 1) {
			// number it
			SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
			std::string text = tomString(j);
			int wid = calcStringWidthFont(pg->hDC, text);
			drawTextString(pg->hDC, ax - wid/2, yy - 6, 0, text);
		}

		ax += 20;
	}
	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
}


bool Switch_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	int radius = width / 2.0;
	int sliders = setting.size();

	if (check) {

		if (ax < x - width)
			return false;
		if (ax > x + width)
			return false;
	}

	// figure out which switch is changing
	int slider = sliders - 1 - floor((x + sliders * 10 - ax) / (2 * 10));
	if (slider < 0)
		slider = 0;
	if (slider >= sliders)
		slider = sliders - 1;

	if (check) {
		// slider = floor((x + sliders * TERMINAL_SIZE - x) / (2 * TERMINAL_SIZE));
		// use memory to remember which one is moving
		memory[0] = slider;

		//  first find the current dial position
		int currentY = y;
		if (setting[slider])
			currentY -= radius;
		else
			currentY += radius;

		if (ay > currentY + radius)
			return false; // missed dial
		if (ay < currentY - radius)
			return false; // missed dial
	}
	else
		slider = memory[0];

	// knob position to setting
	if (ay > y) {
		setting[slider] = 0;
		output[0].value[slider] = 0;
	}
	else {
		setting[slider] = 1;
		output[0].value[slider] = 1;
	}

	// move the dial
	dial(pg);

	return true;
}



bool Switch_Component::onLoad(Sheet* pg) {
	int n = setting.size();
	output[0].value.resize(n);

	if (n == 1)
		width = 15;
	else
		width = n * 10;

	for (int j = 0; j < n; j++)
		output[0].value[j] = setting[j];
	output[0].dimensions[0] = n;
	return true;
}


void Switch_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	int oldCount = setting.size();
	int newCount = doubleValues[0];

	if ((newCount >= 1) && (newCount <= 32)) {
		if (newCount != oldCount) {
			setting.resize(newCount);
			for (int j = oldCount; j < newCount; j++)
				setting[j] = 0;
			onLoad(pg);
		}
	}
}
void doubleClickSwitch(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Switches:" };
	vector<double> doubleValues = { (double)pg->dgrm.cL[i]->setting.size() };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}


std::vector<Button*>switchButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickSwitch) };

Switch_Component::Switch_Component()
	: Component(L"Switch", L"Switch", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = switchButtonList;
	setSize(15, 20);
	setOutput(0, 0, 0);
	setMemory(0, 0);
	output[0].dimensions[0] = 1;
	setSetting(0, 0); // this is the memory for the first switch; ALSO the size of settings indicates the number of switches
	completionTestDoubles = 1;
}



// handle the button - momentary
void Button_Component::dial(Sheet* pg) {
	int n = setting[0];
	if (setting[1]) { // stack them vertically
		int ax = pg->cXo + x;
		int ay = pg->cYo + y - (n / 2.0) * 30 + 15;
		for (int j = 0; j < n; j++) {
			int radius =  width;
			BeginPath(pg->hDC);
			Ellipse(pg->hDC, ax - radius, ay - radius, ax + radius, ay + radius);
			EndPath(pg->hDC);
			if (output[0].value[j])
				SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
			else
				SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
			StrokeAndFillPath(pg->hDC);

			if (n > 1) {
				COLORREF textColor;
				if (output[0].value[j])
					textColor = 0x000000;
				else
					textColor = 0xFFFFFF;
				std::string text = tomString(j);
				int wid = calcStringWidthFont(pg->hDC, text);
				drawTextString(pg->hDC, ax - wid/2, ay - 6, textColor, text);
			}
			ay += 30;
		}
	}
	else { // spread them horizontally
		int ax = pg->cXo + x - (n / 2.0) * 30 + 15;
		int ay = pg->cYo + y;
		for (int j = 0; j < n; j++) {
			int radius = height;
			BeginPath(pg->hDC);
			Ellipse(pg->hDC, ax - radius, ay - radius, ax + radius, ay + radius);
			EndPath(pg->hDC);
			if (output[0].value[j])
				SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
			else
				SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
			StrokeAndFillPath(pg->hDC);

			if (n > 1) {
				COLORREF textColor;
				if (output[0].value[j])
					textColor = 0x000000;
				else
					textColor = 0xFFFFFF;
				std::string text = tomString(j);
				int wid = calcStringWidthFont(pg->hDC, text);
				drawTextString(pg->hDC, ax - wid / 2, ay - 6, textColor, text);
			}

			ax += 30;
		}
		SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
	}
}
bool Button_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	int n = setting[0];
	int knobWidth;
	int start;
	double j;
	if (setting[1]) { // stack them vertically
		knobWidth = width * 0.7;
		if (ax < x - knobWidth)
			return false;
		if (ax > x + knobWidth)
			return false;

		start = y - (n / 2.0) * 30.0;
		j = (ay - start) / 30.0;
	}
	else { // spread them horizontally
		knobWidth = height * 0.7;
		if (ay < y - knobWidth)
			return false;
		if (ay > y + knobWidth)
			return false;

		start = x - (n / 2.0) * 30.0;
		j = (ax - start) / 30.0;
	}

	int jj = floor(j);
	if (jj < 0)
		return false;
	if (jj >= n)
		return false;
	switch ((int)setting[2]) {
	case 0: // momentary
		output[0].value[jj] = 1;
		break;
	case 1: // toggle
		if (output[0].value[jj])
			output[0].value[jj] = 0;
		else
			output[0].value[jj] = 1;
		break;
	case 2: // radio group
		for (int k = 0; k < output[0].value.size(); k++)
			output[0].value[k] = 0;
		output[0].value[jj] = 1;
		break;
	}

	// move the dial
	dial(pg);

	return true;
}



bool Button_Component::onLoad(Sheet* pg) {
	// [0] its the number of buttons

	if (setting[1]) {
		height = setting[0] * 15;
		width = 15;
	}
	else {
		width = setting[0] * 15;
		height = 15;
	}

	// clear the outputs
	output[0].value.resize(setting[0]);
	for (int j = 0; j < setting[0]; j++)
		output[0].value[j] = 0;
	output[0].dimensions[0] = setting[0];

	return true;
}


void Button_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if ((doubleValues[0] >= 1) && (doubleValues[0] <= 32)) 
		setting[0] = (int)doubleValues[0]; // buttons

	setting[1] = (boolValues[0] ? 1 : 0);

	if ((radioValue >= 0) && (radioValue <= 2))
		setting[2] = radioValue; // mode

	onLoad(pg);
}
void doubleClickButton(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Buttons:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	vector<string> checkLabels = { "Allign Vertically" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[1] & 1) };

	vector<string> radioLabels = { "Momentary", "Toggle", "Radio group" };
	int radioIndex = pg->dgrm.cL[i]->setting[2];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}


std::vector<Button*>buttonButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickButton) };

Button_Component::Button_Component()
	: Component(L"Button", L"Button", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = buttonButtonList;
	setSize(15, 15);
	setOutput(0, 0, 0);
	output[0].dimensions[0] = 1;
	setSetting(0, 1); // was the output, now its the number of buttons
	setSetting(1, 0); // 0 = horizongal; 1 = vertical
	setSetting(2, 0); // 0 = momentary; 1 = toggle; 2 = radio group
	completionTestDoubles = 1;
	completionTestChecks = 1;
	completionTestRadios = 3;
}


// ************************ Geometry *************************

	// setting[0] = x minimum
	// setting[1] = x Maximum
	// setting[2] = y minimum
	// setting[3] = y Maximum

	// setting[4] = width of component
	// setting[5] = height of component
	// setting[6] = number of points

	// setting[7] = value of x[0]
	// setting[8] = value of y[0]

void Geom_Component::dial(Sheet* pg) {
	int lef = x - width + TERMINAL_SIZE;
	int rig = x + width - 2 * TERMINAL_SIZE;
	double xScale = (setting[1] - setting[0]) / (rig - lef);

	int top = y - height + TERMINAL_SIZE;
	int bot = y + height - TERMINAL_SIZE;
	double yScale = (setting[3] - setting[2]) / (top - bot);

	// show a gray pad
	RECT aRect;
	aRect.left = pg->cXo + lef;
	aRect.top = pg->cYo + top;
	aRect.right = pg->cXo + rig;
	aRect.bottom = pg->cYo + bot;

	HBRUSH fillStyle = GetColorBrush(0xE0E0E0); // CreateSolidBrush light gray
	FillRect(pg->hDC, &aRect, fillStyle);
	//DeleteObject(fillStyle);

	drawResizeBox(pg, this);

	// then the connecting lines
	int j, ax, ay;
	double detent = element[0];
	//SelectObject(pg->hDC, GetStockObject(BLACK_PEN));
	BeginPath(pg->hDC);
	for (j = 0; j < setting[6]; j++) {
		ax = (funcDetent(setting[2 * j + 7], detent) - setting[0]) / xScale + lef;
		ay = (funcDetent(setting[2 * j + 8], detent) - setting[2]) / yScale + bot;
		if (j == 0)
			MoveToEx(pg->hDC, pg->cXo + ax, pg->cYo + ay, NULL);
		else
			LineTo(pg->hDC, pg->cXo + ax, pg->cYo + ay);
	}
	EndPath(pg->hDC);
	StrokePath(pg->hDC);


	// then the knobs
	for (j = 0; j < setting[6]; j++) {
		ax = (funcDetent(setting[2 * j + 7], detent) - setting[0]) / xScale + lef;
		ay = (funcDetent(setting[2 * j + 8], detent) - setting[2]) / yScale + bot;

		BeginPath(pg->hDC);
		Ellipse(pg->hDC, pg->cXo + ax - TERMINAL_SIZE, pg->cYo + ay - TERMINAL_SIZE,
			pg->cXo + ax + TERMINAL_SIZE, pg->cYo + ay + TERMINAL_SIZE);
		EndPath(pg->hDC);
		SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
		StrokeAndFillPath(pg->hDC);

		std::string text = tomString(j);
		//int width = ctx.measureText(text).width;
		//SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
		drawTextString(pg->hDC, pg->cXo + ax - 4, pg->cYo + ay - 6, 0, text);
	}

	// show the value of the last dial moved above the rack
	j = memory[0];
	if (j >= setting[6])
		j = 0;
	if (j >= 0) {
		std::string text = "[" + tomString(j) + "] = ("
			+ tomString(reasonableValue(funcDetent(setting[2 * j + 7], detent))) + ", "
			+ tomString(reasonableValue(funcDetent(setting[2 * j + 8], detent))) + ")";
		int textWidth = calcStringWidthFont(pg->hDC, text);
		//SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
		drawTextString(pg->hDC, pg->cXo + x - textWidth / 2, pg->cYo + y - height - 12, 0, text);
	}
}


int Geom_Component::method2(Sheet* pg) {
	// update if needed
	//if (element[0] == undefined) {
	//	element[0] = 0; // no detents
	//}

	// reset the outputs
	output[0].value.resize(setting[6]);
	output[1].value.resize(setting[6]);

	double detent = element[0];
	for (int j = 0; j < setting[6]; j++) {
		output[0].value[j] = funcDetent(setting[2 * j + 7], detent);
		output[1].value[j] = funcDetent(setting[2 * j + 8], detent);
	}
	output[0].dimensions[0] = setting[6];
	output[1].dimensions[0] = setting[6];

	width = setting[4];
	height = setting[5];

	return true;
}

bool Geom_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	int lef = x - width + TERMINAL_SIZE;
	int rig = x + width - 2 * TERMINAL_SIZE;
	double xScale = (rig - lef) / (setting[1] - setting[0]);

	int top = y - height + TERMINAL_SIZE;
	int bot = y + height - TERMINAL_SIZE;
	double yScale = (top - bot) / (setting[3] - setting[2]);

	int n = setting[6];
	bool hitOne = false;
	int j;
	if (check) {
		//  first find the current dial position
		for (j = n-1; j >= 0; j--) {

			double detent = element[0];
			int currentX = (funcDetent(setting[2 * j + 7], detent) - setting[0]) * xScale + lef;
			int currentY = (funcDetent(setting[2 * j + 8], detent) - setting[2]) * yScale + bot;
		//	currentX += 3;
		//	currentY -= 10;
			if (ax > currentX + TERMINAL_SIZE)
				continue; // missed dial
			if (ax < currentX - TERMINAL_SIZE)
				continue; // missed dial

			if (ay > currentY + TERMINAL_SIZE)
				continue; // missed dial
			if (ay < currentY - TERMINAL_SIZE)
				continue; // missed dial

			memory[0] = j;
			hitOne = true;
			break;
		}
		if (!hitOne) {
			// check for hit on resize box
			if (crankResize(ax, ay, true)) {
				memory[0] = -9; // rember it was the resize not a geom point

				if (width < 30)
					width = 30;
				setting[4] = width;

				if (height < 30)
					height = 30;
				setting[5] = height;

				// because the input terminal moved
				analyzeConnections(&pg->dgrm);
				propagateValues(&pg->dgrm);
				drawCanvas();

				// move the dial
				dial(pg);
				return true;
			}
			return false;
		}
	}
	else {
		if (memory[0] == -9) { // still resizeing
			crankResize(ax, ay, false);

			if (width < 30)
				width = 30;
			setting[4] = width;

			if (height < 30)
				height = 30;
			setting[5] = height;

			// because the input terminal moved
			analyzeConnections(&pg->dgrm);
			propagateValues(&pg->dgrm);
			drawCanvas();

			// move the dial
			dial(pg);
			return true;
		}
		else
			j = memory[0];
	}

	if (ax > rig)
		ax = rig;
	if (ax < lef)
		ax = lef;
	if (ay > bot)
		ay = bot;
	if (ay < top)
		ay = top;

	setting[2 * j + 7] = reasonableValue(overScale((ax - lef), xScale) + setting[0]);
	setting[2 * j + 8] = reasonableValue(overScale((ay - bot), yScale) + setting[2]);

	// move the dial
	dial(pg);
	method2(pg);

	return true;
}


void Geom_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	int oldKnobs = floor(setting[6]);
	int newKnobs = floor(doubleValues[0]);
	if (newKnobs > 32)
		newKnobs = 32;
	if (newKnobs < 1)
		newKnobs = 1;
	setting[6] = newKnobs;
	setting.resize(7 + 2 * newKnobs);

	if (doubleValues[1] > doubleValues[2]) {
		double temp = doubleValues[1];
		doubleValues[1] = doubleValues[2];
		doubleValues[2] = temp;
	}
	if (doubleValues[3] > doubleValues[4]) {
		double temp = doubleValues[3];
		doubleValues[3] = doubleValues[4];
		doubleValues[4] = temp;
	}
	setting[0] = doubleValues[1];
	setting[1] = doubleValues[2];
	setting[2] = doubleValues[3];
	setting[3] = doubleValues[4];

	double tempx = (doubleValues[1] + doubleValues[2]) / 2.0;
	double tempy = (doubleValues[3] + doubleValues[4]) / 2.0;
	for (int j = oldKnobs; j < newKnobs; j++) {
		setting[7 + 2*j] = tempx;
		setting[8 + 2*j] = tempy;
	}

	for (int j = 0; j < newKnobs; j++) {
		// check for bad X limits
		if (setting[7 + 2 * j] < doubleValues[1])
			setting[7 + 2 * j] = doubleValues[1];
		if (setting[7 + 2 * j] > doubleValues[2])
			setting[7 + 2 * j] = doubleValues[2];
		// check for bad Y limits
		if (setting[8 + 2 * j] < doubleValues[3])
			setting[8 + 2 * j] = doubleValues[3];
		if (setting[8 + 2 * j] > doubleValues[4])
			setting[8 + 2 * j] = doubleValues[4];
	}
	onLoad(pg);
}
void doubleClickGeom(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Set Point Count:", "Set Minimum X:", "Set Maximum X:", "Set Minimum Y:", "Set Maximum Y:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[6], pg->dgrm.cL[i]->setting[0], 
		pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2],pg->dgrm.cL[i]->setting[3] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

void actionGeomCopy(Sheet* pg, int i) {
	string aString;

	int points = pg->dgrm.cL[i]->setting[6];
	
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
void actionGeomPaste(Sheet* pg, int i) {

	HANDLE theData = getDataFromClipboard();

	char* text = (char*)theData;
	int jt = 0;
	char buffer[50];
	int jb = 0;

	pg->dgrm.cL[i]->output[0].value.clear();
	pg->dgrm.cL[i]->output[1].value.clear();
	pg->dgrm.cL[i]->setting.resize(7); // minx, max, miny, maxy, width, height, points

	double minX = 1e100;
	double maxX = -1e100;
	double minY = 1e100;
	double maxY = -1e100;
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
			if( readingX ){
				pg->dgrm.cL[i]->output[0].value.push_back(value);
				pg->dgrm.cL[i]->setting.push_back(value);
				if (value < minX)
					minX = value;
				if (value > maxX)
					maxX = value;
			}
			else {
				pg->dgrm.cL[i]->output[1].value.push_back(value);
				pg->dgrm.cL[i]->setting.push_back(value);
				if (value < minY)
					minY = value;
				if (value > maxY)
					maxY = value;
				points++;
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
			if (value < minX)
				minX = value;
			if (value > maxX)
				maxX = value;
		}
		else {
			pg->dgrm.cL[i]->output[1].value.push_back(value);
			pg->dgrm.cL[i]->setting.push_back(value);
			if (value < minY)
				minY = value;
			if (value > maxY)
				maxY = value;
			points++;
		}
	}
	pg->dgrm.cL[i]->setting[0] = minX;
	pg->dgrm.cL[i]->setting[1] = maxX;	// max Y range
	pg->dgrm.cL[i]->setting[2] = minY;
	pg->dgrm.cL[i]->setting[3] = maxY;	// max Y range
	pg->dgrm.cL[i]->setting[6] = points;
	pg->dgrm.cL[i]->output[0].dimensions[0] = points;
	pg->dgrm.cL[i]->output[1].dimensions[0] = points;

	pg->activeDialogComponent = -1;

	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}


std::vector<Button*> geomButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickGeom),
	new Button(-82, 40, 80, 20, L"Copy", actionGeomCopy),
	new Button(-82, 60, 80, 20, L"Paste", actionGeomPaste) };

bool Geom_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
Geom_Component::Geom_Component()
	: Component(L"Geom", L"Geometry", squareShape, 0, 0, 2, 2)
{
	dialogButtonList = geomButtonList;
	setMemory(0, 0); // used to show recent changes
	setElement(0, 0);  // save detent here (not usual)
	setSetting(0, -100);   // min X range
	setSetting(1, 100);   // max X range
	setSetting(2, -100);   // min Y range
	setSetting(3, 100);   // max Y range
	width = 50;
	height = 50;
	setSetting(4, width);
	setSetting(5, height);
	setSetting(6, 3); // number of points
	setSetting(7, -65);  // value of x[0]
	setSetting(8, 65);  // value of y[0]
	setSetting(9, 45);  // value of x[1]
	setSetting(10, 55);  // value of y[1]
	setSetting(11, 15);  // value of x[2]
	setSetting(12, -25);  // value of y[2]
	setOutput(0, 0, setting[7]);
	setOutput(1, 0, setting[8]);
	setOutput(0, 1, setting[9]);
	setOutput(1, 1, setting[10]);
	setOutput(0, 2, setting[11]);
	setOutput(1, 2, setting[12]);
	setRightLabels(3, "X", "Y");
}



// ************************* Function ***************************

	// setting[0] = number of points
	// setting[1] = width of component
	// setting[2] = height of component
	// setting[3] = min Y range
	// setting[4] = max Y range
	// setting[5] = detent
	// element[0] = value of y[0]
	// element[1] = value of y[1]

void Function_Component::dial(Sheet* pg) {
	int lef = x - width + TERMINAL_SIZE;
	int rig = x + width - 2 * TERMINAL_SIZE;
	double xScale = (setting[0]-1) / (rig - lef);

	int top = y - height + TERMINAL_SIZE;
	int bot = y + height - TERMINAL_SIZE;
	double yScale = (setting[4] - setting[3]) / (top - bot);

	// show a gray pad
	RECT aRect;
	aRect.left = pg->cXo + lef;
	aRect.top = pg->cYo + top;
	aRect.right = pg->cXo + rig;
	aRect.bottom = pg->cYo + bot;

	HBRUSH fillStyle = GetColorBrush(0xE0E0E0); 
	FillRect(pg->hDC, &aRect, fillStyle);

	drawResizeBox(pg, this);

	// then the lines
	int j, ax, ay;
	double detent = setting[5];
	
	BeginPath(pg->hDC);
	for (j = 0; j < setting[0]; j++) {
		ax = j / xScale + lef;
		ay = (funcDetent(element[j], detent) - setting[3]) / yScale + bot;
		if (j == 0)
			MoveToEx(pg->hDC, pg->cXo + ax, pg->cYo + ay, NULL);
		else
			LineTo(pg->hDC, pg->cXo + ax, pg->cYo + ay);
	}
	EndPath(pg->hDC);
	StrokePath(pg->hDC);

	// then the dots
	int dotSize = 3;
	BeginPath(pg->hDC);
	for (j = 0; j < setting[0]; j++) {
		ax = j  / xScale + lef;
		ay = (funcDetent(element[j], detent) - setting[3]) / yScale + bot;
		Ellipse(pg->hDC, pg->cXo + ax - dotSize, pg->cYo + ay - dotSize,
			pg->cXo + ax + dotSize, pg->cYo + ay + dotSize);
	}
	EndPath(pg->hDC);
	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
	StrokeAndFillPath(pg->hDC);

	// show the value of the last dial moved above the rack
	j = memory[0];
	if (j >= setting[0])
		j = 0;
	if (j >= 0) {
		std::string text = "[" + tomString(j) + "] = "
			+ tomString(reasonableValue(funcDetent(element[j], detent)));
		int textWidth = calcStringWidthFont(pg->hDC, text);
		drawTextString(pg->hDC, pg->cXo + x - textWidth / 2, pg->cYo + y - height - 12, 0, text);
	}
}


int Function_Component::method2(Sheet* pg) {
	
	// reset the outputs
	output[0].value.resize(setting[0]);

	double detent = setting[5];
	for (int j = 0; j < setting[0]; j++) 
		output[0].value[j] = funcDetent(element[j], detent);
	
	output[0].dimensions[0] = setting[0];
	
	width = setting[1];
	height = setting[2];

	return true;
}

bool Function_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	int lef = x - width + TERMINAL_SIZE;
	int rig = x + width - 2 * TERMINAL_SIZE;
	double xScale = (rig - lef) / (setting[0]-1);

	int top = y - height + TERMINAL_SIZE;
	int bot = y + height - TERMINAL_SIZE;
	double yScale = (top - bot) / (setting[4] - setting[3]);

	int n = setting[0];
	bool hitOne = false;
	double detent = setting[5];
	int j;
	if (check) {
		//  first find the current dial position
		for (j = 0; j < n; j++) {
			int currentX = j * xScale + lef;
			int currentY = (funcDetent(element[j], detent) - setting[3]) * yScale + bot;
			
			if (ax > currentX + TERMINAL_SIZE)
				continue; // missed dial
			if (ax < currentX - TERMINAL_SIZE)
				continue; // missed dial

			if (ay > currentY + TERMINAL_SIZE)
				continue; // missed dial
			if (ay < currentY - TERMINAL_SIZE)
				continue; // missed dial

			memory[0] = j;
			hitOne = true;
			break;
		}
		if (!hitOne) {
			// check for hit on resize box
			if (crankResize(ax, ay, true)) {
				memory[0] = -999; // rember it was the resize not a geom point

				if (width < 30)
					width = 30;
				setting[1] = width;

				if (height < 30)
					height = 30;
				setting[2] = height;

				// because the input terminal moved
				analyzeConnections(&pg->dgrm);
				propagateValues(&pg->dgrm);
				drawCanvas();

				// move the dial
				dial(pg);
				return true;
			}
			return false;
		}
	}
	else {
		if (memory[0] == -999) { // still resizeing
			crankResize(ax, ay, false);

			if (width < 30)
				width = 30;
			setting[1] = width;

			if (height < 30)
				height = 30;
			setting[2] = height;

			// because the input terminal moved
			analyzeConnections(&pg->dgrm);
			propagateValues(&pg->dgrm);
			drawCanvas();

			// move the dial
			dial(pg);
			return true;
		}
		else
			j = memory[0];
	}

	if (ax > rig)
		ax = rig;
	if (ax < lef)
		ax = lef;
	if (ay > bot)
		ay = bot;
	if (ay < top)
		ay = top;

	element[j] = reasonableValue(overScale((ay - bot), yScale) + setting[3]);

	// move the dial
	dial(pg);
	method2(pg);

	return true;
}


void Function_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	int oldPoints = floor(setting[0]);
	int newPoints = floor(doubleValues[0]);
	if (newPoints > 256)
		newPoints = 256;
	if (newPoints < 2)
		newPoints = 2;
	setting[0] = newPoints;
	element.resize(newPoints);

	if (doubleValues[1] > doubleValues[2]) {
		double temp = doubleValues[1];
		doubleValues[1] = doubleValues[2];
		doubleValues[2] = temp;
	}

	setting[3] = doubleValues[1];	// y min
	setting[4] = doubleValues[2];	// y max
	setting[5] = doubleValues[3];	// detent  ************************ soon

	double midY = (doubleValues[1] + doubleValues[2]) / 2.0;
	for (int j = oldPoints; j < newPoints; j++) {
		element[j] = midY;
	}

	for (int j = 0; j < newPoints; j++) {
		// check for bad Y limits
		if (element[j] < doubleValues[1])
			element[j] = doubleValues[1];
		if (element[j] > doubleValues[2])
			element[j] = doubleValues[2];
	}
	onLoad(pg);
}

void doubleClickFunction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Set Point Count:", "Set Minimum Y:", "Set Maximum Y:", "Set Detent:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[3],
		pg->dgrm.cL[i]->setting[4], pg->dgrm.cL[i]->setting[5] };

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}


void actionFunctionCopy(Sheet* pg, int i) {
	string aString;

	int len = pg->dgrm.cL[i]->setting[0];
	for (int j = 0; j < len; j++) {
		if (j)
			aString += "\r\n";
		aString += tomString(pg->dgrm.cL[i]->element[j]);
	}
	
	copyStringToClipboard(aString);
	pg->activeDialogComponent = -1;
}


void actionFunctionPaste(Sheet* pg, int i) {

	HANDLE theData = getDataFromClipboard();

	char* text = (char*)theData;
	int jt = 0;
	char buffer[50];
	int jb = 0;

	pg->dgrm.cL[i]->output[0].value.clear();
	pg->dgrm.cL[i]->element.clear();
	double minimum = 1e100;
	double maximum = -1e100;

	while (text[jt] != 0) {
		if ((jb == 0) && (text[jt] == ' ')) {
			jt++;
			continue;
		}

		if ((text[jt] == ' ') || (text[jt] == ',') || (text[jt] == '\t') || (text[jt] == '\n') || (text[jt] == '\r')) {
			buffer[jb] = 0;
			double value = atof(buffer);
			if (value < minimum)
				minimum = value;
			if (value > maximum)
				maximum = value;
			pg->dgrm.cL[i]->output[0].value.push_back(value);
			pg->dgrm.cL[i]->element.push_back(value);
			jb = 0;

			if ((text[jt] == '\n') && (text[jt + 1] == '\r'))
				jt++;
			if ((text[jt] == '\r') && (text[jt + 1] == '\n'))
				jt++;
			jt++;
		}
		else
			buffer[jb++] = text[jt++];
	}
	if (jb) {
		buffer[jb] = 0;
		double value = atof(buffer);
		if (value < minimum)
			minimum = value;
		if (value > maximum)
			maximum = value;
		pg->dgrm.cL[i]->output[0].value.push_back(value);
		pg->dgrm.cL[i]->element.push_back(value);
	}

	pg->dgrm.cL[i]->setting[0] = pg->dgrm.cL[i]->element.size();// number of points
	pg->dgrm.cL[i]->setting[3] = minimum;	// min Y range
	pg->dgrm.cL[i]->setting[4] = maximum;	// max Y range

	pg->activeDialogComponent = -1;

	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}

std::vector<Button*> functionButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClickFunction),
	new Button(-72, 40, 70, 20, L"Copy", actionFunctionCopy),
	new Button(-72, 60, 70, 20, L"Paste", actionFunctionPaste) };

bool Function_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
Function_Component::Function_Component()
	: Component(L"Function", L"Function", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = functionButtonList;
	setSetting(0, 6); // number of points
	width = 50;
	height = 50;
	setSetting(1, width);
	setSetting(2, height);
	setSetting(3, -100);	// min Y range
	setSetting(4, 100);		// max Y range
	setSetting(5, 0);		// detent
	setMemory(0, 0);
	setElement(0, 45);
	setElement(1, -100);
	setElement(2, 22);
	setElement(3, 14);
	setElement(4, 100);
	setElement(5, 73);
	setOutput(0, 0, 45);
	setOutput(0, 1, -100);
	setOutput(0, 2, 22);
	setOutput(0, 3, 14);
	setOutput(0, 4, 100);
	setOutput(0, 5, 73);
}

// *********************** In Poly ********************************

int InPoly_Component::method2(Sheet* pg) {

	int loops = inputsToLoopsN(3);
	if (loops == 0)
		return clearOutputs();
	if (input[3].value.size() < 2)
		return clearOutputs();
	if (input[3].value.size() != input[4].value.size())
		return clearOutputs();

	output[0].value.resize(loops);
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = loops;

	output[1].value.resize(loops);
	output[1].dimensions.resize(1);
	output[1].dimensions[0] = loops;

	int polyLen = input[3].value.size();
	std::vector<double> polyX;
	std::vector<double> polyY;
	polyX.resize(polyLen);
	polyY.resize(polyLen);
	for (int j = 0; j < polyLen; j++) {
		polyX[j] = input[3].value[j];
		polyY[j] = input[4].value[j];
	}

	if ((polyX[0] != polyX[polyLen - 1]) ||
		(polyY[0] != polyY[polyLen - 1])) {
		polyX.push_back(polyX[0]);
		polyY.push_back(polyY[0]);
		polyLen++;
	}

	for (int k = 0; k < loops; k++) {
		double x = getLoopingValueD(0, k, 0);
		double y = getLoopingValueD(1, k, 0);
		double thresh = getLoopingValueD(2, k, 1);
		output[0].value[k] = 0; // start with not inside
		output[1].value[k] = -1; // start with not on a line

		// count the crossings to see if we are inside the polygon
		int crossings = 0;
		for (int j = 0; j < polyLen - 1; j++) {
			if (y > max(polyY[j], polyY[j + 1]))
				continue; // point is above the line segment, skip
			if (y < min(polyY[j], polyY[j + 1]))
				continue; // point is below the line segment, skip
			if (x < min(polyX[j], polyX[j + 1]))
				continue; // point is left of the line segment, skip
			if (x > max(polyX[j], polyX[j + 1])) {
				crossings++;
				continue; // point is right of the line segment, got it
			}
			// careful check is needed - count the crossing
			double slope = (polyY[j] - polyY[j + 1]) / (polyX[j] - polyX[j + 1]);
			double offset = polyY[j] - slope * polyX[j];
			double testX = (y - offset) / slope;
			if (testX <= x)
				crossings++;

			// how close to the line in x direction?
			if (abs(testX - x) < thresh) {
				output[1].value[k] = j; // which line was hit
			}
			else {
				double testY = slope * x + offset;
				if (abs(testY - y) < thresh)
					output[1].value[k] = j; // which line was hit
			}
		}
		if (crossings & 1)
			output[0].value[k] = 1; // odd crossings means we are inside
	}
	//output[0].dimensions = input[0].dimensions;
	//output[1].dimensions = input[1].dimensions;

	return true;
}


InPoly_Component::InPoly_Component()
	: Component(L"InPoly", L"In Polygon", squareShape, 5, 1, 2, 1)
{
	setLeftLabels(3, "X", "Y", "Edge Width", "Poly Xs", "Poly Ys");
	setRightLabels(3, "Inside", "On Edge");
	width = 55;
}



// ****************** To RGB **************************************


int ToRGB_Component::method2(Sheet* pg) {
	// http://en.wikipedia.org/wiki/HSL_and_HSV
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();

	//setOutputMatrixSizes_(pg, i, result.dims);
	output[0].value.resize( loops );
	output[1].value.resize(loops);
	output[2].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		double hue = getLoopingValueD(0, k, 0);
		double saturation = getLoopingValueD(1, k, 0);
		double value = getLoopingValueD(2, k, 0);

		double chroma = saturation * value;
		double hdash = hue / 60.0;
		double x = chroma * (1.0 - abs(((int)hdash % 2) - 1.0));

		int red = 0;
		int green = 0;
		int blue = 0;

		if (hdash < 1.0) {
			red = chroma;
			green = x;
		}
		else if (hdash < 2.0) {
			red = x;
			green = chroma;
		}
		else if (hdash < 3.0) {
			green = chroma;
			blue = x;
		}
		else if (hdash < 4.0) {
			green = x;
			blue = chroma;
		}
		else if (hdash < 5.0) {
			red = x;
			blue = chroma;
		}
		else if (hdash <= 6.0) {
			red = chroma;
			blue = x;
		}

		int min = value - chroma;

		red += min;
		green += min;
		blue += min;

		output[0].value[k] = red;
		output[1].value[k] = green;
		output[2].value[k] = blue;
	}
	for (int j = 0; j < 3; j++)
		copyDimensions(j, j);
	return true;
}


ToRGB_Component::ToRGB_Component()
	: Component(L"ToRGB", L"To RGB", squareShape, 3, 1, 3, 1)
{
	setLeftLabels(3, "Hue", "Saturation", "Value");
	setRightLabels(3, "Red", "Green", "Blue");
	height = pinToHeight(3);
	width = 50;
}



int ToHSV_Component::method2(Sheet* pg) {
	// http://en.wikipedia.org/wiki/HSL_and_HSV
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();

	//setOutputMatrixSizes_(pg, i, result.dims);
	output[0].value.resize(loops);
	output[1].value.resize(loops);
	output[2].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		int red = getLoopingValueD(0, k, 0);
		int green = getLoopingValueD(1, k, 0);
		int blue = getLoopingValueD(2, k, 0);

		int min = min(min(red, green), blue);
		int max = max(max(red, green), blue);
		double chroma = max - min;

		//If Chroma is 0, then S is 0 by definition, and H is undefined but 0 by convention.
		double hue = 0;
		double saturation = 0;

		if (chroma != 0) {
			if (red == max) {
				hue = (green - blue) / chroma;

				if (hue < 0.0) {
					hue += 6.0;
				}
			}
			else if (green == max) {
				hue = ((blue - red) / chroma) + 2.0;
			}
			else //blue == max
			{
				hue = ((red - green) / chroma) + 4.0;
			}

			hue *= 60.0;
			saturation = chroma / max;
		}

		int value = max;
		output[0].value[k] = hue;
		output[1].value[k] = saturation;
		output[2].value[k] = value;
	}

	for (int j = 0; j < 3; j++) 
		copyDimensions(j, j);
	
	return true;
}

ToHSV_Component::ToHSV_Component()
	: Component(L"ToHSV", L"To HSV", squareShape, 3, 1, 3, 1)
{
	setRightLabels(3, "Hue", "Saturation", "Value");
	setLeftLabels(3, "Red", "Green", "Blue");
	height = pinToHeight(3);
	width = 50;
}


int ToHMS_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	if (input[0].drivenByComponent == -1)    // single input
		return clearOutputs();

	//setOutputMatrixSizes_(pg, i, result.dims);
	output[0].value.resize(loops);
	output[1].value.resize(loops);
	output[2].value.resize(loops);
	output[3].stringValue.resize(loops);
	output[4].stringValue.resize(loops);

	for (int k = 0; k < loops; k++) {
		double day = input[0].value[k];
		day = day - floor(day);

		double hours = floor(day * 24);
		day -= hours / 24;
		double minutes = floor(day * 1440);
		day -= minutes / 1440;
		double seconds = floor(day * 86400);

		output[3].stringValue.resize(loops);
		output[3].stringValue.resize(loops);
		output[0].value[k] = hours;
		output[1].value[k] = minutes;
		output[2].value[k] = seconds;

		//  24 hour time
		std::string text;
		if (hours < 10) text += "0";
		text += tomString((int)hours) + ":";
		if (minutes < 10) text += "0";
		text += tomString((int)minutes) + ":";
		if (seconds < 10) text += "0";
		text += tomString((int)seconds);
		output[3].stringValue[k] = text;

		// 12 hour am / pm time
		text = "";
		bool pm = false;
		if (hours == 0)
			hours = 12;
		else if (hours >= 12)
			pm = true;
		if (hours > 12)
			hours -= 12;
		text += tomString((int)hours) + ":";
		if (minutes < 10) text += "0";
		text += tomString((int)minutes) + ":";
		if (seconds < 10) text += "0";
		text += tomString((int)seconds);
		if (pm)
			text += " pm";
		else
			text += " am";
		output[4].stringValue[k] = text;
	}
	return true;
}

ToHMS_Component::ToHMS_Component()
	: Component(L"ToHMS", L"To HMS", squareShape, 1, 1, 5, 1)
{
	setRightLabels(5, "Hour", "Minute", "Second", "HH:MM:SS", "H:MM:SS am");
	height = pinToHeight(5);
	width = 40;
	output[3].stringType = true;
	output[4].stringType = true;
}


int ToHM_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	if (input[0].drivenByComponent == -1)    // single input
		return clearOutputs();

	//setOutputMatrixSizes_(pg, i, result.dims);
	output[0].value.resize(loops);
	output[1].value.resize(loops);
	output[2].stringValue.resize(loops);
	output[3].stringValue.resize(loops);

	for (int k = 0; k < loops; k++) {
		double day = input[0].value[k];
		day = day - floor(day);

		double hours = floor(day * 24);
		day -= hours / 24;
		double minutes = floor(day * 1440);
		day -= minutes / 1440;
		double seconds = floor(day * 86400);
		if (seconds >= 30) {
			minutes++;
			if (minutes >= 60) {
				minutes -= 60;
				hours++;
			}
		}
		output[0].value[k] = hours;
		output[1].value[k] = minutes;


		//  24 hour time
		std::string text;
		if (hours < 10) text += "0";
		text += tomString((int)hours) + ":";
		if (minutes < 10) text += "0";
		text += tomString((int)minutes);
		output[2].stringValue[k] = text;

		// 12 hour am / pm time
		text = "";
		bool pm = false;
		if (hours == 0)
			hours = 12;
		else if (hours >= 12)
			pm = true;
		if (hours > 12)
			hours -= 12;
		text += tomString((int)hours) + ":";
		if (minutes < 10) text += "0";
		text += tomString((int)minutes);
		if (pm)
			text += " pm";
		else
			text += " am";
		output[3].stringValue[k] = text;

	}
	return true;
}


ToHM_Component::ToHM_Component()
	: Component(L"ToHM", L"To HM", squareShape, 1, 1, 4, 1)
{
	setRightLabels(4, "Hour", "Minute", "HH:MM", "H:MM am");
	height = pinToHeight(4);
	output[2].stringType = true;
	output[3].stringType = true;
}







int StrLen_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].stringValue.size();
	output[0].value.resize(size);

	for (int j = 0; j < size; j++)
		output[0].value[j] = input[0].stringValue[j].size();
	copyDimensions(0, 0);

	return true;
}

StrLen_Component::StrLen_Component()
	: Component(L"StrLen", L"String Length", 1, 1)
{}




// returns -1 0 +1
int innerAlphaSort(int n, std::string inA, std::string inB) {
	if (inA == inB)
		return 0;
	if (inA > inB)
		return 1;
	return -1;
}


// actually sort the incoming data
int AlphaSort_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].stringValue.size();
	output[0].stringValue.resize(size);
	for (int j = 0; j < size; j++)
		output[0].stringValue[j] = input[0].stringValue[j];
	output[0].dimensions[0] = size;

	// simple slow bubble sort
	bool innerChange;
	bool anyChange = false;
	std::string temp;
	int delta;
	do {
		innerChange = false;
		for (int j = 0; j < size - 1; j++) {
			delta = innerAlphaSort(0, output[0].stringValue[j], output[0].stringValue[j + 1]);
			if (delta == 0)
				continue;
			if (delta > 0) {
				temp = output[0].stringValue[j];
				output[0].stringValue[j] = output[0].stringValue[j + 1];
				output[0].stringValue[j + 1] = temp;
				innerChange = true;
			}
		}
		if (innerChange)
			anyChange = true;

	} while (innerChange);

	return anyChange;
}


AlphaSort_Component::AlphaSort_Component()
	: Component(L"ASort", L"Alpha Sort", 1, 1)
{
	output[0].stringType = true;
}



// give the order (sequence) if the data was sorted
int AlphaRank_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].stringValue.size();
	std::vector<std::string> data;
	data.resize(size);

	std::vector<int> key;
	key.resize(size);
	output[0].value.resize(size);

	for (int j = 0; j < size; j++) {
		data[j] = input[0].stringValue[j];
		key[j] = j;
	}

	bool innerChange;
	bool anyChange = false;
	std::string tempstring;
	int tempkey;
	int delta;
	do {
		innerChange = false;
		for (int j = 0; j < size - 1; j++) {
			delta = innerAlphaSort(0, data[j], data[j + 1]);
			if (delta == 0)
				continue;
			if (delta > 0) {
				tempstring = data[j];
				data[j] = data[j + 1];
				data[j + 1] = tempstring;

				tempkey = key[j];
				key[j] = key[j + 1];
				key[j + 1] = tempkey;

				innerChange = true;
			}
		}
		if (innerChange)
			anyChange = true;

	} while (innerChange);

	for (int j = 0; j < size; j++)
		output[0].value[j] = key[j];
	output[0].dimensions[0] = size;

	return anyChange;
}

AlphaRank_Component::AlphaRank_Component()
	: Component(L"ARank", L"Alpha Rank", 1, 1)
{}


int UpCase_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].stringValue.size();
	output[0].stringValue.resize(size);

	for (int j = 0; j < size; j++) {
		int len = input[0].stringValue[j].size();
		for (int k = 0; k < len; k++) {
			if ((input[0].stringValue[j][k] >= 'a') && (input[0].stringValue[j][k] <= 'z'))
				output[0].stringValue[j] += input[0].stringValue[j][k] - 'a' + 'A';
			else
				output[0].stringValue[j] += input[0].stringValue[j][k];
		}
	}
	copyDimensions(0, 0);
	return true;
}

UpCase_Component::UpCase_Component()
	: Component(L"UpCase", L"Up Case", 1, 1)
{
	output[0].stringType = true;
}


int LoCase_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].stringValue.size();
	output[0].stringValue.resize(size);

	for (int j = 0; j < size; j++) {
		int len = input[0].stringValue[j].size();
		for (int k = 0; k < len; k++) {
			if ((input[0].stringValue[j][k] >= 'A') && (input[0].stringValue[j][k] <= 'Z'))
				output[0].stringValue[j] += input[0].stringValue[j][k] - 'A' + 'a';
			else
				output[0].stringValue[j] += input[0].stringValue[j][k];
		}
	}
	copyDimensions(0, 0);
	return true;
}

LoCase_Component::LoCase_Component()
	: Component(L"LoCase", L"Low Case", 1, 1)
{
	output[0].stringType = true;
}

// **************** To Number *****************

int ToNumber_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].stringValue.size();
	output[0].value.resize(size);

	for (int j = 0; j < size; j++)
		output[0].value[j] = tomNumber(input[0].stringValue[j]);
	copyDimensions(0, 0);
	return true;
}

ToNumber_Component::ToNumber_Component()
	: Component(L"ToNumber", L"To Number", 1, 1)
{}

// ********************* To ASCII *************************


int ToASCII_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();
	if (!input[0].stringType)
		return clearOutputs();

	if (input[0].stringValue.size() == 1) {
		// get all the characters in this string
		int size = input[0].stringValue[0].size();
		output[0].value.resize(size);
		string text = input[0].stringValue[0];
		for (int j = 0; j < size; j++)
			output[0].value[j] = text[j];
	}
	else
	{
		// get the first letter in each of the array of input strings
		int size = input[0].stringValue.size();
		output[0].value.resize(size);
		for (int j = 0; j < size; j++) {
			string text = input[0].stringValue[j];
			output[0].value[j] = text[0];
		}
	}
	output[0].dimensions[0] = output[0].value.size();
	
	return true;
}

ToASCII_Component::ToASCII_Component()
	: Component(L"ToASCII", L"To ASCII", 1, 1)
{}

// ******************* to string ******************************

int ToString_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].value.size();
	output[0].stringValue.resize(size);

	for (int j = 0; j < size; j++)
		output[0].stringValue[j] = tomString(input[0].value[j]);
	copyDimensions(0, 0);
	return true;
}

ToString_Component::ToString_Component()
	: Component(L"ToString", L"To String", 1, 1)
{
	output[0].stringType = true;
}


// ******************* to char ******************************

int ToChar_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].value.size();
	output[0].stringValue.resize(1);
	output[0].stringValue[0] = "";
	for (int j = 0; j < size; j++)
		output[0].stringValue[0] += input[0].value[j];
	copyDimensions(0, 0);
	return true;
}

ToChar_Component::ToChar_Component()
	: Component(L"ToChar", L"To Char", 1, 1)
{
	output[0].stringType = true;
}

// ******************* sub string **********************************************

// string array, start, length
int SubStr_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].stringType = true;
	output[0].stringValue.resize(loops);

	int jout = 0;
	int start;
	int len;
	std::string string;
	for (int j = 0; j < loops; j++) {
		string = getLoopingStringValueD(0, j, "");
		start = getLoopingValueD(1, j, 0);
		if (start < 0)
			start = 0;
		len = getLoopingValueD(2, j, 1);
		if (len < 0)
			len = 0;

		if (start + len <= string.size() )
			output[0].stringValue[jout++] = string.substr(start, len);
		else
			output[0].stringValue[jout++] = "";
	}
	output[0].dimensions[0] = loops;
	return true;
}

SubStr_Component::SubStr_Component()
	: Component(L"SubStr", L"Sub String", 3, 1)
{
	setLeftLabels(3, "Array", "Start", "Length");
	output[0].stringType = true;
}


int Parse_Component::method2(Sheet* pg)        // handle calc for a function with just one input
{
	// parse the single left input into right output with n elements
	if ((input[0].drivenByComponent == -1) ||
		(input[1].drivenByComponent == -1)) {
		return clearOutputs();
	}

	output[0].stringValue.clear(); // because we dont know the results
	output[0].stringType = true;

	if(input[0].stringValue.size() == 0 )
		return clearOutputs();
	std::string textIn = input[0].stringValue[0];

	std::string segText;
	if (input[1].stringType) {
		if (input[1].stringValue.size() == 0)
			return clearOutputs();
		segText = input[1].stringValue[0];
	}
	else {
		if (input[1].value.size() == 0)
			return clearOutputs();
		for( int k = 0; k < input[1].value.size(); k++)
			segText += input[1].value[k];
	}

	if (segText.size()) {
		size_t pos = 0;
		while ((pos = textIn.find(segText)) != std::string::npos) {
			output[0].stringValue.push_back(textIn.substr(0, pos));
			textIn.erase(0, pos + segText.length());
		}
		if (textIn.size())
			output[0].stringValue.push_back(textIn);
	}else
		output[0].stringValue.push_back(textIn);

	output[0].dimensions[0] = output[0].stringValue.size();
	return true;
}

Parse_Component::Parse_Component()
	: Component(L"Parse", L"String Parse", 2, 1)
{
	setLeftLabels(2, "String", "Separator");
	output[0].stringType = true;
}

// string array, index
int CharAt_Component::method2(Sheet* pg) {

	// need all inputs
	for (int j = 0; j < 2; j++) {
		if (input[j].drivenByComponent == -1)
			return clearOutputs();
	}
	int stringSize = input[0].stringValue.size(); // todo make this block a function
	int indexSize = input[1].value.size();

	int max = max(stringSize, indexSize);
	if (((stringSize != max) && (stringSize != 1)) ||
		((indexSize != max) && (indexSize != 1)))
		return clearOutputs();

	output[0].stringValue.resize(max);

	int index;
	std::string string;
	for (int j = 0; j < max; j++) {

		if (stringSize == 1)
			string = input[0].stringValue[0];
		else
			string = input[0].stringValue[j];

		if (indexSize == 1)
			index = input[1].value[0];
		else
			index = input[1].value[j];
		if (index >= string.size())
			output[0].stringValue[j] = "";
		else
			output[0].stringValue[j] = string[index];
	}
	output[0].dimensions[0] = max;
	return true;
}

CharAt_Component::CharAt_Component()
	: Component(L"CharAt", L"Get Char At", 2, 1)
{
	setLeftLabels(2, "Array", "Index");
	output[0].stringType = true;
}


int CharIndex_Component::method2(Sheet* pg) {

	// need all inputs
	for (int j = 0; j < 2; j++) {
		if (input[j].drivenByComponent == -1) {
			return clearOutputs();
		}
	}

	int stringSize = input[0].stringValue.size(); // todo make this block a function
	int keySize = input[1].stringValue.size();

	int max = max(stringSize, keySize);
	if (((stringSize != max) && (stringSize != 1)) ||
		((keySize != max) && (keySize != 1)))
		return clearOutputs();

	output[0].value.resize(max);

	std::string key;
	std::string string;
	for (int j = 0; j < max; j++) {

		if (stringSize == 1)
			string = input[0].stringValue[0];
		else
			string = input[0].stringValue[j];

		if (keySize == 1)
			key = input[1].stringValue[0];
		else
			key = input[1].stringValue[j];

		output[0].value[j] = string.find(key);
	}
	output[0].dimensions[0] = max;
	return true;
}


CharIndex_Component::CharIndex_Component()
	: Component(L"CharIndex", L"Find Char Index", 2, 1)
{
	setLeftLabels(2, "Array", "Key");
}


// ******************************* Knob ************************************


void commonKnobDial(Sheet* pg, Component *comp) {
	// draw the track
	int knobSize = comp->setting[2];
	int radius = knobSize - TERMINAL_SIZE;
	BeginPath(pg->hDC);
	Ellipse(pg->hDC, pg->cXo + comp->x - radius, pg->cYo + comp->y - radius,
		            pg->cXo + comp->x + radius, pg->cYo + comp->y + radius);
	EndPath(pg->hDC);
	StrokePath(pg->hDC);

	double revolution = comp->setting[1];
	int xx = pg->cXo + comp->x;
	int yy = pg->cYo + comp->y;
	double angle = comp->setting[0] / revolution;
	angle *= TWO_PI;

	radius = knobSize - TERMINAL_SIZE - 1;
	int knobX = xx + radius * sin(angle);
	int knobY = yy - radius * cos(angle);

	// write the value
	std::string text = tomString(comp->detent(3, 0));
	int originalDecimalPoint = text.find_first_of(".");
	// ctx.fillText( originalDecimalPoint.tomString(), 100, 100 );
	int k0 = text.size();
	int k = k0;
	while (calcStringWidthFont(pg->hDC, text.substr(0, k)) > 2 * comp->width - 25)
		k--;
	text = text.substr(0, k);
	int textWidth = calcStringWidthFont(pg->hDC, text);
	if ((originalDecimalPoint == -1) && (k != k0))
		drawTextString(pg->hDC, pg->cXo + comp->x - textWidth /2, pg->cYo + comp->y - 8, 0, "****");
	else
		drawTextString(pg->hDC, pg->cXo + comp->x - textWidth /2, pg->cYo + comp->y - 8, 0, text);

	// then the knob
	BeginPath(pg->hDC);
	Ellipse(pg->hDC, knobX - TERMINAL_SIZE, knobY - TERMINAL_SIZE,
		            knobX + TERMINAL_SIZE, knobY + TERMINAL_SIZE);
	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
	EndPath(pg->hDC);
	StrokeAndFillPath(pg->hDC);
}


int Knob_Component::method2(Sheet* pg) {
	// this is the Knob constant - send setting out
	// size upgrade
	
	width = setting[2];
	height = setting[2];

	output[0].value[0] = detent(3, 0);
	return true;
}

bool commonCrankKnob(Sheet* pg, Component *comp, int ax, int ay, bool check) {
	int knobSize = comp->setting[2];
	double revolution = comp->setting[1];     // value changed in one revolution
	int xx = comp->x;
	int yy = comp->y;
	double oldAngle = comp->setting[0] / revolution;  // value of output
	oldAngle *= TWO_PI;

	if (check) {
		int radius = knobSize - TERMINAL_SIZE - 1;
		int knobX = xx + radius * sin(oldAngle);
		int knobY = yy - radius * cos(oldAngle);

		if (ax > knobX + 2 * TERMINAL_SIZE)
			return false; // missed knob
		if (ax < knobX - 2 * TERMINAL_SIZE)
			return false; // missed knob
		if (ay > knobY + 2 * TERMINAL_SIZE)
			return false; // missed knob
		if (ay < knobY - 2 * TERMINAL_SIZE)
			return false; // missed knob
	}
	double dx = ax - xx;
	double dy = ay - yy;
	double newAngle = atan2(dy, dx) + PI / 2.0;

	double deltaAngle = newAngle - oldAngle;

	while (deltaAngle > PI)
		deltaAngle -= TWO_PI;
	while (deltaAngle < -PI)
		deltaAngle += TWO_PI;

	double deltaFraction = deltaAngle / TWO_PI;
	deltaFraction *= revolution;

	comp->setting[0] += deltaFraction;
	comp->setting[0] = floor(comp->setting[0] * 100 / revolution + 0.5)
		/ (100 / revolution);

	// move the dial
	comp->dial(pg);

	return true;
}


bool Knob_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankKnob(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}

void Knob_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = doubleValues[0]; // value
	setting[1] = doubleValues[1]; // revolution

	if ((doubleValues[2] >= 10) && (doubleValues[2] <= 200))
		setting[2] = doubleValues[2];// size

	if (doubleValues[3] >= 0)
		setting[3] = doubleValues[3];// detent

	onLoad(pg);
}

void doubleClickKnob(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Knob Value:", "Revolution Value:", "Knob Size:", "Detent Value:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[1],
	pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

std::vector<Button*>knobButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickKnob) };

void Knob_Component::dial(Sheet* pg) {
	commonKnobDial(pg, this);
}
Knob_Component::Knob_Component()
	: Component(L"Knob", L"Knob", roundShape, 0, 0, 1, 1)
{
	dialogButtonList = knobButtonList;
	output[0].dimensions[0] = 1;
	setSetting(0, 0.0);
	setSetting(1, 1.0);      // amount per revolution.
	setSetting(2, 30);
	setSetting(3, 0);  // detent, 0 = disable
	height = COMPONENT_HEIGHT; // added because it was original size
}

// factor

int KnobFactor_Component::method2(Sheet* pg) {
	output[0].value.resize(input[0].value.size());
	copyDimensions(0, 0);

	// this is the Knob factor - send setting out
	for (int j = 0; j < input[0].value.size(); j++)
		output[0].value[j] = detent(3, 0) * input[0].value[j];
	
	width = setting[2];
	height = setting[2];
	return true;
}

bool KnobFactor_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankKnob(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
void KnobFactor_Component::dial(Sheet* pg) {
	commonKnobDial(pg, this);
}
void KnobFactor_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = doubleValues[0]; // value
	setting[1] = doubleValues[1]; // revolution

	if ((doubleValues[2] >= 10) && (doubleValues[2] <= 200))
		setting[2] = doubleValues[2];// size

	if (doubleValues[3] >= 0)
		setting[3] = doubleValues[3];// detent

	onLoad(pg);
}
KnobFactor_Component::KnobFactor_Component()
	: Component(L"KnobFac", L"Knob Factor", roundShape, 1, 1, 1, 1)
{
	dialogButtonList = knobButtonList;
	setSetting(0, 0.0);
	setSetting(1, 1.0);      // amount per revolution.
	setSetting(2, 30);
	setSetting(3, 0);  // detent, 0 = disable
	height = COMPONENT_HEIGHT; // added because it was original size
}

// offset

int KnobOffset_Component::method2(Sheet* pg) {
	output[0].value.resize(input[0].value.size());
	copyDimensions(0, 0);

	// this is the Knob offset - send setting out
	for (int j = 0; j < input[0].value.size(); j++)
		output[0].value[j] = detent(3, 0) + input[0].value[j];
	
	width = setting[2];
	height = setting[2];
	return true;
}


bool KnobOffset_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!commonCrankKnob(pg, this, ax, ay, check))
		return false;

	return method2(pg);
}
void KnobOffset_Component::dial(Sheet* pg) {
	commonKnobDial(pg, this);
}
void KnobOffset_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = doubleValues[0]; // value
	setting[1] = doubleValues[1]; // revolution

	if ((doubleValues[2] >= 10) && (doubleValues[2] <= 200))
		setting[2] = doubleValues[2];// size

	if (doubleValues[3] >= 0)
		setting[3] = doubleValues[3];// detent

	onLoad(pg);
}
KnobOffset_Component::KnobOffset_Component()
	: Component(L"KnobOff", L"Knob Offset", roundShape, 1, 1, 1, 1)
{
	dialogButtonList = knobButtonList;
	setSetting(0, 0.0);
	setSetting(1, 1.0);      // amount per revolution.
	setSetting(2, 30);
	setSetting(3, 0);  // detent, 0 = disable
	height = COMPONENT_HEIGHT; // added because it was original size
}

// ***************************** meter ***************************************

COLORREF penColors[] = { RGB(0,0,0),  RGB(255,0,0), RGB(0,255,0), RGB(0,0,255), RGB(128,128,0), RGB(0,128,128), RGB(128,0,128) };
int maxPenColors = 7;


// setting[0] = x size
	// setting[1] = y size
	// setting[2] = min value
	// setting[3] = max value
void Meter_Component::dial(Sheet* pg) {

	// check for debugging a display
	//pg = getDebugInstancepg(pg, i);

	double SWEEP_ANGLE = 1.5; // 100 deg
	int count = 0;
	if (input[0].drivenByComponent != -1) {
		count = input[0].value.size();
	}

	int size = min(height, width);
	int frame = size - 10;
	int pointer = size;
	int yOffset = 0.6 * frame;

	int xc = pg->cXo + x;
	int yc = pg->cYo + y;

	// white Background
	BeginPath(pg->hDC);
	Rectangle(pg->hDC, xc - frame, yc - frame, xc + frame, yc + frame);
	EndPath(pg->hDC);
	HBRUSH fillColor = GetColorBrush(0xFFFFFF); // CreateSolidBrush
	SelectObject(pg->hDC, fillColor);
	FillPath(pg->hDC);
	//DeleteObject(fillColor);

	drawResizeBox(pg, this);

	//ctx.fillStyle = 0x000000;

	// draw the arc
	//ctx.strokeStyle = 0xAAAAAA;
	int yco = yc + yOffset;
	
	int arcEndX = xc + pointer * sin(SWEEP_ANGLE / 2.0);
	int arcEndY = yco - pointer * cos(SWEEP_ANGLE / 2.0);
	int arcBeginX = xc + pointer * sin(-SWEEP_ANGLE/2.0);
	int arcBeginY = yco - pointer * cos(-SWEEP_ANGLE / 2.0);

	BeginPath(pg->hDC);
	Arc(pg->hDC, xc - pointer, yco - pointer, xc + pointer, yco + pointer,
		arcEndX, arcEndY, arcBeginX, arcBeginY);
	EndPath(pg->hDC);
	StrokePath(pg->hDC);

	// draw some tics
	int ticCount = 20;
	double step = SWEEP_ANGLE / ticCount;
	double minor = pointer * 1.06;
	double tic = pointer * 1.1;
	double label = pointer * 1.16;
	int labelIndex = 0;
	int range = setting[3] - setting[2];
	if (range <= 0)
		return;

	double labels = size / 18.0;
	double xTip;
	double yTip;
	double xBase;
	double yBase;
	if ((setting[3] > 0) && (setting[2] < 0)) {
		// start from zero and work UP
		int skip = floor(ticCount / labels);
		double zeroAngle = -setting[3] / range * SWEEP_ANGLE + SWEEP_ANGLE / 2;
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
			
			BeginPath(pg->hDC);
			MoveToEx(pg->hDC, xc + xBase, yco + yBase, NULL);
			LineTo(pg->hDC, xc + xTip, yco + yTip);
			EndPath(pg->hDC);
			StrokePath(pg->hDC);

			// label some tics
			if (labelIndex % skip == 0) {
				xTip = label * sin(angle);
				yTip = -label * cos(angle);

				std::string text = tomString(labelIndex / ticCount * range);
				if (text.size() > 5)
					text = tomString(round((labelIndex / ticCount * range) * 1000) / 1000);
				int wid = calcStringWidthFont(pg->hDC, text);
				drawTextString(pg->hDC, xc + xTip-wid/2, yco + yTip - 6, 0, text);
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

			BeginPath(pg->hDC);
			MoveToEx(pg->hDC, xc + xBase, yco + yBase, NULL);
			LineTo(pg->hDC, xc + xTip, yco + yTip);
			EndPath(pg->hDC);
			StrokePath(pg->hDC);

			// label some tics
			if (labelIndex % skip == 0) {
				xTip = label * sin(angle);
				yTip = -label * cos(angle);

				std::string text = tomString(labelIndex / ticCount * range);
				if (text.size() > 5)
					text = tomString(round((labelIndex / ticCount * range) * 1000) / 1000);
				int wid = calcStringWidthFont(pg->hDC, text);
				drawTextString(pg->hDC, xc + xTip-wid/2, yco + yTip - 6, 0, text);
			}
			labelIndex--;
		}
	}
	else {
		int skip = floor(ticCount / labels);
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

			BeginPath(pg->hDC);
			MoveToEx(pg->hDC, xc + xBase, yco + yBase, NULL);
			LineTo(pg->hDC, xc + xTip, yco + yTip);
			EndPath(pg->hDC);
			StrokePath(pg->hDC);

			// label some tics
			if (labelIndex % skip == 0) {
				xTip = label * sin(angle);
				yTip = -label * cos(angle);

				std::string text = tomString(labelIndex * range / ticCount + setting[2]);
				if (text.size() > 5)
					text = tomString(round((labelIndex / ticCount * range + setting[2]) * 1000) / 1000);
				int wid = calcStringWidthFont(pg->hDC, text);
				drawTextString(pg->hDC, xc + xTip - wid / 2, yco + yTip - 6, 0, text);
			}
			labelIndex++;
		}
	}

	// draw the pointers for each input value
	for (int j = 0; j < count; j++) {
		//if (input[0].value[j] == null)
		//	continue;

		double value = input[0].value[j];
		if (value < setting[2])
			value = setting[2];
		if (value > setting[3])
			value = setting[3];

		double angle = (value - setting[2]) / range * SWEEP_ANGLE - SWEEP_ANGLE / 2;
		xTip = pointer * sin(angle);
		yTip = -pointer * cos(angle);

		BeginPath(pg->hDC);
		MoveToEx(pg->hDC, xc, yco, NULL);
		LineTo(pg->hDC, xc + xTip, yco + yTip);
		EndPath(pg->hDC);

		HBRUSH frameColor = GetColorBrush(penColors[j % maxPenColors]); // CreateSolidBrush
		SelectObject(pg->hDC, frameColor);
		StrokePath(pg->hDC);
		//DeleteObject(frameColor);
	}

}


bool Meter_Component::crank(Sheet* pg, int ax, int ay, bool check) {

	if (!crankResizeHoldLeft(ax, ay, check))
		return false;

	if (height < 40)
		height = 40;
	if (width < 40)
		width = 40;

	setting[0] = width;
	setting[1] = height;

	// because the input terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//  drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}


void Meter_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[2] = doubleValues[0]; // minimum
	setting[3] = doubleValues[1]; // maximum

	onLoad(pg);
}
void doubleClickMeter(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Minimum:", "Maximum:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

std::vector<Button*>meterButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickMeter) };

bool Meter_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];
	return true;
}
Meter_Component::Meter_Component()
	: Component(L"Meter", L"Meter", squareShape, 1, 1, 0, 0)
{
	dialogButtonList = meterButtonList;
	//hover = hoverMeter;
	setSize(60, 60);
	setSetting(0, 60);     // x size
	setSetting(1, 60);     // y size
	setSetting(2, 0);      // min value
	setSetting(3, 100);    // max value
}

//  ************************ Rotate ******************************

int Rotate_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	
	for (int k = 0; k < 2; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int j = 0; j < loops; j++) {
		double centerX = getLoopingValueD(0, j, 0);
		double centerY = getLoopingValueD(1, j, 0);
		double pointX = getLoopingValueD(2, j, 0);
		double pointY = getLoopingValueD(3, j, 0);
		double angle = getLoopingValueD(4, j, 0);
		angle = rotateInputAngle(angle);

		double sinang = sin(angle);
		double cosang = cos(angle);

		output[0].value[j] = (pointX - centerX) * cosang - (pointY - centerY) * sinang + centerX;
		output[1].value[j] = (pointX - centerX) * sinang + (pointY - centerY) * cosang + centerY;
	}

	return true;
}
void rotationActionRotate(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildRotationDialogButtons(pg, i);
}
std::vector<Button*>rotateButtonList = {
	new Button(3, 40, 85, 20, L"Rotation...", rotationActionRotate) };

Rotate_Component::Rotate_Component()
	: Component(L"Rotate", L"XY Rotate", squareShape, 5, 1, 2, 1)
{
	dialogButtonList = rotateButtonList;
	setLeftLabels(3, "Cx", "Cx", "Px", "Py", "Angle");
	setRightLabels(3, "Px", "Py");
}

// ****************************** to polar ********************************

int ToPolar_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 2; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int j = 0; j < loops; j++) {
		double x0 = getLoopingValueD(0, j, 0);
		double y0 = getLoopingValueD(1, j, 0);
		double x1 = getLoopingValueD(2, j, 0);
		double y1 = getLoopingValueD(3, j, 0);
		double dx = x1 - x0;
		double dy = y1 - y0;
		double rad = sqrt(dx * dx + dy * dy);
		output[0].value[j] = rad;
		double angle = atan2(dx, dy);
		angle = rotateInputAngle(angle, true, false);

		output[1].value[j] = angle;
	}
	return true;
}

std::vector<Button*>toPolarButtonList = {
	new Button(3, 40, 85, 20, L"Rotation...", rotationActionGeneral) };

ToPolar_Component::ToPolar_Component()
	: Component(L"ToPolar", L"XY to Polar", squareShape, 4, 1, 2, 1)
{
	dialogButtonList = toPolarButtonList;
	setLeftLabels(3, "X0", "Y0", "X1", "Y1");
	setRightLabels(3, "Radius", "Angle");
}

// ******************************** Polar to XY ***************************

int ToCart_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 4; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int j = 0; j < loops; j++) {
		double x = getLoopingValueD(0, j, 0);
		double y = getLoopingValueD(1, j, 0);
		double rad = getLoopingValueD(2, j, 0);
		double ang = getLoopingValueD(3, j, 0);
		ang = rotateOutputAngle(ang, true, true);

		double dx = rad * sin(ang);
		double dy = rad * cos(ang);

		output[0].value[j] = x;
		output[1].value[j] = y;
		output[2].value[j] = x + dx;
		output[3].value[j] = y + dy;
	}
	return true;
}

std::vector<Button*>tocartButtonList = {
	new Button(3, 40, 85, 20, L"Rotation...", rotationActionGeneral) };

ToCart_Component::ToCart_Component()
	: Component(L"ToCart", L"Polar to XY", squareShape, 4, 1, 4, 1)
{
	dialogButtonList = tocartButtonList;
	setLeftLabels(3, "X", "Y", "Radius", "Angle");
	setRightLabels(3, "X0", "Y0", "X1", "Y1");
}



int Canvas_Length_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 4; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {
		double x0 = getLoopingValueD(0, k, 0);
		double y0 = getLoopingValueD(1, k, 0);
		double x1 = getLoopingValueD(2, k, 0);
		double y1 = getLoopingValueD(3, k, 0);

		double newLength = getLoopingValueD(4, k, 1); // defaults to the unit vector
		double dx = x1 - x0;
		double dy = y1 - y0;
		double oldLength = sqrt(dx * dx + dy * dy);
		double factor = newLength / oldLength;

		output[0].value[k] = x0;
		output[1].value[k] = y0;
		output[2].value[k] = x0 + factor * (x1 - x0);
		output[3].value[k] = y0 + factor * (y1 - y0);
	}
	return true;
}
Canvas_Length_Component::Canvas_Length_Component()
	: Component(L"2dLength", L"2D Length", squareShape, 5, 1, 4, 1)
{
	setLeftLabels(3, "X0", "Y0", "X1", "Y1", "Length");
	setRightLabels(3, "X0", "Y0", "X1", "Y1");
	setSetting(0, 2);  // dimensions (for later)
}

int Canvas_Scale_Component::method2(Sheet* pg) {
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 4; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {
		double x0 = getLoopingValueD(0, k, 0);
		double y0 = getLoopingValueD(1, k, 0);
		double x1 = getLoopingValueD(2, k, 0);
		double y1 = getLoopingValueD(3, k, 0);

		double scale = getLoopingValueD(4, k, 1); // defaults to the full length

		output[0].value[k] = x0;
		output[1].value[k] = y0;
		output[2].value[k] = x0 + scale * (x1 - x0);
		output[3].value[k] = y0 + scale * (y1 - y0);
	}
	return true;
}
Canvas_Scale_Component::Canvas_Scale_Component()
	: Component(L"2dScale", L"2D Scale", squareShape, 5, 1, 4, 1)
{
	setLeftLabels(3, "X0", "Y0", "X1", "Y1", "Factor");
	setRightLabels(3, "X0", "Y0", "X1", "Y1");
	setSetting(0, 2);  // dimensions (for later)
}



int Canvas_Delta_Component::method2(Sheet* pg) {
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();

	int mode = setting[1]; // mode: 0-hide, 1-show main, 2-show components, 3-show main and components
	//for (int k = 0; k < 4; k++)
	//	output[k].value.resize(mode); // really?

	for (int k = 0; k < 4; k++) {
		output[k].value.resize(loops*mode);
		
		if (mode == 1) {
			output[k].dimensions.resize(1);
			output[k].dimensions[0] = loops;
		}
		if (mode == 2) {
			output[k].dimensions.resize(2);
			output[k].dimensions[0] = 2;
			output[k].dimensions[1] = loops;
		}
		if (mode == 3) {
			output[k].dimensions.resize(2);
			output[k].dimensions[0] = 3;
			output[k].dimensions[1] = loops;
		}
	}

	int j = 0;
	for (int k = 0; k < loops; k++) {
		double x = getLoopingValueD(0, k, 0);
		double y = getLoopingValueD(1, k, 0);
		double dx = getLoopingValueD(2, k, 0);
		double dy = getLoopingValueD(3, k, 0);

		if (mode & 1) {
			// main vector
			output[0].value[j] = x;
			output[1].value[j] = y;
			output[2].value[j] = x + dx;
			output[3].value[j] = y + dy;
			j++;
		}

		if (mode & 2) {
			// X component vector
			output[0].value[j] = x;
			output[1].value[j] = y;
			output[2].value[j] = x + dx;
			output[3].value[j] = y;
			j++;
			// Y component vector
			output[0].value[j] = x;
			output[1].value[j] = y;
			output[2].value[j] = x;
			output[3].value[j] = y + dy;
			j++;
		}
	}

	if (loops == 1) {
		for (int k = 0; k < 4; k++) {
			output[k].dimensions.resize(1);
			output[k].dimensions[0] = mode;
		}
	}
	if (loops > 1) {
		for (int k = 0; k < 4; k++) {
			output[k].dimensions.resize(2);
			output[k].dimensions[0] = mode;
			output[k].dimensions[1] = loops;
		}
	}
	return true;
}
void Canvas_Delta_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	setting[1] = radioValue;
}
void doubleClick2dDelta(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Show Main Vector", "Show Components", "Show All Vectors" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>the2dDeltaButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClick2dDelta) };

Canvas_Delta_Component::Canvas_Delta_Component()
	: Component(L"2dDelta", L"2D Delta", squareShape, 4, 1, 4, 1)
{
	dialogButtonList = the2dDeltaButtonList;
	setLeftLabels(3, "X", "Y", "dX", "dY");
	setRightLabels(3, "X0", "Y0", "X1", "Y1");
	setSetting(0, 2);  // dimensions (for later)
	setSetting(1, 1);  // mode: 0-hide, 1-show main, 2-show Components, 3-show main and Components
	completionTestRadios = 3;
}



int Center2Edge_Component::method2(Sheet* pg) {
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 4; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {
		double x = getLoopingValueD(0, k, 0);
		double y = getLoopingValueD(1, k, 0);
		double dx = getLoopingValueD(2, k, 0);
		double dy = getLoopingValueD(3, k, 0);

		output[0].value[k] = x - dx / 2; // left
		output[1].value[k] = y + dy / 2; // top
		output[2].value[k] = x + dx / 2; // right
		output[3].value[k] = y - dy / 2; // bottom
	}

	return true;
}
Center2Edge_Component::Center2Edge_Component()
	: Component(L"Center2Edge", L"Center to Edge", squareShape, 4, 1, 4, 1)
{
	setLeftLabels(3, "X", "Y", "dX", "dY");
	setRightLabels(3, "Left", "Top", "Right", "Bottom");
	setSetting(0, 2);  // dimensions (for later)
}



int Edge2Center_Component::method2(Sheet* pg) {
	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 4; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {
		double left = getLoopingValueD(0, k, 0);
		double top = getLoopingValueD(1, k, 0);
		double right = getLoopingValueD(2, k, 0);
		double bottom = getLoopingValueD(3, k, 0);

		output[0].value[k] = (left + right) / 2;
		output[1].value[k] = (top + bottom) / 2;
		output[2].value[k] = right - left;
		output[3].value[k] = top - bottom;
	}

	return true;
}
Edge2Center_Component::Edge2Center_Component()
	: Component(L"Edge2Center", L"Edge to Center", squareShape, 4, 1, 4, 1)
{
	setLeftLabels(3, "Left", "Top", "Right", "Bottom");
	setRightLabels(3, "X", "Y", "dX", "dY");
	setSetting(0, 2);  // dimensions (for later)
}




int XyzToLl_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 3; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {
		double xx = getLoopingValueD(0, k, 0);
		double yy = getLoopingValueD(1, k, 0);
		double zz = getLoopingValueD(2, k, 0);

		double radius = sqrt(xx * xx + yy * yy + zz * zz);
		double latitude = asin(yy / radius);
		double longitude = atan2(xx, zz);

		output[0].value[k] = radius;
		output[1].value[k] = latitude * RADIANS_TO_DEGREES;
		output[2].value[k] = longitude * RADIANS_TO_DEGREES;
	}
	return true;
}
XyzToLl_Component::XyzToLl_Component()
	: Component(L"XyzToLl", L"XYZ to LatLon", squareShape, 3, 1, 3, 1)
{
	setLeftLabels(3, "X", "Y", "Z");
	setRightLabels(3, "Radius", "Latitude (Deg)", "Longitude (Deg)");
	width = 50;
}




int LlToXyz_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	for (int k = 0; k < 3; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {
		double radius = getLoopingValueD(0, k, 0);
		double latitude = getLoopingValueD(1, k, 0);
		double longitude = getLoopingValueD(2, k, 0);

		latitude *= DEGREES_TO_RADIANS;
		latitude = max(-PI_OVER_2, min(PI_OVER_2, latitude));
		longitude *= DEGREES_TO_RADIANS;

		double xx = radius * cos(latitude) * sin(longitude);
		double yy = radius * sin(latitude);
		double zz = radius * cos(latitude) * cos(longitude);

		output[0].value[k] = xx;
		output[1].value[k] = yy;
		output[2].value[k] = zz;
	}
	return true;
}
LlToXyz_Component::LlToXyz_Component()
	: Component(L"LlToXyz", L"LatLon to XYZ", squareShape, 3, 1, 3, 1)
{
	setLeftLabels(3, "Radius", "Latitude (Deg)", "Longitude (Deg)");
	setRightLabels(3, "X", "Y", "Z");
	width = 50;
}

// ********************************** Bus In ************************************************


void doubleClickBussIn(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildBussInDialogButtons(pg, i);
}


void actionReplaceWiresWithBuses(Sheet* pg, int i) {
	
	// get the coordinates and name of the bus
	int busX = pg->dgrm.cL[i]->leftTerminalX(0);
	int busY = pg->dgrm.cL[i]->leftTerminalY(0);
	pg->lastBussName = pg->dgrm.cL[i]->literal[0];

	// search for wires connected to the bus terminal
	vector<int> replacedWires;
	for (int iw = 0; iw < pg->dgrm.wL.size(); iw++) {
		int wireX;
		int wireY;
		// check zero end of wire
		if ((abs(pg->dgrm.wL[iw]->x[0] - busX) < TERMINAL_SIZE) &&
			(abs(pg->dgrm.wL[iw]->y[0] - busY) < TERMINAL_SIZE)) {
			wireX = pg->dgrm.wL[iw]->x[1];
			wireY = pg->dgrm.wL[iw]->y[1];
		}
		else // check one end of wire
			if ((abs(pg->dgrm.wL[iw]->x[1] - busX) < TERMINAL_SIZE) &&
				(abs(pg->dgrm.wL[iw]->y[1] - busY) < TERMINAL_SIZE)) {
				wireX = pg->dgrm.wL[iw]->x[0];
				wireY = pg->dgrm.wL[iw]->y[0];
			}
			else
				continue;

		POS pos;
		wireX -= 37; // based on the width of the bus out and terminal size
		pos.x = (wireX + pg->cXo) * pg->cs;
		pos.y = (wireY + pg->cYo) * pg->cs;
		AddComponent(pg, L"BussOut", pos, pg->visibleSheet);
		pg->draggingComponent = -1;

		replacedWires.push_back(iw);
	}

	int count = replacedWires.size();
	for (int j = count-1; j >= 0; j--)
		deleteWire(pg, replacedWires[j]);

	pg->activeDialogComponent = -1;
}
void actionReplaceBusesWithWires(Sheet* pg, int i) {

	// get the coordinates and name of the Bus In
	int busX = pg->dgrm.cL[i]->leftTerminalX(0);
	int busY = pg->dgrm.cL[i]->leftTerminalY(0);
	string bussName = pg->dgrm.cL[i]->literal[0];

	// search for Bus Outs with the same name
	vector<int> replacedComponents;
	for (int j = 0; j < pg->dgrm.cL.size(); j++) {
		if (j == i) { // the bus in needs to be deleted also
			replacedComponents.push_back(j);
			continue;
		}

		if (!z(pg->dgrm.cL[j]->type, L"BussOut"))
			continue;
		if (bussName != pg->dgrm.cL[j]->literal[0])
			continue;

		int endX = pg->dgrm.cL[j]->rightTerminalX(0);
		int endY = pg->dgrm.cL[j]->rightTerminalY(0);
		AddWire(pg, busX, busY, endX, endY);

		replacedComponents.push_back(j);
	}

	int count = replacedComponents.size();
	for (int j = count - 1; j >= 0; j--)
		deleteComponent(pg, replacedComponents[j]);

	pg->activeDialogComponent = -1;
}
std::vector<Button*>bussInButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClickBussIn),
	new Button(-117, 60, 205, 20, L"Replace Wires with Bus Outs", actionReplaceWiresWithBuses),
	new Button(-117, 80, 205, 20, L"Revert Buses to Wires", actionReplaceBusesWithWires) };

void BusIn_Component::dial(Sheet* pg) {
	// label it
	int textWidth = calcStringWidthFont(pg->hDC, literal[0]);
	drawTextString(pg->hDC, pg->cXo + x + 3 - textWidth / 2, pg->cYo + y - 6, 0, literal[0]);
}
bool BusIn_Component::onLoad(Sheet* pg) {
	width = 10 + calcStringWidthFont(pg->hDC, literal[0]) / 2;
	if (width < 30)
		width = 30;
	return true;
}

BusIn_Component::BusIn_Component()
	: Component(L"BussIn", L"Bus In", bussInShape, 1, 1, 0, 0)
{
	dialogButtonList = bussInButtonList;
	height = MIN_COMPONENT_HEIGHT;
	setLiteral(0, "");
}

// ******************* bus Out ************************


void doubleClickBussOut(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildBussOutDialogButtons(pg, i);
}

std::vector<Button*>bussOutButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickBussOut) };

void BusOut_Component::dial(Sheet* pg) {
	// label it
	int textWidth = calcStringWidthFont(pg->hDC, literal[0]);
	drawTextString(pg->hDC, pg->cXo + x - 3 - textWidth / 2, pg->cYo + y - 6, 0, literal[0]);
}
bool BusOut_Component::onLoad(Sheet* pg) {
	width = 10 + calcStringWidthFont(pg->hDC, literal[0]) / 2;
	if (width < 30)
		width = 30;
	return true;
}
BusOut_Component::BusOut_Component(string name)
	: Component(L"BussOut", L"Bus Out", bussOutShape, 0, 0, 1, 1)
{
	dialogButtonList = bussOutButtonList;
	height = MIN_COMPONENT_HEIGHT;
	setLiteral(0, name);
	//hover = hoverBussOut;
	setOutputDimension(0, 0, 1);
}

// ********************************** Bus 2 ************************************

void updateAllBuss2Outs(Sheet* pg) {

	// first make a list of the buss2Outs
	vector<int> busOutIndex;
	vector<bool> busOutProcessed;
	for (int k = 0; k < pg->dgrm.cL.size(); k++) {
		if (pg->dgrm.cL[k]->ghost)
			continue;
		if (pg->dgrm.cL[k]->type == L"Buss2Out") {
			busOutIndex.push_back(k);
			busOutProcessed.push_back(false);
		}
	}

	// scan each buss2In for matching outs
	for (int j = 0; j < pg->dgrm.cL.size(); j++) {
		
		if (pg->dgrm.cL[j]->type != L"Buss2In")
			continue;
		if (pg->dgrm.cL[j]->ghost)
			continue;

		// for this buss2In - find all the buss2Outs and update them
		for (int z = 0; z < busOutIndex.size(); z++) {
			if (busOutProcessed[z])
				continue;
			int k = busOutIndex[z];
			if (pg->dgrm.cL[k]->userLabel != pg->dgrm.cL[j]->userLabel)
				continue;
		
			int literals = pg->dgrm.cL[j]->literal.size();
			pg->dgrm.cL[k]->literal.resize(literals);
			for (int n = 0; n < literals; n++) {
				pg->dgrm.cL[k]->literal[n] = pg->dgrm.cL[j]->literal[n];
			}
			pg->dgrm.cL[k]->onLoad(pg);
			busOutProcessed[z] = true;
		}
	}

	// lastly check for any outs that were not updated
	for (int z = 0; z < busOutIndex.size(); z++) {
		if (busOutProcessed[z])
			continue;
		int k = busOutIndex[z];
		pg->dgrm.cL[k]->buildOutputPins(1);
		pg->dgrm.cL[k]->height = pinToHeight(1);
		pg->dgrm.cL[k]->width = 15;
		pg->dgrm.cL[k]->setting[0] = 15;
	}
}
int calcBuss2Width(Sheet* pg, Component *comp) {

	// need to calculate the width from the output pin labels
	int maxWidth = 20;
	for (int pin = 0; pin < comp->literal.size(); pin++) {
		int textWidth = calcStringWidthFont(pg->hDC, comp->literal[pin]);
		if (textWidth > maxWidth)
			maxWidth = textWidth;
	}
	return maxWidth/2 + 5;
}

void Bus2In_Component::dial(Sheet* pg)
{
	int xx = x - width - 1 - TERMINAL_SIZE;
	int yy = y - (TERMINAL_STEP * (leftTerminalCount - 1) * leftTerminalStep) / 2;
	for (int j = 0; j < leftTerminalCount; j++) {
		drawTextString(pg->hDC, pg->cXo + xx + 10, pg->cYo + yy - 6, 0, literal[j]);
		yy += TERMINAL_STEP * leftTerminalStep;
	}
}

void Bus2In_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	int oldCount = literal.size();
	int newCount = doubleValues[0];

	if ((newCount >= 1) && (newCount <= 32)) {
		if (newCount != oldCount) {
			literal.resize(newCount);
			for (int j = oldCount; j < newCount; j++)
				literal[j] = "";
			onLoad(pg);
		}
	}
	updateAllBuss2Outs(pg);
}
void ActionBuss2PinCount(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Pins:" };
	vector<double> doubleValues = { (double)pg->dgrm.cL[i]->literal.size() };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

void ActionBuss2PinNames(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildBuss2InPinNamesDialogButtons(pg, i);
}
std::vector<Button*>buss2InButtonList = { new Button(-80, 40, 125, 20, L"Set Pin Count...", ActionBuss2PinCount),
										  new Button(-80, 60, 125, 20, L"Set Pin Names...", ActionBuss2PinNames) };

bool Bus2In_Component::onLoad(Sheet* pg) {

	// convert literals to left labels
	int n = literal.size();
	buildInputPins(n);
	
	height = pinToHeight(n);
	width = calcBuss2Width(pg, this);
	setting[0] = width;

	return true;
}
Bus2In_Component::~Bus2In_Component() {

	// search for the matching buss2out
	for (int i = 0; i < page->dgrm.cL.size(); i++) {
		if (page->dgrm.cL[i] != NULL) {
			if (page->dgrm.cL[i]->type == L"Buss2Out") {
				if (page->dgrm.cL[i]->userLabel == userLabel) {
					page->dgrm.cL[i]->buildOutputPins(1);
					page->dgrm.cL[i]->height = pinToHeight(1);
					page->dgrm.cL[i]->width = 15;
					page->dgrm.cL[i]->setting[0] = 15;
				}
			}
		}
	}
}
Bus2In_Component::Bus2In_Component()
	: Component(L"Buss2In", L"Bus 2 In", buss2InShape, 1, 1, 0, 0)
{
	dialogButtonList = buss2InButtonList;
	setLiteral(0, "");
	width = 30;
	setSetting(0, width);
	completionTestDoubles = 1;
}

// **************************** buss 2 out *************************


int updateBuss2Output(Sheet* pg, Component* comp) {

	// search for the matching buss2in
	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		if (pg->dgrm.cL[i]->type == L"Buss2In") {
			if (pg->dgrm.cL[i]->userLabel == comp->userLabel) {
				int n = pg->dgrm.cL[i]->literal.size();
				comp->buildOutputPins(n);
				for (int j = 0; j < n; j++)
					comp->output[j].label = pg->dgrm.cL[i]->literal[j];
				return n;
			}
		}
	}
	return 0;
}

bool Bus2Out_Component::onLoad(Sheet* pg) {

	int n = updateBuss2Output(pg, this);

	height = pinToHeight(n);
	width = calcBuss2Width(pg, this);
	setting[0] = width;

	return true;
}
Bus2Out_Component::Bus2Out_Component()
	: Component(L"Buss2Out", L"Bus 2 Out", buss2OutShape, 0, 0, 1, 1)
{
	width = 30;
	setSetting(0, 10);
	setOutputDimension(0, 0, 1);
}

// ***************************** Value Terms 2 ****************************************


bool myfunction(double i, double j) { return (i < j); }

void numericalSort(vector<double>*list) {
	std::sort((*list).begin(), (*list).end(), myfunction);
}

double merge100X(vector<double>list) {
	if (list.size() > 8)
		list.resize(8); // makes a mess after that

	double output = 0;
	double factor = 1;
	for (int j = list.size() - 1; j >= 0; j--) {
		output += factor * list[j];
		factor *= 100;
	}
	return output;
}

vector<double> divide100(long value) {
	vector<double> list;
	while (value > 0) {
		long x = value % 100;
		value = floor(value / 100);
		list.push_back(x);
	}
	return list;
}
/*
bool methodValueTerms2_(Sheet* pg) {
	// output[0].value.size() = 0;

	output[0].value[0] = 0;
	output[1].value[0] = 0;    // no match so far
	output[2].stringValue[0] = "";   // string
	output[3].value[0] = 0;    // value
	setSimpleOutputDimensions();

	if (input[0].drivenByComponent == -1)    // terms array
		return true;
	int termsArraySize = input[0].value.size();

	int plusSymbol = input[2].value[0];
	int minusSymbol = input[3].value[0];
	int leftParenSymbol = input[4].value[0];
	int rightParenSymbol = input[5].value[0];


	vector<int> outerTempList;
	bool outerNegative = false;

	vector<int> outputList;

	vector<int> innerTempList;
	vector<int> inner100xList;
	vector<int> innerNegativeList;
	bool innerNegative = false;
	bool inParens = false;

	for (int j = 0; j < termsArraySize; j++) {
		int input = input[0].value[j];
		if (inParens) {
			
			if (input == plusSymbol) {
				if (innerTempList.size()) {
					numericalSort(innerTempList);
					var item = merge100X(innerTempList);
					inner100xList.push_back(item);
					innerNegativeList.push_back(innerNegative);
					innerTempList = [];
				}
				innerNegative = false;
			}
			else
			if (input == minusSymbol) {
				if (innerTempList.size()) {
					numericalSort(innerTempList);
					item = merge100X(innerTempList);
					inner100xList.push_back(item);
					innerNegativeList.push_back(innerNegative);
					innerTempList = [];
				}
				innerNegative = true;
			}
			else
			if (input == rightParenSymbol) {
				if (innerTempList.size()) {
					numericalSort(innerTempList);
					item = merge100X(innerTempList);
					inner100xList.push_back(item);
					innerNegativeList.push_back(innerNegative);
					innerTempList = [];
				}
				inParens = false;
			}
			else
				innerTempList.push_back(input);

		}
		else { // not in parens
			if ((input == plusSymbol) || (input == minusSymbol)) {
				if (outerTempList.size() || inner100xList.size()) {
					if (inner100xList.size()) {
						for (int k = 0; k < inner100xList.size(); k++) {
							var innerExpandedList = divide100(inner100xList[k]);
							for (int n = 0; n < outerTempList.size(); n++)
								innerExpandedList.push(outerTempList[n]);
							numericalSort(innerExpandedList);
							if (outerNegative != innerNegativeList[k])
								innerExpandedList.unshift(minusSymbol);
							item = merge100X(innerExpandedList);
							outputList.push_back(item);
						}
						inner100xList = [];
						innerTempList = [];
						innerNegativeList = [];
						innerNegative = false;
						outerTempList = [];
					}
					else {
						numericalSort(outerTempList);
						if (outerNegative)
							outerTempList.unshift(minusSymbol);
						item = merge100X(outerTempList);
						outputList.push_back(item);
						outerTempList = [];
					}
				}
			}
			
			if (input == plusSymbol) {
				outerNegative = false;
			}else
			if (input == minusSymbol) {
				outerNegative = true;
			} else
			if (input == leftParenSymbol) {
				inner100xList = [];
				innerTempList = [];
				innerNegativeList = [];
				innerNegative = false;
				inParens = true;
			} 
			else
				outerTempList.push_back(input);
				
		}
	}
	if (outerTempList.size() || inner100xList.size()) {
		if (inner100xList.size()) {
			for (int k = 0; k < inner100xList.size(); k++) {
				innerExpandedList = divide100(inner100xList[k]);
				for (int n = 0; n < outerTempList.size(); n++)
					innerExpandedList.push(outerTempList[n]);
				numericalSort(innerExpandedList);
				if (outerNegative != innerNegativeList[k])
					innerExpandedList.unshift(minusSymbol);
				item = merge100X(innerExpandedList);
				outputList.push_back(item);
			}
			//outerTempList = [];
		}
		else {
			numericalSort(outerTempList);
			if (outerNegative)
				outerTempList.unshift(minusSymbol);
			item = merge100X(outerTempList);
			outputList.push_back(item);
			//outerTempList = [];
		}
	}

	// the list of terms
	numericalSort(outputList);
	if (!isNaN(outputList[0])) {
		output[0].value.clear();
		for (int j = 0; j < outputList.size(); j++)
			output[0].value.push_back(outputList[j]);
	}

	// check for matching with goal
	if (outputList.size() == input[1].value.size()) {
		var goalList = [];
		for (j = 0; j < input[1].value.size(); j++)
			goalList[j] = input[1].value[j];
		numericalSort(goalList);
		int match = 1; // assumption
		for (j = 0; j < outputList.size(); j++)
			if (outputList[j] != goalList[j]) {
				match = 0;
				break;
			}
		output[1].value[0] = match;
	}

	// can we calculate the Value?
	bool useValues = false;
	if (input[6].drivenByComponent != -1)
		useValues = true;

	// output the result as a string
	std::string aString = "";
	for (int j = 0; j < outputList.size(); j++) {
		var list = divide100(outputList[j]);
		bool skipFirst = false;
		if (list[list.size() - 1] == minusSymbol) {
			aString += " - ";
			skipFirst = true;
		}
		else if (j > 0)
			aString += " + ";

		for (int k = 0; k < list.size(); k++) {
			if (skipFirst && k == 0)
				continue;
			if (!skipFirst && (k > 0))
				aString += " * ";
			if (skipFirst && (k > 1))
				aString += " * ";
			int n = list[list.size() - k - 1];
			if (useValues) {
				if (input[6].value[n] != undefined)
					aString += tomString(input[6].value[n]);
				else
					aString += "0";
			}
			else
				aString += n;
		}
	}
	output[2].value[0] = aString;

	var result = eval(aString);
	if (isNaN(result))
		output[3].value[0] = 0;
	else
		output[3].value[0] = result;

	setSimpleOutputDimensions();
	return true;
}

ValueTerms2_Component::ValueTerms2_Component()
	: Component(L"ValueTerms2", L"Value Terms 2", methodValueTerms2_, squareShape, 7, 1, 4, 1)
{
	setLeftLabels(3, "Terms", "Goa", "+", "-", "(", ")", "Values");
	setRightLabels(3, "Result", "Match", "String", "Value");
}
*/


// ********************************** value terms *********************************************

int ValueTerms_Component::method2(Sheet* pg) {
    output[0].value.clear();
	output[1].value[0] = 0; // no match
	setSimpleOutputDimensions();

	if (!checkValueInputs())
		return clearOutputs();

    if (input[0].drivenByComponent == -1)    // array
        return true;
    int arraySize = input[0].value.size();

	int plusSymbol = input[1].value[0];
	int minusSymbol = input[2].value[0];
	int leftParenSymbol = input[3].value[0];
	int rightParenSymbol = input[4].value[0];

    vector<double> outerTempList;
    bool outerNegative = false;

	vector<double> outputList;

	vector<double> innerTempList;
	vector<double> inner100xList;
	vector<bool> innerNegativeList;
    bool innerNegative = false;
    bool inParens = false;

    for (int j = 0; j < arraySize; j++) {
        int inputValue = input[0].value[j];
        if (inParens) {
			if (inputValue == plusSymbol) {
				if (innerTempList.size()) {
					numericalSort(&innerTempList);
					double item = merge100X(innerTempList);
					inner100xList.push_back(item);
					innerNegativeList.push_back(innerNegative);
					innerTempList.clear();
				}
				innerNegative = false;
			}else
			if (inputValue == minusSymbol) {
				if (innerTempList.size()) {
					numericalSort(&innerTempList);
					double item = merge100X(innerTempList);
					inner100xList.push_back(item);
					innerNegativeList.push_back(innerNegative);
					innerTempList.clear();
				}
				innerNegative = true;
			}else
			if (inputValue == rightParenSymbol) {
				if (innerTempList.size()) {
					numericalSort(&innerTempList);
					double item = merge100X(innerTempList);
					inner100xList.push_back(item);
					innerNegativeList.push_back(innerNegative);
					innerTempList.clear();
				}
				inParens = false;
			}else
                innerTempList.push_back(inputValue);
               
        } else { // not in parens
            if ((inputValue == plusSymbol) || (inputValue == minusSymbol)) {
                if (outerTempList.size() || inner100xList.size()) {
                    if (inner100xList.size()) {
                        for (int k = 0; k < inner100xList.size(); k++) {
                            vector<double> innerExpandedList = divide100(inner100xList[k]);
                            for (int n = 0; n < outerTempList.size(); n++)
                                innerExpandedList.push_back(outerTempList[n]);
                            numericalSort(&innerExpandedList);
                            if (outerNegative != innerNegativeList[k])
                                innerExpandedList.insert(innerExpandedList.begin(), minusSymbol);
							double item = merge100X(innerExpandedList);
                            outputList.push_back(item);
                        }
                        inner100xList.clear();
                        innerTempList.clear();
                        innerNegativeList.clear();
                        innerNegative = false;
                        outerTempList.clear();
                    } else {
                        numericalSort(&outerTempList);
                        if (outerNegative)
                            outerTempList.insert(outerTempList.begin(), minusSymbol);
						double item = merge100X(outerTempList);
                        outputList.push_back(item);
                        outerTempList.clear();
                    }
                }
            }
            if (inputValue == plusSymbol)
                outerNegative = false;
			else
			if (inputValue == minusSymbol)
				outerNegative = true;
			else
			if (inputValue == leftParenSymbol) {
				inner100xList.clear();
				innerTempList.clear();
				innerNegativeList.clear();
				innerNegative = false;
				inParens = true;
			}
			else
                outerTempList.push_back(inputValue);
        }
    }
    if (outerTempList.size() || inner100xList.size()) {
        if (inner100xList.size()) {
            for (int k = 0; k < inner100xList.size(); k++) {
                vector<double> innerExpandedList = divide100(inner100xList[k]);
                for (int n = 0; n < outerTempList.size(); n++)
                    innerExpandedList.push_back(outerTempList[n]);
                numericalSort(&innerExpandedList);
                if (outerNegative != innerNegativeList[k])
                    innerExpandedList.insert(innerExpandedList.begin(), minusSymbol);
				double item = merge100X(innerExpandedList);
                outputList.push_back(item);
            }
        } else {
            numericalSort(&outerTempList);
            if (outerNegative)
                outerTempList.insert(outerTempList.begin(), minusSymbol);
            double item = merge100X(outerTempList);
            outputList.push_back(item);
        }
    }

    numericalSort(&outputList);
    for (int j = 0; j < outputList.size(); j++)
        output[0].value.push_back(outputList[j]);

	// finally check goal for a match (output was set to false earlier)
	int len = input[5].value.size();
	if (output[0].value.size() == len) {
		bool match = true;
		for( int j = 0; j < len; j++ )
			if (input[5].value[j] != output[0].value[j]) {
				match = false;
				break;
			}
		output[1].value[0] = match;
	}
	
    setSimpleOutputDimensions();
    return true;
}

ValueTerms_Component::ValueTerms_Component()
	: Component(L"ValueTerms", L"Value Terms", squareShape, 6, 1, 2, 1)
{
	setLeftLabels(3, "Array", "+", "-", "(", ")", "Goal");
	setRightLabels(2, "Output", "Match");
}


// **************************** Circle Center ********************************************


double sqr( double a) {
	return a * a;
}

bool circle(double bx, double by, double cx, double cy, double dx, double dy, double *outX, double *outY) {
	double temp = sqr(cx) + sqr(cy);
	double bc = (sqr(bx) + sqr(by) - temp) / 2.0;
	double cd = (temp - sqr(dx) - sqr(dy)) / 2.0;
	double det = (bx - cx) * (cy - dy) - (cx - dx) * (by - cy);

	if (abs(det) < 1e-14)
		return false;

	*outX = (bc * (cy - dy) - cd * (by - cy)) / det;
	*outY = ((bx - cx) * cd - (cx - dx) * bc) / det;
	return true;
}


double dist(double ax, double ay, double bx, double by ) {
	return sqrt(sqr(ax - bx) + sqr(ay - by));
}

int CircleCenter_Component::method2(Sheet* pg) {
	output[0].value[0] = 0;
	output[1].value[0] = 0;
	output[2].value[0] = 0;

	output[0].dimensions[0] = 0;
	output[1].dimensions[0] = 0;
	output[2].dimensions[0] = 0;

	if (input[0].value.size() != 3) // x's
		return true;
	if (input[1].value.size() != 3) // y's
		return true;

	double x;
	double y;
	bool res = circle(
		input[0].value[0], input[1].value[0],
		input[0].value[1], input[1].value[1],
		input[0].value[2], input[1].value[2], 
		&x, &y);
	if (!res)
		return false;

	output[0].value[0] = x;
	output[1].value[0] = y;
	output[2].value[0] = dist(input[0].value[0], input[1].value[0], x, y);

	output[0].dimensions[0] = 1;
	output[1].dimensions[0] = 1;
	output[2].dimensions[0] = 1;
	return true;
}


CircleCenter_Component::CircleCenter_Component()
	: Component(L"Cc3pts", L"Circle Center", squareShape, 2, 1, 3, 1)
{
	setLeftLabels(3, "Xs", "Ys");
	setRightLabels(3, "Xc", "Yc", "Radius");
}


// ************************* Finite State Machine FSM *****************************************

void resetFSMsOnSheet_(Sheet* pg, Component *comp) { // TODO decide what RESET means to Sheet*s 1 to 20

	// first gather FSM group names on this Sheet*
	vector<string> group; // record the group names
	vector<int> initial; // record the component in the group with initial set
	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		if (comp->type != L"FSM")
			continue;
		bool found = false;
		int j;
		for (j = 0; j < group.size(); j++)
			if (group[j] == comp->literal[0]) {
				found = true;
				break;
			}

		if (!found) { // add it to the list
			group[group.size()] = comp->literal[0];
			//   initial[initial.size()] = -1;
			if (comp->setting[0]) { // is the initial
				initial[initial.size()] = i;
				comp->output[0].value[0] = 1;
			}
			else {
				initial[initial.size()] = -1;
				comp->output[0].value[0] = 0;
			}
		}
		else { // group already exists
			if (comp->setting[0]) { // initial is set
				if (initial[j] != -1) {// ut oh, cant be extra initials
					comp->setting[0] = 0; // clear it
					comp->output[0].value[0] = 0;
				}
				else {
					initial[j] = i; // set it
					comp->output[0].value[0] = 1;
				}
			}
			else {
				comp->output[0].value[0] = 0;
			}
		}
	}

	// Sheet*s may have FSMs also
	//for (int i = 0; i < pg->dgrm.cL.size(); i++)
	//	if (type == l"Sheet*") {
	//		resetFSMsOnSheet*_(pg);
	//		methodSheet*_(pg, i);
	//	}
}

int FSM_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	if (setting[1] == 0) // 0 = requires clock
		if (memory[0] == pg->systemTime)
			return false; // wait for next clock tick

	if (!input[0].value[0])
		return false; // input not true

	// clear all members in this group
	string groupName = literal[0];
	for (int j = 0; j < pg->dgrm.cL.size(); j++)
		if (pg->dgrm.cL[j]->type == L"FSM")
			if (pg->dgrm.cL[j]->literal[0] == groupName) {
				pg->dgrm.cL[j]->output[0].value[0] = 0;
				pg->dgrm.cL[j]->memory[0] = pg->systemTime;
			}

	// set this one's output true
	output[0].value[0] = 1;
	return true;
}

void FSM_Component::dial(Sheet* pg) {

	int ax = pg->cXo + x - width;
	int ay = pg->cYo + y - height;
	
	string text = trimTextToFit(pg->hDC, literal[0], 2 * width);
	drawTextString(pg->hDC, ax + 4, ay + 2, 0, text);
	if (setting[1] == 0)
		drawTextString(pg->hDC, ax + 16, ay + 15, 0, "clock" );

	// put black dot for initial state
	if (setting[0]) {
		BeginPath(pg->hDC);
		Ellipse(pg->hDC, ax + 8 - 5, ay + 22 - 5, ax + 8 + 5, ay + 22 + 5);
		EndPath(pg->hDC);
		SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
		StrokeAndFillPath(pg->hDC);
	}

	// show the output state
	BeginPath(pg->hDC);
	Ellipse(pg->hDC, ax + 52 - 5, ay + 22 - 5, ax + 52 + 5, ay + 22 + 5);
	EndPath(pg->hDC);
	if (output[0].value[0])
		SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
	else
		SelectObject(pg->hDC, GetStockObject(BLACK_BRUSH));
	StrokeAndFillPath(pg->hDC);

	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
}

//void buttonFSMSetInitialStateAction(int i) {
//	pg->dgrm.cL[i].setting[0] = 1;
//
//	// now clear all the others in this group
//	var groupName = pg->dgrm.cL[i].literal[0];
//	for (var j = 0; j < pg->dgrm.cL.size(); j++)
//		if (pg->dgrm.cL[j].type == 'FSM')
//			if (j != i)
//				if (pg->dgrm.cL[j].literal[0] == groupName)
//					pg->dgrm.cL[j].setting[0] = 0;
//
//	activeDialogButtonList = null;
//	drawCanvas();
//}

//void buttonFSMSetGroupNameAction(int i) {
//
//	var result = prompt("Enter Finite State group name:");
//	if (result) {
//		pg->dgrm.cL[i].literal[0] = result;
//
//		if (pg->dgrm.cL[i].setting[0]) { // this guy is Initial - clear the others in its new group
//			for (var j = 0; j < pg->dgrm.cL.size(); j++)
//				if (pg->dgrm.cL[j].type == 'FSM')
//					if (j != i)
//						if (pg->dgrm.cL[j].literal[0] == result)
//							pg->dgrm.cL[j].setting[0] = 0;
//		}
//
//		activeDialogButtonList = null;
//		propagateValues();
//		drawCanvas();
//		drawCanvas2();
//	}
//
//}

//void buttonFSMSetForceOnAction(int i) {
//	// clear all members in this group
//	var groupName = pg->dgrm.cL[i].literal[0];
//	for (var j = 0; j < pg->dgrm.cL.size(); j++)
//		if (pg->dgrm.cL[j].type == 'FSM')
//			if (pg->dgrm.cL[j].literal[0] == groupName) {
//				pg->dgrm.cL[j].output[0].value[0] = 0;
//				pg->dgrm.cL[j].memory[0] = systemTime;
//			}
//
//	// set this one's output true
//	pg->dgrm.cL[i].output[0].value[0] = 1;
//
//	activeDialogButtonList = null;
//	propagateValues();
//	drawCanvas();
//	drawCanvas2();
//}
//void buttonFSMSetNeedsClockAction(int i) {
//	pg->dgrm.cL[i].setting[1] = 0;
//	pg->dgrm.cL[i].timeBased = true;
//	activeDialogButtonList = null;
//	propagateValues();
//	drawCanvas();
//	drawCanvas2();
//}
//void buttonFSMSetNoClockAction(int i) {
//	pg->dgrm.cL[i].setting[1] = 1;
//	pg->dgrm.cL[i].timeBased = false;
//	activeDialogButtonList = null;
//	propagateValues();
//	drawCanvas();
//	drawCanvas2();
//}


void doubleClickFsm(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildFsmDialogButtons(pg, i);
}
std::vector<Button*> fsmButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickFsm) };

bool FSM_Component::onLoad(Sheet* pg) {
	if (setting[0]) // initial FSM
		output[0].value[0] = 1;
	//if (setting[1] == undefined)
	//	setting[1] = 0; // 0=needs clock to advance, 1=no clock needed
	return true;
}


FSM_Component::FSM_Component()
	: Component(L"FSM", L"Finite State", squareShape, 1, 0, 1, 0)
{
	dialogButtonList = fsmButtonList;
	timeBased = true;
	height = 15;
	setSetting(0, 0); // indicates this is initial state
	setSetting(1, 0); // 0 = requires clock, 1= no clock
	setSetting(2, 0); // 1 = requires edge someday?
	setMemory(0, 0);  // clock for synchronization between group members
	setLiteral(0, "");
	output[0].value[0] = 0;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
}



// *************************** chart *********************************

//#define maxPenColors 7
//COLORREF penColors[maxPenColors] = { 0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0x888800, 0x008888, 0x88008 };

double pickLabelSeparation(double scale) {
	double log10 = log(scale) / 2.3025850929;
	double frac = log10 - floor(log10);
	double range = 5.0;
	if (frac < 0.08) range = 1;
	else if (frac < 0.48) range = 2;
	double decade = floor(log10);
	return 1.0 / (pow(10.0, decade - 1.0) * range);
}


void Chart_Component::dial(Sheet* pg) {
	
	// display mode setting[3]: 0=lines, 1= dots, 2= lines and dots, 3 = X's
	int mode = setting[3];
	bool showLabels = setting[4];

	int wid = 2 * width - 20;
	int hgt = 2 * height - 20;

	int xOff = pg->cXo + x - width + 10;
	int yOff = pg->cYo + y - height + 10;

	// white Background
	BeginPath(pg->hDC);
	Rectangle(pg->hDC, xOff, yOff, xOff + wid, yOff + hgt);
	EndPath(pg->hDC);

	HBRUSH fillColor = GetColorBrush(0xFFFFFF);
	SelectObject(pg->hDC, fillColor);
	FillPath(pg->hDC);
		
	drawResizeBox(pg, this);

	int maxCount = 0;
	int curves = setting[2];

	for (int ci = 0; ci < curves; ci++) {
		if (input[ci].drivenByComponent != -1)
			if (input[ci].value.size() > maxCount)
				maxCount = input[ci].value.size();
	}
	if (maxCount < 2)
		return;

	int j;
	double min = 1e100;
	double max = -1e100;
	for (int ci = 0; ci < curves; ci++)
		if ((int)input[ci].drivenByComponent != -1)
			for (j = 0; j < input[ci].value.size(); j++) {
				if (input[ci].value[j] < min)
					min = input[ci].value[j];
				if (input[ci].value[j] > max)
					max = input[ci].value[j];
			}

	if (showLabels) {
		int labWidth = max(calcStringWidthFont(pg->hDC, tomString(min)),
						   calcStringWidthFont(pg->hDC, tomString(max)));
		wid -= labWidth + 4;
		hgt -= 12;
	}

	double xScale = wid / (maxCount - 1.0);
	double yScale = hgt / (max - min);

	COLORREF strokeBlueStyle = RGB(230, 230, 255);
	COLORREF strokeBlackStyle = RGB(170, 170, 200);
	COLORREF fillWhiteStyle = RGB(255, 255, 255);
	COLORREF strokeStyle = strokeBlueStyle;
	COLORREF colorCurveStyle;
	
	// draw the vertical axis grid - horizontal lines
	// pick nice round number for tic
	double label = pickLabelSeparation(yScale);
	int labelCounter;
	if (label > 0) {
		int start = floor(min / label) * label;
		if (start >= 0)
			labelCounter = (int)(10 - (label * 10 - start) / label) % 10; //  because the data doesnt start at zero
		else
			labelCounter = 10 + ((int)((start - label * 10) / label) % 10);

		if (start + label != start) { // check to see if we will be infinite looping
			for (double lv = start; lv <= max; lv += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC, GetColorPen(strokeBlackStyle, 1));
				int y = (max - lv) * yScale + yOff;

				BeginPath(pg->hDC);
				MoveToEx(pg->hDC, 0 + xOff, y, NULL);
				LineTo(pg->hDC, wid + xOff, y);
				EndPath(pg->hDC);
				StrokePath(pg->hDC);

				if (labelCounter >= 10) {
					labelCounter = 0;
					if (showLabels)
						drawTextString(pg->hDC, wid + xOff + 2, y - 5, strokeBlackStyle, tomString(reasonableValue(lv)));
				}
				SelectObject(pg->hDC, GetColorPen(strokeBlueStyle, 1));
				labelCounter++;
			}
		}
	}
	

	// draw the horizontal axis grid - vertical lines
	// pick nice round number for tic
	label = pickLabelSeparation(xScale);
	labelCounter = 10;
	if (label > 0) {
		//start = Math.ceil(0 / label) * label;
		for (double lv = 0; lv < maxCount - 1 + label / 10.0; lv += label) {
			if (labelCounter >= 10)
				SelectObject(pg->hDC, GetColorPen(strokeBlackStyle, 1));
			int ax = (lv)* xScale + xOff;

			BeginPath(pg->hDC);
			MoveToEx(pg->hDC, ax, 0 + yOff, NULL);
			LineTo(pg->hDC, ax, hgt + yOff);
			EndPath(pg->hDC);
			StrokePath(pg->hDC);

			if (labelCounter >= 10) {
				labelCounter = 0;
				if (showLabels)
					drawTextString(pg->hDC, ax, yOff + hgt - 0, strokeBlackStyle, tomString(reasonableValue(lv)));
			}
			SelectObject(pg->hDC, GetColorPen(strokeBlueStyle, 1));
			labelCounter++;
		}
	}

	// finally draw the data
	for (int ci = 0; ci < curves; ci++) {
		if ((int)input[ci].drivenByComponent == -1)
			continue;

		colorCurveStyle = penColors[ci % maxPenColors];
		SelectObject(pg->hDC, GetColorPen(colorCurveStyle, 1));

		// colored lines
		if ((mode == 0) || (mode == 2) || (mode == 4)) {// display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
			BeginPath(pg->hDC);
			for (j = 0; j < input[ci].value.size(); j++) {
				int ax = j * xScale + xOff;
				int ay = (max - input[ci].value[j]) * yScale + yOff;
				if (j == 0)
					MoveToEx(pg->hDC, ax, ay, NULL);
				else
					LineTo(pg->hDC, ax, ay);
			}
			EndPath(pg->hDC);
			StrokePath(pg->hDC);
		}

		// filled dots
		if ((mode == 1) || (mode == 2)) {// display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
			BeginPath(pg->hDC);
			for (j = 0; j < input[ci].value.size(); j++) {
				int ax = j * xScale + xOff;
				int ay = (max - input[ci].value[j]) * yScale + yOff;
				Ellipse(pg->hDC, ax - 3, ay - 3, ax + 4, ay + 4);
			}
			EndPath(pg->hDC);

			fillColor = GetColorBrush(colorCurveStyle);
			SelectObject(pg->hDC, fillColor);
			FillPath(pg->hDC);
		}
		if ((mode == 3) || (mode == 4)) {// display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
			BeginPath(pg->hDC);
			for (j = 0; j < input[ci].value.size(); j++) {
				int ax = j * xScale + xOff;
				int ay = (max - input[ci].value[j]) * yScale + yOff;
				MoveToEx(pg->hDC, ax + 3, ay + 3, NULL);
				LineTo(pg->hDC, ax - 3, ay - 3);
				MoveToEx(pg->hDC, ax + 3, ay - 3, NULL);
				LineTo(pg->hDC, ax - 3, ay + 3);
			}
			EndPath(pg->hDC);
			StrokePath(pg->hDC);
		}	
	}
	SelectObject(pg->hDC, GetColorPen(RGB(0,0,0), 1));
}

bool Chart_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!crankResizeHoldLeft(ax, ay, check))
		return false;

	int minHeight = pinToHeight(setting[2]);
	if (height < minHeight)
		height = minHeight;

	setting[0] = width;
	setting[1] = height;

	// because the input terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//  drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}



bool Chart_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	if (setting.size() < 3)   // in case we read an old component
		setting[2] = 1;

	int n = setting[2];
	int minHeight = pinToHeight(n);
	if (height < minHeight) {
		height = minHeight;
		setting[1] = minHeight;
	}
	// left terminals are inputs
	buildInputPins(n);

	//if (setting[3] == undefined)
	//	setting[3] = 0;
	return true;
}

void Chart_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if ((doubleValues[0] > 0) && (doubleValues[0] <= 6))
		setting[2] = (int)doubleValues[0]; // the number of inputs

	setting[4] = (boolValues[0] ? 1 : 0);	// the labels
	
	if ((radioValue >= 0) && (radioValue <= 4))
		setting[3] = radioValue; // display mode

	onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
}
void doubleClickChart(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Inputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2] };

	vector<string> checkLabels = { "Show Labels" };
	vector<bool> checkValues{ (bool)pg->dgrm.cL[i]->setting[4] };

	vector<string> radioLabels = { "Display Lines", "Display Dots", "Lines and Dots", "Display Crosses", "Lines and Crosses" };
	int radioIndex = pg->dgrm.cL[i]->setting[3];// display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>chartButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickChart) };

Chart_Component::Chart_Component()
	: Component(L"Chart", L"Chart", squareShape, 1, 1, 0, 0)
{
	dialogButtonList = chartButtonList;
	//hover = hoverChart;
	setSize(60, 60);
	setSetting(0, 60);
	setSetting(1, 60);
	setSetting(2, 1); // number of input pins
	setSetting(3, 0); // display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
	setSetting(4, 0); // display labels
	completionTestDoubles = 1;
	completionTestChecks = 1;
	completionTestRadios = 5;
}

// *************************** chart2 *********************************

void XyChart_Component::dial(Sheet* pg) {
	if (!checkValueInputs())
		return;
	// display mode setting[3]: // display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
	int mode = setting[3];
	bool showLabels = setting[4];
	
	// inputs setting[2]
	int inputs = setting[2];

	int wid = 2 * width - 25;
	int hgt = 2 * height - 20;

	int xOff = pg->cXo + x - width + 20;
	int yOff = pg->cYo + y - height + 10;

	// white Background
	BeginPath(pg->hDC);
	Rectangle(pg->hDC, xOff, yOff, xOff + wid, yOff + hgt);
	EndPath(pg->hDC);

	HBRUSH fillColor = GetColorBrush(0xFFFFFF);
	SelectObject(pg->hDC, fillColor);
	FillPath(pg->hDC);

	drawResizeBox(pg, this);
	int j;

	// first size up the data
	int xPin = 0;
	int yPin = inputs;
	double xmin = 1e200;//Number.MAX_VALUE;
	double xmax = -1e200;//Number.MIN_VALUE;
	double ymin = 1e200;//Number.MAX_VALUE;
	double ymax = -1e200;//Number.MIN_VALUE;

	for (int chart = 0; chart < inputs; chart++) {
		
		if (((int)input[xPin].drivenByComponent != -1) &&
			((int)input[yPin].drivenByComponent != -1)) {

			int count = input[xPin].value.size();
			if (count < 2)
				continue;
			if (input[yPin].value.size() != count)
				continue;

			for (j = 0; j < count; j++) {
				if (input[xPin].value[j] < xmin)
					xmin = input[xPin].value[j];
				if (input[xPin].value[j] > xmax)
					xmax = input[xPin].value[j];
			}

			for (j = 0; j < count; j++) {
				if (input[yPin].value[j] < ymin)
					ymin = input[yPin].value[j];
				if (input[yPin].value[j] > ymax)
					ymax = input[yPin].value[j];
			}
		}
		xPin++;
		yPin++;
	}

	// now plot the grid

	if (showLabels) {
		int labWidth = max(calcStringWidthFont(pg->hDC, tomString(ymin)),
						   calcStringWidthFont(pg->hDC, tomString(ymax)));
		wid -= labWidth + 4;
		hgt -= 12;
	}
	double xScale = wid / (xmax - xmin);
	double yScale = hgt / (ymax - ymin);

	COLORREF strokeBlueStyle = RGB(230, 230, 255);
	COLORREF strokeBlackStyle = RGB(170, 170, 200);
	COLORREF fillWhiteStyle = RGB(255, 255, 255);
	COLORREF strokeStyle = strokeBlueStyle;
	COLORREF colorCurveStyle;

	// draw the vertical axis grid - horizontal lines
	// pick nice round number for tic
	double label = pickLabelSeparation(yScale);
	if (label > 0) {
		int start = floor(ymin / label) * label;
		int labelCounter;
		if (start >= 0)
			labelCounter = (int)(10 - (label * 10 - start) / label) % 10; //  because the data doesnt start at zero
		else
			labelCounter = 10 + ((int)((start - label * 10) / label) % 10);
		if (start + label != start) { // check to see if we ill be infinite looping
			for (double lv = start; lv <= ymax; lv += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC, GetColorPen(strokeBlackStyle, 1));
				int y = (ymax - lv) * yScale + yOff;

				BeginPath(pg->hDC);
				MoveToEx(pg->hDC, 0 + xOff, y, NULL);
				LineTo(pg->hDC, wid + xOff, y);
				EndPath(pg->hDC);
				StrokePath(pg->hDC);

				if (labelCounter >= 10) {
					strokeStyle = RGB(136, 136, 255);
					labelCounter = 0;
					if (showLabels)
						drawTextString(pg->hDC, wid + xOff + 2, y - 5, strokeBlackStyle, tomString(reasonableValue(lv)));
				}
				SelectObject(pg->hDC, GetColorPen(strokeBlueStyle, 1));
				labelCounter++;
			}
		}
	}

	// draw the horizontal axis grid - vertical lines
	// pick nice round number for tic
	label = pickLabelSeparation(xScale);
	int labelCounter;
	if (label > 0) {
		int start = floor(xmin / label) * label;
		if (start >= 0)
			labelCounter = (int)(10 - (label * 10 - start) / label) % 10; //  because the data doesnt start at zero
		else
			labelCounter = 10 + ((int)((start - label * 10) / label) % 10);
		if (start + label != start) { // check to see if we ill be infinite looping
			for (double lv = start; lv <= xmax; lv += label) {
				if (labelCounter >= 10)
					SelectObject(pg->hDC, GetColorPen(strokeBlackStyle, 1));
				int ax = (lv - xmin) * xScale + xOff;

				BeginPath(pg->hDC);
				MoveToEx(pg->hDC, ax, 0 + yOff, NULL);
				LineTo(pg->hDC, ax, hgt + yOff);
				EndPath(pg->hDC);
				StrokePath(pg->hDC);

				if (labelCounter >= 10) {
					labelCounter = 0;
					if (showLabels)
						drawTextString(pg->hDC, ax, yOff + hgt + 0, strokeBlackStyle, tomString(reasonableValue(lv)) );
				}
				SelectObject(pg->hDC, GetColorPen(strokeBlueStyle, 1));
				labelCounter++;
			}
		}
	}
	
	// finally draw the data
	xPin = 0;
	yPin = inputs;
	for (int chart = 0; chart < inputs; chart++) {

		if ((input[xPin].drivenByComponent != -1) &&
			(input[yPin].drivenByComponent != -1)) {
			int count = input[xPin].value.size();
			if (count != input[yPin].value.size())
				continue;

			colorCurveStyle = penColors[chart % maxPenColors];
			SelectObject(pg->hDC, GetColorPen(colorCurveStyle, 1));

			if ((mode == 0) || (mode == 2) || (mode == 4)) { // display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
				BeginPath(pg->hDC);
				for (j = 0; j < count; j++) {
					int ax = (input[xPin].value[j] - xmin) * xScale + xOff;
					int ay = (ymax - input[yPin].value[j]) * yScale + yOff;

					if (j == 0)
						MoveToEx(pg->hDC, ax, ay, NULL);
					else
						LineTo(pg->hDC, ax, ay);
				}
				EndPath(pg->hDC);
				StrokePath(pg->hDC);
			}
			if ((mode == 1) || (mode == 2)) { // display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
				BeginPath(pg->hDC);
				for (j = 0; j < count; j++) {
					int ax = (input[xPin].value[j] - xmin) * xScale + xOff;
					int ay = (ymax - input[yPin].value[j]) * yScale + yOff;
					Ellipse(pg->hDC, ax - 3, ay - 3, ax + 4, ay + 4);
				}
				EndPath(pg->hDC);

				fillColor = GetColorBrush(colorCurveStyle);
				SelectObject(pg->hDC, fillColor);
				FillPath(pg->hDC);
			}
			if ((mode == 3) || (mode == 4)) { // display mode: 0 lines, 1 dots, 2 lines and dots, 3 crosses, 4 lines and crosses
				BeginPath(pg->hDC);
				for (j = 0; j < count; j++) {
					int ax = (input[xPin].value[j] - xmin) * xScale + xOff;
					int ay = (ymax - input[yPin].value[j]) * yScale + yOff;
					MoveToEx(pg->hDC, ax + 3, ay + 3, NULL);
					LineTo(pg->hDC, ax - 3, ay - 3);
					MoveToEx(pg->hDC, ax + 3, ay - 3, NULL);
					LineTo(pg->hDC, ax - 3, ay + 3);
				}
				EndPath(pg->hDC);
				StrokePath(pg->hDC);
			}
			
		}
		xPin++;
		yPin++;
	}
	SelectObject(pg->hDC, GetColorPen(RGB(0, 0, 0), 1));
}



bool XyChart_Component::crank(Sheet* pg,int ax, int ay, bool check) {
	if (!crankResizeHoldLeft(ax, ay, check))
		return false;

	int minHeight = pinToHeight(2 * setting[2]);
	if (height < minHeight)
		height = minHeight;

	setting[0] = width;
	setting[1] = height;

	// because the input terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//  drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}



bool XyChart_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	// left terminals are inputs
	int n = setting[2];
	int minHeight = pinToHeight(2*n);
	if (height < minHeight) {
		height = minHeight;
		setting[1] = minHeight;
	}
	
	buildInputPins(n * 2);
	int j = 0;
	for (int k = 0; k < n; k++) 
		input[j++].label = "X" + tomString(k);
	for (int k = 0; k < n; k++) 
		input[j++].label = "Y" + tomString(k);
	
	return true;
}

void XyChart_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	if ((doubleValues[0] > 0) && (doubleValues[0] <= 6))
		setting[2] = (int)doubleValues[0]; // the number of inputs

	setting[4] = (boolValues[0] ? 1 : 0);	// the labels

	if ((radioValue >= 0) && (radioValue <= 4))
		setting[3] = radioValue; // display mode

	onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
}

XyChart_Component::XyChart_Component()
	: Component(L"Chart2", L"XY Chart", squareShape, 2, 1, 0, 0)
{
	dialogButtonList = chartButtonList;		// same as Chart
	//hover = hoverChart2;
	setSize(60, 60);
	setSetting(0, 60);
	setSetting(1, 60);
	setSetting(2, 1); // inputs
	setSetting(3, 0); // display mode: 0=lines, 1= dots, 2= lines and dots
	setSetting(4, 0); // labels
}


// ***************************** Recorder *****************************************

void Recorder_Component::dial(Sheet* pg) {
	//.setting[0] = 60; // width
	//.setting[1] = 60; // height
	//.setting[2] = 100; // milliseconds per step
	//.setting[3] = // old system time

	if (input[0].drivenByComponent == -1)
		vectorMemoryY.clear(); // clear previous plots

	int points = 0;
	if (vectorMemoryY.size())
		points = vectorMemoryY.size();
	int channels = input[0].value.size();
	if (channels > 10)
		return;

	int c, p;

	// compare number of input channels with channels in memory
	if (channels != vectorMemoryY.size())// restart
	{
		vectorMemoryY.resize(channels);
		for (c = 0; c < channels; c++) {
			vectorMemoryY[c].resize(0);
		}
		points = 0;
	}

	int wid = 2 * width - 10;
	int hgt = 2 * height - 10;

	if (checkValueInputs()) {

		// reset - and capture the first data point
		if (pg->systemTime == 0) {
			if (points != 0) {
				for (c = 0; c < channels; c++)
					vectorMemoryY[c].resize(0);
			}
			setting[3] = 0; // system Time
		}
		else {
			bool stepNow = false;

			if (pg->systemTime > setting[3]) // DEBUG with single step
			{
				setting[3] = pg->systemTime;
				stepNow = true;
			}

			if (stepNow) {
				// add a point to each channel
				for (c = 0; c < channels; c++)
					vectorMemoryY[c].push_back(input[0].value[c]);
				points = vectorMemoryY[0].size();
			}
		}

		if (points > wid) // trim if needed
		{
			for (c = 0; c < channels; c++)
				vectorMemoryY[c].erase(vectorMemoryY[c].begin(), vectorMemoryY[c].begin() + points - wid);
			points = vectorMemoryY[0].size();
		}
	}

	int xOff = pg->cXo + x - width + 5;
	int yOff = pg->cYo + y - height + 5;

	// white Background
	BeginPath(pg->hDC);
	Rectangle(pg->hDC, xOff, yOff, xOff + wid, yOff + hgt);
	EndPath(pg->hDC);

	HBRUSH fillColor = GetColorBrush(0xFFFFFF);
	SelectObject(pg->hDC, fillColor);
	FillPath(pg->hDC);

	drawResizeBox(pg, this);

	if (checkValueInputs()) {
		points = vectorMemoryY[0].size();
		if (points < 2)
			return;

		// scan data for y range
		double max = vectorMemoryY[0][0];
		double min = max;
		for (c = 0; c < channels; c++) {
			for (p = 0; p < points; p++) {
				if (vectorMemoryY[c][p] < min)
					min = vectorMemoryY[c][p];
				if (vectorMemoryY[c][p] > max)
					max = vectorMemoryY[c][p];
			}
		}
		if ((max - min) < 1e-10) {
			double ave = (max + min) / 2;
			max = ave + 1e-10;
			min = ave - 1e-10;
		}
		
		int ax;
		int ay;
		double xScale = 1; // was: wid / (count-1);
		double yScale = hgt / (max - min);

		// draw the time grid - one second intervals
		COLORREF strokeBlueStyle = RGB(230, 230, 255);
		SelectObject(pg->hDC, GetColorPen(strokeBlueStyle, 1));
		double temp1 = pg->systemTime - points * pg->timeStep;
		double timeAtZero = temp1 - floor(temp1);
		int step = 1.0 / pg->timeStep;
		int start = (1.0 - timeAtZero) / pg->timeStep;
		int gridXoff = pg->cXo + x - width + 4;
		int top = pg->cYo + y - height + 5;
		int bot = pg->cYo + y + height - 5;
		BeginPath(pg->hDC);
		for (ax = start; ax < 2 * width - 10; ax += step) {
			MoveToEx(pg->hDC, ax + gridXoff, top, NULL);
			LineTo(pg->hDC, ax + gridXoff, bot);
		}
		EndPath(pg->hDC);
		StrokePath(pg->hDC);

		// draw the data grid
		// pick nice round number for tic
		int label = pickLabelSeparation(yScale);
		if (label != 0) {
			start = ceil(min / label) * label;
			BeginPath(pg->hDC);
			for (int lv = start; lv <= max; lv += label) {
				ay = (max - lv) * yScale + yOff;
				MoveToEx(pg->hDC, 0 + gridXoff, y, NULL);
				LineTo(pg->hDC, wid + gridXoff, y);
			}
			EndPath(pg->hDC);
			StrokePath(pg->hDC);
		}

		// plot the data
		int penc;
		for (c = 0; c < channels; c++) {
			penc = c % maxPenColors;
			COLORREF strokeColorStyle = penColors[penc];
			SelectObject(pg->hDC, GetColorPen(strokeColorStyle, 1));

			BeginPath(pg->hDC);
			for (p = 0; p < points; p++) {
				ax = p * xScale + xOff;
				ay = (max - vectorMemoryY[c][p]) * yScale + yOff;
				if (p == 0)
					MoveToEx(pg->hDC, ax, ay, NULL);
				else
					LineTo(pg->hDC, ax, ay);
			}
			EndPath(pg->hDC);
			StrokePath(pg->hDC);
		}
	}
	SelectObject(pg->hDC, GetColorPen(0, 1));
}


bool Recorder_Component::crank(Sheet* pg, int ax, int ay, bool check) {

	if (!crankResizeHoldLeft(ax, ay, check))
		return false;

	setting[0] = width;
	setting[1] = height;

	//  memory = []; // clear previous plots

	// because the input terminal moved
	//   analyzeConnections(pg);
	//   propagateValues();
	//   drawCanvas();
	//  drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}


bool Recorder_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	return true;
}
Recorder_Component::Recorder_Component( double systemTime)
	: Component(L"Rec", L"Recorder", squareShape, 1, 1, 0, 0)
{
	timeBased = true;
	//dialogList = dialogRecButtonList;
	//hover = hoverRec;
	setSize(60, 60);
	setSetting(0, 60); // width
	setSetting(1, 60); // height
	setSetting(2, 100); // limit
	setSetting(3, systemTime);
}

// ******************************************* X Y Recorder *******************************************


void XYRecorder_Component::dial(Sheet* pg) {

	int border = 30;
	int wid = 2 * width - border;
	int hgt = 2 * height - border;

	int xOff = pg->cXo + x - width + border / 2;
	int yOff = pg->cYo + y - height + border / 2;

	// white Background
	BeginPath(pg->hDC);
	Rectangle( pg->hDC, xOff, yOff, xOff + wid, yOff + hgt);
	EndPath(pg->hDC);

	HBRUSH fillColor = GetColorBrush(0xFFFFFF);
	SelectObject(pg->hDC, fillColor);
	FillPath(pg->hDC);

	drawResizeBox(pg, this);

	// scan all channels for x and y range
	double minX = 1e200;
	double maxX = -1e200;
	double minY = 1e200;
	double maxY = -1e200;

	int channels = vectorMemoryX.size();
	for (int c = 0; c < channels; c++) {
		int points = vectorMemoryX[c].size();
		for (int p = 0; p < points; p++) {
			if (vectorMemoryX[c][p] < minX)
				minX = vectorMemoryX[c][p];
			if (vectorMemoryX[c][p] > maxX)
				maxX = vectorMemoryX[c][p];

			if (vectorMemoryY[c][p] < minY)
				minY = vectorMemoryY[c][p];
			if (vectorMemoryY[c][p] > maxY)
				maxY = vectorMemoryY[c][p];
		}
	}
	if (minX == maxX)
		return;
	if (minY == maxY)
		return;

	int ax;
	int ay;
	double xScale = wid / (maxX - minX);
	double yScale = hgt / (maxY - minY);

	int gridXoff = pg->cXo + x - width + border / 2;
	int gridYoff = pg->cYo + y - height + border / 2;

	// draw the X grid - vertical lines
	COLORREF strokeBlueStyle = RGB(230, 230, 255);
	SelectObject(pg->hDC, GetColorPen(strokeBlueStyle, 1));

	if (xScale > 0.0) {
		int label = pickLabelSeparation(xScale);
		if (label != 0) {
			int start = floor(minX / label) * label;
			BeginPath(pg->hDC);
			for (int lv = start; lv <= maxX; lv += label) {
				ax = (lv - minX) * xScale;
				MoveToEx(pg->hDC, ax + gridXoff, 0 + gridYoff, NULL);
				LineTo(pg->hDC, ax + gridXoff, hgt + gridYoff);
			}
			EndPath(pg->hDC);
			StrokePath(pg->hDC);
		}
	}

	// draw the Y grid - horizontal lines
	// pick nice round number for tic
	if (yScale > 0.0) {
		int label = pickLabelSeparation(yScale);
		if (label != 0) {
			int start = floor(minY / label) * label;
			BeginPath(pg->hDC);
			for (int lv = start; lv <= maxY; lv += label) {
				ay = (lv - minY) * yScale;
				MoveToEx(pg->hDC, 0 + gridXoff, ay + gridYoff, NULL);
				LineTo(pg->hDC, wid + gridXoff, ay + gridYoff);
			}
			EndPath(pg->hDC);
			StrokePath(pg->hDC);
		}
	}

	// plot the data
	int penc;
	for (int c = 0; c < channels; c++) {
		penc = c % maxPenColors;
		COLORREF strokeColorStyle = penColors[penc];
		SelectObject(pg->hDC, GetColorPen(strokeColorStyle, 1));

		BeginPath(pg->hDC);
		int points = vectorMemoryY[c].size();

		for (int p = 0; p < points; p++) {
			//if (vectorMemoryY[c][p] == null)
			//	continue;
			//if (vectorMemoryX[c][p] == null)
			//	continue;

			ax = (vectorMemoryX[c][p] - minX) * xScale;
			ay = (maxY - vectorMemoryY[c][p]) * yScale;

			if (p == 0)
				MoveToEx(pg->hDC, ax + gridXoff, ay + gridYoff, NULL);
			else
				LineTo(pg->hDC, ax + gridXoff, ay + gridYoff);
		}
		EndPath(pg->hDC);
		StrokePath(pg->hDC);
	}
	SelectObject(pg->hDC, GetColorPen(0, 1));
}

bool XYRecorder_Component::crank(Sheet* pg, int ax, int ay, bool check) {

	if (!crankResizeHoldLeft(ax, ay, check))
		return false;

	int minHeight = pinToHeight(3);
	if (height < minHeight)
		height = minHeight;

	setting[0] = width;
	setting[1] = height;

	// display it
	dial(pg);

	return true;
}

int XYRecorder_Component::method2(Sheet* pg) {
	//.setting[0] = 60; // width
	//.setting[1] = 60; // heigh

	if ((input[0].drivenByComponent == -1) ||
		(input[1].drivenByComponent == -1)) {
		vectorMemoryX.clear(); // clear previous plots
		vectorMemoryY.clear(); // clear previous plots
	}

	int channels = inputsToLoops();
	if (channels == 0) {
		vectorMemoryX.clear(); // clear previous plots
		vectorMemoryY.clear(); // clear previous plots
	}
	int c;

	// compare number of input channels with channels in memory
	if (channels != vectorMemoryX.size())// restart
	{
		vectorMemoryX.resize(channels);
		vectorMemoryY.resize(channels);
		for (c = 0; c < channels; c++) {
			vectorMemoryX[c].clear();
			vectorMemoryY[c].clear();
		}
	}

	if (input[2].drivenByComponent != -1) {
		for (c = 0; c < channels; c++) {
			if (getLoopingValueD(2, c, 0)) {
				// reset
				vectorMemoryX[c].clear();
				vectorMemoryY[c].clear();
			}
		}
	}

	for (c = 0; c < channels; c++) {
		bool addPoint = false;
		int points = 0;
		if (vectorMemoryX.size())
			points = vectorMemoryX[c].size();
		int lastPoint = points - 1;
		if (lastPoint < 0)
			addPoint = true;
		else {
			if (vectorMemoryX[c][lastPoint] != getLoopingValueD(0, c, 0)) {
				addPoint = true;
			}
			if (vectorMemoryY[c][lastPoint] != getLoopingValueD(1, c, 0)) {
				addPoint = true;
			}
		}

		if (addPoint) {
			// add a point to this channel
			vectorMemoryX[c].push_back(getLoopingValueD(0, c, 0));
			vectorMemoryY[c].push_back(getLoopingValueD(1, c, 0));
		}
	}
	return true;
}

bool XYRecorder_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	return true;
}
XYRecorder_Component::XYRecorder_Component()
	: Component(L"XyRec", L"XY Recorder", squareShape, 3, 1, 0, 0)
{
	//dialogList = dialogXyRecButtonList;
	//hover = hoverXyRec;
	setSize(60, 60);
	setSetting(0, 60); // width
	setSetting(1, 60); // height
	//memory2 = [];
	setLeftLabels(3, "X", "Y", "C");
}

// **************************** FIFO and LIFO *********************************

void FIFO_Component::updateOutputFIFO(Sheet* pg) {

	int outputTerminals = setting[0];
	int stackSize = vectorMemoryX.size();
	int dataSize = 0;
	if( stackSize )
		dataSize = vectorMemoryX[0].size();

	output[0].value[0] = stackSize;
	output[0].dimensions.resize(1); 
	output[0].dimensions[0] = 1;

	// check sizes
	if (input[0].drivenByComponent != -1) {

		if (type == L"FIFO") {
			for (int j = 0; j < outputTerminals; j++) {// limit to size of stack
				if (j < stackSize) {
					output[j + 1].value.resize(dataSize);
					for (int k = 0; k < dataSize; k++)
						output[j + 1].value[k] = vectorMemoryX[j][k];
					output[j + 1].dimensions.resize(1);
					output[j + 1].dimensions[0] = dataSize;
				}
				else {
					output[j + 1].value.clear();
					output[j + 1].dimensions.clear();
				}
			}
		}
		else
		{
			int revj = stackSize - 1;
			for (int j = 0; j < outputTerminals; j++) {// limit to size of stack
				if (j < stackSize) {
					output[j + 1].value.resize(dataSize);
					for (int k = 0; k < dataSize; k++)
						output[j + 1].value[k] = vectorMemoryX[revj][k];
					output[j + 1].dimensions.resize(1);
					output[j + 1].dimensions[0] = dataSize;
					revj--;
				}
				else {
					output[j + 1].value.clear();
					output[j + 1].dimensions.clear();
				}
			}
		}
	}
	else
		for (int j = 0; j < outputTerminals; j++) {
			output[j + 1].value.clear();
			output[j + 1].dimensions.clear();
		}
}



int FIFO_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	// first check for reset
	if (input[3].value[0]) {
		// clear all data
		vectorMemoryX.clear(); // may need to clear the individual vectors first
		updateOutputFIFO(pg);
		return true;
	}

	bool change = false;

	// check the push input
	if (input[1].value[0])
	{
		if( !memory[0] ){ // positive edge of Push input
			memory[0] = true;

			// check sizes
			if (vectorMemoryX.size() > 0) {
				if (vectorMemoryX[0].size() != input[0].value.size()) {
					vectorMemoryX.clear(); // may need to clear the individual vectors first
				}
			}

			// save the input in the next memory slot
			std::vector<double> temp;
			for (int k = 0; k < input[0].value.size(); k++) 
				temp.push_back(input[0].value[k]);
			vectorMemoryX.push_back(temp);
			change = true;
		}
	}
	else // negative edge
		memory[0] = false;
		
	// output clock
	if (input[2].value[0])  // positive edge of "Pop" input
	{
		if (!memory[1]) {
			memory[1] = true;

			// output next value; shift everyone down // may need to clear the individual vectors first
			int size = vectorMemoryX.size();
			if (size > 0) {
				change = true;
				if (type == L"FIFO") {
					for (int j = 1; j < size; j++)
						vectorMemoryX[j-1] = vectorMemoryX[j];
				}
				vectorMemoryX.resize(size - 1);
			}
		}
	}
	else // negative edge
		memory[1] = false;

	if (change)
		updateOutputFIFO(pg);
	return change;
}

bool FIFO_Component::onLoad(Sheet* pg) {
	
	int n = setting[0];
	height = pinToHeight(max(4, n + 1));

	// right terminals are outputs
	buildOutputPins(n + 1);
	output[0].label = "Size";
	for (int j = 0; j < n; j++)
		output[j+1].label = tomString(j);

	return true;
}

void FIFO_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if ((doubleValues[0] > 0) && (doubleValues[0] < 32)) {
		setting[0] = doubleValues[0];
		onLoad(pg);
	}
}
void buttonFifo_SetOptionsAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	
	vector<string> doubleLabels = { "Number of Inputs:"  };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

std::vector<Button*> fifoButtonList = { new Button(-90, 40, 130, 20, L"Set Opions...", buttonFifo_SetOptionsAction) };

FIFO_Component::FIFO_Component()
	: Component(L"FIFO", L"FIFO", squareShape, 4, 1, 4, 1)
{
	dialogButtonList = fifoButtonList;
	setLeftLabels(3, "Data", "Push", "Pop", "Clear");
	setMemory(0, true); // push clock memory
	setMemory(1, true); // pop clock memory
	output[0].value.clear(); // start empty
	setSetting(0, 3); // output terminals
	//dialogList = dialogFIFOButtonList;
}

//LIFO_Component::LIFO_Component()
//	: Component(L"LIFO", L"LIFO", methodFIFO_, squareShape, 4, 1, 4, 1)
//{
//	setLeftLabels(3, "Data", "Push", "Pop", "Clear");
//	//setRightLabels(3, "Size", "0");
//	setMemory(0, true); // push clock edge memory
//	setMemory(1, true); // pop clock edge memory
//	output[0].value.clear(); // start empty
//	setSetting(0, 3); // output terminals
//
//	//dialogList = dialogFIFOButtonList;
//}

// *************************** Savings *********************************************

// inputs 0) Initial Deposit, 1)Interest Rate, 2) Compounds per Year, 3) Years
	// outputs 0) Value, 1) Interest
int Savings_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	for (int k = 0; k < 2; k++) {
		output[k].value.clear();
		output[k].dimensions.clear();
	}

	if ((input[0].drivenByComponent == -1) ||  // Deposit
		(input[1].drivenByComponent == -1)) {   // Rate
		return clearOutputs();
	}

	double deposit = getLoopingValueD(0, 0, 0);
	double rate = getLoopingValueD(1, 0, 0) / 100;
	int compounds = floor(getLoopingValueD(2, 0, 12));
	double years = getLoopingValueD(3, 0, 1);

	if ((deposit <= 0) ||
		(rate <= 0) ||
		(compounds <= 0) ||
		(years <= 0)) {
		return clearOutputs();
	}
	int loops = floor(compounds * years);
	if (loops < 0)
		return clearOutputs();
	double compoundRate = rate / compounds;
	double interest = deposit * compoundRate;
	double principal = deposit;

	output[0].value.resize(loops);
	output[1].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		output[0].value[k] = principal;
		output[1].value[k] = interest;
		interest = principal * compoundRate;
		principal += interest;
	}
	for (int k = 0; k < 2; k++) {
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}
	return 1;
}

Savings_Component::Savings_Component()
	: Component(L"Savings", L"Savings", squareShape, 4, 1, 2, 1)
{
	setLeftLabels(3, "Initial Deposit", "Interest Rate(%)", "Compounds/Year", "Years");
	setRightLabels(3, "Value", "Interest");
	width = 75;
}

// *************************** Loan *********************************************

// inputs 0) Amount, 1)Interest Rate, 2) Ponts, 3) Years
	// outputs 0) Principal, 1) Interest, 2) Total, 3) Payment
int Loan_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	for (int k = 0; k < 4; k++) {
		output[k].value.clear();
		output[k].dimensions.clear();
	}
	if ((input[0].drivenByComponent == -1) ||   // Amount
		(input[1].drivenByComponent == -1) ||   // Rate
		(input[3].drivenByComponent == -1))     // Years
		return clearOutputs();

	int points = 0;
	if (input[2].drivenByComponent != -1)     // Ponts
		points = input[2].value[0];

	double amount = input[0].value[0];
	double rate = input[1].value[0] / 100;
	double years = input[3].value[0];

	if ((amount <= 0) ||
		(rate <= 0) ||
		(years <= 0))
		return clearOutputs();

	int loops = floor(12 * years);
	if( loops < 0)
		return clearOutputs();
	double compoundRate = rate / 12;
	double interest = amount * compoundRate;
	double principalRemaining = amount;
	double interestPaid = 0;
	double payment = amount * (compoundRate / (1 - pow(1 + compoundRate, -loops)));
	double totalPaid = amount * points / 100;

	for (int k = 0; k < 3; k++) {
		output[k].value.resize(loops);
		output[k].dimensions.resize(1);
		output[k].dimensions[0] = loops;
	}
	output[3].value.resize(1);
	output[3].value[0] = payment;
	output[3].dimensions.resize(1);
	output[3].dimensions[0] = 1;

	// outputs 0) Principal, 1) Interest, 2) Total,
	for (int k = 0; k < loops; k++) {
		interest = principalRemaining * compoundRate;
		principalRemaining += interest - payment;
		interestPaid += interest;
		totalPaid += payment;
		output[0].value[k] = principalRemaining;
		output[1].value[k] = interestPaid;
		output[2].value[k] = totalPaid;
	}
	return 1;
}

Loan_Component::Loan_Component()
	: Component(L"Loan", L"Loan", squareShape, 4, 1, 4, 1)
{
	setLeftLabels(3, "Amount", "Interest Rate(%)", "Ponts", "Years");
	setRightLabels(3, "Principal", "Interest", "Total", "Payment");
	width = 75;
}

// *************************** Primes *********************************************

int Primes_Component::method2(Sheet* pg) {

	
	//startMemCheck();
	//LPWSTR test;
	//test = ConvertToLPWSTR("Lets test the string conversion");
	//delete test;
	//endMemCheck();
	

	//startMemCheck();
	//vector<InputClass*> testVector1;
	//endMemCheck();

	// this results in two test vectors that are independent
	/*
	vector<InputClass> testVector1;
	vector<InputClass> testVector2;

	for (int j = 0; j < 10; j++) {
		InputClass* temp = new InputClass();
		temp->value.push_back(j);
		temp->dimensions.push_back(j);
		temp->drivenByComponent = j;
		temp->drivenByOutputTerminal = j;
		testVector1.push_back(*temp);
		testVector2.push_back(*temp);
		delete temp;
	}

	for (int j = 0; j < 10; j++) {
		testVector1[j].value[0] = 99;
		testVector1[j].dimensions[0] = 99;
		testVector1[j].drivenByComponent = 99;
		testVector1[j].drivenByOutputTerminal = 99;
	}
	*/


	// Sieve of Eratosthenes
	if( !checkValueInputs())
		return clearOutputs();

	if (input[0].drivenByComponent == -1)
		return true;

	int n = 1 + floor(input[0].value[0]);
	if (n < 2)
		return true;

	// build an array that will be used to find the prime numbers
	output[0].value.clear();
	output[1].value.resize(n);

	// initialize the array to all true
	for (int j = 0; j < n; j++)
		output[1].value[j] = 1;

	int out = 0;

	output[1].value[0] = 0; // 0 is not a prime
	output[1].value[1] = 0; // amazingly one is not a prime

	for (int j = 2; j < n; j++) {

		if (j * j > n) { // wrap it up
			for (; j < n; j++)
				if (output[1].value[j] != 0) // push the remaining primes to list
					output[0].value.push_back(j);
			break;
		}

		if (output[1].value[j] == 0) // skip non primes
			continue;

		// as we encounter prime numbers (1's in the array) put them in the list
		output[0].value.push_back(j);

		// then set all the multiples of that number to false
		// starting with the square of the prime
		for (int k = j * j; k < n; k += j)
			output[1].value[k] = 0;
	}

	output[0].dimensions.resize(1);
	output[0].dimensions[0] = out;
	output[1].dimensions.resize(1);
	output[1].dimensions[0] = n;
	return true;
}


Primes_Component::Primes_Component()
	: Component(L"Primes", L"Primes", squareShape, 1, 1, 2, 1)
{
	setLeftLabels(3, "Limit");
	setRightLabels(3, "Numbers", "Positions");
	width = 45;
}



// *************************** Color Patch *********************************


void ColorPatch_Component::dial(Sheet* pg) {

	if (!checkValueInputs())
		return;

	int wid = 2 * width - 50;
	int hgt = 2 * height - 20;

	int xOff = pg->cXo + x - width + 40;
	int yOff = pg->cYo + y - height + 10;

	// color patch

	BeginPath(pg->hDC);
	Rectangle(pg->hDC, xOff, yOff, xOff + wid, yOff + hgt);
	EndPath(pg->hDC);

	int val[3];
	for (int k = 0; k < 3; k++) {
		val[k] = input[k].value[0];
		if (val[k] < 0)
			val[k] = 0;
		if (val[k] > 255)
			val[k] = 255;
	}
	HBRUSH fillColor = GetColorBrush(RGB(val[0], val[1], val[2]));
	SelectObject(pg->hDC, fillColor);
	FillPath(pg->hDC);

	SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH));
	drawResizeBox(pg, this);
	
	SelectObject(pg->hDC, GetColorPen(RGB(0, 0, 0), 1));
}

bool ColorPatch_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	if (!crankResizeHoldLeft(ax, ay, check))
		return false;

	int minHeight = pinToHeight(3);
	if (height < minHeight)
		height = minHeight;

	if (width < 40)
		width = 40;

	setting[0] = width;
	setting[1] = height;

	// move the dial
	dial(pg);

	return true;
}



bool ColorPatch_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];
	return true;
}

ColorPatch_Component::ColorPatch_Component()
	: Component(L"ClrPatch", L"Color Patch", squareShape, 3, 1, 0, 0)
{
	setLeftLabels(3, "Red", "Green", "Blue");
	setSize(60, 30);
	setSetting(0, 60);
	setSetting(1, 30);
}

// ****************************** Bezier *************************************



int binomial[10][10] = {
	{1},
	{1,1}, 
	{1,2,1},
	{1,3,3,1}, 
	{1, 4, 6, 4, 1},
	{1, 5, 10, 10, 5, 1},
	{1, 6, 15, 20, 15, 5, 1},
	{1, 7, 21, 35, 35, 21, 7, 1},
	{1, 8, 28, 56, 70, 56, 28, 8, 1},
	{1, 9, 36, 84, 126, 126, 84, 36, 9, 1} 
};


int Bezier_Component::method2(Sheet* pg) {
	int dims = setting[0];

	// validate inputs
	int points = input[1].value.size();
	if ((points < 1) || (points > 10))
		return clearOutputs();
	for (int k = 1; k < dims; k++)
		if (points != input[k + 1].value.size())
			return clearOutputs();

	// initialize outputs
	int loops = input[0].value.size();
	for (int k = 0; k < dims; k++) {
		output[k].value.resize(loops);
		output[k].dimensions[0] = loops;
	}

	double x;// = [];
	vector<double> t;
	vector<double> oneMinusT;

	for (int j = 0; j < loops; j++) {

		double inputT = input[0].value[j];
		double input1minusT = 1.0 - inputT;

		t.resize(points);
		oneMinusT.resize(points);
		t[0] = 1;
		oneMinusT[0] = 1;
		for (int n = 1; n < points; n++) {
			t[n] = t[n - 1] * inputT;
			oneMinusT[n] = oneMinusT[n - 1] * input1minusT;
		}

		for (int k = 0; k < dims; k++) {
			x = 0;
			for (int n = 0; n < points; n++)
				x += binomial[points - 1][n] * oneMinusT[points - 1 - n] * t[n] * input[k + 1].value[n];
			output[k].value[j] = x;
		}
	}
	return true;
}



// BEZIER and Bezier Derivitave 
void buttonBezier2DimsAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 2;
	pg->dgrm.cL[i]->onLoad(pg);
	pg->dgrm.cL[i]->method2(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}
void buttonBezier3DimsAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 3;
	pg->dgrm.cL[i]->onLoad(pg);
	pg->dgrm.cL[i]->method2(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}

std::vector<Button*>bezierButtonList = { new Button(-80, 40, 125, 20, L"Set 2 Dimensions", buttonBezier2DimsAction),
										 new Button(-80, 60, 125, 20, L"Set 3 Dimensions", buttonBezier3DimsAction) };

bool Bezier_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n + 1);

	// right terminals are outputs
	buildOutputPins(n);

	// left terminals are inputs
	buildInputPins(n + 1);

	if (n == 2) {
		setRightLabels(2, "X", "Y");
		setLeftLabels(3, "t (0 to 1)", "Px", "Py");
	}
	if (n == 3) {
		setRightLabels(3, "X", "Y", "Z");
		setLeftLabels(4, "t (0 to 1)", "Px", "Py", "Pz");
	}
	return true;
}

Bezier_Component::Bezier_Component()
	: Component(L"Bezier", L"Bezier", squareShape, 3, 1, 2, 1)
{
	dialogButtonList = bezierButtonList;
	setSetting(0, 2); // X,Y
}

// ****************************** Bezier Derivative **************************

int BezierDeriv_Component::method2(Sheet* pg) {

	int dims = setting[0];

	// validate inputs
	int points = input[1].value.size();
	if ((points < 1) || (points > 10))
		return clearOutputs();
	for (int k = 1; k < dims; k++)
		if (points != input[k + 1].value.size())
			return clearOutputs();

	// initialize outputs
	int loops = input[0].value.size();
	for (int k = 0; k < dims; k++) {
		output[k].value.resize(loops);
		output[k].dimensions[0] = loops;
	}

	double x;// = [];
	vector<double> t;
	vector<double> oneMinusT;

	for (int j = 0; j < loops; j++) {

		double inputT = input[0].value[j];
		double input1minusT = 1.0 - inputT;

		t.resize(points);
		oneMinusT.resize(points);
		t[0] = 1;
		oneMinusT[0] = 1;
		for (int n = 1; n < points; n++) {
			t[n] = t[n - 1] * inputT;
			oneMinusT[n] = oneMinusT[n - 1] * input1minusT;
		}

		for (int k = 0; k < dims; k++) {
			x = 0;
			for (int n = 0; n < points - 1; n++)
				x += binomial[points - 2][n] * oneMinusT[points - 1 - n] * t[n] * (input[k + 1].value[n + 1] - input[k + 1].value[n]);
			output[k].value[j] = points * x;
		}
	}
	return true;
}
bool BezierDeriv_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n + 1);

	// right terminals are outputs
	buildOutputPins(n);

	// left terminals are inputs
	buildInputPins(n + 1);

	if (n == 2) {
		setRightLabels(2, "X", "Y");
		setLeftLabels(3, "t (0 to 1)", "Px", "Py");
	}
	if (n == 3) {
		setRightLabels(3, "X", "Y", "Z");
		setLeftLabels(4, "t (0 to 1)", "Px", "Py", "Pz");
	}
	return true;
}

BezierDeriv_Component::BezierDeriv_Component()
	: Component(L"BezDeriv", L"Bezier Derivitave", squareShape, 3, 1, 2, 1)
{ 
	dialogButtonList = bezierButtonList;
	setSetting(0, 2); // X,Y
}



//*******************************************************
//  ================= Simplex Search ====================
//*******************************************************

const int PHASE_INITIAL_PROBE = 0;		// pg->dgrm.cL[i].lines tells how its going
const int PHASE_GO_AGAIN = 1;
const int PHASE_100_B = 3;
//var PHASE_200_A			= 4;
const int PHASE_200_B = 5;
//var PHASE_300_A			= 6;
const int PHASE_300_B = 7;
const int PHASE_EVALUATE = 8;		// pg->dgrm.cL[i].lines tells how its going
const int PHASE_DONE = 99;


const int TOP_INPUTS = 4;  // Feedback, Tolerance, Hold, and Reset inputs
const int TOP_OUTPUTS = 2; // Done and Error outputs
//  var MAX_SIMPLEX_PARAM_LIMIT = 10;

bool Simplex_Component::initialSetup(double func)
{
	int j, k;
	bool done;
	vector<double> param;
	vector<double> delta;

	int params = input[4].value.size();
	param.resize(params);
	for (k = 0; k < params; k++)
		param[k] = input[4].value[k];

	delta.resize(params);
	if (input[5].drivenByComponent == -1) {
		for (k = 0; k < params; k++)
			delta[k] = 1;
	}
	else {
		if (input[5].value.size() < 1) {
			for (k = 0; k < params; k++)
				delta[k] = 1;
		}
		else {
			if (input[5].value.size() < params) {
				for (k = 0; k < params; k++)
					delta[k] = input[5].value[0];
			}
			else {
				for (k = 0; k < params; k++)
					delta[k] = input[5].value[k];
			}
		}
	}

	// ok now we are going to setup an internal array to hold the vectors
	if (lines == -1) {   // initial
		simp_p.resize(params + 2);
		for (j = 0; j < params + 2; j++) {
			simp_p[j].resize(params+2);
			for (k = 0; k < params + 2; k++)
				simp_p[j][k] = 0;
		}
	}
	else // not initial
	{
		// grab the previous input
		simp_y[lines] = func;
		shorts[lines] = true;
	}
	// added this to prevent array overrun
	if (lines == params)
		return true;

	// pass the inputs to the outputs
	// k = (lines+1) * params;
	output[TOP_OUTPUTS].value.resize(params);
	for (j = 0; j < params; j++) {
		output[TOP_OUTPUTS].value[j] = param[j]; // output
		simp_p[lines + 1][j] = param[j]; // may be over written later // todo debug this
	}

	// check for empty verticies
	done = true; // lets hope for the best
	for (j = 0; j <= params; j++) {
		if (!shorts[j]) {
			done = false; // reality sets in

			// found an empty
			phase = PHASE_INITIAL_PROBE; // not done, come back here
			lines = j;
			if (j == 0)
				break;	// vertex 0 is just the inputs

			// other vertices add the delta
			output[TOP_OUTPUTS].value[j - 1] += delta[j - 1];

			//k = lines * params;
			simp_p[lines][j - 1] = param[j - 1] + delta[j - 1]; // overwrite // todo debug this
			break;
		}
	}

	return done;
}

void Simplex_Component::getPsum() {
	int j, k;
	double sum;
	int params = input[4].value.size();

	simp_pSum.resize(params);
	for (j = 0; j < params; j++) {
		sum = 0.0;

		for (k = 0; k <= params; k++)
			sum += simp_p[k][j];

		simp_pSum[j] = sum;
	}
}

void Simplex_Component::findBestAndWorst() {
	int j;
	double params = input[4].value.size();

	//this is cute but i cant debug it
	ilo = 0;
	inhi = 0; // added in javascript
	ihi = 0; // added in javascript

	// determine which point is the highest (worst),
	//   next-highest, and lowest (best), by looping over the points in the simplex.
	ihi = simp_y[0] > simp_y[1] ? (inhi = 1, 0) : (inhi = 0, 1);
	for (j = 0; j <= params; j++) {
		if (simp_y[j] <= simp_y[ilo])
			ilo = j;
		if (simp_y[j] > simp_y[ihi]) {
			inhi = ihi;
			ihi = j;
		}
		else if (simp_y[j] > simp_y[inhi] && j != ihi)
			inhi = j;
	}
}

bool Simplex_Component::checkTolerance() {
	double rtol;//, swap;
	double ftol = input[1].value[0]; // should be ready by now
	double temp;

	int j;
	//var numerator, denominator;
	int params = input[4].value.size();

	rtol = abs(simp_y[ihi] - simp_y[ilo]);

	if (rtol < ftol)	// if returning, put best point and value in slot 0.
	{
		if (ilo != 0) {
			//SWAP( y[0], y[pg->dgrm.cL[i].ilo] );
			temp = simp_y[0];
			simp_y[0] = simp_y[ilo];
			simp_y[ilo] = temp;

			for (j = 0; j < params; j++) {
				//SWAP( p[0][i], p[ilo][i] )
				temp = simp_p[0][j];
				simp_p[0][j] = simp_p[ilo][j];
				simp_p[ilo][j] = temp;
			}
		}
		// send the lowest result to the output pins
		for (j = 0; j < params; j++) {
			output[TOP_OUTPUTS].value[j] = simp_p[ilo][j];
		}

		output[0].value[0] = true;
		return true; //  break out of the loop
	}
	return false;
}

void Simplex_Component::amotryA(double fac) {
	int j;
	double fac1, fac2;
	
	int params = input[4].value.size();

	fac1 = (1.0 - fac) / params;
	fac2 = fac1 - fac;

	for (j = 0; j < params; j++) {
		output[TOP_OUTPUTS].value[j] = simp_pSum[j] * fac1 - simp_p[ihi][j] * fac2;
	}
	copyDimensions(TOP_INPUTS, TOP_OUTPUTS);
	// evaluate function at Ptry -- wait for results
}

void Simplex_Component::amotryB(double ytry) {
	int j;
	
	int params = input[4].value.size();

	// Evaluate the function at the trial point

	if (ytry < simp_y[ihi]) //  if it's better than the highest point...
	{
		// then replace the highest
		simp_y[ihi] = ytry;

		for (j = 0; j < params; j++) {
			simp_pSum[j] += output[TOP_OUTPUTS].value[j] - simp_p[ihi][j];
			simp_p[ihi][j] = output[TOP_OUTPUTS].value[j];
		}
	}
}

void Simplex_Component::contract() // contract around lowest point
{
	int j, ii;
	int params = input[4].value.size();

	for (ii = 0; ii <= params; ii++) {
		if (ii != ilo) {
			for (j = 0; j < params; j++) {
				simp_p[ii][j] = 0.5 * (simp_p[ii][j] + simp_p[ilo][j]);
				// from book, not in legacy program:  simp_pSum[j] = simp_p[ii][j];
			}
		}
	}

	// all the verts have to be reevaluated at this point - start the ball rolling
	lines = 0; // first vertex
	for (j = 0; j < params; j++) {
		output[TOP_OUTPUTS].value[j] = simp_p[0][j];// todo is this right
	}
}

void Simplex_Component::capture(double value) {
	int j;
	int params = input[4].value.size();

	// remember the new point's value
	//doubles[DOUBLES_Y+shorts[lines]] = function;
	simp_y[lines] = value;

	// set the outputs for the next value
	for (j = 0; j < params; j++) {
		output[TOP_OUTPUTS].value[j] = simp_p[lines][j];
	}

}

bool Simplex_Component::clear() {
	int j;
	int params = input[4].value.size();
	//vector<double> value; //MAX_SIMPLEX_PARAM_LIMIT];
	
	output[0].value[0] = 0; // done = false
	output[0].dimensions[0] = 1;

	phase = PHASE_INITIAL_PROBE; // use this for the phase

	simp_y.resize(params + 2);
	shorts.resize(params + 2);
	for (j = 0; j <= params + 1; j++) { // java script try params not MAX_SIMPLEX_PARAM_LIMIT
		simp_y[j] = 0.0;	  // clear the function results
		shorts[j] = false;    // clear the flag
	}

	// pass the inputs to the outputs
	bool change = false;
	if (output[TOP_OUTPUTS].value.size() != params) {
		output[TOP_OUTPUTS].value.resize(params);
		change = true;
	}
	//value.resize(params);
	for (j = 0; j < params; j++) {
		double value = 0.0;
		if (input[TOP_INPUTS].drivenByComponent != -1)
			value = input[TOP_INPUTS].value[j];
		if (output[TOP_OUTPUTS].value[j] != value) {
			output[TOP_OUTPUTS].value[j] = value; // output
			change = true;
		}
	}
	if (change)
		copyDimensions(TOP_INPUTS, TOP_OUTPUTS);
	lines = -1;
	return change;
}

int Simplex_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	int params = input[4].value.size();
	if (params < 1)
		return false;

	if (input[0].drivenByComponent == -1)
		return false;
	if (input[0].value.size() < 1)
		return false;
	double func = input[0].value[0];

	if (input[4].drivenByComponent == -1)
		return false;

	int reset = 0;
	if (input[3].drivenByComponent != -1)
		reset = input[3].value[0];

	if (simp_p.size() != params + 2) {
		initialSetup(func);
		reset = 1;//pg->dgrm.cL[i].phase = PHASE_INITIAL_PROBE;
	}

	if (reset || (pg->systemTime == 0.0)) {
		memory[0] = pg->systemTime;
		return clear();
	}

	if (input[2].drivenByComponent != -1) // check for Hold
		if (input[2].value[0])
			return false;

	if (pg->systemTime == 0) {
		if (phase != PHASE_DONE) {
			for (int j = 0; j < params; j++)
				output[TOP_OUTPUTS].value[j] = input[TOP_INPUTS].value[j];
			copyDimensions(TOP_INPUTS, TOP_OUTPUTS);
		}
	}
	else // clock changed
	{
		if (memory[0] == pg->systemTime) // when clock does NOT tick
			return false;

		memory[0] = pg->systemTime;
		bool done = false;
		do {
			switch (phase) {
			case PHASE_INITIAL_PROBE:
				done = initialSetup(func);
				if (!done)
					break;

				// ok now we have a simplex to play with
				getPsum();

			case PHASE_GO_AGAIN:
				findBestAndWorst();
				if (checkTolerance()) {
					// we are done -- error is below tolerance
					phase = PHASE_DONE;
					break;
				}
				// else just fall into 100 A
				// begin new iteration
				// reflect from high point
				amotryA(-1.0); // 100 A
				phase = PHASE_100_B;
				break;

			case PHASE_100_B:	// got results of new iteration in function
				amotryB(func); // 100 B
				if (func < simp_y[ilo]) {
					// new result is better than best point, so try an additional extrapolation by factor of 2
					amotryA(2.0); // 200 A
					phase = PHASE_200_B;
				}
				else if (func >= simp_y[inhi]) {
					// the new result is worse than the second highest, so look for an intermediate lower point
					// i.e. do a one dimensional contraction
					ySave = simp_y[ihi];
					amotryA(0.5); // 300 A
					phase = PHASE_300_B;
				}
				else
					phase = PHASE_GO_AGAIN;
				break;

			case PHASE_200_B:
				amotryB(func); // 200 B
				phase = PHASE_GO_AGAIN;
				break;

			case PHASE_300_B:
				amotryB(func); // 300 B
				// ysave is part[i].object.setting
				if (func >= ySave) {
					contract(); // contract around lowest point
					// all the verts have to be reevaluated at this point
					lines = 0;
					phase = PHASE_EVALUATE;
				}
				else
					phase = PHASE_GO_AGAIN;
				break;

			case PHASE_EVALUATE:
				capture(func);
				lines++;
				if (lines >= params) {
					getPsum();
					phase = PHASE_GO_AGAIN;
				}
				break;

			case PHASE_DONE:
				break;
			}
		} while (phase == PHASE_GO_AGAIN);

	}
	return true;
}


Simplex_Component::Simplex_Component()
	: Component(L"Simp", L"Simplex Solver", squareShape, 6, 1, 3, 1)
{
	setLeftLabels(3, "Feedback", "Tolerance", "Hold", "Reset", "Params", "Deltas");
	setRightLabels(3, "Done", "Error", "Params");
	width = 50;
	timeBased = true;
	simp_y.resize(1);
	simp_y[0] = 0.0;	// clear the function results
	shorts.resize(1);
	shorts[0] = false;	// clear the flag
	phase = PHASE_INITIAL_PROBE; // use this for the phase
	lines = -1;
	setMemory(0, 0); // system time
}

// *********************** OneEdge = Radio *******************************

int OneEdge_Component::method2(Sheet* pg) {
	// works like a radio button, edge triggers the output
	if (!checkValueInputs())
		return clearOutputs();

	int pins = setting[0];
	bool changed = false;

	if (output[1].value.size() > pins) { // shrinking
		output[1].value.resize(pins);
		memory.resize(pins);
		if (output[0].value[0] >= pins)
			output[0].value[0] = -1;
		changed = true;
	}
	else if (output[1].value.size() < pins) { // expanding
		output[1].value.resize(pins);
		memory.resize(pins);
		for (int j = output[1].value.size(); j < pins; j++) {
			output[1].value[j] = 0;
			memory[j] = 0;
		}
		changed = true;
	}

	for (int j = 0; j < pins; j++) {
		// look for rising edge
		if ((input[j].value[0] > 0) && (memory[j] <= 0)) {
			int old = output[0].value[0];
			if( old >= 0 )
				if (old < pins)
					output[1].value[old] = 0; // clear old list
			output[0].value[0] = j; // set new index
			output[1].value[j] = 1; // set new list
			changed = true;
		}
		memory[j] = input[j].value[0];
	}
	return changed;
}

void OneEdge_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
bool OneEdge_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, MULTI_PIN_MAX);
}
bool OneEdge_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	return true;
}
OneEdge_Component::OneEdge_Component()
	: Component(L"OneEdge", L"Radio", squareShape, 4, 1, 2, 1)
{
	setSetting(0, 4);
	setMemory(0, 0);
	setMemory(1, 0);
	setMemory(2, 0);
	setMemory(3, 0);
	output[0].value[0] = -1; // index
	output[1].value = { 0, 0, 0, 0 }; // list
	setLeftLabels(3, "0", "1", "2", "3");
	setRightLabels(3, "Index", "List");
}

// *********************** AnyEdge = Toggle ****************************


int AnyEdge_Component::method2(Sheet* pg) {
	// works like a checkboxes, edge toggles the output
	if (!checkValueInputs())
		return clearOutputs();

	int pins = setting[0];
	bool changed = false;

	if (output[0].value.size() > pins) {
		output[0].value.resize(pins);
		memory.resize(pins);
		changed = true;
	}
	else if (output[0].value.size() < pins) {
		output[0].value.resize(pins);
		memory.resize(pins);
		for (int j = output[0].value.size(); j < pins; j++) {
			output[0].value[j] = 0;
			memory[j] = 0;
		}
		changed = true;
	}

	for (int j = 0; j < pins; j++) {
		// look for rising edge
		if ((input[j].value[0] > 0) && (memory[j] <= 0)) {
			if (output[0].value[j] == 0) // toggle it
				output[0].value[j] = 1; // set
			else
				output[0].value[j] = 0; // clear
			changed = true;
		}
		memory[j] = input[j].value[0];
	}
	return changed;
}

void AnyEdge_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
bool AnyEdge_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, MULTI_PIN_MAX);
}
bool AnyEdge_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n); 

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	return true;
}
AnyEdge_Component::AnyEdge_Component()
	: Component(L"AnyEdge", L"Toggle", squareShape, 4, 1, 1, 1)
{
	setSetting(0, 4);
	setMemory(0, 0);
	setMemory(1, 0);
	setMemory(2, 0);
	setMemory(3, 0);
	output[0].value = { 0, 0, 0, 0 }; // list
	setLeftLabels(3, "0", "1", "2", "3");
	setRightLabels(3, "List");
}

// ****************** compare *******************



int Compare_Component::method2(Sheet* pg) {

	int inputCount = input[0].value.size();
	int patternCount = input[1].stringValue.size();

	output[0].value[0] = -1;
	output[1].value.clear();

	if( (inputCount == 0) || (patternCount == 0) )
		return true;

	if( output[1].value.size() != patternCount )
		output[1].value.resize(patternCount);

	for (int j = 0; j < patternCount; j++) {
		output[1].value[j] = 0;
		
		string pattern = input[1].stringValue[j]; // get the pattern string
		if (pattern == "")
			continue;
		
		// check for ANY or ALL
		bool allMode = true; // default
		if ((pattern.substr(0, 4) == "ANY ") ||
			(pattern.substr(0, 4) == "Any ") ||
			(pattern.substr(0, 4) == "any ")) {
			allMode = false;
			pattern = pattern.substr(4);
		}
		else {
			if ((pattern.substr(0, 4) == "ALL ") ||
				(pattern.substr(0, 4) == "All ") ||
				(pattern.substr(0, 4) == "all "))
				pattern = pattern.substr(4);
		}
		if (pattern.size() != inputCount)
			continue;
		int tests = inputCount;
		bool match;
		if (allMode) { // all must match
			match = true; // hopefully
			for (int k = 0; k < tests; k++) {
				switch (pattern[k]) {
				case 'X':
				case 'x':
					break;
				case '0': // must be false
				case 'F': // must be false
				case 'f': // must be false
					if (input[0].value[k] != 0)
						match = false;
					break;
				case '1': // must be true
				case 'T': // must be true
				case 't': // must be true
					if (input[0].value[k] == 0)
						match = false;
					break;
				
				default: // anything else is no match
					match = false;
					break;
				}
				if (!match)
					break;
			}
		}
		else { // any match mode
			match = false;
			for (int k = 0; k < tests; k++) {
				switch (pattern[k]) {
				case '0': // must be false
				case 'F': // must be false
				case 'f': // must be false
					if (input[0].value[k] == 0)
						match = true;
					break;
				case '1': // must be true
				case 'T': // must be true
				case 't': // must be true
					if (input[0].value[k] != 0)
						match = true;
					break;
				default: // anything else is "don't care"
					break;
				}
				if (match)
					break;
			}
		}
		if (match && (output[0].value[0] == -1))
			output[0].value[0] = j; // output the first match only
		if (match)
			output[1].value[j] = 1;
	}
	setSimpleOutputDimensions();
	return true;
}


Compare_Component::Compare_Component()
	: Component(L"Compare", L"Compare", squareShape, 2, 1, 2, 1)
{
	setLeftLabels(3, "Array", "Pattern");
	setRightLabels(3, "First Match", "Match Array");
	width = 60;
	output[0].value[0] = 0;
	output[0].dimensions[0] = 1;
}

// ***************************** Sunrise ************************************

double toRad(double angle) {
	return angle * DEGREES_TO_RADIANS;
}
double toDeg(double angle) {
	return angle * RADIANS_TO_DEGREES;
}

void Sunrise_Component::sunCalc(int julianDay, double lat, double lon, int timeZone) {
	double G2 = (julianDay - 2451545) / 36525; // Julian Century
	double I2 = (int)(280.46646 + G2 * (36000.76983 + G2 * 0.0003032)) % 360; // Geom Mean Long Sun (deg)
	double J2 = 357.52911 + G2 * (35999.05029 - 0.0001537 * G2); // Geom Mean Anom Sun (deg)
	double K2 = 0.016708634 - G2 * (0.000042037 + 0.0000001267 * G2); // Eccent Earth Orbit
	double L2 = sin(toRad(J2)) * (1.914602 - G2 * (0.004817 + 0.000014 * G2)) + sin(toRad(2 * J2)) * (0.019993 - 0.000101 * G2) + sin(toRad(3 * J2)) * 0.000289; // Sun Eq of Ctr
	double M2 = I2 + L2; // Sun True Long (deg)
	double P2 = M2 - 0.00569 - 0.00478 * sin(toRad(125.04 - 1934.136 * G2)); // Sun App Long (deg)
	double Q2 = 23 + (26 + ((21.448 - G2 * (46.815 + G2 * (0.00059 - G2 * 0.001813)))) / 60) / 60; // Mean Obliq Ecliptic (deg)
	double R2 = Q2 + 0.00256 * cos(toRad(125.04 - 1934.136 * G2)); // Obliq Corr (deg)
	double T2 = toDeg(asin(sin(toRad(R2)) * sin(toRad(P2)))); // Sun Declin (deg)
	double U2 = tan(toRad(R2 / 2)) * tan(toRad(R2 / 2)); // var y
	double V2 = 4 * toDeg(U2 * sin(2 * toRad(I2)) - 2 * K2 * sin(toRad(J2)) + 4 * K2 * U2 * sin(toRad(J2)) * cos(2 * toRad(I2)) - 0.5 * U2 * U2 * sin(4 * toRad(I2)) - 1.25 * K2 * K2 * sin(2 * toRad(J2))); // Eq of Time (minutes)
	double W2 = toDeg(acos(cos(toRad(90.833)) / (cos(toRad(lat)) * cos(toRad(T2))) - tan(toRad(lat)) * tan(toRad(T2)))); // HA Sunrise (deg)
	double X2 = (720 - 4 * lon - V2 + timeZone * 60) / 1440; // Solar Noon (LST)
	double Y2 = X2 - W2 * 4 / 1440; // Sunrise Time (LST)
	double Z2 = X2 + W2 * 4 / 1440; // Sunset Time (LST)

	noon = X2;
	sunrise = Y2;
	sunset = Z2;
	return;
}

int Sunrise_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	
	output[0].value.resize(loops);
	output[1].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		double lat = getLoopingValueD(0, k, 0);
		double lon = getLoopingValueD(1, k, 0);
		int month = floor(getLoopingValueD(2, k, 1));
		int day = floor(getLoopingValueD(3, k, 1));
		int year = getLoopingValueD(4, k, 2000);
		int timeZone = floor(getLoopingValueD(5, k, 0));

		int a = floor((14 - month) / 12);
		int y = year + 4800 - a;
		int m = month + 12 * a - 3;
		int JDN = day + floor((153 * m + 2) / 5) + 365 * y + floor(y / 4) - floor(y / 100) + floor(y / 400) - 32045;

		double E2 = 0.1 / 24.0;
		double F2 = JDN + E2 - timeZone / 24 - 0.5; // Julian Day

		sunCalc(F2, lat, lon, timeZone);
		double SolarNoon = noon;
		double SolarSunrise = sunrise;
		double SolarSunset = sunset;

		// move to sunrise
		F2 = JDN + SolarSunrise - timeZone / 24 - 0.5;
		sunCalc(F2, lat, lon, timeZone);
		double SunriseRise = sunrise;

		// move to sunset
		F2 = JDN + SolarSunset - timeZone / 24 - 0.5;
		sunCalc(F2, lat, lon, timeZone);
		double SunsetSet = sunset;

		output[0].value[k] = SunriseRise;
		output[1].value[k] = SunsetSet;
	}
	return true;
}


Sunrise_Component::Sunrise_Component()
	: Component(L"Sunrise", L"Sunrise", squareShape, 6, 1, 2, 1)
{
	setLeftLabels(3, "Latitude (deg)", "Longitude(deg)", "Month", "Day", "Year", "TimeZone");
	setRightLabels(3, "Sunrise", "Sunset");
	width = 50;
}


// *************************** sun position **************************************





void SunPosition_Component::sunPosition(double julianDay, double lat, double lon, int timeZone, double E2) {
	double G2 = (julianDay - 2451545) / 36525; // Julian Century
	double I2 = (int)(280.46646 + G2 * (36000.76983 + G2 * 0.0003032)) % 360; // Geom Mean Long Sun (deg)
	double J2 = 357.52911 + G2 * (35999.05029 - 0.0001537 * G2); // Geom Mean Anom Sun (deg)
	double K2 = 0.016708634 - G2 * (0.000042037 + 0.0000001267 * G2); // Eccent Earth Orbit
	double L2 = sin(toRad(J2)) * (1.914602 - G2 * (0.004817 + 0.000014 * G2)) + sin(toRad(2 * J2)) * (0.019993 - 0.000101 * G2) + sin(toRad(3 * J2)) * 0.000289; // Sun Eq of Ctr
	double M2 = I2 + L2; // Sun True Long (deg)
	double P2 = M2 - 0.00569 - 0.00478 * sin(toRad(125.04 - 1934.136 * G2)); // Sun App Long (deg)
	double Q2 = 23 + (26 + ((21.448 - G2 * (46.815 + G2 * (0.00059 - G2 * 0.001813)))) / 60) / 60; // Mean Obliq Ecliptic (deg)
	double R2 = Q2 + 0.00256 * cos(toRad(125.04 - 1934.136 * G2)); // Obliq Corr (deg)
	double T2 = toDeg(asin(sin(toRad(R2)) * sin(toRad(P2)))); // Sun Declin (deg)
	double U2 = tan(toRad(R2 / 2)) * tan(toRad(R2 / 2)); // var y
	double V2 = 4 * toDeg(U2 * sin(2 * toRad(I2)) - 2 * K2 * sin(toRad(J2)) + 4 * K2 * U2 * sin(toRad(J2)) * cos(2 * toRad(I2)) - 0.5 * U2 * U2 * sin(4 * toRad(I2)) - 1.25 * K2 * K2 * sin(2 * toRad(J2))); // Eq of Time (minutes)
	//var W2 = toDeg(Math.acos(Math.cos(toRad(90.833)) / (Math.cos(toRad(lat)) * Math.cos(toRad(T2))) - Math.tan(toRad(lat)) * Math.tan(toRad(T2)))); // HA Sunrise (deg)
	//    var X2 = (720 - 4 * lon - V2 + timeZone * 60) / 1440; // Solar Noon (LST)
	//    var Y2 = X2 - W2 * 4 / 1440; // Sunrise Time (LST)
	//    var Z2 = X2 + W2 * 4 / 1440; // Sunset Time (LST)

	//   var AA2 = 8*W2; // Sunlight Duration (minutes)
	double AB2 = (int)(E2 * 1440 + V2 + 4 * lon - 60 * timeZone) % 1440; // True Solar Time (min)
	if (AB2 < 0)
		AB2 += 1440;
	double AC2; // Hour Angle (deg)
	if (AB2 / 4 < 0)
		AC2 = AB2 / 4 + 180;
	else
		AC2 = AB2 / 4 - 180;
	double AD2 = toDeg(acos(sin(toRad(lat)) * sin(toRad(T2)) + cos(toRad(lat)) * cos(toRad(T2)) * cos(toRad(AC2)))); // Solar Zenith Angle (deg)
	double AE2 = 90 - AD2; // Solar Elevation Angle (deg)
	double AF2; // Approx Atmospheric Refraction (deg)
	if (AE2 > 85)
		AF2 = 0;
	else if (AE2 > 5)
		AF2 = 58.1 / tan(toRad(AE2)) - 0.07 / pow(tan(toRad(AE2)), 3) + 0.000086 / pow(tan(toRad(AE2)), 5);
	else if (AE2 > -0.575)
		AF2 = 1735 + AE2 * (-518.2 + AE2 * (103.4 + AE2 * (-12.79 + AE2 * 0.711)));
	else AF2 = -20.772 / tan(toRad(AE2));
	AF2 /= 3600;
	double AG2 = AE2 + AF2; // Solar Elevation corrected for atm refraction (deg)
	double AH2;  // Solar Azimuth Angle (deg cw from N)
	if (AC2 > 0)
		AH2 = (int)(toDeg(acos(((sin(toRad(lat)) * cos(toRad(AD2))) - sin(toRad(T2))) / (cos(toRad(lat)) * sin(toRad(AD2))))) + 180) % 360;
	else
		AH2 = (int)(540 - toDeg(acos(((sin(toRad(lat)) * cos(toRad(AD2))) - sin(toRad(T2))) / (cos(toRad(lat)) * sin(toRad(AD2)))))) % 360;

	elevation = AG2;
	azimuth = AH2;
	return;
}

int SunPosition_Component::method2(Sheet* pg) {
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	
	output[0].value.resize(loops);
	output[1].value.resize(loops);


	for (int k = 0; k < loops; k++) {
		double lat = getLoopingValueD(0, k, 0);
		double lon = getLoopingValueD(1, k, 0);
		int month = floor(getLoopingValueD(2, k, 1));
		int day = floor(getLoopingValueD(3, k, 1));
		int year = getLoopingValueD(4, k, 2000);
		int timeZone = floor(getLoopingValueD(5, k, 0));
		int hour = getLoopingValueD(6, k, 12); // noon

		int a = floor((14 - month) / 12);
		int y = year + 4800 - a;
		int m = month + 12 * a - 3;
		int JDN = day + floor((153 * m + 2) / 5) + 365 * y + floor(y / 4) - floor(y / 100) + floor(y / 400) - 32045;

		double E2 = hour / 24;
		double F2 = JDN + E2 - timeZone / 24 - 0.5; // Julian Day

		sunPosition(F2, lat, lon, timeZone, E2);

		output[0].value[k] = azimuth;
		output[1].value[k] = elevation;
	}
	return true;
}


SunPosition_Component::SunPosition_Component()
	: Component(L"SunPos", L"Sun Position", squareShape, 7, 1, 2, 1)
{
	setLeftLabels(3, "Latitude (deg)", "Longitude(deg)", "Month", "Day", "Year", "TimeZone", "Hour");
	setRightLabels(3, "Asimuth", "Elevation");
	width = 50;
}

//********************************** Moon Phase *********************************


	// Moon Phase calc from http://www.ben-daglish.net/moon.shtml
int julday(int year, int month, int day) {
	if (year < 0) {
		year++;
	}
	int jy = year;
	int jm = month + 1;
	if (month <= 2) {
		jy--;
		jm += 12;
	}
	int jul = floor(365.25 * jy) + floor(30.6001 * jm) + day + 1720995;
	if (day + 31 * (month + 12 * year) >= (15 + 31 * (10 + 12 * 1582))) {
		int ja = floor(0.01 * jy);
		jul = jul + 2 - ja + floor(0.25 * ja);
	}
	return jul;
}



int Trig2(int year, int month, int day) {
	int n = floor(12.37 * (year - 1900 + ((1.0 * month - 0.5) / 12.0)));
	double RAD = 3.14159265 / 180.0;
	double t = n / 1236.85;
	double t2 = t * t;
	double as = 359.2242 + 29.105356 * n;
	double am = 306.0253 + 385.816918 * n + 0.010730 * t2;
	double xtra = 0.75933 + 1.53058868 * n + ((1.178e-4) - (1.55e-7) * t) * t2;
	xtra += (0.1734 - 3.93e-4 * t) * sin(RAD * as) - 0.4068 * sin(RAD * am);
	int i = (xtra > 0.0 ? floor(xtra) : ceil(xtra - 1.0));
	int j1 = julday(year, month, day);
	int jd = (2415020 + 28 * n) + i;
	return (j1 - jd + 30) % 30;
}



int MoonPhase_Component::method2(Sheet* pg) {
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		int month = floor(getLoopingValueD(0, k, 1));
		int day = floor(getLoopingValueD(1, k, 1));
		int year = getLoopingValueD(2, k, 2000);
		output[0].value[k] = Trig2(year, month, day);
	}
	return true;
}


MoonPhase_Component::MoonPhase_Component()
	: Component(L"MoonPhase", L"Moon Phase", 3, 1)
{
	setLeftLabels(3, "Month", "Day", "Year", "TimeZone", "Hour");
}

//********************************** Random Position *********************************



int RandPos_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	bool trigger = false;
	if (input[4].value[0]) {
		if (memory[0] == 0)
			trigger = true;
		memory[0] = 1;
	}
	else
		memory[0] = 0;
	if (!trigger)
		return false;

	int dims = setting[0];
	int points = input[0].value[0];
	if (points < 0)
		return clearOutputs();
	if (points > 10000)
		points = 10000;

	int successPin = dims;
	bool change = false;

		// gather info
	double safetyFactor = 1;
	switch (dims) {
	case 1:	safetyFactor = 4;	break;
	case 2:	safetyFactor = 0.4;	break;
	case 3:	safetyFactor = 0.04;	break;
	}

	vector<double> min;
	vector<double> max;
	vector<double> rad;
	min.resize(dims);
	max.resize(dims);
	rad.resize(dims);

	for (int d = 0; d < dims; d++) {
		min[d] = getLoopingValueD(1, d, 0);
		max[d] = getLoopingValueD(2, d, 0);
		rad[d] = getLoopingValueD(3, d, 0);
		if (min[d] >= max[d]) {
			output[successPin].value[0] = 0;
			return true;
		}
		if (rad[d] < 0) { // zero is ok
			output[successPin].value[0] = 0;
			return true;
		}
		if ((max[d] - min[d]) < 2 * rad[d] * points * safetyFactor) {
			output[successPin].value[0] = 0;
			return true;
		}
	
		output[d].value.resize(points);
	}

	// here we go
	int attempts = 0;
	vector<vector<double>> pos;
	pos.resize(points);
	bool overlap;
	for (int k = 0; k < points; k++) {
		pos[k].resize(dims);
		do {
			for (int d = 0; d < dims; d++)
				pos[k][d] = ((double)rand() / (double)RAND_MAX) * (max[d] - min[d] - 2 * rad[d]) + min[d]+ rad[d];

			overlap = false;
			if (k > 0) {
				// check for collision
				for (int j = 0; j < k; j++) {
					// see if these two (j and k) are not touching in any dimension
					bool touching = true;
					for (int d = 0; d < dims; d++) {
						if (abs(pos[k][d] - pos[j][d]) > 2 * rad[d]) {
							touching = false;
							break;
						}
					}
					if (touching) {
						overlap = true;
						break;
					}
				}
				if (overlap) 
					attempts++;
			}
		} while (overlap && attempts < 100);
		for (int d = 0; d < dims; d++)
			output[d].value[k] = pos[k][d];
	}
	change = true;
	if (attempts < 100)
		output[successPin].value[0] = 1;
	else
		output[successPin].value[0] = 0;
	setSimpleOutputDimensions();

	return change;
}

bool RandPos_Component::onLoad(Sheet* pg) {

	if (setting[0] > 3)
		setting[0] = 3;

	int n = setting[0];
	int outputs = n + 1;
	buildOutputPins(n+1);

	int k = 0;
	string letter[3] = { "Xs", "Ys", "Zs" };
	for (int j = 0; j < n; j++) {
		output[k++].label = letter[j];
	}
	output[k++].label = "Success";
	return true;
}

void RandPos_1Dimension(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 1;
	pg->activeDialogComponent = -1;
	pg->dgrm.cL[i]->onLoad(pg);
}
void RandPos_2Dimension(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 2;
	pg->activeDialogComponent = -1;
	pg->dgrm.cL[i]->onLoad(pg);
}
void RandPos_3Dimension(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 3;
	pg->activeDialogComponent = -1;
	pg->dgrm.cL[i]->onLoad(pg);
}

std::vector<Button*>RandPosButtonList = { new Button(-80, 40, 125, 20, L"Set 1 Dimension", RandPos_1Dimension),
	new Button(-80, 60, 125, 20, L"Set 2 Dimensions", RandPos_2Dimension),
	new Button(-80, 80, 125, 20, L"Set 3 Dimensions", RandPos_3Dimension) };

RandPos_Component::RandPos_Component()
	: Component(L"RandPos", L"Random Position", squareShape, 5, 1, 2, 1)
{
	setLeftLabels(3, "Positions", "Minimum", "Maximum", "Radius", "Trigger");
	dialogButtonList = RandPosButtonList;
	setSetting(0, 1); // dimensions
	setMemory(0, 0); // trigger edge
	setMemory(1, 0); // save dims
	setMemory(2, 0); // save points
	width = 55;
}


// *************** Multiplexer *******************

int Multiplexer_Component::method2(Sheet* pg) {

	if (input[2].stringType || (input[2].value.size() == 0))
		return clearOutputs();

	if (input[2].value.size() == 1) { // single control input
		
		int control = 0;
		if (input[2].value[0]) // simple selection
			control = 1;

		if (input[control].stringType) {
			int size = input[control].stringValue.size();
			output[0].stringValue.resize(size);
			output[0].value.clear();
			output[0].stringType = true;
			for (int j = 0; j < size; j++)
				output[0].stringValue[j] = input[control].stringValue[j];
		}
		else {
			int size = input[control].value.size();
			output[0].value.resize(size);
			output[0].stringValue.clear();
			output[0].stringType = false;
			for (int j = 0; j < size; j++)
				output[0].value[j] = input[control].value[j];
		}
		copyDimensions(control, 0);
		return 1;
	}
	
	// more complex if the control is also an array
	// need to choose an output type
	// for now lets use the more common type
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	if (input[0].stringType != input[1].stringType)
		return clearOutputs();

	bool outputIsString = input[0].stringType;
	
	if (outputIsString) {
		output[0].stringValue.resize(loops);
		output[0].value.clear();
		output[0].stringType = true;

		for (int j = 0; j < loops; j++) {
			if( getLoopingValueD(2, j, 0) <= 0 )
				output[0].stringValue[j] = getLoopingStringValueD(0, j, "");
			else
				output[0].stringValue[j] = getLoopingStringValueD(1, j, "");
		}
	}
	else // output is numbers
	{
		output[0].value.resize(loops);
		output[0].stringValue.clear();
		output[0].stringType = false;

		for (int j = 0; j < loops; j++) {
			if (getLoopingValueD(2, j, 0) <= 0)
				output[0].value[j] = getLoopingValueD(0, j, 0);
			else
				output[0].value[j] = getLoopingValueD(1, j, 0);
		}
	}
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = loops;
	return 1;
}
Multiplexer_Component::Multiplexer_Component()
	: Component(L"Mux", L"Multiplexer", 3, 1)
{
	setLeftLabels(3, "0", "1", "Control");
}

// ************************* triangle area ************************************

int TriangleArea_Component::method2(Sheet *pg) {
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);

	for (int j = 0; j < loops; j++) {
		double Ax= getLoopingValueD(0, j, 0);
		double Ay= getLoopingValueD(1, j, 0);
		double Bx= getLoopingValueD(2, j, 0);
		double By= getLoopingValueD(3, j, 0);
		double Cx= getLoopingValueD(4, j, 0);
		double Cy= getLoopingValueD(5, j, 0);

		output[0].value[j] = abs(Ax * (By - Cy) + Bx * (Cy - Ay) + Cx * (Ay - By)) / 2;
	}
	copyDimensions(0, 0);

	return 1;
}
TriangleArea_Component::TriangleArea_Component()
	: Component(L"TriArea", L"Triangle Area", squareShape, 6, 1, 1, 1)
{
	setLeftLabels(3, "X0", "Y0", "X1", "Y1", "X2", "Y2");
	setSetting(0, 2);  // dimensions (for later)
}

//*********************** distance point to line *******************************

int PointToLine_Component::method2(Sheet *pg) {
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	output[1].value.resize(loops);
	output[2].value.resize(loops);

	for (int j = 0; j < loops; j++) {
		double PointX = getLoopingValueD(0, j, 0);
		double PointY = getLoopingValueD(1, j, 0);
		double LineX0 = getLoopingValueD(2, j, 0);
		double LineY0 = getLoopingValueD(3, j, 0);
		double LineX1 = getLoopingValueD(4, j, 0);
		double LineY1 = getLoopingValueD(5, j, 0);

		double dx = LineX1 - LineX0;
		double dy = LineY1 - LineY0;
		if (dx == 0 && dy == 0) {
			output[0].value[j] = LineX1;
			output[1].value[j] = LineY1;
			output[2].value[j] = 0;
		}
		else {
			double slope0 = (LineY1 - LineY0) / (LineX1 - LineX0);
			double intercept0 = LineY0 - (slope0 * LineX0);
			double slope1 = -1 / slope0;
			double intercept1 = PointY - (slope1 * PointX);
			double xOut = (intercept1 - intercept0) / (slope0 - slope1);
			double yOut = xOut * slope0 + intercept0;
			output[0].value[j] = xOut;
			output[1].value[j] = yOut;

			double lineLength = sqrt(dx * dx + dy * dy);
			double triangleArea = abs(PointX * (LineY0 - LineY1) + LineX0 * (LineY1 - PointY) + LineX1 * (PointY - LineY0)) / 2;
			output[2].value[j] = 2.0 * triangleArea / lineLength;
		}
	}
	copyDimensions(0, 0);

	return 1;
}
PointToLine_Component::PointToLine_Component()
	: Component(L"Point2Line", L"Point to Line", squareShape, 6, 1, 3, 1)
{
	setLeftLabels(3, "PointX", "PointY", "LineX0", "LineY0", "LineX1", "LineY1");
	setRightLabels(3, "X", "Y", "Distance");
	width = 50;
	setSetting(0, 2);  // dimensions (for later)
}

// ********************************* Input **********************************




int nextInOutNumber(Sheet* pg, LPCTSTR theType) {

	int n = 0;
	bool found;
	do {
		found = false;
		for (int i = 0; i < pg->dgrm.cL.size(); i++) {
			if (! z(pg->dgrm.cL[i]->type, theType))
				continue; // continue looping
			if (pg->dgrm.cL[i]->setting[0] == n) {
				found = true;
				break; // found this n, break out of for loop
			}
		}
		if (!found)
			return n;
		n++;
	} while (found);
	return -1;
}

void Input_Component::dial(Sheet* pg) {
	int yy = y - height + 5;

	string text = literal[0];
	int k = text.size();
	bool showLineEllipsis = false;
	while (calcStringWidthFont(pg->hDC, text) > 2 * width - 15) {
		k--;
		showLineEllipsis = true;
		text = literal[0].substr(0, k);
	}
	if (showLineEllipsis)
		text += "...";

	int xx = x - calcStringWidthFont(pg->hDC, text) / 2;
	drawTextString(pg->hDC, pg->cXo + xx, pg->cYo + yy + 4, 0, text);

	if ( literal[0] != tomString(setting[0])) 
		drawTextString(pg->hDC, pg->cXo + x + 20, pg->cYo + y + 5, 0, tomString(setting[0]));
}



bool Input_Component::onLoad(Sheet* pg) {
	output[0].value[0] = setting[1]; // text value
	return true;
}
void doubleClickInput(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildInputDialogButtons(pg, i);
}
std::vector<Button*> inputButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickInput) };

Input_Component::Input_Component(Sheet* pg)
	: Component(L"Input", L"Input", diamondShape, 0, 0, 1, 1)
{
	dialogButtonList = inputButtonList;
	height /= 2;
	setSetting(0, nextInOutNumber(pg, L"Input"));
	setSetting(1, 0); // default value
	setLiteral(0, tomString(setting[0]));
	setOutputDimension(0, 0, 1);
}

// ***************************** Output ********************************


void Output_Component::dial(Sheet* pg) {
	int yy = y - height + 5;

	string text = literal[0];
	int k = text.size();
	bool showLineEllipsis = false;
	while (calcStringWidthFont(pg->hDC, text) > 2 * width - 15) {
		k--;
		showLineEllipsis = true;
		text = literal[0].substr(0, k);
	}
	if (showLineEllipsis)
		text += "...";

	int xx = x - calcStringWidthFont(pg->hDC, text) / 2;
	drawTextString(pg->hDC, pg->cXo + xx, pg->cYo + yy + 4, 0, text);

	if (literal[0] != tomString(setting[0]))
		drawTextString(pg->hDC, pg->cXo + x - 30, pg->cYo + y + 5, 0, tomString(setting[0]));
}

void doubleClickOutput(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildOutputDialogButtons(pg, i);
}
std::vector<Button*> outputButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickOutput) };

Output_Component::Output_Component(Sheet* pg)
	: Component(L"Output", L"Output", diamondShape, 1, 1, 0, 0)
{
	dialogButtonList = outputButtonList;
	height /= 2;
	setSetting(0, nextInOutNumber(pg, L"Output"));
	setLiteral(0, tomString(setting[0]));
}


// ******************************** Sheet Component *********************************************


void Sheet_Component::setPinsHeightTime() {

	bool timeBased = false;
	pinInputComp.clear();
	pinOutputComp.clear();

	for (int i = 0; i < idgrm->cL.size(); i++) {

		// check for time based components
		if (idgrm->cL[i]->timeBased)
			timeBased = true;

		// grab up inputs
		if (z(idgrm->cL[i]->type, L"Input"))
			pinInputComp.push_back(i);

		// grab up the outputs
		if (z(idgrm->cL[i]->type, L"Output"))
			pinOutputComp.push_back(i);
	}
	leftTerminalCount = pinInputComp.size();
	rightTerminalCount = pinOutputComp.size();
	timeBased = timeBased;
	height = pinToHeight(max(leftTerminalCount, rightTerminalCount) + 2); // leave room at top and botton

	// sort them
	bool sorted;
	do {
		sorted = true; // always optimistic
		for (int jin = 1; jin < leftTerminalCount; jin++) {
			if (idgrm->cL[pinInputComp[jin - 1]]->setting[0] >
				idgrm->cL[pinInputComp[jin]]->setting[0]) {
				// swap them
				int temp = pinInputComp[jin - 1];
				pinInputComp[jin - 1] = pinInputComp[jin];
				pinInputComp[jin] = temp;
				sorted = false;
			}
		}
	} while (!sorted);

	// set the labels
	input.resize(leftTerminalCount);
	for (int jin = 0; jin < leftTerminalCount; jin++) {
		input[jin].label = idgrm->cL[pinInputComp[jin]]->literal[0];
		input[jin].value.resize(1);
		input[jin].value[0] = 0;
		input[jin].dimensions.resize(1);
		input[jin].dimensions[0] = 0;
		input[jin].drivenByComponent = -1;
		input[jin].drivenByOutputTerminal = -1;
	}

	// sort them
	do {
		sorted = true; // always optimistic
		for (int jout = 1; jout < rightTerminalCount; jout++) {
			if (idgrm->cL[pinOutputComp[jout - 1]]->setting[0] >
				idgrm->cL[pinOutputComp[jout]]->setting[0]) {
				// swap them
				int temp = pinOutputComp[jout - 1];
				pinOutputComp[jout - 1] = pinOutputComp[jout];
				pinOutputComp[jout] = temp;
				sorted = false;
			}
		}
	} while (!sorted);

	// set the labels
	output.resize(rightTerminalCount);
	for (int jout = 0; jout < rightTerminalCount; jout++) {
		output[jout].label = idgrm->cL[pinOutputComp[jout]]->literal[0];
		output[jout].value.resize(1);
		output[jout].value[0] = 0;
		output[jout].dimensions.resize(1);
		output[jout].dimensions[0] = 0;
	}
}


void Sheet_Component::loadSheetComponent() {
	
	if (idgrm != NULL)
		return;
	int sheetNum = setting[0];
	if (sheetNum <= 0)  // cant load the main page
		return;
	if (sheetNum >= isL.size())
		return;

	// copy the sheet into this component
	idgrm = new Diagram(); //  cXo: 0, cYo: 0, cL: cl, wL: wl, cs: 1, name: ""
//	ipg->name = pgL[sheet].name;
//	string text = sheetToText(sheetNum); // cloning
//	loadFromString(ipg, text, true);
//	ipg_text = text; // save the original so we know when to update



	// duplicate the Components
	int clen = isL[sheetNum]->dgrm.cL.size();
	idgrm->cL.resize(clen);
	for (int i = 0; i < clen; i++) {
		idgrm->cL[i] = buildComponent(page, isL[sheetNum]->dgrm.cL[i]->type, 0);

		// should be moved to a copy constructor
		idgrm->cL[i]->x = isL[sheetNum]->dgrm.cL[i]->x;
		idgrm->cL[i]->y = isL[sheetNum]->dgrm.cL[i]->y;
		idgrm->cL[i]->fillColor = isL[sheetNum]->dgrm.cL[i]->fillColor;

		idgrm->cL[i]->userLabel = isL[sheetNum]->dgrm.cL[i]->userLabel;
		idgrm->cL[i]->displayMode = isL[sheetNum]->dgrm.cL[i]->displayMode;
		idgrm->cL[i]->rotation = isL[sheetNum]->dgrm.cL[i]->rotation;

		int n = isL[sheetNum]->dgrm.cL[i]->setting.size();
		idgrm->cL[i]->setting.resize(n);
		for (int k = 0; k < n; k++)
			idgrm->cL[i]->setting[k] = isL[sheetNum]->dgrm.cL[i]->setting[k];

		n = isL[sheetNum]->dgrm.cL[i]->literal.size();
		idgrm->cL[i]->literal.resize(n);
		for (int k = 0; k < n; k++)
			idgrm->cL[i]->literal[k] = isL[sheetNum]->dgrm.cL[i]->literal[k];

		n = isL[sheetNum]->dgrm.cL[i]->memory.size();
		idgrm->cL[i]->memory.resize(n);
		for (int k = 0; k < n; k++)
			idgrm->cL[i]->memory[k] = isL[sheetNum]->dgrm.cL[i]->memory[k];

		n = isL[sheetNum]->dgrm.cL[i]->element.size();
		idgrm->cL[i]->element.resize(n);
		for (int k = 0; k < n; k++)
			idgrm->cL[i]->element[k] = isL[sheetNum]->dgrm.cL[i]->element[k];

		idgrm->cL[i]->onLoad(page);
	}

	// duplicate the Wires
	int wlen = isL[sheetNum]->dgrm.wL.size();
	for (int i = 0; i < wlen; i++) {
		Wire* newWire = new Wire(isL[sheetNum]->dgrm.wL[i]);
		idgrm->wL.push_back(newWire);
	}

	// analyze it once
	analyzeConnections(idgrm);
	setPinsHeightTime();
}

int Sheet_Component::method2(Sheet* pg) {
	if (idgrm == NULL)
		return 0;

	// grab the values from the input pins on this sheet icon and push them into the sheet's input components inside
	for (int j = 0; j < input.size(); j++) {
		int k = pinInputComp[j];
		
		idgrm->cL[k]->output[0].value.resize(input[j].value.size());
		for (int n = 0; n < input[j].value.size(); n++) {
			if (input[j].drivenByComponent == -1) // is it driven
				idgrm->cL[k]->output[0].value[n] = idgrm->cL[k]->setting[1]; // NO, use the default
			else if( isnan(input[j].value[n]))
				idgrm->cL[k]->output[0].value[n] = idgrm->cL[k]->setting[1]; // use the default
			else
				idgrm->cL[k]->output[0].value[n] = input[j].value[n]; // YES, use the value passed in
		}
		idgrm->cL[k]->output[0].dimensions.resize(input[j].dimensions.size());
		for (int n = 0; n < input[j].dimensions.size(); n++)
			idgrm->cL[k]->output[0].dimensions[n] = input[j].dimensions[n];
	}

	propagateSheetValues(idgrm); // process input to outputs on that sheet

	// pull the result from the sheet's output component and insert it in the output pin
	bool changed = false;
	for (int j = 0; j < output.size(); j++) {
		int k = pinOutputComp[j];
		
		if (output[j].value.size() != idgrm->cL[k]->input[0].value.size()) {
			changed = true; // different lengths
			// copy all the values into a fresh vector
			output[j].value.resize(idgrm->cL[k]->input[0].value.size());
			for (int n = 0; n < idgrm->cL[k]->input[0].value.size(); n++)
				output[j].value[n] = idgrm->cL[k]->input[0].value[n];
		}
		else { // same length check values
			for (int n = 0; n < idgrm->cL[k]->input[0].value.size(); n++)
				if (output[j].value[n] != idgrm->cL[k]->input[0].value[n]) {
					output[j].value[n] = idgrm->cL[k]->input[0].value[n];
					changed = true; // some items different values
				}
		}
		
		if (output[j].dimensions.size() != idgrm->cL[k]->input[0].dimensions.size()) {
			// copy all the dimensions into a fresh vector
			output[j].dimensions.resize(idgrm->cL[k]->input[0].dimensions.size());
			for (int n = 0; n < idgrm->cL[k]->input[0].dimensions.size(); n++)
				output[j].dimensions[n] = idgrm->cL[k]->input[0].dimensions[n];
			changed = true; // dimensions changed
		}
		else {// same length check values
			for (int n = 0; n < idgrm->cL[k]->input[0].dimensions.size(); n++)
				if (output[j].dimensions[n] != idgrm->cL[k]->input[0].dimensions[n]) {
					output[j].dimensions[n] = idgrm->cL[k]->input[0].dimensions[n];
					changed = true; // one dimension changed
				}
		}
	}

	return changed;
}

bool Sheet_Component::onLoad(Sheet* pg) {
	if (setting[1])
		width = setting[1];
	else
		width = COMPONENT_WIDTH;
	loadSheetComponent();
	return true;
}

void Sheet_Component::dial(Sheet* pg) {

	// number at top
	string text = tomString(setting[0]);
	int textWidth = calcStringWidthFont(pg->hDC, text);
	drawTextString(pg->hDC, pg->cXo + x - textWidth/2, pg->cYo + y - height, 0, text);

	// name at the bottom
	text = isL[setting[0]]->name;
	int k = text.size();
	bool showLineEllipsis = false;
	while (calcStringWidthFont(pg->hDC, text.substr(0, k)) > 2 * width - 15) {
		k--;
		showLineEllipsis = true;
		text = isL[setting[0]]->name.substr(0, k);
	}
	if (showLineEllipsis)
		text += "...";

	textWidth = calcStringWidthFont(pg->hDC, text);
	if( showLineEllipsis )
		drawTextString(pg->hDC, pg->cXo + x - width+1, pg->cYo + y + height - 12, 0, text);
	else
		drawTextString(pg->hDC, pg->cXo + x - textWidth / 2, pg->cYo + y + height - 12, 0, text);

	drawResizeBox(pg, this);

}



bool Sheet_Component::crank(Sheet* pg, int mouseX, int mouseY, bool check) {
	int left = x + width - RESIZE_HANDLE_SIZE;
	int top = y + height - RESIZE_HANDLE_SIZE;

	if (check) {
		if (mouseX < left)
			return false; // missed
		if (mouseY < top)
			return false; // missed
	}

	left = x - width;
	width = floor((mouseX - left + RESIZE_HANDLE_SIZE / 2) / 2);

	if (width < 15)
		width = 15;

	setting[1] = width;
	return true;
}
bool Sheet_Component::crank2d(Sheet* pg, int mouseX, int mouseY, bool check) {

	// run all the canvas objects in this sheet
	if (check) {
		for (int i = 0; i < idgrm->cL.size(); i++)
			if (idgrm->cL[i]->ghost == NORMAL_GHOST)
				if (idgrm->cL[i]->crank2d(pg, mouseX, mouseY, check)) {
					innerCrankComponent = i;
					return true;
				}
		return false;
	}

	if(innerCrankComponent >= 0 ){
		int i = innerCrankComponent;
		idgrm->cL[i]->crank2d(pg, mouseX, mouseY, check);
		method2(pg); // push the output
		return true;
	}
	return false;
}

bool Sheet_Component::dial2d(Sheet* pg) {

	if (pg->pCanvasWindow == NULL)
		return false;

	// run all the canvas objects in this sheet
	bool result = false;
	if (idgrm) {
		for (int i = 0; i < idgrm->cL.size(); i++)
			if (idgrm->cL[i]->ghost == NORMAL_GHOST)
				if (idgrm->cL[i]->dial2d(page))
					result = true;
	}
	return result;
}
void Sheet_Component::new3dOjbect() {

	if (page->GLFW_window == NULL)
		return;

	// run all the 3D objects in this sheet
	for (int i = 0; i < idgrm->cL.size(); i++)
		if (idgrm->cL[i]->ghost == NORMAL_GHOST)
			idgrm->cL[i]->new3dOjbect();
		
}

Sheet_Component::Sheet_Component(int sheetNum)
	: Component(L"Sheet", L"Sheet", sheetShape, 0, 1, 0, 1)
{
	setSetting(1, width);
	setSetting(2, 0); // run Sheet once
	idgrm = NULL;
	if (sheetNum != 0) {
		setSetting(0, sheetNum);
		loadSheetComponent();
	}
}
Sheet_Component::~Sheet_Component() {
	if (idgrm == NULL)
		return;

	int size = idgrm->cL.size();
	for (int i = size - 1; i >= 0; i--) {
		delete idgrm->cL[i];
	}
	idgrm->cL.clear();
	
	size = idgrm->wL.size();
	for (int i = size - 1; i >= 0; i--)
		delete idgrm->wL[i];
	idgrm->wL.clear();

	delete idgrm;
	idgrm = 0;
}


// ************************** UTF *************************


void UTF8_Component::dial(Sheet* pg) {
	drawTextString(pg->hDC, x - width + 2, y-6, 0, literal[1]);
}

void openUTFDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildUTFDialogButtons(pg, i);
}
std::vector<Button*> utfButtonList = { 
	new Button(3, 40, 85, 20, L"Options... ", openUTFDialog) };

bool UTF8_Component::onLoad(Sheet* pg) {

	output[0].stringValue[0] = literal[0];
	return true;
}
UTF8_Component::UTF8_Component()
	: Component(L"UTF", L"UTF 8", squareShape, 0, 1, 1, 1)
{
	dialogButtonList = utfButtonList;
	output[0].stringType = true;
	output[0].stringValue.resize(1);
	setLiteral(0, "&960;");
	setLiteral(1, "pi");
}
