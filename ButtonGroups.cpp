#include "stdafx.h"

#include "Button.h"
#include "FrameButtons.h"
#include "Sheet.h"
#include "buildComponent.h"
#include "ScreenDialog.h"
#include "color.h"
#include <stdlib.h>
#include <GL/glut.h>
#include "Fileio.h"
#include "Details.h" // so we can see what components is missing a detail
#include "Mouse.h"
#include "Printer.h"
#include "MST01.h"
#include "Text.h"



#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

extern HWND localHwnd; // clipboard uses it
extern void toUpper(basic_string<char>& s);

extern vector<Detail*>detailList;
extern vector< InnerSheet*> isL;

extern void reloadAllSubroutines();
extern Sheet* page;


int undoButtonIndex = -1;

void innerDeleteAll(Sheet* pg) {
	// delete the main sheet components
	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		delete(pg->dgrm.cL[i]);
	}
	pg->dgrm.cL.clear();
	// and wires
	for (int i = 0; i < pg->dgrm.wL.size(); i++)
		delete pg->dgrm.wL[i];
	pg->dgrm.wL.clear();

	// clear the innerSheetList
	resetSheets();
}
void deleteAll(Sheet* pg) {
	
	// if dirty ask to save
	if (pg->dgrm.cL.size() > 0) {
		if (pg->isDirty) {
			int quitResult = MessageBox(NULL, L"You have unsaved changes. Do you want to save them now?", 
				L"MST Workshop", MB_YESNOCANCEL | MB_ICONWARNING);
			if (quitResult == IDCANCEL)
				return;
			if (quitResult == IDYES)
				if (!writeComponentsAndWires(pg)) // returns false when user cancels the save
					return;
		}
	}

	innerDeleteAll(pg);

	// reset the subroutine menu
	int firstIndex = pg->buttonGroup[6];
	for (int i = 1; i < isL.size(); i++){
		int firstIndex = pg->buttonGroup[6];
		int index = firstIndex + 2 + i;
		pg->buttonList[index]->visibleText = ConvertToLPWSTR("Sub " + tomString(i) + ":");
	}

	pg->isDirty = false;
	pg->filePath = NULL;

	// clear the window title
	setTitleFromPathName(pg);
	
	pg->cXo = 0;
	pg->cYo = 0;
	if(pg->threeDwindowHandle)   // GLUT window handle - Manoj
		pg->update3Dview = true;
	drawCanvas();
	drawCanvas2();
}

void selectAll(Sheet* pg) {
	for (int i = 0; i < pg->dgrm.cL.size(); i++)
		pg->dgrm.cL[i]->selected = true;
	drawCanvas();
}

void undoLastAction() {

	int vs = page->visibleSheet;
	int size = isL[vs]->saveDiagramBuffer.size();
	if (isL[vs]->saveIndex > 0) {
		deepCopyDiagram(isL[vs]->saveDiagramBuffer[--isL[vs]->saveIndex], &page->dgrm);
		analyzeConnections(&page->dgrm);
		propagateValues(&page->dgrm);
	}
	
}
void redoLastAction() {
	int vs = page->visibleSheet;
	int size = isL[vs]->saveDiagramBuffer.size();
	if (isL[vs]->saveIndex < size - 1) {
		deepCopyDiagram(isL[vs]->saveDiagramBuffer[++isL[vs]->saveIndex], &page->dgrm);
		analyzeConnections(&page->dgrm);
		propagateValues(&page->dgrm);
	}
}




void shiftCanvasUp(Sheet* pg) {
	pg->cYo -= pg->workspaceHeight;
	drawCanvas();
}
void shiftCanvasDown(Sheet* pg) {
	pg->cYo += pg->workspaceHeight;
	drawCanvas();
}
void shiftCanvasLeft(Sheet* pg) {
	pg->cXo -= pg->workspaceWidth;
	drawCanvas();
}
void shiftCanvasRight(Sheet* pg) {
	pg->cXo += pg->workspaceWidth;
	drawCanvas();
}
void resetCanvas(Sheet* pg) {
	pg->cXo = 0;
	pg->cYo = 0;
	drawCanvas();
}
void autoCenterCanvas(Sheet* pg) {
	int xCenter = 0;
	int yCenter = 0;
	if (pg->dgrm.cL.size() > 0) {
		for (int i = 0; i < pg->dgrm.cL.size(); i++) {
			xCenter += pg->dgrm.cL[i]->x;
			yCenter += pg->dgrm.cL[i]->y;
		}
		xCenter /= pg->dgrm.cL.size();
		yCenter /= pg->dgrm.cL.size();
	}
	pg->cXo = pg->workspaceWidth / (2 * pg->cs) - xCenter;
	pg->cYo = pg->workspaceHeight / (2 * pg->cs) - yCenter;

	drawCanvas2();
}
void setNewCenter(Sheet* pg) {
	//  use the current position as the new sentor center of the workspace
	int i;
	for (i = 0; i < pg->dgrm.cL.size(); i++) {
		pg->dgrm.cL[i]->x += pg->cXo;
		pg->dgrm.cL[i]->y += pg->cYo;
	}
	for (i = 0; i < pg->dgrm.wL.size(); i++) {
		for (int j = 0; j < pg->dgrm.wL[i]->x.size(); j++) {
			pg->dgrm.wL[i]->x[j] += pg->cXo;
			pg->dgrm.wL[i]->y[j] += pg->cYo;
		}
	}

	pg->cXo = 0;
	pg->cYo = 0;
	drawCanvas();
}

int buttonCount = 0;


#if MEMORY_CHECK
_CrtMemState s1;
void startMemCheck() {
	_CrtMemCheckpoint(&s1);
}
void endMemCheck() {
	_CrtMemState s2;
	_CrtMemCheckpoint(&s2);
	_CrtMemState s3;
	if (_CrtMemDifference(&s3, &s1, &s2))
		_CrtMemDumpStatistics(&s3);
	}
#endif


Button* magnetWiresButton;
void magneticWires()				{ magnetWiresButton->isSelected = ! magnetWiresButton->isSelected; }
Button* magnetComponentsButton;
void magneticComponents()			{ magnetComponentsButton->isSelected = ! magnetComponentsButton->isSelected; }
//Button* stickyComponentsButton;
void stickyComponents(Sheet* pg) 	{ pg->stickyComponentsButton->isSelected = !pg->stickyComponentsButton->isSelected; }
Button* startWiresFromTerminalsButton;
void startWiresFromTerminals()		{ startWiresFromTerminalsButton->isSelected = ! startWiresFromTerminalsButton->isSelected; }
Button* allowWiresToBendButton;
void allowWiresToBend()				{ allowWiresToBendButton->isSelected = ! allowWiresToBendButton->isSelected; }
Button* showGraphPaperButton;
void showGraphPaper()				{ showGraphPaperButton->isSelected = !showGraphPaperButton->isSelected; }

Button* keyboardSearch;
void toggleKeyboardSearch()			{ keyboardSearch->isSelected = !keyboardSearch->isSelected; }
Button* reopenWindows;
void toggleReopenWindows() { reopenWindows->isSelected = !reopenWindows->isSelected; }


Button* showMouseButton;
void showMouse() {}
//Button* showAxisButton;
void showAxis(Sheet* pg) { pg->showAxisButton->isSelected = !pg->showAxisButton->isSelected; drawCanvas2(); }
//Button* showGridButton;
void showGrid(Sheet* pg) { pg->showGridButton->isSelected = !pg->showGridButton->isSelected; drawCanvas2(); }


Button* enable3dDefaultLightingButton;
void toggle3dDefaultLighting() { 
	enable3dDefaultLightingButton->isSelected = !enable3dDefaultLightingButton->isSelected; 
	page->update3Dview = true;
}

void copyStringToClipboard(string aString) {
	if (!OpenClipboard(localHwnd))
	{
		MessageBox(NULL, L"Cannot open the Clipboard", L"Clipboard", MB_OK);
		return;
	}
	// Remove the current Clipboard contents
	if (!EmptyClipboard())
	{
		MessageBox(NULL, L"Cannot empty the Clipboard", L"Clipboard", MB_OK);
		return;
	}

	int theSize = aString.size()+1;
	HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, theSize);
	for (int i = 0; i < theSize; i++)
		((char*)hGlob)[i] = aString[i];
	((char*)hGlob)[theSize] = 0;

	if (::SetClipboardData(CF_TEXT, hGlob) == NULL)
	{
		//int err = GetLastError();
		MessageBox(NULL, L"Unable to set Clipboard data, error: %d", L"Clipboard", MB_OK);
		CloseClipboard();
		GlobalFree(hGlob);
		return;
	}
	CloseClipboard();
}

void copyToClipboard(Sheet* pg) {
	
	// write the Component details
	string theSheet;
	for (int i = 0; i < pg->dgrm.cL.size(); i++)
		if(pg->dgrm.cL[i]->selected )
			theSheet += pg->dgrm.cL[i]->toString().c_str();

	// write the wire details
	for (int i = 0; i < pg->dgrm.wL.size(); i++)
		if (pg->dgrm.wL[i]->selected)
			theSheet += pg->dgrm.wL[i]->toString().c_str();

	copyStringToClipboard(theSheet);
}

void cutToClipboard(Sheet* pg) {
	copyToClipboard(pg);
	for (int i = 0; i < pg->dgrm.cL.size(); i++)
		if (pg->dgrm.cL[i]->selected)
			deleteComponent(pg, i);

	for (int i = 0; i < pg->dgrm.wL.size(); i++)
		if (pg->dgrm.wL[i]->selected)
			deleteWire(pg, i);

	pg->cXo = 0;
	pg->cYo = 0;
	if (pg->threeDwindowHandle)
		pg->update3Dview = true;
	drawCanvas();
	drawCanvas2();
}

HANDLE getDataFromClipboard() {

	if (!OpenClipboard(localHwnd))
	{
		MessageBox(NULL, L"Cannot open the Clipboard", L"Clipboard", MB_OK);
		return NULL;
	}

	HANDLE theData = GetClipboardData(CF_TEXT);
	if (theData == NULL)
	{
		//int err = GetLastError();
		MessageBox(NULL, L"Unable to get Clipboard data, error: %d", L"Clipboard", MB_OK);
		CloseClipboard();
		return NULL;
	}
	CloseClipboard();

	return theData;
}

