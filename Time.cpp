#include "stdafx.h"
#include <winuser.h>
#include "Sheet.h"
#include "Time.h"
#include <string.h>
#include "Text.h"
#include "color.h"
#include "MethodComponents.h"
#include "FunctionComponents.h"
#include "ScreenDialog.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

extern Sheet* page;

TIMERPROC CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	Sheet* testPage;
	//LPCSTR name = "page";
	//testPage = (Sheet*)GetPropA(hWnd, name);

	testPage = reinterpret_cast<Sheet *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	//KillTimer(NULL, TimerId);
	//MessageBox(hWnd, L"Show Widget", L"Widget", MB_OK); // somehow this kills the rest of the actions in this method
	stepClock(testPage);
	propagateValues(&testPage->dgrm);
	drawCanvas();
	return 0;
}

void notifyAllRunners(Sheet* pg) {
	for (int i = 0; i < pg->dgrm.cL.size(); i++)
		if (pg->dgrm.cL[i]->type == L"Running")
			pg->dgrm.cL[i]->method2(pg);
}

void stopClock(Sheet* pg) {
	KillTimer(pg->hWnd, 0); // zero works, using timerId doesn't
	pg->timerID = 0;
	notifyAllRunners(pg);
}
void resetClock(Sheet* pg) {
	pg->systemTime = 0.0;
}
void stepClock(Sheet* pg) {
	// Single Step Timers
	pg->systemTime += pg->timeStep;
}
void startClock(Sheet* pg) {
	pg->timerID = SetTimer(pg->hWnd, 0, (UINT)(pg->timeStep*1000), (TIMERPROC)&TimerProc);
	notifyAllRunners(pg);
}

void displaySystemTime(Sheet* pg) {
	//ctx.scale(1 / pg->cs, 1 / pg->cs);
	int heightoffset = 20;
	HBRUSH frameStyle = GetColorBrush(RGB(0, 0, 0)); // CreateSolidBrush
	HBRUSH fillStyle = GetColorBrush(RGB(255, 255, 255)); // CreateSolidBrush
	SelectObject(pg->hDC, frameStyle);
	SelectObject(pg->hDC, fillStyle);

	RECT aRect;
	aRect.left = 30;
	aRect.top = pg->workspaceHeight - 45;
	aRect.right = 190;
	aRect.bottom = pg->workspaceHeight - 30;

	FillRect(pg->hDC, &aRect, fillStyle);
	FrameRect(pg->hDC, &aRect, frameStyle);

	string text = "time = ";
	text.append(tomString(pg->systemTime));
	SetBkMode(pg->hDC, TRANSPARENT);
	drawTextString(pg->hDC, 32, pg->workspaceHeight - 43, 0, text);

}


//  ********************************* Stop **************************

int Stop_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].value[0]) {
		if (!memory[0])// toggle
		{
			memory[0] = true;
			stopClock(pg);
		}
	}
	else
		memory[0] = false;
	return false;
}
Stop_Component::Stop_Component()
	: Component(L"Stop", L"Stop Clock", squareShape, 1, 1, 0, 0)
{
	setLeftLabels(3, "Trigger");
	setMemory(0, 0);
}

//  *********************************  Start **************************


int Start_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].value[0]) {
		if (!memory[0])// toggle
		{
			memory[0] = true;
			startClock(pg);
		}
	}
	else
		memory[0] = false;
	return false;
}
Start_Component::Start_Component()
	: Component(L"Start", L"Start Clock", squareShape, 1, 1, 0, 0)
{
	setLeftLabels(3, "Trigger");
	setMemory(0, 0);
}

//  *********************************  Step **************************

int Step_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].value[0]) {
		if (!memory[0])// toggle
		{
			memory[0] = true;
			stepClock(pg);
		}
	}
	else
		memory[0] = false;
	return false;
}
Step_Component::Step_Component()
	: Component(L"Step", L"Step Clock", squareShape, 1, 1, 0, 0)
{
	setLeftLabels(3, "Trigger");
	setMemory(0, 0);
}

//  ********************************* Reset **************************


int Reset_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].value[0]) {
		if (!memory[0])// toggle
		{
			memory[0] = true;
			resetClock(pg);
		}
	}
	else
		memory[0] = false;
	return false;
}
Reset_Component::Reset_Component()
	: Component(L"Reset", L"Reset Clock", squareShape, 1, 1, 0, 0)
{
	setLeftLabels(3, "Trigger");
	setMemory(0, 0);
}

