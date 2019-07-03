
#include "stdafx.h"
#include "MathComponents.h"
#include "FunctionComponents.h"
#include "LogicComponents.h"
#include "Sheet.h"
#include "color.h"
#include "Text.h"
#include "ScreenDialog.h"



#pragma warning (disable: 4267)
#pragma warning (disable: 4244)


LogicComponent::LogicComponent(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS) 
	: FunctionN_Component( _type, _label, shapePath, LTC, LTS, RTC, RTS){
}

void LogicComponent::dial(Sheet* pg) {

		// output indicator
		if (output[0].value.size() != 1)
			return;

		HBRUSH fillStyle;
		if (output[0].value[0])
			fillStyle = GetColorBrush(0xFFFFFF); // CreateSolidBrush
		else
			fillStyle = GetColorBrush(0x000000); // CreateSolidBrush

		int x1 = pg->cXo + x + width - 16;
		int y1 = pg->cYo + y;

		RECT aRect;
		aRect.left = x1 - 5;
		aRect.top = y1 - 5;
		aRect.right = x1 + 5;
		aRect.bottom = y1 + 5;

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






double GT_Component::innerMethodN(double *a) {
	return a[0] > a[1] ? 1 : 0;
}
GT_Component::GT_Component()
	: LogicComponent(L"GT", L">", triangleShape, 2, 2, 1, 1)
{}

double LessThan_Component::innerMethodN(double *a) {
	return a[0] < a[1] ? 1 : 0;
}
LessThan_Component::LessThan_Component()
	: LogicComponent(L"LT", L"<", triangleShape, 2, 2, 1, 1)
{}

double EQ_Component::innerMethodN(double *a) {
	return a[0] == a[1] ? 1 : 0;
}
EQ_Component::EQ_Component()
	: LogicComponent(L"EQ", L"=", triangleShape, 2, 2, 1, 1)
{}

double GTE_Component::innerMethodN(double *a) {
	return a[0] >= a[1];
}
GTE_Component::GTE_Component()
	: LogicComponent(L"GTE", L">=", triangleShape, 2, 2, 1, 1)
{}

double LessThanOrEqual_Component::innerMethodN(double *a) {
	return a[0] <= a[1] ? 1 : 0;
}
LessThanOrEqual_Component::LessThanOrEqual_Component()
	: LogicComponent(L"LTE", L"<=", triangleShape, 2, 2, 1, 1)
{}

double NEQ_Component::innerMethodN(double *a) {
	return a[0] != a[1] ? 1 : 0;
}
NEQ_Component::NEQ_Component()
	: LogicComponent(L"NEQ", L"!=", triangleShape, 2, 2, 1, 1)
{}

double GTZ_Component::innerMethodN(double *a) {
	return a[0] > 0 ? 1 : 0;
}
GTZ_Component::GTZ_Component()
	: LogicComponent(L"GTZ", L">0", triangleShape, 1, 1, 1, 1)
{}

double LessThanZero_Component::innerMethodN(double *a) {
	return a[0] < 0 ? 1 : 0;
}
LessThanZero_Component::LessThanZero_Component()
	: LogicComponent(L"LTZ", L"<0", triangleShape, 1, 1, 1, 1)
{}

double EQZ_Component::innerMethodN(double *a) {
	return a[0] == 0 ? 1 : 0;
}
EQZ_Component::EQZ_Component()
	: LogicComponent(L"EQZ", L"=0", triangleShape, 1, 1, 1, 1)
{}

double GTEZ_Component::innerMethodN(double *a) {
	return a[0] >= 0 ? 1 : 0;
}
GTEZ_Component::GTEZ_Component()
	: LogicComponent(L"GTEZ", L">=0", triangleShape, 1, 1, 1, 1)
{}

double LTEZ_Component::innerMethodN(double *a) {
	return a[0] <= 0 ? 1 : 0;
}
LTEZ_Component::LTEZ_Component()
	: LogicComponent(L"LTEZ", L"<=0", triangleShape, 1, 1, 1, 1)
{}

double NEQZ_Component::innerMethodN(double *a) {
	return a[0] != 0 ? 1 : 0;
}
NEQZ_Component::NEQZ_Component()
	: LogicComponent(L"NEQZ", L"!=0", triangleShape, 1, 1, 1, 1)
{}

double And_Component::innerMethodN(double *a) {
	return ((int)a[0]) && ((int)a[1]) ? 1 : 0;
}
And_Component::And_Component()
	: LogicComponent(L"And", L"AND", triangleShape, 2, 2, 1, 1)
{}

double  Nand_Component::innerMethodN(double *a) {
	return  ((int)a[0]) && ((int)a[1]) ? 0 : 1;
}
Nand_Component::Nand_Component()
	: LogicComponent(L"Nand", L"NAND", triangleShape, 2, 2, 1, 1)
{}

double Or_Component::innerMethodN(double *a) {
	return  ((int)a[0]) || ((int)a[1]) ? 1 : 0;
}
Or_Component::Or_Component()
	: LogicComponent(L"Or", L"OR", triangleShape, 2, 2, 1, 1)
{}

double Nor_Component::innerMethodN(double *a) {
	return  ((int)a[0]) && ((int)a[1]) ? 0 : 1;}
Nor_Component::Nor_Component()
	: LogicComponent(L"Nor", L"NOR", triangleShape, 2, 2, 1, 1)
{}

double Xor_Component::innerMethodN(double *a) {
	if (((int)a[0]) && !((int)a[1]))
		return 1;
	if (!((int)a[0]) && ((int)a[1]))
		return 1;
	return 0;}
Xor_Component::Xor_Component()
	: LogicComponent(L"Xor", L"XOR", triangleShape, 2, 2, 1, 1)
{}

double Not_Component::innerMethodN(double *a) {
	return a[0] ? 0 : 1;}
Not_Component::Not_Component()
	: LogicComponent(L"Not", L"NOT", triangleShape, 1, 1, 1, 1)
{}


//***********************************************************************
//**********************************************************************

// ************************ Logic ***************************



bool Logic_Component::getInputVal(int n, int loop) {
	if (input[n].value.size() == 0)
		return 0;
	if (input[n].value.size() == 1)
		return input[n].value[0];
	return input[n].value[loop];
}


int Logic_Component::method2(Sheet* pg) {
	// read the literals and sequence through them to find a match
	// first comes the inputs: 'X'=don't care, 'T'=true, 'F'=false, 'P'=positive edge, 'N'=negative edge
	// then comes the outputs, 'X'=no change, 'T'=true, 'F'=false.
	int line, inj, outj;
	int match = 0;
	int inputCount = setting[0];
	int outputCount = setting[1];
	int len[32];

	// check inputs for matches
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	if (inputMemory.size() != inputCount)
		inputMemory.resize(inputCount);

	for (inj = 0; inj < inputCount; inj++) {
		len[inj] = input[inj].value.size();
		if (inputMemory[inj].size() != loops) {
			inputMemory[inj].resize(loops);
			for (int ink = 0; ink < loops; ink++)
				inputMemory[inj][ink] = false;
		}
	}
	if (output[0].value.size() != loops) {
		for (outj = 0; outj < outputCount; outj++)
			output[outj].value.resize(loops);
	}
	bool change = false;

	for (int loop = 0; loop < loops; loop++) {

		for (line = inputCount + outputCount; line < literal.size(); line++) {
			match = true;
			for (inj = 0; inj < inputCount; inj++) {
				bool inVal = getInputVal(inj, loop);
				string xx = literal[line].substr(inj, 1);
				if      (xx == "T") { if (!inVal)							match = false; } 
				else if (xx == "F") { if (inVal)							match = false; }
				else if (xx == "P") { if (!inVal || inputMemory[inj][loop]) match = false; }
				else if (xx == "N") { if (inVal || !inputMemory[inj][loop]) match = false; }
				if (!match)
					break;
			}
			if (match)
				break;
		}

		// for the edge trigger
		for (inj = 0; inj < inputCount; inj++)
			inputMemory[inj][loop] = getInputVal(inj, loop);

		// finally we have a match - output the results
		if (match) {
			// grab output values so we dont short circut ourselves
			bool oldOut[32];
			for (outj = 0; outj < outputCount; outj++) {
				if (output[outj].value.size() == 1)
					oldOut[outj] = output[outj].value[0];
				else
					oldOut[outj] = output[outj].value[loop]; // was 0 like above
			}
			for (outj = 0; outj < outputCount; outj++) {
				string xx = literal[line].substr(inputCount + outj, 1);
				if (xx == "T") { output[outj].value[loop] = 1; } else
				if (xx == "F"){ output[outj].value[loop] = 0; } else
				if (xx == "0"){ output[outj].value[loop] = getInputVal(0, loop); } else
				if (xx == "1"){ output[outj].value[loop] = getInputVal(1, loop); } else
				if (xx == "2"){ output[outj].value[loop] = getInputVal(2, loop); } else
				if (xx == "3"){ output[outj].value[loop] = getInputVal(3, loop); } else
				if (xx == "4"){ output[outj].value[loop] = getInputVal(4, loop); } else
				if (xx == "5"){ output[outj].value[loop] = getInputVal(5, loop); } else
				if (xx == "6"){ output[outj].value[loop] = getInputVal(6, loop); } else
				if (xx == "7"){ output[outj].value[loop] = getInputVal(7, loop); } else
				if (xx == "8"){ output[outj].value[loop] = getInputVal(8, loop); } else
				if (xx == "9"){ output[outj].value[loop] = getInputVal(9, loop); } else
				if (xx == "a"){ output[outj].value[loop] = oldOut[0]; } else
				if (xx == "b"){ output[outj].value[loop] = oldOut[1]; } else
				if (xx == "c"){ output[outj].value[loop] = oldOut[2]; } else
				if (xx == "d"){ output[outj].value[loop] = oldOut[3]; } else
				if (xx == "e"){ output[outj].value[loop] = oldOut[4]; } else
				if (xx == "f"){ output[outj].value[loop] = oldOut[5]; } else
				if (xx == "g"){ output[outj].value[loop] = oldOut[6]; } else
				if (xx == "h"){ output[outj].value[loop] = oldOut[7]; } else
				if (xx == "i"){ output[outj].value[loop] = oldOut[8]; } else
				if (xx == "j"){ output[outj].value[loop] = oldOut[9]; } 
			}
			change = true;
		}
	}
	setSimpleOutputDimensions();
	return change;
}



// Display dialog and actions for the Logic popup buttons
void logicSetInputCount(Sheet* pg, int i, int inputCount) {
	if (inputCount == pg->dgrm.cL[i]->setting[0])
		return;

	int originalSetting = pg->dgrm.cL[i]->setting[0];
	pg->dgrm.cL[i]->setting[0] = inputCount;

	if (pg->dgrm.cL[i]->setting[0] < 1)
		pg->dgrm.cL[i]->setting[0] = 1;

	if (pg->dgrm.cL[i]->setting[0] > 17 - pg->dgrm.cL[i]->setting[1])
		pg->dgrm.cL[i]->setting[0] = 17 - pg->dgrm.cL[i]->setting[1];

	int j, k;
	string front, back;
	if (pg->dgrm.cL[i]->setting[0] < originalSetting) {

		// remove some input labels
		j = originalSetting;
		k = pg->dgrm.cL[i]->setting[0];
		do {
			pg->dgrm.cL[i]->literal[k++] = pg->dgrm.cL[i]->literal[j++];
		} while (j < pg->dgrm.cL[i]->literal.size());
		pg->dgrm.cL[i]->literal.resize(k);

		// and shift logic left
		j = pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1];
		do {
			front = pg->dgrm.cL[i]->literal[j].substr(0, pg->dgrm.cL[i]->setting[0]);
			back = pg->dgrm.cL[i]->literal[j].substr(originalSetting);
			pg->dgrm.cL[i]->literal[j] = front + back;
			j++;
		} while (j < pg->dgrm.cL[i]->literal.size());
	}
	else if (pg->dgrm.cL[i]->setting[0] > originalSetting) {
		// insert some input labels
		j = pg->dgrm.cL[i]->literal.size() + pg->dgrm.cL[i]->setting[0] - originalSetting - 1; // dest
		k = j - pg->dgrm.cL[i]->setting[0] + originalSetting; // src
		pg->dgrm.cL[i]->literal.resize(j + 1);
		do {
			pg->dgrm.cL[i]->literal[j--] = pg->dgrm.cL[i]->literal[k--];
		} while (k >= originalSetting);
		for (j = originalSetting; j < pg->dgrm.cL[i]->setting[0]; j++)
			pg->dgrm.cL[i]->literal[j] = "in " + tomString(j);

		// and shift logic right
		string mid = "";
		for (j = originalSetting; j < pg->dgrm.cL[i]->setting[0]; j++)
			mid += 'X';
		j = pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1];
		do {
			front = pg->dgrm.cL[i]->literal[j].substr(0, originalSetting);
			back = pg->dgrm.cL[i]->literal[j].substr(originalSetting);
			pg->dgrm.cL[i]->literal[j] = front + mid + back;
			j++;
		} while (j < pg->dgrm.cL[i]->literal.size());
	}

	pg->dgrm.cL[i]->onLoad(pg);

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	//drawCanvas2();
	//drawCanvas();
}

