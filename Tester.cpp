#include "stdafx.h"
//#include "MathComponents.h"
//#include "FunctionComponents.h"
#include "Sheet.h"
//#include "Text.h"
//#include "ScreenDialog.h"
//#include "ArrayComponents.h"
//#include "MethodComponents.h"
//#include "buildComponent.h"
#include "Tester.h"






// *********************** 2D Position Tester *******************************

int Tester_2dPosition_Component::method2(Sheet* pg) {

	if (pg->timerID == NULL) {
		memory[0] = 0;
		return 0;
	}

	double inputStart = getLoopingValueD(0, 0, 0);
	double inputDuration = getLoopingValueD(1, 0, 10);
	double inputLoop = getLoopingValueD(2, 0, 10);
	double inputXyRange = getLoopingValueD(3, 0, 100);

	memory[0] += pg->timeStep;
	if (memory[0] > inputLoop)
		memory[0] = 0;

	if ((memory[0] < inputStart) || (memory[0] > inputStart + inputDuration)) {
		for (int k = 0; k < rightTerminalCount; k++)
			output[k].value.resize(4);

		output[0].value[0] = inputXyRange; // x
		output[0].value[1] = inputXyRange; // x
		output[0].value[2] = -inputXyRange; // x
		output[0].value[3] = -inputXyRange; // x

		output[1].value[0] = inputXyRange; // y
		output[1].value[1] = -inputXyRange; // y
		output[1].value[2] = inputXyRange; // y
		output[1].value[3] = -inputXyRange; // y
		return 1;
	}

	for (int k = 0; k < rightTerminalCount; k++)
		output[k].value.resize(1);

	double fraction = (memory[0] - inputStart) / inputDuration;

	double angle = fraction * TWO_PI;
	output[0].value[0] = inputXyRange * sin(angle); // x
	output[1].value[0] = inputXyRange * cos(angle); // y

	return true;
}

Tester_2dPosition_Component::Tester_2dPosition_Component()
	: Component(L"2dPosTester", L"2D Position Tester", squareShape, 4, 1, 2, 1)
{
	timeBased = true;
	setLeftLabels(3, "Start", "Duration", "Loop", "XY Range");
	setRightLabels(3, "X", "Y");
	setMemory(0, 0); //time based index
	width = 40;
}




// *********************** 3D Position Tester *******************************

int Tester_3dPosition_Component::method2(Sheet* pg) {

	if (pg->timerID == NULL) {
		memory[0] = 0;
		return 0;
	}

	double inputStart = getLoopingValueD(0, 0, 0);
	double inputDuration = getLoopingValueD(1, 0, 10);
	double inputLoop = getLoopingValueD(2, 0, 10);
	double inputXyzRange = getLoopingValueD(3, 0, 100);

	memory[0] += pg->timeStep;
	if (memory[0] > inputLoop)
		memory[0] = 0;

	if ((memory[0] < inputStart) || (memory[0] > inputStart + inputDuration)) {
		for (int k = 0; k < rightTerminalCount; k++)
			output[k].value.resize(4);

		output[0].value[0] = inputXyzRange; // x
		output[1].value[0] = inputXyzRange; // y
		output[2].value[0] = inputXyzRange; // z

		output[0].value[1] = inputXyzRange; // x
		output[1].value[1] = 0; // y
		output[2].value[1] = 0; // z

		output[0].value[2] = 0; // x
		output[1].value[2] = inputXyzRange; // y
		output[2].value[2] = 0; // z

		output[0].value[3] = 0; // x		
		output[1].value[3] = 0; // y
		output[2].value[3] = inputXyzRange; // z
		return 1;
	}

	for (int k = 0; k < rightTerminalCount; k++)
		output[k].value.resize(1);

	double fraction = (memory[0] - inputStart) / inputDuration;

	if (fraction < 0.333) { // rotate on x
		double angle = fraction * 3 * TWO_PI;
		output[0].value[0] = inputXyzRange * sin(angle); // x
		output[1].value[0] = 0; // y
		output[2].value[0] = 0; // z
	}
	else if (fraction < 0.666) { // rotate on y
		double angle = (fraction - 0.333) * 3 * TWO_PI;
		output[0].value[0] = 0; // x
		output[1].value[0] = inputXyzRange * sin(angle); // y
		output[2].value[0] = 0; // z
	}
	else // rotate on z
	{
		double angle = (fraction - 0.666) * 3 * TWO_PI;
		output[0].value[0] = 0; // x
		output[1].value[0] = 0; // y
		output[2].value[0] = inputXyzRange * sin(angle); // z
	}
	return true;
}

