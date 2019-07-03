#pragma once

#include "buildComponent.h"



bool commonCrankVerticalResize(Sheet*, Component *comp, int mouseX, int mouseY, bool check);
double pickLabelSeparation(double scale);	
void updateAllBuss2Outs(Sheet*);

class Time_Component :		public Component {
public:	Time_Component();
	int method2(Sheet*);
};
class Year_Component :		public Component {
public:	Year_Component();
	int method2(Sheet*);
};
class Month_Component :		public Component {
public:	Month_Component();
	int method2(Sheet*);
};
class Day_Component :		public Component {
public:	Day_Component();
	int method2(Sheet*);
};
class Hour_Component :		public Component {
public:	Hour_Component();
	int method2(Sheet*);
};
class Minute_Component :	public Component {
public:	Minute_Component();
	int method2(Sheet*);
};
class Second_Component :	public Component {
public:	Second_Component();
	int method2(Sheet*);
};

void doubleClickConstant(Sheet* pg, int i);// used when right click on a terminal
class Constant_Component :		public Component {
public:	Constant_Component(); 
int method2(Sheet*);
void dial(Sheet*);
bool crank(Sheet*, int, int, bool);
bool onLoad(Sheet*);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class ConstantFactor_Component :	public FunctionN_Component {
public:	ConstantFactor_Component();
void dial(Sheet*);
bool crank(Sheet*, int, int, bool);
bool onLoad(Sheet*);
double innerMethodN(double *a);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class ConstantOffset_Component :	public FunctionN_Component {
public:	ConstantOffset_Component();
void dial(Sheet*); 
bool crank(Sheet*, int, int, bool);
bool onLoad(Sheet*);
double innerMethodN(double *a);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Sld_Component :		public Component {
public:	Sld_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void onMouseUp(Sheet*);
};
class SldFac_Component :	public Component {
public:	SldFac_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void onMouseUp(Sheet*);
};
class SldOff_Component :	public Component {
public:	SldOff_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void onMouseUp(Sheet*);
};
class Sld2_Component :		public Component {
public:	Sld2_Component(); 
	int method2(Sheet*);
	bool crank(Sheet*, int, int, bool);
	void dial(Sheet*);
	bool onLoad(Sheet*);
	void onMouseUp(Sheet*);
};
class Sld2Fac_Component :	public Component {
public:	Sld2Fac_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void onMouseUp(Sheet*);
};
class Sld2Off_Component :	public Component {
public:	Sld2Off_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void onMouseUp(Sheet*);
};
class DisplayList_Component :		public Component {
public:	DisplayList_Component(); 
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class Grid_Component :		public Component {
public:	Grid_Component(); 
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class Select_Component :	public Component {
public:	Select_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class Deselect_Component :	public Component {
public:	Deselect_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class Sample_Component :	public Component {
public:	Sample_Component();
	int method2(Sheet*);
};
class Hold_Component :		public Component {
public:	Hold_Component(); 
	int method2(Sheet*);
};
class Quad_Component :		public Component {
public:	Quad_Component();
	int method2(Sheet*);
};
class Alternate_Component :		public Component {
public:	Alternate_Component(); 
	int method2(Sheet*);
};

class LED_Component :		public Component { 
public:	LED_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool onLoad(Sheet*);
};
class Switch_Component :	public Component { 
public:	Switch_Component();
void dial(Sheet*);
bool crank(Sheet*, int, int, bool);
bool onLoad(Sheet*);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Button_Component :	public Component { 
public:	Button_Component(); 
void dial(Sheet*);
bool crank(Sheet*, int, int, bool);
bool onLoad(Sheet*);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Geom_Component :		public Component { 
public:	Geom_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Function_Component : public Component {
public:	Function_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class InPoly_Component :	public Component { 
public:	InPoly_Component(); 
	int method2(Sheet*);
};

class ToRGB_Component :		public Component {
public:	ToRGB_Component(); 
	int method2(Sheet*);
};
class ToHSV_Component :		public Component { 
public:	ToHSV_Component(); 
	int method2(Sheet*);
};
class ToHMS_Component :		public Component { 
public:	ToHMS_Component();
	int method2(Sheet*);
};
class ToHM_Component :		public Component { 
public:	ToHM_Component(); 
	int method2(Sheet*);
};
class StrLen_Component :	public Component { 
public:	StrLen_Component(); 
	int method2(Sheet*);
};
class AlphaSort_Component :		public Component { 
public:	AlphaSort_Component(); 
	int method2(Sheet*);
};

class AlphaRank_Component :		public Component {
public:	AlphaRank_Component(); 
	int method2(Sheet*);
};
class UpCase_Component :	public Component {
public:	UpCase_Component();
	int method2(Sheet*);
};
class LoCase_Component :	public Component { 
public:	LoCase_Component();
	int method2(Sheet*);
};
class ToASCII_Component : public Component {
public:	ToASCII_Component();
	int method2(Sheet*);
};
class ToNumber_Component :	public Component { 
public:	ToNumber_Component();
	int method2(Sheet*);
};
class ToString_Component :	public Component { 
public:	ToString_Component();
	int method2(Sheet*);
};
class ToChar_Component : public Component {
public:	ToChar_Component();
	int method2(Sheet*);
};
class SubStr_Component :	public Component { 
public:	SubStr_Component();
	int method2(Sheet*);
};
class Parse_Component :		public Component { 
public:	Parse_Component(); 
	int method2(Sheet*);
};
class CharAt_Component :	public Component { 
public:	CharAt_Component(); 
	int method2(Sheet*);
};
class CharIndex_Component : public Component { 
public:	CharIndex_Component(); 
	int method2(Sheet*);
};

class Knob_Component :		public Component { 
public:	Knob_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class KnobFactor_Component :	public Component {
public:	KnobFactor_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class KnobOffset_Component :	public Component {
public:	KnobOffset_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Meter_Component :		public Component { 
public:	Meter_Component(); 
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Rotate_Component :		public Component {
public:	Rotate_Component();
	int method2(Sheet*);
};
class ToPolar_Component :		public Component {
public:	ToPolar_Component(); 
	int method2(Sheet*);
};
class ToCart_Component :		public Component {
public:	ToCart_Component(); 
	int method2(Sheet*);
};
class Canvas_Length_Component :	public Component { 
public:	Canvas_Length_Component(); 
	int method2(Sheet*);
};
class Canvas_Scale_Component :	public Component { 
public:	Canvas_Scale_Component(); 
	int method2(Sheet*);
};
class Canvas_Delta_Component :	public Component { 
public:	Canvas_Delta_Component(); 
	int method2(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Center2Edge_Component :	public Component {
public:	Center2Edge_Component(); 
	int method2(Sheet*);
};
class Edge2Center_Component :	public Component {
public:	Edge2Center_Component(); 
	int method2(Sheet*);
};
class XyzToLl_Component :		public Component {
public:	XyzToLl_Component(); 
	int method2(Sheet*);
};
class LlToXyz_Component :		public Component {
public:	LlToXyz_Component(); 
	int method2(Sheet*);
};

class BusIn_Component :		public Component {
public:	BusIn_Component(); 
	void dial(Sheet*);
	bool onLoad(Sheet*);
};
class BusOut_Component :		public Component { 
public:	BusOut_Component(string); 
	void dial(Sheet*);
	bool onLoad(Sheet*);
};
class Bus2In_Component :		public Component { 
public:	Bus2In_Component(); 
		~Bus2In_Component();
	void dial(Sheet*);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Bus2Out_Component :		public Component {
public:	Bus2Out_Component(); 
	bool onLoad(Sheet*);
};
//class ValueTerms2_Component :	public Component { 
// public:  ValueTerms2_Component(); };

class ValueTerms_Component :	public Component { 
public:  ValueTerms_Component(); 
	int method2(Sheet*);
};
class CircleCenter_Component :		public Component { 
public:  CircleCenter_Component(); 
	int method2(Sheet*);
};
class FSM_Component :			public Component {
public:  FSM_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool onLoad(Sheet*);
};
class Chart_Component :			public Component { 
public:  Chart_Component(); 
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class XyChart_Component :		public Component { 
public:  XyChart_Component(); 
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Recorder_Component :		public Component { 
public:  Recorder_Component(double time); 
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class XYRecorder_Component :	public Component {
public:  XYRecorder_Component(); 
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class FIFO_Component :			public Component {
public:  FIFO_Component(); 
	int method2(Sheet*);
	void updateOutputFIFO(Sheet* pg);
	bool onLoad(Sheet*);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
//class LIFO_Component :			public Component { 
//public:  LIFO_Component(); 
//	int method2(Sheet*);
//};
class Savings_Component :		public Component { 
public:  Savings_Component(); 
	int method2(Sheet*);
};
class Loan_Component :			public Component { 
public:  Loan_Component(); 
	int method2(Sheet*);
};
class Primes_Component :		public Component {
public:  Primes_Component(); 
	int method2(Sheet*);
};

class ColorPatch_Component : public Component {		
public:  ColorPatch_Component();
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};

class Bezier_Component : public Component {		
public:  Bezier_Component();
	int method2(Sheet*);
	bool onLoad(Sheet*);
};
class BezierDeriv_Component : public Component {	
public:  BezierDeriv_Component();
	int method2(Sheet*);
	bool onLoad(Sheet*);
};

class Simplex_Component : public Component {
public:  Simplex_Component();
	int method2(Sheet*);
	//bool onLoad(Sheet*);
	int phase; // use this for the phase
	int lines;
	vector<bool> shorts; // shorts hold the status of the search. TRUE shows that the item has been evaluated
	vector<double> simp_y;
	vector<vector<double>> simp_p;
	vector<double> simp_pSum;
	bool initialSetup(double func);
	void getPsum();
	void findBestAndWorst();
	int ilo;
	int inhi; // added in javascript
	int ihi; // added in javascript
	bool checkTolerance();
	void amotryA(double fac);
	void amotryB(double ytry);
	void contract();
	void capture(double value);
	bool clear();
	double ySave;
};


class OneEdge_Component : public Component {
public:  OneEdge_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};
class AnyEdge_Component : public Component {
public:  AnyEdge_Component();
	int method2(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool onLoad(Sheet*);
};

class Compare_Component : public Component {
public:  Compare_Component();
	int method2(Sheet*);
};

class Sunrise_Component : public Component {
public:  Sunrise_Component();
	void sunCalc(int julianDay, double lat, double lon, int timeZone);
	int method2(Sheet*);
	double noon;
	double sunrise;
	double sunset;
};

class SunPosition_Component : public Component {
public:  SunPosition_Component();
	void sunPosition(double julianDay, double lat, double lon, int timeZone, double E2);
	int method2(Sheet*);
	double elevation;
	double azimuth;
};

class MoonPhase_Component : public Component {
public:  MoonPhase_Component();
	int method2(Sheet*);
};

class RandPos_Component : public Component {
public:  RandPos_Component();
	int method2(Sheet*);
	bool onLoad(Sheet*);
};

class Multiplexer_Component : public Component {
public:	Multiplexer_Component();
	int method2(Sheet*);
};

class TriangleArea_Component : public Component {
public:	TriangleArea_Component();
	int method2(Sheet*);
}; 

class PointToLine_Component : public Component {
public:	PointToLine_Component();
	int method2(Sheet*);
};

class Input_Component : public Component {
public:	Input_Component(Sheet*);
	void dial(Sheet*);
	bool onLoad(Sheet*);
};
class Output_Component : public Component {
public:	Output_Component(Sheet*);
	void dial(Sheet*);
};

class Sheet_Component : public Component {
public:	Sheet_Component(int sheetNum);
	~Sheet_Component();
	vector<int> pinInputComp;
	vector<int> pinOutputComp;
	Diagram* idgrm; // inner diagram (code)
	string ipg_text;
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool onLoad(Sheet*);
	void dial(Sheet*);
	bool crank(Sheet*, int, int, bool);
	bool crank2d(Sheet*, int, int, bool);
	int innerCrankComponent;
	void loadSheetComponent();
	void setPinsHeightTime();
	void new3dOjbect();
};
class UTF8_Component : public Component {
public:	UTF8_Component();
	bool onLoad(Sheet*);
	void dial(Sheet*);
};
