#include "stdafx.h"
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
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
#include "MST01.h"

#pragma warning (disable: 4267)

extern vector< InnerSheet*> isL;
extern Sheet* page;

extern void Open3DViewPlus(int posX, int posY, int width, int height);
extern void Open2DViewPlus(int posX, int posY, int width, int height);
extern Button* reopenWindows;

bool grabTag(int *startPtr, char *string, char *tag ) {
	int st = -1;
	int len = 0;

	for (int j = *startPtr; j < *startPtr + 20; j++)
		if (string[j] == '<') {
			st = j + 1;
			break;
		}
	if (st == -1) {
		//grabDone = true;
		return true;// { res: '', st : start, done : true };
	}
	for (int j = st; j < st + 30; j++)
		if (string[j] == '>') {
			len = j - st;
			*startPtr = j + 1;
			break;
		}

	if (len == 0) {
		//grabDone = true;
		return true;// { res: '', st : start, done : true };
	}

	for (int j = 0; j < len; j++)
		tag[j] = string[j + st];
	tag[len] = 0;
	return false;// { res: string.substr(st, len), st : start, done : false };
}


bool grabString(int *startPtr, char *string, char *result) {
	int st = *startPtr;
	int len = 0;
	int max = strlen(string);

	for (int j = st; j < max; j++)
		if (string[j] == '<') {
			len = j - st;
			*startPtr = j + 1;
			break;
		}

	if (len == 0) 
		return true;

	for (int j = 0; j < len; j++)
		result[j] = string[j + st];
	result[len] = 0;
	
	return false; // { res: string.substr(st, len), st : start, done : false };// not sure about done here
}

bool grabInteger(int *startPtr, char *string, int *valuePtr) {
	
	char text[20];
	bool done = grabString(startPtr, string, text);
	*valuePtr = atoi(text);
	if (done)
		return true;
	return false;
}


bool grabVectordoubles(int *startPtr, char *string, std::vector<double> *result) {
	int st = *startPtr;
	int max = strlen(string);
	char buffer[40];

	result->clear();
	int k = 0;
	for (int j = st; j < max; j++) {

		if (string[j] == ' ') { // every double ends in a space, even the last one
			buffer[k] = 0;
			result->push_back(atof(buffer));
			k = 0;
		}
		else
		if (string[j] == '<') { // the end is </s> or similar
			*startPtr = j + 1;
			return false;
		}

		buffer[k++] = string[j];
	}
	return true; 
}

bool grabVectorStrings(int *startPtr, char *string, std::vector<std::string> *result) {
	int st = *startPtr;
	int max = strlen(string);
	char buffer[200];

	result->clear();
	int k = 0;
	bool inLiteral = true;
	for (int j = st; j < max; j++) {

		// check for an end of this literal 
		if (inLiteral && (string[j] == '<') && (string[j + 1] == '/') && (string[j + 2] == 'l') && (string[j + 3] == '>')) {
			// end of this literal
			buffer[k] = 0;
			std::string temp(buffer, k);
			result->push_back(temp);
			j += 4; // skip the end sequence
			k = 0;
			inLiteral = false;
			continue;
		}
		
		if (!inLiteral && (string[j] == '<') && (string[j + 1] == 'l') && (string[j + 2] == '>')) { // start another literal
			k = 0;
			j += 3; // skip the start sequence
			inLiteral = true;
			buffer[k++] = string[j];
			continue;
		}
		
		if( !inLiteral && (string[j] == '<')) { // we are done here
			*startPtr = j;
			return false;
		}
				
		if( inLiteral )
			buffer[k++] = string[j];
	}
	return true;
}

bool grabVectorIntegers(int *startPtr, char *string, std::vector<int> *result) {
	int st = *startPtr;
	int max = strlen(string);
	char buffer[20];

	result->clear();
	int k = 0;
	for (int j = st; j < max; j++) {

		if (string[j] == ' ') { // every double ends in a space, even the last one
			buffer[k] = 0;
			result->push_back(atoi(buffer));
			k = 0;
		}
		else
			if (string[j] == '<') { // the end is </s> or similar
				*startPtr = j + 1;
				return false;
			}

		buffer[k++] = string[j];
	}
	return true;
}

void convertTextToWide(char *text, wchar_t *output) {
	int len = strlen(text);
	for (int j = 0; j < len; j++)
		output[j] = text[j];
	output[len] = 0;
}