// ********************** running **************************

int Running_Component::method2(Sheet* pg) {
	if (pg->timerID == NULL)
		output[0].value[0] = 0;
	else
		output[0].value[0] = 1;
	return true;
}
Running_Component::Running_Component()
	: Component(L"Running", L"Running", squareShape, 0, 0, 1, 1)
{
	output[0].dimensions[0] = 1;
}

// ********************** Time Step **************************

int TimeStep_Component::method2(Sheet* pg) {
	
	if (!checkValueInputs())
		return 0;
	if (input[0].drivenByComponent == -1) {
		pg->timeStep = 0.05;
		return 1;
	}
	if (input[0].value[0] > 1000) {
		pg->timeStep = 1000;
		return 1;
	}
	if (input[0].value[0] <= 0) {
		pg->timeStep = 0.001;
		return 1;
	}

	if (pg->timeStep != input[0].value[0]) {
		pg->timeStep = input[0].value[0];
		return 1;
	}
	return 0;
}
TimeStep_Component::~TimeStep_Component() {

	if (page->timeStep == 0.050)
		return;
	page->timeStep = 0.050;
	// for some reason method will be called next - override the input values
	if (input[0].value.size())
		input[0].value[0] = 0.050;
	return;
}
TimeStep_Component::TimeStep_Component()
	: Component(L"TimeStep", L"Time Step", squareShape, 1, 1, 0, 1)
{
	setLeftLabels(3, "Seconds");
}



// *********************************** Wave generators **********************************



void doubleClickGenerator(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Output Period:", "Output Maximum:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[2] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}
std::vector<Button*>generatorButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickGenerator) };



// *********************************** Square Wave **********************************

int SquareWave_Component::method2(Sheet* pg) {
	//.setting[0] = 1.0; // limit = rate
	//.setting[1] = 0.0; // accumulator
	//.setting[2] = 1.0; // output voltage

	if (pg->systemTime == 0.0) // Reset
	{
		setting[1] = 0;
		output[0].value[0] = 0.0;
		memory[0] = pg->systemTime;
	}

	bool changed = false;
	// add time
	if (memory[0] != pg->systemTime) {
		setting[1] += pg->timeStep;
		if (setting[1] > setting[0] / 2) {
			// reached limit -- toggle output
			if (output[0].value[0] == 0.0)
				output[0].value[0] = setting[2];
			else
				output[0].value[0] = 0.0;
			changed = true;
			// and reset timer
			setting[1] -= setting[0] / 2;
		}
		memory[0] = pg->systemTime;
	}
	return changed;
}
void SquareWave_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if (doubleValues[0] >= 0.01)
		setting[0] = doubleValues[0]; // period
	if (doubleValues[1] > 0)
		setting[2] = doubleValues[1]; // max output

	onLoad(pg);
}
SquareWave_Component::SquareWave_Component()
	: Component(L"SquareWave", L"Square Wave", squareShape, 0, 0, 1, 1)
{
	timeBased = true;
	dialogButtonList = generatorButtonList;
	setSetting(0, 1.0); // period
	setSetting(1, 0.0); // accumulator
	setSetting(2, 1.0); // output voltage
	setMemory(0, 0);  // system time
	height = 9;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
}



// *********************************** Sawtooth **********************************

int Sawtooth_Component::method2(Sheet* pg) {
	//.setting[0] = 1.0; // limit = rate
	//.setting[1] = 0.0; // accumulator
	//.setting[2] = 1.0; // output voltage
	bool change = false;
	if (pg->systemTime == 0.0) // Reset
	{
		setting[1] = 0;
		if (output[0].value[0] != 0.0) {
			output[0].value[0] = 0.0;
			change = true;
		}
		memory[0] = pg->systemTime;
	}

	// add time
	if (memory[0] != pg->systemTime) {
		setting[1] += pg->timeStep;
		if (setting[1] > setting[0]) {
			// reset timer
			setting[1] -= setting[0];
		}
		memory[0] = pg->systemTime;
	}
	double out = setting[2] * setting[1] / setting[0];
	if (output[0].value[0] != out) {
		output[0].value[0] = out;
		change = true;
	}
	return change;
}
void Sawtooth_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if (doubleValues[0] >= 0.01)
		setting[0] = doubleValues[0]; // period
	if (doubleValues[1] > 0)
		setting[2] = doubleValues[1]; // max output

	onLoad(pg);
}
Sawtooth_Component::Sawtooth_Component()
	: Component(L"Sawtooth", L"Sawtooth", squareShape, 0, 0, 1, 1)
{
	timeBased = true;
	dialogButtonList = generatorButtonList;
	setSetting(0, 1.0); // period
	setSetting(1, 0.0); // accumulator
	setSetting(2, 1.0); // output voltage
	setMemory(0, 0);  // system time
	height = 9;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
}