Tester_3dPosition_Component::Tester_3dPosition_Component()
	: Component(L"3dPosTester", L"3D Position Tester", squareShape, 4, 1, 3, 1)
{
	timeBased = true;
	setLeftLabels(3, "Start", "Duration", "Loop", "XYZ Range");
	setRightLabels(3, "X", "Y","Z");
	setMemory(0, 0); //time based index
	width = 40;
}


// *********************** Color Tester *******************************

int Tester_2dColor_Component::method2(Sheet* pg) {

	if (pg->timerID == NULL) {
		memory[0] = 0;
		return 0;
	}

	double inputStart = getLoopingValueD(0, 0, 0);
	double inputDuration = getLoopingValueD(1, 0, 10);
	double inputLoop = getLoopingValueD(2, 0, 10);
	double inputDefault = getLoopingValueD(3, 0, 0);

	memory[0] += pg->timeStep;
	if (memory[0] > inputLoop)
		memory[0] = 0;

	if ((memory[0] < inputStart) || (memory[0] > inputStart + inputDuration)) {

		for (int k = 0; k < 3; k++)
			output[k].value.resize(1);

		output[0].value[0] = inputDefault; // red line
		output[1].value[0] = inputDefault; // green line
		output[2].value[0] = inputDefault; // blue line
		
		return 1;
	}

	double fraction = (memory[0] - inputStart) / inputDuration;

	for (int k = 0; k < 3; k++)
		output[k].value.resize(4);

	int color1 = 255 * fraction;
	int color2;
	int color3;
	if (fraction < 0.5) {
		color2 = 255 * fraction * 2;
		color3 = 0;
	}
	else {
		color3 = 255 * (fraction - 0.5) * 2;
		color2 = 255;
	}
	output[0].value[0] = color1; // red line
	output[1].value[0] = color1; // green line
	output[2].value[0] = color1; // blue line

	output[0].value[1] = color2; // red line
	output[1].value[1] = color3; // green line
	output[2].value[1] = color3; // blue line

	output[0].value[2] = color3; // red line
	output[1].value[2] = color2; // green line
	output[2].value[2] = color3; // blue line

	output[0].value[3] = color3; // red line
	output[1].value[3] = color3; // green line
	output[2].value[3] = color2; // blue line
		
	return true;
}

Tester_2dColor_Component::Tester_2dColor_Component()
	: Component(L"2dColorTester", L"2D Color Tester", squareShape, 4, 1, 3, 1)
{
	timeBased = true;
	setLeftLabels(3, "Start", "Duration", "Loop", "Default");
	setRightLabels(3, "Red", "Green", "Blue");
	setMemory(0, 0);//time based index
	width = 50;
}


// *********************** 2 Phase Tester *******************************

int Tester_2Phase_Component::method2(Sheet* pg) {

	if (pg->timerID == NULL) {
		memory[0] = 0;
		return 0;
	}

	double inputStart = getLoopingValueD(0, 0, 0);
	double inputDuration = getLoopingValueD(1, 0, 10);
	double inputLoop = getLoopingValueD(2, 0, 10);
	double inputSizeMin = getLoopingValueD(3, 0, 0);
	double inputSizeMax = getLoopingValueD(4, 0, 100);

	memory[0] += pg->timeStep;
	if (memory[0] > inputLoop)
		memory[0] = 0;

	if ((memory[0] < inputStart) || (memory[0] > inputStart + inputDuration)) {
		output[0].value[0] = (inputSizeMax + inputSizeMin) / 2; // width
		output[1].value[0] = (inputSizeMax + inputSizeMin) / 2; // height
		return 1;
	}

	double fraction = (memory[0] - inputStart) / inputDuration;

	double size;
	if (fraction < 0.5)
		size = fraction * 2;
	else
		size = -2 * fraction + 2;

	output[0].value[0] = size * (inputSizeMax - inputSizeMin) + inputSizeMin; // width
	output[1].value[0] = (1 - size) * (inputSizeMax - inputSizeMin) + inputSizeMin; // height

	return true;
}

