#include "stdafx.h"
#include "ArrayComponents.h"
#include "FunctionComponents.h"
#include "ComponentButtons.h"
#include "MethodComponents.h"
#include "CanvasComponents.h"
#include "Sheet.h"
#include "Text.h"
#include "ScreenDialog.h"
#include "Fileio.h"
#include "ButtonGroups.h"
#include <iostream>
#include <fstream>
#include <direct.h>



#pragma warning (disable: 4267)
#pragma warning (disable: 4244)


extern std::vector<Button*>imageGenericButtonList;

void commonDialSplit_(Sheet* pg, Component *comp) {
	// draw the size knob on the LEFT

	int x1 = pg->cXo + comp->x - comp->width;
	int y1 = pg->cYo + comp->y + comp->height;

	BeginPath(pg->hDC);
	MoveToEx(pg->hDC, x1, y1, NULL);
	LineTo(pg->hDC, x1 + RESIZE_HANDLE_SIZE, y1);
	LineTo(pg->hDC, x1, y1 - RESIZE_HANDLE_SIZE);
	LineTo(pg->hDC, x1, y1);
	EndPath(pg->hDC);
	StrokeAndFillPath(pg->hDC);
}

bool leftCrankVerticalResize(Sheet* pg, Component *comp, int x, int y, bool check) {
	int right = comp->x - comp->width + RESIZE_HANDLE_SIZE;
	int top = comp->y + comp->height - RESIZE_HANDLE_SIZE;

	if (check) {
		if (x > right)
			return false; // missed
		if (y < top)
			return false; // missed
	}

	top = comp->y - comp->height;

	comp->height = (int)round((y - top + RESIZE_HANDLE_SIZE / 2) / 2);

	if (comp->height < 15)
		comp->height = 15;

	return true;
}



int Dimensions_Component::method2(Sheet* pg) {
	int dims = input[0].dimensions.size();
	output[0].value[0] = dims;
	output[0].dimensions[0] = 1;

	int n = rightTerminalCount;
	for (int j = 1; j < n; j++) {
		if (j <= dims) {
			output[j].value[0] = input[0].dimensions[j - 1];
			output[j].dimensions[0] = 1;
		}
		else
		{
			output[j].value[0] = 0;
			output[j].dimensions[0] = 0;
		}
	}

	return true;
}



bool crankSplitLimit_(Sheet* pg, Component *comp, int x, int y, bool check, int limit) {
	bool result = true;
	int oldTop = comp->y - comp->height;
	if (check) {
		// check for hit on resize box
		if( !leftCrankVerticalResize(pg, comp, x, y, true))
			return false;
	}
	else
		leftCrankVerticalResize(pg, comp, x, y, false);

	int pins = (int)round((2 * (comp->height - 2) - 2 * TERMINAL_SIZE) / TERMINAL_STEP + 1);
	if (pins < 2)
		pins = 2;
	if (pins > limit)
		pins = limit;

	if (comp->setting[0] != pins) {
		comp->height = pinToHeight(pins);
		comp->y = oldTop + comp->height;
		comp->setting[0] = pins;
	}
	comp->onLoad(pg); //  **************************************
	comp->method2(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();

	// move the dial
	comp->dial(pg);

	return result;
}


// ***************************************** Dimensions **********************************

void Dimensions_Component::dial(Sheet* pg) {
	commonDialSplit_(pg, this);
}
bool Dimensions_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankSplitLimit_(pg, this, x, y, check, MULTI_PIN_MAX);
}
bool Dimensions_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);
	setRightLabels(n, "Dimensions", "Dimension 1", "Dimension 2", "Dimension 3", "Dimension 4", "Dimension 5");
	return true;
}
Dimensions_Component::Dimensions_Component()
	: Component(L"Dims", L"Dimensions", squareShape, 1, 1, 2, 1)
{
	setSetting(0, 2);
	width = 40;
	setRightLabels(3, "Dimensions", "Dimension 1");
}


// ******************************** Table ************************************

void buttonTablePasteValuesAction(Sheet* pg, int i) {
	
	HANDLE theData = getDataFromClipboard();

	char* text = (char*)theData;
	int jt = 0;
	char buffer[50];
	int jb = 0;

	pg->dgrm.cL[i]->output[0].value.clear();
	pg->dgrm.cL[i]->setting.resize(2);// width and height

	while (text[jt] != 0) {
		if ((jb == 0) && (text[jt] == ' ')) {
			jt++;
			continue;
		}
		if ((text[jt] == ' ') || (text[jt] == ',') || (text[jt] == '\t') || (text[jt] == '\n') || (text[jt] == '\r')) {
			buffer[jb] = 0;
			double value = atof(buffer);
			pg->dgrm.cL[i]->output[0].value.push_back(value);
			pg->dgrm.cL[i]->setting.push_back(value);
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
		pg->dgrm.cL[i]->output[0].value.push_back(value);
		pg->dgrm.cL[i]->setting.push_back(value);
	}

	pg->activeDialogComponent = -1;

	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}


void Table_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	int oldCount = setting.size() - 2;
	int newCount = doubleValues[0];

	if ((newCount >= 1) && (newCount <= 32)) {
		if (newCount != oldCount) {
			setting.resize(newCount + 2);
			for (int j = oldCount; j < newCount; j++)
				setting[j + 2] = 0;
			onLoad(pg);
		}
	}
}

void buttonTableValueCountAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Values:" };
	vector<double> doubleValues = { (double)pg->dgrm.cL[i]->setting.size() - 2 };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}



void openTableDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildTableDialogButtons(pg, i);
}

void vector2TableDialog(Sheet* pg, int i, double *vector, int size) {
	
	pg->dgrm.cL[i]->setting.resize( 2 + size);
	pg->dgrm.cL[i]->output[0].value.resize( size);

	for (int k = 0; k < size; k++) {
		pg->dgrm.cL[i]->setting[2 + k] = vector[k];
		pg->dgrm.cL[i]->output[0].value[k] = vector[k];
	}
	pg->dgrm.cL[i]->setSimpleOutputDimensions();
	pg->dgrm.cL[i]->method2(pg);
	propagateValues(&pg->dgrm);

	pg->activeDialogComponent = -1;
	drawCanvas();
	drawCanvas2();
}

void gauss3TableDialog(Sheet* pg, int i) {
	double gauss[3] = { 0.25, 0.5, 0.25 };
	vector2TableDialog(pg, i, gauss, 3);
}

void gauss5TableDialog(Sheet* pg, int i) {
	double gauss[5] = { 0.0625, 0.25, 0.375, 0.25, 0.0625 };
	vector2TableDialog(pg, i, gauss, 5);
}

void gauss7TableDialog(Sheet* pg, int i) {
	double gauss[7] = { 0.015625, 0.09375, 0.234375, 0.3125, 0.234375, 0.09375, 0.015625 };
	vector2TableDialog(pg, i, gauss, 7);
}


void buttonTableReadFileAction(Sheet* pg, int i) {
	readTableGeneric(pg, i);
}

std::vector<Button*> tableButtonList = { new Button(-100, 40, 165, 20, L"Set Value Count...", buttonTableValueCountAction),
	new Button(-100, 60, 165, 20, L"Edit Table Values...", openTableDialog),

	new Button(-100, 85, 165, 20, L"Load 3 pt. Gaussian", gauss3TableDialog),
	new Button(-100, 105, 165, 20, L"Load 5 pt. Gaussian", gauss5TableDialog),
	new Button(-100, 125, 165, 20, L"Load 7 pt. Gaussian", gauss7TableDialog),

	new Button(-100, 150, 165, 20, L"Paste...", buttonTablePasteValuesAction),

	new Button(-100, 175, 165, 20, L"Read File ...", buttonTableReadFileAction) };

bool Table_Component::crank(Sheet* pg, int mouseX, int mouseY, bool check) {

	if (!crankResize(mouseX, mouseY, check))
		return false;

	setting[0] = width;
	setting[1] = height;

	// because the output terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}

void Table_Component::dial(Sheet* pg) {
	int totalLineHeight = 10;
	bool showHeightEllipsis = false;
	int count = setting.size() - 2;
	if (count * totalLineHeight + 3 > 2 * height - 10) {
		showHeightEllipsis = true;
		count = floor((2 * height - 10 - 3) / totalLineHeight);
		if (count < 0)
			count = 0;
	}

	drawResizeBox(pg, this);

	int an_x = x - width + 5;
	int an_y = y - height + 5;

	//ctx.fillStyle = '#000000';
	for (int j = 0; j < count; j++) {
		std::string text = tomString(setting[j + 2]);

		if (count > 9)
			//text = j.tomString() + ') ' + text;
			text.insert(0, tomString(j) + ") ");

		text = trimTextToFit(pg->hDC, text, 2 * width - 12);
		drawTextString(pg->hDC, pg->cXo + an_x + 2, pg->cYo + an_y - 3, 0, text);
		an_y += 10;
	}

	if (showHeightEllipsis) {
		drawTextString(pg->hDC, pg->cXo + an_x + 2, pg->cYo + an_y - 5, 0, "...");
	}
}

bool Table_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	int values = setting.size() - 2;
	output[0].value.resize(values);
	for (int j = 0; j < values; j++) {
		output[0].value[j] = setting[j + 2];
	}
	output[0].dimensions[0] = values;
	return true;
}

Table_Component::Table_Component()
	: Component(L"Table", L"Table", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = tableButtonList;
	height = width;
	setSetting(0, width);
	setSetting(1, height);
	setSetting(2, 11);   // output[0]
	setSetting(3, 22);   // output[1]
	setOutput(0, 0, 11);
	setOutput(0, 1, 22);
	setOutputDimension(0, 0, 2);
	completionTestDoubles = 1;
}

// ************************ Matrix ******************************************************

void load2DMatrix(Sheet* pg, int i, int *mat, int cols, int rows) {
	
	int size = cols * rows;
	pg->dgrm.cL[i]->setting.resize( 4 + size);
	pg->dgrm.cL[i]->setting[2] = cols;
	pg->dgrm.cL[i]->setting[3] = rows;

	pg->dgrm.cL[i]->output[0].value.resize(size);
	pg->dgrm.cL[i]->output[0].dimensions[0] = cols;
	pg->dgrm.cL[i]->output[0].dimensions[1] = rows;

	int k = 0;
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
			pg->dgrm.cL[i]->setting[4 + k] = mat[k];
			pg->dgrm.cL[i]->output[0].value[k] = mat[k];
			k++;
		}
	}
	pg->dgrm.cL[i]->method2(pg);
	propagateValues(&pg->dgrm);

	pg->activeDialogComponent = -1;
	drawCanvas();
	drawCanvas2();
}

void resistorMatrixAction(Sheet* pg, int i) {
	int resist[24] = { 
		0, -55,
		0, -45,
		13, -35,
		-13, -25,
		13, -15,
		-13, -5,
		13, 5,
		-13, 15,
		13, 25,
		-13, 35,
		0, 45,
		0, 55 };
	load2DMatrix(pg, i, resist, 2, 12);
}
void inductorMatrixAction(Sheet* pg, int i) {
	int coil[52] = {
		0, 50,
		0, 40,
		0, 30,
		10, 20,
		-10, 15,
		-10, 25,
		10, 20,
		10, 10,
		-10, 5,
		-10, 15,
		10, 10,
		10, 0,
		-10, -5,
		-10, 5,
		10, 0,
		10, -10,
		-10, -15,
		-10, -5,
		10, -10,
		10, -20,
		-10, -25,
		-10, -15,
		10, -20,
		0, -30,
		0, -40,
		0, -50 };
	load2DMatrix(pg, i, coil, 2, 26);
}
void batteryMatrixAction(Sheet* pg, int i) {
	int battery[45] = {
		0, 30, 1,
		0, 10, 1,
		-25, 10, 0,
		25, 10, 1,
		-10, -10, 0,
		10, -10, 1,
		0, -10, 0,
		0, -30, 1,
		-6, -14, 0,
		-2, -14, 1,
		-6, 14, 0,
		-2, 14, 1,
		-4, 16, 0,
		-4, 12, 1 };
	load2DMatrix(pg, i, battery, 3, 15);
}
void acSourceMatrixAction(Sheet* pg, int i) {
	int battery[105] = {
		0, 55, 1,
		0, 28, 1,
		20, -1, 0,
		16, -7, 1,
		12, -12, 1,
		9, -14, 1,
		5, -11, 1,
		2, -5, 1,
		-2, 2, 1,
		-5, 9, 1,
		-9, 12, 1,
		-13, 12, 1,
		-16, 8, 1,
		-20, 1, 1,
		0, 28, 0,
		9, 26, 1,
		17, 21, 1,
		24, 14, 1,
		27, 4, 1,
		27, -5, 1,
		24, -14, 1,
		17, -22, 1,
		9, -27, 1,
		0, -28, 1,
		-10, -27, 1,
		-18, -22, 1,
		-25, -15, 1,
		-28, -5, 1,
		-28, 4, 1,
		-25, 13, 1,
		-18, 21, 1,
		-10, 26, 1,
		0, 28, 1,
		0, -28, 0,
		0, -55, 1 };
	load2DMatrix(pg, i, battery, 3, 35);
}
void polarCapacitorMatrixAction(Sheet* pg, int i) {
	int cap[42] = {
		0, 30, 1,
		0, 10, 1,
		-25, 10, 0,
		25, 10, 1,
		-25, -10, 0,
		25, -10, 1,
		0, -10, 0,
		0, -30, 1,
		-6, -14, 0,
		-2, -14, 1,
		-6, 14, 0,
		-2, 14, 1,
		-4, 16, 0,
		-4, 12, 1 };
	load2DMatrix(pg, i, cap, 3, 14);
}
void diodeMatrixAction(Sheet* pg, int i) {
	int cap[30] = {
		0, 58, 1,
		0, 12, 1,
		20, 12, 1,
		0, -23, 1,
		-20, 12, 1,
		0, 12, 1,
		-20, -23, 0,
		20, -23, 1,
		0, -23, 1,
		0, -58, 1 };
	load2DMatrix(pg, i, cap, 3, 10);
}
void groundMatrixAction(Sheet* pg, int i) {
	int cap[12] = {
		0, 38,
		0, -3,
		20, -3,
		0, -38,
		-20, -3,
		0, -3 };
	load2DMatrix(pg, i, cap, 2, 6);
}
void zenerDiodeMatrixAction(Sheet* pg, int i) {
	int cap[36] = {
		0, 58, 1,
		0, 12, 1,
		20, 12, 1,
		0, -23, 1,
		-20, 12, 1,
		0, 12, 1,
		-20, -33, 0,
		-20, -23, 1,
		20, -23, 1,
		20, -13, 1,
		0, -23, 0,
		0, -58, 1 };
	load2DMatrix(pg, i, cap, 3, 12);
}
void capacitorMatrixAction(Sheet* pg, int i) {
	int cap[24] = {
		0, 30, 1,
		0, 10, 1,
		-25, 10, 0,
		25, 10, 1,
		-25, -10, 0,
		25, -10, 1,
		0, -10, 0,
		0, -30, 1};
	load2DMatrix(pg, i, cap, 3, 8);
}

//void buttonMatrixPasteValuesAction(int i) {}
//void buttonMatrixCopyCommaAction(int i) {}

void buttonMatrix_Copy_Action(Sheet* pg, int i) {
	string aString;

	int columns = pg->dgrm.cL[i]->setting[2];
	int rows = pg->dgrm.cL[i]->setting[3];
	
	int j = 0;
	for (int r = 0; r < rows; r++) {
		if (r)
			aString += "\r\n";
		for (int c = 0; c < columns; c++) {
			if (c)
				aString += ", ";
			aString += tomString(pg->dgrm.cL[i]->output[0].value[j++]);
		}
	}
	copyStringToClipboard(aString);
	pg->activeDialogComponent = -1;
}
void buttonMatrix_Paste_Action(Sheet* pg, int i) {

	HANDLE theData = getDataFromClipboard();

	char* text = (char*)theData;
	int jt = 0;
	char buffer[50];
	int jb = 0;

	pg->dgrm.cL[i]->output[0].value.clear();
	pg->dgrm.cL[i]->setting.resize(4); // width, height, columns, rows
	int row = 0;
	int column = 0;
	int maxColumns = 0;

	while (text[jt] != 0) {
		if ((jb == 0) && (text[jt] == ' ')) {
			jt++;
			continue;
		}
		if ((text[jt] == ' ') || (text[jt] == ',') || (text[jt] == '\t') || (text[jt] == '\n') || (text[jt] == '\r')) {
			buffer[jb] = 0;
			double value = atof(buffer);
			if (row == 0)
				maxColumns++; // expand columns during the first row only
			if (column < maxColumns) { // dont go past the column limit
				pg->dgrm.cL[i]->output[0].value.push_back(value);
				pg->dgrm.cL[i]->setting.push_back(value);
				column++;
				jb = 0;
			}
			if (((text[jt] == '\n') && (text[jt + 1] == '\r')) ||
				((text[jt] == '\r') && (text[jt + 1] == '\n'))) {
				if (row > 0) {
					if( column < maxColumns ) // need to pad this row
						while (column < maxColumns) {
							pg->dgrm.cL[i]->output[0].value.push_back(0);
							pg->dgrm.cL[i]->setting.push_back(0);
							column++;
						}
				}
				row++;
				column = 0;
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

		if ((row > 0) && (column < maxColumns)) { // during other rows dont go past the column limit
			pg->dgrm.cL[i]->output[0].value.push_back(value);
			pg->dgrm.cL[i]->setting.push_back(value);
			column++;
		}
		if (row > 0) {
			if (column < maxColumns) // need to pad this row
				while (column < maxColumns) {
					pg->dgrm.cL[i]->output[0].value.push_back(0);
					pg->dgrm.cL[i]->setting.push_back(0);
					column++;
				}
		}
		row++;
	}

	pg->dgrm.cL[i]->setting[0] = pg->dgrm.cL[i]->element.size();// number of points
	pg->dgrm.cL[i]->setting[2] = maxColumns;	
	pg->dgrm.cL[i]->setting[3] = row;	// max Y range
	pg->dgrm.cL[i]->output[0].dimensions[0] = maxColumns;
	pg->dgrm.cL[i]->output[0].dimensions[1] = row;

	pg->activeDialogComponent = -1;

	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();
}


void Matrix_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	int oldSize = setting[2] * setting[3];
	if ((doubleValues[0] >= 1) && (doubleValues[0] < 32))
		setting[2] = doubleValues[0]; // columns
	if ((doubleValues[1] >= 1) && (doubleValues[1] < 32))
		setting[3] = doubleValues[1]; // rows

	int newSize = setting[2] * setting[3];
	setting.resize(4 + newSize);
	for (int j = oldSize; j < newSize; j++)
		setting[4 + j] = 0;

	onLoad(pg);
}
void buttonSetMatrixSizeAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	
	vector<string> doubleLabels = { "Set Order:", "Set Number of Inputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

void openMatrixDialog(Sheet* pg, int i) {
	int columns = pg->dgrm.cL[i]->setting[2];
	int rows = pg->dgrm.cL[i]->setting[3];
	if (columns > 16) {
		//alert("You can only edit a matrix with 16 or fewer columns.");
		return;
	}
	if (rows > 22) {
		//alert("You can only edit a matrix with 22 or fewer rows.");
		return;
	}
	pg->modalDialogComponent = i;
	buildMatrixDialogButtons(pg, i);
}

std::vector<Button*> matrixButtonList = { new Button(-110, 40, 108, 20, L"Copy", buttonMatrix_Copy_Action),
	new Button(-110, 60, 108, 20, L"Paste", buttonMatrix_Paste_Action),
	new Button(-110, 90, 108, 20, L"Load Resistor", resistorMatrixAction),
	new Button(-110, 110, 108, 20, L"Load Inductor", inductorMatrixAction),
	new Button(-110, 130, 108, 20, L"Load Diode", diodeMatrixAction),
	new Button(-110, 150, 108, 20, L"Load Capacitor", capacitorMatrixAction),
	new Button(-110, 170, 108, 20, L"Load Battery", batteryMatrixAction),

	new Button(3, 40, 150, 20, L"Set Matrix Size...", buttonSetMatrixSizeAction),
	new Button(3, 60, 150, 20, L"Edit Matrix Values...", openMatrixDialog),

	new Button(3, 90, 150, 20, L"Load Zener Diode", zenerDiodeMatrixAction),
	new Button(3, 110, 150, 20, L"Load Polar Capacitor", polarCapacitorMatrixAction),
	new Button(3, 130, 150, 20, L"Load Ground", groundMatrixAction),
	new Button(3, 150, 150, 20, L"Load AC Source", acSourceMatrixAction) };



bool Matrix_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	int xWidth = setting[2];
	int yHeight = setting[3];

	int values = xWidth * yHeight;
	output[0].value.resize(values);
	for (int j = 0; j < values; j++) {
		output[0].value[j] = setting[j + 4];
	}
	output[0].dimensions.resize(2);
	output[0].dimensions[0] = xWidth;
	output[0].dimensions[1] = yHeight;
	return true;
}

void Matrix_Component::dial(Sheet* pg) {
	commonDialGrid(pg, this);
}
bool Matrix_Component::crank(Sheet* pg, int x, int y, bool check) {
	return commonCrankTable(pg, this, x, y, check);
}
Matrix_Component::Matrix_Component()
	: Component(L"Matrix", L"Matrix", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = matrixButtonList;
	height = width;
	setSetting(0, width);
	setSetting(1, height);
	setSetting(2, 2);   // width of matrix (columns)
	setSetting(3, 2);   // height of matrix (rows)
	setSetting(4, 10);   // output[0,0]
	setSetting(5, 20);   // output[1,0]
	setSetting(6, 30);   // output[0,1]
	setSetting(7, 40);   // output[1,1]
	setOutput(0, 0, 10);
	setOutput(0, 1, 20);
	setOutput(0, 2, 30);
	setOutput(0, 3, 40);
	setOutputDimension(0, 0, 2);
	setOutputDimension(0, 1, 2);
}


// **************************** Literal ******************************

void Literal_Component::dial(Sheet* pg) {

	int totalLineHeight = 10;
	bool showHeightEllipsis = false;

	// size up the situation
	int count = output[0].stringValue.size();
	if (count * totalLineHeight > 2 * height - 4) {
		showHeightEllipsis = true;
		count = (int)floor((2 * height - 4 - 3) / totalLineHeight);
	}

	drawResizeBox(pg, this);

	// draw the text
	int zx = x - width + 5;
	int zy = y - height + 5;

	for (int j = 0; j < count; j++) {
		std::string text = output[0].stringValue[j];
		if (count > 9)
			text.insert(0, tomString(j) + ") ");
		text = trimTextToFit(pg->hDC, text, 2 * width - 6);
		drawTextString(pg->hDC, pg->cXo + zx, pg->cYo + zy - 3, 0, text);

		zy += totalLineHeight;
	}

	if (showHeightEllipsis)
		drawTextString(pg->hDC, pg->cXo + zx + 8, pg->cYo + zy - 5, 0, "...");
}

bool Literal_Component::crank(Sheet* pg, int mx, int my, bool check) {
	if (!crankResize(mx, my, check))
		return false;

	setting[0] = width;
	setting[1] = height;

	// because the output terminal moved
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	
	// move the dial
	dial(pg);

	return true;
}

void pasteLiteralGeneric(Sheet* pg, int i, int mode) {

	int jLit = 0;

	HANDLE theData = getDataFromClipboard();

	char* text = (char*)theData;
	int jt = 0;
	char buffer[150];
	int jb = 0;

	pg->dgrm.cL[i]->output[0].stringValue.clear();
	pg->dgrm.cL[i]->literal.clear();

	while (text[jt] != 0) {

		bool breakIt = false;
		switch (mode) {
		case 0:	if (text[jt] == ',') breakIt = true; break;
		case 1: if (text[jt] == '\t') breakIt = true; break;
		case 2: if ((text[jt] == '\n') || (text[jt] == '\r')) breakIt = true; break;
		}

		if (breakIt) {
			buffer[jb] = 0;
			pg->dgrm.cL[i]->output[0].stringValue.push_back(buffer);
			pg->dgrm.cL[i]->literal.push_back(buffer);
			jb = 0;

			if ((mode == 2) && ((text[jt + 1] == '\r') || (text[jt + 1] == '\n')))
				jt++;
			jt++;
		}
		else
			buffer[jb++] = text[jt++];
	}
	if (jb > 0)
	{
		buffer[jb] = 0;
		pg->dgrm.cL[i]->output[0].stringValue.push_back(buffer);
		pg->dgrm.cL[i]->literal.push_back(buffer);
	}

	pg->activeDialogComponent = -1;
	propagateValues(&pg->dgrm);
	drawCanvas();
}
void buttonLitPasteCommaAction(Sheet* pg, int i) {
	pasteLiteralGeneric(pg, i, 0);
}
void buttonLitPasteTabAction(Sheet* pg, int i) {
	pasteLiteralGeneric(pg, i, 1);
}
void buttonLitPasteNewLineAction(Sheet* pg, int i) {
	pasteLiteralGeneric(pg, i, 2);
}
 
void buttonLitReadFileCommasAction(Sheet* pg, int i) {
	readLiteralGeneric(pg, i, 0);
}
void buttonLitReadFileTabsAction(Sheet* pg, int i) {
	readLiteralGeneric(pg, i, 1);
}
void buttonLitReadFileNewlinesAction(Sheet* pg, int i) {
	readLiteralGeneric(pg, i, 2);
}

void Literal_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
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
}
void buttonLitSetValueCountAction(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Values:" };
	vector<double> doubleValues = { (double)pg->dgrm.cL[i]->literal.size() };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}


void openLiteralDialog(Sheet* pg, int i) {

	if (pg->dgrm.cL[i]->literal.size() == 0) {
		buttonLitSetValueCountAction(pg, i);
	}
	else {
		pg->modalDialogComponent = i;
		buildLiteralDialogButtons(pg, i);
	}
}
void filePathLiteralDialog(Sheet* pg, int i) {
	// put the file path in the first literal
	PWSTR name = get_a_file_Path();
	if (name != NULL) {
		string name2 = convertLPCTSTR_toString(name);
		pg->dgrm.cL[i]->literal[0] = name2;
		pg->dgrm.cL[i]->output[0].stringValue[0] = name2;
	}
	pg->activeDialogComponent = -1;
}


std::vector<Button*> litButtonList = { new Button(3, 40, 140, 20, L"Set Value Count...", buttonLitSetValueCountAction),
	new Button(3, 60, 140, 20, L"Edit Literal Values...", openLiteralDialog),
	new Button(3, 80, 140, 20, L"Get a File Path...", filePathLiteralDialog),

	new Button(-143, 40, 140, 20, L"Paste w/commas...", buttonLitPasteCommaAction),
	new Button(-143, 60, 140, 20, L"Paste w/tabs...", buttonLitPasteTabAction),
	new Button(-143, 80, 140, 20, L"Paste w/NewLines...", buttonLitPasteNewLineAction),

	new Button(-100, 105, 165, 20, L"Read File w/commas...", buttonLitReadFileCommasAction),
	new Button(-100, 125, 165, 20, L"Read File w/tabs...", buttonLitReadFileTabsAction),
	new Button(-100, 145, 165, 20, L"Read File w/NewLines...", buttonLitReadFileNewlinesAction) };


bool Literal_Component::onLoad(Sheet* pg) {
	width = setting[0];
	height = setting[1];

	output[0].stringValue.resize(literal.size());
	for (size_t j = 0; j < literal.size(); j++) {
		output[0].stringValue[j] = literal[j];
	}
	output[0].dimensions[0] = literal.size();
	return true;
}

Literal_Component::Literal_Component()
	: Component(L"Lit", L"Literal", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = litButtonList;
	setSetting(0, width);
	setSetting(1, height);
	output[0].stringType = true;
	literal.resize(2);
	literal[0] = "Foo";
	literal[1] = "Bar";
	output[0].stringValue.resize(2);
	output[0].stringValue[0] = "Foo";
	output[0].stringValue[1] = "Bar";
	output[0].dimensions[0] = 2;
	completionTestDoubles = 1;
}


// *********************************** Count **************************


int Count_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
	{
		output[0].value[0] = 0;
		setSimpleOutputDimensions();
		return 1;
	}

	if( input[0].stringType )
		output[0].value[0] = input[0].stringValue.size();
	else
		output[0].value[0] = input[0].value.size();
	setSimpleOutputDimensions();
	return 1;
}

Count_Component::Count_Component()
	: Component(L"Count", L"Count", triangleShape, 1, 1, 1, 1)
{}