// *********************************** Trianglar **********************************



int TriangleWave_Component::method2(Sheet* pg) {
	//.setting[0] = 1.0; // limit = rate
	//.setting[1] = 0.0; // accumulator
	//.setting[2] = 1.0; // output voltage
	bool change = false;
	if (pg->systemTime == 0.0) // Reset
	{
		setting[1] = 0;
		if (output[0].value[0] != 0.0) {
			output[0].value[0] = 0.0;
			change = true;
		}
		memory[0] = pg->systemTime;
		memory[1] = 1; // start on increasing slope
	}

	// add time
	if (memory[0] != pg->systemTime) {
		if (memory[1] > 0) { // we are in the rising phase
			setting[1] += pg->timeStep * 2.0;
			if (setting[1] >= setting[0]) {
				// reset timer - reflect off the limit for accuracy
				double overage = setting[1] - setting[0];
				setting[1] = setting[0] - 2 * overage;
				memory[1] = -1; // switch to falling phase
			}
		}
		else { // now falling phase
			setting[1] -= pg->timeStep * 2.0;
			if (setting[1] <= 0) {
				// reset timer
				double underage = -setting[1];
				setting[1] = underage;
				memory[1] = 1; // switch to rising phase
			}
		}
		memory[0] = pg->systemTime;
	}
	double out = setting[2] * setting[1] / setting[0];
	if (output[0].value[0] != out) {
		output[0].value[0] = out;
		change = true;
	}
	return change;
}

void TriangleWave_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if (doubleValues[0] >= 0.01)
		setting[0] = doubleValues[0]; // period
	if (doubleValues[1] > 0)
		setting[2] = doubleValues[1]; // max output

	onLoad(pg);
}
TriangleWave_Component::TriangleWave_Component()
	: Component(L"TriWave", L"Triangular Wave", squareShape, 0, 0, 1, 1)
{
	timeBased = true;
	dialogButtonList = generatorButtonList;
	setSetting(0, 1.0); // period
	setSetting(1, 0.0); // accumulator
	setSetting(2, 1.0); // output voltage
	setMemory(0, 0);  // system time
	setMemory(1, 0);  // phase
	height = 9;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
}

// *********************************** Sine Wave **********************************



int SineWave_Component::method2(Sheet* pg) {
	//.setting[0] = 1.0; // limit = rate
	//.setting[1] = 0.0; // accumulator
	//.setting[2] = 1.0; // output voltage
	bool change = false;

	if (pg->systemTime == 0) // Reset
	{
		setting[1] = 0;
		if (output[0].value[0] != 0.0) {
			output[0].value[0] = 0.0;
			change = true;
		}
		memory[0] = pg->systemTime;
	}

	// add time
	if (memory[0] != pg->systemTime) {
		setting[1] += pg->timeStep;
		if (setting[1] > setting[0]) {
			// reset timer
			setting[1] -= setting[0];
		}
		memory[0] = pg->systemTime;
	}
	double ang = setting[1] / setting[0] * TWO_PI;
	double out = setting[2] * sin(ang);
	if (output[0].value[0] != out) {
		output[0].value[0] = out;
		change = true;
	}
	return change;
}

void SineWave_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if (doubleValues[0] >= 0.01)
		setting[0] = doubleValues[0]; // period
	if (doubleValues[1] > 0)
		setting[2] = doubleValues[1]; // max output

	onLoad(pg);
}
SineWave_Component::SineWave_Component()
	: Component(L"SineWave", L"Sine Wave", squareShape, 0, 0, 1, 1)
{
	timeBased = true;
	dialogButtonList = generatorButtonList;
	setSetting(0, 1.0); // period
	setSetting(1, 0.0); // accumulator
	setSetting(2, 1.0); // output voltage
	setMemory(0, 0);  // system time
	height = 9;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
}


