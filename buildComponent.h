#pragma once
#include "structs.h"
#include <vector>
#include <string>
#include "Sheet.h"
#include "Button.h"

using namespace std;

class Sheet;
class Diagram;

class InputClass {
public:
	std::vector<double> value;         // these are the vectors of values for each input pin
	std::vector<int> dimensions;	// these are the vectors of dimensions for each input pin (new)
	int drivenByComponent;
	int drivenByOutputTerminal;
	string label;
	bool stringType = false;
	std::vector<std::string> stringValue;
	~InputClass();
};

class OutputClass {
public:
	std::vector<double> value;
	std::vector<int> dimensions;
	string label;
	bool stringType = false;
	std::vector<std::string> stringValue;
	~OutputClass();
};

class Component {
public:
	int x;
	int y;
	int width;
	int height;
	int ghost;
	LPCTSTR type = L"";
	LPCTSTR label;
	string userLabel = "";
	
	bool selected = false;
	//int attributionKey = 0;
	int displayMode = 0; // 0=on canvas; 1=pixels on glass
	COLORREF fillColor;

	int leftTerminalCount;
	int rightTerminalCount;
	int leftTerminalStep;
	int rightTerminalStep;

	void setLeftLabels(int count, ...);
	void setRightLabels(int count, ...);
	void setSize(int wid, int hgt);
	double getDoubleInputOrZero(int pin, int j);

	void buildPinLabels();
	virtual vector<int> GetPinGroups();
	virtual vector<string> GetPinLabels();
	int pinMaskIndex;
	bool timeBased;

	std::vector<InputClass> input;         // these are the vectors of values for each input pin
	std::vector<OutputClass> output;
	std::vector<double> setting;
	std::vector<double> memory;
	std::vector<std::string> literal;

	std::vector<double> element;
	std::vector<double> initialMemoryX;
	std::vector<double> initialMemoryY;
	std::vector<std::vector<double>> vectorMemoryX;
	std::vector<std::vector<double>> vectorMemoryY;

	int shapePath;

	// lets go virtual 
	virtual int method2(Sheet*);	//  OUTPUT_CHANGED 1, REDRAW_CANVAS2 2
	virtual void dial(Sheet*);
	virtual void onMouseUp(Sheet*);
	virtual bool crank(Sheet*, int, int, bool);
	virtual bool onLoad(Sheet*);
	virtual double innerMethodN(double*);					// FunctionN_Component -- inputRepeaterNew_
	virtual double innerMethodCN(Component, double*);		// FunctionCN_Component -- inputRepeaterWithComp_
	virtual double innerMethodNMO(Sheet*, double*, double*, double);// FunctionNMO_Component -- inputRepeaterWithOutput_

	virtual bool dial2d(Sheet*);
	virtual bool crank2d(Sheet*, int, int, bool);
	virtual void onMouseUp2d(Sheet*);
	virtual bool canvasMouseMoved(Sheet* pg, int x, int y, bool mouseDown[5], int zDelta);

//	double(*innerMethodCJN)(Component*, int, double*) = innerMethodCJN; // inputRepeaterWithCompIndex_
//	void(*innerMethod2d)(Sheet* pg, int i, int j, double *a) = innerMethod2d;

	virtual void new3dOjbect();

	Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	Component(LPCTSTR _type, LPCTSTR _label, int LTC, int RTC);
	virtual ~Component();

	Component(Component*);

	void setPosition(int xin, int yin);
	void shiftPosition(int dx, int dy);
	void draw(Sheet*, HDC hDC);
	void buildInputPins(int pins);
	void buildOutputPins(int pins);
	void setSetting(int index, double value);
	void setOutput(int pin, int index, double value);
	void setMemory(int index, double value);
	void setElement(int index, double value);
	void setLiteral(int index, std::string value);
	bool crankResize(int mouseX, int mouseY, bool check);
	bool crankResizeHoldLeft(int mouseX, int mouseY, bool check);

	double detent(int detentIndex, double valueIndex);

	int inputsToLoopsN(int n);
	int inputsToLoops();
	int inputsToLoopsSN(int start, int n);

	double getLoopingValueD(int pin, int loop, double defValue);
	double getLoopingValueD(int group, int *pin, int loop, double defValue);
	int getLoopingValueIntPositiveLimit(int pin, int loop, int defValue, int limit);
	int getLoopingValueIntPositiveLimit(int group, int* pin, int loop, int defValue, int limit);
	int getLoopingValueColor(int group, int* pin, int loop);
	int getLoopingValueColorSplit(int group, int* pin, int loop);
	std::string getLoopingStringValueD(int pin, int loop, std::string defValue);
	void setComponentWidth();
	double rotateInputAngle(double inAngle, bool inNotEquals = false, bool inPlus = false);
	double rotateOutputAngle(double inAngle, bool inNotEquals = false, bool inPlus = false);

	std::string toString();
//	void setInputDimension(int pin, int dim, int size);
	void setOutputDimension(int pin, int dim, int size);