void copyDiagram(Diagram* srcDgrm, Diagram* dstDgrm) {
	int count = srcDgrm->cL.size();
	dstDgrm->cL.resize(count);
	for (int i = 0; i < count; i++)
		dstDgrm->cL[i] = srcDgrm->cL[i];

	count = srcDgrm->wL.size();
	dstDgrm->wL.resize(count);
	for (int i = 0; i < count; i++) {
		dstDgrm->wL[i] = srcDgrm->wL[i];
	}
}

void deepCopyDiagram(Diagram* srcDgrm, Diagram* dstDgrm) {
	
	
	int dstCompCount = dstDgrm->cL.size();
	for (int i = 0; i < dstCompCount; i++) {
		//startMemCheck();
		delete dstDgrm->cL[i];
		//endMemCheck();
	}
	dstDgrm->cL.clear();
	

	int dstWireCount = dstDgrm->wL.size();
	for (int i = 0; i < dstWireCount; i++)
		delete dstDgrm->wL[i];
	dstDgrm->wL.clear();
	
	int srcCompCount = srcDgrm->cL.size();
	for (int i = 0; i < srcCompCount; i++) {
		//startMemCheck();
		Component* temp = buildComponent(page, srcDgrm->cL[i]->type, 0);
		//endMemCheck();
		
		temp->x = srcDgrm->cL[i]->x;
		temp->y = srcDgrm->cL[i]->y;
		temp->width = srcDgrm->cL[i]->width;
		temp->height = srcDgrm->cL[i]->height;

		temp->ghost = srcDgrm->cL[i]->ghost;
		temp->userLabel = srcDgrm->cL[i]->userLabel;
		temp->selected = srcDgrm->cL[i]->selected;
		temp->displayMode = srcDgrm->cL[i]->displayMode;
		temp->timeBased = srcDgrm->cL[i]->timeBased;
		temp->fillColor = srcDgrm->cL[i]->fillColor;

		temp->displayMode = srcDgrm->cL[i]->displayMode;
		temp->pinMaskIndex = srcDgrm->cL[i]->pinMaskIndex;
		temp->rotation = srcDgrm->cL[i]->rotation;
		temp->dialogButtonList = srcDgrm->cL[i]->dialogButtonList;
		
		//startMemCheck();
		temp->input.clear();
		temp->buildInputPins(srcDgrm->cL[i]->input.size());
		for (int j = 0; j < temp->leftTerminalCount; j++) {
			temp->input[j].label      = srcDgrm->cL[i]->input[j].label;
			temp->input[j].stringType = srcDgrm->cL[i]->input[j].stringType;

			int size = srcDgrm->cL[i]->input[j].value.size();
			temp->input[j].value.resize(size);
			for (int k = 0; k < size; k++)
				temp->input[j].value[k] = srcDgrm->cL[i]->input[j].value[k];

			size = srcDgrm->cL[i]->input[j].stringValue.size();
			temp->input[j].stringValue.resize(size);
			for (int k = 0; k < size; k++)
				temp->input[j].stringValue[k] = srcDgrm->cL[i]->input[j].stringValue[k];

			size = srcDgrm->cL[i]->input[j].dimensions.size();
			temp->input[j].dimensions.resize(size);
			for (int k = 0; k < size; k++)
				temp->input[j].dimensions[k] = srcDgrm->cL[i]->input[j].dimensions[k];
		}
		//endMemCheck();

		//startMemCheck();
		temp->output.clear();
		temp->buildOutputPins(srcDgrm->cL[i]->output.size());
		for (int j = 0; j < temp->rightTerminalCount; j++) {
			temp->output[j].label      = srcDgrm->cL[i]->output[j].label;
			temp->output[j].stringType = srcDgrm->cL[i]->output[j].stringType;

			int size = srcDgrm->cL[i]->output[j].value.size();
			temp->output[j].value.resize(size);
			for( int k = 0; k < size; k++)
				temp->output[j].value[k] = srcDgrm->cL[i]->output[j].value[k];

			size = srcDgrm->cL[i]->output[j].stringValue.size();
			temp->output[j].stringValue.resize(size);
			for (int k = 0; k < size; k++)
				temp->output[j].stringValue[k] = srcDgrm->cL[i]->output[j].stringValue[k];

			size = srcDgrm->cL[i]->output[j].dimensions.size();
			temp->output[j].dimensions.resize(size);
			for (int k = 0; k < size; k++)
				temp->output[j].dimensions[k] = srcDgrm->cL[i]->output[j].dimensions[k];
		}
		//endMemCheck();
		
		temp->setting.clear();
		for (int k = 0; k < srcDgrm->cL[i]->setting.size(); k++) 
			temp->setting.push_back(srcDgrm->cL[i]->setting[k]);

		temp->memory.clear();
		for (int k = 0; k < srcDgrm->cL[i]->memory.size(); k++)
			temp->memory.push_back(srcDgrm->cL[i]->memory[k]);

		temp->element.clear();
		for (int k = 0; k < srcDgrm->cL[i]->element.size(); k++)
			temp->element.push_back(srcDgrm->cL[i]->element[k]);

		temp->literal.clear();
		for (int k = 0; k < srcDgrm->cL[i]->literal.size(); k++)
			temp->literal.push_back(srcDgrm->cL[i]->literal[k]);
	
		dstDgrm->cL.push_back(temp);
	}

	int srcWireCount = srcDgrm->wL.size();
	for (int i = 0; i < srcWireCount; i++) {
		Wire* temp = new Wire(
			srcDgrm->wL[i]->x[0], srcDgrm->wL[i]->y[0],
			srcDgrm->wL[i]->x[1], srcDgrm->wL[i]->y[1]);
		temp->selected = srcDgrm->wL[i]->selected;

		for( int k = 2; k < srcDgrm->wL[i]->x.size(); k++){
			temp->x.push_back(srcDgrm->wL[i]->x[k]);
			temp->y.push_back(srcDgrm->wL[i]->y[k]);
			temp->pointSelected.push_back(srcDgrm->wL[i]->pointSelected[k]);
		}
		dstDgrm->wL.push_back(temp);
	}
}