// mode: 0 sum, 1 product
bool innerProductSum(Sheet* pg, Component *comp, bool mode) {

	for (int j = 0; j < comp->output.size(); j++) {
		comp->output[j].value.clear();
		comp->output[j].dimensions.clear();
	}
	if (comp->input[0].drivenByComponent == -1)
		return true;

	double sumX;
	//double sumY;
	//double sumZ;
	int sizeX;
	int sizeY;
	int sizeZ;
	int x, y, z;
	vector<double> arrayX;
	vector<double> arrayY;
	vector<double> arrayZ;

	int sizeArray = comp->input[0].value.size();
	switch ((int)comp->setting[0]) {
	case 1: // sum everything together
		if (!mode) { // sum
			sumX = 0;
			for (x = 0; x < sizeArray; x++)
				sumX += comp->input[0].value[x];
		}
		else{ // product
			if (sizeArray == 0)
				sumX = 0;
			else
				sumX = 1;
			for (x = 0; x < sizeArray; x++)
				sumX *= comp->input[0].value[x];
		}
		comp->output[0].value.push_back(sumX);
		comp->output[0].dimensions.push_back(1);
		break;

	case 2:
		if (comp->input[0].dimensions.size() != 2)
			return true;
		sizeX = comp->input[0].dimensions[0];
		arrayX.resize(sizeX);
		sizeY = comp->input[0].dimensions[1];
		arrayY.resize(sizeY);

		switch (mode) {
		case 0: // sum
			for (x = 0; x < sizeX; x++)
				arrayX[x] = 0;
			for (y = 0; y < sizeY; y++)
				arrayY[y] = 0;
			for (int j = 0; j < sizeArray; j++) {
				x = j % sizeX;
				y = floor(j / sizeX);
				arrayX[x] += comp->input[0].value[j];
				arrayY[y] += comp->input[0].value[j];
			}
			break;
		case 1: // product
			for (x = 0; x < sizeX; x++)
				arrayX[x] = 1;
			for (y = 0; y < sizeY; y++)
				arrayY[y] = 1;
			for (int j = 0; j < sizeArray; j++) {
				x = j % sizeX;
				y = floor(j / sizeX);
				arrayX[x] *= comp->input[0].value[j];
				arrayY[y] *= comp->input[0].value[j];
			}
			break;
		}
		for (x = 0; x < sizeX; x++)
			comp->output[0].value.push_back(arrayX[x]);
		comp->output[0].dimensions.push_back(sizeX);
		for (y = 0; y < sizeY; y++)
			comp->output[1].value.push_back(arrayY[y]);
		comp->output[1].dimensions.push_back(sizeY);
		break;

	case 3:
		if (comp->input[0].dimensions.size() != 3)
			return true;
		sizeX = comp->input[0].dimensions[0];
		arrayX.resize(sizeX);
		sizeY = comp->input[0].dimensions[1];
		arrayY.resize(sizeY);
		sizeZ = comp->input[0].dimensions[2];
		arrayZ.resize(sizeZ);

		switch (mode) {
		case 0: // sum
			for (int x = 0; x < sizeX; x++)
				arrayX[x] = 0;
			for (int y = 0; y < sizeY; y++)
				arrayY[y] = 0;
			for (int z = 0; z < sizeZ; z++)
				arrayZ[z] = 0;
			for (int j = 0; j < sizeArray; j++) {
				x = j % sizeX;
				y = floor((j / sizeX) % sizeY);
				z = floor(j / (sizeX * sizeY));
				arrayX[x] += comp->input[0].value[j];
				arrayY[y] += comp->input[0].value[j];
				arrayZ[z] += comp->input[0].value[j];
			}
			break;
		case 1: // product
			for (int x = 0; x < sizeX; x++)
				arrayX[x] = 1;
			for (int y = 0; y < sizeY; y++)
				arrayY[y] = 1;
			for (int z = 0; z < sizeZ; z++)
				arrayZ[z] = 1;
			for (int j = 0; j < sizeArray; j++) {
				x = j % sizeX;
				y = floor((j / sizeX) % sizeY);
				z = floor(j / (sizeX * sizeY));
				arrayX[x] *= comp->input[0].value[j];
				arrayY[y] *= comp->input[0].value[j];
				arrayZ[z] *= comp->input[0].value[j];
			}
			break;
		}
		for (x = 0; x < sizeX; x++)
			comp->output[0].value.push_back(arrayX[x]);
		comp->output[0].dimensions.push_back(sizeX);
		for (y = 0; y < sizeY; y++)
			comp->output[1].value.push_back(arrayY[y]);
		comp->output[1].dimensions.push_back(sizeY);
		for (z = 0; z < sizeZ; z++)
			comp->output[2].value.push_back(arrayZ[z]);
		comp->output[2].dimensions.push_back(sizeZ);
		break;
	}
	return true;
}

int Sum_Component::method2(Sheet* pg) {
	return innerProductSum(pg, this, 0);
}

int ProdA_Component::method2(Sheet* pg) {
	return innerProductSum(pg, this, 1);
}

bool onLoadArrayDimensions_(Sheet* pg, Component *comp) {
	int dim;
	if (comp->setting.size() < 1)
		dim = 1;
	else
		dim = comp->setting[0];

	comp->height = pinToHeight(dim);

	// right terminals are outputs
	comp->buildOutputPins(dim);

	switch (dim) {
	case 3:
		comp->output[2].label = "layers";
	case 2:
		comp->output[0].label = "columns";
		comp->output[1].label = "rows";
		break;
	case 1:
		comp->output[0].label = "";
		break;
	}
	return true;
}

void Sum_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = radioValue+1;

	onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
}
void doubleClickSumProduct(Sheet * pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "One Dimension", "Two Dimensions", "Three Dimensions" };
	int radioIndex = pg->dgrm.cL[i]->setting[0]-1;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>sumProductButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickSumProduct) };

bool Sum_Component::onLoad(Sheet* pg) {
	return onLoadArrayDimensions_(pg, this);
}
Sum_Component::Sum_Component()
	: Component(L"Sum", L"Inner Sum", 1, 1)
{
	dialogButtonList = sumProductButtonList;
	setLeftLabels(3, "Array");
	width = 40;
	setSetting(0, 1); //  dimensions
	completionTestRadios = 3;
}

bool ProdA_Component::onLoad(Sheet* pg) {
	return onLoadArrayDimensions_(pg, this);
}
ProdA_Component::ProdA_Component()
	: Component(L"ProdA", L"Inner Product", 1, 1)
{
	dialogButtonList = sumProductButtonList;
	width = 40;
	setLeftLabels(3, "Array");
	setSetting(0, 1); //  dimensions
}



bool methodOuter_(Sheet* pg, Component *comp, int code) {
	if ((comp->input[0].drivenByComponent == -1) ||
		(comp->input[1].drivenByComponent == -1)) // inputs
	{
		for (int j = 0; j < comp->output.size(); j++) {
			comp->output[j].value.clear();
			comp->output[j].dimensions.clear();
		}
		return 1;
	}

	int sizeA = comp->input[0].value.size();
	int sizeB = comp->input[1].value.size();
	//console.log(sizeA, "   ", sizeB);
	comp->output[0].value.resize( sizeA * sizeB);
	int c = 0;
	for (int b = 0; b < sizeB; b++)
		for (int a = 0; a < sizeA; a++)
			switch (code) {
			case 0:
				comp->output[0].value[c++] = comp->input[0].value[a] + comp->input[1].value[b];
				break;
			case 1:
				comp->output[0].value[c++] = comp->input[0].value[a] * comp->input[1].value[b];
				break;
			}


	sizeA = comp->input[0].dimensions.size();
	sizeB = comp->input[1].dimensions.size();
	c = 0;
	comp->output[0].dimensions.resize(sizeA + sizeB);
	if (sizeA + sizeB != 0) {
		for (int a = 0; a < sizeA; a++)
			comp->output[0].dimensions[c++] = comp->input[0].dimensions[a];
		for (int b = 0; b < sizeB; b++)
			comp->output[0].dimensions[c++] = comp->input[1].dimensions[b];
	}
	return true;
}


int OuterSum_Component::method2(Sheet* pg) {
	return methodOuter_(pg, this, 0);
}

OuterSum_Component::OuterSum_Component()
	: Component(L"OuterSum", L"Outer Sum", 2, 1)
{
	setLeftLabels(3, "Array", "Array");
}



int OuterProd_Component::method2(Sheet* pg) {
	return methodOuter_(pg, this, 1);
}
OuterProd_Component::OuterProd_Component()
	: Component(L"OuterProd", L"Outer Product", 2, 1)
{
	setLeftLabels(3, "Array", "Array");
}




int Ave_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
	{
		return clearOutputs();
	}
	double sum = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++)
		sum += input[0].value[j];

	output[0].value[0] = sum / size;
	output[0].dimensions[0] = 1;
	return true;
}

Ave_Component::Ave_Component()
	: Component(L"Ave", L"Average", triangleShape, 1, 1, 1, 1){}




int Median_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	
	std::vector<double> theList;
	int size = input[0].value.size();
	if( size == 0)
		return clearOutputs();

	theList.resize(size);
	for (int j = 0; j < size; j++)
		theList[j] = input[0].value[j];

	bool innerChange;
	bool anyChange = false;
	double temp;
	double delta;
	do {
		innerChange = false;
		for (int j = 0; j < size - 1; j++) {
			delta = theList[j] - theList[j + 1];
			if (delta == 0)
				continue;
			if (delta > 0) {
				temp = theList[j];
				theList[j] = theList[j + 1];
				theList[j + 1] = temp;
				innerChange = true;
			}
		}
		if (innerChange)
			anyChange = true;

	} while (innerChange);
	double med;
	if (size % 2)
		med = theList[size / 2 - 0.5];
	else
		med = (theList[size / 2] + theList[size / 2 - 1]) / 2;
	output[0].value[0] = med;
	output[0].dimensions[0] = 1;
	return true;
}


Median_Component::Median_Component()
	:  Component(L"Median", L"Median", triangleShape, 1, 1, 1, 1){}




int Mode_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	std::vector<double> theValue;
	std::vector<int> theCount;

	int theBiggest = 0;
	int size = input[0].value.size();
	if (size > 0)
		theBiggest = 1;
	int k;
	int j;
	for (j = 0; j < size; j++) { // for each input
		int seenAt = -1;
		for (k = 0; k < theValue.size(); k++) // search to see if its been seen before
			if (theValue[k] == input[0].value[j]) {
				theCount[k]++; // count it
				if (theCount[k] > theBiggest)
					theBiggest = theCount[k]; // we have a new Mode candidate
				break;
			}
		if (seenAt == -1) { // add it to the list
			theValue.push_back(input[0].value[j]);
			theCount.push_back(1);
		}
	}
	
	size = theValue.size();
	output[0].value.clear();
	for (int k = 0; k < size; k++) {
		if (theCount[k] == theBiggest)
			output[0].value.push_back( theValue[k]);
	}
	output[0].dimensions[0] = output[0].value.size();
	return true;
}

Mode_Component::Mode_Component()
	: Component(L"Mode", L"Mode", triangleShape, 1, 1, 1, 1)
{}


int Stdev_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	double sum = 0;
	double squares = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		sum += input[0].value[j];
		squares += input[0].value[j] * input[0].value[j];
	}

	output[0].value[0] = sqrt((squares - sum * sum / size) / (size - 1));
	output[0].dimensions[0] = 1;
	return true;
}

Stdev_Component::Stdev_Component()
	:Component(L"Stdev", L"Std. Dev.", triangleShape, 1, 1, 1, 1){}


int Var_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	double sum = 0;
	double squares = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		sum += input[0].value[j];
		squares += input[0].value[j] * input[0].value[j];
	}

	output[0].value[0] = (squares - sum * sum / size) / (size - 1);
	output[0].dimensions[0] = 1;
	return true;
}

Var_Component::Var_Component()
	: Component(L"Var", L"Variance", triangleShape, 1, 1, 1, 1){}


int Correlation_Component::method2(Sheet* pg) {

	int dataSets = setting[0];
	int dataPoints = input[0].value.size();
	if (dataPoints < 2)
		return clearOutputs();
	for (int n = 1; n < dataSets; n++) {
		if (input[n].value.size() != dataPoints)
			return clearOutputs();
	}

	// calc the mean for each data set
	std::vector<double> mean;
	mean.resize(dataSets);
	for (int d = 0; d < dataSets; d++) {
		double sum = 0;
		for (int p = 0; p < dataPoints; p++) {
			sum += input[d].value[p];
		}
		mean[d] = sum / dataPoints;
	}

	// here we go

	// first the diagonal
	output[0].value.resize(dataSets*dataSets);
	for (int dA = 0; dA < dataSets; dA++) {
		int j = dA + dA * dataSets;
		output[0].value[j] = 1;
	}

	// then half the rest
	int j;
	for (int dA = 0; dA < dataSets; dA++) {
		for (int dB = dA + 1; dB < dataSets; dB++) {

			int numer = 0;
			for (int p = 0; p < dataPoints; p++) {
				numer += (input[dA].value[p] - mean[dA]) *
					(input[dB].value[p] - mean[dB]);
			}

			double aSquared = 0;
			double bSquared = 0;
			for (int p = 0; p < dataPoints; p++) {
				aSquared += (input[dA].value[p] - mean[dA]) *
					(input[dA].value[p] - mean[dA]);
				bSquared += (input[dB].value[p] - mean[dB]) *
					(input[dB].value[p] - mean[dB]);
			}
			double denom = sqrt(aSquared * bSquared);
			j = dA + dB * dataSets;
			output[0].value[j] = numer / denom;
			j = dB + dA * dataSets;
			output[0].value[j] = numer / denom;
		}
	}
	output[0].dimensions.resize(2);
	output[0].dimensions[0] = dataSets;
	output[0].dimensions[1] = dataSets;
	return true;
}



bool Correlation_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);

	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	return true;
}

Correlation_Component::Correlation_Component()
	: Component(L"Corr", L"Correlation", 2, 1)
{
	setSetting(0, 2);
	//dialogList = dialogCorrelationButtonList;
}



int Sums_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	double sum = 0;
	int size = input[0].value.size();
	output[0].value.resize(size);
	for (int j = 0; j < size; j++) {
		sum += input[0].value[j];
		output[0].value[j] = sum;
	}
	output[0].dimensions[0] = size;
	return true;
}

Sums_Component::Sums_Component()
	: Component(L"Sums", L"Array Sums", 1, 1){}



int Differences_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int size = input[0].value.size();
	output[0].value.resize(size);
	for (int j = 1; j < size; j++)
		output[0].value[j - 1] = input[0].value[j] - input[0].value[j - 1];
	output[0].dimensions[0] = size - 1;
	return true;
}

Differences_Component::Differences_Component()
	: Component(L"Difs", L"Array Diffs", 1, 1)
{}
	

int ArrayRand_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();
	if (input[0].drivenByComponent == -1)    // count
		return clearOutputs();
	if (input[0].value.size() == 0)
		return clearOutputs();

	int count = floor(input[0].value[0]);
	if (count < 1)
		return clearOutputs();

	bool renew = false;
	if (memory[0] != count) {
		memory[0] = count;
		renew = true;
	}

	if (input[1].drivenByComponent != -1) {   // renew
		if (input[1].value[0]) {
			if (memory[1] == 0) {
				memory[1] = 1; // renew just went high
				renew = true;
			}
		}
		else
			memory[1] = 0;
	}

	if (!renew)
		return false;

	output[0].value.resize(count);
	for (int j = 0; j < count; j++)
		output[0].value[j] = (double)rand() / (double)RAND_MAX;
	output[0].dimensions[0] = count;
	return true;
}

ArrayRand_Component::ArrayRand_Component()
	: Component(L"ArrayRand", L"Array Rands", 2, 1)
{
	setLeftLabels(3, "Count", "Renew");
	setMemory(0, 0); // count change detection
	setMemory(1, 0); // edge detection for renew
}
	



int MatMul_Component::method2(Sheet* pg) {

	// need both inputs
	if ((input[0].drivenByComponent == -1) ||
		(input[1].drivenByComponent == -1))
		return clearOutputs();

	if ((input[0].dimensions.size() < 1) ||
		(input[1].dimensions.size() < 1))
		return clearOutputs();

	int mSize1 = input[0].value.size();
	int xSize1 = input[0].dimensions[0]; // i to n
	int ySize1;
	if (input[0].dimensions.size() > 1)
		ySize1 = input[0].dimensions[1]; // k to m
	else
		ySize1 = 1;

	int mSize2 = input[1].value.size();
	int xSize2 = input[1].dimensions[0]; // k to m
	int ySize2;
	if (input[1].dimensions.size() > 1)
		ySize2 = input[1].dimensions[1];   // j to p
	else
		ySize2 = 1;

	if ((mSize1 < 1) || (xSize1 < 1) || (mSize2 < 1) || (xSize2 < 1))
		return clearOutputs();

	if (mSize1 == 1) {
		output[0].value.resize(mSize2);
		// constant times matrix
		for (int ii = 0; ii < mSize2; ii++)
			output[0].value[ii] = input[0].value[0] * input[1].value[ii];
		output[0].dimensions.resize(2);
		output[0].dimensions[0] = xSize2;
		output[0].dimensions[1] = ySize2;
		return 1;
	}

	if (mSize2 == 1) {
		output[0].value.resize(mSize1);
		// constant times matrix
		for (int ii = 0; ii < mSize1; ii++)
			output[0].value[ii] = input[0].value[ii] * input[1].value[0];
		output[0].dimensions.resize(2);
		output[0].dimensions[0] = xSize1;
		output[0].dimensions[1] = ySize1;
		return 1;
	}

	if (xSize1 != ySize2)
		return 1;

	output[0].value.resize(xSize2 * ySize1);
	output[0].dimensions.resize(2);
	output[0].dimensions[0] = xSize2;
	output[0].dimensions[1] = ySize1;
	int z = 0;

	for (int ii = 0; ii < ySize1; ii++) {
		for (int jj = 0; jj < xSize2; jj++) {
			double sum = 0;
			for (int kk = 0; kk < xSize1; kk++) {
				double A = input[0].value[ii * xSize1 + kk];
				double B = input[1].value[kk * xSize2 + jj];
				sum += A * B;
			}
			output[0].value[z++] = sum;
		}
	}
	return 1;
}


MatMul_Component::MatMul_Component()
	: Component(L"MatMul", L"Matrix Multiply", 2, 1)
{
	setLeftLabels(3, "Matrix 0", "Matrix 1");
	setRightLabels(3, "Matrix");
	width = 40;
}
	
bool methodMatrixInvert_(Sheet* pg) {
	return false;
	/*
	if ((input[0].drivenByComponent == -1) ||
		(input[0].dimensions.size() != 2))
		return clearOutputs();

	int mSize = input[0].value.size();
	int xSize = input[0].dimensions[0]; // i to n
	int ySize = input[0].dimensions[1]; // k to m

	if ((mSize < 1) || (xSize < 1) || (xSize != ySize))
		return clearOutputs();

	int n = xSize;
	var a = [];
	int k, m;
	for (int j = 0; j <= n; j++)
		a[j] = [];
	j = 1;
	k = 1;
	for (m = 0; m < mSize; m++) {
		a[j][k] = input[0].value[m];
		j++;
		if (j > n) {
			j = 1;
			k++;
		}
	}

	int result = ludcmp(a, n);//, index, &determinant ) != 0 )
	if (!result.success)
		return clearOutputs();

	output[0].value = [];
	output[0].value.size() = xSize * ySize;
	output[0].dimensions = [xSize, ySize];

	var matY = [];
	for (j = 0; j <= n; j++)
		matY[j] = [];

	var column = [];
	int index = result.indx;
	m = 0;
	for (int jj = 1; jj <= n; jj++) {
		for (int ii = 1; ii <= n; ii++)
			column[ii] = 0.0;

		column[jj] = 1.0;

		double outCol = lubksb(a, n, index, column);

		for (int ii = 1; ii <= n; ii++)
			output[0].value[m++] = outCol[ii];
	}

	return true;*/
}

//MatInv_Component::MatInv_Component()
//	: Component(L"MatInv", L"Matrix Invert", methodMatrixInvert_, 1, 1)
//{
//	setLeftLabels(3, "Matrix");	
//}
//	
int MatrixTranspose_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	int mSize = input[0].value.size();
	if (input[0].dimensions.size() != 2)
		return true;
	int xSize = input[0].dimensions[0]; // i to n
	int ySize = input[0].dimensions[1];   // k to m

	if ((mSize < 1) || (xSize < 1) ||
		(xSize * ySize != mSize))
		return true;

	output[0].value.resize(mSize);
	output[0].dimensions.resize(2);
	output[0].dimensions[0] = ySize;
	output[0].dimensions[1] = xSize;

	int z = 0;
	int kk;
	for (int jj = 0; jj < xSize; jj++) {
		for (int ii = 0; ii < ySize; ii++) {
			kk = ii * xSize + jj;
			output[0].value[z++] = input[0].value[kk];
		}
	}

	if (input[0].dimensions.size() == 2) {
		output[0].dimensions[0] = input[0].dimensions[1];
		output[0].dimensions[1] = input[0].dimensions[0];
	}

	return true;
}

MatrixTranspose_Component::MatrixTranspose_Component()
	: Component(L"MatTran", L"Matrix Transpose", 1, 1)
{
	setLeftLabels(3, "Matrix");
	setRightLabels(3, "Matrix");
	width = 40;
}


bool ludcmp(double a[20][20], int n, int *indx, double *d) 
{
	//int ludcmp( double **a, int n, int *indx, double *d )
	int i, imax, j, k;
	double big, dum, sum, temp;
	double vv[20];
	*d = 1.0;
	
	for (i = 1; i <= n; i++) {
		big = 0.0;
		for (j = 1; j <= n; j++)
			if ((temp = abs(a[i][j])) > big)
				big = temp;
		if (big == 0.0) //nrerror( "Singular matrix in routine ludcmp" );
			return false;

		vv[i] = 1.0 / big;
	}
	for (j = 1; j <= n; j++) {
		for (i = 1; i < j; i++) {
			sum = a[i][j];
			for (k = 1; k < i; k++)
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
		}
		big = 0.0;
		for (i = j; i <= n; i++) {
			sum = a[i][j];
			for (k = 1; k < j; k++)
				sum -= a[i][k] * a[k][j];
			a[i][j] = sum;
			if ((dum = vv[i] * abs(sum)) >= big) {
				big = dum;
				imax = i;
			}
		}
		if (j != imax) {
			for (k = 1; k <= n; k++) {
				dum = a[imax][k];
				a[imax][k] = a[j][k];
				a[j][k] = dum;
			}
			*d = -(*d);
			vv[imax] = vv[j];
		}
		indx[j] = imax;
		if (a[j][j] == 0.0)
			a[j][j] = 1.0e-200;
		if (j != n) {
			dum = 1.0 / (a[j][j]);
			for (i = j + 1; i <= n; i++)
				a[i][j] *= dum;
		}
	}
	return true; 
};



int MatrixDeterminent_Component::method2(Sheet* pg) {

	int arraySize = input[0].value.size();
	if (arraySize < 2)
		return clearOutputs();

	int n = floor(sqrt(arraySize));
	if (n * n != arraySize)
		return clearOutputs();

	if (n > 20)return 0;
	if (arraySize > 20)return 0;

	double a[20][20];
	
	int j, k, m;
	
	j = 1;
	k = 1;
	for (m = 0; m < arraySize; m++) {
		a[j][k] = input[0].value[m];
		j++;
		if (j > n) {
			j = 1;
			k++;
		}
	}

	int indx[20];
	double d;
	bool result = ludcmp(a, n, indx, &d);
	if (!result)
		return 1;

	for (j = 1; j <= n; j++)
		d *= a[j][j];
	output[0].value[0] = d;
	output[0].dimensions[0] = 1;
	return 1;
}

MatrixDeterminent_Component::MatrixDeterminent_Component()
	: Component(L"MatDet", L"Determinant", 1, 1)
{
	setLeftLabels(3, "Matrix");	
}


int Mirror_Component::method2(Sheet* pg) {
	int j;

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	bool isString = input[0].stringType;
	output[0].stringType = isString;

	if (isString) {
		int size = input[0].stringValue.size();
		output[0].stringValue.resize(size);
		for (j = 0; j < size; j++) {
			output[0].stringValue[size - j - 1] = input[0].stringValue[j];
		}
	}
	else {
		int size = input[0].value.size();
		output[0].value.resize(size);
		for (j = 0; j < size; j++) {
			output[0].value[size - j - 1] = input[0].value[j];
		}
	}
	copyDimensions(0, 0);
	return true;
}
Mirror_Component::Mirror_Component()
	: Component(L"Mirror", L"Mirror", 1, 1){}






int Repeat_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();

	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int nSize;
	if( isString )
		nSize = input[0].stringValue.size();
	else
		nSize = input[0].value.size();

	int mSize = input[1].value.size();

	// quick safety check
	int outsize = 0;
	for (int m = 0; m < mSize; m++)
		outsize += input[1].value[m];
	if (outsize * nSize > 1e6)
		return clearOutputs();

	output[0].stringValue.clear();
	output[0].value.clear();

	for (int m = 0; m < mSize; m++) {
		int rep = floor(input[1].value[m]);

		for (int n = 0; n < nSize; n++) {
			for (int k = 0; k < rep; k++) {
				if( isString )
					output[0].stringValue.push_back(input[0].stringValue[n]);
				else
					output[0].value.push_back(input[0].value[n]);
			}
		}
	}
	
	output[0].dimensions.clear();
	for (int k = 0; k < input[1].value.size(); k++)
		if( input[1].value[k] != 1)
			output[0].dimensions.push_back(input[1].value[k]);

	for (int k = 0; k < input[0].dimensions.size(); k++)
		output[0].dimensions.push_back(input[0].dimensions[k]);
	
	return true;
}
Repeat_Component::Repeat_Component()
	: Component(L"Repeat", L"Inner Repeat", 2, 1)
{
	setLeftLabels(3, "Array", "Reps");
}

// *********************** Duplicate ****************************************


int Duplicate_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();
	if( input[1].value.size() == 0)
		return clearOutputs();
	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int size;
	if (isString)
		size = input[0].stringValue.size();
	else
		size = input[0].value.size();

	int loops = floor(input[1].value[0]);
	
	if (loops * size > 1e6)
		return true; // for safety
	
	output[0].stringValue.clear();
	output[0].value.clear();

	for (int k = 0; k < loops; k++) {
		for (int jin = 0; jin < size; jin++) {
			if (isString)
				output[0].stringValue.push_back(input[0].stringValue[jin]);
			else
				output[0].value.push_back( input[0].value[jin]);
		}
	}

	output[0].dimensions.clear();
	for (int k = 0; k < input[0].dimensions.size(); k++)
		output[0].dimensions.push_back(input[0].dimensions[k]);

	for (int k = 0; k < input[1].value.size(); k++)
		if (input[1].value[0] != 1)
			output[0].dimensions.push_back(input[1].value[k]);

	return true;
}
Duplicate_Component::Duplicate_Component()
	: Component(L"Dup", L"Duplicate", 2, 1)
{
	setLeftLabels(3, "Array", "Dups");
}



int Min_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	double min = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		if ((j == 0) || (input[0].value[j] < min)) {
			min = input[0].value[j];
		}
	}

	output[0].value[0] = min;
	output[0].dimensions[0] = 1;
	return true;
}

Min_Component::Min_Component()
	: Component(L"Min", L"Minimum", triangleShape, 1, 1, 1, 1){}




int Max_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	double max = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		if ((j == 0) || (input[0].value[j] > max)) {
			max = input[0].value[j];
		}
	}

	output[0].value[0] = max;
	output[0].dimensions[0] = 1;
	return true;
}

Max_Component::Max_Component()
	: Component(L"Max", L"Maximum", triangleShape, 1, 1, 1, 1){}



int Range_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	double min = 0;
	double max = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		if ((j == 0) || (input[0].value[j] < min))
			min = input[0].value[j];

		if ((j == 0) || (input[0].value[j] > max))
			max = input[0].value[j];
	}

	output[0].value[0] = max - min;
	output[0].dimensions[0] = 1;
	return true;
}


Range_Component::Range_Component()
	: Component(L"Range", L"Data Range", 1, 1){}




int MaxIndex_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	int high = 0;
	double max = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		if ((j == 0) || (input[0].value[j] > max)) {
			max = input[0].value[j];
			high = j;
		}
	}

	output[0].value[0] = high;
	output[0].dimensions[0] = 1;
	return true;
}

MaxIndex_Component::MaxIndex_Component()
	: Component(L"MaxIdx", L"Max Index", 1, 1){}




int MinIndex_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();

	int low = 0;
	double min = 0;
	int size = input[0].value.size();
	for (int j = 0; j < size; j++) {
		if ((j == 0) || (input[0].value[j] < min)) {
			min = input[0].value[j];
			low = j;
		}
	}

	output[0].value[0] = low;
	output[0].dimensions[0] = 1;
	return true;
}

MinIndex_Component::MinIndex_Component()
	: Component(L"MinIdx", L"Min Index", 1, 1)
{
};




int Small_Component::method2(Sheet* pg) {
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	
	output[0].value.resize(loops);
	for (int j = 0; j < loops; j++) {
		double data = getLoopingValueD(0, j, 0);
		int reset = getLoopingValueD(1, j, 0);
		if (reset)
			output[0].value[j] = data;
		else {
			output[0].value[j] = min(data, output[0].value[j]);
		}
	}
	return true;
}

Small_Component::Small_Component()
	: Component(L"Small", L"Smallest", 2, 1)
{
	setLeftLabels(3, "Data", "Reset");
};




int Large_Component::method2(Sheet* pg) {
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	
	output[0].value.resize(loops);
	for (int j = 0; j < loops; j++) {
		double data = getLoopingValueD(0, j, 0);
		int reset = getLoopingValueD(1, j, 0);
		if (reset)
			output[0].value[j] = data;
		else {
			output[0].value[j] = max(data, output[0].value[j]);
		}
	}
	return true;
}


Large_Component::Large_Component()
	: Component(L"Large", L"Largest", 2, 1)
{
	setLeftLabels(3, "Data", "Reset");
};




int Roll_Component::method2(Sheet* pg) {
	//output[0].value.resize() = 0;
	//setSimpleOutputDimensions(pg->dgrm.cL[i]);

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	if (input[1].drivenByComponent == -1)    // amount
		return clearOutputs();

	int shift = floor(getDoubleInputOrZero(1, 0));
	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int size;
	if( isString )
		size = input[0].stringValue.size();
	else
		size = input[0].value.size();
	if (size <= 0)
		return clearOutputs();

	if (isString)
		output[0].stringValue.resize(size);
	else
		output[0].value.resize(size);
	for (int j = 0; j < size; j++) {
		int source = (j + shift) % size;
		if (source < 0)
			source += size;
		if (isString)
			output[0].stringValue[j] = input[0].stringValue[source];
		else
			output[0].value[j] = input[0].value[source];
	}
	copyDimensions(0, 0);
	return true;
}

Roll_Component::Roll_Component()
	: Component(L"Roll", L"Roll", 2, 1)
{
	setLeftLabels(3, "Array", "Amount");
};


int FirstSet_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	if (input[1].drivenByComponent == -1)    // first count
		return clearOutputs();

	int count = floor(getDoubleInputOrZero(1, 0));
	if (count < 0)
		count = 0;

	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int size;
	if( isString )
		size = input[0].stringValue.size();
	else
		size = input[0].value.size();
	if (count > size)
		count = size;

	if (isString) {
		output[0].stringValue.resize(count);
		for (int j = 0; j < count; j++) {
			output[0].stringValue[j] = input[0].stringValue[j];
		}
	}
	else {
		output[0].value.resize(count);
		for (int j = 0; j < count; j++) {
			output[0].value[j] = input[0].value[j];
		}
	}

	setSimpleOutputDimensions();
	return true;
}

FirstSet_Component::FirstSet_Component()
	: Component(L"FirstSet", L"First N", 2, 1)
{
	setLeftLabels(3, "Array", "N");
};


int LastSet_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	if (input[1].drivenByComponent == -1)    // first count
		return clearOutputs();

	int count = floor(getDoubleInputOrZero(1, 0));
	if (count < 0)
		count = 0;

	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int size;
	if (isString)
		size = input[0].stringValue.size();
	else
		size = input[0].value.size();
	if (count > size)
		count = size;

	if (isString) {
		output[0].stringValue.resize(count);
		int k = size - count;
		for (int j = 0; j < count; j++) 
			output[0].stringValue[j] = input[0].stringValue[k++];
	}
	else
	{
		output[0].value.resize(count);
		int k = size - count;
		for (int j = 0; j < count; j++) 
			output[0].value[j] = input[0].value[k++];
	}
	setSimpleOutputDimensions();
	return true;
}