	void copyDimensions(int it, int ot);
	bool clearOutputs();
	bool checkValueInputs();
	bool checkStringInputs();
	bool checkValueAndStringInputs();
	bool checkMatrixInput(int pin);
	void setSimpleOutputDimensions();

	void pushInToOut(int out_pin, int in_pin, int in_index);
	int getInputSize(int in_pin);

	int leftTerminalX(int it);
	int leftTerminalY(int it);
	int rightTerminalX(int it);
	int rightTerminalY(int it);

	std::vector<Button*> dialogButtonList;

	int completionTestDoubles = 0;
	int completionTestChecks = 0;
	int completionTestRadios = 0;
	virtual void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
	int rotation = 0;

	
};


void deleteComponent(Sheet* pg, int i);
void resetSheets();


class FunctionN_Component : public Component { // use: innerMethodN
public:	FunctionN_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	virtual int method2(Sheet*);
};
class FunctionCN_Component : public Component { // use: innerMethodCN
public:	FunctionCN_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	virtual int method2(Sheet*);
	virtual void dial(Sheet*);
};
class FunctionNMO_Component : public Component { // use: innerMethodCN
public:	FunctionNMO_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	virtual int method2(Sheet*);
	//virtual void dial(Sheet*);
};



class Fractions_Component : public Component { // method is special for fractions
public:	Fractions_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
		virtual int method2(Sheet*);
};










void unselectAllItems(Sheet* pg);

void buildSheets(HWND hWnd, int temp);

Component* buildComponent(Sheet* pg, LPCTSTR name, int sheetNum);
string sheetToText(int sheetNum);

int AddComponent(Sheet* pg, LPCTSTR name, POS pos, int sheetNum);
void loadFromString(Sheet* pg, string text, bool justSheet);

//void drawComponents(HDC hDC, int except);
void dragSelection();
void drawCanvasForReal(Sheet* pg, HWND hWnd, HDC hDC, bool printing);
void drawCanvas();
void drawCanvas2();
bool checkForTimeBased(Sheet* pg);
int convertToLocalX(Sheet* pg, int x);
int convertToLocalY(Sheet* pg, int y);
int findObject(Sheet* pg, int x, int y, bool canSeeGhosts);
void dragComponent(Sheet* pg, int component, int dx, int dy);

int getComponentX(Sheet* pg, int i);
int getComponentY(Sheet* pg, int i);



int AddWire(Sheet* pg, int startX, int startY, int endX, int endY);
//void drawWires(HDC hDC, int except);
void dragWire(Sheet* pg, int inx, int iny);
bool destroyShortWire(Sheet* pg);
void finishWire(Sheet* pg, int x, int y);
void destroyWire(Sheet* pg);

bool writeComponentsAndWires(Sheet*);
void writeComponentsAndWires_void(Sheet* pg);
void writeComponentsAndWiresAs_void(Sheet* pg);
string componentsAndWiresToString(Sheet* pg);

void readComponentsAndWires(Sheet*);
void readComponentsAndWiresFromString(Sheet* pg, string theString);
void setTitleFromPathName(Sheet* pg);

void analyzeWires(Diagram* dgrm);
void analyzeConnections(Diagram* dgrm);

void testanalyze();
void remoteAnalyzeConnections();
PATH findWireTerminal(int x, int y);

void propagateSheetValues(Diagram* dgrm);
void propagateValues(Diagram* dgrm);
bool componentIsGhost(int i);
bool componentHasCrank(HDC hDC, int i, int posX, int posY, bool check);
void doCrank(HDC hDC, int i, int posx, int posy, bool check);
void doMouseUp(int i);

POS findComponentTerminal(int posX, int posY);
void showDebugString();

int pinToHeight(int pins);

void drawCanvas2ForReal(HWND hWnd, HDC hDC);

void doubleClickDialog(int i);
void loadComponentButtons(Sheet* pg);

void buttonBase_Delete_Action(Sheet* pg, int i);
void buttonBase_Details_Action(Sheet* pg, int i);
void buttonBase_Duplicate_Action(Sheet* pg, int i);
void buttonBase_EditLabel_Action(Sheet* pg, int i);
void innerComponentCopy(Diagram* src_dgrm, Diagram* dst_dgrm, int isrc, int jdst, int dx, int dy);

void buttonBase_MoveToTop_Action(Sheet* pg, int i);
void buttonBase_MoveUp_Action(Sheet* pg, int i);
void buttonBase_MoveDown_Action(Sheet* pg, int i);
void buttonBase_MoveToBottom_Action(Sheet* pg, int i);
bool checkComponentInputToWire(Diagram* dgrm, int ic, int it, int wn, int wt);
bool checkComponentOutputToWire(Diagram* dgrm, int ic, int it, int wn, int wt);
bool checkOverlap(Diagram* dgrm, int ic, int it, int oc, int ot);

void deleteDoubleBuffer();
bool z(LPCTSTR text1, LPCTSTR text2);


PWSTR get_a_file_Path();
void componentMemoryTest(Sheet* pg);
void componentFastRandomTest(Sheet* pg);
void componentRandomTest(Sheet* pg);
void setScreenSizeYoutube(Sheet* pg);