void pasteFromClipboard(Sheet *pg) {

	HANDLE theData = getDataFromClipboard();
	if (theData == NULL)
		return;

	int originalComponentListSize = pg->dgrm.cL.size();
	int originalWireListSize = pg->dgrm.wL.size();

	char buffer[5000]; // hope that a component won't be bigger than this
	int ib = 0;
	char *text;
	text = (char*)theData;
	int it = 0;
	while (text[it] != 0) {
		if (text[it] == '\n') {
			buffer[ib++] = 0;
			loadSolution_(pg, buffer, true);
			ib = 0;
			it++;
			if (it > 4990) {
				MessageBox(NULL, L"Component is bigger than 5000 characters!", L"Clipboard", MB_OK);
				return;
			}
		}
		else
		{
			buffer[ib++] = text[it++];
		}
	}
	if (ib > 0) {
		buffer[ib++] = 0;
		loadSolution_(pg, buffer, true);
	}

	// select all the new components and wires
	for (int i = originalComponentListSize; i < pg->dgrm.cL.size(); i++)
		pg->dgrm.cL[i]->selected = true;

	for (int i = originalWireListSize; i < pg->dgrm.wL.size(); i++)
		pg->dgrm.wL[i]->selected = true;

	analyzeConnections(&pg->dgrm);
	propagateValues(&pg->dgrm);
	drawCanvas();
	setVisibleScreenDirty("Paste");// pg->isDirty = true;
}