LastSet_Component::LastSet_Component()
	: Component(L"LastSet", L"Last N", 2, 1)
{
	setLeftLabels(3, "Array", "N");
};


int SubSet_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int size;
	if( isString )
		size = input[0].stringValue.size();
	else
		size = input[0].value.size();

	int start;
	if (input[1].drivenByComponent == -1)    // start
		start = 0;
	else
		start = floor(getDoubleInputOrZero(1, 0));
	if (start < 0)
		start = 0;

	int len;
	if (input[2].drivenByComponent == -1)    // length
		len = size - start;
	else
		len = floor(getDoubleInputOrZero(2, 0));
	if (len < 0)
		len = 0;

	output[0].value.clear();
	output[0].stringValue.clear();

	if (start > size)
		return true;

	if (start + len > size)
		len = size - start;

	
	for (int j = 0; j < len; j++) {
		if (isString) 
			output[0].stringValue.push_back(input[0].stringValue[start + j]);
		else
			output[0].value.push_back(input[0].value[start + j]);
	}

	setSimpleOutputDimensions();
	return true;
}


SubSet_Component::SubSet_Component()
	: Component(L"SubSet", L"Subset", 3, 1)
{
	setLeftLabels(3, "Array", "First", "Length");
};


int Excise_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)    // array
		return clearOutputs();
	if (input[1].drivenByComponent == -1)    // start
		return clearOutputs();
	if (input[2].drivenByComponent == -1)    // length
		return clearOutputs();
	
	bool isString = input[0].stringType;
	output[0].stringType = isString;
	int dataSize;
	if( isString )
		dataSize = input[0].stringValue.size();
	else
		dataSize = input[0].value.size();

	int start = floor(getDoubleInputOrZero(1, 0));
	if ((start < 0) || (start >= dataSize))
		return clearOutputs();
	int len = floor(getDoubleInputOrZero(2, 0));
	if (len < 0)
		len = 0;
	if (start + len > dataSize)
		len = dataSize - start;
	if (start + len > dataSize)
		len = dataSize - start;

	output[0].value.clear();
	output[0].stringValue.clear();
	if (isString) {
		for (int j = 0; j < start; j++)
			output[0].stringValue.push_back(input[0].stringValue[j]);
		for (int j = start + len; j < dataSize; j++)
			output[0].stringValue.push_back(input[0].stringValue[j]);
	}
	else {
		for (int j = 0; j < start; j++)
			output[0].value.push_back(input[0].value[j]);
		for (int j = start + len; j < dataSize; j++)
			output[0].value.push_back(input[0].value[j]);
	}
	//setSimpleOutputDimensions(pg->dgrm.cL[i]);
	output[0].dimensions[0] = output[0].value.size();
	return true;
}

Excise_Component::Excise_Component()
	: Component(L"Excise", L"Excise", 3, 1)
{
	setLeftLabels(3, "Array", "Start", "Length");
};







bool commonCrankRack_(Sheet* pg, Component *comp, int ax, int ay, bool check) {
	int sliders = comp->setting[2];

	if (check) {
		if (ax > comp->x + sliders * TERMINAL_SIZE)
			return false; // missed dial
		if (ax < comp->x - sliders * TERMINAL_SIZE)
			return false; // missed dial
	}

	int top = comp->y - comp->height + TERMINAL_SIZE;
	int bot = comp->y + comp->height - TERMINAL_SIZE;
	double yScale = (top - bot) / (comp->setting[1] - comp->setting[0]);

	// figure out which slider is changing
	int slider = sliders - 1 - floor((comp->x + sliders * TERMINAL_SIZE - ax) / (2 * TERMINAL_SIZE));

	if (check) {
		// slider = floor((x + sliders * TERMINAL_SIZE - x) / (2 * TERMINAL_SIZE));
		// use memory to remember which one is moving
		comp->memory[0] = slider;

		//  first find the current dial position
		double currentY = (comp->detent(4, 5 + slider) - comp->setting[0]) * yScale + bot;
		if (ay > currentY + TERMINAL_SIZE)
			return false; // missed dial
		if (ay < currentY - TERMINAL_SIZE)
			return false; // missed dial
	}
	else
		slider = comp->memory[0];

	if (ay > bot)
		ay = bot;

	if (ay < top)
		ay = top;

	comp->setting[5 + slider] = reasonableValue(overScale((ay - bot), yScale) + comp->setting[0]);

	return true;
}





void commonDialRack(Sheet* pg, Component *comp) {
	int top = comp->y - comp->height + TERMINAL_SIZE;
	int bot = comp->y + comp->height - TERMINAL_SIZE;

	int sliders = comp->setting[2];
	int trackX = pg->cXo + comp->x;
	trackX -= (sliders - 1) * TERMINAL_SIZE;

	for (int j = 0; j < sliders; j++) {
		int y = (comp->detent(4, 5 + j) - comp->setting[0]) /
			(comp->setting[1] - comp->setting[0]) *
			(top - bot) + bot;

		// first the vertical track
		BeginPath(pg->hDC);
		MoveToEx(pg->hDC, trackX, pg->cYo + top, NULL);
		LineTo(pg->hDC, trackX, pg->cYo + bot);
		EndPath(pg->hDC);
		StrokeAndFillPath(pg->hDC);

		// then the knob
		BeginPath(pg->hDC);
		Ellipse(pg->hDC, trackX - TERMINAL_SIZE, pg->cYo + y - TERMINAL_SIZE,
			trackX + TERMINAL_SIZE, pg->cYo + y + TERMINAL_SIZE);
		EndPath(pg->hDC);
		SelectObject(pg->hDC, GetStockObject(WHITE_BRUSH)); // '#FFFFFF';  // white
		StrokeAndFillPath(pg->hDC);

		// draw the knob numbers 
		std::string aString = tomString(j);
		//int width = calcStringWidthFont(pg->hDC, aString);
		drawTextString(pg->hDC, trackX - 4, pg->cYo + y - 6, 0, aString);

		trackX += 2 * TERMINAL_SIZE;
	}

	// show the value of the last dial moved above the rack
	int j = comp->memory[0];
	if (j < comp->setting.size()) {
		std::string text = "[" + tomString(j) + "] = ";
		text += tomString(comp->detent(4, 5 + j));
		int width = calcStringWidthFont(pg->hDC, text);
		drawTextString(pg->hDC, pg->cXo + comp->x - width / 2, pg->cYo + comp->y - comp->height - 14, 0, text);
	}
}



// setting[0] is min
	// setting[1] is max
	// setting[2] is number of sliders
	// setting[3] is the height;
	// setting[4] is detent
	// setting[5] is first slider value
int RackZ_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);

	// this is the rack of constants - send setting out
	int knobs = setting[2];
	output[0].value.resize(knobs);
	for (int j = 0; j < knobs; j++)
		output[0].value[j] = detent(4, 5 + j);//setting[5 + j];
	output[0].dimensions[0] = knobs;

	width = (setting[2] + 1) * TERMINAL_SIZE;
	height = setting[3];
	return true;
}

bool RackZ_Component::crank(Sheet* pg, int x, int y, bool check) {
	if (!commonCrankRack_(pg, this, x, y, check))
		return false;

	// move the dial
	dial(pg);
	return method2(pg);
}

void RackZ_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	for (int j = 0; j < 5; j++)
		setting[j] = doubleValues[j];

	int dials = floor(setting[2]);
	if (dials > 32) {
		dials = 32;
		setting[2] = 32;
	}
	if (dials < 1) {
		dials = 1;
		setting[2] = 1;
	}

	if (setting.size() != 5 + dials) {
		setting.resize(5 + dials);
		output[0].value.resize(dials);
	}

	// check for bad X limits
	if (setting[1] < setting[0]) {
		setting[0] = 0;
		setting[1] = 100;
		for (int k = 0; k < dials; k++) {
			setting[5 + k] = 0;
			if (type == L"RackZ")
				output[0].value[k] = 0;
		}
	}
	else // be sure output is in limits
	{
		for (int k = 0; k < dials; k++) {
			if (setting[5 + k] < setting[0]) {
				setting[5 + k] = setting[0];
				if (type == L"RackZ")
					output[0].value[k] = setting[0];
			}
			else if (setting[5 + k] > setting[1]) {
				setting[5 + k] = setting[1];
				if (type == L"RackZ")
					output[0].value[k] = setting[1];
			}
			else
				if (type == L"RackZ")
					output[0].value[k] = setting[5 + k];
		}
	}


}


void buttonRack_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Minimum:", "Maximum:", "Knobs:", "Vertical Size:", "Detent:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[1], 
		pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

void buttonRack_SetRangeAction(Sheet* pg, int i, double minimum, double maximum) {

	pg->dgrm.cL[i]->setSetting(0, minimum);      // Minimum
	pg->dgrm.cL[i]->setSetting(1, maximum);    // Maximum

	int count = pg->dgrm.cL[i]->setting[2];
	for (int j = 0; j < count; j++) {
		int k = j + 5;
		if (pg->dgrm.cL[i]->setting[k] < minimum)
			pg->dgrm.cL[i]->setting[k] = minimum;
		if (pg->dgrm.cL[i]->setting[k] > maximum)
			pg->dgrm.cL[i]->setting[k] = maximum;
		pg->dgrm.cL[i]->output[0].value[j] = pg->dgrm.cL[i]->setting[k];
	}

	pg->dgrm.cL[i]->method2(pg);
	propagateValues(&pg->dgrm);

	pg->activeDialogComponent = -1;
	drawCanvas();
	drawCanvas2();
}
void buttonRack_SetRange0Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, 0, 255); }
void buttonRack_SetRange1Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, -100, 100); }
void buttonRack_SetRange2Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, 0, 100); }
void buttonRack_SetRange3Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, -10, 10); }
void buttonRack_SetRange4Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, 0, 10); }
void buttonRack_SetRange5Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, 0, TWO_PI); }
void buttonRack_SetRange6Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, -PI, PI); }
void buttonRack_SetRange7Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, -1, 1); }
void buttonRack_SetRange8Action(Sheet* pg, int i) { buttonRack_SetRangeAction(pg, i, 0, 1); }


std::vector<Button*> rackButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonRack_SetOptions_Action),
	new Button(-117, 40, 115, 20, L"Range: 0, 255", buttonRack_SetRange0Action),
	new Button(-117, 60, 115, 20, L"Range: -100, 100", buttonRack_SetRange1Action),
	new Button(-117, 80, 115, 20, L"Range: 0, 100", buttonRack_SetRange2Action),
	new Button(-117, 100, 115, 20, L"Range: -10, 10", buttonRack_SetRange3Action),
	new Button(-117, 120, 115, 20, L"Range: 0, 10", buttonRack_SetRange4Action),
	new Button(-117, 140, 115, 20, L"Range: 0, 2*Pi", buttonRack_SetRange5Action),
	new Button(-117, 160, 115, 20, L"Range: -Pi, Pi", buttonRack_SetRange6Action),
	new Button(-117, 180, 115, 20, L"Range: -1, 1", buttonRack_SetRange7Action),
	new Button(-117, 200, 115, 20, L"Range: 0, 1", buttonRack_SetRange8Action) };

void RackZ_Component::dial(Sheet* pg) {
	commonDialRack(pg, this);
}
bool RackZ_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
RackZ_Component::RackZ_Component()
	: Component(L"RackZ", L"Rack", squareShape, 0, 0, 1, 1)
{
	dialogButtonList = rackButtonList;
	setMemory(0, 0); // as if the zero knob was recently moved
	//hover = hoverRack;
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 3);      // outputs
	height = 50;
	setSetting(3, height); // height
	width = (setting[2] + 1) * TERMINAL_SIZE;
	setSetting(4, 0);      // detent
	setSetting(5, 0);      // value[0]
	setSetting(6, 0);      // value[1]
	setSetting(7, 0);      // value[2]
	setOutput(0, 0, 0.0);
	setOutput(0, 1, 0.0);
	setOutput(0, 2, 0.0);

};

// **************** factor

int RackFacZ_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);

	// this is the rack of constants
	int count;
	if (input[0].value.size() == 1)
		count = setting[2];
	else
		count = min(setting[2], input[0].value.size());
	output[0].value.resize(count);

	if (setting[2] == input[0].value.size())
		copyDimensions(0, 0);
	else
		output[0].dimensions[0] = count;

	int jj;
	for (int j = 0; j < count; j++) {
		if (input[0].value.size() == 1)
			jj = 0;
		else
			jj = j;
		
		output[0].value[j] = reasonableValue(input[0].value[jj] * detent(4, 5 + j));
	}
	width = (setting[2] + 1) * TERMINAL_SIZE;
	height = setting[3];
	return true;
}

bool RackFacZ_Component::crank(Sheet* pg, int x, int y, bool check) {
	if (!commonCrankRack_(pg, this, x, y, check))
		return false;

	// move the dial
	dial(pg);
	return method2(pg);
}

void RackFacZ_Component::dial(Sheet* pg) {
	commonDialRack(pg, this);
}
bool RackFacZ_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
void RackFacZ_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	for (int j = 0; j < 5; j++)
		setting[j] = doubleValues[j];

	int dials = floor(setting[2]);
	if (dials > 32) {
		dials = 32;
		setting[2] = 32;
	}
	if (dials < 1) {
		dials = 1;
		setting[2] = 1;
	}

	if (setting.size() != 5 + dials) {
		setting.resize(5 + dials);
		output[0].value.resize(dials);
	}

	// check for bad X limits
	if (setting[1] < setting[0]) {
		setting[0] = 0;
		setting[1] = 100;
		for (int k = 0; k < dials; k++) {
			setting[5 + k] = 0;
			if (type == L"RackZ")
				output[0].value[k] = 0;
		}
	}
	else // be sure output is in limits
	{
		for (int k = 0; k < dials; k++) {
			if (setting[5 + k] < setting[0]) {
				setting[5 + k] = setting[0];
				if (type == L"RackZ")
					output[0].value[k] = setting[0];
			}
			else if (setting[5 + k] > setting[1]) {
				setting[5 + k] = setting[1];
				if (type == L"RackZ")
					output[0].value[k] = setting[1];
			}
			else
				if (type == L"RackZ")
					output[0].value[k] = setting[5 + k];
		}
	}


}
RackFacZ_Component::RackFacZ_Component()
	: Component(L"RackFacZ", L"Rack Factor", 1, 1)
{
	dialogButtonList = rackButtonList;
	setMemory(0, 0); // as if the zero knob was recently moved
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 3);      // outputs
	height = 50;
	setSetting(3, height); // height
	width = (setting[2] + 1) * TERMINAL_SIZE;
	setSetting(4, 0);      // detent
	setSetting(5, 1);      // value[0]
	setSetting(6, 1);      // value[1]
	setSetting(7, 1);      // value[2]
}

// ************** offset

int RackOffZ_Component::method2(Sheet* pg) {
	//upgradeAddDetent(pg, i);
	int count;
	if (input[0].value.size() == 1)
		count = setting[2];
	else
		count = min(setting[2], input[0].value.size());
	output[0].value.resize(count);

	if (setting[2] == input[0].value.size())
		copyDimensions(0, 0);
	else
		output[0].dimensions[0] = count;

	// this is the rack of constants - send setting out
	int jj;
	for (int j = 0; j < count; j++) {
		if (input[0].value.size() == 1)
			jj = 0;
		else
			jj = j;
		
		output[0].value[j] = reasonableValue(input[0].value[jj] + detent(4, 5 + j));
	}
	width = (setting[2] + 1) * TERMINAL_SIZE;
	height = setting[3];
	return true;
}
bool RackOffZ_Component::crank(Sheet* pg, int x, int y, bool check) {
	if (!commonCrankRack_(pg, this, x, y, check))
		return false;

	// move the dial
	dial(pg);
	return method2(pg);
}
void RackOffZ_Component::dial(Sheet* pg) {
	commonDialRack(pg, this);
}
bool RackOffZ_Component::onLoad(Sheet* pg) {
	return method2(pg);
}
void RackOffZ_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	for (int j = 0; j < 5; j++)
		setting[j] = doubleValues[j];

	int dials = floor(setting[2]);
	if (dials > 32) {
		dials = 32;
		setting[2] = 32;
	}
	if (dials < 1) {
		dials = 1;
		setting[2] = 1;
	}

	if (setting.size() != 5 + dials) {
		setting.resize(5 + dials);
		output[0].value.resize(dials);
	}

	// check for bad X limits
	if (setting[1] < setting[0]) {
		setting[0] = 0;
		setting[1] = 100;
		for (int k = 0; k < dials; k++) {
			setting[5 + k] = 0;
			if (type == L"RackZ")
				output[0].value[k] = 0;
		}
	}
	else // be sure output is in limits
	{
		for (int k = 0; k < dials; k++) {
			if (setting[5 + k] < setting[0]) {
				setting[5 + k] = setting[0];
				if (type == L"RackZ")
					output[0].value[k] = setting[0];
			}
			else if (setting[5 + k] > setting[1]) {
				setting[5 + k] = setting[1];
				if (type == L"RackZ")
					output[0].value[k] = setting[1];
			}
			else
				if (type == L"RackZ")
					output[0].value[k] = setting[5 + k];
		}
	}


}
RackOffZ_Component::RackOffZ_Component()
	: Component(L"RackOffZ", L"Rack Offset", 1, 1)
{
	dialogButtonList = rackButtonList;
	setMemory(0, 0); // as if the zero knob was recently moved
	setSetting(0, 0);      // Minimum
	setSetting(1, 100);    // Maximum
	setSetting(2, 3);      // outputs
	height = 50;
	setSetting(3, height); // height
	width = (setting[2] + 1) * TERMINAL_SIZE;
	setSetting(4, 0);      // detent
	setSetting(5, 0);      // value[0]
	setSetting(6, 0);      // value[1]
	setSetting(7, 0);      // value[2]
}


// ***************************** Synth *****************************************

int Synth_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	output[0].value.clear();
	setSimpleOutputDimensions();

	double start;
	double end;
	double step;
	int n;
	int count;
	switch ((int)setting[0]) {
	case 0:  // START END STEP -------------------------------
		start = 0;
		if (input[0].drivenByComponent != -1)
			start = input[0].value[0];
		end = 1;
		if (input[1].drivenByComponent != -1)
			end = input[1].value[0];

		step = 1;
		if (input[2].drivenByComponent != -1)
			step = input[2].value[0];
		if (step == 0)
			return false;

		n = floor((end - start) / step);
		if ((n < 1) || (n > 1000000))
			break;
		for (int j = 0; j <= n; j++)
			output[0].value.push_back(start + j * step);
		break;

	case 1: // "Start", "End", "Count");

		start = 0;
		if (input[0].drivenByComponent != -1)
			start = input[0].value[0];
		end = 1;
		if (input[1].drivenByComponent != -1)
			end = input[1].value[0];

		count = 1;
		if (input[2].drivenByComponent != -1)
			count = input[2].value[0];
		if (count <= 0)
			return false;
		if (count > 10000000)
			return false;
		
		step = (end - start) / (count - 1);

		for (int j = 0; j < count; j++)
			output[0].value.push_back(start + j * step);
		break;

	case 2: // "Start", "Step", "Count"
		start = 0;
		if (input[0].drivenByComponent != -1)
			start = input[0].value[0];
		step = 1;
		if (input[1].drivenByComponent != -1)
			step = input[1].value[0];

		count = 1;
		if (input[2].drivenByComponent != -1)
			count = input[2].value[0];
		if (count <= 0)
			return false;
		if (count > 10000000)
			return false;
	
		for (int j = 0; j < count; j++)
			output[0].value.push_back(start + j * step);

		break;
	}
	setSimpleOutputDimensions();
	return true;
}

void Synth_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	if ((radioValue >= 0) && (radioValue <= 2))
		setting[0] = radioValue; // insertion

	onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
}
void doubleClickSynth(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Start, End Step", "Start, End Count", "Start, Step, Count" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>synthButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickSynth) };

bool Synth_Component::onLoad(Sheet* pg) {
	switch ((int)setting[0]) {
	case 0: // Start End Step
		setLeftLabels(3, "Start", "End", "Step");
		break;
	case 1: // Start End Count
		setLeftLabels(3, "Start", "End", "Count");
		break;
	case 2: // Start Step Count
		setLeftLabels(3, "Start", "Step", "Count");
		break;
	}
	return true;
}


Synth_Component::Synth_Component()
	: Component(L"Synth", L"Synthesizer", 3, 1)
{
	dialogButtonList = synthButtonList;
	setLeftLabels(3, "Start", "End", "Step");
	setSetting(0, 0); // start end step
	completionTestRadios = 3;
}



// inputs: 0:Points, 1:Radius
int PolySynth_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	int points;
	if (input[0].drivenByComponent != -1)
		points = floor(input[0].value[0]);
	else
		points = 0;
	double radius;
	if (input[1].drivenByComponent != -1)
		radius = input[1].value[0];
	else
		radius = 0.0;

	setSimpleOutputDimensions();

	if (radius <= 0 || points < 2)
		return clearOutputs();

	output[0].value.resize(points + 1);   // x
	output[1].value.resize(points + 1);   // y

	double angle = 0;
	double step = TWO_PI / points;
	for (int j = 0; j < points; j++) {
		output[0].value[j] = radius * sin(angle);
		output[1].value[j] = radius * cos(angle);
		angle += step;
	}
	// close it
	output[0].value[points] = output[0].value[0];
	output[1].value[points] = output[1].value[0];

	setSimpleOutputDimensions();
	return true;
}


PolySynth_Component::PolySynth_Component()
	: Component(L"PolySynth", L"Poly Synth", squareShape, 2, 1, 2, 1)
{
	//  nC->dialogList = dialogSynthButtonList;
	setLeftLabels(3, "Points", "Radius");
	setRightLabels(3, "X", "Y");
	// nC->setSetting(0, 0); // start end step
	// nC->onLoad = onLoadSynth_;
}

// **************** Join **********************

int Join_Component::method2(Sheet* pg) {
	// pull the n left inputs into a single right output
	// empty inputs get zero output
	// array input get whole array added to the output
	int n = leftTerminalCount;
	int j;
	int k;
	int q = 0;
	bool allSameDimensions = true; // assumtion
	int inputDimension = 0;

	output[0].value.clear();
	output[0].stringValue.clear();
	
	bool outputIsString = false; // must be all values
	for (j = 0; j < n; j++) {
		if (input[j].drivenByComponent != -1)
			if (input[j].stringType) {
				outputIsString = true;
				break;
			}
	}

	for (j = 0; j < n; j++) {
		if (input[j].drivenByComponent != -1) {
			if (outputIsString) {
				if (input[j].stringType) {
					for (k = 0; k < input[j].stringValue.size(); k++)
						output[0].stringValue.push_back(input[j].stringValue[k]);
					if (j == 0)
						inputDimension = input[j].stringValue.size();
					else {
						if (inputDimension != input[j].stringValue.size())
							allSameDimensions = false;
					}
				}
				else // convert input to string
				{
					for (k = 0; k < input[j].value.size(); k++)
						output[0].stringValue.push_back(tomString(input[j].value[k]));
					if (j == 0)
						inputDimension = input[j].value.size();
					else {
						if (inputDimension != input[j].value.size())
							allSameDimensions = false;
					}
				}
			}
			else // no inputs are strings
			{
				for (k = 0; k < input[j].value.size(); k++)
					output[0].value.push_back(input[j].value[k]);
				if (j == 0)
					inputDimension = input[j].value.size();
				else {
					if (inputDimension != input[j].value.size())
						allSameDimensions = false;
				}
			}
		}
		else {
			if (outputIsString)
				output[0].stringValue.push_back("");
			else
				output[0].value.push_back(0);
			if (j == 0)
				inputDimension = 1;
			else {
				if (inputDimension != 1)
					allSameDimensions = false;
			}
		}
	}
	output[0].stringType = outputIsString;

	if (allSameDimensions) {
		output[0].dimensions.resize(2);
		output[0].dimensions[0] = inputDimension;
		output[0].dimensions[1] = n;
	}
	else
		setSimpleOutputDimensions();
	return true;
}



bool Join_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	return true;
}


bool crankJoinLimit_(Sheet* pg, Component *comp, int x, int y, bool check, int limit) {
	
	int oldTop = comp->y - comp->height;
	
	// check for hit on resize box
	if(! commonCrankVerticalResize(pg, comp, x, y, check))
			return false;

	int pins = round((2 * (comp->height - 2) - 2 * TERMINAL_SIZE) / TERMINAL_STEP + 1);
	if (pins < 2)
		pins = 2;
	if (pins > limit)
		pins = limit;

	if (comp->setting[0] != pins) {
		comp->height = pinToHeight(pins);
		y = oldTop + comp->height;
		comp->setting[0] = pins;
	}
	comp->onLoad(pg);
	comp->method2(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	
	// move the dial
	comp->dial(pg);

	return true;
}
bool Join_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, MULTI_PIN_MAX);
}

void Join_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
Join_Component::Join_Component()
	: Component(L"Join", L"Joiner", squareShape, 4, 1, 1, 1)
{
	setSetting(0, 4);
	width /= 2;
	setLeftLabels(3, "0", "1", "2", "3");	
}



// **************** Inner Joiner **********************

int InnerJoin_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();

	bool outputIsString = false; // output is numerical if all inputs are numerical values
	for (int j = 0; j < leftTerminalCount; j++) {
		if (input[j].drivenByComponent != -1)
			if (input[j].stringType) {
				outputIsString = true;
				break;
			}
	}

	if (outputIsString) {
		output[0].stringValue.resize(loops * leftTerminalCount);
		output[0].stringType = true;
	}
	else {
		output[0].value.resize(loops * leftTerminalCount);
		output[0].stringType = false;
	}

	int q = 0;
	for (int j = 0; j < loops; j++) {
		for (int k = 0; k < leftTerminalCount; k++) {
			if (outputIsString) {
				if (input[k].stringType) 
					output[0].stringValue[q++] = getLoopingStringValueD(k, j, "");
				else // convert input to string
					output[0].stringValue[q++] = tomString(getLoopingValueD(k, j, 0));
			}
			else // all inputs are values
				output[0].value[q++] = getLoopingValueD(k, j, 0);
		}
	}
	setSimpleOutputDimensions();
	return true;
}

bool InnerJoin_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	return true;
}


bool InnerJoin_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, MULTI_PIN_MAX);
}

void InnerJoin_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
InnerJoin_Component::InnerJoin_Component()
	: Component(L"InJoin", L"Inner Joiner", squareShape, 4, 1, 1, 1)
{
	setSetting(0, 4);
	width /= 2;
	setLeftLabels(3, "0", "1", "2", "3");
}


// ***************** split *************************

int Split_Component::method2(Sheet* pg)       // handle calc for a function with just one input
{
	// split the single array input into n output arrays
	int n = rightTerminalCount;

	if (input[0].drivenByComponent == -1) {
		for (int j = 0; j < n; j++) {
			output[j].stringValue.clear();
			output[j].value.resize(1);
			output[j].value[0] = 0;
			output[j].stringType = false;
		}
		return true;
	}
	
	
	int m;
	if (input[0].stringType) {

		m = input[0].stringValue.size();
		for (int j = 0; j < n; j++) {
			output[j].stringValue.resize(1);
			output[j].value.clear();
			if (j < m)
				output[j].stringValue[0] = input[0].stringValue[j];
			else
				output[j].stringValue[0] = "";
			output[j].stringType = true;
		}

	}
	else {
		m = input[0].value.size();
		for (int j = 0; j < n; j++) {
			output[j].value.resize(1);
			output[j].stringValue.clear();
			if (j < m)
				output[j].value[0] = input[0].value[j];
			else
				output[j].value[0] = 0.0;
			output[j].stringType = false;
		}

	}
	setSimpleOutputDimensions();
	return true;
}


bool Split_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);

	return true;
}

void Split_Component::dial(Sheet* pg) {
	commonDialSplit_(pg, this);
}
bool Split_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankSplitLimit_(pg, this, x, y, check, 45);
}
Split_Component::Split_Component()
	: Component(L"Split", L"Splitter", squareShape, 1, 1, 4, 1)
{
	setSetting(0, 4);
	width /= 2;
	//dialogList = dialogSplitButtonList;
}

// ******************* gather *********************

void Gather_Component::innerGather(Sheet* pg) {

	for (int p = 0; p < setting[2]; p++) {
		if (input[p].stringType) {
			output[p].stringType = true;
			for (int j = 0; j < input[p].stringValue.size(); j++)
				output[p].stringValue.push_back(input[p].stringValue[j]);
			// check the limit
			if (setting[1] > 0) {
				if (output[p].stringValue.size() > setting[1]) {
					int newLength = setting[1];
					double delta = output[p].stringValue.size() - newLength;
					// move the data forward
					for (int j = 0; j < newLength; j++)
						output[p].stringValue[j] = output[p].stringValue[j + delta];
					output[p].stringValue.resize(newLength);
				}
			}
		}
		else {
			output[p].stringType = false;
			for (int j = 0; j < input[p].value.size(); j++)
				output[p].value.push_back(input[p].value[j]);
			// check the limit
			if (setting[1] > 0) {
				if (output[p].value.size() > setting[1]) {
					int newLength = setting[1];
					double delta = output[p].value.size() - newLength;
					// move the data forward
					for (int j = 0; j < newLength; j++)
						output[p].value[j] = output[p].value[j + delta];
					output[p].value.resize(newLength);
				}
			}
		}
		output[p].dimensions.resize(1);
		output[p].dimensions[0] = output[p].value.size();
	}
}


int Gather_Component::method2(Sheet* pg) {
	if (!checkValueAndStringInputs())
		return clearOutputs();
	int params = setting[2];
	if (input[params + 1].value.size() == 0)
		return clearOutputs();

	bool reset = false;
	if ((input[0].drivenByComponent == -1) || // need data input and
		(input[params].drivenByComponent == -1))   // Trigger, Gate, or Automatic input
		reset = true;

	if (input[params + 1].drivenByComponent != -1)    // reset on command - not on clock
		if (input[params + 1].value[0])
			reset = true;

	if (reset) {
		int change = 0;
		for (int p = 0; p < params; p++) {
			if ((output[p].value.size() != 0) || (output[p].stringValue.size() != 0)) {
				output[p].value.clear();
				output[p].stringValue.clear();
				output[p].dimensions[0] = 0;
				change = 1;
			}
		}
		return change;
	}
	 
	if (input[params].stringType)
		return 0;

	switch ((int)setting[0]) {// 0=Trigger, 1=Gate, or 2=Automatic input
	case 0: // Trigger mode
		if (!memory[0]) {
			if (input[params].value[0]) {
				memory[0] = true;
				innerGather(pg); // gather all the inputs
				return 1; // always is changed
			}
		}
		else {
			if (!input[params].value[0])
				memory[0] = false;
		}
		break;
	case 1: // Gate mode
		if (input[params].value[0])
			if (pg->systemTime != memory[0]) {
				innerGather(pg); // gather all the inputs
				memory[0] = pg->systemTime;
				return 1; // always is changed
			}
		break;
	case 2: // Automatic mode
		bool gather = input[params].value[0];  // only gather when input is true;
		//for (int p = 0; p < params; p++) {
		//	if (input[p].value[0] != memory[p]) {
		//		memory[p] = input[p].value[0]; // always update on change
		//		gather = true;
		//	}
		//}
		if (gather) { // no change needed, the fact that we are called is enough
			innerGather(pg); // gather ALL the inputs
			return 1; // always is changed
		}
		break;
	}
	return 0;
}