// *********************************** Pulse Generator **********************************



int Pulse_Component::method2(Sheet* pg) {
	//nC.memory[0] = 1;  // generators
	//nC.memory[1+2*j] = 0;  // gen[j]: time to off (from pulse width)
	//nC.memory[2+2*j] = 0;  // gen[j]: time time to next pulse (from interval)

	if (pg->timerID == NULL) // if the timer is not running - allow number of timers to change
	{
		int loops = inputsToLoops();
		if (loops == 0)
			return clearOutputs();

		if ((memory[0] != loops) || (pg->systemTime == 0.0)) { // Reset
			output[0].value.resize(loops); // resize output
			memory.resize(1 + 2 * loops); // resize memory
			for (int j = 0; j < loops; j++) {
				memory[1 + 2 * j] = 0; // signal off now
				memory[2 + 2 * j] = 0; // signal off now
				output[0].value[j] = 0; // clear output
			}
			output[0].dimensions[0] = loops;
			memory[0] = loops;
		}
		return true;
	}

	// start looping
	bool change = false;
	for (int j = 0; j < memory[0]; j++) {

		// always finish pulse - no matter if enabled
		if ((memory[1 + 2 * j] <= pg->systemTime) && (output[0].value[j])) {
			output[0].value[j] = 0;
			change = true;
		}

		// if enabled and interval time is up - start new interval
		bool enable = getLoopingValueD(0, j, 0);
		if (enable && (memory[2 + 2 * j] <= pg->systemTime)) {
			double interval = getLoopingValueD(1, j, 1);
			double width = getLoopingValueD(2, j, 0.1);

			output[0].value[j] = 1;
			change = true;
			memory[1 + 2 * j] = pg->systemTime + width; // set up the pulse width
			memory[2 + 2 * j] = pg->systemTime + interval; // set up the interval
		}
	}
	return change;
}


Pulse_Component::Pulse_Component()
	: Component(L"Pulse", L"Pulse Generator", 3, 1)
{
	timeBased = true;
	setLeftLabels(3, "Enable", "Interval", "Pulse Width");
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
	setMemory(0, 1);   //  number of generators
	setMemory(1, 0);   // gen[0] pulse width timing
	setMemory(2, 1);   // gen[1] interval timing
	
	width = 40;
}



// *********************************** integrator **************************************

// input[0] initial, 
// input[1] + input,
// input[2] - input, 
// input[3] hold, 
// input[4] reset,
// setting[0] stabilizer,
// setting[1] real time

void Integrator_Component::dial(Sheet* pg) {
	int ax = x;
	int ay = y - height + 15;

	if (setting[0])
		drawTextString(pg->hDC, pg->cXo + ax + 4, pg->cYo + ay + 15, 0, "Stab");
	if (setting[1])
		drawTextString(pg->hDC, pg->cXo + ax + 7, pg->cYo + ay + 30, 0, "RT");
}

double Integrator_Component::innerMethodNMO(Sheet* pg, double *input, double *memory, double oldOutput) {

	if (pg->systemTime == 0) {
		memory[0] = 0.0;
		return input[0];
	}
	if (input[4])  // reset always wins
		return input[0];

	if (input[3])  // hold stays put
		return oldOutput;

	if (memory[0] == pg->systemTime) // dont change if the clock is not running
		return oldOutput;
	memory[0] = pg->systemTime;

	oldOutput += (input[1] - input[2]) * pg->timeStep;

	return oldOutput;
}
Integrator_Component::Integrator_Component()
	: FunctionNMO_Component(L"Integ", L"Integrator", squareShape, 5, 1, 1, 1)
{
	//dialogList = dialogIntegButtonList;
	timeBased = true;
	setLeftLabels(3, "Initial", "+ In", "- In", "Hold", "Reset");
	setSetting(0, 0); // default to stabilizer off
	setSetting(1, 0); // default to not real time
	setMemory(0, 0);  // used to see if the system time changed
}


// *********************************** Time Random **********************************



int TimeRand_Component::method2(Sheet* pg) {
	
	if (pg->timerID) {
		output[0].value[0] = (double)rand() / (double)RAND_MAX;
		return true;
	}
	return false;
}