void LoadButtonGroups(Sheet* pg)
{
	//int buttonGroups = 0;
	int firstComponentButton = 0;

	// Options Menu ************************************************************************

	Button::Setup(20, 40, 140, 20, 0xE0E0E0, 0); // gray
	pg->buttonGroup.push_back(0);

	
	//pg->buttonList.push_back(new Button(L"New Project", L"NewProj", CreateNewWindow));
	pg->buttonList.push_back(new Button(L"Open File...", L"OpenFile", readComponentsAndWires));
	reopenWindows = new Button(L"Reopen Windows", L"ReopenWindows", toggleReopenWindows);
	reopenWindows->isCheckbox = true;
	reopenWindows->isSelected = true;
	pg->buttonList.push_back(reopenWindows);
	pg->buttonList.push_back(new Button(L"Save File", L"SaveFile", writeComponentsAndWires_void));
	pg->buttonList.push_back(new Button(L"Save As...", L"SaveAs", writeComponentsAndWiresAs_void));
	pg->buttonList.push_back(new Button(L"Print Workspace...", L"PrintMain", PrintTheMainWindow));
	pg->buttonList.push_back(new Button(L"Print 2D View...", L"Print2dView", PrintTheCanvasWindow));
	//pg->buttonList.push_back(new Button(L"Main to BMP", L"MainToBmp", SaveBmpTheMainWindow));
	pg->buttonList.push_back(new Button(L"Quit...", L"Quit", quitProgram));

	
	Button::NextColumn(40, 90);
	undoButtonIndex = pg->buttonList.size();
	pg->buttonList.push_back(new Button(L"Undo", L"Undo", undoLastAction));
	pg->buttonList.push_back(new Button(L"Redo", L"Redo", redoLastAction));
	pg->buttonList.push_back(new Button(L"Cut", L"Cut", cutToClipboard));
	pg->buttonList.push_back(new Button(L"Copy", L"Copy", copyToClipboard));
	pg->buttonList.push_back(new Button(L"Paste", L"Paste", pasteFromClipboard));
	//pg->buttonList.push_back(new Button(L"Select All", L"SelectAll", selectAll));
	pg->buttonList.push_back(new Button(L"Delete All...", L"DeleteAll", deleteAll));
	

	Button::NextColumn(40, 115);
	pg->buttonList.push_back(new Button(L"Page Up", L"PageUp", shiftCanvasUp));
	pg->buttonList.push_back(new Button(L"Page Down", L"PageDown", shiftCanvasDown));
	pg->buttonList.push_back(new Button(L"Page Left", L"PageLeft", shiftCanvasLeft));
	pg->buttonList.push_back(new Button(L"Page Right", L"PageRight", shiftCanvasRight));
	pg->buttonList.push_back(new Button(L"Reset Page", L"ResetPage", resetCanvas));
	pg->buttonList.push_back(new Button(L"Auto Center", L"Auto Center", autoCenterCanvas));
	pg->buttonList.push_back(new Button(L"Set New Center", L"SetNewCenter", setNewCenter));
	//pg->buttonList.push_back(new Button(L"Set Time Step...", L"SetTimeStep", setTimeStep));

	Button::NextColumn(40, 180);
	magnetWiresButton = new Button(L"Magnetic Wires", L"MagneticWires", magneticWires);
	magnetWiresButton->isCheckbox = true;
	magnetWiresButton->isSelected = true;
	pg->buttonList.push_back(magnetWiresButton);

	magnetComponentsButton = new Button(L"Magnetic Components", L"MagneticComponents", magneticComponents);
	magnetComponentsButton->isCheckbox = true;
	magnetComponentsButton->isSelected = true;
	pg->buttonList.push_back(magnetComponentsButton);

	pg->stickyComponentsButton = new Button(L"Sticky Components", L"StickyComponents", stickyComponents);
	pg->stickyComponentsButton->isCheckbox = true;
	pg->stickyComponentsButton->isSelected = true;
	pg->buttonList.push_back(pg->stickyComponentsButton);

	allowWiresToBendButton = new Button(L"Allow Wires To Bend", L"AllowWiresToBend", allowWiresToBend);
	allowWiresToBendButton->isCheckbox = true;
	allowWiresToBendButton->isSelected = true;
	pg->buttonList.push_back(allowWiresToBendButton);

	showGraphPaperButton = new Button(L"Show Graph Paper", L"ShowGraphPaper", showGraphPaper);
	showGraphPaperButton->isCheckbox = true;
	showGraphPaperButton->isSelected = false;
	pg->buttonList.push_back(showGraphPaperButton);

	keyboardSearch = new Button(L"Keyboard Search", L"KeyboardSearch");
	keyboardSearch->isCheckbox = true;
	keyboardSearch->isSelected = true;
	keyboardSearch->action = toggleKeyboardSearch;
	pg->buttonList.push_back(keyboardSearch);

#if defined _DEBUG
	pg->buttonList.push_back(new Button(L"Fast Random Test", L"CompTest", componentFastRandomTest));
	pg->buttonList.push_back(new Button(L"Random Test", L"CompTest", componentRandomTest));
	pg->buttonList.push_back(new Button(L"Youtube setup 1280x720", L"Youtube", setScreenSizeYoutube));
	pg->buttonList.push_back(new Button(L"Generate Component List", L"CompList", generateDetailDocument));

	Button::NextColumn(40, 165);
	pg->buttonList.push_back(new Button(L"2D XY Position Tester", L"2dPosTester"));
	pg->buttonList.push_back(new Button(L"2D XYZ Position Tester", L"3dPosTester"));
	pg->buttonList.push_back(new Button(L"2D Color Tester", L"2dColorTester"));
	pg->buttonList.push_back(new Button(L"2 Phase Tester", L"2PhaseTester"));
	pg->buttonList.push_back(new Button(L"3 Phase Tester", L"3PhaseTester"));
	pg->buttonList.push_back(new Button(L"Bit Tester", L"BitTester"));
#endif

#if MEMORY_CHECK
	pg->buttonList.push_back(new Button(L"Memory Test Components", L"CompTest", componentMemoryTest));
	pg->buttonList.push_back(new Button(L"Start Memory Check", L"", startMemCheck));
	pg->buttonList.push_back(new Button(L"End Memory Check", L"", endMemCheck));
#endif

	// Control buttons ************************************************************************

	firstComponentButton = pg->buttonList.size();

	Button::Setup(20, 40, 120, 20, CONTROL_COLORS, 0); // green was  0xD0F0D0
	pg->buttonGroup.push_back(pg->buttonList.size());

	pg->buttonList.push_back(new Button(L"Constant", L"Con"));
	pg->buttonList.push_back(new Button(L"Slider", L"Sld"));
	pg->buttonList.push_back(new Button(L"XY Slider", L"Sld2"));
	pg->buttonList.push_back(new Button(L"Knob", L"Knob"));
	pg->buttonList.push_back(new Button(L"Rack", L"RackZ"));
	pg->buttonList.push_back(new Button(L"Geometry", L"Geom"));
	pg->buttonList.push_back(new Button(L"Function", L"Function"));
	pg->buttonList.push_back(new Button(L"2D Mouse", L"Mouse"));
	pg->buttonList.push_back(new Button(L"2D Vertical Slider", L"2dSldVZ"));
	pg->buttonList.push_back(new Button(L"2D Angled Slider", L"2dSldAZ"));
	pg->buttonList.push_back(new Button(L"2D Knob", L"2dKnobZ"));
	pg->buttonList.push_back(new Button(L"2D Sin/Cos", L"2dSinCosZ"));
	pg->buttonList.push_back(new Button(L"2D Button", L"2dButton"));
	pg->buttonList.push_back(new Button(L"2D Button 2", L"2dButton2"));
	//pg->buttonList.push_back(new Button(L"2D Popup List", L"2dPopup"));
	//pg->buttonList.push_back(new Button(L"2D Text Spinner", L"2dTxSpin"));
	pg->buttonList.push_back(new Button(L"2D Array Slider", L"2dArrSld"));

	Button::NextColumn(40, 140);
	pg->buttonList.push_back(new Button(L"Constant Factor", L"ConFac"));
	pg->buttonList.push_back(new Button(L"Slide Factor", L"SldFac"));
	pg->buttonList.push_back(new Button(L"XY Slide Factor", L"Sld2Fac"));
	pg->buttonList.push_back(new Button(L"Knob Factor", L"KnobFac"));
	pg->buttonList.push_back(new Button(L"Rack Factor", L"RackFacZ"));
	pg->buttonList.push_back(new Button(L"2D Geometry", L"2dGeom"));
	pg->buttonList.push_back(new Button(L"2D Linear Slider", L"2dLinSld"));
	pg->buttonList.push_back(new Button(L"2D Horizontal Slider", L"2dSldHZ"));
	pg->buttonList.push_back(new Button(L"2D Arc Slider", L"2dArcSldZ"));
	pg->buttonList.push_back(new Button(L"2D Vector", L"2dVector"));
	pg->buttonList.push_back(new Button(L"2D Checkbox", L"2dCHECK"));
	//pg->buttonList.push_back(new Button(L"2D List", L"2dList"));
	//pg->buttonList.push_back(new Button(L"2D String", L"2dString"));
	//pg->buttonList.push_back(new Button(L"2D Thumb Wheel", L"2dThumb"));
	pg->buttonList.push_back(new Button(L"2D Spinner", L"2dSpin"));
	pg->buttonList.push_back(new Button(L"2D Number Spinner", L"2dNumSpin"));
	//pg->buttonList.push_back(new Button(L"2D Toggle Text", L"2dToggle"));
	pg->buttonList.push_back(new Button(L"2D Radio Group", L"2dRADIO"));

	Button::NextColumn(40, 110);
	pg->buttonList.push_back(new Button(L"Constant Offset", L"ConOff"));
	pg->buttonList.push_back(new Button(L"Slide Offset", L"SldOff"));
	pg->buttonList.push_back(new Button(L"XY Slide Offset", L"Sld2Off"));
	pg->buttonList.push_back(new Button(L"Knob Offset", L"KnobOff"));
	pg->buttonList.push_back(new Button(L"Rack Offset", L"RackOffZ"));
	pg->buttonList.push_back(new Button(L"Reset Clock", L"Reset"));
	pg->buttonList.push_back(new Button(L"Start Clock", L"Start"));
	pg->buttonList.push_back(new Button(L"Stop Clock", L"Stop"));
	pg->buttonList.push_back(new Button(L"Step Clock", L"Step"));
	pg->buttonList.push_back(new Button(L"Time Step", L"TimeStep"));
	pg->buttonList.push_back(new Button(L"Running", L"Running"));
	pg->buttonList.push_back(new Button(L"Snap Point", L"SnapPoint2"));

	Button::NextColumn(40, 95);
	pg->buttonList.push_back(new Button(L"Display List", L"Dis"));
	pg->buttonList.push_back(new Button(L"Display Grid", L"Grid"));
	pg->buttonList.push_back(new Button(L"Meter", L"Meter"));
	//pg->buttonList.push_back(new Button(L"Compass", L"Compass"));
	//pg->buttonList.push_back(new Button(L"Level", L"Level"));
	pg->buttonList.push_back(new Button(L"Chart", L"Chart"));
	//pg->buttonList.push_back(new Button(L"Bar Chart", L"BarChart"));
	pg->buttonList.push_back(new Button(L"XY Chart", L"Chart2"));
	pg->buttonList.push_back(new Button(L"Recorder", L"Rec"));
	pg->buttonList.push_back(new Button(L"XY Recorder", L"XyRec"));
	//pg->buttonList.push_back(new Button(L"Contour Plot", L"Contour"));
	pg->buttonList.push_back(new Button(L"Light", L"LED"));
	pg->buttonList.push_back(new Button(L"Switch", L"Switch"));
	pg->buttonList.push_back(new Button(L"Button", L"Button"));
	pg->buttonList.push_back(new Button(L"Beep", L"Beep"));
	pg->buttonList.push_back(new Button(L"Color Patch", L"ClrPatch"));


	Button::NextColumn(40, 85);
	pg->buttonList.push_back(new Button(L"Input", L"Input"));
	pg->buttonList.push_back(new Button(L"Output", L"Output"));
	pg->buttonList.push_back(new Button(L"Bus In", L"BussIn"));
	pg->buttonList.push_back(new Button(L"Bus Out", L"BussOut"));
	pg->buttonList.push_back(new Button(L"Bus 2 In", L"Buss2In"));
	pg->buttonList.push_back(new Button(L"Bus 2 Out", L"Buss2Out"));
	//pg->buttonList.push_back(new Button(L"Transmitter", L"Transmit"));
	//pg->buttonList.push_back(new Button(L"Receiver", L"Receive"));
	//pg->buttonList.push_back(new Button(L"Preset", L"Preset"));
	//pg->buttonList.push_back(new Button(L"Remote", L"Remote"));
	//pg->buttonList.push_back(new Button(L"Get Text", L"GetText"));
	//pg->buttonList.push_back(new Button(L"Get Value", L"GetValue"));
	//pg->buttonList.push_back(new Button(L"Get Value Group", L"GetValueGrp"));
	//pg->buttonList.push_back(new Button(L"Get Bool", L"GetBool"));
	//pg->buttonList.push_back(new Button(L"Program Quiz", L"ProgQuiz"));
	//pg->buttonList.push_back(new Button(L"Submit", L"Submit"));
	//pg->buttonList.push_back(new Button(L"Get Constant", L"GetConst"));

	// Function buttons ****************************************************************

	Button::Setup(20, 40, 90, 20, FUNCTION_COLORS, 0);  // red was  0xD0D0FF
	pg->buttonGroup.push_back(pg->buttonList.size());

	pg->buttonList.push_back(new Button(L"Add", L"Add"));
	pg->buttonList.push_back(new Button(L"Subtract", L"Sub"));
	pg->buttonList.push_back(new Button(L"Multiply", L"Mul"));
	pg->buttonList.push_back(new Button(L"Divide", L"Div"));
	pg->buttonList.push_back(new Button(L"Invert", L"Inv"));
	pg->buttonList.push_back(new Button(L"Negate", L"Neg"));
	pg->buttonList.push_back(new Button(L"Abs", L"Abs"));
	pg->buttonList.push_back(new Button(L"Min", L"Min2"));
	pg->buttonList.push_back(new Button(L"Max", L"Max2"));
	pg->buttonList.push_back(new Button(L"Modulo", L"Mod"));
	pg->buttonList.push_back(new Button(L"Sqrt", L"Sqrt"));
	pg->buttonList.push_back(new Button(L"Square", L"Square"));
	pg->buttonList.push_back(new Button(L"Cube", L"Cube"));
	pg->buttonList.push_back(new Button(L"Exponential", L"Exp"));
	pg->buttonList.push_back(new Button(L"Power", L"Pow"));
	pg->buttonList.push_back(new Button(L"Log", L"Log"));
	pg->buttonList.push_back(new Button(L"Quadratic", L"Quad"));

	// column 2
	Button::NextColumn(40, 90);
	pg->buttonList.push_back(new Button(L"Floor", L"Floor"));
	pg->buttonList.push_back(new Button(L"Ceiling", L"Ceil"));
	pg->buttonList.push_back(new Button(L"Round", L"Round"));
	pg->buttonList.push_back(new Button(L"Precision", L"Precision"));
	pg->buttonList.push_back(new Button(L"Detent", L"Detent"));
	pg->buttonList.push_back(new Button(L"Reasonable", L"Reasonable"));
	pg->buttonList.push_back(new Button(L"Sign", L"Sign"));
	pg->buttonList.push_back(new Button(L"Ave", L"Ave2"));
	pg->buttonList.push_back(new Button(L"Smallest", L"Small"));
	pg->buttonList.push_back(new Button(L"Largest", L"Large"));
	pg->buttonList.push_back(new Button(L"Limit", L"Limit"));
	pg->buttonList.push_back(new Button(L"In Range", L"InRange"));
	pg->buttonList.push_back(new Button(L"In Polygon", L"InPoly"));
	pg->buttonList.push_back(new Button(L"Sample", L"Sample"));
	pg->buttonList.push_back(new Button(L"Hold", L"Hold"));
	pg->buttonList.push_back(new Button(L"Compare", L"Compare"));
	pg->buttonList.push_back(new Button(L"Finite State", L"FSM"));

	// column 3
	Button::NextColumn(40, 105);
	pg->buttonList.push_back(new Button(L"AND", L"And"));
	pg->buttonList.push_back(new Button(L"OR", L"Or"));
	pg->buttonList.push_back(new Button(L"NAND", L"Nand"));
	pg->buttonList.push_back(new Button(L"NOR", L"Nor"));
	pg->buttonList.push_back(new Button(L"XOR", L"Xor"));
	pg->buttonList.push_back(new Button(L"NOT", L"Not"));
	pg->buttonList.push_back(new Button(L"Logic", L"Logic"));
	pg->buttonList.push_back(new Button(L"D to A", L"DtoA"));
	pg->buttonList.push_back(new Button(L"Bit AND", L"BitAnd"));
	pg->buttonList.push_back(new Button(L"Bit OR", L"BitOr"));
	pg->buttonList.push_back(new Button(L"Bit XOR", L"BitXor"));
	pg->buttonList.push_back(new Button(L"Bit NOT", L"BitNot"));
	pg->buttonList.push_back(new Button(L"Flip Flop", L"Flip"));
	pg->buttonList.push_back(new Button(L"D Flip Flop", L"DFlip"));
	pg->buttonList.push_back(new Button(L"Triggered Sum", L"TrigSum"));
	pg->buttonList.push_back(new Button(L"Radio", L"OneEdge"));
	pg->buttonList.push_back(new Button(L"Toggle", L"AnyEdge"));

	// column 4
	Button::NextColumn(40, 45);
	pg->buttonList.push_back(new Button(L">", L"GT"));
	pg->buttonList.push_back(new Button(L"<", L"LT"));
	pg->buttonList.push_back(new Button(L"=", L"EQ"));
	pg->buttonList.push_back(new Button(L">=", L"GTE"));
	pg->buttonList.push_back(new Button(L"<=", L"LTE"));
	pg->buttonList.push_back(new Button(L"!=", L"NEQ"));
	pg->buttonList.push_back(new Button(L">0", L"GTZ"));
	pg->buttonList.push_back(new Button(L"<0", L"LTZ"));
	pg->buttonList.push_back(new Button(L"=0", L"EQZ"));
	pg->buttonList.push_back(new Button(L">=0", L"GTEZ"));
	pg->buttonList.push_back(new Button(L"<=0", L"LTEZ"));
	pg->buttonList.push_back(new Button(L"!=0", L"NEQZ"));
	pg->buttonList.push_back(new Button(L"LIFO", L"LIFO"));
	pg->buttonList.push_back(new Button(L"FIFO", L"FIFO"));

	// column 5
	Button::NextColumn(40, 80);
	pg->buttonList.push_back(new Button(L"Sine", L"Sin"));
	pg->buttonList.push_back(new Button(L"Cosine", L"Cos"));
	pg->buttonList.push_back(new Button(L"Tangent", L"Tan"));
	pg->buttonList.push_back(new Button(L"Arc Sin", L"Asin"));
	pg->buttonList.push_back(new Button(L"Arc Cos", L"Acos"));
	pg->buttonList.push_back(new Button(L"Arc Tan", L"Atan"));
	pg->buttonList.push_back(new Button(L"Arc Tan2", L"Atan2"));
	pg->buttonList.push_back(new Button(L"To RGB", L"ToRGB"));
	pg->buttonList.push_back(new Button(L"To HSV", L"ToHSV"));
	pg->buttonList.push_back(new Button(L"To HMS", L"ToHMS"));
	pg->buttonList.push_back(new Button(L"To HM", L"ToHM"));
	pg->buttonList.push_back(new Button(L"Scale", L"Scale"));
	pg->buttonList.push_back(new Button(L"Log Scale", L"LogScale"));
	pg->buttonList.push_back(new Button(L"Factorial", L"Fact"));
	//pg->buttonList.push_back(new Button(L"RT Filter", L"RtFilter"));
	
	// column 6
	Button::NextColumn(40, 130);
	pg->buttonList.push_back(new Button(L"Multiplexer", L"Mux"));
	pg->buttonList.push_back(new Button(L"Fader", L"Fade"));
	pg->buttonList.push_back(new Button(L"Selector", L"Select"));
	pg->buttonList.push_back(new Button(L"Deselector", L"Deselect"));
	pg->buttonList.push_back(new Button(L"Integrator", L"Integ"));
	//pg->buttonList.push_back(new Button(L"Trig. Integ.", L"TrigInteg"));
	//pg->buttonList.push_back(new Button(L"Differentiate", L"Diff"));
	//pg->buttonList.push_back(new Button(L"Chain Model", L"ChainModel"));
	//pg->buttonList.push_back(new Button(L"Product", L"Prod"));
	pg->buttonList.push_back(new Button(L"Simplex Solver", L"Simp"));
	//pg->buttonList.push_back(new Button(L"Raster", L"Rast"));
	pg->buttonList.push_back(new Button(L"Polynomial", L"Polynomial"));
	pg->buttonList.push_back(new Button(L"Poly. Integral", L"PolyInteg"));
	pg->buttonList.push_back(new Button(L"Poly. Derivative", L"PolyDeriv"));
	//pg->buttonList.push_back(new Button(L"Binary Search", L"BinSear"));
	//pg->buttonList.push_back(new Button(L"Test", L"Test"));
	pg->buttonList.push_back(new Button(L"Time Random", L"TimeRand"));
	pg->buttonList.push_back(new Button(L"Triggered Random", L"TrigRand"));
	pg->buttonList.push_back(new Button(L"Random Position", L"RandPos"));
	pg->buttonList.push_back(new Button(L"MIDI Play", L"MidiPlay"));
	pg->buttonList.push_back(new Button(L"MIDI Percussion", L"MidiDrum"));
	pg->buttonList.push_back(new Button(L"Sequencer", L"Sequ"));

	// column 7
	Button::NextColumn(40, 105);
	pg->buttonList.push_back(new Button(L"Time", L"Time"));
	pg->buttonList.push_back(new Button(L"RT Clock", L"Clock"));
	pg->buttonList.push_back(new Button(L"Year", L"Year"));
	pg->buttonList.push_back(new Button(L"Month", L"Month"));
	pg->buttonList.push_back(new Button(L"Day", L"Day"));
	pg->buttonList.push_back(new Button(L"Hour", L"Hour"));
	pg->buttonList.push_back(new Button(L"Minute", L"Minute"));
	pg->buttonList.push_back(new Button(L"Second", L"Second"));
	//pg->buttonList.push_back(new Button(L"Milisec.", L"Millisec"));
	pg->buttonList.push_back(new Button(L"One Shot", L"OneShot"));
	pg->buttonList.push_back(new Button(L"Timer", L"Timer"));
	pg->buttonList.push_back(new Button(L"Timer 2", L"Timer2"));
	//pg->buttonList.push_back(new Button(L"Delay", L"Delay"));
	pg->buttonList.push_back(new Button(L"Day Of Week", L"DayOfWeek"));
	pg->buttonList.push_back(new Button(L"Days In Month", L"DaysInMon"));
	pg->buttonList.push_back(new Button(L"Debug", L"Debug"));

	// column 8
	Button::NextColumn(40, 140);
	pg->buttonList.push_back(new Button(L"Square Wave", L"SquareWave"));
	pg->buttonList.push_back(new Button(L"Sawtooth", L"Sawtooth"));
	pg->buttonList.push_back(new Button(L"Sine Wave", L"SineWave"));
	pg->buttonList.push_back(new Button(L"Triangular Wave", L"TriWave"));
	pg->buttonList.push_back(new Button(L"Pulse Generator", L"Pulse"));
	pg->buttonList.push_back(new Button(L"Binary Counter", L"Counter"));
	pg->buttonList.push_back(new Button(L"Scalar Counter", L"SCount"));
	pg->buttonList.push_back(new Button(L"2D Dot Product", L"Dot2Prod"));
	pg->buttonList.push_back(new Button(L"3D Dot Product", L"Dot3Prod"));
	pg->buttonList.push_back(new Button(L"Cross Product", L"CrossProd"));
	pg->buttonList.push_back(new Button(L"2D Flat Distance", L"FDist"));
	pg->buttonList.push_back(new Button(L"Spherical Distance", L"SDist"));
	pg->buttonList.push_back(new Button(L"3D Distance", L"3Dist"));
	pg->buttonList.push_back(new Button(L"Slope Intercept", L"Slope"));
	pg->buttonList.push_back(new Button(L"Intersect Lines", L"Intersect"));
	pg->buttonList.push_back(new Button(L"Intersect Circles", L"Intersect2"));
	pg->buttonList.push_back(new Button(L"Intersect Line/Circle", L"Intersect3"));
	
	// column 9
	Button::NextColumn(40, 125);
	//pg->buttonList.push_back(new Button(L"Audio Square", L"AudSquare"));
	//pg->buttonList.push_back(new Button(L"Audio Sawtooth", L"AudSawtooth"));
	//pg->buttonList.push_back(new Button(L"Audio Sine", L"AudSine"));
	//pg->buttonList.push_back(new Button(L"Audio Triangle", L"AudTriangle"));
	pg->buttonList.push_back(new Button(L"Factor", L"Factor"));
	pg->buttonList.push_back(new Button(L"Common Factor", L"ComFactor"));
	pg->buttonList.push_back(new Button(L"Reduce", L"Reduce"));
	pg->buttonList.push_back(new Button(L"Add Fractions", L"AddFrac"));
	pg->buttonList.push_back(new Button(L"Multiply Fractions", L"MulFrac"));
	pg->buttonList.push_back(new Button(L"Divide Fractions", L"DivFrac"));
	pg->buttonList.push_back(new Button(L"Sunrise", L"Sunrise"));
	pg->buttonList.push_back(new Button(L"Sun Position", L"SunPos"));
	pg->buttonList.push_back(new Button(L"Moon Phase", L"MoonPhase"));
	pg->buttonList.push_back(new Button(L"Elliptical Path", L"Ellipse"));
	pg->buttonList.push_back(new Button(L"Parabolic Path", L"Parabola"));
	pg->buttonList.push_back(new Button(L"Play Sound File", L"PlySndFile"));
	pg->buttonList.push_back(new Button(L"Version", L"Version"));
	//pg->buttonList.push_back(new Button(L"Warning", L"Warning"));


	// Drawing buttons ***************  2D  *********************************************

	Button::Setup(20, 40, 145, 20, DRAWING_COLORS, 0);  // BLUE was  0xFFD0D0,
	pg->buttonGroup.push_back(pg->buttonList.size());

	Button* open2dview = new Button(L"Open 2D View", L"Open2dView"); 
	open2dview->action = Open2DView;
	pg->buttonList.push_back(open2dview);

	//showMouseButton = new Button(L"Show Coords", L"ShowCoords");
	//showMouseButton->isCheckbox = true;
	//showMouseButton->isSelected = false;
	//showMouseButton->action = showMouse;
	//pg->buttonList.push_back(showMouseButton);

	pg->showAxisButton = new Button(L"Show 2D Axis", L"Show2dAxis");
	pg->showAxisButton->isCheckbox = true;
	pg->showAxisButton->isSelected = false;
	pg->showAxisButton->actionPage = showAxis;
	pg->buttonList.push_back(pg->showAxisButton);

	pg->showGridButton = new Button(L"Show 2D Grid", L"Show2dGrid");
	pg->showGridButton->isCheckbox = true;
	pg->showGridButton->isSelected = false;
	pg->showGridButton->actionPage = showGrid;
	pg->buttonList.push_back(pg->showGridButton);
	pg->buttonList.push_back(new Button(L"2D View Control", L"2dDisp"));

	pg->buttonList.push_back(new Button(L"2D Box", L"BOX"));
	pg->buttonList.push_back(new Button(L"2D Circle", L"CIRCLE"));
	pg->buttonList.push_back(new Button(L"2D Parabola", L"PARAB"));
	pg->buttonList.push_back(new Button(L"2D Polygon", L"POLY"));
	pg->buttonList.push_back(new Button(L"2D Poly Line", L"LINE"));
	pg->buttonList.push_back(new Button(L"2D Line Segment", L"LINES"));
	pg->buttonList.push_back(new Button(L"2D Cross", L"CROSS"));
	pg->buttonList.push_back(new Button(L"2D Marker", L"MARKER"));
	pg->buttonList.push_back(new Button(L"2D Value", L"VALUE"));
	pg->buttonList.push_back(new Button(L"2D Arrow", L"ARROW"));
	pg->buttonList.push_back(new Button(L"2D Ellipse on Center", L"ELIPS"));
	pg->buttonList.push_back(new Button(L"2D Ellipse on Focus", L"ELIPS2"));
	pg->buttonList.push_back(new Button(L"2D Horizontal Axis", L"HAXIS"));
	pg->buttonList.push_back(new Button(L"2D Vertical Axis", L"VAXIS"));
	pg->buttonList.push_back(new Button(L"2D XY Axis", L"XyAxis"));
	pg->buttonList.push_back(new Button(L"Tic and Label", L"TicLab"));
	//pg->buttonList.push_back(new Button(L"Circuit Analysis", L"CircAnal"));
	//pg->buttonList.push_back(new Button(L"Pattern Analysis", L"PatAnal"));
	//pg->buttonList.push_back(new Button(L"Bounding Box", L"BoundBox"));

	Button::NextColumn(40, 105);
	pg->buttonList.push_back(new Button(L"Triangle Area", L"TriArea"));
	pg->buttonList.push_back(new Button(L"Point to Line", L"Point2Line"));
	pg->buttonList.push_back(new Button(L"Circle Center", L"Cc3pts"));
	pg->buttonList.push_back(new Button(L"XY Rotate", L"Rotate"));
	pg->buttonList.push_back(new Button(L"XY to Polar", L"ToPolar"));
	pg->buttonList.push_back(new Button(L"Polar to XY", L"ToCart"));
	pg->buttonList.push_back(new Button(L"XYZ to LatLon", L"XyzToLl"));
	pg->buttonList.push_back(new Button(L"LatLon to XYZ", L"LlToXyz"));
	pg->buttonList.push_back(new Button(L"Center to Edge", L"Center2Edge"));
	pg->buttonList.push_back(new Button(L"Edge to Center", L"Edge2Center"));
	
	pg->buttonList.push_back(new Button(L"2D Length", L"2dLength"));
	pg->buttonList.push_back(new Button(L"2D Scale", L"2dScale"));
	pg->buttonList.push_back(new Button(L"2D Delta", L"2dDelta"));
	pg->buttonList.push_back(new Button(L"2D Meter", L"2dMeter"));
	pg->buttonList.push_back(new Button(L"2D Chart", L"2dChart"));
	pg->buttonList.push_back(new Button(L"2D XY Chart", L"2dChart2"));
	//pg->buttonList.push_back(new Button(L"2D Contour Plot", L"2dContour"));
	pg->buttonList.push_back(new Button(L"2D Field Plot", L"2dField"));
	//pg->buttonList.push_back(new Button(L"Line Analysis", L"LineAnal"));
	pg->buttonList.push_back(new Button(L"2D Text", L"2dText"));
	pg->buttonList.push_back(new Button(L"2D Bitmap", L"2dBitmap"));



	// Drawing buttons ***************** 3D  **********************************

	Button::NextColumn(40, 120);
	Button* open3dview = new Button(L"Open 3D View", L"Open3dView");
	open3dview->action = Open3DView;
	pg->buttonList.push_back(open3dview);
	pg->buttonList.push_back(new Button(L"3D View Control", L"3dDisp"));

	//pg->buttonList.push_back(new Button(L"3D View", L"3View"));
	//pg->buttonList.push_back(new Button(L"3D Origin", L"3Origin"));
	pg->buttonList.push_back(new Button(L"3D Rotate", L"3XYZROT"));
	pg->buttonList.push_back(new Button(L"3D X Rotate", L"3ROTX"));
	pg->buttonList.push_back(new Button(L"3D Y Rotate", L"3ROTY"));
	pg->buttonList.push_back(new Button(L"3D Z Rotate", L"3ROTZ"));
	pg->buttonList.push_back(new Button(L"3D Shift", L"3XYZSHIFT"));
	pg->buttonList.push_back(new Button(L"3D Scale", L"3XYZSCALE"));
	pg->buttonList.push_back(new Button(L"3D Get Direction", L"3UNROT"));
	pg->buttonList.push_back(new Button(L"3D Bridge", L"3BRIDGE"));


	Button::NextColumn(40, 130);
	pg->buttonList.push_back(new Button(L"3D Axis", L"3Axis"));
	pg->buttonList.push_back(new Button(L"3D Poly Line", L"3Line"));
	pg->buttonList.push_back(new Button(L"3D Line Segment", L"3Lines"));
	pg->buttonList.push_back(new Button(L"3D Polygon", L"3Polygon"));
	pg->buttonList.push_back(new Button(L"3D Circle", L"3Circle"));
	pg->buttonList.push_back(new Button(L"3D Arrow", L"3Arrows"));
	pg->buttonList.push_back(new Button(L"3D Grid", L"3Grid"));
	pg->buttonList.push_back(new Button(L"3D Cube", L"3CUBE"));
	pg->buttonList.push_back(new Button(L"3D Sphere", L"3SPHERE"));
	pg->buttonList.push_back(new Button(L"3D Cylinder", L"3CYLIND"));
	pg->buttonList.push_back(new Button(L"3D Cone", L"3CONE"));
	pg->buttonList.push_back(new Button(L"3D Torus", L"3TORUS"));
//	pg->buttonList.push_back(new Button(L"3D Tetrahedron", L"3Tetra"));
//	pg->buttonList.push_back(new Button(L"3D Octahedron", L"3Octa"));
//	pg->buttonList.push_back(new Button(L"3D Dodecahedron", L"3Dodeca"));
//	pg->buttonList.push_back(new Button(L"3D Icosahedron", L"3Icosa"));

	Button::NextColumn(40, 150);
	pg->buttonList.push_back(new Button(L"3D Light", L"3Light"));
	//pg->buttonList.push_back(new Button(L"3D Directional Light", L"3DirLight"));
	//pg->buttonList.push_back(new Button(L"3D Point Light", L"3PointLight"));

	enable3dDefaultLightingButton = new Button(L"3D Default Lighting");
	enable3dDefaultLightingButton->isCheckbox = true;
	enable3dDefaultLightingButton->isSelected = true;
	enable3dDefaultLightingButton->action = toggle3dDefaultLighting;
	pg->buttonList.push_back(enable3dDefaultLightingButton);

	pg->buttonList.push_back(new Button(L"3D Plane Surface", L"3PlSurf"));
	pg->buttonList.push_back(new Button(L"3D Plane Surface 2", L"3PlSurf2"));
	pg->buttonList.push_back(new Button(L"3D Sphere Surface", L"3SphSurf"));
	pg->buttonList.push_back(new Button(L"3D Sphere Surface 2", L"3SphSurf2"));
	pg->buttonList.push_back(new Button(L"3D Cylinder Surface", L"3CylSurf"));
	pg->buttonList.push_back(new Button(L"3D Cylinder Surface 2", L"3CylSurf2"));
	pg->buttonList.push_back(new Button(L"3D Extrude", L"3Extrude"));
	pg->buttonList.push_back(new Button(L"3D Lathe", L"3Lathe"));
	pg->buttonList.push_back(new Button(L"3D Surface", L"3Surf"));
	

	// ARRAY BUTTONS *************************************************************

	Button::Setup(20, 40, 95, 20, ARRAY_COLORS, 0);  //VIOLET was  0xFFD0FF
	pg->buttonGroup.push_back(pg->buttonList.size());

	pg->buttonList.push_back(new Button(L"Table", L"Table"));
	pg->buttonList.push_back(new Button(L"Matrix", L"Matrix"));
	pg->buttonList.push_back(new Button(L"Synthesizer", L"Synth"));
	pg->buttonList.push_back(new Button(L"Poly Synth", L"PolySynth"));
	pg->buttonList.push_back(new Button(L"Gather", L"Gather"));
	pg->buttonList.push_back(new Button(L"Buffer", L"Buffer"));
	pg->buttonList.push_back(new Button(L"Joiner", L"Join"));
	pg->buttonList.push_back(new Button(L"Splitter", L"Split"));
	//pg->buttonList.push_back(new Button(L"Array Parse", L"Parse2"));
	pg->buttonList.push_back(new Button(L"Merge", L"Merge"));
	pg->buttonList.push_back(new Button(L"Shuffle", L"Shuffle"));
	pg->buttonList.push_back(new Button(L"Deal", L"Deal"));
	pg->buttonList.push_back(new Button(L"First N", L"FirstSet"));
	pg->buttonList.push_back(new Button(L"Last N", L"LastSet"));
	pg->buttonList.push_back(new Button(L"Subset", L"SubSet"));
	pg->buttonList.push_back(new Button(L"Excise", L"Excise"));
	pg->buttonList.push_back(new Button(L"Expander", L"Expand"));
	pg->buttonList.push_back(new Button(L"Roll", L"Roll"));
	pg->buttonList.push_back(new Button(L"Splice In", L"SpliceIn"));
	pg->buttonList.push_back(new Button(L"Splice Out", L"SpliceOut"));
	pg->buttonList.push_back(new Button(L"Func. Gen.", L"FuncGen"));

	Button::NextColumn(40, 100);

	pg->buttonList.push_back(new Button(L"Mirror", L"Mirror"));
	pg->buttonList.push_back(new Button(L"Keep", L"Keep"));
	pg->buttonList.push_back(new Button(L"Keep First", L"KeepFirst"));
	pg->buttonList.push_back(new Button(L"Keep Last", L"KeepLast"));
	pg->buttonList.push_back(new Button(L"Arrange", L"Arrange"));
	pg->buttonList.push_back(new Button(L"Inner Repeat", L"Repeat"));
	pg->buttonList.push_back(new Button(L"Inner Joiner", L"InJoin"));
	pg->buttonList.push_back(new Button(L"Duplicate", L"Dup"));
	pg->buttonList.push_back(new Button(L"Replace", L"Replace"));
	pg->buttonList.push_back(new Button(L"Find Index", L"FindIndex"));
	pg->buttonList.push_back(new Button(L"Find Bool", L"FindBool"));
	pg->buttonList.push_back(new Button(L"Get At", L"GetAt"));
	pg->buttonList.push_back(new Button(L"Get Until", L"GetUntil"));
	pg->buttonList.push_back(new Button(L"Put At", L"PutAt"));
	pg->buttonList.push_back(new Button(L"Interpolate", L"Interp"));
	pg->buttonList.push_back(new Button(L"Spline", L"Spline"));
	pg->buttonList.push_back(new Button(L"Spline XY", L"SplineXY"));
	pg->buttonList.push_back(new Button(L"Filter", L"Filter"));
	pg->buttonList.push_back(new Button(L"Ravel", L"Ravel"));
	pg->buttonList.push_back(new Button(L"Dimensions", L"Dims"));

	Button::NextColumn(40, 140);

	pg->buttonList.push_back(new Button(L"Count", L"Count"));
	pg->buttonList.push_back(new Button(L"Inner Sum", L"Sum"));
	pg->buttonList.push_back(new Button(L"Inner Product", L"ProdA"));
	pg->buttonList.push_back(new Button(L"Array Sums", L"Sums"));
	pg->buttonList.push_back(new Button(L"Array Diffs", L"Difs"));
	pg->buttonList.push_back(new Button(L"Outer Sum", L"OuterSum"));
	pg->buttonList.push_back(new Button(L"Outer Product", L"OuterProd"));
	pg->buttonList.push_back(new Button(L"Average", L"Ave"));
	pg->buttonList.push_back(new Button(L"Median", L"Median"));
	pg->buttonList.push_back(new Button(L"Mode", L"Mode"));
	pg->buttonList.push_back(new Button(L"Standard Deviation", L"Stdev"));
	pg->buttonList.push_back(new Button(L"Variance", L"Var"));
	pg->buttonList.push_back(new Button(L"Correlation", L"Corr"));
	//pg->buttonList.push_back(new Button(L"String Simulation", L"StringSim"));
	pg->buttonList.push_back(new Button(L"Array Rands", L"ArrayRand"));
	pg->buttonList.push_back(new Button(L"Minimum", L"Min"));
	pg->buttonList.push_back(new Button(L"Maximum", L"Max"));
	pg->buttonList.push_back(new Button(L"Data Range", L"Range"));
	pg->buttonList.push_back(new Button(L"Min Index", L"MinIdx"));
	pg->buttonList.push_back(new Button(L"Max Index", L"MaxIdx"));
	pg->buttonList.push_back(new Button(L"Alternate", L"Alt"));
	//pg->buttonList.push_back(new Button(L"Diagonal Matrix", L"MatDiag"));

	Button::NextColumn(40, 135);
	pg->buttonList.push_back(new Button(L"Matrix Get At", L"MatGetAt"));
	pg->buttonList.push_back(new Button(L"Matrix Multiply", L"MatMul"));
	pg->buttonList.push_back(new Button(L"Matrix Invert", L"MatInv"));
	pg->buttonList.push_back(new Button(L"Matrix Transpose", L"MatTran"));
	pg->buttonList.push_back(new Button(L"Matrix Subset", L"MatSubset"));
	pg->buttonList.push_back(new Button(L"Matrix Excise", L"MatExcise"));
	pg->buttonList.push_back(new Button(L"Matrix Joiner", L"MJoin"));
	pg->buttonList.push_back(new Button(L"Matrix Splitter", L"MSplit"));
	//pg->buttonList.push_back(new Button(L"Matrix Sums", L"MatSums"));
	pg->buttonList.push_back(new Button(L"Matrix Interpolate", L"MatInterp"));
	pg->buttonList.push_back(new Button(L"Determinant", L"MatDet"));
	pg->buttonList.push_back(new Button(L"Histogram", L"Histo"));
	pg->buttonList.push_back(new Button(L"Numerical Sort", L"NSort"));
	pg->buttonList.push_back(new Button(L"Numerical Rank", L"NRank"));
	pg->buttonList.push_back(new Button(L"Bit Joiner", L"BitJoin"));
	pg->buttonList.push_back(new Button(L"Bit Splitter", L"BitSplit"));
	pg->buttonList.push_back(new Button(L"Bits to Number", L"BitToN"));
	pg->buttonList.push_back(new Button(L"Number to Bits", L"NToBit"));
	pg->buttonList.push_back(new Button(L"Index to Position", L"IToPos"));
	//pg->buttonList.push_back(new Button(L"Point Charge", L"PointCharge"));
	//pg->buttonList.push_back(new Button(L"Gravity", L"Gravity"));
	pg->buttonList.push_back(new Button(L"Bezier", L"Bezier"));
	pg->buttonList.push_back(new Button(L"Bezier Derivitave", L"BezDeriv"));
	//pg->buttonList.push_back(new Button(L"Bell Jar", L"BellJar"));
	//pg->buttonList.push_back(new Button(L"Bell Jar2", L"BellJar2"));

	Button::NextColumn(40, 140);

	pg->buttonList.push_back(new Button(L"Union", L"Union"));
	pg->buttonList.push_back(new Button(L"Intersection", L"Inter"));
	pg->buttonList.push_back(new Button(L"Difference", L"SetDiff"));
	pg->buttonList.push_back(new Button(L"Only in A", L"OnlyA"));
	pg->buttonList.push_back(new Button(L"Only in B", L"OnlyB"));
	pg->buttonList.push_back(new Button(L"Periodic", L"Period"));
	pg->buttonList.push_back(new Button(L"Planet", L"Planet"));
	pg->buttonList.push_back(new Button(L"US Cities", L"UsCity"));
	pg->buttonList.push_back(new Button(L"World Cities", L"WorldCity"));
	pg->buttonList.push_back(new Button(L"Savings", L"Savings"));
	pg->buttonList.push_back(new Button(L"Loan", L"Loan"));
	pg->buttonList.push_back(new Button(L"Primes", L"Primes"));
	pg->buttonList.push_back(new Button(L"Combine", L"Perm"));
	pg->buttonList.push_back(new Button(L"Permute", L"Perm2"));
	pg->buttonList.push_back(new Button(L"r-permute", L"Perm3"));
	//pg->buttonList.push_back(new Button(L"Life", L"Life"));
	//pg->buttonList.push_back(new Button(L"Split On Value", L"SplitValue"));
	pg->buttonList.push_back(new Button(L"Eliminate Value", L"ElimValue"));
	//pg->buttonList.push_back(new Button(L"Match Pattern", L"MatchPatt"));
	//pg->buttonList.push_back(new Button(L"Value Merge", L"ValueMerge"));
	//pg->buttonList.push_back(new Button(L"Value Unmerge", L"ValueUnmerge"));
	//pg->buttonList.push_back(new Button(L"Value Enclosed", L"ValueEnclosed"));
	pg->buttonList.push_back(new Button(L"Value Terms", L"ValueTerms"));
	//pg->buttonList.push_back(new Button(L"Value Terms 2", L"ValueTerms2"));
	//pg->buttonList.push_back(new Button(L"Value Replace", L"ValueReplace"));
	pg->buttonList.push_back(new Button(L"Linear Regression", L"LinReg"));
	pg->buttonList.push_back(new Button(L"Least Squares Fit", L"LstSqr"));
	pg->buttonList.push_back(new Button(L"Least Squares N Fit", L"LstSqrN"));
	pg->buttonList.push_back(new Button(L"Harmonic Gen", L"HarmGen"));
	pg->buttonList.push_back(new Button(L"Harmonic Gen 2", L"HarmGen2"));

	Button::NextColumn(40, 115);

	pg->buttonList.push_back(new Button(L"Literal", L"Lit"));
	pg->buttonList.push_back(new Button(L"String Length", L"StrLen"));
	pg->buttonList.push_back(new Button(L"Alpha Sort", L"ASort"));
	pg->buttonList.push_back(new Button(L"Alpha Rank", L"ARank"));
	pg->buttonList.push_back(new Button(L"String Joiner", L"SJoin"));
	pg->buttonList.push_back(new Button(L"Concatenate", L"Concat"));
	pg->buttonList.push_back(new Button(L"Up Case", L"UpCase"));
	pg->buttonList.push_back(new Button(L"Low Case", L"LoCase"));
	pg->buttonList.push_back(new Button(L"To Number", L"ToNumber"));
	pg->buttonList.push_back(new Button(L"To ASCII", L"ToASCII"));
	pg->buttonList.push_back(new Button(L"To String", L"ToString"));
	pg->buttonList.push_back(new Button(L"To Char", L"ToChar"));
	pg->buttonList.push_back(new Button(L"UTF 8", L"UTF"));
	pg->buttonList.push_back(new Button(L"String 2D Width", L"Str2dWid"));
	pg->buttonList.push_back(new Button(L"Sub String", L"SubStr"));
	pg->buttonList.push_back(new Button(L"String Parse", L"Parse"));
	pg->buttonList.push_back(new Button(L"Get Char At", L"CharAt"));
	pg->buttonList.push_back(new Button(L"Find Char Index", L"CharIndex"));
	pg->buttonList.push_back(new Button(L"Morse Code", L"Morse"));
	pg->buttonList.push_back(new Button(L"Array to Sound", L"Arr2Sound"));
	pg->buttonList.push_back(new Button(L"MIDI Pitch", L"MidiPitch"));
	pg->buttonList.push_back(new Button(L"Inst. Harmonics", L"InstHarm"));

	Button::NextColumn(40, 130);
	pg->buttonList.push_back(new Button(L"Read Text File", L"ReadTxFile"));
	pg->buttonList.push_back(new Button(L"Read Data File", L"ReadDatFile"));
	pg->buttonList.push_back(new Button(L"Random Read File", L"RandReadFile"));
	pg->buttonList.push_back(new Button(L"Write Text File", L"WriteTxFile"));
	pg->buttonList.push_back(new Button(L"Write Data File", L"WriteDatFile"));
	pg->buttonList.push_back(new Button(L"Random Write File", L"RandWriteFile"));
	pg->buttonList.push_back(new Button(L"Get File Info", L"GetFileInfo"));
	pg->buttonList.push_back(new Button(L"Delete File", L"DelFile"));
	pg->buttonList.push_back(new Button(L"Get Directory", L"GetDir"));
	pg->buttonList.push_back(new Button(L"Create Folder", L"CreateFldr"));
	//pg->buttonList.push_back(new Button(L"Note", L"Note"));
	pg->buttonList.push_back(new Button(L"GraphBase Words", L"GraphBaseWords"));


	//Button::refDebug = true; // ********************************************************

	// IMAGE BUTTONS ***************************************************************

	Button::Setup(20, 40, 130, 20, 0xFFFFDD, 0);
	pg->buttonGroup.push_back(pg->buttonList.size());

	pg->buttonList.push_back(new Button(L"Camera", L"ImCamera"));
	//pg->buttonList.push_back(new Button(L"Screen Capture", L"Capture"));
	pg->buttonList.push_back(new Button(L"Checkers", L"ImCheckers"));
	pg->buttonList.push_back(new Button(L"Horizontal Stripes", L"ImHStripes"));
	pg->buttonList.push_back(new Button(L"Vertical Stripes", L"ImVStripes"));
	pg->buttonList.push_back(new Button(L"Color Bars", L"ImClrBars"));
	pg->buttonList.push_back(new Button(L"Solid Color", L"ImSolidClr"));
	pg->buttonList.push_back(new Button(L"Color to Grayscale", L"ImColor2Gray"));
	pg->buttonList.push_back(new Button(L"RGB to HSV", L"ImRGB2HSV"));
	pg->buttonList.push_back(new Button(L"HSV to RGB", L"ImHSV2RGB"));
	
	pg->buttonList.push_back(new Button(L"Image Crop", L"ImCrop"));
	pg->buttonList.push_back(new Button(L"Image Insert", L"ImInsert"));
	pg->buttonList.push_back(new Button(L"Image Rotate", L"ImRotate"));
	pg->buttonList.push_back(new Button(L"Image Resize", L"ImResize"));

	Button::NextColumn(40, 135);

	pg->buttonList.push_back(new Button(L"Image Gaussian", L"ImGaussBlur"));
	pg->buttonList.push_back(new Button(L"Image Median Blur", L"ImMedianBlur"));
	pg->buttonList.push_back(new Button(L"Bilateral Filter", L"ImBilatFilt"));

	pg->buttonList.push_back(new Button(L"Edge Enhancer", L"ImEdge"));

	pg->buttonList.push_back(new Button(L"Brightness", L"ImBrightness"));
	pg->buttonList.push_back(new Button(L"Contrast", L"ImContrast"));

	pg->buttonList.push_back(new Button(L"Image Threshold", L"ImThresh"));
	pg->buttonList.push_back(new Button(L"Image Erode", L"ImErode"));
	pg->buttonList.push_back(new Button(L"Image Dilate", L"ImDilate"));

	pg->buttonList.push_back(new Button(L"Chroma Key", L"ImChromaKey"));
	pg->buttonList.push_back(new Button(L"Image Gradient", L"ImGradient"));
	pg->buttonList.push_back(new Button(L"Gray Image Modify", L"ImGrayMod"));
	pg->buttonList.push_back(new Button(L"Gray Colorize", L"ImGrayColor"));
	pg->buttonList.push_back(new Button(L"Color Image Modify", L"ImColorMod"));
	

	Button::NextColumn(40, 125);

	pg->buttonList.push_back(new Button(L"Image Line", L"ImLine"));
	pg->buttonList.push_back(new Button(L"Image Cross", L"ImCross"));
	pg->buttonList.push_back(new Button(L"Image Box", L"ImBox"));
	pg->buttonList.push_back(new Button(L"Image Circle", L"ImCircle"));
	pg->buttonList.push_back(new Button(L"Image Ellipse", L"ImEllipse"));
	pg->buttonList.push_back(new Button(L"Image Arrow", L"ImArrow"));
	pg->buttonList.push_back(new Button(L"Image Polygon", L"ImPolygon"));
	pg->buttonList.push_back(new Button(L"Image Text", L"ImText"));

	pg->buttonList.push_back(new Button(L"Image Multiplexer", L"ImMux"));
	pg->buttonList.push_back(new Button(L"Image Fader", L"ImFader"));
	

	Button::NextColumn(40, 105);

	pg->buttonList.push_back(new Button(L"Segmenter", L"ImSegment"));
	pg->buttonList.push_back(new Button(L"Image Area", L"ImArea"));
	pg->buttonList.push_back(new Button(L"Image Bounds", L"ImBounds"));
	pg->buttonList.push_back(new Button(L"Image Centroid", L"ImCent"));
	pg->buttonList.push_back(new Button(L"Image Angle", L"ImAngle"));
	//pg->buttonList.push_back(new Button(L"Face Detector", L"ImFace"));

	Button::NextColumn(40, 140);

	pg->buttonList.push_back(new Button(L"Color Image Display", L"ImDisplay"));
	pg->buttonList.push_back(new Button(L"Image Reader", L"ImRead"));
	pg->buttonList.push_back(new Button(L"Image Writer", L"ImWrite"));
	pg->buttonList.push_back(new Button(L"Image to Data[3]", L"ImToData3"));
	pg->buttonList.push_back(new Button(L"Data[3] to Image", L"DataToImage3"));
	pg->buttonList.push_back(new Button(L"Grayscale to Data[1]", L"ImToData1"));
	pg->buttonList.push_back(new Button(L"Data[1] to Grayscale", L"DataToImage1"));
	pg->buttonList.push_back(new Button(L"Image Color Index", L"ImClrIndx"));

	

	// SUBROUTINE BUTTONS ***************************************************************

	pg->buttonGroup.push_back(pg->buttonList.size());

	// first 3 are the main screen buttons
	Button::Setup(20, 40, 260, 20, 0xFF88EE, 0);
	pg->mainNameIndex = pg->buttonList.size();
	pg->buttonList.push_back(new Button(L"Main: <untitled>", L"Main"));
	Button::NextColumn(40, 60);
	pg->buttonList.push_back(new Button(L"Edit", L"MainEdit"));
	Button::NextColumn(40, 70+80);
	pg->buttonList.push_back(new Button(L"Reload All Subs", L"Reload"));

	// then the array of subroutine buttons
	Button::Setup(20, 60, 260, 20, 0xFF88EE, 0);
	pg->subroutineNameIndex = pg->buttonList.size();
	string title;
	for (int j = 0; j < SUBROUTINE_ROWS; j++) {
		title = "Sub " + tomString(j + 1) + ": <untitled>";
		pg->buttonList.push_back(new Button(ConvertToLPWSTR(title), L"SubName"));
	}
	Button::NextColumn(60, 60);
	for (int j = 0; j < SUBROUTINE_ROWS; j++) {
		title = "Edit " + tomString(j + 1);
		pg->buttonList.push_back(new Button(ConvertToLPWSTR(title), L"SubEdit"));
	}

	Button::NextColumn(60, 70);
	for (int j = 0; j < SUBROUTINE_ROWS; j++) {
		title = "Place " + tomString(j + 1);
		pg->buttonList.push_back(new Button(ConvertToLPWSTR(title), L"SubPlace"));
	}

	Button::NextColumn(60, 80);
	for (int j = 0; j < SUBROUTINE_ROWS; j++) {
		title = "Delete " + tomString(j + 1);
		pg->buttonList.push_back(new Button(ConvertToLPWSTR(title), L"SubDelete"));
	}

	pg->buttonGroup.push_back(pg->buttonList.size());

	buttonCount = pg->buttonList.size();

	// ****** dim the unused component buttons and color it yellow when the detail entry is not found *********
#if defined _DEBUG

	// mark all details unused
	markAllDetailsUnused();
	

	for (int j = firstComponentButton; j < buttonCount; j++) {
		if (pg->buttonList[j]->isCheckbox)
			continue;

		Component *comp = buildComponent(pg, pg->buttonList[j]->componentName, 0);
		if (comp == NULL)
			pg->buttonList[j]->dim = true;

		LPCTSTR lstring = lookupDetail(pg->buttonList[j]->componentName);
		if (lstring == L"NOT FOUND")
			pg->buttonList[j]->backgroundColor = 0x88FFFF; // somehow this makes the background yellow
		else {
			markDetailUsed(pg->buttonList[j]->componentName);

			if (comp != NULL) {
				if (!z(pg->buttonList[j]->visibleText, comp->label))
					int k = 0; // debug point only

				string theDetail = convertLPCTSTR_toString(lstring);
				string theLabel = convertLPCTSTR_toString(comp->label);
				int result = theDetail.find(theLabel);
				if( result == -1)
					int k = 1;// debug point only
			}
		}
		pg->isDirty = false; // this code sets it, we need to clear it

		if (comp != NULL)
			delete comp;
	}

	lookupUnusedDetails();

	isL[0]->saveDiagramBuffer.clear();
	isL[0]->saveIndex = -1;
#endif

	// initialize the ring buffer
	
	setVisibleScreenDirty("Initialize");
	
	isL[0]->isDirty = false;
	pg->isDirty = false;
}