void syncVisibleSheet(Sheet* pg) {
	int vs = pg->visibleSheet;
	isL[vs]->dgrm = pg->dgrm;
	isL[vs]->cXo = pg->cXo;
	isL[vs]->cYo = pg->cYo;
	isL[vs]->cs = pg->cs;
	isL[vs]->isDirty = pg->isDirty;
}

void switchToSheet(Sheet* pg, int sheetNumber) {
	if (pg->visibleSheet == sheetNumber)
		return;

	// save current setup
	int vs = pg->visibleSheet;
	isL[vs]->dgrm = pg->dgrm;
	isL[vs]->cXo = pg->cXo;
	isL[vs]->cYo = pg->cYo;
	isL[vs]->cs = pg->cs;
	isL[vs]->isDirty = pg->isDirty;

	// create some if needed
	while (sheetNumber >= isL.size()) {
		InnerSheet* inner = new InnerSheet();
		isL.push_back(inner);
	}

	// load the new sheet
	int ns = sheetNumber;
	pg->dgrm = isL[ns]->dgrm;

	// and its setup
	pg->cXo = isL[ns]->cXo;
	pg->cYo = isL[ns]->cYo;
	pg->cs = isL[ns]->cs;
	pg->isDirty = isL[ns]->isDirty;

	pg->visibleSheet = sheetNumber;
}

bool checkIsDirty(Sheet* pg) {

	if( pg->dgrm.cL.size() || pg->dgrm.wL.size())
		if (pg->isDirty)
			return true;

	for (int j = 0; j < isL.size(); j++)
		if( isL[j]->dgrm.cL.size() || isL[j]->dgrm.wL.size())
			if (isL[j]->isDirty)
				return true;

	return false;
}

void clearAllSaveBuffers() {
	page->isDirty = false;

	for (int j = 0; j < isL.size(); j++) {
		isL[j]->isDirty = false;
		isL[j]->saveDiagramBuffer.clear();
		isL[j]->saveIndex = -1;
	}
}