TimeRand_Component::TimeRand_Component()
	: Component(L"TimeRand", L"Time Random", squareShape, 0, 1, 1, 1)
{
	timeBased = true;
	output[0].dimensions[0] = 1;
}

// *********************************** Trig Random **********************************

int TrigRand_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	int loops = input[0].value.size();
	if (memory.size() != loops) {
		output[0].value.resize(loops);
		memory.resize(loops);
		output[0].value[0] = 0;
		output[0].dimensions[0] = loops;
	}

	bool change = false;
	for( int j = 0; j < loops; j++ ) {
		if (input[0].value[j]) {
			if (memory[j] == 0) {
				output[0].value[j] = (double)rand() / (double)RAND_MAX;
				change = true;
			}
			memory[j] = 1;
		}
		else
			memory[j] = 0;
	}
	return change;
}


TrigRand_Component::TrigRand_Component()
	: Component(L"TrigRand", L"Triggered Random", 1, 1)
{
	setLeftLabels(3, "Trigger");
	output[0].dimensions[0] = 1;
	setMemory(0, 0); // edge detector
}


// ********************** one shot **************************


int OneShot_Component::method2(Sheet* pg) {

	//initialMemoryX.push_back(0);	// phase
	//initialMemoryY.push_back(0);	// time left
	//memory[0] = 1;   //  timers

	if (pg->timerID == NULL) { // if the timer is NOT running - allow number of timers to change
		int loops = inputsToLoops();
		if (loops == 0)
			return clearOutputs();

		if ((memory[0] != loops) || (pg->systemTime == 0.0)) { // Reset
			output[0].value.clear(); // normal
			output[1].value.clear(); // invert
			initialMemoryX.clear();
			initialMemoryY.clear();
			for (int j = 0; j < loops; j++) {
				initialMemoryX.push_back(0); // phase = ready for trigger
				initialMemoryY.push_back(0);
				output[0].value.push_back(0); // normal
				output[1].value.push_back(1); // invert
			}
			output[0].dimensions[0] = loops; // normal
			output[1].dimensions[0] = loops; // invert
			memory[0] = loops;
		}
		return true;
	}

	// start looping
	int change = 0;
	for (int j = 0; j < memory[0]; j++) {
		double delay = getLoopingValueD(0, j, 10);
		double hold = getLoopingValueD(1, j, 10);
		bool trigger = getLoopingValueD(2, j, 0);

		int newOutput = 0; // no change
		switch ((int)initialMemoryX[j]) { // decide what to do based on phase
		case 0: // ready to start
			if (trigger) {
				if (delay == 0) {
					newOutput = 1; // turn on
					initialMemoryX[j] = 2; // phase = on for hold interval
					initialMemoryY[j] = pg->systemTime + hold;
				}
				else {
					initialMemoryX[j] = 1; // phase is off for delay interval
					initialMemoryY[j] = pg->systemTime + delay;
				}
			}
			break;
		case 1: // delaying (still off)
			if (initialMemoryY[j] <= pg->systemTime) {
				newOutput = 1; // turn on
				initialMemoryX[j] = 2; // phase = on for hold interval
				initialMemoryY[j] = pg->systemTime + hold;
			}
			break;
		case 2: // holding (on)
			if (initialMemoryY[j] <= pg->systemTime) {
				newOutput = 2; // turn off
				if (trigger)
					initialMemoryX[j] = 3; // phase = waiting for trigger to go low
				else
					initialMemoryX[j] = 0; // phase = ready
			}
			break;
		case 3: // waiting for trigger to go low (off)
			if (!trigger)
				initialMemoryX[j] = 0; // phase = ready
			break;
		}
		switch (newOutput) {
		case 1: // turn on output
			output[0].value[j] = 1;
			output[1].value[j] = 0;
			change = 1;
			break;
		case 2: // turn off output
			output[0].value[j] = 0;
			output[1].value[j] = 1;
			change = 1;
			break;
		}
	}
	return change;
}

OneShot_Component::OneShot_Component()
	: Component(L"OneShot", L"One Shot", squareShape, 3, 1, 2, 1)
{
	timeBased = true;
	setLeftLabels(3, "Delay Interval", "Hold Interval", "Trigger");
	setRightLabels(3, "Normal", "Inverted");
	width = 60;
	setMemory(0, 1);	//  timers
	output[0].value[0] = 0;	// start low
	output[1].value[0] = 1;	// start high
	initialMemoryX.push_back(0);	// phase
	initialMemoryY.push_back(0);	// time left
}