void dimButtonsBasedKeySearch(Sheet* pg, string keySearchText) {
	toUpper(keySearchText);
	int keySearchTextLen = keySearchText.size();

	// first dim buttons in all groups
	for (int j = 0; j < pg->buttonList.size(); j++) {
		if (!pg->buttonList[j]->isCheckbox) {
			string UpperCaseText = convertLPCTSTR_toString(pg->buttonList[j]->visibleText);
			toUpper(UpperCaseText);
			std::size_t found = UpperCaseText.find(keySearchText);
			if (found != std::string::npos)
				pg->buttonList[j]->dim = false;
			else
				pg->buttonList[j]->dim = true;
		}
	}
	// then if all the buttons are dim in a group, dim the group heading

	for (int group = 1; group < 6; group++) {
		bool allDim = true;
		for (int j = pg->buttonGroup[group]; j < pg->buttonGroup[group + 1]; j++) {
			if (!pg->buttonList[j]->dim) {
				allDim = false;
				break;
			}
		}
		if( allDim )
			pg->frameTopButtons[group]->dim = true;
		else
			pg->frameTopButtons[group]->dim = false;
	}

}

void undimAllButtons(Sheet* pg) {
	for (int j = 0; j < pg->buttonList.size(); j++)
		pg->buttonList[j]->dim = false;

	for (int j = 1; j < 6; j++)
		pg->frameTopButtons[j]->dim = false;
}