void logicSetOutputCount(Sheet* pg, int i, int outputCount) {
	if (outputCount == pg->dgrm.cL[i]->setting[1])
		return;

	int originalSetting = pg->dgrm.cL[i]->setting[1];
	pg->dgrm.cL[i]->setting[1] = outputCount;
	if (pg->dgrm.cL[i]->setting[1] < 1)
		pg->dgrm.cL[i]->setting[1] = 1;
	if (pg->dgrm.cL[i]->setting[1] > 17 - pg->dgrm.cL[i]->setting[0])
		pg->dgrm.cL[i]->setting[1] = 17 - pg->dgrm.cL[i]->setting[0];

	int j, k;
	if (pg->dgrm.cL[i]->setting[1] < originalSetting) {
		// remove some input labels
		j = pg->dgrm.cL[i]->setting[0] + originalSetting;
		k = pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1];
		do {
			pg->dgrm.cL[i]->literal[k++] = pg->dgrm.cL[i]->literal[j++];
		} while (j < pg->dgrm.cL[i]->literal.size());
		pg->dgrm.cL[i]->literal.resize(k);

		// and shift logic left
		j = pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1];
		do {
			pg->dgrm.cL[i]->literal[j] = pg->dgrm.cL[i]->literal[j].substr(0, pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1]);
			j++;
		} while (j < pg->dgrm.cL[i]->literal.size());
	}
	else if (pg->dgrm.cL[i]->setting[1] > originalSetting) {
		// insert some input labels
		j = pg->dgrm.cL[i]->literal.size() + pg->dgrm.cL[i]->setting[1] - originalSetting - 1; // dest
		k = j - pg->dgrm.cL[i]->setting[1] + originalSetting; // src
		pg->dgrm.cL[i]->literal.resize(j + 1);
		do {
			pg->dgrm.cL[i]->literal[j--] = pg->dgrm.cL[i]->literal[k--];
		} while (k >= pg->dgrm.cL[i]->setting[0] + originalSetting);
		for (j = originalSetting; j < pg->dgrm.cL[i]->setting[1]; j++)
			pg->dgrm.cL[i]->literal[pg->dgrm.cL[i]->setting[0] + j] = "out " + tomString(j);

		// and shift logic right
		string back = "";
		for (j = originalSetting; j < pg->dgrm.cL[i]->setting[1]; j++)
			back += 'X';
		j = pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1];
		do {
			pg->dgrm.cL[i]->literal[j] += back;
			j++;
		} while (j < pg->dgrm.cL[i]->literal.size());
	}
	pg->dgrm.cL[i]->onLoad(pg);

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	//drawCanvas2();
	//drawCanvas();
}