void setVisibleScreenDirty(string reasonForChange) {
	

	int vs = page->visibleSheet;
	int size = isL[vs]->saveDiagramBuffer.size();
	if (isL[vs]->saveIndex == size-1) { // save and add to list
		if (size < 20) {
			Diagram* newDgrm = new Diagram();
			deepCopyDiagram(&page->dgrm, newDgrm);
			isL[vs]->saveDiagramBuffer.push_back(newDgrm);
			isL[vs]->saveIndex = size;
		}
		else {
			
			Diagram* temp = isL[vs]->saveDiagramBuffer[0];
			for (int j = 0; j < size - 1; j++)
				isL[vs]->saveDiagramBuffer[j] = isL[vs]->saveDiagramBuffer[j + 1];
			deepCopyDiagram(&page->dgrm, temp);
			isL[vs]->saveDiagramBuffer[size - 1] = temp;
		}
	}
	else // save and clip list
	{
		deepCopyDiagram(&page->dgrm, isL[vs]->saveDiagramBuffer[++isL[vs]->saveIndex]);

		for (int j = isL[vs]->saveIndex + 1; j < size - 1; j++) {
			for (int k = 0; k < isL[vs]->saveDiagramBuffer[j]->cL.size(); k++)
				delete isL[vs]->saveDiagramBuffer[j]->cL[k];
			for (int k = 0; k < isL[vs]->saveDiagramBuffer[j]->wL.size(); k++)
				delete isL[vs]->saveDiagramBuffer[j]->wL[k];
			delete isL[vs]->saveDiagramBuffer[j];
		}
		isL[vs]->saveDiagramBuffer.resize(isL[vs]->saveIndex+1);
	}

	page->isDirty = true;

	
}
void clearAllDirty() {
	page->isDirty = false;

	for (int j = 0; j < isL.size(); j++) 
		isL[j]->isDirty = false;
}



vector<string> solutionToString(int i) {
	vector<string> result;
	string aString;

	for (int i = 0; i < isL.size(); i++) {
		for (int j = 0; j < isL[i]->dgrm.cL.size(); j++) {
			aString = isL[i]->dgrm.cL[j]->toString();
			result.push_back(aString);
		}
		for (int j = 0; j < isL[i]->dgrm.wL.size(); j++) {
			aString = isL[i]->dgrm.wL[j]->toString();
			result.push_back(aString);
		}
	}
	return result;
}

//void loadSubroutineStringArray(int i) {
//	vector<string> result;
//	string aString;
//
//	for (int i = 0; i < isL.size(); i++) {
//		for (int j = 0; j < isL[i]->dgrm.cL.size(); j++) {
//			aString = isL[i]->dgrm.cL[j]->toString();
//			result.push_back(aString);
//		}
//		for (int j = 0; j < isL[i]->dgrm.wL.size(); j++) {
//			aString = isL[i]->dgrm.wL[j]->toString();
//			result.push_back(aString);
//		}
//	}
//	return aString;
//}

void reloadAllSubroutines() {

	// put back current visible sheet
	syncVisibleSheet(page);

	string theSolution = componentsAndWiresToString(page);
	readComponentsAndWiresFromString(page, theSolution);

	analyzeConnections(&page->dgrm);
	drawCanvas();
}