bool Gather_Component::crank(Sheet* pg, int x, int y, bool check) {

	int oldTop = y - height;
	
	// check for hit on resize box
	if (!commonCrankVerticalResize(pg, this, x, y, check))
		return false;

	int pins = round((2 * (height - 2) - 2 * TERMINAL_SIZE) / TERMINAL_STEP + 1);
	if (pins < 3)
		pins = 3;
	if (pins > 12)
		pins = 12;

	if (setting[2] != pins - 2) {
		height = pinToHeight(pins);
		y = oldTop + height;
		setting[2] = pins - 2;
	}
	onLoad(pg);
	method2(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	//drawCanvas2();

	// move the dial
	dial(pg);

	return true;
}
void Gather_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
bool Gather_Component::onLoad(Sheet* pg) {

	int mode = setting[0];
	int params = setting[2];
	height = pinToHeight(params + 2);

	// build inputs
	buildInputPins(params + 2);
	for (int j = 0; j < params+1; j++)
		input[j].label = "Input " + tomString(j);
	input[params + 1].label = "Reset"; 

	// build outputs
	buildOutputPins(params);	
	memory.resize(params);
	for (int j = 0; j < params; j++) {
		output[j].value.clear();
		output[j].label = tomString(j);
		memory[j] = false;
	}

	switch (mode) { // overwrite next to last one
	case 0:
		input[params].label = "Trigger";
		timeBased = false;
		break;
	case 1:
		input[params].label = "Gate";
		timeBased = true;
		break;
	case 2:
		input[params].label = "Automatic";
		timeBased = false;
		break;
	}
	return true;
}



void Gather_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	if ((doubleValues[0] >= 0) && (doubleValues[0] <= 10000))
		setting[1] = (int)doubleValues[0]; // max values captured

	setting[0] = radioValue; // mode

	onLoad(pg);
	//pg->dgrm.cL[i]->setMemory(0, true);
	analyzeConnections(&pg->dgrm);
	//propagateValues(&pg->dgrm);
	drawCanvas();
}
void doubleClickGather(Sheet * pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Maximum size of output:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1] };

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Trigger Mode", "Gate Mode", "Automatic Mode" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>gatherButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickGather) };

Gather_Component::Gather_Component()
	: Component(L"Gather", L"Gather", 3, 1)
{
	dialogButtonList = gatherButtonList;
	setLeftLabels(3, "Input 0", "Trigger", "Reset");
	setRightLabels(3, "0");
	setMemory(0, false); // changes depending on mode: 0) Edge in trigger mode, 1) system time in gate mode, 2) last input in automatic mode
	setSetting(0, 0);  //  gather mode
	setSetting(1, 0);  // limit
	setSetting(2, 1);  // number of parameters gathered
	width = 40;
	completionTestDoubles = 1;
	completionTestRadios = 3;
}



int Mem_Component::method2(Sheet* pg) {
	
	// reset
	if (input[5].value[0]) {

		int size = 1;
		output[0].dimensions[0] = 0;
		for (int j = 0; j < input[0].value.size(); j++) {
			size *= floor(input[0].value[j]);
			output[0].dimensions[j] = floor(input[0].value[j]);
		}

		output[0].value.clear();
		for (int j = 0; j < size; j++)
			output[0].value[j] = 0;

		return true;
	}

	bool setNow = false;
	bool addNow = false;

	// check Set Now for positive edge
	if (!memory[0]) {
		if (input[3].value[0]) {
			memory[0] = true;
			setNow = true;
		}
	}
	else {
		if (!input[3].value[0])
			memory[0] = false;
	}

	// check Add Now for positive edge
	if (!memory[1]) {
		if (input[4].value[0]) {
			memory[1] = true;
			addNow = true;
		}
	}
	else {
		if (!input[4].value[0])
			memory[1] = false;
	}

	if (setNow || addNow) {

		if (input[2].value.size() != output[0].dimensions.size())
			return false;

		// calc the index from the address
		int index = 0;
		int size = 1;
		for (int j = 0; j < input[2].value.size(); j++) {
			int k = floor(input[2].value[j]);
			if (k < 0)
				return false;
			if (k >= output[0].dimensions[j])
				return false;

			index += k * size;
			size *= output[0].dimensions[j];
		}
		if (index >= output[0].value.size())
			return false;

		if (setNow)
			output[0].value[index] = input[1].value[0];
		else
			output[0].value[index] += input[1].value[0];
		return true;
	}
	return false;
}

Mem_Component::Mem_Component()
	: Component(L"Mem", L"Memory", squareShape, 6, 1, 1, 1)
{
	//dialogList = dialogMemoryButtonList;
	setLeftLabels(3, "Dimensions", "Data", "Index", "Set", "Accumulate", "Init/Clear");
	setMemory(0, false); // set now pin memory
	setMemory(1, false); // add now pin memory
	width = 40;
}




// List Memory ---------------
//bool ListMem_Component::method2(Sheet* pg) {
	/*
		int channels = setting[0];
		int entries = setting[1];
		int mode = setting[2];

		bool appendIt = input[channels + 0].value[0]; // adds a new set of entries to the end of the list
		// ******************** bool indexBool = input[channels + 1].value;    // the index/bool of the entities to track or delete
		bool trackIt = input[channels + 2].value[0]; // tracks an entry <index> in the list
		bool deleteIt = input[channels + 3].value[0]; // deletes an entry <index> from the list
		bool clearIt = input[channels + 4].value[0]; // clears all entries from the channels

		bool outputChanged = false;

		if (!memory[0] && appendIt) { // append one entry
			int z = channels * entries;
			for (int c = 0; c < channels; c++) {
				element[z] = input[c].value[0];
				output[c].value[entries] = element[z];
				output[c].dimensions.resize(2);
				output[c].dimensions[0] = entries;
				output[c].dimensions[1] = 1;
				z++;
			}
			setting[1]++;
			outputChanged = true;
		}
		memory[0] = appendIt;

		if (!memory[1] && deleteIt) { // delete entries based on boolean/index array
			// copy all but the deleted entries
			int newE = 0;
			int z = 0;
			for (int e = 0; e < entries; e++) {
				if (!indexBool[e]) {
					for (c = 0; c < channels; c++) {
						output[c].value[newE] = output[c].value[e];
						element[z++] = output[c].value[e];
					}
					newE++;
				}
			}
			entries = newE;
			for (int c = 0; c < channels; c++) {
				output[c].value.resize( entries);
				element.resize(z);
				output[c].dimensions.resize();
				output[c].dimensions[0] = entries;
			}
			setting[1] = entries;
			outputChanged = true;
		}
		memory[1] = deleteIt;

		// clear all entries
		if (!memory[2] && clearIt) {
			for (int c = 0; c < channels; c++) {
				element.clear();
				output[c].value.clear();
				output[c].dimensions.resize(1);
				output[c].dimensions[0] = 0;
			}
			setting[1] = 0;
			outputChanged = true;
		}
		memory[2] = clearIt;

		// continously track the entries that match the indexBool input array
		if (trackIt) {
			int max = min(indexBool.size(), entries);
			for (int e = 0; e < max; e++) {
				if (indexBool[e]) {
					for (int c = 0; c < channels; c++) {
						// outputValue[c][e] = inputValue[c][0];
						output[c].value[e] = getLoopingValueD(pg, i, c, e, 0);
						int z = e * channels + c;
						element[z] = output[c].value[0];
					}
				}
			}
			outputChanged = true;
		}

		return outputChanged;
	 return false;
}*/

//bool ListMem_Component::onLoad(Sheet* pg) {
/*	var channels = setting[0];
	var entries = setting[1];
	var mode = setting[2];

	height = pinToHeight(channels + 5);

	// left terminals are inputs
	buildInputPins(pg->dgrm.cL[i], channels + 5);
	leftLabels = [];
	for (var j = 0; j < channels; j++)
		leftLabels[j] = "Ch " + j;
	leftLabels[j++] = "Append";
	if (mode)
		leftLabels[j++] = "Index";
	else
		leftLabels[j++] = "Boolean";
	leftLabels[j++] = "Track";
	leftLabels[j++] = "Delete";
	leftLabels[j++] = "Clear";

	// right terminals are outputs
	buildOutputPins(pg->dgrm.cL[i], channels);
	rightLabels = [];
	for (j = 0; j < channels; j++)
		rightLabels[j] = "Ch " + j;

	// now output the data
	var z = 0
		for (var e = 0; e < entries; e++) {
			for (var c = 0; c < channels; c++) {
				outputValue[c][e] = element[z++];
			}
		}
	for (c = 0; c < channels; c++) {
		outputValue[c].size() = entries;
		outputValueDimensions[c][0] = entries;
	}
*/
//	return true;
//}

//ListMem_Component::ListMem_Component()
//	: Component(L"ListMem", L"List Memory", squareShape, 6, 1, 1, 1)
//{
//	//dialogList = dialogListMemButtonList;
//	setMemory(0, 0); // append edge memory
//	setMemory(1, 0); // delete edge memory
//	setMemory(2, 0); // clear edge memory
//	width = 35;
//	setSetting(0, 1);  // number of list channels
//	setSetting(1, 0);  // number of entries in each list
//	setSetting(2, 0);  // choose: 0=boolean array vs. 1=index array
//	setElement(0, 0);  // value of x[0]
//	setOutput(0, 0, 0.0);
//	setLeftLabels(3, "Ch 0", "Append", "Boolean", "Track", "Delete", "Clear");
//	setRightLabels(3, "Ch 0");
//}


// ***************************** Splice In ************************************

int SpliceIn_Component::method2(Sheet* pg) {

	output[0].value.clear();
	output[0].stringValue.clear();
	if (input[1].value.size() == 0)
		return 1;
	bool isString = input[0].stringType;
	if (isString != input[2].stringType)
		return 1;
	output[0].stringType = isString;

	int inArraySize;	// array
	int valueSize;		// values'
	if (isString) {
		inArraySize = input[0].stringValue.size();	// array
		valueSize = input[2].stringValue.size();		// values
	}
	else {
		inArraySize = input[0].value.size();	// array
		valueSize = input[2].value.size();		// values
	}

	int position = floor(input[1].value[0]);	// position
	if( position < 0)
		position = 0;
	if( position > inArraySize )
		position = inArraySize;

	if (isString) {
		for (int j = 0; j < position; j++)
			output[0].stringValue.push_back(input[0].stringValue[j]);

		for (int j = 0; j < valueSize; j++)
			output[0].stringValue.push_back(input[2].stringValue[j]);

		for (int j = position; j < inArraySize; j++)
			output[0].stringValue.push_back(input[0].stringValue[j]);
	}else {
		for (int j = 0; j < position; j++)
			output[0].value.push_back(input[0].value[j]);

		for (int j = 0; j < valueSize; j++)
			output[0].value.push_back(input[2].value[j]);

		for (int j = position; j < inArraySize; j++)
			output[0].value.push_back(input[0].value[j]);
	}

	output[0].dimensions[0] = inArraySize + valueSize;

	return true;
}

SpliceIn_Component::SpliceIn_Component()
	: Component(L"SpliceIn", L"Splice In", 3, 1)
{
	setLeftLabels(3, "Array", "Position", "Values");
}


// ******************************** Splice Out **************************************

int SpliceOut_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();
	
	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int inArraySize;
	if (isString)
		inArraySize = input[0].stringValue.size();	// array
	else
		inArraySize = input[0].value.size();
	
	int position = floor(getDoubleInputOrZero(1, 0));
	int count = floor(getDoubleInputOrZero(2, 0));
	if (count <= 0)
		return clearOutputs();

	if ((count == 0) || (position < 0) || (position > inArraySize)) {
		for (int j = 0; j < inArraySize; j++)
			if (isString)
				output[0].stringValue.push_back( input[0].stringValue[j]);
			else
				output[0].value.push_back(input[0].value[j]);

		output[0].dimensions[0] = inArraySize;
		return 1;
	}

	if (isString) {
		for (int j = 0; j < position; j++)
			output[0].stringValue.push_back(input[0].stringValue[j]);
		for (int j = position + count; j < inArraySize; j++)
			output[0].stringValue.push_back(input[0].stringValue[j]);
	}else{
		for (int j = 0; j < position; j++)
			output[0].value.push_back(input[0].value[j]);
		for (int j = position + count; j < inArraySize; j++)
			output[0].value.push_back(input[0].value[j]);
	}
	output[0].dimensions[0] = inArraySize - count;
	return 1;
}

SpliceOut_Component::SpliceOut_Component()
	: Component(L"SpliceOut", L"Splice Out", 3, 1)
{
	setLeftLabels(3, "Array", "Position", "Count");
}

// ********************************** Keep *************************************

int Keep_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();

	if (input[1].drivenByComponent == -1)    // boolean array
		return clearOutputs();

	bool stringType = input[0].stringType;
	int size;
	output[0].stringType = stringType;
	if (stringType)
		size = input[0].stringValue.size();
	else
		size = input[0].value.size();

	// array must be at least as big as boolean
	if (size != input[1].value.size())
		return clearOutputs();
	
	output[0].value.clear();
	output[0].stringValue.clear();

	for (int jin = 0; jin < size; jin++) {
		if (input[1].value[jin]) {
			if( stringType )
				output[0].stringValue.push_back(input[0].stringValue[jin]);
			else
				output[0].value.push_back(input[0].value[jin]);
		}
	}
	setSimpleOutputDimensions();
	return true;
}

Keep_Component::Keep_Component()
	: Component(L"Keep", L"Keep", 2, 1)
{
	setLeftLabels(3, "Array", "Boolean");
}


// ********************* Keep First *************************************

vector<double> innerKeepFirstDouble(vector<double> tempA, double key) {
	vector<double> tempB;
	bool found = false;
	int jout = 0;
	for (int j = 0; j < tempA.size(); j++) {
		if (!found) {
			tempB.push_back(tempA[j]);  // copy input to reduced output
			if (tempA[j] == key) // check for key
				found = true;
		}
		else {
			if (tempA[j] != key) {
				tempB.push_back(tempA[j]);  // copy input to reduced output
			}
		}
	}
	return tempB;
}
vector<string> innerKeepFirstString(vector<string> tempA, string key) {
	vector<string> tempB;
	bool found = false;
	int jout = 0;
	for (int j = 0; j < tempA.size(); j++) {
		if (!found) {
			tempB.push_back(tempA[j]);  // copy input to reduced output
			if (tempA[j] == key) // check for key
				found = true;
		}
		else {
			if (tempA[j] != key) {
				tempB.push_back(tempA[j]);  // copy input to reduced output
			}
		}
	}
	return tempB;
}

int KeepFirst_Component::method2(Sheet* pg) {
	output[0].value.clear();
	output[0].stringValue.clear();
	output[0].dimensions[0] = 0;

	output[1].value.clear();
	output[1].stringValue.clear();
	output[1].dimensions[0] = 0;

	if (input[0].drivenByComponent == -1)    // data array
		return true;
	if (input[1].drivenByComponent == -1)    // key
		return true;

	bool stringType = input[0].stringType;
	output[0].stringType = stringType;
	output[1].stringType = stringType;

	int arraySize;
	int keySize;
	if (stringType) {
		arraySize = input[0].stringValue.size();
		keySize = input[1].stringValue.size();
	}
	else
	{
		arraySize = input[0].value.size();
		keySize = input[1].value.size();
	}

	// for the padded version - copy everything to the output
	for (int jin = 0; jin < arraySize; jin++)
		if (stringType)
			output[0].stringValue.push_back(input[0].stringValue[jin]);  // copy input to full output
		else
			output[0].value.push_back(input[0].value[jin]);  // copy input to full output

	// then scan and overwrite extra occurances
	for (int k = 0; k < keySize; k++) {
		bool found = false;
		for (int jin = 0; jin < arraySize; jin++) {
			if (stringType) {
				if (input[0].stringValue[jin] == input[1].stringValue[k]) {
					if (!found)
						found = true; // dont overwrite but set flag
					else
						output[0].stringValue[jin] = "";  // overwrite with nothing
				}
			}
			else {
				if (input[0].value[jin] == input[1].value[k]) {
					if (!found) 
						found = true; // dont overwrite but set flag
					else
						output[0].value[jin] = 0;  // overwrite with zero
				}
			}
		}
	}

	// for the reduced version - use temp arrays
	if (stringType) {
		vector<string> tempA;
		vector<string> tempB;
		for (int j = 0; j < arraySize; j++)
			tempA.push_back(input[0].stringValue[j]);

		for (int k = 0; k < keySize; k++) {
			tempB = innerKeepFirstString(tempA, input[1].stringValue[k]);

			if (k + 1 < keySize) { // setup for next iteration
				tempA.clear();
				for (int j = 0; j < tempB.size(); j++)
					tempA.push_back(tempB[j]);
			}
		}
		for (int j = 0; j < tempB.size(); j++)
			output[1].stringValue.push_back(tempB[j]);
	}
	else
	{
		vector<double> tempA;
		vector<double> tempB;
		for (int j = 0; j < arraySize; j++)
			tempA.push_back(input[0].value[j]);

		for (int k = 0; k < keySize; k++) {
			tempB = innerKeepFirstDouble(tempA, input[1].value[k]);

			if (k + 1 < keySize) { // setup for next iteration
				tempA.clear();
				for (int j = 0; j < tempB.size(); j++)
					tempA.push_back(tempB[j]);
			}
		}
		for (int j = 0; j < tempB.size(); j++)
			output[1].value.push_back(tempB[j]);
	}
	setSimpleOutputDimensions();
	return true;
}



KeepFirst_Component::KeepFirst_Component()
	: Component(L"KeepFirst", L"Keep First", squareShape, 2, 1, 2, 1)
{
	setLeftLabels(3, "Array", "Key");
	setRightLabels(3, "Padded", "Reduced");
	width = 40;
}

// ************************** Keep Last *********************************


int KeepLast_Component::method2(Sheet* pg) {
	output[0].value.clear();
	output[1].value.clear();
	output[0].dimensions[0] = 0;
	output[1].dimensions[0] = 0;

	if (input[0].drivenByComponent == -1)    // data array
		return true;
	if (input[1].drivenByComponent == -1)    // key
		return true;

	bool stringType = input[0].stringType;
	output[0].stringType = stringType;
	output[1].stringType = stringType;

	int arraySize;
	int keySize;
	if (stringType) {
		arraySize = input[0].stringValue.size();
		keySize = input[1].stringValue.size();
	}
	else
	{
		arraySize = input[0].value.size();
		keySize = input[1].value.size();
	}

	// for the padded version - copy everything to the output
	for (int jin = 0; jin < arraySize; jin++)
		if (stringType)
			output[0].stringValue.push_back(input[0].stringValue[jin]);  // copy input to full output
		else
			output[0].value.push_back(input[0].value[jin]);  // copy input to full output

	// then scan and overwrite extra occurances
	for (int k = 0; k < keySize; k++) {
		bool found = false;
		for (int jin = arraySize - 1; jin >= 0; jin--) { // BACKWARDS SCANNING
			if (stringType) {
				if (input[0].stringValue[jin] == input[1].stringValue[k]) {
					if (!found) 
						found = true; // dont overwrite but set flag
					else
						output[0].stringValue[jin] = "";  // overwrite with nothing
				}
			}
			else
			{
				if (input[0].value[jin] == input[1].value[k]) {
					if (!found) 
						found = true; // dont overwrite but set flag
					else 
						output[0].value[jin] = 0;  // overwrite with zero
				}
			}
		}
	}

	// for the reduced version - use temp arrays
	if (stringType) {
		vector<string> tempA;
		vector<string> tempB;
		for (int j = 0; j < arraySize; j++)
			tempA.push_back(input[0].stringValue[arraySize - j - 1]); // reverse it at beginning

		for (int k = 0; k < keySize; k++) {
			tempB = innerKeepFirstString(tempA, input[1].stringValue[k]);

			if (k + 1 < keySize) { // setup for next iteration
				tempA.clear();
				for (int j = 0; j < tempB.size(); j++)
					tempA.push_back(tempB[j]);
			}
		}
		for (int j = 0; j < tempB.size(); j++)
			output[1].stringValue.push_back(tempB[tempB.size() - j - 1]); // straighten it at the end
	}
	else
	{
		vector<double> tempA;
		vector<double> tempB;
		for (int j = 0; j < arraySize; j++)
			tempA.push_back(input[0].value[arraySize - j - 1]); // reverse it at beginning

		for (int k = 0; k < keySize; k++) {
			tempB = innerKeepFirstDouble(tempA, input[1].value[k]);

			if (k + 1 < keySize) { // setup for next iteration
				tempA.clear();
				for (int j = 0; j < tempB.size(); j++)
					tempA.push_back(tempB[j]);
			}
		}
		for (int j = 0; j < tempB.size(); j++)
			output[1].value.push_back(tempB[tempB.size() - j - 1]); // straighten it at the end
	}
	setSimpleOutputDimensions();
	return true;
}


KeepLast_Component::KeepLast_Component()
	: Component(L"KeepLast", L"Keep Last", squareShape, 2, 1, 2, 1)
{
	setLeftLabels(3, "Array", "Key");
	setRightLabels(3, "Padded", "Reduced");
	width = 40;
}


// *********************** Get At ********************************

	// find elements at the Index and output them
int GetAt_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();
	if (input[1].drivenByComponent == -1)    //  Index
		return clearOutputs();

	int arraySize;
	bool arrayIsString = input[0].stringType;
	if( arrayIsString) 
		arraySize = input[0].stringValue.size();
	else
		arraySize = input[0].value.size();
	output[0].stringType = arrayIsString;

	int indexSize = input[1].value.size();

	output[0].value.clear();
	output[0].stringValue.clear();

	int index = 0;
	for (int j = 0; j < indexSize; j++) {
		if (input[1].value[j] >= 0) {
			index = floor(input[1].value[j]); // from the front
		}
		else {
			index = floor(arraySize + input[1].value[j]); //from the back
		}
		if (index < 0)
			continue;
		else if (index >= arraySize)
			continue;
		else {
			if (arrayIsString)
				output[0].stringValue.push_back(input[0].stringValue[index]);
			else
				output[0].value.push_back(input[0].value[index]);
		}
	}
	copyDimensions(1, 0);
	return true;
}

GetAt_Component::GetAt_Component()
	: Component(L"GetAt", L"Get At", 2, 1)
{
	setLeftLabels(3, "Array", "Index");
}

// *********************** Interpolate ********************************

int Interpolate_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();
	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();
	if (input[1].drivenByComponent == -1)    //  Index
		return clearOutputs();

	int arraySize = input[0].value.size();
	int indexSize = input[1].value.size();
	output[0].value.clear();

	int index = 0;
	double fraction;
	
	for (int j = 0; j < indexSize; j++) {
		if (input[1].value[j] >= 0) {
			index = floor(input[1].value[j]); // from the front
			fraction = input[1].value[j] - index;
		}
		else {
			index = floor(arraySize + input[1].value[j]); //from the back
			fraction = arraySize + input[1].value[j] - index;
		}
		if (index < 0)
			output[0].value.push_back(input[0].value[0]);
		else if (index >= arraySize-1)
			output[0].value.push_back(input[0].value[arraySize-1]);
		else {
			double oneMinusFraction = 1.0 - fraction;
			double result = oneMinusFraction * input[0].value[index] +
				fraction * input[0].value[index+1];
			output[0].value.push_back(result);
		}
	}
	copyDimensions(1, 0);
	return true;
}

Interpolate_Component::Interpolate_Component()
	: Component(L"Interp", L"Interpolate", 2, 1)
{
	setLeftLabels(3, "Array", "Index");
}


// ************************* string join *****************************


int SJoin_Component::method2(Sheet* pg) {
	// pull the n left inputs into a single right output
	// empty inputs get no output
	// assemble multiples to multiple outputs
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].stringValue.resize(loops);
	output[0].stringType = true;

	int n = leftTerminalCount;

	for (int k = 0; k < loops; k++) {
		output[0].stringValue[k] = "";
		for (int j = 0; j < n; j++) {
			if (input[j].drivenByComponent != -1) {
				if (input[j].stringType) {
					if (j > 0) {
						switch ((int)setting[1]) {
						case 1:
							output[0].stringValue[k] += " ";
							break;
						case 2:
							output[0].stringValue[k] += ",";
							break;
						}
					}
				}
				if (input[j].stringType) {
					if (input[j].stringValue.size() == 1)
						output[0].stringValue[k] += input[j].stringValue[0];
					else
						output[0].stringValue[k] += input[j].stringValue[k];
				}
				else { // numeric - need to convert
					if (input[j].value.size() != 0) {
						if (input[j].value.size() == 1)
							output[0].stringValue[k] += tomString(input[j].value[0]);
						else
							output[0].stringValue[k] += tomString(input[j].value[k]);
					}
				}
			}
		}
	}
	setSimpleOutputDimensions();
	return true;
}

void SJoin_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}

void SJoin_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if ((doubleValues[0] > 0) && (doubleValues[0] <= 32))
		setting[0] = (int)doubleValues[0]; // inputs
	
	if ((radioValue >= 0) && (radioValue <= 2))
		setting[1] = radioValue; // insertion

	onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
}
void doubleClickSJoin(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Inputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "No Spacing", "Spaces Between Inputs", "Commas Between Inputs" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>sJoinButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickSJoin)};

bool SJoin_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, 32);
}
bool SJoin_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	
	return true;
}
SJoin_Component::SJoin_Component()
	: Component(L"SJoin", L"String Joiner", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = sJoinButtonList;
	setSetting(0, 4);
	setSetting(1, 0); // 0-no spaces, 1-spaces, 2-commas
	width /= 2;
	completionTestDoubles = 1;
	completionTestRadios = 3;
}

// ***************************** matrix Joiner *****************************************



int MatrixJoin_Component::method2(Sheet* pg) {
	// pull the n left matricies into a single right matrix
	// no empty inputs allowed
	// array input get whole array added to the output
	// scalars allowed now
	
	if (!checkValueAndStringInputs())
		return clearOutputs();

	int n = leftTerminalCount;
	int j;
	int k;
	int len[32];
	int dims = 0;
	int xSize = 0;
	int ySize = 0;
	
	// all strings or all values
	bool isString = input[0].stringType;
	for (j = 1; j < n; j++) {
		if (isString != input[j].stringType)
			return clearOutputs();
	}
	
	// now size up the situation
	for (j = 0; j < n; j++) {
		// need all inputs
		if (input[j].drivenByComponent == -1)
			return clearOutputs();
		// cant handle 3d arrays - yet
		if (input[j].dimensions.size() > 3)
			return clearOutputs();

		len[j] = getInputSize(j);

		// all inputs must be the same dimensions or scalar
		if (len[j] > 1) {
			if (dims == 0)
				dims = input[j].dimensions.size();
			else if (dims != input[j].dimensions.size())
				return clearOutputs();
		}
	}
	
	output[0].stringType = isString;
	output[0].value.clear();
	output[0].stringValue.clear();
	
	// lots of possibilites
	switch (dims) {
		
	case 0: // ------------------------ all inputs are scalar ----------------------------------
		for (j = 0; j < n; j++) 
			pushInToOut(0, j, 0);
		
		switch ((int)setting[1]) { // direction of joining
		case 0: // join in the x direction - make columns
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = n;
			output[0].dimensions[1] = 1;
			break;
		case 1:  // join in the y direction - make rows
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = 1;
			output[0].dimensions[1] = n;
			break;
		case 2: // join in the z direction - make layers
			output[0].dimensions.resize(3);
			output[0].dimensions[0] = 1;
			output[0].dimensions[1] = 1;
			output[0].dimensions[2] = n;
			break;
		}
		break;
		
	case 1:  // ------------------------- inputs are 1 dimensional vectors ------------------------------
		// Step 1 - check sizes
		// all inputs must be the same width
		xSize = 0;
		for (j = 0; j < n; j++) {
			if (len[j] != 1) {
				if (xSize == 0) 
					xSize = getInputSize(j);
				else 
					if (xSize != getInputSize(j))
						return true;
			}
		}
		
		// Step 2 move the data
		switch ((int)setting[1]) { // direction of joining
		case 0: // join in the x direction
			for (k = 0; k < xSize; k++) {
				for (j = 0; j < n; j++) {
					if (len[j] == 1) 
						pushInToOut(0, j, 0);
					else 
						pushInToOut(0, j, k);
				}
			}
			break;
		case 1:  // join in the y direction
		case 2: // join in the z direction - if possible
			for (j = 0; j < n; j++) {
				if (len[j] == 1) 
					pushInToOut(0, j, 0);
				else 
					for (k = 0; k < len[j]; k++)
						pushInToOut(0, j, k);
			}
			break;
		}
		
		// Step 3 set the dimensions
		switch ((int)setting[1]) { // direction of joining
		case 0: // join in the x direction
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = n;
			output[0].dimensions[1] = xSize;
			break;
		case 1:  // join in the y direction
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = xSize;
			output[0].dimensions[1] = n;
			break;
		case 2: // join in the z direction - if possible
			output[0].dimensions.resize(3);
			output[0].dimensions[0] = 1;
			output[0].dimensions[1] = xSize;
			output[0].dimensions[2] = n;
			break;
		}
		break;
		
	case 2:  // ----------------------- inputs are 2 Dimensional ------------------------------
		// Step 1 - check sizes
		switch ((int)setting[1]) { // direction of joining
		case 0: // join in the x direction
			// y dimension must match
			ySize = 0;
			for (j = 0; j < n; j++) {
				if (len[j] != 1) {
					if (ySize == 0)
						ySize = input[j].dimensions[1];
					else if (ySize != input[j].dimensions[1])
						return true;
				}
			}
			break;
		case 1:  // join in the y direction
			// x directions must match
			xSize = 0;
			for (j = 0; j < n; j++) {
				if (len[j] != 1) {
					if (xSize == 0)
						xSize = input[j].dimensions[0];
					else if (xSize != input[j].dimensions[0])
						return true;
				}
			}
			break;
		case 2: // join in the z direction - if possible
			// both x and y dimensions must match
			xSize = 0;
			ySize = 0;
			for (j = 1; j < n; j++) {
				if (len[j] != 1) {
					if (xSize == 0)
						xSize = input[j].dimensions[0];
					else if (xSize != input[j].dimensions[0])
						return true;
					if (ySize == 0)
						ySize = input[j].dimensions[1];
					else if (ySize != input[j].dimensions[1])
						return true;
				}
			}
			break;
		}
		// Step 2 move the data
		switch ((int)setting[1]) { // direction of joining
		case 0: // join in the x direction
			for (int y = 0; y < ySize; y++) {
				for (k = 0; k < n; k++) {
					if (len[k] == 1) 
						pushInToOut(0, k, 0);
					else 
						for (j = 0; j < input[k].dimensions[0]; j++)
							pushInToOut(0, k, y* input[k].dimensions[0] + j);
				}
			}
			break;
		case 1:  // join in the y direction
			for (j = 0; j < n; j++) {
				if (len[j] == 1) {
					for (k = 0; k < xSize; k++)
						pushInToOut(0, j, 0);
				}
				else {
					for (k = 0; k < len[j]; k++)
						pushInToOut(0, j, k);
				}
			}
			break;
		case 2: // join in the z direction
			for (j = 0; j < n; j++) {
				if (len[j] == 1) {
					for (k = 0; k < xSize * xSize; k++)
						pushInToOut(0, k, 0);
				}
				else {
					for (k = 0; k < len[j]; k++)
						pushInToOut(0, j, k);
				}
			}
			break;
		}
		// Step 3 set the dimensions
		switch ((int)setting[1]) { // direction of joining
		case 0: // join in the x direction
			xSize = 0;
			for (j = 0; j < n; j++)
				xSize += input[j].dimensions[0];
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = xSize;
			output[0].dimensions[1] = ySize;
			break;
		case 1:  // join in the y direction
			ySize = 0;
			for (j = 0; j < n; j++) {
				if (len[j] == 1)
					ySize++;
				else
					ySize += input[j].dimensions[1];
			}
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = xSize;
			output[0].dimensions[1] = ySize;
			break;
		case 2: // join in the z direction
			output[0].dimensions.resize(3);
			output[0].dimensions[0] = xSize;
			output[0].dimensions[1] = ySize;
			output[0].dimensions[2] = n;
			break;
		}
		break;
		
	}
	
	return true;
}