void logicSetLogicsCount(Sheet* pg, int i, int logicsCount) {
	int originalLength = pg->dgrm.cL[i]->literal.size() - pg->dgrm.cL[i]->setting[0] - pg->dgrm.cL[i]->setting[1];

	if (logicsCount == originalLength)
		return;

	// ask the user for a value
	if (logicsCount < 1)
		logicsCount = 1;
	if (logicsCount > 30)
		logicsCount = 30;

	int j, k;
	if (logicsCount < originalLength) {
		// remove some logic lines
		j = pg->dgrm.cL[i]->literal.size() + logicsCount - originalLength;
		pg->dgrm.cL[i]->literal.resize(j);
	}
	else if (logicsCount > originalLength) {
		// insert some logic lines
		string temp = "";
		for (j = 0; j < pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1]; j++)
			temp += 'X';
		j = pg->dgrm.cL[i]->literal.size();
		pg->dgrm.cL[i]->literal.resize(pg->dgrm.cL[i]->setting[0] + pg->dgrm.cL[i]->setting[1] + logicsCount);
		for (k = originalLength; k < logicsCount; k++)
			pg->dgrm.cL[i]->literal[j++] = temp;

	}
	pg->dgrm.cL[i]->onLoad(pg);

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	//drawCanvas2();
	//drawCanvas();
}