void updateSheetButtonText(Sheet* pg, int sheet, string theName) {
	int firstIndex = pg->buttonGroup[6];
	int index = firstIndex + 2 + sheet;
	pg->buttonList[index]->visibleText = ConvertToLPWSTR(theName);
}

void drawGroupButtons(Sheet* pg, HDC hDC) {
	
	if (pg->visibleButtonGroup < 0)
		return;

	int firstIndex = pg->buttonGroup[pg->visibleButtonGroup];
	int lastIndex = pg->buttonGroup[pg->visibleButtonGroup + 1];

	if (pg->visibleButtonGroup == 0) {
		int vs = page->visibleSheet;
		
		string undoText = "Undo (" + tomString(isL[vs]->saveIndex) + ")";
		pg->buttonList[undoButtonIndex]->visibleText = ConvertToLPWSTR(undoText);

		int size = isL[vs]->saveDiagramBuffer.size();
		string redoText = "Redo (" + tomString(size-isL[vs]->saveIndex-1) + ")";
		pg->buttonList[undoButtonIndex+1]->visibleText = ConvertToLPWSTR(redoText);
	}

	// scan the buttons for range and background size
	int leftFrame = 10000;
	int rightFrame = -10000;
	int topFrame = 10000;
	int bottomFrame = -10000;

	for (int j = firstIndex; j < lastIndex; j++) {
		if (pg->buttonList[j]->active) {
			if (pg->buttonList[j]->x < leftFrame)
				leftFrame = pg->buttonList[j]->x;
			if (pg->buttonList[j]->x + pg->buttonList[j]->width > rightFrame)
				rightFrame = pg->buttonList[j]->x + pg->buttonList[j]->width;

			if (pg->buttonList[j]->y < topFrame)
				topFrame = pg->buttonList[j]->y;
			if (pg->buttonList[j]->y + pg->buttonList[j]->height > bottomFrame)
				bottomFrame = pg->buttonList[j]->y + pg->buttonList[j]->height;
		}
	}
	
	// add the background frame
	HBRUSH hbrush = GetColorBrush(RGB(240, 240, 240)); 
	RECT aRect;
	aRect.left = leftFrame - 5;
	aRect.top = topFrame - 5;
	aRect.right = rightFrame - leftFrame + 25;
	aRect.bottom = bottomFrame - topFrame + 45;
	FillRect(hDC, &aRect, hbrush);
		
	hbrush = GetColorBrush(RGB(0, 0, 0)); 
	FrameRect(hDC, &aRect, hbrush);

	// ok now draw the buttons
	if (pg->visibleButtonGroup != 6) {
		for (int j = firstIndex; j < lastIndex; j++)
			if (pg->buttonList[j]->active)
				pg->buttonList[j]->Draw(hDC);
	}
	else {
		int j = firstIndex;
		// the top row for the main buttons
		for (int k = 0; k < 3; k++) 
			pg->buttonList[j++]->Draw(hDC);

		// first column for the subroutine NAMES
		for (int k = 0; k < SUBROUTINE_ROWS; k++) 
			pg->buttonList[j++]->Draw(hDC);

		// second column to EDIT the subroutines
		for (int k = 0; k < SUBROUTINE_ROWS; k++)
			pg->buttonList[j++]->Draw(hDC);

		// third column to PLACE the sheet --  and have some components
		bool show;
		for (int k = 0; k < SUBROUTINE_ROWS; k++) {
			show = true;
			if (k < pg->visibleSheet) { // can only place subroutines that are after the visible one
				show = false;
			}else{ // and must have some code
				if (k + 1 < isL.size()){
					if (isL[k + 1]->dgrm.cL.size() == 0)
						show = false;
				}else
					show = false;
			}
			pg->buttonList[j]->active = show;
			pg->buttonList[j]->dim = !show;
			pg->buttonList[j++]->Draw(hDC);
		}

		// fourth column is the delete
		syncVisibleSheet(pg);// put back current visible sheet
		for (int k = 0; k < SUBROUTINE_ROWS; k++) {
			show = true;
			// can only delete subroutines that have code
			if (k + 1 < isL.size()) {
				if (isL[k + 1]->dgrm.cL.size() == 0)
					show = false;
			}else
				show = false;
			pg->buttonList[j]->active = show;
			pg->buttonList[j]->dim = !show;
			pg->buttonList[j++]->Draw(hDC);
		}
	}
	//ctx.scale(page->cs, page->cs);
}