bool MatrixJoin_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	input.clear();
	buildInputPins(n);

	switch ((int)setting[1]) {
	case 0:
		for (int j = 0; j < n; j++)
			input[j].label = "Column " + tomString(j);
		break;
	case 1:
		for (int j = 0; j < n; j++)
			input[j].label = "Row " + tomString(j);
		break;
	case 2:
		for (int j = 0; j < n; j++)
			input[j].label = "Layer " + tomString(j);
		break;
	}

	return true;
}

void MatrixJoin_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	
	if ((radioValue >= 0) && (radioValue <= 2))
		setting[1] = radioValue; // axis

	onLoad(pg);
}
void doubleClickMatJoinSplit(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> radioLabels = { "Columns", "Rows", "Layers" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		{}, {}, radioLabels, radioIndex, NULL);
}


std::vector<Button*>matJoinButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickMatJoinSplit) };

bool MatrixJoin_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, 32);
}
void MatrixJoin_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
MatrixJoin_Component::MatrixJoin_Component()
	: Component(L"MJoin", L"Matrix Joiner", 2, 1)
{
	dialogButtonList = matJoinButtonList;
	width = 50;
	setRightLabels(3, "Matrix");
	setSetting(0, 2); // inputs
	setSetting(1, 0); // mode: 0) Join on X columns, 1) Join on Y rows,  2) Join on Z layers someday >
	//dialogList = dialogMatJoinButtonList;
}

// ************************************ Matrix Splitter ***********************************

int MatrixSplit_Component::method2(Sheet* pg) {
	// split the Matrix left input into n right outputs
	// split columns, rows or layers

	int outputs = setting[0];
	for (int j = 0; j < outputs; j++) {
		output[j].value.clear();
		output[j].dimensions.clear();
	}

	if (input[0].drivenByComponent == -1)
	//	(input[1].drivenByComponent == -1))  //WHY IS THIS HERE
		return clearOutputs();

	int loops = 0;
	int width = 0;
	int height = 0;
	int layers = 0;
	int inj = 0;
	switch ((int)setting[1]) {

	case 0: // columns
		if (input[0].dimensions.size() != 2)
			return true;

		width = input[0].dimensions[0];
		height = input[0].dimensions[1];
		inj = 0;
		for (int k = 0; k < height; k++) {
			for (int j = 0; j < width; j++) {
				if (j < outputs)
					output[j].value.push_back( input[0].value[inj]);
				inj++;
			}
		}
		for (int j = 0; j < outputs; j++) {
			output[j].dimensions.resize(1);
			output[j].dimensions[0] = height;
		}
		if (outputs > width) {
			for (int j = width; j < outputs; j++) {
				output[j].value.resize(1);
				output[j].value[0] = 0;
				output[j].dimensions.resize(1);
				output[j].dimensions[0] = 0;
			}
		}
		break;

	case 1: // rows
		if (input[0].dimensions.size() != 2)
			return true;

		width = input[0].dimensions[0];
		height = input[0].dimensions[1];
		loops = min(height, outputs);
		inj = 0;
		for (int k = 0; k < loops; k++) {
			for (int j = 0; j < width; j++)
				output[k].value.push_back( input[0].value[inj++]);
			output[k].dimensions.resize(1);
			output[k].dimensions[0] = width;
		}
		break;
	case 2: // layers output each layer on a separate pin
		if (input[0].dimensions.size() != 3)
			return true;

		width = input[0].dimensions[0];
		height = input[0].dimensions[1];
		layers = input[0].dimensions[2];
		loops = min(layers, outputs);
		inj = 0;
		for (int k = 0; k < loops; k++) {
			int outj = 0;
			for (int j = 0; j < height; j++) {
				for (int w = 0; w < width; w++)
					output[k].value.push_back(input[0].value[inj++]);
			}
			output[0].dimensions.resize(2);
			output[0].dimensions[0] = width;
			output[0].dimensions[1] = height;
		}
		break;
	}
	return true;
}



bool MatrixSplit_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);

	switch ((int)setting[1]) {
	case 0:
		for (int j = 0; j < n; j++)
			output[j].label = "Column " + tomString(j);
		break;
	case 1:
		for (int j = 0; j < n; j++)
			output[j].label = "Row " + tomString(j);
		break;
	case 2:
		for (int j = 0; j < n; j++)
			output[j].label = "Layer " + tomString(j);
		break;
	}
	
	return true;
}


std::vector<Button*>matSplitButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClickMatJoinSplit) };

bool MatrixSplit_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankSplitLimit_(pg, this, x, y, check, 32);
}
void MatrixSplit_Component::dial(Sheet* pg) {
	commonDialSplit_(pg, this);
}
MatrixSplit_Component::MatrixSplit_Component()
	: Component(L"MSplit", L"Matrix Splitter", squareShape, 1, 1, 4, 1)
{
	dialogButtonList = matSplitButtonList;
	setSetting(0, 4); // outputs
	setSetting(1, 0); // mode: 0) Split on X columns, 1) split on Y rows,  2) split on Z layers someday >
	setLeftLabels(3, "Matrix");
	width = 50;
	
}

// ************************* Match *********************************

bool centerCrankVerticalResize(Sheet* pg, Component *comp, int ax, int ay, bool check) {
	int left = comp->x - RESIZE_HANDLE_SIZE;
	int right = comp->x + RESIZE_HANDLE_SIZE;
	int top = comp->y + comp->height - RESIZE_HANDLE_SIZE;

	if (check) {
		if (ax > right)
			return false; // missed
		if (ax < left)
			return false; // missed
		if (ay < top)
			return false; // missed
	}

	top = comp->y - comp->height;

	comp->height = round((ay - top + RESIZE_HANDLE_SIZE) / 2);

	if (comp->height < 15)
		comp->height = 15;

	return true;
}





//void setOutputMatrixSizes_(Sheet* pg, int *dims, int dimSize) {
//	int size = 1;
//	for (int k = 0; k < dimSize; k++)
//		size *= dims[k];
//	for (int j = 0; j < rightTerminalCount; j++) { // for each output pin
//		output[j].value.resize(size);
//
//		output[j].dimensions.clear();
//		for (int k = 0; k < dimSize; k++)
//			output[j].dimensions.push_back( dims[k]);
//	}
//}

//bool inputsToDimensions(Sheet* pg, int *dims, int *dimsSize) {
//	int loops = 0;
//	int pinOfDimensions = -1;
//	//int dims[32];
//	for (int j = 0; j < leftTerminalCount; j++) {
//		if (input[j].value.size() != 1) {
//			if (loops == 0) {
//				loops = input[j].value.size();
//				pinOfDimensions = j;
//			}
//			else if (input[j].value.size() != loops) {
//				dims[0] = 0;
//				*dimsSize = 0;
//				return false; // no match
//			}
//		}
//	}
//
//	if (loops == 0)
//		loops = 1;
//	if (pinOfDimensions == -1) {
//		dims[0] = loops;
//		*dimsSize = 1;
//	}
//	else {
//		int limit = input[pinOfDimensions].dimensions.size();
//		for (int j = 0; j < limit; j++)
//			dims[j] = input[pinOfDimensions].dimensions[j];
//		*dimsSize = limit;
//	}
//	
//	return true;
//}



// *************************** Expander (and Matcher) ************************************

int Expand_Component::method2(Sheet* pg) {
	// Expand the n left inputs into the n right outputs
	// permutes all combinations

	int n = leftTerminalCount;
	int k, q, p;

	// clear the outputs
	for (k = 0; k < n; k++) {
		output[k].value.clear();
		output[k].stringValue.clear();
	}
	// check for bad inputs
	for (k = 0; k < n; k++)
		if (input[k].drivenByComponent == -1) {
			setSimpleOutputDimensions();
			return 1;
		}
	bool isString = input[0].stringType;
	for (k = 1; k < n; k++)
		if (isString != input[k].stringType) {
			setSimpleOutputDimensions();
			return 1;
		}

	// initialize the counters
	int j[32] = { 0 };

	// calculate the total number of outputs needed
	q = 1;
	if (isString) {
		for (k = 0; k < n; k++)
			q *= input[k].stringValue.size();
		// here we go strings
		for (p = 0; p < q; p++) // for each required permutation
		{
			for (k = 0; k < n; k++)  // for each output pin
				output[k].stringValue.push_back(input[k].stringValue[j[k]]);

			j[0] += 1;
			for (k = 0; k < n; k++) {
				if (j[k] < input[k].stringValue.size())
					break;
				j[k] = 0;
				j[k + 1] += 1;
			}
		}
	}
	else {
		for (k = 0; k < n; k++)
			q *= input[k].value.size();
		// here we go values
		for (p = 0; p < q; p++) // for each required permutation
		{
			for (k = 0; k < n; k++)  // for each output pin
				output[k].value.push_back(input[k].value[j[k]]);

			j[0] += 1;
			for (k = 0; k < n; k++) {
				if (j[k] < input[k].value.size())
					break;
				j[k] = 0;
				j[k + 1] += 1;
			}
		}
	}

	// fancy dimensions to reflect the inputs
	for (k = 0; k < n; k++) { // for each input pin
		output[k].stringType = isString;
		output[k].dimensions.clear();
		for (p = 0; p < n; p++) { // for each output pin
			if (isString)
				output[k].dimensions.push_back(input[p].stringValue.size());
			else
				output[k].dimensions.push_back(input[p].value.size());
		}
	}
	return 1;
}



void Expand_Component::dial(Sheet* pg) {
	// draw the size knob on the bottom CENTER
	//ctx.fillStyle = 0xFFFFFF;

	int ax = pg->cXo + x;
	int ay = pg->cYo + y + height;
	BeginPath(pg->hDC);
	MoveToEx(pg->hDC, ax + RESIZE_HANDLE_SIZE, ay, NULL);
	LineTo(pg->hDC, ax, ay - RESIZE_HANDLE_SIZE);
	LineTo(pg->hDC, ax - RESIZE_HANDLE_SIZE, ay);
	LineTo(pg->hDC, ax + RESIZE_HANDLE_SIZE, ay);
	EndPath(pg->hDC);
	StrokeAndFillPath(pg->hDC);
}

bool Expand_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);
	
	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);
	return true;
}

bool Expand_Component::crank(Sheet* pg, int ax, int ay, bool check) {
	bool result = true;
	int oldHeight = height;
	int oldTop = y - height;

	if (check)
		// check for hit on resize box
		result = centerCrankVerticalResize(pg, this, ax, ay, true);
	else
		centerCrankVerticalResize(pg, this, ax, ay, false);
	if (height == oldHeight)
		return false;

	int pins = round((2 * (height - 2) - 2 * TERMINAL_SIZE) / TERMINAL_STEP + 1);
	if (pins < 2)
		pins = 2;
	if (pins > 6)
		pins = 6;

	if (setting[0] != pins) {
		height = pinToHeight(pins);
		y = oldTop + height;
		setting[0] = pins;
	}

	onLoad(pg);
	method2(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	drawCanvas2();

	// move the dial
	dial(pg);

	return result;
}

Expand_Component::Expand_Component()
	: Component(L"Expand", L"Expander", squareShape, 4, 1, 4, 1)
{
	setSetting(0, 4);
}


// ******************************* Bits **************************************


int BitJoin_Component::method2(Sheet* pg) {
	// pull the n left inputs into a single right output
	// empty inputs get zero output
	// array input get whole array added to the output

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	int n = leftTerminalCount;
	output[0].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		int bitValue = 1;
		int outputValue = 0;
		for (int j = 0; j < n; j++) {
			if (getLoopingValueD(j, k, 0))
				outputValue |= bitValue;
			bitValue *= 2;
		}
		output[0].value[k] = outputValue;
	}
	
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = loops;
	return true;
}
bool BitJoin_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, MULTI_PIN_MAX);
}

void BitJoin_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
bool BitJoin_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals are inputs
	n = min(n, 13);
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);

	return true;
}
BitJoin_Component::BitJoin_Component()
	: Component(L"BitJoin", L"Bit Joiner", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialogJoinButtonList;
	setSetting(0, 4);
	width /= 2;
}

int BitSplit_Component::method2(Sheet* pg)        // handle calc for a function with just one input
{
	// split the scaler left input into n bits on the right

	int n = rightTerminalCount;
	int loops = input[0].value.size();

	for (int j = 0; j < n; j++) {
		output[j].value.resize(loops);
		output[j].dimensions.resize(1);
		output[j].dimensions[0] = loops;
	}

	for (int k = 0; k < loops; k++) {

		int bitValue = 1;
		int integerInput = floor(getLoopingValueD(0, k, 0));
		
		for (int j = 0; j < n; j++) {
			if (integerInput & bitValue)
				output[j].value[k] = 1;
			else
				output[j].value[k] = 0;
			bitValue *= 2;
		}
	}
	return true;
}
void BitSplit_Component::dial(Sheet* pg) {
	commonDialSplit_(pg, this);
}
bool BitSplit_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankSplitLimit_(pg, this, x, y, check, 15);
}

bool BitSplit_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);

	return true;
}
BitSplit_Component::BitSplit_Component()
	: Component(L"BitSplit", L"Bit Splitter", squareShape, 1, 1, 4, 1)
{
	setSetting(0, 4);
	width /= 2;
	//dialogList = dialogSplitButtonList;
}


int BitsToNumber_Component::method2(Sheet* pg) {
	// array of bits to single scalar
	int n = input[0].value.size();
	int j;
	int bitValue = 1;
	int outputValue = 0;

	for (j = 0; j < n; j++) {
		if (input[0].value[j])
			outputValue |= bitValue;

		bitValue *= 2;
	}
	output[0].value.resize(1);
	output[0].value[0] = outputValue;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
	return true;
}
BitsToNumber_Component::BitsToNumber_Component()
	: Component(L"BitToN", L"Bits to Number", 1, 1){}


int NToBit_Component::method2(Sheet* pg)        // handle calc for a function with just one input
{
	if (input[0].value.size() == 0)
		return clearOutputs();

	// single scalar to array of bits
	int n = setting[0];
	int bitValue = 1;
	int integerInput = floor(input[0].value[0]);
	output[0].value.resize(n);

	for (int j = 0; j < n; j++) {
		if (integerInput & bitValue)
			output[0].value[j] = 1;
		else
			output[0].value[j] = 0;
		bitValue *= 2;
	}
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = n;
	return true;
}
NToBit_Component::NToBit_Component()
	: Component(L"NToBit", L"Number to Bits", 1, 1)
{
	setSetting(0, 16);
	//dialogList = dialogNToBitButtonList;
}


// ********************* Function Generator ******************************
double safeSlope(double x0, double y0, double x1, double y1) {
	double rise = y1 - y0;
	double run = x1 - x0;
	if (abs(run) < 1e-100) {
		if (run < 0)
			run = -1e-100;
		else
			run = 1e-100;
	}
	return rise / run;
}
int FunctionGenerator_Component::method2(Sheet* pg)
{

	double inputMinX = getLoopingValueD(0, 0, 0);
	double inputMaxX = getLoopingValueD(1, 0, 100);
	double inputStepX = getLoopingValueD(2, 0, 1);
	
	double inputMinY = getLoopingValueD(3, 0, 0);
	double inputMaxY = getLoopingValueD(4, 0, 100);
	if (inputMaxY <= inputMinY) {
		output[0].value.resize(1);
		output[0].value[0] = 0;
		return 1;
	}

	int xCount = (inputMaxX - inputMinX) / inputStepX;
	if (xCount < 3) {
		output[0].value.resize(1);
		output[0].value[0] = 0;
		return 1;
	}

	int inputXsize = input[5].value.size();
	int inputYsize = input[6].value.size();
	if( (inputXsize != inputYsize) || (inputXsize < 2)) {
		output[0].value.resize(1);
		output[0].value[0] = 0;
		return 1;
	}

	output[0].value.resize(xCount);
	int fromPoint = 0;
	int toPoint = 1;
	int section = 0; // 0 up to first point
	int outIndex = 0;
	double x = inputMinX;

	// flat before the first point
	while( (x < input[5].value[0]) && (outIndex < xCount-1)) {
		output[0].value[outIndex++] = input[6].value[0];
		x += inputStepX;
	}
	//skipping to first point
	while ((input[5].value[toPoint] < x) && (toPoint < inputXsize - 1)) {
		fromPoint++;
		toPoint++;
	}

	double slope = safeSlope(input[5].value[fromPoint], input[6].value[fromPoint],
							 input[5].value[toPoint], input[6].value[toPoint]);
	double intercept = input[6].value[fromPoint] - slope * input[5].value[fromPoint];

	for (; x < inputMaxX; x += inputStepX) {
		
		if (x > input[5].value[toPoint]) { // is this run over?
			fromPoint++;
			toPoint++;
			if (toPoint >= inputXsize)
				break;
			slope = safeSlope(input[5].value[fromPoint], input[6].value[fromPoint],
							  input[5].value[toPoint], input[6].value[toPoint]);
			intercept = input[6].value[fromPoint] - slope * input[5].value[fromPoint];
		}
		output[0].value[outIndex++] = slope * x + intercept;
	}

	// flat section after the last point
	while( outIndex < xCount)
		output[0].value[outIndex++] = input[6].value[inputXsize-1];

	// finally clean up the Y limits
	for (int k = 0; k < xCount; k++) {
		if (output[0].value[k] < inputMinY)
			output[0].value[k] = inputMinY;
		if (output[0].value[k] > inputMaxY)
			output[0].value[k] = inputMaxY;
	}
	output[0].dimensions[0] = xCount;
	return 1;
}

FunctionGenerator_Component::FunctionGenerator_Component()
	: Component(L"FuncGen", L"Func. Gen.", squareShape, 7, 1, 1, 1)
{
	setLeftLabels(5, "Min X", "Max X", "Step X", "Min Y", "Max Y", "Xs", "Xs");
}


// ********************* Histogram ******************************

int Histogram_Component::method2(Sheet* pg)
{
	if (!checkValueInputs())
		return clearOutputs();

	if (input[0].drivenByComponent == -1)    // data array
		return clearOutputs();
	
	int inputDataCount = input[0].value.size();

	double minimum = getLoopingValueD(1, 0, 0);
	double maximum = getLoopingValueD(2, 0, 100);
	double channelCount = getLoopingValueD(3, 0, 100);
	
	if( (inputDataCount < 2) || (minimum >= maximum) || (channelCount < 2) || (channelCount > 10000)){
		output[0].value.resize(1);
		output[0].value[0] = 0;
		return clearOutputs();
	}

	output[0].value.resize(channelCount);
	double channelStep = (maximum - minimum + 1) / channelCount;
	for (int k = 0; k < channelCount; k++)
		output[0].value[k] = 0;

	for (int k = 0; k < inputDataCount; k++) {
		int bin = floor((input[0].value[k] - minimum) / channelStep);
		if ((bin > 0) && (bin < channelCount))
			output[0].value[bin]++;
	}
	output[0].dimensions[0] = channelCount;
	return 1;
}

Histogram_Component::Histogram_Component()
	: Component(L"Histo", L"Histogram", squareShape, 4, 1, 1, 1)
{
	setLeftLabels(5, "Data", "Minimum", "Maximum", "Channels");
}


// ************************** Linear Regression **********************************


//xArray, yArray, xSize, wtArray, wtSize
void fit(vector<double> x, vector<double> y, int ndata, vector<double> sig, int mwt,
	double* reta, double *retb, double *retsiga, double *retsigb, double *retchi2 ) // returns a, b, siga, sigb, chi2
{
	int i;
	double wt, t, sxoss, sx = 0.0, sy = 0.0, st2 = 0.0, ss, sigdat;

	double b = 0.0;
	if (mwt) {
		ss = 0.0;
		for (i = 1; i <= ndata; i++) {
			wt = 1.0 / (sig[i] * sig[i]);
			ss += wt;
			sx += x[i] * wt;
			sy += y[i] * wt;
		}
	}
	else {
		for (i = 1; i <= ndata; i++) {
			sx += x[i];
			sy += y[i];
		}
		ss = ndata;
	}
	sxoss = sx / ss;
	if (mwt) {
		for (i = 1; i <= ndata; i++) {
			t = (x[i] - sxoss) / sig[i];
			st2 += t * t;
			b += t * y[i] / sig[i];
		}
	}
	else {
		for (i = 1; i <= ndata; i++) {
			t = x[i] - sxoss;
			st2 += t * t;
			b += t * y[i];
		}
	}
	b /= st2;
	double a = (sy - sx * (b)) / ss;
	double siga = sqrt((1.0 + sx * sx / (ss * st2)) / ss);
	double sigb = sqrt(1.0 / st2);
	double chi2 = 0.0;
	if (mwt == 0) {
		for (i = 1; i <= ndata; i++)
			chi2 += (y[i] - (a)-(b)* x[i])*(y[i] - (a)-(b)* x[i]);// square
		sigdat = sqrt((chi2) / (ndata - 2));
		siga *= sigdat;
		sigb *= sigdat;
	}
	else {
		for (i = 1; i <= ndata; i++)
			chi2 += ((y[i] - (a)-(b)* x[i]) / sig[i]) * ((y[i] - (a)-(b)* x[i]) / sig[i]);// square
	}

	*reta = a;
	*retb = b;
	*retsiga = siga;
	*retsigb = sigb;
	*retchi2 = chi2;
}



int LinearRegression_Component::method2(Sheet* pg) {

		int xSize = input[0].value.size();
		int ySize = input[1].value.size();
		int wtSize;
		if (input[2].drivenByComponent == -1)
			wtSize = 0;
		else
			wtSize = input[2].value.size();

		bool abort = false;
		if ((xSize < 2) || (ySize < 2))
			abort = true;
		else if (xSize != ySize)
			abort = true;
		else if ((wtSize != 0) && (wtSize != xSize))
			abort = true;
		if (abort) {
			for (int j = 0; j < 6; j++)
				output[j].value[0] = 0; // NOT READY
			return true;
		}

		// do it like CalcArrayLinearRegression in legacy code
		//var a;		// Slope
		//var b;		// Intercept
		//var siga;	// uncertanty of Slope
		//var sigb;	// uncertanty of Intercept
		//var chi2;	// chi-square
		//var q;		// goodness of fit probgability
		vector<double> xArray;
		vector<double> yArray;
		vector<double> wtArray;
		
		int ip;
		double xMin = 0;
		double xMax = 0;
		double yMin = 0;
		double yMax = 0;

		/* load up the temp x/y arrays */
		xArray.resize(xSize+1);
		yArray.resize(xSize + 1);
		wtArray.resize(xSize + 1);
		int k;
		for (ip = 0; ip < xSize; ip++) {
			k = ip + 1;
			xArray[k] = input[0].value[ip];
			yArray[k] = input[1].value[ip];

			if (wtSize != 0)		// param 2 is weight
			{
				wtArray[k] = input[2].value[ip];
				if (wtArray[k] < 0.000001)
					continue;
			}

			if (ip == 1) {
				xMin = xArray[k];
				xMax = xArray[k];
				yMin = yArray[k];
				yMax = yArray[k];
			}
			else {
				if (xMin > xArray[k])
					xMin = xArray[k];
				if (xMax < xArray[k])
					xMax = xArray[k];
				if (yMin > yArray[k])
					yMin = yArray[k];
				if (yMax < yArray[k])
					yMax = yArray[k];
			}
		}

		if (xMin == xMax)
			abort = true;
		if (abort) {
			for (int j = 0; j < 6; j++) {
				output[j].value[0] = 0; // NOT READY
				output[j].dimensions.clear();
			}
			return true;
		}
		if (yMin == yMax) {
			output[0].value[0] = 0;    // slope
			output[1].value[0] = yMin; // intercept
			output[2].value[0] = 0;
			output[3].value[0] = 0;  // sigb
			output[4].value[0] = 0;  // siga
			output[5].value[0] = 1; // READY
			for (int j = 0; j < 6; j++) {
				output[j].dimensions.resize(1);
				output[j].dimensions[0] = 1;
			}
			return true;
		}

		/* pick a regression */
		double a = 0, b = 0, siga = 0, sigb = 0, chi2 = 0;
		switch ((int)setting[0]) {
		case 0:    /* uncertanty in Y */
			fit(xArray, yArray, xSize, wtArray, wtSize, &a, &b, &siga, &sigb, &chi2);//, &q );
			break;

			//case 1: /* uncertanty in both X and Y */
			//    result = fitexy( xArray, yArray, k );//, &a, &b, &siga, &sigb, &chi2, &q );
			//    break;
		}

		output[0].value[0] = b;  // b is slope
		output[1].value[0] = a;  // a is intercept
		output[2].value[0] = chi2;
		output[3].value[0] = sigb;  // sigb
		output[4].value[0] = siga;  // siga
		output[5].value[0] = 1; // READY
		for (int j = 0; j < 6; j++) {
			output[j].dimensions.resize(1);
			output[j].dimensions[0] = 1;
		}
		return true;

}
LinearRegression_Component::LinearRegression_Component()
	: Component(L"LinReg", L"Linear Regression", squareShape, 3, 1, 6, 1)
{
	setLeftLabels(3, "X values", "Y values", "Weights");
	setRightLabels(3, "Slope", "Intercept", "Chi Squared", "Sigma Slope", "Sigma Intercept", "Ready");
	setSetting(0, 0); // input uncertanty in Y, 1 = uncertanty in both X and Y
	width = 70;
}


// ************************** Least Squares Fit **********************************


int svd_order;
vector<double> svd_x1;
vector<double> svd_x2;


double PYTHAG(double a, double b) {
	double at = abs(a);
	double bt = abs(b);
	if (at > bt) {
		double ct = bt / at;
		return at * sqrt(1.0 + ct * ct);
	}
	else {
		if (bt) {
			double ct = at / bt;
			return bt * sqrt(1.0 + ct * ct);
		}
		else
			return 0.0;
	}
}


double SIGN(double a, double b) {
	if (b >= 0.0) 
		return abs(a); 
	else 
		return -abs(a);
}