// ************************ Timer ****************************
int Timer_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	if ((memory.size() != loops) || (output[0].value.size() != loops)) {
		memory.resize(loops);
		output[0].value.resize(loops);
		for (int j = 0; j < loops; j++) {
			memory[j] = getLoopingValueD(2, j, 1); // prime the pump
			output[0].value[j] = 0; // not running
		}
	}

	// start looping
	for (int j = 0; j < loops; j++) {
		bool run = (bool)getLoopingValueD(0, j, 0);
		bool reset = (bool)getLoopingValueD(1, j, 0);
		double interval = getLoopingValueD(2, j, 1);

		output[0].value[j] = 0; // not running

		if (reset || (pg->systemTime == 0.0)) // Reset
		{
			memory[j] = interval; // prime the pump
			output[0].value[j] = 0;
		}

		if (run) 
			memory[j] -= pg->timeStep;

		if (memory[j] < 0)
			output[0].value[j] = 1;
		else
			output[0].value[j] = 0;
	}
	return 1;
}
Timer_Component::Timer_Component()
	: Component(L"Timer", L"Timer", 3, 1)
{
	timeBased = true;
	setLeftLabels(3, "Run", "Reset", "Interval");
	output[0].value[0] = 0;   // start low
	setSetting(0, 1);  // interval 1 second
	setMemory(0, 1);   // primed
}

// ************************ Timer2 ****************************

int Timer2_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	if ((memory.size() != loops) || 
		(output[0].value.size() != loops) ||
		(output[1].value.size() != loops)) {
		memory.resize(loops); 
		output[0].value.resize(loops);
		output[1].value.resize(loops);
		for (int j = 0; j < loops; j++) {
			memory[j] = 1; // prime the pump
			output[0].value[j] = 0; // not running
			output[1].value[j] = 0; // not done
		}
	}

	// start looping
	for (int j = 0; j < loops; j++) {
		bool run = getLoopingValueD(0, j, 0);
		bool reset = getLoopingValueD(1, j, 0);
		double interval = getLoopingValueD(2, j, 1);

		if (reset || (pg->systemTime == 0.0)) { // Reset
			memory[j] = interval; // prime the pump
			output[0].value[j] = 0; // not running
			output[1].value[j] = 0; // not done
		}
		else {
			if (run) // run
				memory[j] -= pg->timeStep;

			if ((memory[j] < interval) && (memory[j] > 0)) {
				output[0].value[j] = 1; // running
				output[1].value[j] = 0; // not done
			}
			if (memory[j] <= 0) {
				output[0].value[j] = 0; // not running
				output[1].value[j] = 1; // done
			}
		}
	}

	return 1;
}

Timer2_Component::Timer2_Component()
	: Component(L"Timer2", L"Timer 2", squareShape, 3, 1, 2, 1)
{
	//   nC->dialogList = dialogTimerButtonList;
	timeBased = true;
	setLeftLabels(3, "Run", "Reset", "Interval");
	setRightLabels(3, "Running", "Done");
	width = 40;
	output[0].value[0] = 0;   // start low
	output[1].value[0] = 0;   // start low
	setMemory(0, 1);   // time left
}

// *********************** Scalar Counter *****************************


int ScalarCounter_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	if ((memory.size() != loops) || (output[0].value.size() != loops)) {
		memory.resize(loops);
		output[0].value.resize(loops);
		for (int j = 0; j < loops; j++) {
			memory[j] = 0; 
			output[0].value[j] = 0; 
		}
	}

	// start looping
	for (int j = 0; j < loops; j++) {
		bool count = getLoopingValueD(0, j, 0);
		bool reset = getLoopingValueD(1, j, 0);
		
		if (reset ) { 
			memory[j] = 0;
			output[0].value[j] = 0;
		}
		else {
			if (count) {
				if( memory[j] == 0 )// edge detector
					output[0].value[j]++;
				memory[j] = 1;
			}else
				memory[j] = 0;
		}
	}

	return 1;
}
ScalarCounter_Component::ScalarCounter_Component()
	: Component(L"SCount", L"Scalar Counter", 2, 1)
{
	setLeftLabels(3, "Count", "Reset");
	output[0].value[0] = 0;
	setMemory(0, 0);   // edge detector
}