int debugComponentCounter = 0;
void loadSolution_(Sheet* pg, char *theString, bool select) {

	// may want to move this up to where loadSolution is called by the user directly
	//if (!autoUpdateSheet*Button.isSelected)
	//	debug_hover_depth = 0; // stop debugging Sheet*s
	
	int start = 0;
	bool grabDone = false;
	Component *newComponent = NULL;
	Wire *newWire = NULL;
	char tag[20] = { 0 };

	std::string problemCode;
	std::string problemCodeString;
	int item = 0;// count items for debugging
	char loadType[20] = { 0 };
	char loadDetail[20] = { 0 };
	std::string loadTag = "";
	int modelVersion = 0; // incase there is no version specified
	char text[1200] = { 0 };
	wchar_t widetext[20];
	int visibleSheet = 0; // the main


		//try {
	grabDone = grabTag(&start, theString, tag);
	strcpy_s(loadType, tag);

	if (strcmp(tag, "C") == 0) { //  component details: t=type, x=x_coord, y=y_coord, g=ghost, s=setting
			// l=literal, n=note, u=user_label
		strcpy_s(loadType, "Component");
		do
		{
			grabDone = grabTag(&start, theString, tag);
			loadTag = tag;
			if (strcmp(tag, "t") == 0) { // start a new Component  based on it's type
				grabDone = grabString(&start, theString, text);
				convertTextToWide(text, widetext);
				debugComponentCounter++;
				newComponent = buildComponent(pg, widetext, visibleSheet);
				if (newComponent == NULL) {
					MessageBox(NULL, widetext, L"Can't load component", MB_OK);
					return;
				}
				strcpy_s(loadDetail, text);
				continue;
			}
			if (strcmp(tag, "x") == 0) { // X coordinate
				grabDone = grabInteger(&start, theString, &(newComponent->x));
				continue;
			}
			if (strcmp(tag, "y") == 0) { // Y coordinate
				grabDone = grabInteger(&start, theString, &(newComponent->y));
				continue;
			}
			if (strcmp(tag, "g") == 0) { // ghost
				grabDone = grabInteger(&start, theString, &(newComponent->ghost));
				continue;
			}		
			if (strcmp(tag, "s") == 0) { // setting
				int sda = strlen(theString);
				grabDone = grabVectordoubles(&start, theString, &newComponent->setting);
				continue;
			}			
			if (strcmp(tag, "e") == 0) { // Elements
				grabDone = grabVectordoubles(&start, theString, &newComponent->element);
				continue;
			}				
			if (strcmp(tag, "l") == 0) { // Literal
				grabDone = grabVectorStrings(&start, theString, &newComponent->literal);
				continue;
			}
			if (strcmp(tag, "u") == 0) { // User label
				grabDone = grabString(&start, theString, text);
				newComponent->userLabel = text; // showLTGT(text);
				continue;
			}
			if (strcmp(tag, "d") == 0) { // ghost
				grabDone = grabInteger(&start, theString, &(newComponent->displayMode));
				continue;
			}
			if (strcmp(tag, "r") == 0) { // rotation
				grabDone = grabInteger(&start, theString, &(newComponent->rotation));
				continue;
			}
					
			if (strcmp(tag, "/C") == 0) {  // put the component in the list
				newComponent->fillColor = GetButtonBackgroundColor(pg, newComponent->type);
				newComponent->selected = select;
				int j = pg->dgrm.cL.size();
				pg->dgrm.cL.push_back(newComponent);
				pg->dgrm.cL[j]->onLoad(pg);
				pg->dgrm.cL[j]->method2(pg); // for components that output something when zero is input
				return;
			}
			if (strcmp(tag, "") == 0) {
				return;
			}
		} while (!grabDone);
	}else
	if (strcmp(tag, "W") == 0) { //  wire details: x=x_coords, y=y_coords, g=ghost
		
		strcpy_s(loadType, "Wire"); // start a new one
		newWire = new Wire();
		do {
			grabDone = grabTag(&start, theString, tag);
			loadTag = tag;
			if (strcmp(tag, "x") == 0) { // X coordinate
				grabDone = grabVectorIntegers(&start, theString, &(newWire->x));
			} else
			if (strcmp(tag, "y") == 0) { // Y coordinate
				grabDone = grabVectorIntegers(&start, theString, &(newWire->y));
			}else
			if (strcmp(tag, "g") == 0) { // ghost
				grabDone = grabInteger(&start, theString, &(newWire->ghost));
			} else
			if (strcmp(tag, "/W") == 0) {     // put it in the list
				int len = newWire->x.size();
				newWire->pointSelected.resize(len);
				for (int k = 0; k < len; k++)
					newWire->pointSelected[k] = false;
				newWire->selected = select;
				pg->dgrm.wL.push_back(newWire);

				if (pg->dgrm.wL.size() == 125)
					len = 0;

				return;
			} else
			if (strcmp(tag, "") == 0) {
				return;
			}
		} while(!grabDone);

	}
	else {
		if (strcmp(tag, "setSheet") == 0) { // and build Sheets if needed
			//if (justSheet)
			//	break;
			int sheetNumber;
			grabDone = grabInteger(&start, theString, &sheetNumber);
			switchToSheet(pg, sheetNumber);
		}
		if (strcmp(tag, "sheetName") == 0) {
			grabDone = grabString(&start, theString, text);
			isL[pg->visibleSheet]->name = text;
			// and rebuild the button
			updateSheetButtonText(pg, pg->visibleSheet, text);
		}
		if (strcmp(tag, "version") == 0) { 
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			pg->modelVersion;
		}
		if (strcmp(tag, "magwire") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			magnetWiresButton->isSelected = temp;
		}
		if (strcmp(tag, "magcomp") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			magnetComponentsButton->isSelected = temp;
		}
		if (strcmp(tag, "sticky") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			pg->stickyComponentsButton->isSelected = temp;
		}
		//if (strcmp(tag, "termwire") == 0) {
		//	int temp;
		//	grabDone = grabInteger(&start, theString, &temp);
		//	startWiresFromTerminalsButton->isSelected = temp;
		//}
		if (strcmp(tag, "bend") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			allowWiresToBendButton->isSelected = temp;
		}
		if (strcmp(tag, "kbdsearch") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			keyboardSearch->isSelected = temp;
		}
		if (strcmp(tag, "graphwork") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			showGraphPaperButton->isSelected = temp;
		}
		if (strcmp(tag, "graph2d") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			pg->showGridButton->isSelected = temp;
		}
		if (strcmp(tag, "axis") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			pg->showAxisButton->isSelected = temp;
		}
		if (strcmp(tag, "3dlight") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			enable3dDefaultLightingButton->isSelected = temp;
		}
		if (strcmp(tag, "start") == 0) {
			int temp;
			grabDone = grabInteger(&start, theString, &temp);
			if(temp)
				startClock(pg);
		}
		if (strcmp(tag, "resizeWork") == 0) {
			vector<int> temp;
			grabDone = grabVectorIntegers(&start, theString, &temp);
			if (temp.size() == 4)
				if (reopenWindows->isSelected)
					SetWindowPos(pg->hWnd, NULL, temp[0], temp[1], temp[2], temp[3], 0);
		}
		if (strcmp(tag, "open2d") == 0) {
			vector<int> temp;
			grabDone = grabVectorIntegers(&start, theString, &temp);
			if (temp.size() == 4)
				if( reopenWindows->isSelected)
					Open2DViewPlus(temp[0], temp[1], temp[2], temp[3]);
		}
		if (strcmp(tag, "open3d") == 0) {
			vector<int> temp;
			grabDone = grabVectorIntegers(&start, theString, &temp);
			if (temp.size() == 4)
				if (reopenWindows->isSelected)
					Open3DViewPlus(temp[0], temp[1], temp[2], temp[3]);
		}
		
	}
}
		