void buttonLogic4AndAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 4;
	pg->dgrm.cL[i]->setting[1] = 1;
	pg->dgrm.cL[i]->literal.resize(7);
	pg->dgrm.cL[i]->literal[0] = "A";
	pg->dgrm.cL[i]->literal[1] = "B";
	pg->dgrm.cL[i]->literal[2] = "C";
	pg->dgrm.cL[i]->literal[3] = "D";
	pg->dgrm.cL[i]->literal[4] = "out";
	pg->dgrm.cL[i]->literal[5] = "TTTTT";
	pg->dgrm.cL[i]->literal[6] = "XXXXF";
	pg->dgrm.cL[i]->userLabel = "AND";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}

void buttonLogic4OrAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 4;
	pg->dgrm.cL[i]->setting[1] = 1;
	pg->dgrm.cL[i]->literal.resize(7);
	pg->dgrm.cL[i]->literal[0] = "A";
	pg->dgrm.cL[i]->literal[1] = "B";
	pg->dgrm.cL[i]->literal[2] = "C";
	pg->dgrm.cL[i]->literal[3] = "D";
	pg->dgrm.cL[i]->literal[4] = "out";
	pg->dgrm.cL[i]->literal[5] = "FFFFF";
	pg->dgrm.cL[i]->literal[6] = "XXXXT";
	pg->dgrm.cL[i]->userLabel = "OR";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}

void buttonLogicDLatchAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 4;
	pg->dgrm.cL[i]->setting[1] = 2;
	pg->dgrm.cL[i]->literal.resize(10);
	pg->dgrm.cL[i]->literal[0] = "Set";
	pg->dgrm.cL[i]->literal[1] = "D";
	pg->dgrm.cL[i]->literal[2] = "Clock";
	pg->dgrm.cL[i]->literal[3] = "Reset";
	pg->dgrm.cL[i]->literal[4] = "Q";
	pg->dgrm.cL[i]->literal[5] = "/Q";
	pg->dgrm.cL[i]->literal[6] = "TXXFTF";
	pg->dgrm.cL[i]->literal[7] = "FXXTFT";
	pg->dgrm.cL[i]->literal[8] = "FTPFTF";
	pg->dgrm.cL[i]->literal[9] = "FFPFFT";
	pg->dgrm.cL[i]->userLabel = "D-FlipFlop";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}

void buttonLogic4bitRotateAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 7;
	pg->dgrm.cL[i]->setting[1] = 4;
	pg->dgrm.cL[i]->literal.resize(14);
	pg->dgrm.cL[i]->literal[0] = "Load";
	pg->dgrm.cL[i]->literal[1] = "A";
	pg->dgrm.cL[i]->literal[2] = "B";
	pg->dgrm.cL[i]->literal[3] = "C";
	pg->dgrm.cL[i]->literal[4] = "D";
	pg->dgrm.cL[i]->literal[5] = "Left";
	pg->dgrm.cL[i]->literal[6] = "Right";
	pg->dgrm.cL[i]->literal[7] = "A";
	pg->dgrm.cL[i]->literal[8] = "B";
	pg->dgrm.cL[i]->literal[9] = "C";
	pg->dgrm.cL[i]->literal[10] = "D";
	pg->dgrm.cL[i]->literal[11] = "PXXXXFF1234";
	pg->dgrm.cL[i]->literal[12] = "FXXXXPFbcda";
	pg->dgrm.cL[i]->literal[13] = "FXXXXFPdabc";
	pg->dgrm.cL[i]->userLabel = "4 Bit Rotate";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}
void buttonLogicFullAdderAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 3;
	pg->dgrm.cL[i]->setting[1] = 2;
	pg->dgrm.cL[i]->literal.resize(13);
	pg->dgrm.cL[i]->literal[0] = "C";
	pg->dgrm.cL[i]->literal[1] = "A";
	pg->dgrm.cL[i]->literal[2] = "B";
	pg->dgrm.cL[i]->literal[3] = "S";
	pg->dgrm.cL[i]->literal[4] = "C";
	pg->dgrm.cL[i]->literal[5] = "FFFFF";
	pg->dgrm.cL[i]->literal[6] = "TFFTF";
	pg->dgrm.cL[i]->literal[7] = "FTFTF";
	pg->dgrm.cL[i]->literal[8] = "TTFFT";
	pg->dgrm.cL[i]->literal[9] = "FFTTF";
	pg->dgrm.cL[i]->literal[10] = "TFTFT";
	pg->dgrm.cL[i]->literal[11] = "FTTFT";
	pg->dgrm.cL[i]->literal[12] = "TTTTT";
	pg->dgrm.cL[i]->userLabel = "Full Adder";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}
void buttonLogicSRFlipFlopAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 2;
	pg->dgrm.cL[i]->setting[1] = 2;
	pg->dgrm.cL[i]->literal.resize(6);
	pg->dgrm.cL[i]->literal[0] = "S";
	pg->dgrm.cL[i]->literal[1] = "R";
	pg->dgrm.cL[i]->literal[2] = "0";
	pg->dgrm.cL[i]->literal[3] = "1";
	pg->dgrm.cL[i]->literal[4] = "PXTF";
	pg->dgrm.cL[i]->literal[5] = "XPFT";
	pg->dgrm.cL[i]->userLabel = "SR Flip Flop";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}
void buttonLogic_SetOptionsAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildLogicOptionsDialogButtons(pg, i);
}
void openLogicDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildLogicLogicsDialogButtons(pg, i);
}
std::vector<Button*> logicButtonList = { new Button(3, 40, 135, 20, L"Set Opions...", buttonLogic_SetOptionsAction),
	new Button(3, 60, 135, 20, L"Edit internal logic...", openLogicDialog),

	new Button(-102, 40, 100, 20, L"4 input AND", buttonLogic4AndAction),
	new Button(-102, 60, 100, 20, L"4 input OR", buttonLogic4OrAction),
	new Button(-102, 80, 100, 20, L"D type Latch", buttonLogicDLatchAction),
	new Button(-102, 100, 100, 20, L"4 Bit Rotate", buttonLogic4bitRotateAction),
	new Button(-102, 120, 100, 20, L"Full Adder", buttonLogicFullAdderAction),
	new Button(-102, 140, 100, 20, L"SR Flip Flop", buttonLogicSRFlipFlopAction) };



//  copy this for logic 2 after it works
bool Logic_Component::onLoad(Sheet* pg) {
	int inPins = setting[0];
	int outPins = setting[1];
	height = pinToHeight(max(inPins, outPins));

	// left terminals are inputs
	buildInputPins(inPins);

	//pg->dgrm.cL[i].leftLabels = [];
	for (int j = 0; j < inPins; j++) {
		input[j].label = literal[j];
	}

	// for the edge triggers
	inputMemory.resize(inPins);

	// right terminals are outputs
	buildOutputPins(outPins);

	//pg->dgrm.cL[i].rightLabels = [];
	for (int j = 0; j < outPins; j++) {
		output[j].label = literal[inPins + j];
	}
	return true;
}


Logic_Component::Logic_Component()
	: Component(L"Logic", L"Logic", 1, 1)
{
	dialogButtonList = logicButtonList;
	setSetting(0, 1);
	setSetting(1, 1);
	//setSetting(2, 1); // temp for number of logic lines needed
	setMemory(0, false); // for edge detection
	setLiteral(0, "in 0"); // input label
	setLeftLabels(1, "in 0");
	setLiteral(1, "out 0"); // output label
	setRightLabels(1, "out 0");
	setLiteral(2, "TF"); // inverter logic
	setLiteral(3, "FT"); // inverter logic
}