void deactivateAllButtons(Sheet *pg) {
	for (int j = 0; j < buttonCount; j++) {
		pg->buttonList[j]->active = false;
	}

	// including the dialog buttons
	//activeDialogpg->buttonList;
	pg->activeDialogComponent = -1;
	pg->modalDialogComponent = -1;
	pg->visibleButtonGroup = -1;
}


void activateButtonGroup(Sheet* pg, int group)
{
	if (group == pg->frameTopButtons.size()-2) { // find button was hit
		if (pg->visibleButtonGroup != -1)
		{
			// but first deactivate any visible group
			for (int i = pg->buttonGroup[pg->visibleButtonGroup]; i < pg->buttonGroup[pg->visibleButtonGroup + 1]; i++)
				pg->buttonList[i]->active = false;
			pg->visibleButtonGroup = -1;
		}
		buildFindComponentDialogButtons(pg);
		return;
	}
	if (group == pg->frameTopButtons.size()-1) { // help button was hit
		if (pg->visibleButtonGroup != -1)
		{
			// but first deactivate any visible group
			for (int i = pg->buttonGroup[pg->visibleButtonGroup]; i < pg->buttonGroup[pg->visibleButtonGroup + 1]; i++)
				pg->buttonList[i]->active = false;
			pg->visibleButtonGroup = -1;
		}
		buildHelpComponentDialogButtons(pg);
		return;
	}

	// if this group is visible - deactivate it - ignore active input
	if (group == pg->visibleButtonGroup) {
		for (int i = pg->buttonGroup[group]; i < pg->buttonGroup[group + 1]; i++)
			pg->buttonList[i]->active = false;
		pg->visibleButtonGroup = -1;
		return;
	}

	if (pg->visibleButtonGroup != -1)
	{
		// but first deactivate any visible group
		for (int i = pg->buttonGroup[pg->visibleButtonGroup]; i < pg->buttonGroup[pg->visibleButtonGroup + 1]; i++)
			pg->buttonList[i]->active = false;
	}

	// finally - activate this group
	for (int i = pg->buttonGroup[group]; i < pg->buttonGroup[group + 1]; i++)
		pg->buttonList[i]->active = true;
	pg->visibleButtonGroup = group;
	
}