Tester_2Phase_Component::Tester_2Phase_Component()
	: Component(L"2PhaseTester", L"2 Phase Tester", squareShape, 5, 1, 2, 1)
{
	timeBased = true;
	setLeftLabels(3, "Start", "Duration", "Loop", "Size Min", "Size Max");
	setRightLabels(3,  "Width", "Height");
	setMemory(0, 0);//time based index
	width = 50;
}



// *********************** 3 Phase Tester *******************************

int Tester_3Phase_Component::method2(Sheet* pg) {

	if (pg->timerID == NULL) {
		memory[0] = 0;
		return 0;
	}

	double inputStart = getLoopingValueD(0, 0, 0);
	double inputDuration = getLoopingValueD(1, 0, 10);
	double inputLoop = getLoopingValueD(2, 0, 10);
	double inputSizeMin = getLoopingValueD(3, 0, 0);
	double inputSizeMax = getLoopingValueD(4, 0, 100);

	memory[0] += pg->timeStep;
	if (memory[0] > inputLoop)
		memory[0] = 0;

	if ((memory[0] < inputStart) || (memory[0] > inputStart + inputDuration)) {
		output[0].value[0] = (inputSizeMax + inputSizeMin) / 2; // width
		output[1].value[0] = (inputSizeMax + inputSizeMin) / 2; // height
		output[2].value[0] = (inputSizeMax + inputSizeMin) / 2; // depth
		return 1;
	}

	double fraction = (memory[0] - inputStart) / inputDuration;

	
	double angle = fraction * TWO_PI;

	output[0].value[0] = sin(angle) * (inputSizeMax - inputSizeMin) + inputSizeMin; // width
	output[1].value[0] = sin(angle + TWO_PI/3.0) * (inputSizeMax - inputSizeMin) + inputSizeMin; // height
	output[2].value[0] = sin(angle + 2.0 *  TWO_PI / 3.0) * (inputSizeMax - inputSizeMin) + inputSizeMin; // depth

	return true;
}

Tester_3Phase_Component::Tester_3Phase_Component()
	: Component(L"3PhaseTester", L"3 Phase Tester", squareShape, 5, 1, 3, 1)
{
	timeBased = true;
	setLeftLabels(3, "Start", "Duration", "Loop", "Minimum", "Maximum");
	setRightLabels(3, "Width", "Height", "Depth");
	setMemory(0, 0);//time based index
	width = 50;
}



// *********************** 3 Phase Tester *******************************

int Tester_Bit_Component::method2(Sheet* pg) {

	if (pg->timerID == NULL) {
		memory[0] = 0;
		return 0;
	}

	double inputStart = getLoopingValueD(0, 0, 0);
	double inputDuration = getLoopingValueD(1, 0, 10);
	double inputLoop = getLoopingValueD(2, 0, 10);
	double inputChannels = getLoopingValueD(3, 0, 4);


	memory[0] += pg->timeStep;
	if (memory[0] > inputLoop)
		memory[0] = 0;

	if ((memory[0] < inputStart) || (memory[0] > inputStart + inputDuration)) {
		output[0].value.resize(1);
			output[0].value[0] = 1;
		return 1;
	}

	output[0].value.resize(inputChannels);
	double fraction = (memory[0] - inputStart) / inputDuration;
	int steps = pow(2, inputChannels);
	int bits = fraction * steps;

	for (int j = 0; j < inputChannels; j++)
		output[0].value[j] = (bits >> j) & 1;

	return true;
}

Tester_Bit_Component::Tester_Bit_Component()
	: Component(L"BitTester", L"Bit Tester", squareShape, 4, 1, 1, 1)
{
	timeBased = true;
	setLeftLabels(3, "Start", "Duration", "Loop", "channels");
	setRightLabels(3, "Bits");
	setMemory(0, 0);//time based index
	width = 50;
}