// ****************** logic 2 ***********************************
#if 0
bool Logic2_Component::method2(Sheet* pg) {
	// read the literals and sequence through them to find a match
	// first comes the inputs: 'X'=don't care, 'T'=true, 'F'=false, 'P'=positive edge, 'N'=negative edge
	// then the feedback of the outputs: 'X'=don't care, 'T'=true, 'F'=false
	// then comes the output actions, 'X'=no change, 'T'=true, 'F'=false.
	var line, inj, outj, match;
	var inputCount = pg->dgrm.cL[i].setting[0];
	var outputCount = pg->dgrm.cL[i].setting[1];
	var len = [];

	// check inputs for matches
	var result = inputsToLoopsAndDimensions(pg, i);
	if (result.loops == 0)
		return clearOutputs_(pg, i);
	setOutputMatrixSizes_(pg, i, result.dims);


	for (inj = 0; inj < inputCount; inj++) {
		len[inj] = pg->dgrm.cL[i].inputValue[inj].length;
		if (pg->dgrm.cL[i].memory[inj].length != len[inj])
			pg->dgrm.cL[i].memory[inj] = [];
	}
	var change = false;

	for (var loop = 0; loop < result.loops; loop++) {

		// process each line of the logics
		for (line = inputCount + outputCount; line < pg->dgrm.cL[i].literal.length; line++) {
			match = true;
			// first check for matches with the inputs
			for (inj = 0; inj < inputCount; inj++) {
				var inVal = 0;
				if (len[inj] == 1)
					inVal = pg->dgrm.cL[i].inputValue[inj][0];
				if (len[inj] > 1)
					inVal = pg->dgrm.cL[i].inputValue[inj][loop];

				var xx = pg->dgrm.cL[i].literal[line].substr(inj, 1);
				switch (xx) {
				case 'T':
					if (!inVal)
						match = false;
					break;
				case 'F':
					if (inVal)
						match = false;
					break;
				case 'P':
					if (!inVal || pg->dgrm.cL[i].memory[inj][loop])
						match = false;
					break;
				case 'N':
					if (inVal || !pg->dgrm.cL[i].memory[inj][loop])
						match = false;
					break;
				}
				if (!match) // inputs dont match logic
					break;
			}
			// then (NEW) check for matches with the outputs
			for (outj = 0; outj < outputCount; outj++) {
				var outVal = pg->dgrm.cL[i].outputValue[outj][loop];
				xx = pg->dgrm.cL[i].literal[line].substr(inputCount + outj, 1);
				switch (xx) {
				case 'T':
					if (!outVal)
						match = false;
					break;
				case 'F':
					if (outVal)
						match = false;
					break;
				}
				if (!match) // outputs (feedback) dont match logic
					break;
			}

			if (match) // WOW we matched all the inputs and outputs
				break;
		}

		// for the edge trigger
		for (inj = 0; inj < inputCount; inj++)
			pg->dgrm.cL[i].memory[inj][loop] = pg->dgrm.cL[i].inputValue[inj][loop];

		if (match) {
			for (outj = 0; outj < outputCount; outj++) {
				xx = pg->dgrm.cL[i].literal[line].substr(inputCount + outputCount + outj, 1);
				switch (xx) {
				case 'T':
					pg->dgrm.cL[i].outputValue[outj][loop] = 1;
					break;
				case 'F':
					pg->dgrm.cL[i].outputValue[outj][loop] = 0;
					break;
				}
			}
			change = true;
		}
	}
	return change;
}


Logic2_Component::Logic2_Component()
	: Component(L"Logic2", L"Logic2", 1, 1)
{
	//dialogList = dialogLogic2ButtonList;
	setSetting(0, 1); // inputs
	setSetting(1, 1); // outputs
	setMemory(0, false); // for input edge detection
	setLiteral(0, "in 0"); // input label
	setLeftLabels(1, L"in 0");
	setLiteral(1, "out 0"); // output label
	setRightLabels(1, L"out 0");
	setLiteral(2, "TXF"); // logic, number of logics = literals - inputs - outputs
}
#endif



// ********************** D to A **************************************

bool DtoA_Component::getInputVal(int n, int loop) {
	if (input[n].value.size() == 0)
		return 0;
	if (input[n].value.size() == 1)
		return input[n].value[0];
	return input[n].value[loop];
}
int DtoA_Component::method2(Sheet* pg) {
	// read the literals and sequence through them to find a match
	// first comes the inputs: 'X'=don't care, 'T'=true, 'F'=false, 'P'=positive edge, 'N'=negative edge
	// the output values are in the elements
	int line, inj, match;
	int inputCount = setting[0];
	int len[32];

	// check inputs for matches
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	if (inputMemory.size() != inputCount)
		inputMemory.resize(inputCount);

	for (inj = 0; inj < inputCount; inj++) {
		len[inj] = input[inj].value.size();
		if (inputMemory[inj].size() != loops) {
			inputMemory[inj].resize(loops);
			for (int ink = 0; ink < loops; ink++)
				inputMemory[inj][ink] = false;
		}
	}
	if (output[0].value.size() != loops) {
		output[0].value.resize(loops);
	}
	bool change = false;

	for (int loop = 0; loop < loops; loop++) {

		for (line = 0; line < element.size(); line++) {
			int lit = inputCount + line;
			match = true;
			for (inj = 0; inj < inputCount; inj++) {
				bool inVal = getInputVal(inj, loop);
				string xx = literal[lit].substr(inj, 1);
				if      (xx == "T") { if (!inVal)							match = false; }
				else if (xx == "F") { if (inVal)							match = false; }
				else if (xx == "P") { if (!inVal || inputMemory[inj][loop]) match = false; }
				else if (xx == "N") { if (inVal || !inputMemory[inj][loop]) match = false; 	}
				if (!match)
					break;
			}
			if (match)
				break;
		}

		// for the edge trigger
		for (inj = 0; inj < inputCount; inj++)
			inputMemory[inj][loop] = getInputVal(inj, loop);

		// finally we have a match - output the result
		if (match) {
			if (output[0].value[loop] != element[line]) {
				output[0].value[loop] = element[line];
				change = true;
			}
		}
	}
	setSimpleOutputDimensions();
	return change;
}