void svdcmp(vector<vector<double>> a, int m, int n, vector<double> w, vector<vector<double>> v)
// double **a,*w,**v;
//  int m,n;
{
	int flag, i, its, j, jj, k, l, nm;
	double c, f, h, s, x, y, z;
	double anorm = 0.0, g = 0.0, scale = 0.0;
	vector<double> rv1;

	if (m < n) 
		MessageBox(NULL, L"SVDCMP: You must augment A with extra zero rows", L"svdcmp", MB_OK);
		
	rv1.resize(n+1);
	for (i = 1; i <= n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if (i <= m) {
			for (k = i; k <= m; k++)
				scale += abs(a[k][i]);
			if (scale) {
				for (k = i; k <= m; k++) {
					a[k][i] /= scale;
					s += a[k][i] * a[k][i];
				}
				f = a[i][i];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][i] = f - g;
				if (i != n) {
					for (j = l; j <= n; j++) {
						for (s = 0.0, k = i; k <= m; k++) s += a[k][i] * a[k][j];
						f = s / h;
						for (k = i; k <= m; k++) a[k][j] += f * a[k][i];
					}
				}
				for (k = i; k <= m; k++) a[k][i] *= scale;
			}
		}
		w[i] = scale * g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if (i <= m && i != n) {
			for (k = l; k <= n; k++) scale += abs(a[i][k]);
			if (scale) {
				for (k = l; k <= n; k++) {
					a[i][k] /= scale;
					s += a[i][k] * a[i][k];
				}
				f = a[i][l];
				g = -SIGN(sqrt(s), f);
				h = f * g - s;
				a[i][l] = f - g;
				for (k = l; k <= n; k++) rv1[k] = a[i][k] / h;
				if (i != m) {
					for (j = l; j <= m; j++) {
						for (s = 0.0, k = l; k <= n; k++) s += a[j][k] * a[i][k];
						for (k = l; k <= n; k++) a[j][k] += s * rv1[k];
					}
				}
				for (k = l; k <= n; k++) a[i][k] *= scale;
			}
		}
		anorm = max(anorm, (abs(w[i]) + abs(rv1[i])));
	}
	for (i = n; i >= 1; i--) {
		if (i < n) {
			if (g) {
				for (j = l; j <= n; j++)
					v[j][i] = (a[i][j] / a[i][l]) / g;
				for (j = l; j <= n; j++) {
					for (s = 0.0, k = l; k <= n; k++) s += a[i][k] * v[k][j];
					for (k = l; k <= n; k++) v[k][j] += s * v[k][i];
				}
			}
			for (j = l; j <= n; j++) {
				v[i][j] = 0;
				v[j][i] = 0.0;
			}
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = n; i >= 1; i--) {
		l = i + 1;
		g = w[i];
		if (i < n)
			for (j = l; j <= n; j++) a[i][j] = 0.0;
		if (g) {
			g = 1.0 / g;
			if (i != n) {
				for (j = l; j <= n; j++) {
					for (s = 0.0, k = l; k <= m; k++) s += a[k][i] * a[k][j];
					f = (s / a[i][i]) * g;
					for (k = i; k <= m; k++) a[k][j] += f * a[k][i];
				}
			}
			for (j = i; j <= m; j++) a[j][i] *= g;
		}
		else {
			for (j = i; j <= m; j++) a[j][i] = 0.0;
		}
		++a[i][i];
	}
	for (k = n; k >= 1; k--) {
		for (its = 1; its <= 30; its++) {
			flag = 1;
			for (l = k; l >= 1; l--) {
				nm = l - 1;
				if (abs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (abs(w[nm]) + anorm == anorm) break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					if (abs(f) + anorm != anorm) {
						g = w[i];
						h = PYTHAG(f, g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 1; j <= m; j++) {
							y = a[j][nm];
							z = a[j][i];
							a[j][nm] = y * c + z * s;
							a[j][i] = z * c - y * s;
						}
					}
				}
			}
			z = w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = -z;
					for (j = 1; j <= n; j++) v[j][k] = (-v[j][k]);
				}
				break;
			}
			if (its == 30)
				MessageBox(NULL, L"No convergence in 30 SVDCMP iterations", L"svdcmp", MB_OK);
			
			x = w[l];
			nm = k - 1;
			y = w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = PYTHAG(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
			c = 1.0;
			s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = w[i];
				h = s * g;
				g = c * g;
				z = PYTHAG(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 1; jj <= n; jj++) {
					x = v[jj][j];
					z = v[jj][i];
					v[jj][j] = x * c + z * s;
					v[jj][i] = z * c - x * s;
				}
				z = PYTHAG(f, h);
				w[j] = z;
				if (z) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 1; jj <= m; jj++) {
					y = a[jj][j];
					z = a[jj][i];
					a[jj][j] = y * c + z * s;
					a[jj][i] = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
}




void svbksb(vector<vector<double>> u, vector<double>w, vector<vector<double>>v, int m, int n, vector<double> b, vector<double> *x)
//double **u,w[],**v,b[],x[];
//int m,n;
{
	int jj, j, i;
	double s;
	vector<double> tmp;
	tmp.resize(n + 1);
	for (j = 1; j <= n; j++) {
		s = 0.0;
		if (w[j]) {
			for (i = 1; i <= m; i++) s += u[i][j] * b[i];
			s /= w[j];
		}
		tmp[j] = s;
	}
	for (j = 1; j <= n; j++) {
		s = 0.0;
		for (jj = 1; jj <= n; jj++) s += v[j][jj] * tmp[jj];
		(*x)[j] = s;
	}
}


double svdfit(
	vector<double> x,		// input data points [1...ndata]
	vector<double> y,		// input data points [1...ndata]
	vector<double> sig,		// standard deviation for each point [1...ndata]
	int ndata,				// number of data points
	vector<double> *a,		// coefficients of the fitting function [1...ma]
	int ma,					// number of coefficients
	vector<vector<double>> u,	// workspace [1...ndata][1...ma]
	vector<vector<double>> v,	// workspace [1...ndata][1...ma]
	vector<double> w,		// workspace [1...ma]
	//double *chisq,		// output chi squared
	void (*funcs)( double, vector<double>, int)	// ANSI: void (*funcs)(float,float *,int);
)
//float x[],y[],sig[],a[],**u,**v,w[],*chisq;
//int ndata,ma;
//void (*funcs)();	/* ANSI: void (*funcs)(float,float *,int); */
{
	int j, i;
	double wmax, tmp, thresh, sum;
	vector<double> b;
	vector<double> afunc;
	//void svdcmp(),svbksb(),free_vector();

	b.resize(ndata + 1);
	afunc.resize(ma + 1);
	for (i = 1; i <= ndata; i++) {
		funcs(x[i], afunc, ma);
		tmp = 1.0 / sig[i];
		for (j = 1; j <= ma; j++)
			u[i][j] = afunc[j] * tmp;
		b[i] = y[i] * tmp;
	}
	svdcmp(u, ndata, ma, w, v);
	wmax = 0.0;
	for (j = 1; j <= ma; j++)
		if (w[j] > wmax)
			wmax = w[j];
	thresh = 1.0e-10 * wmax;
	for (j = 1; j <= ma; j++)
		if (w[j] < thresh)
			w[j] = 0.0;
	
	svbksb(u, w, v, ndata, ma, b, a);
	double chisq = 0.0;
	for (i = 1; i <= ndata; i++) {
		funcs(x[i], afunc, ma);
		for (sum = 0.0, j = 1; j <= ma; j++)
			sum += (*a)[j] * afunc[j];
		chisq += (tmp = (y[i] - sum) / sig[i], tmp * tmp);
	}
	return chisq;
}

void polyIn2Dimensions( double x, vector<double> p, int np) {
	int i = floor(x);

	switch (svd_order) {
	case 3: p.resize(11); break;
	case 2: p.resize(7); break; 
	case 1: p.resize(4); break;
	case 0: p.resize(2); break;
	}

	switch (svd_order) {
	case 3:
		p[7] = svd_x1[i] * svd_x1[i] * svd_x1[i];	// X^3
		p[8] = svd_x1[i] * svd_x1[i] * svd_x2[i];	// X^2 Y
		p[9] = svd_x1[i] * svd_x2[i] * svd_x2[i];	// X   Y^2
		p[10] = svd_x2[i] * svd_x2[i] * svd_x2[i];	//     Y^3
	case 2:
		p[4] = svd_x1[i] * svd_x1[i];	// X^2
		p[5] = svd_x1[i] * svd_x2[i];	// XY
		p[6] = svd_x2[i] * svd_x2[i];	// Y^2
	case 1:
		p[2] = svd_x1[i];	// X
		p[3] = svd_x2[i];	// Y
	case 0:
		p[1] = 1.0;
		break;
	}
}


bool my_2D_svdfit(
	vector<double>x1,	// input data points [1...ndata]
	vector<double>x2,	// input data points [1...ndata]
	vector<double>y,	// input data points [1...ndata]
	vector<double>sig,	// standard deviation for each point [1...ndata]
	int ndata,			// number of data points
	vector<double>*a,	// coefficients of the fitting function [1...ma]
	int ma 				// number of coefficients
				// output chi squared
) {
	vector<vector<double>> u;
	vector<vector<double>> v;
	vector<double> w;
	int j;
	vector<double> xn;

	// be sure they match
	switch (ma) {
	case 10:
		svd_order = 3;
		break;
	case 6:
		svd_order = 2;
		break;
	case 3:
		svd_order = 1;
		break;
	case 1:
		svd_order = 0;
		break;
	default:
		return false;
	}

	svd_x1 = x1;
	svd_x2 = x2;
	xn.resize(ndata+1);
	for (j = 1; j <= ndata; j++)
		xn[j] = j;

	u.resize(ndata + 1);
	v.resize(ndata + 1);
	for (j = 1; j <= ndata; j++) {
		u[j].resize(ma + 1);
		v[j].resize(ma + 1);
	}
	w.resize(ndata + 1);
	svdfit(xn, y, sig, ndata, a, ma, u, v, w, polyIn2Dimensions);

	return true; 
}

	// mode 1: y = f( x1, x2 )
bool calcArraySingularValueDecomposition2(vector<double>x1Array, vector<double>x2Array, vector<double>yArray, vector<double>sigma, int size, int coefficients, vector<double> *results) {
	int i;
	int k;//, size;
	double x1Min = 0;
	double x1Max = 0;
	double x2Min = 0;
	double x2Max = 0;
	double yMin = 0;
	double yMax = 0;

	// y = f( x1, x2 )

	/* create and load up the temp x/y arrays */
	//var NRsize = size + 1; // numerical recipes
	k = 1;
	for (i = 0; i < size; i++) {
		if (k == 1) {
			x1Min = x1Array[k];
			x1Max = x1Array[k];
			x2Min = x2Array[k];
			x2Max = x2Array[k];
			yMin = yArray[k];
			yMax = yArray[k];
		}
		else {
			if (x1Min > x1Array[k])
				x1Min = x1Array[k];
			if (x1Max < x1Array[k])
				x1Max = x1Array[k];
			if (x2Min > x2Array[k])
				x2Min = x2Array[k];
			if (x2Max < x2Array[k])
				x2Max = x2Array[k];
			if (yMin > yArray[k])
				yMin = yArray[k];
			if (yMax < yArray[k])
				yMax = yArray[k];
		}
		k++;
	}

	k--; // to get count
	if (k < coefficients)
		return false;

	if ((x1Min == x1Max) ||
		(x2Min == x2Max) ||
		(yMin == yMax))
		return false;


	/* Numerical Recipies to the rescue */
	double chisq = my_2D_svdfit(x1Array, x2Array, yArray, sigma, k, results, coefficients);

	return true;// { results : results, chi2 : chisq };
}


int LeastSquares_Component::method2(Sheet* pg) {

	for (int j = 0; j < 12; j++) {
		output[j].value[0] = 0;
		output[j].dimensions.clear();
	}

	for (int k = 0; k < 3; k++)
		if (input[k].drivenByComponent == -1)
			return true; // missing a required input

	int size = input[0].value.size();
	if ((input[1].value.size() != size) || (input[2].value.size() != size))
		return true; // inputs not the same size

	vector<double> x1data;
	vector<double> x2data;
	vector<double> ydata;
	x1data.resize(size+1);
	x2data.resize(size+1);
	ydata.resize(size+1);
	for (int j = 0; j < size; j++) {
		x1data[j + 1] = input[0].value[j];
		x2data[j + 1] = input[1].value[j];
		ydata[j + 1] = input[2].value[j];
	}
	vector<double> sigma;
	sigma.resize(size+1);
	if (input[3].drivenByComponent != -1) {
		if (input[3].value.size() != size)
			return true; // optional input not same size

		for (int j = 0; j < size; j++)
			sigma[j + 1] = input[3].value[j];
	}
	else {
		for (int j = 0; j < size; j++)
			sigma[j + 1] = 1;
	}

	double chi2 = 0;// not sure where this should come from
	vector<double> results;
	results.resize(setting[0] + 1);
	bool result = calcArraySingularValueDecomposition2(x1data, x2data, ydata, sigma, size, setting[0], &results);
	if (result) {
		for (int j = 0; j < setting[0]; j++)
			output[j].value[0] = results[j + 1];
		output[10].value[0] = chi2;
		output[11].value[0] = 1;

		for (int j = 0; j < 12; j++) {
			output[j].dimensions.resize(1);
			output[j].dimensions[0] = 1;
		}
		return true;
	}
	return false;
}
void LeastSquares_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	switch (radioValue) {
	case 0:	setting[0] = 1; break;
	case 1:	setting[0] = 3; break;
	case 2:	setting[0] = 6; break;
	case 3: setting[0] = 10; break;
	}

	onLoad(pg);
}
void doubleClickLeastSquares(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "First Order", "Second Order", "Third Order", "Fourth Order" };
	int radioIndex = 0;
	switch ((int)pg->dgrm.cL[i]->setting[0]) {
	case 1:	 radioIndex = 0; break;
	case 3:	 radioIndex = 1; break;
	case 6:	 radioIndex = 2; break;
	case 10: radioIndex = 3; break;
	}

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*> LeastSquaresButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickLeastSquares) };

LeastSquares_Component::LeastSquares_Component()
	: Component(L"LstSqr", L"Least Squares Fit", squareShape, 4, 1, 12, 1)
{ 
	dialogButtonList = LeastSquaresButtonList;
	setLeftLabels(3, "X0 values", "X1 values", "Y values", "Weights");
	setRightLabels(3, "C", "X0", "X1", "X0^2", "X0*X1", "X1^2", "X0^3", "X0^2*X1", "X0*X1^2", "X1^3", "Chi^2", "Ready");
	setSetting(0, 3); // 3) C, X1, X2; 6) plus X1^2, X1*X2, X2^2; 10) X1^3, X1^2*X2, X1*X2^2, X2^3
	width = 60;
	completionTestRadios = 4;
}

// ************************** Least Squares N Fit **********************************

vector<vector<double>>svd_xz;
int svd_nz;

void polyInNDimensions(double x, vector<double> p, int np) {
	int i = floor(x);
	int idx = 1;

	p[idx++] = 1.0;

	for (int z = 0; z < svd_nz; z++) {
		double pow = 1;
		for (int j = 0; j < svd_order; j++) {
			pow *= svd_xz[z][i];
			p[idx++] = pow;
		}
	}
}



double my_nD_svdfit(
	vector<vector<double>> x,	// input data points [0...n][1...ndata]
	int n,						// input dimensions as in x1, x2, xn
	vector<double> y,			// input data points [1...ndata]
	vector<double> sig,			// standard deviation for each point [1...ndata]
	int ndata,					// number of data points
	vector<double> *a,			// coefficients of the fitting function [1...ma]
	int ma 						// number of coefficients
	// function returns chi squared
) {
	vector<vector<double>> u;
	vector<vector<double>> v;
	vector<double> w;
	int j;
	vector<double> xn;

	svd_order = ma;
	svd_xz = x;
	svd_nz = n;
	xn.resize(ndata+1);
	for (j = 1; j <= ndata; j++)
		xn[j] = j;

	u.resize(ndata+1);
	v.resize(ndata+1);
	for (j = 1; j <= ndata; j++) {
		u[j].resize(ma * n + 2);
		v[j].resize(ma * n + 2);
	}
	w.resize(ma * n + 2);
	(*a).resize(ma * n + 2);// should be outsize
	return svdfit(xn, y, sig, ndata, a, ma * n + 1, u, v, w, polyInNDimensions);
}



bool calcArraySingularValueDecompositionN(vector<vector<double>> xArray, int n, vector<double> yArray, vector<double> sigma, int size, int coefficients, vector<double> *results) {
	int i;
	int k;//, size;
	vector<double> xMin;
	vector<double> xMax;
	double yMin;
	double yMax;

	// y = f( x1, x2, x3, ... xn )

	if (n >= xArray.size())
		return false;

	/* create and load up the temp x/y arrays */
	//var NRsize = size + 1; // numerical recipes
	k = 1;
	xMin.resize(size + 1);
	xMax.resize(size + 1);
	for (i = 0; i < size; i++) {

		if (k == 1) {
			for (int z = 0; z < n; z++) {
				xMin[z] = xArray[z][k];
				xMax[z] = xArray[z][k];
			}
			yMin = yArray[k];
			yMax = yArray[k];
		}
		else {
			for (int z = 0; z < n; z++) {
				if (xMin[z] > xArray[z][k])
					xMin[z] = xArray[z][k];
				if (xMax[z] < xArray[z][k])
					xMax[z] = xArray[z][k];
			}
			if (yMin > yArray[k])
				yMin = yArray[k];
			if (yMax < yArray[k])
				yMax = yArray[k];
		}
		k++;
	}

	k--; // to get count
	if (k < coefficients)
		return false;

	for (int z = 0; z < n; z++)
		if (xMin[z] == xMax[z])
			return false;

	/* Numerical Recipies to the rescue */
	double chisq = my_nD_svdfit(xArray, n, yArray, sigma, k, results, coefficients);

	return true;// results: results, chi2 : chisq
}


int LeastSquaresN_Component::method2(Sheet* pg) {
	
	int n = setting[1];
	for (int j = 0; j < n + 3; j++) {
		output[j].value[0] = 0;
		output[j].dimensions.clear();
	}

	for (int k = 0; k < n + 1; k++)
		if (input[k].drivenByComponent == -1)
			return true; // missing a required input

	int size = input[0].value.size();
	for (int k = 1; k < n + 1; k++)
		if (input[k].value.size() != size)
			return true; // inputs not the same size

	vector<vector<double>> xdata;
	xdata.resize(n);
	for (int k = 0; k < n; k++) {
		xdata[k].resize(size+1);
		for (int j = 0; j < size; j++)
			xdata[k][j + 1] = input[k].value[j];
	}
	vector<double> ydata;
	ydata.resize(size+1);
	for (int j = 0; j < size; j++)
		ydata[j + 1] = input[n].value[j];

	vector<double> sigma; // optional weight input is at [n+1]
	sigma.resize(size + 1);
	if (input[n + 1].drivenByComponent != -1) {
		if ((input[n + 1].value.size() != size) && (input[n + 1].value.size() != 1))
			return true; // optional input not same size
		if (input[n + 1].value.size() == size) {
			for (int j = 0; j < size; j++)
				sigma[j + 1] = input[n + 1].value[j];
		}
		else {
			for (int j = 0; j < size; j++)
				sigma[j + 1] = input[n + 1].value[0];
		}
	}
	else {
		for (int j = 0; j < size; j++)
			sigma[j + 1] = 1;
	}

	vector<double> results;
	double chi2 = 0; // send to func below?
	bool result = calcArraySingularValueDecompositionN(xdata, n, ydata, sigma, size, setting[0], &results);
	if (result) {
		int z = 1;
		output[0].value[0] = results[z++];
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = 1;
		for (int k = 0; k < n; k++) {
			for (int j = 0; j < setting[0]; j++)
				output[k + 1].value[j] = results[z++];
			output[k + 1].dimensions.resize(1);
			output[k + 1].dimensions[0] = setting[0];
		}
		output[n + 1].value[0] = chi2;
		output[n + 1].dimensions.resize(1);
		output[n + 1].dimensions[0] = 1;
		output[n + 2].value[0] = 1;
		output[n + 2].dimensions.resize(1);
		output[n + 2].dimensions[0] = 1;
	}
	return true;

}



bool LeastSquaresN_Component::onLoad(Sheet* pg) {

	int n = setting[1];
	height = pinToHeight(n);
	int j;

	// left terminals are inputs
	buildInputPins(n + 2);

	for (j = 0; j < n; j++)
		input[j].label = "X" + tomString(j + 1) + " values";
	input[n].label = "Y values";
	input[n+1].label = "Weights";

	// right terminals are outputs
	buildOutputPins(n + 3);

	output[0].label = "C";
	for (j = 1; j <= n; j++)
		output[j].label = "X" + tomString(j) + " coefs";
	output[n + 1].label = "Chi^2";
	output[n + 2].label = "Ready";

	height = pinToHeight(n + 3);
	return true;
}

void LeastSquaresN_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	
	if (doubleValues[0] >= 1)
		setting[0] = doubleValues[0]; // order
	if (doubleValues[1] >= 3)
		setting[1] = doubleValues[1]; // inputs

	onLoad(pg);
}
void LeastSquaresN_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Set Order:", "Set Number of Inputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0], pg->dgrm.cL[i]->setting[1] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}



std::vector<Button*> LeastSquaresNButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", LeastSquaresN_SetOptions_Action) };

LeastSquaresN_Component::LeastSquaresN_Component()
	: Component(L"LstSqrN", L"Least Squares N Fit", squareShape, 5, 1, 6, 1)
{
	dialogButtonList = LeastSquaresNButtonList;
	setLeftLabels(3, "X0 values", "X1 values", "X2 values", "Y values", "Weights");
	setRightLabels(3, "C", "X0 coefs", "X1 coefs", "X2 coefs", "Chi^2", "Ready");
	setSetting(0, 1); //  1) Xn; 2) Xn^2; 3) Xn^3; 4) Xn^4; ...
	setSetting(1, 3); //  number of inputs (functions in X)
	width = 60;
	completionTestDoubles = 2;
}


// ************************ buffer ***********************


int Buffer_Component::method2(Sheet* pg) {
	
	if (!checkValueInputs())
		return clearOutputs();

	int n = setting[0];

	const int push = 0;
	const int popFirst = 1;
	const int popLast = 2;
	const int clear = 3;

	// get control edges
	bool edge[4];
	for (int k = 0; k < 4; k++) {
		edge[k] = false;
		if (input[n + k].value.size()) {
			if (input[n + k].value[0]) {
				if (memory[k] == 0)
					edge[k] = true;
				memory[k] = 1;
			}
			else
				memory[k] = 0;
		}else
			memory[k] = 0;
	}

	if (edge[push]) {
		int size = ++output[n].value[0];
		for (int j = 0; j < n; j++) {
			output[j].value.push_back(input[j].value[0]);
			output[j].dimensions[0] = size;
		}
	}
	if (edge[popFirst]) {
		if (output[n].value[0] > 0) {
			int size = --output[n].value[0];
			for (int j = 0; j < n; j++) {
				output[j].value.erase(output[j].value.begin());
				output[j].dimensions[0] = size;
			}
		}
	}
	if (edge[popLast]) {
		if (output[n].value[0] > 0) {
			int size = --output[n].value[0];
			for (int j = 0; j < n; j++) {
				output[j].value.erase(output[j].value.begin()+size);
				output[j].dimensions[0] = size;
			}
		}
	}
	if (edge[clear]) {
		for (int j = 0; j < n; j++)
			output[j].value.clear();
		output[n].value[0] = 0;
	}
	return true;
}



bool Buffer_Component::onLoad(Sheet* pg) {
	
	int n = setting[0];
	int j;

	// left terminals are inputs
	buildInputPins(n + 4);

	for (j = 0; j < n; j++)
		input[j].label = "Data " + tomString(j);
	input[n].label = "Push";
	input[n + 1].label = "Pop First";
	input[n + 2].label = "Pop Last";
	input[n + 3].label = "Clear";

	// right terminals are outputs
	buildOutputPins(n+1);

	for (j = 0; j < n; j++)
		output[j].label = "Data " + tomString(j);
	output[n].label = "Size";

	for (int j = 0; j < n; j++)
		output[j].value.clear();
	output[n].value[0] = 0;

	height = pinToHeight(n + 4);
	return true;
}


void Buffer_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if ((doubleValues[0] > 0) && (doubleValues[0] < 32)) {
		setting[0] = doubleValues[0];
		onLoad(pg);
	}
}
void Buffer_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Inputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}

std::vector<Button*> BufferButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", Buffer_SetOptions_Action) };

Buffer_Component::Buffer_Component()
	: Component(L"Buffer", L"Buffer", squareShape, 5, 1, 2, 1)
{
	dialogButtonList = BufferButtonList;
	setSetting(0, 1); //  inputs / outputs
	setMemory(0, 0); // push edge memory
	setMemory(1, 0); // pop first edge memory
	setMemory(2, 0); // pop last edge memory
	setMemory(3, 0); // clear edge memory
	width = 50;
}


// ********************* String Width ******************************

int Str2dWid_Component::method2(Sheet* pg) {
	int loops = inputsToLoops();// AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	output[0].value.resize(loops);
	for (int j = 0; j < loops; j++) {
		string value = getLoopingStringValueD(0, j, "");
		//value = decodeUTF8(value);
		int size = getLoopingValueD(1, j, 9);
		output[0].value[j] = calcStringWidthFontSize(pg->hDC, size, value);
	}
	return true;
}



Str2dWid_Component::Str2dWid_Component()
	: Component(L"Str2dWid", L"String 2D Width", 2, 1)
{
	setLeftLabels(3, "String", "Size");
}

// *********************************** Spline ********************************************




double evalSpline(double x, vector<double> xs, vector<double> ys, vector<double>ks) {
	int i = 1;
	while (xs[i] < x) i++;

	double t = (x - xs[i - 1]) / (xs[i] - xs[i - 1]);

	double a = ks[i - 1] * (xs[i] - xs[i - 1]) - (ys[i] - ys[i - 1]);
	double b = -ks[i] * (xs[i] - xs[i - 1]) + (ys[i] - ys[i - 1]);

	return (1 - t) * ys[i - 1] + t * ys[i] + t * (1 - t) * (a * (1 - t) + b * t);
};




// SPLINE =============================================

void gaussJ_swapRows(vector<vector<double>> m, int k, int l) {
	vector<double> p = m[k];
	m[k] = m[l];
	m[l] = p;
};

bool gaussJ_solve(vector<vector<double>> A, vector<double> *x)	// in Matrix, out solutions
{
	int m = A.size();
	for (int k = 0; k < m; k++)	// column
	{
		// pivot for column
		int i_max = 0;
		double vali = -1e300;
		for (int i = k; i < m; i++) if (A[i][k] > vali) {
			i_max = i;
			vali = A[i][k];
		}
		gaussJ_swapRows(A, k, i_max);

		if (A[i_max][k] == 0) return false; //matrix is singular!

		// for all rows below pivot
		for (int i = k + 1; i < m; i++) {
			for (int j = k + 1; j < m + 1; j++)
				A[i][j] = A[i][j] - A[k][j] * (A[i][k] / A[k][k]);
			A[i][k] = 0;
		}
	}

	for (int i = m - 1; i >= 0; i--)	// rows = columns
	{
		double v = A[i][m] / A[i][i];
		(*x)[i] = v;
		for (int j = i - 1; j >= 0; j--)	// rows
		{
			A[j][m] -= A[j][i] * v;
			A[j][i] = 0;
		}
	}
	return true;
}

vector<vector<double>> gaussJ_zerosMat(int r, int c) {
	vector<vector<double>> A;
	A.resize(r);
	for (int i = 0; i < r; i++) {
		A[i].resize(c);
		for (int j = 0; j < c; j++) 
			A[i][j] = 0;
	}
	return A;
}

void getNaturalKs(vector<double> xs, vector<double> ys, vector<double> *ks)	// in x values, in y values, out k values
{
	int n = xs.size() - 1;
	vector<vector<double>> A = gaussJ_zerosMat(n + 1, n + 2);

	for (int i = 1; i < n; i++)	// rows
	{
		A[i][i - 1] = 1 / (xs[i] - xs[i - 1]);
		A[i][i] = 2 * (1 / (xs[i] - xs[i - 1]) + 1 / (xs[i + 1] - xs[i]));
		A[i][i + 1] = 1 / (xs[i + 1] - xs[i]);
		A[i][n + 1] = 3 * ((ys[i] - ys[i - 1]) / ((xs[i] - xs[i - 1]) * (xs[i] - xs[i - 1])) + (ys[i + 1] - ys[i]) / ((xs[i + 1] - xs[i]) * (xs[i + 1] - xs[i])));
	}

	A[0][0] = 2 / (xs[1] - xs[0]);
	A[0][1] = 1 / (xs[1] - xs[0]);
	A[0][n + 1] = 3 * (ys[1] - ys[0]) / ((xs[1] - xs[0]) * (xs[1] - xs[0]));

	A[n][n - 1] = 1 / (xs[n] - xs[n - 1]);
	A[n][n] = 2 / (xs[n] - xs[n - 1]);
	A[n][n + 1] = 3 * (ys[n] - ys[n - 1]) / ((xs[n] - xs[n - 1]) * (xs[n] - xs[n - 1]));

	gaussJ_solve(A, ks);
};


int Spline_Component::method2(Sheet* pg) {

	int dataSize = inputsToLoopsN(2);
	if (dataSize < 3)
		return clearOutputs();

	if( input[0].value.size() != dataSize )
		return clearOutputs();
	if (input[1].value.size() != dataSize)
		return clearOutputs();

	output[0].value.clear();
	//setSimpleOutputDimensions();

	vector<double> xs;
	vector<double> ys;
	vector<double> ks;
	
	for (int j = 0; j < dataSize; j++) {
		xs.push_back( getLoopingValueD(0, j, 0));
		ys.push_back( getLoopingValueD(1, j, 0));
	}
	ks.resize(dataSize);
	getNaturalKs(xs, ys, &ks);

	// then run the interp for each index
	int indexSize = input[2].value.size();
	output[0].value.resize(indexSize);
	for (int j = 0; j < indexSize; j++) {
		int index = getLoopingValueD(2, j, 0);
		if (index < xs[0])
			output[0].value[j] = ys[0];
		else if (index >= xs[dataSize - 1])
			output[0].value[j] = ys[dataSize - 1];
		else
			output[0].value[j] = evalSpline(index, xs, ys, ks);
	}
	copyDimensions(2, 0);

	return 1;
}

Spline_Component::Spline_Component()
	: Component(L"Spline", L"Spline", 3, 1)
{
	setLeftLabels(3, "X", "Y", "Index");
	setRightLabels(3, "Y");
}

int SplineXY_Component::method2(Sheet* pg) {

	int n = setting[0];
	int indexType = setting[1];
	int dataSize = inputsToLoopsN(n);
	if (dataSize < 3)
		return clearOutputs();

	for (int dim = 0; dim < n; dim++)
		if (input[dim].value.size() != dataSize)
			return clearOutputs();

	for (int dim = 0; dim < n; dim++)
		output[dim].value.clear();
	//setSimpleOutputDimensions();

	vector<double> xs;
	double limit;

	if (indexType == 1) {
		xs.push_back(0);
		vector<double> x;
		vector<double> oldx;
		double da;
		for (int dim = 0; dim < n; dim++)
			oldx.push_back( getLoopingValueD(dim, 0, 0));
		x.resize(n);
		xs.resize(dataSize);
		for (int j = 1; j < dataSize; j++) {
			double dist = 0;
			for (int dim = 0; dim < n; dim++) {
				x[dim] = getLoopingValueD(dim, j, 0);
				da = oldx[dim] - x[dim];
				dist += da * da;
				oldx[dim] = x[dim];
			}
			xs[j] = xs[j - 1] + sqrt(dist);
		}
		limit = xs[dataSize - 1];
	}
	else {
		for (int j = 0; j < dataSize; j++)
			xs.push_back( j);
		limit = dataSize - 1;
	}
	output[n].value[0] = limit;


	vector<double> ys;
	vector<double> ks;

	for (int dim = 0; dim < n; dim++) {
		// first calc ks derivitaves
		ys.clear();
		for (int j = 0; j < dataSize; j++)
			ys.push_back( getLoopingValueD(dim, j, 0));

		ks.clear();
		ks.resize(dataSize);
		getNaturalKs(xs, ys, &ks);

		// run the interp for each index
		int indexSize = input[n].value.size();
		output[dim].value.resize(indexSize);

		for (int j = 0; j < indexSize; j++) {
			double index = getLoopingValueD(n, j, 0);
			if (index < 0)
				output[dim].value[j] = ys[0];
			else if (index >= limit)
				output[dim].value[j] = ys[dataSize - 1];
			else
				output[dim].value[j] = evalSpline(index, xs, ys, ks);
		}
		copyDimensions(n, dim);
	}
		
	return 1;
}