/*
		
			case 'reset':
				output = grabBool(start, string);
				start = output.st;
				grabDone = output.done;
				if (output.res) resetClock();
				break;
			case 'cent':
				output = grabBool(start, string);
				start = output.st;
				grabDone = output.done;
				if (output.res) resetCanvas();
				break;
			case 'timestep':
				output = grabValue(start, string);
				start = output.st;
				grabDone = output.done;
				timeStep = parseInt(output.res);
				if (engineOptions.minimumTimeStep && engineOptions.minimumTimeStep > parseInt(output.res)) {
					timeStep = engineOptions.minimumTimeStep;
				}
				break;
			case '2dscale':
				output = grabValue(start, string);
				start = output.st;
				grabDone = output.done;
				canvas2dScale = parsedouble(output.res);
				break;
			case '2dxoff':
				output = grabValue(start, string);
				start = output.st;
				grabDone = output.done;
				canvas2dxOffset = parsedouble(output.res);
				break;
			case '2dyoff':
				output = grabValue(start, string);
				start = output.st;
				grabDone = output.done;
				canvas2dyOffset = parsedouble(output.res);
				break;
			case 'compScale':
				if (pg->cs != 1) {
					ctx.scale(1 / pg->cs, 1 / pg->cs);
					pg->cs = 1;
				}
				output = grabValue(start, string);
				start = output.st;
				grabDone = output.done;
				var zoomFactor = parsedouble(output.res);
				ctx.scale(zoomFactor, zoomFactor);
				pg->cs *= zoomFactor;
				break;
			
			
			case 'internal': // internal settings recursive array
				output = grabValuesAndTypes(start, string);
				var array = output.res;
				start = output.st;
				grabDone = output.done;
				// try setting it now
				//dumpArray( 'loadSolution_', array );
				var res = loadAllInternals(pgL[0], array);
				if (res) {
					console.log("*** loadAllInternals failed: " + res);
					grabDone = true;
				}
				break;
			case '':
				grabDone = true;
				break;
			}
			item++;
			if (item == 162)
				item = 163;
		//}
		//catch (err) {
		//	console.log(err);
		//	problemCode.push(output);
		//	//    problemCodeString = problemCodeString + output.res + ' ' + 'Item = ' + item;
		//	problemCodeString = problemCodeString + ' Item = ' + item + ', ' + loadType + ' = ' + loadDetail + ', tag = ' + loadTag + '; ';
		//	innerDone = true;
		//}
*/
	//	} while (!grabDone);

		
		//
		//propagateValues();
		//drawCanvas();
		//drawCanvas2();
		//
		//if (problemCode.length > 0) {
		//	window.test = problemCode;
		//	alert('Your model was partially loaded. However, we were unable to parse the model definition. Problem strings: ' + problemCodeString);
		//}
	//}