void dtoASetInputCount(Sheet* pg, int i, int inputCount) {
	if (inputCount == pg->dgrm.cL[i]->setting[0])
		return;

	int originalSetting = pg->dgrm.cL[i]->setting[0];
	pg->dgrm.cL[i]->setting[0] = inputCount;

	if (pg->dgrm.cL[i]->setting[0] < 1)
		pg->dgrm.cL[i]->setting[0] = 1;

	if (pg->dgrm.cL[i]->setting[0] > 17)
		pg->dgrm.cL[i]->setting[0] = 17 ;

	int j, k;
	string front, back;
	if (pg->dgrm.cL[i]->setting[0] < originalSetting) {

		// remove some input labels
		j = originalSetting;
		k = pg->dgrm.cL[i]->setting[0];
		do {
			pg->dgrm.cL[i]->literal[k++] = pg->dgrm.cL[i]->literal[j++];
		} while (j < pg->dgrm.cL[i]->literal.size());
		pg->dgrm.cL[i]->literal.resize(k);

		// and shift logic left
		j = pg->dgrm.cL[i]->setting[0];
		do {
			front = pg->dgrm.cL[i]->literal[j].substr(0, pg->dgrm.cL[i]->setting[0]);
			back = pg->dgrm.cL[i]->literal[j].substr(originalSetting);
			pg->dgrm.cL[i]->literal[j] = front + back;
			j++;
		} while (j < pg->dgrm.cL[i]->literal.size());
	}
	else if (pg->dgrm.cL[i]->setting[0] > originalSetting) {
		// insert some input labels
		j = pg->dgrm.cL[i]->literal.size() + pg->dgrm.cL[i]->setting[0] - originalSetting - 1; // dest
		k = j - pg->dgrm.cL[i]->setting[0] + originalSetting; // src
		pg->dgrm.cL[i]->literal.resize(j + 1);
		do {
			pg->dgrm.cL[i]->literal[j--] = pg->dgrm.cL[i]->literal[k--];
		} while (k >= originalSetting);
		for (j = originalSetting; j < pg->dgrm.cL[i]->setting[0]; j++)
			pg->dgrm.cL[i]->literal[j] = "in " + tomString(j);

		// and shift logic right
		string mid = "";
		for (j = originalSetting; j < pg->dgrm.cL[i]->setting[0]; j++)
			mid += 'X';
		j = pg->dgrm.cL[i]->setting[0];
		do {
			front = pg->dgrm.cL[i]->literal[j].substr(0, originalSetting);
			back = pg->dgrm.cL[i]->literal[j].substr(originalSetting);
			pg->dgrm.cL[i]->literal[j] = front + mid + back;
			j++;
		} while (j < pg->dgrm.cL[i]->literal.size());
	}

	pg->dgrm.cL[i]->onLoad(pg);

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	
}

void dtoASetLogicsCount(Sheet* pg, int i, int logicsCount) {
	int originalLength = pg->dgrm.cL[i]->element.size();

	if (logicsCount == originalLength)
		return;

	// ask the user for a value
	if (logicsCount < 1)
		logicsCount = 1;
	if (logicsCount > 30)
		logicsCount = 30;

	int j, k;
	if (logicsCount < originalLength) {
		// remove some logic lines
		j = pg->dgrm.cL[i]->literal.size() + logicsCount - originalLength;
		pg->dgrm.cL[i]->literal.resize(j);
		pg->dgrm.cL[i]->element.resize(logicsCount);
	}
	else if (logicsCount > originalLength) {
		// insert some logic lines
		string temp = "";
		for (j = 0; j < pg->dgrm.cL[i]->setting[0]; j++)
			temp += 'X';
		j = pg->dgrm.cL[i]->literal.size();
		pg->dgrm.cL[i]->literal.resize(pg->dgrm.cL[i]->setting[0] + logicsCount);
		pg->dgrm.cL[i]->element.resize(logicsCount);
		for (k = originalLength; k < logicsCount; k++) {
			pg->dgrm.cL[i]->literal[j++] = temp;
			pg->dgrm.cL[i]->element[k] = -1;
		}
	}
	pg->dgrm.cL[i]->onLoad(pg);

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);

}
void buttonDtoA3BitAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 3;
	pg->dgrm.cL[i]->literal.resize(11);
	pg->dgrm.cL[i]->literal[0] = "A";
	pg->dgrm.cL[i]->literal[1] = "B";
	pg->dgrm.cL[i]->literal[2] = "C";
	pg->dgrm.cL[i]->literal[3] = "FFF";
	pg->dgrm.cL[i]->literal[4] = "TFF";
	pg->dgrm.cL[i]->literal[5] = "FTF";
	pg->dgrm.cL[i]->literal[6] = "TTF";
	pg->dgrm.cL[i]->literal[7] = "FFT";
	pg->dgrm.cL[i]->literal[8] = "TFT";
	pg->dgrm.cL[i]->literal[9] = "FTT";
	pg->dgrm.cL[i]->literal[10] = "TTT";
	pg->dgrm.cL[i]->element.resize(8);
	pg->dgrm.cL[i]->element[0] = 0;
	pg->dgrm.cL[i]->element[1] = 1;
	pg->dgrm.cL[i]->element[2] = 2;
	pg->dgrm.cL[i]->element[3] = 3;
	pg->dgrm.cL[i]->element[4] = 4;
	pg->dgrm.cL[i]->element[5] = 5;
	pg->dgrm.cL[i]->element[6] = 6;
	pg->dgrm.cL[i]->element[7] = 7;

	pg->dgrm.cL[i]->userLabel = "3-Bit";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}