int checkGroupButtons(Sheet *pg, POS rawpos) {
	if (pg->visibleButtonGroup == -1)
		return -1;

	int firstIndex = pg->buttonGroup[pg->visibleButtonGroup];
	int lastIndex = pg->buttonGroup[pg->visibleButtonGroup + 1];

	// first check the subroutine buttons
	if (pg->visibleButtonGroup == 6) {

		int buttonsInGroup = lastIndex - firstIndex - 3; // 3 extra for main
		
		 // hit the main name
		if (pg->buttonList[firstIndex]->checkForMouseHit(rawpos)) {
			buildSubroutineNameDialogButtons(pg, 0); // rename the main
			return -3;
		}
		firstIndex++;

		if (pg->buttonList[firstIndex]->checkForMouseHit(rawpos)) {
			switchToSheet(pg, 0); // switch to main
			return -2;
		}
		firstIndex++; // skip the main "Edit"

		if (pg->buttonList[firstIndex]->checkForMouseHit(rawpos)) {
			reloadAllSubroutines(); // update
			return -2;
		}
		firstIndex++; // skip the "update all subs"

		for (int j = firstIndex; j < lastIndex; j++) {
			if (pg->buttonList[j]->active) {
				if (pg->buttonList[j]->checkForMouseHit(rawpos)) {
					int col = (j - firstIndex) / SUBROUTINE_ROWS;
					int row = (j - firstIndex) % SUBROUTINE_ROWS;
					int sheetIndex = row + 1;

					switch (col) {
					case 0:	// set name
						// create some if needed
						while (sheetIndex >= isL.size()) {
							InnerSheet* inner = new InnerSheet();
							isL.push_back(inner);
						}
						buildSubroutineNameDialogButtons(pg, sheetIndex);
						return -3;
					case 1:	// edit
						switchToSheet(pg, sheetIndex);
						return -2;
					case 2:	// place
						if (pg->buttonList[j]->dim)
							break;
						pg->modalDialogComponent = -1;
						POS pos;
						pos.x = pg->buttonList[j]->x + pg->buttonList[j]->width / 2;
						pos.y = pg->buttonList[j]->y + pg->buttonList[j]->height / 2;
						pg->mouseOffsetX = pos.x;
						pg->mouseOffsetY = pos.y;
						AddComponent(pg, L"Sheet", pos, sheetIndex);
						drawCanvas();
						return -2;
					case 3:	// delete
						if (pg->buttonList[j]->dim)
							break;
						if( sheetIndex == pg->visibleSheet)
							switchToSheet(pg, 0);

						// delete the subroutine components
						for (int i = 0; i < isL[sheetIndex]->dgrm.cL.size(); i++) {
							delete(isL[sheetIndex]->dgrm.cL[i]);
						}
						isL[sheetIndex]->dgrm.cL.clear();

						// and wires
						for (int i = 0; i < isL[sheetIndex]->dgrm.wL.size(); i++)
							delete isL[sheetIndex]->dgrm.wL[i];
						isL[sheetIndex]->dgrm.wL.clear();

						isL[sheetIndex]->name = "";
						int butIdx = pg->subroutineNameIndex + sheetIndex - 1;
						pg->buttonList[butIdx]->visibleText = ConvertToLPWSTR(
							"Sub " + tomString(sheetIndex) + ": " );
						
						return -2;
					}
				}
			}
		}
		return -2;
	}

	// then check the other buttons
	for (int j = firstIndex; j < lastIndex; j++) {
		if (pg->buttonList[j]->active) {
			if (pg->buttonList[j]->checkForMouseHit(rawpos)) {
				if (pg->buttonList[j]->action != NULL) {
					pg->buttonList[j]->action();
					return -2;
				}
				else if (pg->buttonList[j]->actionPage != NULL) {
					pg->buttonList[j]->actionPage(pg);
					return -2;
				}
				else
					return AddComponent(pg, pg->buttonList[j]->componentName, rawpos, pg->visibleSheet);
			}
		}
	}
	return -1;
}

