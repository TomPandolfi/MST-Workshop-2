#pragma once

#include "buildComponent.h"

extern int bitSetArray[16];
extern int bitClearArray[16];

extern double xTo2dCanvas(Sheet* pg, double x);
extern double yTo2dCanvas(Sheet* pg, double y);

std::wstring decodeUTF8(std::string inText);

extern int drawMainWindowCounter;
extern int drawCanvasWindowCounter;
extern int analyzeCounter;
extern int updateCounter;

void MouseDownCanvasListener(Sheet* pg, LPARAM lParam);
void MouseMoveCanvasListener(Sheet* pg, LPARAM lParam);
void MouseUpCanvasListener(Sheet* pg, LPARAM lParam);
void MouseUpDownCanvasButtonListener(Sheet* pg, int buttonIndex, bool down);
void MouseWheelCanvasButtonListener(Sheet* pg, int zDelta);
void drawAxis(Sheet*);

double sizeTo2dCanvas(Sheet* pg, double inputSize, int displayMode);
double xTo2dCanvas2(Sheet* pg, double x, int displayMode);
double yTo2dCanvas2(Sheet* pg, double y, int displayMode);

class Canvas_Component : public Component { // method is special for fractions
public:	Canvas_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	virtual int method2(Sheet*);
	void getPositionGroup(int group, int* pinptr, int k);
	double inputX;
	double inputY;
	void getSizeGroup(int group, int* pinptr, int k, double def);
	double inputSizeX;
	double inputSizeY;
	void getColorLineGroup(int group, int* pinptr, int k);
	int	inputLineRed;
	int	inputLineGreen;
	int	inputLineBlue;
	void getColorFillGroup(int group, int* pinptr, int k);
	int	inputFillRed;
	int	inputFillGreen;
	int	inputFillBlue;
};





class Canvas_BOX_Component :			
public Canvas_Component { public:	Canvas_BOX_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_CIRCLE_Component :		
public Canvas_Component { public:	Canvas_CIRCLE_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_LINES_Component :	public Canvas_Component { 
public:	Canvas_LINES_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_LINE_Component :	public Canvas_Component { 
public:	Canvas_LINE_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_CROSS_Component :		public Canvas_Component { 
public:	Canvas_CROSS_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_MARKER_Component :		public Canvas_Component { 
public:	Canvas_MARKER_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_ARROW_Component :		public Canvas_Component { 
public:	Canvas_ARROW_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_VerticalSlider_Component :	public Component {
public:	Canvas_VerticalSlider_Component();
		~Canvas_VerticalSlider_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
};
class Canvas_HorizontalSlider_Component :	public Component { 
public:	Canvas_HorizontalSlider_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
};
class Canvas_RadioButton_Component :	public Component { 
public:	Canvas_RadioButton_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_Checkbox_Component :	public Component { 
public:	Canvas_Checkbox_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_Polygon_Component :		public Canvas_Component { 
public:	Canvas_Polygon_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_Parabola_Component :		public Canvas_Component { 
public:	Canvas_Parabola_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_Value_Component :		public Component { public:	
	Canvas_Value_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	bool outputChanged;
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Canvas_EllipseOnCenter_Component :		public Canvas_Component {
public:	Canvas_EllipseOnCenter_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_EllipseOnFocus_Component :			public Canvas_Component {
public:	Canvas_EllipseOnFocus_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_RoundButton_Component :	public Component {
public:	Canvas_RoundButton_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
};
class Canvas_RoundedRectangleButton_Component :	public Component {
public:	Canvas_RoundedRectangleButton_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_Vector_Component :	public Component { public:	Canvas_Vector_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Canvas_TextBlock_Component :		public Component {
public:	Canvas_TextBlock_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_AngledSlider_Component :	public Component { 
public:	Canvas_AngledSlider_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
};
class Canvas_ArcSlider_Component :	public Component { 
public:	Canvas_ArcSlider_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
};
class Canvas_Knob_Component :	public Component { 
public:	Canvas_Knob_Component();  
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
};
class Canvas_LinearSlider_Component :	public Component { 
public:	Canvas_LinearSlider_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class SnapPoint2_Component :		public Component { 
public:	SnapPoint2_Component();	
	int method2(Sheet*);
	bool snapPoint2Test(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
};
class Canvas_Geometry_Component :		public Component { 
public:	Canvas_Geometry_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_TextSpinner_Component :	public Component { 
public:	Canvas_TextSpinner_Component();
	int method2(Sheet*);
};
class Canvas_NumberSpinner_Component :	public Component { 
public:	Canvas_NumberSpinner_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void onMouseUp2d(Sheet*);
	vector<int> chevronCenterX; // set by the dial routine
	vector<int> chevronStepX;
	vector<int> chevronMinY; // then read by the crank routine
	vector<int> chevronMaxY;
	vector<int> chevronCount;
	vector<double> initialMemory;
	int swippingChevron = 0;
	int swippingSize;
	int swippingEffect;
	int swippingDelta;
	vector<double> swippingMinimum;
	vector<double> swippingMaximum;
	double swippingScaleFactor;
	int swippingMouseDownY = 0;   // needed for new Swipping
	double swippingInitialValue = 0; // needed for new Swipping
	int swipping = -1;
	string genNumberString(Sheet* pg, int j, int decimal, bool stripSign, bool trimZeros);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Canvas_Spinner_Component :		public Component { 
public:	Canvas_Spinner_Component(); 
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	void onMouseUp2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_Mouse_Component : public Component {
public:	Canvas_Mouse_Component();
	bool canvasMouseMoved(Sheet* pg, int x, int y, bool *mouseDown, int zDelta);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_SinCosKnob_Component : public Component {
public:	Canvas_SinCosKnob_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_HorizontalAxis_Component : public Canvas_Component {
public:	Canvas_HorizontalAxis_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_VerticalAxis_Component : public Canvas_Component {
public:	Canvas_VerticalAxis_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_ViewControl_Component : public Component {
public:	Canvas_ViewControl_Component();
		~Canvas_ViewControl_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Debug_Component : public Component {
public:	Debug_Component();
	int method2(Sheet*);
	int old_drawMainWindowCounter;
	int old_drawCanvasWindowCounter;
	int old_analyzeCounter;
	int old_updateCounter;
};

class Canvas_ArraySlider_Component : public Component {
	public:	Canvas_ArraySlider_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool crank2d(Sheet*, int, int, bool);
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_XyAxis_Component : public Canvas_Component {
	public:	Canvas_XyAxis_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class TicAndLabel_Component : public Component {
public:	TicAndLabel_Component();
	int method2(Sheet*);
};

class Canvas_Meter_Component : public Canvas_Component {
	public:	Canvas_Meter_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_Chart_Component : public Canvas_Component {
public:	Canvas_Chart_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class Canvas_XyChart_Component : public Canvas_Component {
public:	Canvas_XyChart_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_FieldPlot_Component : public Canvas_Component {
public:	Canvas_FieldPlot_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Canvas_Bitmap_Component : public Canvas_Component {
public:	Canvas_Bitmap_Component();
		~Canvas_Bitmap_Component();
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	LPWSTR pathLoaded;
	HBITMAP hBitmapLoaded;
};