void buttonDtoA4BitAction(Sheet* pg, int i) {
	pg->dgrm.cL[i]->setting[0] = 4;
	pg->dgrm.cL[i]->literal.resize(20);
	pg->dgrm.cL[i]->literal[0] = "A";
	pg->dgrm.cL[i]->literal[1] = "B";
	pg->dgrm.cL[i]->literal[2] = "C";
	pg->dgrm.cL[i]->literal[3] = "D";
	pg->dgrm.cL[i]->literal[4] = "FFFF";
	pg->dgrm.cL[i]->literal[5] = "TFFF";
	pg->dgrm.cL[i]->literal[6] = "FTFF";
	pg->dgrm.cL[i]->literal[7] = "TTFF";
	pg->dgrm.cL[i]->literal[8] = "FFTF";
	pg->dgrm.cL[i]->literal[9] = "TFTF";
	pg->dgrm.cL[i]->literal[10] = "FTTF";
	pg->dgrm.cL[i]->literal[11] = "TTTF";
	pg->dgrm.cL[i]->literal[12] = "FFFT";
	pg->dgrm.cL[i]->literal[13] = "TFFT";
	pg->dgrm.cL[i]->literal[14] = "FTFT";
	pg->dgrm.cL[i]->literal[15] = "TTFT";
	pg->dgrm.cL[i]->literal[16] = "FFTT";
	pg->dgrm.cL[i]->literal[17] = "TFTT";
	pg->dgrm.cL[i]->literal[18] = "FTTT";
	pg->dgrm.cL[i]->literal[19] = "TTTT";
	pg->dgrm.cL[i]->element.resize(16);
	pg->dgrm.cL[i]->element[0] = 0;
	pg->dgrm.cL[i]->element[1] = 1;
	pg->dgrm.cL[i]->element[2] = 2;
	pg->dgrm.cL[i]->element[3] = 3;
	pg->dgrm.cL[i]->element[4] = 4;
	pg->dgrm.cL[i]->element[5] = 5;
	pg->dgrm.cL[i]->element[6] = 6;
	pg->dgrm.cL[i]->element[7] = 7;
	pg->dgrm.cL[i]->element[8] = 8;
	pg->dgrm.cL[i]->element[9] = 9;
	pg->dgrm.cL[i]->element[10] = 10;
	pg->dgrm.cL[i]->element[11] = 11;
	pg->dgrm.cL[i]->element[12] = 12;
	pg->dgrm.cL[i]->element[13] = 13;
	pg->dgrm.cL[i]->element[14] = 14;
	pg->dgrm.cL[i]->element[15] = 15;

	pg->dgrm.cL[i]->userLabel = "4-Bit";
	pg->dgrm.cL[i]->onLoad(pg);
	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas2();
	drawCanvas();
}


void buttonDtoA_SetOptionsAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildDtoAOptionsDialogButtons(pg, i);
}
void openDtoADialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildDtoALogicsDialogButtons(pg, i);
}
std::vector<Button*> dtoAButtonList = { new Button(3, 40, 135, 20, L"Set Opions...", buttonDtoA_SetOptionsAction),
	new Button(3, 60, 135, 20, L"Edit internal logic...", openDtoADialog),
	new Button(-102, 40, 100, 20, L"3 bit", buttonDtoA3BitAction),
	new Button(-102, 60, 100, 20, L"4 bit", buttonDtoA4BitAction) };

bool DtoA_Component::onLoad(Sheet* pg) {
	int inPins = setting[0];
	height = pinToHeight(inPins);

	// left terminals are inputs
	buildInputPins(inPins);

	for (int j = 0; j < inPins; j++) {
		input[j].label = literal[j];
	}

	// for the edge triggers
	inputMemory.resize(inPins);

	return true;
}


DtoA_Component::DtoA_Component()
	: Component(L"DtoA", L"D to A", 1, 1)
{
	dialogButtonList = dtoAButtonList;
	setSetting(0, 1); // inputs
	setMemory(0, false); // for edge detection
	setLiteral(0, "in 0"); // input label
	setLeftLabels(1, "in 0");
	setLiteral(1, "T"); // logic
	setElement(0, 123); // output 0
}