int checkGroupButtonsAndShowDetails(Sheet* pg, POS rawpos) {

	// check the frame buttons
	for (int j = 0; j < pg->frameTopButtons.size(); j++) {
		if (pg->frameTopButtons[j]->checkForMouseHit(rawpos)) {
			buildDetailsDialogButtonsFromComponentType(pg, pg->frameTopButtons[j]->componentName);
			drawCanvas();
			return j;
		}
	}
	for (int j = 0; j < pg->frameBottomButtons.size(); j++) {
		if (pg->frameBottomButtons[j]->checkForMouseHit(rawpos)) {
			buildDetailsDialogButtonsFromComponentType(pg, pg->frameBottomButtons[j]->componentName);
			drawCanvas();
			return j;
		}
	}

	if (pg->visibleButtonGroup == -1)
		return -1;

	// lets be efficient
	int firstIndex = pg->buttonGroup[pg->visibleButtonGroup];
	int lastIndex = pg->buttonGroup[pg->visibleButtonGroup + 1];

	for (int j = firstIndex; j < lastIndex; j++) {
		if (pg->buttonList[j]->checkForMouseHit(rawpos)) {
			deactivateAllButtons(pg);
			buildDetailsDialogButtonsFromComponentType(pg, pg->buttonList[j]->componentName);
			drawCanvas();
			return j;
		}
	}

	return -1;
}





COLORREF GetButtonBackgroundColor(Sheet *pg, LPCTSTR componentName) {

	for (int j = 0; j < buttonCount; j++)
		if (pg->buttonList[j]->componentName == componentName)
			return pg->buttonList[j]->backgroundColor;
	return 0xFF88FF; // red
}