void SplineXY_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if ((doubleValues[0] >= 2) && (doubleValues[0] <= 32))
		setting[0] = (int)doubleValues[0]; // dimensions

	if ((radioValue >= 0) && (radioValue <= 1))
		setting[1] = radioValue; // index type

	onLoad(pg);
	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
}
void doubleClickSplineXY(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Dimensions:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	vector<string> radioLabels = { "Index input is Points", "Index input is Distance" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		{}, {}, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*>splineXYButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickSplineXY) };

bool SplineXY_Component::onLoad(Sheet* pg) {
	if (setting.size() < 2)
		setSetting(1, 0);
	int n = setting[0];
	height = pinToHeight(n+1);

	// left terminals are inputs
	buildInputPins(n+1);
	for (int j = 0; j < n; j++)
		input[j].label = "Y" + tomString(j) + "s";
	if( setting[1] )
		input[n].label = "Distance";
	else
		input[n].label = "Index";

	// right terminals are outputs
	buildOutputPins(n + 1);
	for (int j = 0; j < n; j++)
		output[j].label = "Y" + tomString(j);
	output[n].label = "Max";

	width = 40;
	return true;
}

SplineXY_Component::SplineXY_Component()
	: Component(L"SplineXY", L"Spline XY", squareShape, 3, 1, 3, 1)
{
	dialogButtonList = splineXYButtonList;
	setLeftLabels(3, "X", "Y", "Index");
	setRightLabels(3, "X", "Y", "Max");
	setSetting(0, 2); // XY
	setSetting(1, 0); // 0-index, 1-distance
	completionTestDoubles = 1;
	completionTestRadios = 2;
}

// ********************* Get Until ****************************************

int GetUntil_Component::method2(Sheet* pg) {
	output[0].value.clear();
	output[0].stringValue.clear();
	output[0].dimensions[0] = 0;
	if (!checkValueAndStringInputs())
		return 1;

	if (input[0].drivenByComponent == -1)    // data array
		return 1;
	if (input[1].drivenByComponent == -1)    //  Until input
		return 1;
	bool isString = input[0].stringType;
	if (isString != input[1].stringType)
		return 1; // need both to be same type

	if (isString) {
		string untilString = input[1].stringValue[0];
		int arraySize = input[0].stringValue.size();

		for (int j = 0; j < arraySize; j++) {
			if (input[0].stringValue[j] == untilString)
				break;
			output[0].stringValue.push_back(input[0].stringValue[j]);
		}
		output[0].dimensions[0] = output[0].stringValue.size();
		output[0].stringType = true;
	}
	else
	{
		double untilValue = input[1].value[0];
		int arraySize = input[0].value.size();

		for (int j = 0; j < arraySize; j++) {
			if (input[0].value[j] == untilValue)
				break;
			output[0].value.push_back(input[0].value[j]);
		}
		output[0].dimensions[0] = output[0].value.size();
		output[0].stringType = false;
	}
	return 1;
}
GetUntil_Component::GetUntil_Component()
	: Component(L"GetUntil", L"Get Until", 2, 1)
{
	setLeftLabels(3, "Array", "Until");
}

// ********************** matrix get at **********************

int MatrixGetAt_Component::method2(Sheet* pg) {

	for (int j = 0; j < 3; j++)
		if (input[j].drivenByComponent == -1)
			return true; // not all connected
	if (input[0].dimensions.size() != 2)
		return clearOutputs(); // not matrix

	bool isString = input[0].stringType;
	int aSize;
	if( isString)
		aSize = input[0].stringValue.size();
	else
		aSize = input[0].value.size();

	int xSize = input[0].dimensions[0];
	int ySize = input[0].dimensions[1];
	if ((aSize < 1) || (xSize < 1) || (aSize < xSize))
		return clearOutputs();

	int loops = inputsToLoopsSN(1, 2);
	if( loops == 0)
		return clearOutputs();

	output[0].stringType = isString;
	if(isString)
		output[0].stringValue.resize(loops);
	else
		output[0].value.resize(loops);

	for (int j = 0; j < loops; j++) {

		int xIndex = floor(getLoopingValueD(1, j, 0));
		int yIndex = floor(getLoopingValueD(2, j, 0));
		
		if (xIndex < 0) xIndex = 0;
		if (xIndex >= xSize) xIndex = xSize - 1;

		if (yIndex < 0) yIndex = 0;
		if (yIndex >= ySize) yIndex = ySize - 1;

		int k = yIndex * xSize + xIndex;

		if (isString)
			output[0].stringValue[j] = input[0].stringValue[k];
		else
			output[0].value[j] = input[0].value[k];
	}

	setOutputDimension(0, 0, loops);

	return 1;
}
MatrixGetAt_Component::MatrixGetAt_Component()
	: Component(L"MatGetAt", L"Matrix Get At", 3, 1)
{
	setLeftLabels(3, "Matrix", "X Index", "Y Index");
}

// ****************************** Put At **********************************

	// find elements at the Index and change them
int PutAt_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();
	output[0].dimensions[0] = 0;

	bool isString = input[0].stringType; //data
	output[0].stringType = isString;
	if (input[3].drivenByComponent != -1)
		if (isString != input[3].stringType) // default
			return 1;

	int values;
	if( isString )
		values = input[0].stringValue.size(); // Data
	else
		values = input[0].value.size();

	int positions = input[1].value.size(); // position
	if (values != positions)
		return 1;

	int lastPosition;
	if (input[2].drivenByComponent != -1) {// Full Size is driven
		lastPosition = getDoubleInputOrZero(2, 0);
		if ((lastPosition < 0) || (lastPosition > 100000))
			return 1;
	}
	else {
		// Full Size is not driven, find the biggest position
		lastPosition = -1;
		for (int k = 0; k < positions; k++)
			if (input[1].value[k] > lastPosition) // position
				lastPosition = input[1].value[k]; // position
		lastPosition = floor(lastPosition) + 1;
		if ((lastPosition < 0) || (lastPosition > 100000))
			return 1;
	}

	// fill the output with default values
	if (isString) {
		// allocate the output space
		output[0].stringValue.resize(lastPosition);
		int defaultSize = input[3].stringValue.size(); // default values
		if (defaultSize == 1) {
			// one default value
			for (int k = 0; k < lastPosition; k++)
				output[0].stringValue[k] = input[3].stringValue[0];
		}
		else if (defaultSize == lastPosition) {
			// full set of default values
			for (int k = 0; k < lastPosition; k++)
				output[0].stringValue[k] = input[3].stringValue[k];
		}
		else {
			// mismatch, just repeat the few defaults we have
			int n = 0;
			for (int k = 0; k < lastPosition; k++) {
				output[0].stringValue[k] = input[3].stringValue[n++];
				if (n >= defaultSize)
					n = 0;
			}
		}
		// now Put the input values as specified by the position input
		for (int k = 0; k < values; k++) {
			int pos = floor(input[1].value[k]);
			if ((pos >= 0) && (pos < lastPosition))
				output[0].stringValue[pos] = input[0].stringValue[k];
		}
	}
	else {
		// allocate the output space
		output[0].value.resize(lastPosition);
		int defaultSize = input[3].value.size(); // default values
		if (defaultSize == 1) {
			// one default value
			for (int k = 0; k < lastPosition; k++)
				output[0].value[k] = input[3].value[0];
		}
		else if (defaultSize == lastPosition) {
			// full set of default values
			for (int k = 0; k < lastPosition; k++)
				output[0].value[k] = input[3].value[k];
		}
		else {
			// mismatch, just repeat the few defaults we have
			int n = 0;
			for (int k = 0; k < lastPosition; k++) {
				output[0].value[k] = input[3].value[n++];
				if (n >= defaultSize)
					n = 0;
			}
		}
		// now Put the input values as specified by the position input
		for (int k = 0; k < values; k++) {
			int pos = floor(input[1].value[k]);
			if ((pos >= 0) && (pos < lastPosition))
				output[0].value[pos] = input[0].value[k];
		}
	}

	output[0].dimensions[0] = lastPosition;
	return 1;
}


PutAt_Component::PutAt_Component()
	: Component(L"PutAt", L"Put At", squareShape, 4, 1, 1, 1)
{
	setLeftLabels(3, "Value(s)", "Position(s)", "Full Size", "Default(s)");
}


// ************************** Arrange ************************************

int Arrange_Component::method2(Sheet *pg) {
	output[0].value.clear();
	output[0].stringValue.clear();
	setSimpleOutputDimensions();

	if (input[0].drivenByComponent == -1)    // data array
		return true;
	if (input[1].drivenByComponent == -1)    // integer array
		return true;

	bool isString = input[0].stringType;
	output[0].stringType = isString;

	int jout = 0;
	int size = input[1].value.size();
	int limit;
	if( isString )
		limit = input[0].stringValue.size();
	else
		limit = input[0].value.size();

	int k;
	for (int jin = 0; jin < size; jin++) {
		k = input[1].value[jin];
		if (k < 0)
			continue;
		if (k >= limit)
			continue;
		if( isString )
			output[0].stringValue.push_back( input[0].stringValue[k]);
		else
			output[0].value.push_back(input[0].value[k]);
	}
	setSimpleOutputDimensions();
	return 1;
}

Arrange_Component::Arrange_Component()
	: Component(L"Arrange", L"Arrange", 2, 1)
{
	setLeftLabels(3, "Array", "Order");
}

// *************************** Merge ********************************

int Merge_Component::method2(Sheet* pg)
{
	// pull the n left inputs into a single right output
	// empty inputs get zero output
	// array input get whole array added to the output
	int n = leftTerminalCount;
	int j;
	int k;
	int q = 0;

	// what is the largest input length
	int length = 0;
	for (j = 0; j < n; j++)
		if (input[j].drivenByComponent != -1)
			if (input[j].value.size() > length)
				length = input[j].value.size();

	output[0].value.clear();
	for (k = 0; k < length; k++) {
		for (j = 0; j < n; j++) {
			if (input[j].drivenByComponent != -1) {
				if (k < input[j].value.size())
					output[0].value.push_back(input[j].value[k]);
				else
					output[0].value.push_back(0);
			}
			else
				output[0].value.push_back(0);
		}
	}
	setSimpleOutputDimensions();
	return 1;
}
void Merge_Component::dial(Sheet* pg) {
	drawResizeBox(pg, this);
}
bool Merge_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankJoinLimit_(pg, this, x, y, check, 15);
}

bool Merge_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// left terminals 
	buildInputPins(n);
	for (int j = 0; j < n; j++)
		input[j].label = tomString(j);

	return true;
}
Merge_Component::Merge_Component()
	: Component(L"Merge", L"Merge", squareShape, 4, 1, 1, 1)
{
	setSetting(0, 4);
	width /= 2;
}

//*********************** Shuffle *******************************

int Shuffle_Component::method2(Sheet* pg)
{
	if (input[1].value.size() == 0)
		return 0;
	if ((input[1].value[0] == 0) && // trigger input is low
		(memory[0] == 1)) {			// trigger input was high
		memory[0] = 0;				// remember the input is low
		return 0;				// but do nothing
	}
	if (input[1].value[0] != 0)		// trigger input is high
		memory[0] = 1;				// remember its high

	// split the single left input into 2 groups and shuffle together - like dealing cards
	bool isString = input[0].stringType;

		// split in half
	int len;
	if( isString )
		len = input[0].stringValue.size();
	else
		len = input[0].value.size();

	int lenA = floor(len / 2);
	int lenB = len - lenA;
	int k = 0;
	int a = 0;
	int b = 0;
	int midRand = RAND_MAX / 2;

	if (isString) {
		output[0].stringValue.resize(len);
		output[0].value.clear();
		output[0].stringType = true;

		while ((a < lenA) && (b < lenB)) {
			if (rand() > midRand)
				output[0].stringValue[k++] = input[0].stringValue[a++];
			else
				output[0].stringValue[k++] = input[0].stringValue[lenA + (b++)];
		}
		while (a < lenA)
			output[0].stringValue[k++] = input[0].stringValue[a++];
		while (b < lenB)
			output[0].stringValue[k++] = input[0].stringValue[lenA + (b++)];
	}
	else {
		output[0].stringValue.clear();
		output[0].value.resize(len);
		output[0].stringType = false;

		while ((a < lenA) && (b < lenB)) {
			if (rand() > midRand)
				output[0].value[k++] = input[0].value[a++];
			else
				output[0].value[k++] = input[0].value[lenA + (b++)];
		}
		while (a < lenA)
			output[0].value[k++] = input[0].value[a++];
		while (b < lenB)
			output[0].value[k++] = input[0].value[lenA + (b++)];
	}

	output[0].dimensions[0] = len;

	return 1;
}
Shuffle_Component::Shuffle_Component()
	: Component(L"Shuffle", L"Shuffle", 2, 1)
{
	setLeftLabels(2, "Array", "Trigger");
	setMemory(0, 0); // for edge of trigger input
}

// *********************** Deal ************************************

int Deal_Component::method2(Sheet* pg)
{
	// split the single left input into n right outputs - like dealing cards

	int n = rightTerminalCount;
	bool isStringType = input[0].stringType;

	for (int j = 0; j < n; j++) {
		output[j].value.clear();
		output[j].stringValue.clear();
		output[j].stringType = isStringType;
	}

	int k = 0;
	int len;
	if (isStringType) {
		len = input[0].stringValue.size();
		for (int j = 0; j < len; j++) {
			output[k++].stringValue.push_back(input[0].stringValue[j]);
			if (k >= n)
				k = 0;
		}
	}
	else {
		len = input[0].value.size();
		for (int j = 0; j < len; j++) {
			output[k++].value.push_back(input[0].value[j]);
			if (k >= n)
				k = 0;
		}
	}
	setSimpleOutputDimensions();

	return 1;
}
void Deal_Component::dial(Sheet* pg) {
	commonDialSplit_(pg, this);
}
bool Deal_Component::crank(Sheet* pg, int x, int y, bool check) {
	return crankSplitLimit_(pg, this, x, y, check, 15);
}
bool Deal_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals 
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);

	return true;
}

Deal_Component::Deal_Component()
	: Component(L"Deal", L"Deal", squareShape, 1, 1, 4, 1)
{
	setSetting(0, 4);
	width /= 2;
}


// ************************ Find Index ***********************************

int FindIndex_Component::method2(Sheet* pg)
{
	output[0].value.clear();
	setSimpleOutputDimensions();

	if (input[0].drivenByComponent == -1)    // data array
		return true;
	if (input[1].drivenByComponent == -1)    //  Key
		return true;

	bool isString = input[0].stringType;
	if (input[1].stringType != isString)
		return true;

	int size;
	int keys;
	bool found;
	if (isString) {
		size = input[0].stringValue.size();
		keys = input[1].stringValue.size();

		for (int jin = 0; jin < size; jin++) {
			found = false;
			for (int k = 0; k < keys; k++) {
				if (input[0].stringValue[jin] == input[1].stringValue[k])
					found = true;
			}
			if (found)
				output[0].value.push_back(jin);
		}
	}
	else {
		size = input[0].value.size();
		keys = input[1].value.size();

		for (int jin = 0; jin < size; jin++) {
			found = false;
			for (int k = 0; k < keys; k++) {
				if (input[0].value[jin] == input[1].value[k])
					found = true;
			}
			if (found)
				output[0].value.push_back(jin);
		}
	}
	setSimpleOutputDimensions();

	return 1;
}
FindIndex_Component::FindIndex_Component()
	: Component(L"FindIndex", L"Find Index", 2, 1)
{
	setLeftLabels(2, "Array", "Key(s)");
}

// ***************************** Find Bool *******************************************

int FindBool_Component::method2(Sheet* pg)
{
	output[0].value.clear();
	setSimpleOutputDimensions();

	if (input[0].drivenByComponent == -1)    // data array
		return true;
	if (input[1].drivenByComponent == -1)    //  Key
		return true;

	bool isString = input[0].stringType;
	if (input[1].stringType != isString)
		return true;

	int size;
	int keys;
	bool found;
	if (isString) {
		size = input[0].stringValue.size();
		keys = input[1].stringValue.size();

		for (int jin = 0; jin < size; jin++) {
			found = false;
			for (int k = 0; k < keys; k++) {
				if (input[0].stringValue[jin] == input[1].stringValue[k])
					found = true;
			}
			output[0].value.push_back(found);
		}
	}
	else {
		size = input[0].value.size();
		keys = input[1].value.size();

		for (int jin = 0; jin < size; jin++) {
			found = false;
			for (int k = 0; k < keys; k++) {
				if (input[0].value[jin] == input[1].value[k])
					found = true;
			}
			output[0].value.push_back(found);
		}
	}
	setSimpleOutputDimensions();
	
	return 1;
}
FindBool_Component::FindBool_Component()
	: Component(L"FindBool", L"Find Bool", 2, 1)
{
	setLeftLabels(2, "Array", "Key");
}


// *********************** Ravel *********************************


int Ravel_Component::method2(Sheet* pg)
{
	output[0].value.clear();
	
	if (input[0].drivenByComponent == -1)   
		return true;
	if (input[1].drivenByComponent == -1)   
		return true;

	// check for a match
	int dimSize = 1;
	int dims = input[1].value.size();
	for (int j = 0; j < dims; j++)
		dimSize *= input[1].value[j];

	int size = input[0].value.size();
	if (size != dimSize)
		return true;

	output[0].value.resize(size);

	for (int j = 0; j < size; j++) 
		output[0].value[j] = input[0].value[j];
		
	output[0].dimensions.resize(dims);
	for (int j = 0; j < dims; j++)
		output[0].dimensions[j] = input[1].value[j];

	return 1;
}
Ravel_Component::Ravel_Component()
	: Component(L"Ravel", L"Ravel", 2, 1)
{
	//crank = crankJoin_;
	//dial = dialJoin_;
	//setSetting(0, 2);
	setLeftLabels(2, "Data", "Dimensions");
	//onLoad = onLoadRavel_;
	width = 40;
}

// ************************* Filter ******************************


int Filter_Component::method2(Sheet* pg) {

	output[0].value.clear();
	setSimpleOutputDimensions();

	if (input[0].drivenByComponent == -1)    // data array
		return true;
	if (input[1].drivenByComponent == -1)    //  filter array
		return 1;

	int dataSize = input[0].value.size();
	output[0].value.resize( dataSize);

	int filterSize = input[1].value.size();
	if (dataSize < filterSize) {
		return 1;
	}

	int half = floor(filterSize / 2);
	for (int j = 0; j < dataSize; j++) {
		double out = 0;
		for (int k = 0; k < filterSize; k++) {
			int jj = j - half + k;
			if (jj < 0)
				continue;
			if (jj >= dataSize)
				continue;
			out += input[1].value[k] * input[0].value[jj];
		}
		output[0].value[j] = out;
	}
	setSimpleOutputDimensions();
	return 1;
}

Filter_Component::Filter_Component()
	: Component(L"Filter", L"Filter",  squareShape, 2, 1, 1, 1)
{
	setLeftLabels(3, "Data", "Filter");
}

// ************************* Matrix Subset ****************************

int MatrixSubset_Component::method2(Sheet* pg) {

	output[0].value.clear();

	if ((input[0].drivenByComponent == -1) ||
		(input[3].drivenByComponent == -1) ||
		(input[4].drivenByComponent == -1))
		return clearOutputs();
	if (input[0].dimensions.size() < 2)
		return clearOutputs();

	bool isString = input[0].stringType;
	int aSize;
	if (isString)
		aSize = input[0].stringValue.size();
	else
		aSize = input[0].value.size();

	int xSize = input[0].dimensions[0];
	int ySize = input[0].dimensions[1];

	int xStart = floor(getLoopingValueD(1, 0, 0));
	int yStart = floor(getLoopingValueD(2, 0, 0));
	int xWidth = floor(getLoopingValueD(3, 0, 0));
	int yHeight = floor(getLoopingValueD(4, 0, 0));
	if ((aSize < 1) || (xSize < 1) || (xWidth < 1) || (yHeight < 1) || (aSize < xSize))
		return 1;

	if (xStart < 0) xStart = 0;
	if (xStart >= xSize) xStart = xSize - 1;
	if (xStart + xWidth > xSize) xWidth = xSize - xStart;

	if (yStart < 0) yStart = 0;
	if (yStart >= ySize) yStart = ySize - 1;
	if (yStart + yHeight > ySize) yHeight = ySize - yStart;

	output[0].stringType = isString;
	if (isString)
		output[0].stringValue.resize(xWidth * yHeight);
	else
		output[0].value.resize(xWidth * yHeight);

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = xWidth;
	output[0].dimensions[1] = yHeight;

	int j = 0;
	int k = yStart * xSize + xStart;
	for (int y = 0; y < yHeight; y++) {
		for (int x = 0; x < xWidth; x++) {
			if (isString)
				output[0].stringValue[j] = input[0].stringValue[k];
			else
				output[0].value[j++] = input[0].value[k++];
		}
		k += xSize - xWidth;
	}

	return 1;
}
MatrixSubset_Component::MatrixSubset_Component()
	: Component(L"MatSubset", L"Matrix Subset", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "Matrix", "X start", "Y start", "X width", "Y height");
	setRightLabels(3, "Matrix");
	width = 40;

}
// ************************ Matrix Excise ***************************

int MatrixExcise_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	if ((input[0].drivenByComponent == -1) ||
		(input[2].drivenByComponent == -1))
		return clearOutputs();

	int aSize = input[0].value.size();
	if (aSize < 1)
		return clearOutputs();
	if (input[0].dimensions.size() < 2)
		return clearOutputs();
	int xSize = input[0].dimensions[0];
	if ((xSize < 1) || (aSize < xSize))
		return clearOutputs();
	int ySize = input[0].dimensions[1];
	if ((ySize < 1) || (aSize < ySize))
		return clearOutputs();

	output[0].value.clear();

	int xx, yy;
	int jIn, jOut;
	int outSize;
	int xStart, xWidth;
	int yStart, yHeight;

	switch ((int)setting[0]) {
	case 0: // X mode
		xStart = floor(input[1].value[0]);
		xWidth = floor(input[2].value[0]);

		if (xStart < 0) { // trim off the non existant left size of the range
			xWidth += xStart;
			xStart = 0;
		}
		if (xWidth < 0)
			xWidth = 0;
		if (xStart >= xSize) { // setup to skip if range starts beyond array
			xStart = xSize;
			xWidth = 0;
		}
		if (xStart + xWidth > xSize) // trim if end is beyone array 
			xWidth = xSize - xStart;

		outSize = xSize - xWidth;
		output[0].value.resize(outSize * ySize);
		output[0].dimensions.resize(2);
		output[0].dimensions[0] = outSize;
		output[0].dimensions[1] = ySize;

		// copy columns left of xStart
		for (xx = 0; xx < xStart; xx++) {
			for (yy = 0; yy < ySize; yy++) {
				jIn = xx + yy * xSize;
				jOut = xx + yy * outSize;
				output[0].value[jOut] = input[0].value[jIn];
			}
		}
		// then the rest
		for (xx = xStart + xWidth; xx < xSize; xx++) {
			for (yy = 0; yy < ySize; yy++) {
				jIn = xx + yy * xSize;
				jOut = (xx - xWidth) + yy * outSize;
				output[0].value[jOut] = input[0].value[jIn];
			}
		}
		break;
	case 1: // Y mode
		yStart = floor(input[1].value[0]);
		yHeight = floor(input[2].value[0]);

		if (yStart < 0) {
			yHeight += yStart;
			yStart = 0;
		}
		if (yHeight < 0)
			yHeight = 0;
		if (yStart >= ySize) { // setup to skip if range starts beyond array
			yStart = ySize;
			yHeight = 0;
		}
		if (yStart+yHeight > ySize)
			yHeight = ySize - yStart;

		outSize = ySize - yHeight;
		output[0].value.resize(outSize * xSize);
		output[0].dimensions.resize(2);
		output[0].dimensions[0] = xSize;
		output[0].dimensions[1] = outSize;

		// copy rows above of yStart
		for (xx = 0; xx < xSize; xx++) {
			for (yy = 0; yy < yStart; yy++) {
				jIn = xx + yy * xSize;
				output[0].value[jIn] = input[0].value[jIn];
			}
		}
		// then the rest
		for (xx = 0; xx < xSize; xx++) {
			for (yy = yStart + yHeight; yy < ySize; yy++) {
				jIn = xx + yy * xSize;
				jOut = xx + (yy - yHeight) * xSize;
				output[0].value[jOut] = input[0].value[jIn];
			}
		}
		break;
	}

	return 1;
}
void MatrixExcise_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	
	if ((radioValue >= 0) && (radioValue <= 1))
		setting[0] = radioValue; 

	propagateValues(&pg->dgrm);
}
void doubleClickMatrixExcise(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Excise Columns", "Excise Rows" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}
std::vector<Button*>matrixExciseButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickMatrixExcise) };

MatrixExcise_Component::MatrixExcise_Component()
	: Component(L"MatExcise", L"Matrix Excise", 3, 1)
{
	dialogButtonList = matrixExciseButtonList;
	setLeftLabels(3, "Matrix", "Start", "Size");
	setRightLabels(1, "Matrix");
	setSetting(0, 0); // 0=excise X, 1=excise Y
	width = 40;
	completionTestRadios = 2;
}

// ************************ Matrix Inversion ****************************



void lubksb(vector<vector<double>> a, int n, vector<int> *indx, vector<double> *b) {
	int i, ii = 0, ip, j;
	double sum;

	for (i = 1; i <= n; i++) {
		ip = (*indx)[i];
		sum = (*b)[ip];
		(*b)[ip] = (*b)[i];
		if (ii)
			for (j = ii; j <= i - 1; j++) sum -= a[i][j] * (*b)[j];
		else if (sum) ii = i;
		(*b)[i] = sum;
	}
	for (i = n; i >= 1; i--) {
		sum = (*b)[i];
		for (j = i + 1; j <= n; j++) sum -= a[i][j] * (*b)[j];
		(*b)[i] = sum / a[i][i];
	}
}





bool ludcmp(vector<vector<double>> *a, int n, vector<int> *indx, double* d) {
	//int ludcmp( double **a, int n, int *indx, double *d )
	int imax = 0;
	double big, dum, sum, temp;
	vector<double> vv;
	vv.resize(n + 1); // just a guess
	*d = 1.0;

	int ii;
	for (ii = 1; ii <= n; ii++) {
		big = 0.0;
		for (int j = 1; j <= n; j++)
			if ((temp = abs((*a)[ii][j])) > big)
				big = temp;
		if (big == 0.0) //nrerror( "Singular matrix in routine ludcmp" );
			return false;

		vv[ii] = 1.0 / big;
	}
	for (int j = 1; j <= n; j++) {
		for (ii = 1; ii < j; ii++) {
			sum = (*a)[ii][j];
			for (int k = 1; k < ii; k++)
				sum -= (*a)[ii][k] * (*a)[k][j];
			(*a)[ii][j] = sum;
		}
		big = 0.0;
		for (ii = j; ii <= n; ii++) {
			sum = (*a)[ii][j];
			for (int k = 1; k < j; k++)
				sum -= (*a)[ii][k] * (*a)[k][j];
			(*a)[ii][j] = sum;
			if ((dum = vv[ii] * abs(sum)) >= big) {
				big = dum;
				imax = ii;
			}
		}
		if (j != imax) {
			for (int k = 1; k <= n; k++) {
				dum = (*a)[imax][k];
				(*a)[imax][k] = (*a)[j][k];
				(*a)[j][k] = dum;
			}
			*d = -*d;
			vv[imax] = vv[j];
		}
		(*indx)[j] = imax;
		if ((*a)[j][j] == 0.0)
			(*a)[j][j] = 1.0e-20;
		if (j != n) {
			dum = 1.0 / ((*a)[j][j]);
			for (ii = j + 1; ii <= n; ii++)
				(*a)[ii][j] *= dum;
		}
	}
	return true;// indx : indx, d : d, a : a };
}




int MatrixInversion_Component::method2(Sheet* pg) {

	if ((input[0].drivenByComponent == -1) ||
		(input[0].dimensions.size() != 2))
		return clearOutputs();

	int mSize = input[0].value.size();
	int xSize = input[0].dimensions[0]; // i to n
	int ySize = input[0].dimensions[1]; // k to m

	if ((mSize < 1) || (xSize < 1) || (xSize != ySize))
		return clearOutputs();

	int n = xSize;
	vector<vector<double>> a;
	int j, k, m;
	a.resize(n + 1);
	for (j = 1; j <= n; j++)
		a[j].resize(n + 1);

	m = 0;
	for (j = 1; j <= n; j++) {
		for (k = 1; k <= n; k++) 
			a[k][j] = input[0].value[m++];
	}

	vector<int> indx;
	indx.resize(n + 1);
	double d;

	bool result = ludcmp(&a, n, &indx, &d);//, index, &determinant ) != 0 )
	if (!result)
		return clearOutputs();

	output[0].value.resize( xSize * ySize);
	output[0].dimensions.resize(2);
	output[0].dimensions[0] = xSize;
	output[0].dimensions[1] = ySize;

	//vector<vector<double>> matY;
	//matY.resize(n+1);

	vector<double> column;
	column.resize(n + 1);
	m = 0;
	for (int jj = 1; jj <= n; jj++) {
		for (int ii = 1; ii <= n; ii++)
			column[ii] = 0.0;

		column[jj] = 1.0;

		lubksb(a, n, &indx, &column);

		for (int ii = 1; ii <= n; ii++)
			output[0].value[m++] = column[ii];
	}

	return 1;
}
MatrixInversion_Component::MatrixInversion_Component()
	: Component(L"MatInv", L"Matrix Invert", 1, 1)
{
	setLeftLabels(1, "Matrix");
}


// ********************************* Quick Sort ***************************


// A utility function to swap two elements 
void swapVec(vector<double>* arrPtr, int a, int b)
{
	double t = (*arrPtr)[a];
	(*arrPtr)[a] = (*arrPtr)[b];
	(*arrPtr)[b] = t;
}

/* This function takes last element as pivot, places the pivot element at its 
	correct position in sorted array, and places all smaller (smaller than pivot) 
	to left of pivot and all greater elements to right of pivot */
int partition(vector<double> *arrPtr, int low, int high)
{
	int pivot = (*arrPtr)[high]; // pivot 
	int i = (low - 1); // Index of smaller element 

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or 
		// equal to pivot 
		if ((*arrPtr)[j] <= pivot)
		{
			i++; // increment index of smaller element 
			swapVec(arrPtr, i, j);
		}
	}
	swapVec(arrPtr, i + 1, high);
	return (i + 1);
}