//}



//************************************** read Literal Generic **************************


void readLiteralGeneric(Sheet* pg, int i, int mode) {

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
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						// got the name now open the file
						pg->dgrm.cL[i]->output[0].stringValue.clear();
						pg->dgrm.cL[i]->literal.clear();

						char aChar;
						char buffer[5000];
						int jb = 0;
						bool wasNewline = false;

						std::ifstream cin(pszFilePath, ios::in);
						if (cin) {
						
							while (!cin.eof()) {
								aChar = cin.get();
								if (aChar == -1)
									break;

								if ((mode == 2) && wasNewline && ((aChar == '\r') || (aChar == '\n'))) {
									wasNewline = false;
									continue;
								}

								bool breakIt = false;
								switch (mode) {
								case 0:	
									if (aChar != ',')
										break;
									breakIt = true;
									wasNewline = false; 
									break;
								case 1: 
									if (aChar != '\t')
										break;
									breakIt = true; 
									wasNewline = false;
									break;
								case 2: 
									if ((aChar != '\n') && (aChar != '\r'))
										break;
									breakIt = true; 
									wasNewline = true; 
									break;
								}

								if (breakIt) {
									buffer[jb] = 0;
									pg->dgrm.cL[i]->output[0].stringValue.push_back(buffer);
									pg->dgrm.cL[i]->literal.push_back(buffer);
									jb = 0;
								}
								else
									buffer[jb++] = aChar;
								
								if (jb > 4999)
									break;
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

						int q = 0;
						pg->filePath = SysAllocStringLen(0, 100);
						while (pszFilePath[q] && (q < 95)) {
							pg->filePath[q] = pszFilePath[q];
							q++;
						}
						pg->filePath[q] = 0;

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



//************************************** read Table Generic **************************


void readTableGeneric(Sheet* pg, int i) {

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
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						// got the name now open the file
						pg->dgrm.cL[i]->output[0].value.clear();
						pg->dgrm.cL[i]->setting.resize(2);

						char aChar;
						char buffer[50];
						int jb = 0;
						bool whiteSpace = true;

						std::ifstream cin(pszFilePath, ios::in);
						if (cin) {

							while (!cin.eof()) {
								aChar = cin.get();
								if (aChar == -1)
									break;

								if (whiteSpace && ((aChar == ' ') || (aChar == ',') || (aChar == '\t') || (aChar == '\r') || (aChar == '\n')))
									continue;
								whiteSpace = false;

								if ((aChar == ' ') || (aChar == ',') || (aChar == '\t') || (aChar == '\r') || (aChar == '\n')) {
									buffer[jb] = 0;
									double value = atof(buffer);
									pg->dgrm.cL[i]->output[0].value.push_back(value);
									pg->dgrm.cL[i]->setting.push_back(value);
									whiteSpace = true;
									jb = 0;
								}
								else
									buffer[jb++] = aChar;

								if (jb > 49)
									break;
							}
							if (jb > 0)
							{
								buffer[jb] = 0;
								double value = atof(buffer);
								pg->dgrm.cL[i]->output[0].value.push_back(value);
								pg->dgrm.cL[i]->setting.push_back(value);
							}

							pg->activeDialogComponent = -1;
							propagateValues(&pg->dgrm);
							drawCanvas();
						}

						int q = 0;
						pg->filePath = SysAllocStringLen(0, 100);
						while (pszFilePath[q] && (q < 95)) {
							pg->filePath[q] = pszFilePath[q];
							q++;
						}
						pg->filePath[q] = 0;

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

string setFileEnding(string fileName, string fileEnding) {

	string newFileName;

	int theDotPos = fileName.find_last_of('.');
	if (theDotPos != -1) // remove the old ending
		newFileName = fileName.substr(0, theDotPos);
	else
		newFileName = fileName;
	newFileName += ".";

	// add the new extension
	newFileName += fileEnding;
	return newFileName;
}

bool confirmExtension(string fileName, string fileEnding0, string fileEnding1, string fileEnding2)
{
	int theDotPos = fileName.find_last_of('.');
	if (theDotPos == -1)
		return false;

	if (theDotPos+1 >= fileName.size())
		return false;

	string extension = fileName.substr(theDotPos+1, 999);
	if (extension == fileEnding0)
		return true;
	if (extension == fileEnding1)
		return true;
	if (extension == fileEnding1)
		return true;

	return false;
}