/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low --> Starting index,
high --> Ending index */
void quickSort(vector<double> *arrPtr, int low, int high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now at right place */
		int pi = partition(arrPtr, low, high);

		// Separately sort elements before 
		// partition and after partition 
		quickSort(arrPtr, low, pi - 1);
		quickSort(arrPtr, pi + 1, high);
	}
}

// ********************************* Numerical Sort ***************************

int NumericalSort_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	int size = input[0].value.size();
	output[0].value.resize(size);

	for (int j = 0; j < size; j++)
		output[0].value[j] = input[0].value[j];

	quickSort( &output[0].value, 0, size - 1);

	return 1;
}
NumericalSort_Component::NumericalSort_Component()
	: Component(L"NSort", L"Numerical Sort", 1, 1)
{
}

// ********************************* Numerical Rank ***************************




/* This function takes last element as pivot, places the pivot element at its
	correct position in sorted array, and places all smaller (smaller than pivot)
	to left of pivot and all greater elements to right of pivot */
int partitionRank(vector<double> arrPtr, vector<double> *index, int low, int high)
{
	int pivot = arrPtr[(*index)[high]]; // pivot 
	int i = (low - 1); // Index of smaller element 

	for (int j = low; j <= high - 1; j++)
	{
		// If current element is smaller than or 
		// equal to pivot 
		if (arrPtr[(*index)[j]] <= pivot)
		{
			i++; // increment index of smaller element 
			swapVec(index, i, j);
		}
	}
	swapVec(index, i + 1, high);
	return (i + 1);
}

/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low --> Starting index,
high --> Ending index */
void quickRank(vector<double> arrPtr, vector<double> *index, int low, int high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now at right place */
		int pi = partitionRank(arrPtr, index, low, high);

		// Separately sort elements before 
		// partition and after partition 
		quickRank(arrPtr, index, low, pi - 1);
		quickRank(arrPtr, index, pi + 1, high);
	}
}


int NumericalRank_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	int size = input[0].value.size();
	output[0].value.resize(size);

	for (int j = 0; j < size; j++)
		output[0].value[j] = j;

	quickRank(input[0].value, &output[0].value, 0, size - 1);

	return 1;
}
NumericalRank_Component::NumericalRank_Component()
	: Component(L"NRank", L"Numerical Rank", 1, 1)
{
}


// ************************ Matrix Interpolate *****************************



double interpolate2D(vector<double>matrix, int xWidth, int yHeight, double indexX, double indexY) {

	if (indexX < 0)
		return 0;
	if (indexX > xWidth - 1)
		return 0;

	if (indexY < 0)
		return 0;
	if (indexY > yHeight - 1)
		return 0;

	int lowX = floor(indexX);
	int highX = lowX + 1;
	if (highX > xWidth - 1) {
		highX = xWidth - 1;
		lowX = highX - 1;
	}

	int lowY = floor(indexY);
	int highY = lowY + 1;
	
	if (highY > yHeight - 1) {
		highY = yHeight - 1;
		lowY = highY - 1;
	}
	
	int topLeft = lowY * xWidth + lowX;
	int topRight = lowY * xWidth + highX;
	int botLeft = highY * xWidth + lowX;
	int botRight = highY * xWidth + highX;
	
	double left =
			(indexY - lowY) * matrix[botLeft] +
			(highY - indexY) * matrix[topLeft];
		
	double right =
			(indexY - lowY) * matrix[botRight] +
			(highY - indexY) * matrix[topRight];

	return (indexX - lowX) * right +
		(highX - indexX) * left;
}

int MatrixInterpolate_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	if (input[0].dimensions.size() != 2)
		return clearOutputs(); // array is not 2 dimensions

	int indexXSize = input[1].value.size();
	int indexYSize = input[2].value.size();
	if (indexXSize != indexYSize)
		return clearOutputs();; // indexes must be the same size

	int xWidth = input[0].dimensions[0];
	int yHeight = input[0].dimensions[1];
	if(( xWidth < 2) || (yHeight < 2) )
		return clearOutputs();

	output[0].value.clear();

	for (int j = 0; j < indexXSize; j++) {
		double indexX = input[1].value[j];
		double indexY = input[2].value[j];

		output[0].value.push_back( interpolate2D(input[0].value, xWidth, yHeight, indexX, indexY));
	}
	copyDimensions( 1, 0);
	return 1;
}
MatrixInterpolate_Component::MatrixInterpolate_Component()
	: Component(L"MatInterp", L"Matrix Interpolate", 3, 1)
{
	setLeftLabels(3, "Array", "X Index", "Y Index");
}

//************************** Index To Position **********************************

int IndexToPosition_Component::method2(Sheet* pg) {

	int entries = input[0].value.size();
	if (entries == 0)
		return clearOutputs();
	if( input[1].value.size() == 0)
		return clearOutputs();
	int sizeInput = floor(input[1].value[0]);
	if( sizeInput < 0)
		return clearOutputs();
	if( sizeInput > 10000)
		return clearOutputs();
	output[0].value.resize(sizeInput);
	for (int j = 0; j < sizeInput; j++)
		output[0].value[j] = 0;

	for (int k = 0; k < entries; k++) {
		int index = floor(input[0].value[k]);
		if (index < 0)
			continue;
		if (index >= sizeInput)
			continue;
		output[0].value[index] = 1;
	}
	output[0].dimensions[0] = sizeInput;
	return 1;
}
IndexToPosition_Component::IndexToPosition_Component()
	: Component(L"IToPos", L"Index to Position", 2, 1)
{
	setSetting(0, 16);
	//dialogList = dialogNToBitButtonList;
	setLeftLabels(2, "Index", "Array Size");
}

// ************************* Eliminate Value ************************************

int EliminateValue_Component::method2(Sheet* pg) {

	output[0].value.clear();
	setSimpleOutputDimensions();

	if (input[0].drivenByComponent == -1)    // array
		return true;
	int arraySize = input[0].value.size();

	if (input[1].drivenByComponent == -1)    // value
		return true;
	int valueSize = input[1].value.size();

	output[0].value.clear();
	output[1].value.clear();

	for (int j = 0; j < arraySize; j++) {
		bool found = false;
		double theInput = input[0].value[j];

		for (int k = 0; k < valueSize; k++) {
			if (theInput == input[1].value[k]) {
				found = true;
				break;
			}
		}
		if (!found)
			output[0].value.push_back(theInput);
		else {
			// check for repeats
			bool repeat = false;
			for (int n = 0; n < output[1].value.size(); n++)
				if (output[1].value[n] == theInput) {
					repeat = true;
					break;
				}
			if (!repeat)
				output[1].value.push_back(theInput);
		}
	}
	setSimpleOutputDimensions();
	return 1;
}

EliminateValue_Component::EliminateValue_Component()
	: Component(L"ElimValue", L"Eliminate Value", squareShape, 2, 1, 2, 1)
{
	setLeftLabels(3, "Array", "Value");
	setRightLabels(3, "Array", "Value");
	width = 40;
}

// **************** Replace **********************************


int Replace_Component::method2(Sheet* pg) {

	output[0].value.clear();
	setSimpleOutputDimensions();

	output[0].value.clear(); // array
	output[0].stringValue.clear(); // array
	output[1].value.clear(); // found
	output[1].stringValue.clear(); // found
	output[2].value.clear(); // replaced
	output[2].stringValue.clear(); // replaced

	if (input[0].drivenByComponent == -1)    // array
		return true;

	if (input[1].drivenByComponent == -1)    // find
		return true;

	bool isString = input[0].stringType;
	if (input[0].stringType != isString)
		return true;

	int replaceSize = 0;
	if (input[2].drivenByComponent != -1) {    // replace?
		if (input[2].stringType != isString)
			return true;
		if( isString )
			replaceSize = input[2].stringValue.size();
		else
			replaceSize = input[2].value.size();
	}

	int arraySize;
	int findSize;
	if (isString) {
		arraySize = input[0].stringValue.size();
		findSize = input[1].stringValue.size();
	}
	else {
		arraySize = input[0].value.size();
		findSize = input[1].value.size();
	}

	for (int k = 0; k < 3; k++)
		output[k].stringType = isString;

	if (isString) {
		for (int j = 0; j < arraySize; j++) {
			bool found = false;
			string theInput = input[0].stringValue[j];

			for (int k = 0; k < findSize; k++) {
				if (theInput == input[1].stringValue[k]) {
					found = true;
					output[1].stringValue.push_back(theInput); // found
					if (replaceSize == 1) {
						output[0].stringValue.push_back(input[2].stringValue[0]); // output
						output[2].stringValue.push_back(input[2].stringValue[0]); // replaced
					}
					else if ((replaceSize > 1) && (k < replaceSize)) {
						output[0].stringValue.push_back(input[2].stringValue[k]); // output
						output[2].stringValue.push_back(input[2].stringValue[k]); // replaced
					}
					break;
				}
			}
			if (!found)
				output[0].stringValue.push_back(theInput);
		}
	}
	else {
		for (int j = 0; j < arraySize; j++) {
			bool found = false;
			double theInput = input[0].value[j];

			for (int k = 0; k < findSize; k++) {
				if (theInput == input[1].value[k]) {
					found = true;
					output[1].value.push_back(theInput); // found
					if (replaceSize == 1) {
						output[0].value.push_back(input[2].value[0]); // output
						output[2].value.push_back(input[2].value[0]); // replaced
					}
					else if ((replaceSize > 1) && (k < replaceSize)) {
						output[0].value.push_back(input[2].value[k]); // output
						output[2].value.push_back(input[2].value[k]); // replaced
					}
					break;
				}
			}
			if (!found)
				output[0].value.push_back(theInput);
		}
	}
	setSimpleOutputDimensions();
	return 1;
}
Replace_Component::Replace_Component()
	: Component(L"Replace", L"Replace", squareShape, 3, 1, 3, 1)
{
	setLeftLabels(3, "Array", "Find", "Replace");
	setRightLabels(3, "Array", "Found", "Replaced");
	width = 50;
}




// ************************* Read Text File ************************************

int ReadTextFile_Component::method2(Sheet* pg) {

	output[1].value[0] = 0; // not done
	if (input[2].value.size() == 0)
		return clearOutputs();
	if (input[2].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	const int size = 500;
	char buffer[size];
	output[0].stringValue.clear();
	output[0].stringType = true;
	std::ifstream cin(fullName, ios::in);
	if (cin) {
	
		while (!cin.eof()) {
			cin.getline(buffer, size);
			output[0].stringValue.push_back( buffer);
		}
		cin.close();
		output[0].dimensions[0] = output[0].stringValue.size();
		output[1].value[0] = 1; // done
	}
	return 1;
}

ReadTextFile_Component::ReadTextFile_Component()
	: Component(L"ReadTxFile", L"Read Text File", squareShape, 3, 1, 2, 1)
{
	setLeftLabels(3, "Name", "Path", "Trigger");
	setRightLabels(3, "Array", "Done");
	setMemory(0, 0); // for trigger
	width = 40;
}


// ************************* Read Data File ************************************

int ReadDataFile_Component::method2(Sheet* pg) {

	output[1].value[0] = 0; // not done
	if (input[2].value.size() == 0)
		return clearOutputs();

	if (input[2].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;
	
	output[0].value.clear();
	std::ifstream cin(fullName, ios::binary);
	if (cin) {

		switch ((int)setting[0]) {
		case 0: //Write byte
			while (!cin.eof()) {
				long tempChar = 0;
				cin.read(reinterpret_cast<char*>(&tempChar), sizeof(char));
				if (cin)
					output[0].value.push_back(tempChar);
			}
			break;
		case 1: //Write word
			while (!cin.eof()) {
				long tempShort = 0;
				cin.read(reinterpret_cast<char*>(&tempShort), sizeof(short));
				if (cin)
					output[0].value.push_back(tempShort);
			}
			break;
		case 2: //Write long
			while (!cin.eof()) {
				long tempLong = 0;
				cin.read(reinterpret_cast<char*>(&tempLong), sizeof(long));
				if (cin)
					output[0].value.push_back(tempLong);
			}
			break;
		case 3: //Read double
			while (!cin.eof()) {
				double temp = 0;
				cin.read(reinterpret_cast<char*>(&temp), sizeof(double));
				if (cin)
					output[0].value.push_back(temp);
			}
			break;
		}
		cin.close();
		output[0].dimensions[0] = output[0].value.size();
		output[1].value[0] = 1; // done
	}
	return 1;
}
void ReadDataFile_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = radioValue;

}
void buttonReadDataFile_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Read byte", "Read word", "Read long", "Read double" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*> readDataFileButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonReadDataFile_SetOptions_Action) };

ReadDataFile_Component::ReadDataFile_Component()
	: Component(L"ReadDatFile", L"Read Data File", squareShape, 3, 1, 2, 1)
{
	dialogButtonList = readDataFileButtonList;
	setLeftLabels(3, "Name", "Path", "Trigger");
	setRightLabels(3, "Array", "Done");
	setMemory(0, 0); // for trigger
	setSetting(0, 3); // mode
	width = 40;
	completionTestRadios = 4;
}


// ************************* Write Text File ************************************

int WriteTextFile_Component::method2(Sheet* pg) {
	
	// keep it simple for now - one file, multiple array values
	output[0].value[0] = 0; // not done
	if (input[3].value.size() == 0)
		return clearOutputs();

	if (input[3].value[0]){
		if(memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	//  open the file
	int mode = ios::out;
	if ((int)setting[0] & 1)
		mode += ios::app;

	wofstream cout(fullName, mode);
	if (cout) {

		bool stringType = input[2].stringType;
		if (stringType) {
			int size = input[2].stringValue.size();
			for (int j = 0; j < size; j++) {

				if ((int)setting[0] & 2)
					cout << decodeUTF8(input[2].stringValue[j]).c_str();
				else
					cout << input[2].stringValue[j].c_str();
				cout << "\n";
			}
		}
		else {
			int size = input[2].value.size();
			for (int j = 0; j < size; j++) {	
				cout << input[2].value[j];
				cout << "\n";
			}
		}
		cout.close();
		output[0].value[0] = 1; // done
	}
	return 1;
}
void WriteTextFile_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = (boolValues[0] ? 1 : 0) + (boolValues[1] ? 2 : 0);

}
void buttonWriteTextFile_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels = { "Append to existing file", "Convert Symbols: (&nnnn;)" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[0] & 1), (bool)((int)pg->dgrm.cL[i]->setting[0] & 2) };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*> writeTextFileButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonWriteTextFile_SetOptions_Action) };

WriteTextFile_Component::WriteTextFile_Component()
	: Component(L"WriteTxFile", L"Write Text File", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = writeTextFileButtonList;
	setLeftLabels(3, "Name", "Path", "Array", "Trigger");
	setRightLabels(3, "Done");
	setMemory(0, 0); // for trigger
	setSetting(0, 0); // bit 0 = append, bit 1 = convert symbols
	width = 40;
	completionTestChecks = 2;
}

// ************************* Write Data File ************************************

int WriteDataFile_Component::method2(Sheet* pg) {

	// keep it simple for now - one file, multiple array values
	output[0].value[0] = 0; // not done
	if (input[3].value.size() == 0)
		return clearOutputs();

	if (input[3].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	//  open the file
	int mode = ios::binary;
	if (setting[0])
		mode += ios::app;

	ofstream cout(fullName, mode);
	if (cout) {
		
		bool stringType = input[2].stringType;
		if (stringType) {
			int size = input[2].stringValue.size();
			for (int j = 0; j < size; j++) {
				int stringLen = input[2].stringValue[j].size();
				cout.write(input[2].stringValue[j].c_str(), stringLen);
			}
		}
		else {
			int size = input[2].value.size();
			switch ((int)setting[1]) {
			case 0: //Write byte
				for (int j = 0; j < size; j++) {
					char tempChar = input[2].value[j];
					cout.write(reinterpret_cast<char*>(&tempChar), sizeof(char));
				}
				break;
			case 1: //Write word
				for (int j = 0; j < size; j++) {
					short tempShort = input[2].value[j];
					cout.write(reinterpret_cast<char*>(&tempShort), sizeof(short));
				}
				break;
			case 2: //Write long
				for (int j = 0; j < size; j++) {
					long tempLong = input[2].value[j];
					cout.write(reinterpret_cast<char*>(&tempLong), sizeof(long));
				}
				break;
			case 3: //Write double
				for (int j = 0; j < size; j++) {
					cout.write(reinterpret_cast<char*>(&input[2].value[j]), sizeof(double));
				}
				break;
			}
		}
		cout.close();
		output[0].value[0] = 1; // done
	}
	return 1;
}

void WriteDataFile_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	
	setting[0] = boolValues[0];
	setting[1] = radioValue;
	
}
void buttonWriteDataFile_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels = { "Append to existing file" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->setting[0] };

	vector<string> radioLabels = { "Write byte", "Write word", "Write long", "Write double" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*> writeDataFileButtonList = {
	new Button(3, 40, 85, 20, L"Options...", buttonWriteDataFile_SetOptions_Action)};

WriteDataFile_Component::WriteDataFile_Component()
	: Component(L"WriteDatFile", L"Write Data File", squareShape, 4, 1, 1, 1)
{
	dialogButtonList = writeDataFileButtonList;
	setLeftLabels(3, "Name", "Path", "Array", "Trigger");
	setRightLabels(3, "Done");
	setMemory(0, 0); // for trigger
	setSetting(0, 0); // don't append;
	setSetting(1, 3); // doubles;
	width = 40;
	completionTestChecks = 1;
	completionTestRadios = 4;
}


// ************************* Get File Info ************************************

string ExpandSystemTime(SYSTEMTIME systemTime) {

	string result;
	result =  tomString(systemTime.wMonth) + "/";
	result += tomString(systemTime.wDay) + "/";
	result += tomString(systemTime.wYear) + "  ";
	if (systemTime.wHour < 10)
		result += "0";
	result += tomString(systemTime.wHour) + ":";
	if (systemTime.wMinute < 10)
		result += "0";
	result += tomString(systemTime.wMinute) + ":";
	if (systemTime.wSecond < 10)
		result += "0";
	result += tomString(systemTime.wSecond);
			  
	return result;
}


int GetFileInfo_Component::method2(Sheet* pg) {

	WIN32_FILE_ATTRIBUTE_DATA fInfo;

	int loops = inputsToLoopsN(2); // loop on name and path only
	if (loops == 0)
		return clearOutputs();
	if (input[2].value.size() == 0)
		return clearOutputs();

	if (input[2].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	// initialize output sizes and types
	output[0].value.resize(loops);
	output[1].stringValue.resize(loops);
	output[1].stringType = true;
	output[2].stringValue.resize(loops);
	output[2].stringType = true;
	output[3].stringValue.resize(loops);
	output[3].stringType = true;
	output[4].value.resize(loops); 
	output[5].value.resize(loops);

	for (int j = 0; j < loops; j++) {

		// clear outputs
		output[0].value[j] = 0;
		output[1].stringValue[j] = "";
		output[2].stringValue[j] = "";
		output[3].stringValue[j] = "";
		output[4].value[j] = 0;
		output[5].value[j] = 0; // if success

		string name = getLoopingStringValueD(0, j, "noname");
		string path = getLoopingStringValueD(1, j, "");

		string fullName = path;
		if (path.size())
			fullName += "\\" + name;
		else
			fullName = ".\\" + name;

		int result = GetFileAttributesEx(ConvertToLPWSTR(fullName), GetFileExInfoStandard, &fInfo);

		output[0].value[0] = fInfo.dwFileAttributes;

		SYSTEMTIME SystemTime;
		FileTimeToSystemTime(&fInfo.ftCreationTime, &SystemTime);
		output[1].stringValue[j] = ExpandSystemTime(SystemTime);

		FileTimeToSystemTime(&fInfo.ftLastAccessTime, &SystemTime);
		output[2].stringValue[j] = ExpandSystemTime(SystemTime);

		FileTimeToSystemTime(&fInfo.ftLastWriteTime, &SystemTime);
		output[3].stringValue[j] = ExpandSystemTime(SystemTime);

		output[4].value[j] = (LONG64)fInfo.nFileSizeHigh << 32;
		output[4].value[j] += (LONG64)fInfo.nFileSizeLow;

		output[5].value[j] = result; // if success
	}

	return 1;
}

GetFileInfo_Component::GetFileInfo_Component()
	: Component(L"GetFileInfo", L"Get File Info", squareShape, 3, 1, 6, 1)
{
	setLeftLabels(3, "Name", "Path", "Trigger");
	setRightLabels(3, "Attributes", "Created", "Accessed", "Written", "Size", "Done" );
	setMemory(0, 0); // for trigger
	width = 50;
}


// ************************* Delete File ************************************

int DeleteFile_Component::method2(Sheet* pg) {

	// because this component can delete files, it has a safety feature that can't be stored
	// the user must enable it each time the component is loaded
	if (!setting[0])
		return 0;

	// keep it simple for now - one file
	output[0].value[0] = 0; // not done
	if (input[2].value.size() == 0)
		return clearOutputs();

	if (input[2].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	if( remove(fullName.c_str()) == 0 )
		output[0].value[0] = 1; //  done
	else
		output[0].value[0] = 0; //  fail
	return 1;
}
void DeleteFile_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	setting[0] = boolValues[0]; // safety
}

void doubleClickDelFile(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> checkLabels{ "Enable Deleting Files" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->setting[0] };

	buildMultipleValuesDialog(pg, i, {}, {}, 80, checkLabels, checkValues, {}, 0, NULL);
}

std::vector<Button*>delFileButtonList = {
	new Button(3, 40, 85, 20, L"Options...", doubleClickDelFile) };

bool DeleteFile_Component::onLoad(Sheet* pg) {
	setting[0] = 0; // disable for safety
	return 0;
}
DeleteFile_Component::DeleteFile_Component()
	: Component(L"DelFile", L"Delete File", 3, 1)
{
	setSetting(0, 0);
	dialogButtonList = delFileButtonList;
	setLeftLabels(3, "Name", "Path", "Trigger");
	setMemory(0, 0); // for trigger
	setRightLabels(3, "Done");
	width = 40;
}


// ************************* Get Directory ************************************

int GetDir_Component::method2(Sheet* pg) {

	output[1].value[0] = 0; // not done
	if (input[1].value.size() == 0)
		return clearOutputs();

	if (input[1].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string path = getLoopingStringValueD(0, 0, "");

	string fullName = path;
	if (path.size())
		fullName = path;
	else
		fullName = ".";

	vector<string> names;
	string tempName;
	string search_path = fullName + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(ConvertToLPWSTR(search_path), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			
			switch ((int)setting[0]) {
			case 0: // List files only
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					continue;
				break;
			case 1: // List folders only
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					continue;
				break;
			}
			int strlength = _tcslen(fd.cFileName);

			if (strlength == 1) 
				if(fd.cFileName[0] == '.')
					continue;
			if (strlength == 2)
				if (fd.cFileName[0] == '.')
					if (fd.cFileName[1] == '.')
						continue;
			
			tempName.resize(strlength + 1);
			for (int k = 0; k < strlength; k++)
				tempName[k] = fd.cFileName[k];
			tempName[strlength] = 0;
			names.push_back(tempName);
			
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}

	int len = names.size();
	output[0].stringValue.resize(len);
	output[0].stringType = true;
	for (int j = 0; j < len; j++)
		output[0].stringValue[j] = names[j];

	output[1].value[0] = 1;

	return 1;
}
void GetDir_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = radioValue;

}
void buttonGetDirectory_SetOptions_Action(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "List files only", "List folders only", "List files and folders" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*> getDirectoryButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonGetDirectory_SetOptions_Action) };

GetDir_Component::GetDir_Component()
	: Component(L"GetDir", L"Get Directory", squareShape, 2, 1, 2, 1)
{
	dialogButtonList = getDirectoryButtonList;
	setLeftLabels(3, "Path", "Trigger");
	setRightLabels(3, "Array", "Done");
	setMemory(0, 0); // for trigger
	setSetting(0, 0); // list files only
	width = 40;
	completionTestRadios = 3;
}


// ************************* Create Folder ************************************


int CreateFldr_Component::method2(Sheet* pg) {

	output[0].value[0] = 0; // not done
	if (input[2].value.size() == 0)
		return clearOutputs();

	if (input[2].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	if (name.size() == 0)
		return 0;

	string path = getLoopingStringValueD(1, 0, "");

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	if( _mkdir(fullName.c_str()) == 0 )
		output[0].value[0] = 1; //  done

	return 1;
}

CreateFldr_Component::CreateFldr_Component()
	: Component(L"CreateFldr", L"Create Folder", 3, 1)
{
	setLeftLabels(3, "Name", "Path", "Trigger");
	setRightLabels(3, "Done");
	setMemory(0, 0); // for trigger
	width = 40;
}


// ************************* Random Read File ************************************

int RandReadFile_Component::method2(Sheet* pg) {

	output[1].value[0] = 0; // not done
	if (input[4].value.size() == 0)
		return clearOutputs();

	if (input[4].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");
	int position = getLoopingValueD(2, 0, 0);
	int count = getLoopingValueD(3, 0, 1);

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	output[0].value.clear();
	std::ifstream cin(fullName, ios::binary);
	if (cin) {
		switch ((int)setting[0]) {
		case 0: //Write byte
			if (position)
				cin.seekg(sizeof(char)*position);
			for( int k = 0; k < count; k++ ) {
				long tempChar = 0;
				cin.read(reinterpret_cast<char*>(&tempChar), sizeof(char));
				if (cin)
					output[0].value.push_back(tempChar);
			}
			break;
		case 1: //Write word
			if (position)
				cin.seekg(sizeof(short) *position);
			for (int k = 0; k < count; k++) {
				long tempShort = 0;
				cin.read(reinterpret_cast<char*>(&tempShort), sizeof(short));
				if (cin)
					output[0].value.push_back(tempShort);
			}
			break;
		case 2: //Write long
			if (position)
				cin.seekg(sizeof(long) *position);
			for (int k = 0; k < count; k++) {
				long tempLong = 0;
				cin.read(reinterpret_cast<char*>(&tempLong), sizeof(long));
				if (cin)
					output[0].value.push_back(tempLong);
			}
			break;
		case 3: //Read double
			if (position)
				cin.seekg(sizeof(double)*position);
			for (int k = 0; k < count; k++) {
				double temp = 0;
				cin.read(reinterpret_cast<char*>(&temp), sizeof(double));
				if (cin)
					output[0].value.push_back(temp);
			}
			break;
		}
		cin.close();
		output[0].dimensions[0] = output[0].value.size();
		output[1].value[0] = 1; // done
	}

	return 1;
}
void RandReadFile_Component::completion(Sheet * pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[0] = radioValue;

}
void buttonRandomReadFile_SetOptions_Action(Sheet * pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Read byte", "Read word", "Read long", "Read double" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*> randomReadFileButtonList = {
	new Button(3, 40, 85, 20, L"Options...", buttonRandomReadFile_SetOptions_Action)};

RandReadFile_Component::RandReadFile_Component()
	: Component(L"RandReadFile", L"Random Read File", squareShape, 5, 1, 2, 1)
{
	dialogButtonList = randomReadFileButtonList;
	setLeftLabels(3, "Name", "Path", "Position", "count", "Trigger");
	setRightLabels(3, "Array", "Done");
	setMemory(0, 0); // for trigger
	setSetting(0, 3); // mode
	width = 40;
	completionTestRadios = 4;
}

// ************************* Random Write File ************************************

int RandWriteFile_Component::method2(Sheet* pg) {

	// keep it simple for now - one file, multiple array values
	output[0].value[0] = 0; // not done
	if (input[4].value.size() == 0)
		return clearOutputs();

	if (input[4].value[0]) {
		if (memory[0] == 1)
			return 0;
		memory[0] = 1;
	}
	else {
		memory[0] = 0;
		return 0;
	}

	string name = getLoopingStringValueD(0, 0, "noname");
	string path = getLoopingStringValueD(1, 0, "");
	int position = getLoopingValueD(2, 0, 0);

	string fullName = path;
	if (path.size())
		fullName += "\\" + name;
	else
		fullName = ".\\" + name;

	//  open the file
	ofstream cout(fullName, ios::binary );
	if (cout) {
		bool stringType = input[3].stringType;
		if (stringType) {
			if (position)
				cout.seekp(sizeof(char)*position);
			int size = input[3].stringValue.size();
			for (int j = 0; j < size; j++) {
				int stringLen = input[3].stringValue[j].size();
				cout.write(input[3].stringValue[j].c_str(), stringLen);
			}
		}
		else {
			int size = input[3].value.size();
			switch ((int)setting[1]) {
			case 0: //Write byte
				if (position)
					cout.seekp(sizeof(char) * position);
				for (int j = 0; j < size; j++) {
					char tempChar = input[3].value[j];
					cout.write(reinterpret_cast<char*>(&tempChar), sizeof(char));
				}
				break;
			case 1: //Write word
				if (position)
					cout.seekp(sizeof(short) * position);
				for (int j = 0; j < size; j++) {
					short tempShort = input[3].value[j];
					cout.write(reinterpret_cast<char*>(&tempShort), sizeof(short));
				}
				break;
			case 2: //Write long
				if (position)
					cout.seekp(sizeof(long) * position);
				for (int j = 0; j < size; j++) {
					long tempLong = input[3].value[j];
					cout.write(reinterpret_cast<char*>(&tempLong), sizeof(long));
				}
				break;
			case 3: //Write double
				if (position)
					cout.seekp(sizeof(double) * position);
				for (int j = 0; j < size; j++) {
					cout.write(reinterpret_cast<char*>(&input[3].value[j]), sizeof(double));
				}
				break;
			}
		}
		//cout.seekp(ios_base::end);
		cout.close();
		output[0].value[0] = 1; // done
	}
	return 1;
}

void RandWriteFile_Component ::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	setting[1] = radioValue;

}
void buttonRandomWriteFile_SetOptions_Action(Sheet * pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Write byte", "Write word", "Write long", "Write double" };
	int radioIndex = pg->dgrm.cL[i]->setting[1];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		NULL);
}

std::vector<Button*> randomWriteFileButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", buttonRandomWriteFile_SetOptions_Action) };

RandWriteFile_Component::RandWriteFile_Component()
	: Component(L"RandWriteFile", L"Random Write File", squareShape, 5, 1, 1, 1)
{
	dialogButtonList = randomWriteFileButtonList;
	setLeftLabels(3, "Name", "Path", "Position", "Array", "Trigger");
	setRightLabels(3, "Done");
	setMemory(0, 0); // for trigger
	setSetting(0, 0); // don't append is not used here;
	setSetting(1, 3); // doubles;
	width = 40;
	completionTestRadios = 4;
}
