
#include "stdafx.h"
#include "MathComponents.h"
#include "FunctionComponents.h"
#include "Sheet.h"
#include "Text.h"
#include "ScreenDialog.h"
#include "ArrayComponents.h"
#include "MethodComponents.h"
#include "buildComponent.h"

#pragma warning (disable: 4267)
#pragma warning (disable: 4244)



extern void rotationActionGeneral(Sheet* pg, int i);



double Add_Component::innerMethodN(double *a) { return a[0] + a[1]; }
Add_Component::Add_Component()
	: FunctionN_Component(L"Add", L"Add", triangleShape, 2, 2, 1, 1) {}

double Subtract_Component::innerMethodN(double *a) { return a[0] - a[1]; }
Subtract_Component::Subtract_Component()
	: FunctionN_Component(L"Sub", L"Subtract", triangleShape, 2, 2, 1, 1) {}

double Mul_Component::innerMethodN(double *a) { return a[0] * a[1]; }
Mul_Component::Mul_Component()
	: FunctionN_Component (L"Mul", L"Multiply", triangleShape, 2, 2, 1, 1){ }

double Divide_Component::innerMethodN(double *a) { return a[0] / a[1]; }
Divide_Component::Divide_Component()
	: FunctionN_Component (L"Div", L"Divide", triangleShape, 2, 2, 1, 1){}

double Inv_Component::innerMethodN(double *a) { return 1.0 / a[0]; }
Inv_Component::Inv_Component()
	: FunctionN_Component (L"Inv", L"Invert", triangleShape, 1, 1, 1, 1){}

double Neg_Component::innerMethodN(double *a) { return -a[0]; }
Neg_Component::Neg_Component()
	: FunctionN_Component (L"Neg", L"Negate", triangleShape, 1, 1, 1, 1){	}

double Floor_Component::innerMethodN(double *a) { return floor(a[0]); }
Floor_Component::Floor_Component()
	: FunctionN_Component (L"Floor", L"Floor", triangleShape, 1, 1, 1, 1){}

double Ceiling_Component::innerMethodN(double *a) { return ceil(a[0]); }
Ceiling_Component::Ceiling_Component()
	: FunctionN_Component (L"Ceil", L"Ceiling", triangleShape, 1, 1, 1, 1){}

double Exp_Component::innerMethodN(double *a) { return exp(a[0]); }
Exp_Component::Exp_Component()
	: FunctionN_Component (L"Exp", L"Exp.", triangleShape, 1, 1, 1, 1){}

double Pow_Component::innerMethodN(double *a) { return pow(a[0], a[1]); }
Pow_Component::Pow_Component()
	: FunctionN_Component (L"Pow", L"Power", triangleShape, 2, 2, 1, 1){}

double Log_Component::innerMethodN(double *a) { return log(a[0]); }
Log_Component::Log_Component()
	: FunctionN_Component (L"Log", L"Log", triangleShape, 1, 1, 1, 1){}

double Sign_Component::innerMethodN(double *a) {
	if (a[0] > 0.0)
		return 1.0;
	if (a[0] < 0.0)
		return -1.0;
	return 0.0;
}
Sign_Component::Sign_Component()
	: FunctionN_Component (L"Sign", L"Sign", triangleShape, 1, 1, 1, 1){}

double Ave2_Component::innerMethodN(double *a) { return (a[0] + a[1]) / 2.0; }
Ave2_Component::Ave2_Component()
	: FunctionN_Component (L"Ave2", L"Ave", triangleShape, 2, 2, 1, 1){}

double Abs_Component::innerMethodN(double *a) { return fabs(a[0]); }
Abs_Component::Abs_Component()
	: FunctionN_Component (L"Abs", L"Abs", triangleShape, 1, 1, 1, 1){}

double Min2_Component::innerMethodN(double *a) { return min(a[0], a[1]); }
Min2_Component::Min2_Component()
	: FunctionN_Component (L"Min2", L"Min", triangleShape, 2, 2, 1, 1){}

double Max2_Component::innerMethodN(double *a) { return max(a[0], a[1]); }
Max2_Component::Max2_Component()
	: FunctionN_Component (L"Max2", L"Max", triangleShape, 2, 2, 1, 1){}

double Mod_Component::innerMethodN(double *a) {
	if (a[1] == 0)
		return 0;
	double temp1 = a[0] / a[1];
	double temp2 = floor(temp1);
	return (temp1 - temp2)* a[1];
}
Mod_Component::Mod_Component()
	: FunctionN_Component (L"Mod", L"Modulo", triangleShape, 2, 2, 1, 1){}

double SquareRoot_Component::innerMethodN(double *a) { return sqrt(a[0]); }
SquareRoot_Component::SquareRoot_Component()
	: FunctionN_Component (L"Sqrt", L"Sqrt", triangleShape, 1, 1, 1, 1){}

double Square_Component::innerMethodN(double *a) { return a[0] * a[0]; }
Square_Component::Square_Component()
	: FunctionN_Component (L"Square", L"Square", triangleShape, 1, 1, 1, 1){}

double Cube_Component::innerMethodN(double *a) { return a[0] * a[0] * a[0]; }
Cube_Component::Cube_Component()
	: FunctionN_Component (L"Cube", L"Cube", triangleShape, 1, 1, 1, 1){}


//********************* trig ****************************

void buttonTrigDialog_Completion(Sheet* pg, int i, double *doubleValues, bool *boolValues, int radioValue) {
	pg->lastTrigMemory = boolValues[0];

	if ((bool)pg->dgrm.cL[i]->setting[0] != boolValues[0]) {
		pg->dgrm.cL[i]->setting[0] = boolValues[0];
		propagateValues(&pg->dgrm);
		drawCanvas();
	}
}
void doubleTrigDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels{ "Degrees" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->setting[0] };

	vector<string> radioLabels;
	int radioIndex = 0;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex,
		buttonTrigDialog_Completion);
}

std::vector<Button*>trigButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleTrigDialog) };


double Sine_Component::innerMethodCN(Component comp, double *a) {
	if (comp.setting[0])
		a[0] *= DEGREES_TO_RADIANS;
	return sin(a[0]);
}
Sine_Component::Sine_Component(Sheet* pg)
	: FunctionCN_Component(L"Sin", L"Sine", triangleShape, 1, 1, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}

double Cosine_Component::innerMethodCN(Component comp, double *a) {
	if (comp.setting[0])
		a[0] *= DEGREES_TO_RADIANS;
	return cos(a[0]);
}
Cosine_Component::Cosine_Component(Sheet* pg)
	: FunctionCN_Component(L"Cos", L"Cosine", triangleShape, 1, 1, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}

double Tan_Component::innerMethodCN(Component comp, double *a) {
	if (comp.setting[0])
		a[0] *= DEGREES_TO_RADIANS;
	return tan(a[0]);
}
Tan_Component::Tan_Component(Sheet* pg)
	: FunctionCN_Component(L"Tan", L"Tangent", triangleShape, 1, 1, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}

double ArcSin_Component::innerMethodCN(Component comp, double *a) {
	double x = asin(a[0]);
	if (comp.setting[0])
		return RADIANS_TO_DEGREES * x;
	return x;
}
ArcSin_Component::ArcSin_Component(Sheet* pg)
	: FunctionCN_Component(L"Asin", L"Arc Sin", triangleShape, 1, 2, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}

double ArcCos_Component::innerMethodCN(Component comp, double *a) {
	double x = acos(a[0]);
	if (comp.setting[0])
		return RADIANS_TO_DEGREES * x;
	return x;
}
ArcCos_Component::ArcCos_Component(Sheet* pg)
	: FunctionCN_Component(L"Acos", L"Arc Cos", triangleShape, 1, 2, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}

double ArcTan_Component::innerMethodCN(Component comp, double *a) {
	double x = atan(a[0]);
	if (comp.setting[0])
		return RADIANS_TO_DEGREES * x;
	return x;
}
ArcTan_Component::ArcTan_Component(Sheet* pg)
	: FunctionCN_Component(L"Atan", L"Arc Tan", triangleShape, 1, 2, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}

double ArcTan2_Component::innerMethodCN(Component comp, double *a) {
	double x = atan2(a[0], a[1]);
	if (comp.setting[0])
		return RADIANS_TO_DEGREES * x;
	return x;
}
ArcTan2_Component::ArcTan2_Component(Sheet* pg)
	: FunctionCN_Component(L"Atan2", L"Arc Tan2", triangleShape, 2, 2, 1, 1)
{
	dialogButtonList = trigButtonList;
	setSetting(0, pg->lastTrigMemory);
}


// ********************** Bit **************************

double BitAnd_Component::innerMethodN(double *a) {
	return ((int)a[0]) & ((int)a[1]);
}
BitAnd_Component::BitAnd_Component()
	: FunctionN_Component (L"BitAnd", L"Bit AND", triangleShape, 2, 2, 1, 1){}

double BitOr_Component::innerMethodN(double *a) {
	return ((int)a[0]) | ((int)a[1]);
}
BitOr_Component::BitOr_Component()
	: FunctionN_Component (L"BitOr", L"Bit OR", triangleShape, 2, 2, 1, 1){}

double BitXor_Component::innerMethodN(double *a) {
	return ((int)a[0]) ^ ((int)a[1]);
}
BitXor_Component::BitXor_Component()
	: FunctionN_Component (L"BitXor", L"Bit XOR", triangleShape, 2, 2, 1, 1){}

double BitNot_Component::innerMethodN(double *a) {
	return ~((int)a[0]);
}
BitNot_Component::BitNot_Component()
	: FunctionN_Component (L"BitNot", L"Bit NOT", triangleShape, 1, 1, 1, 1){}

double InRange_Component::innerMethodN(double *a) {
	if (a[0] < a[2])
		return 0;	// invalid low and high
	if (a[1] > a[0])
		return 0; // input too high
	if (a[1] < a[2])
		return 0; // input too low
	return 1;
}
InRange_Component::InRange_Component()
	: FunctionN_Component (L"InRange", L"In Range", squareShape, 3, 1, 1, 1)
{
	setLeftLabels(1, "High", "Input", "Low");
}

double Fade_Component::innerMethodN(double *a) {
	if (a[2] >= 1.0)
		return a[1];
	if (a[2] <= 0.0)
		return a[0];

	return a[1] * a[2] + a[0] * (1.0 - a[2]);
}
Fade_Component::Fade_Component()
	: FunctionN_Component (L"Fade", L"Fader", squareShape, 3, 1, 1, 1)
{
	setLeftLabels(3, "0", "1", "Control");
}

double Fact_Component::innerMethodN(double *a) {
	double result = 1;
	if (a[0] > 19)
		return INFINITY;
	
	for (int i = 1; i < a[0]; i++)
		result *= (i + 1);

	return result;
}
Fact_Component::Fact_Component()
	: FunctionN_Component (L"Fact", L"Factorial", triangleShape, 1, 1, 1, 1){}


// ***************************** polynomial *************************************

int Polynomial_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1) // x input
		return clearOutputs();
	if (input[1].drivenByComponent == -1) // coefficient input
		return clearOutputs();

	int points = input[0].value.size();
	int coefs = input[1].value.size();
	double value;
	double x;
	int k;

	output[0].value.resize(points);
	copyDimensions(0, 0);

	for (int j = 0; j < points; j++) {
		x = 1;
		value = 0;
		for (k = 0; k < coefs; k++) {
			value += x * input[1].value[k];
			x *= input[0].value[j];
		}
		output[0].value[j] = value;
	}

	return true;
}
Polynomial_Component::Polynomial_Component()
	: Component (L"Polynomial", L"Polynomial", 2, 1)
{
	setLeftLabels(3, "X", "Coefs.");
}


// aX^N -->  aX^(n+1)/(n+1)
int PolyInteg_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();
	if (input[0].drivenByComponent == -1) // x start input
		return clearOutputs();
	if (input[1].drivenByComponent == -1) // x end input
		return clearOutputs();
	if (input[2].drivenByComponent == -1) // coefficient input
		return clearOutputs();

	int starts = input[0].value.size();
	int ends = input[1].value.size();
	if ((starts != ends) && (starts != 1) && (ends != 1)) {
		output[0].value.clear();
		return false;
	}
	int coefs = input[2].value.size();

	double x;
	int k, n;
	int startN = 0;
	int endN = 0;
	int loops = max(starts, ends);
	output[0].value.resize(loops);

	for (n = 0; n < loops; n++) {
		x = input[0].value[startN];
		double startValue = 0;
		for (k = 0; k < coefs; k++) {
			startValue += x *input[2].value[k] / (k + 1);
			x *= input[0].value[startN];
		}
		if (starts > 1)
			startN++;

		x = input[1].value[endN];
		double endValue = 0;
		for (k = 0; k < coefs; k++) {
			endValue += x * input[2].value[k] / (k + 1);
			x *= input[1].value[endN];
		}
		if (ends > 1)
			endN++;

		output[0].value[n] = endValue - startValue;
	}
	copyDimensions(0, 0);
	return true;
}
PolyInteg_Component::PolyInteg_Component()
	: Component (L"PolyInteg", L"Polynomial Integral", 3, 1)
{
	setLeftLabels(3, "X start", "X end", "Coefs.");
}



// aX^n  -->  naX^(n-1)
int PolyDeriv_Component::method2(Sheet* pg) {
	if (input[0].drivenByComponent == -1) // x input
		return clearOutputs();
	if (input[1].drivenByComponent == -1) // coefficient input
		return clearOutputs();

	int points = input[0].value.size();
	int coefs = input[1].value.size();
	double value;
	double x;
	int k;

	output[0].value.resize(points);

	for (int j = 0; j < points; j++) {
		x = 1;
		value = 0;
		for (k = 1; k < coefs; k++) {
			value += k * x * input[1].value[k];
			x *= input[0].value[j];
		}
		output[0].value[j] = value;
	}
	copyDimensions(0, 0);
	return true;
}
PolyDeriv_Component::PolyDeriv_Component()
	: Component (L"PolyDeriv", L"Polynomial Derivitive", 2, 1)
{
	setLeftLabels(3, "X", "Coefs.");
}

// ************************** Dot and Cross products ********************************



double Dot2Prod_Component::innerMethodN(double *a) {
	double dotProduct = 0.0;
	dotProduct += a[0] * a[2];
	dotProduct += a[1] * a[3];
	return dotProduct;
}
Dot2Prod_Component::Dot2Prod_Component()
	: FunctionN_Component (L"Dot2Prod", L"2D Dot Product", squareShape, 4, 1, 1, 1)
{
	setLeftLabels(3, "X0", "Y0", "X1", "Y1");
}


double Dot3Prod_Component::innerMethodN(double *a) {
	double dotProduct = 0.0;
	dotProduct += a[0] * a[3];
	dotProduct += a[1] * a[4];
	dotProduct += a[2] * a[5];
	return dotProduct;
}
Dot3Prod_Component::Dot3Prod_Component()
	: FunctionN_Component (L"Dot3Prod", L"3D Dot Product", squareShape, 6, 1, 1, 1)
{
	setLeftLabels(3, "X0", "Y0", "Z0", "X1", "Y1", "Z1");
}


int CrossProduct_Component::method2(Sheet* pg) {

	int loops = inputsToLoops(); //AndDimensions(pg, i);
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	output[1].value.resize(loops);
	output[2].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		double X0 = getLoopingValueD(0, k, 0);
		double Y0 = getLoopingValueD(1, k, 0);
		double Z0 = getLoopingValueD(2, k, 0);
		double X1 = getLoopingValueD(3, k, 0);
		double Y1 = getLoopingValueD(4, k, 0);
		double Z1 = getLoopingValueD(5, k, 0);

		output[0].value[k] = Y0 * Z1 - Z0 * Y1;
		output[1].value[k] = Z0 * X1 - X0 * Z1;
		output[2].value[k] = X0 * Y1 - Y0 * X1;
	}
	copyDimensions(0, 0);
	copyDimensions(1, 1);
	copyDimensions(2, 2);
	return true;
}

CrossProduct_Component::CrossProduct_Component()
	: Component (L"CrossProd", L"Cross Product", squareShape, 6, 1, 3, 1)
{
	setLeftLabels(3, "X0", "Y0", "Z0", "X1", "Y1", "Z1");
	setRightLabels(3, "X", "Y", "Z");
}



// ****************************** distance *****************************************


double Flat2dDist_Component::innerMethodN(double *a) {
	double dx = a[0] - a[2];
	double dy = a[1] - a[3];
	return sqrt(dx * dx + dy * dy);
}
Flat2dDist_Component::Flat2dDist_Component()
	: FunctionN_Component (L"FDist", L"2D Flat Distance", squareShape, 4, 1, 1, 1)
{
	setLeftLabels(3, "X0", "Y0", "X1", "Y1");
}


double SphericalDistance_Component::innerMethodN(double *a) {
	if (a[4] == 0)
		a[4] = 6371; // radius of earth in km

	double dLat = a[2] - a[0];
	double dLon = a[3] - a[1];
	double aa = sin(dLat / 2) * sin(dLat / 2) +
		sin(dLon / 2) * sin(dLon / 2) * cos(a[0]) * cos(a[2]);
	double c = 2 * atan2(sqrt(aa), sqrt(1 - aa));
	return a[4] * c;
}
SphericalDistance_Component::SphericalDistance_Component()
	: FunctionN_Component (L"SDist", L"Spherical Distance", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "Lat0 (rad)", "Lon0 (rad)", "Lat1 (rad)", "Lon1 (rad)", "Radius");
}


double ThreeDDist_Component::innerMethodN(double *a) {
	double dx = a[0] - a[3];
	double dy = a[1] - a[4];
	double dz = a[2] - a[5];
	return sqrt(dx * dx + dy * dy + dz * dz);
}
ThreeDDist_Component::ThreeDDist_Component()
	: FunctionN_Component (L"3Dist", L"3D Distance", squareShape, 6, 1, 1, 1)
{
	setLeftLabels(3, "X0", "Y0", "Z0", "X1", "Y1", "Z1");
}


// *************************** counters **********************************



int Counter_Component::method2(Sheet* pg)  {      // handle calc for a function with just one input
	
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	int outPins = rightTerminalCount;
	bool resize = false;

	if ((loops != initialMemoryX.size()) || (loops != initialMemoryY.size()))
		resize = true;
	else
		for (int j = 0; j < outPins; j++)
			if (loops != output[j].value.size())
				resize = true;
	if (resize) {
		initialMemoryX.resize(loops);
		initialMemoryY.resize(loops);
		for (int k = 0; k < loops; k++) {
			initialMemoryX[k] = 0;
			initialMemoryY[k] = 0;
		}

		for (int j = 0; j < outPins; j++) {
			output[j].value.resize(loops);
			for (int k = 0; k < loops; k++) 
				output[j].value[k] = 0;
		}
	}

	
	bool changed = false;

	for (int k = 0; k < loops; k++) {

		// reset
		if (getLoopingValueD(2, k, 0)) {
			for (int j = 0; j < outPins; j++)
				output[j].value[k] = 0;
			changed = true;
			continue;
		}

		// up
		if (!initialMemoryX[k]) {
			if (getLoopingValueD(0, k, 0)) {
				initialMemoryX[k] = true;
				//increment
				for (int j = 0; j < outPins; j++) {
					if (output[j].value[k]) {
						output[j].value[k] = 0;
					}
					else {
						output[j].value[k] = 1;
						break;
					}
				}
				changed = true;
				continue;
			}
		}
		else
			initialMemoryX[k] = false;

		// down
		if (!initialMemoryY[k]) {
			if (getLoopingValueD(1, k, 0)) {
				initialMemoryY[k] = true;
				//decrement
				for (int j = 0; j < outPins; j++) {
					if (output[j].value[k]) {
						output[j].value[k] = 0;
						break;
					}
					else {
						output[j].value[k] = 1;
					}
				}
				changed = true;
				continue;
			}
		}
		else
			initialMemoryY[k] = false;
	}
	return changed;
}

bool Counter_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(max(n, leftTerminalCount));

	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);

	return true;
}

void Counter_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if ((doubleValues[0] >= 2) && (doubleValues[0] <= 20))
		setting[0] = doubleValues[0];
	onLoad(pg);
}
void doubleClickCounter(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	
	vector<string> doubleLabels = { "Number of Outputs:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}
std::vector<Button*> counterButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleClickCounter) };

Counter_Component::Counter_Component()
	: Component (L"Counter", L"Binary Counter", squareShape, 3, 1, 4, 1)
{
	setSetting(0, 4); // number of bits to output
	for (int kk = 0; kk < 4; kk++) {
		output[kk].value[0] = 0.0;
		output[kk].dimensions[0] = 1;
	}
	dialogButtonList = counterButtonList;
	setRightLabels(3, "0", "1", "2", "3");
	setLeftLabels(3, "Up", "Down", "Reset");
	initialMemoryX.resize(1);
	initialMemoryX[0] = 0;  // up edge
	initialMemoryY.resize(1);
	initialMemoryY[0] = 0;  // down edge
}


#if 0
double funcScalarCounter(Component *comp, int j, double *a){// count, mem, auxMem, j, a, b, c) {
	// a[0] = Up, a[1] = Down, a[2] = Reset

	if (j >= comp->initialMemoryX.size()) {
		comp->initialMemoryX.resize(j+1);
		comp->initialMemoryY.resize(j+1);
		for (int k = 0; k < j+1; k++) {
			comp->initialMemoryX[k] = 0;
			comp->initialMemoryY[k] = 0;
		}

		comp->output[0].value.resize(j+1);
		for (int k = 0; k < j+1; k++)
			comp->output[0].value[k] = 0;
	}

	// reset
	if (a[2])
		return 0;

	// up
	if (!comp->initialMemoryX[j]) {
		if (a[0]) {
			comp->initialMemoryX[j] = true;
			return comp->output[0].value[j] + 1;
		}
	}
	else
		comp->initialMemoryX[j] = false;

	// down
	if (!comp->initialMemoryY[j]) {
		if (a[1]) {
			comp->initialMemoryY[j] = true;
			return comp->output[0].value[j] - 1;
		}
	}
	else
		comp->initialMemoryY[j] = false;

	return comp->output[0].value[j]; // no change
}
SCount_Component::SCount_Component()
	: Function_Component (L"SCount", L"Scalar Counter", inputRepeaterWithCompIndex_, squareShape, 3, 1, 1, 1)
{
	innerMethodCJN = funcScalarCounter;
	setLeftLabels(3, L"Up", L"Down", L"Reset");
	initialMemoryX.resize(1);
	initialMemoryX[0] = 0;  // up edge
	initialMemoryY.resize(1);
	initialMemoryY[0] = 0;  // down edge
}
#endif

// ********************************** slope and intersect **************************************


int Slope_Component::method2(Sheet* pg) {
	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	for (int j = 0; j < 2; j++) {
		output[j].value.resize(loops);
		output[j].dimensions.resize(1);
		output[j].dimensions[0] = loops;
	}

	for (int j = 0; j < loops; j++) {
		double x0 = getLoopingValueD(0, j, 0);
		double y0 = getLoopingValueD(1, j, 0);
		double x1 = getLoopingValueD(2, j, 0);
		double y1 = getLoopingValueD(3, j, 0);

		double dx = x1 - x0;
		double dy = y1 - y0;
		double slope = dy / dx;
		double intercept = y0 - slope * x0;

		output[0].value[j] = slope;
		output[1].value[j] = intercept;
	}
	return true;
}

Slope_Component::Slope_Component()
	: Component (L"Slope", L"Slope Intercept", squareShape, 4, 1, 2, 1)
{
	setLeftLabels(3, "X0", "Y0", "X1", "Y1");
	setRightLabels(3, "Slope", "Intercept");
	width = 40;
}


int IntersectLines_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	for (int j = 0; j < 2; j++) {
		output[j].value.resize(loops);
		output[j].dimensions.resize(1);
		output[j].dimensions[0] = loops;
	}

	for (int j = 0; j < loops; j++) {
		double m0 = getLoopingValueD(0, j, 0);
		double b0 = getLoopingValueD(1, j, 0);
		double m1 = getLoopingValueD(2, j, 0);
		double b1 = getLoopingValueD(3, j, 0);

		double db = b1 - b0;
		double dm = m0 - m1;
		double x = db / dm;
		double y = x * m0 + b0;

		output[0].value[j] = x;
		output[1].value[j] = y;
	}
	return true;
}

IntersectLines_Component::IntersectLines_Component()
	: Component (L"Intersect", L"Intersect Lines", squareShape, 4, 1, 2, 1)
{
	setLeftLabels(3, "Slope0", "Intercept0", "Slope1", "Intercept1");
	setRightLabels(3, "X", "Y");
	width = 40;
}


// this calculates the intersection point(s) of two circles
int IntersectCircles_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	for (int j = 0; j < 2; j++) {
		output[j].value.clear();
		output[j].dimensions.resize(0);
	}

	double x1 = input[0].value[0];
	double y1 = input[1].value[0];
	double r1 = input[2].value[0];
	double x2 = input[3].value[0];
	double y2 = input[4].value[0];
	double r2 = input[5].value[0];

	if (x1 == x2 && y1 == y2 && r1 == r2)
		return true;

	// based on the math here:
	// http://math.stackexchange.com/a/1367732

	// x1,y1 is the center of the first circle, with radius r1
	// x2,y2 is the center of the second ricle, with radius r2

	double centerdx = x1 - x2;
	double centerdy = y1 - y2;
	double R = sqrt(centerdx * centerdx + centerdy * centerdy);
	if (!(abs(r1 - r2) <= R && R <= r1 + r2))  // no intersection
		return true; // empty list of results


	// intersection(s) should exist
	double R2 = R * R;
	double R4 = R2 * R2;
	double a = (r1*r1 - r2 * r2) / (2 * R2);
	double r2r2 = (r1*r1 - r2 * r2);
	double c = sqrt(2 * (r1*r1 + r2 * r2) / R2 - (r2r2 * r2r2) / R4 - 1);

	double fx = (x1 + x2) / 2 + a * (x2 - x1);
	double gx = c * (y2 - y1) / 2;
	double ix1 = fx + gx;
	double ix2 = fx - gx;

	double fy = (y1 + y2) / 2 + a * (y2 - y1);
	double gy = c * (x1 - x2) / 2;
	double iy1 = fy + gy;
	double iy2 = fy - gy;

	// note if gy == 0 and gx == 0 then the circles are tangent and there is only one solution
	// but that one solution will just be duplicated as the code is currently written

	output[0].value.resize(1);
	output[0].value[0] = ix1;
	output[1].value.resize(1);
	output[1].value[0] = iy1;
	output[0].dimensions.resize(1);
	output[0].dimensions[0] = 1;
	output[1].dimensions.resize(1);
	output[1].dimensions[0] = 1;

	if (gy != 0 || gx != 0) {
		output[0].value.resize(2);
		output[0].value[1] = ix2;
		output[1].value.resize(2);
		output[1].value[1] = iy2;
		output[0].dimensions[0] = 2;
		output[1].dimensions[0] = 2;
	}
	return true;
}


IntersectCircles_Component::IntersectCircles_Component()
	: Component (L"Intersect2", L"Intersect Circles", squareShape, 6, 1, 2, 1)
{
	setLeftLabels(3, "X0", "Y0", "Radius0", "X1", "Y1", "Radius1");
	setRightLabels(3, "Xs", "Ys");
	width = 40;
}


// this calculates the intersection point(s) of two circles
int IntersectLineCircle_Component::method2(Sheet* pg ) {

	if (!checkValueInputs())
		return clearOutputs();
	for (int j = 0; j < 2; j++) {
		output[j].value.clear();
		output[j].dimensions.clear();
	}

	double circleX      = input[0].value[0];
	double circleY      = input[1].value[0];
	double CircleRadius = input[2].value[0];
	double slope        = input[3].value[0];
	double intercept    = input[4].value[0];

	if (CircleRadius == 0)
		return true;

	// circle: (x - circleX)^2 + (y - circleY)^2 = CircleRadius^2
	// line: y = m * x + n
	// circleRadius = r: circle radius
	// circleX = h: x value of circle centre
	// circleY = k: y value of circle centre
	// slope = m: slope
	// intercept = n: y-intercept

	// get a, b, c values
	double a = 1 + (slope * slope);
	double b = -circleX * 2 + (slope * (intercept - circleY)) * 2;
	double c = (circleX*circleX) + (intercept - circleY)*(intercept - circleY) - (CircleRadius*CircleRadius);

	// get discriminant
	double d = (b*b) - 4 * a * c;
	if (d >= 0) {
		// insert into quadratic formula
		double intersectionX1 = (-b + sqrt((b*b) - 4 * a * c)) / (2 * a);
		double intersectionX2 = (-b - sqrt((b*b) - 4 * a * c)) / (2 * a);

		output[0].value.resize(1);
		output[0].value[0] = intersectionX1;
		output[1].value.resize(1);
		output[1].value[0] = slope * intersectionX1 + intercept;

		if (d == 0) {
			// only 1 intersection
			output[0].dimensions.resize(1);
			output[0].dimensions[0] = 1;
			output[1].dimensions.resize(1);
			output[1].dimensions[0] = 1;
			return true;
		}

		// 2 intersections
		output[0].value.resize(2);
		output[0].value[1] = intersectionX2;
		output[1].value.resize(2);
		output[1].value[1] = slope * intersectionX2 + intercept;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = 2;
		output[1].dimensions.resize(1);
		output[1].dimensions[0] = 2;
	}
	// no intersection
	output[0].dimensions.clear();
	output[1].dimensions.clear();
	return true;
}


IntersectLineCircle_Component::IntersectLineCircle_Component()
	: Component (L"Intersect3", L"Intersect Line/Circle", squareShape, 5, 1, 2, 1)
{
	setLeftLabels(3, "Circle X", "Circle Y", "Radius", "Line Slope", "Line Intercept");
	setRightLabels(3, "Xs", "Ys");
	width = 40;
}

// ************************************* Scale *****************************************

double Scale_Component::innerMethodN(double *a) {
	if (a[2] < a[0])
		return a[3];
	if (a[2] > a[1])
		return a[4];
	// Scale the inputs into the outputs
	return (a[2] - a[0]) * (a[4] - a[3]) / (a[1] - a[0]) + a[3];
}
Scale_Component::Scale_Component()
	: FunctionN_Component (L"Scale", L"Scale", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "In Min", "In Max", "In", "Out Min", "Out Max");
	setRightLabels(3, "Out");
}

double LogScale_Component::innerMethodN(double *a) { //0 inMin, 1 inMax, 2 input, 3 outMin, 4 outMax)
	// Scale using log
	return log(a[2] - a[0] + 1) * (a[4] - a[3]) / log(a[1] - a[0]) + a[3];
}
LogScale_Component::LogScale_Component()
	: FunctionN_Component (L"LogScale", L"Log Scale", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "In Min", "In Max", "In", "Out Min", "Out Max");
	setRightLabels(3, "Out");
}

// ******************************** Permute ****************************************


void Permute_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {

	if ((doubleValues[0] >= 2) && (doubleValues[0] <= 20)) {
		setting[0] = doubleValues[0];
		onLoad(pg);
	}
}
void doubleClickPermute(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	
	vector<string> doubleLabels = { "Group Size:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[0] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80, NULL);
}
std::vector<Button*> permuteButtonList = { new Button(-83, 40, 120, 20, L"Set Group Size...", doubleClickPermute) };

int factorial(int nn) {
	int result = 1;
	for (int i = 1; i < nn; i++)
		result *= (i + 1);
	return result;
}

int factorial2(int start, int end) {
	int result = 1;
	for (int i = end; i < start; i++)
		result *= (i + 1);
	return result;
}

int permute(vector<int> j, int limit, int level, int groupSize, int m, Component  *comp) {
	int i;
	int start;
	bool stringType = comp->input[0].stringType;

	if (level == 0)
		start = 0;
	else {
		start = j[level - 1] + 1;
		j.push_back(0); // just to add an element for the loop
	}
	for (j[level] = start; j[level] < limit; j[level]++) {
		if (level == groupSize - 1) {
			if (stringType) {
				for (i = 0; i < groupSize; i++)
					comp->output[i].stringValue[m] = comp->input[0].stringValue[j[i]];
			}
			else
			{
				for (i = 0; i < groupSize; i++)
					comp->output[i].value[m] = comp->input[0].value[j[i]];
			}
			m++;
		}
		else {
			m = permute(j, limit + 1, level + 1, groupSize, m, comp);
		}
	}
	return m;
}

int Permute_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1) {
		for (int n = 0; n < setting[0]; n++) {
			output[n].value.clear();
			output[n].dimensions.clear();
		}
		return clearOutputs();
	}

	bool stringType = input[0].stringType;
	int poolSize;
	if( stringType )
		poolSize = input[0].stringValue.size();
	else
		poolSize = input[0].value.size();
	int groupSize = setting[0];
	int perms = factorial2(poolSize, groupSize) / factorial(poolSize - groupSize);
	if( perms == 0)
		return clearOutputs();

	if (stringType) {
		for (int n = 0; n < groupSize; n++)
			output[n].stringValue.resize(perms);
	}
	else
	{
		for (int n = 0; n < groupSize; n++)
			output[n].value.resize(perms);
	}
	int k = poolSize - groupSize + 1;
	int m = 0;
	vector<int> j = { 0 };

	permute(j, k, 0, groupSize, m, this);
	for (int n = 0; n < setting[0]; n++) {
		output[n].stringType = stringType;
		output[n].dimensions.resize(1);
		output[n].dimensions[0] = perms;
	}
	return true;
}

bool Permute_Component::onLoad(Sheet* pg) {  // same code as onLoadSplit_
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);
	
	return true;
}


Permute_Component::Permute_Component()
	: Component (L"Perm", L"Combine", squareShape, 1, 1, 2, 1)
{
	dialogButtonList = permuteButtonList;
	setSetting(0, 2);
}


void permutationList(Component *comp, int n, vector<vector<int>> *list) {
	
	bool stringType = comp->input[0].stringType;

	if (n == 1) {
		if( stringType )
			comp->output[0].stringValue[0] = comp->input[0].stringValue[0];
		else
			comp->output[0].value[0] = comp->input[0].value[0];
		comp->output[0].stringType = stringType;
		list->clear();
		return;
	}
	
	vector<int> aa = { 0, 1, 4 };
	list->push_back(aa);
	vector<int> bb = { 1, 0, 6 };
	list->push_back(bb);
	//int lenx = (*list)[0].size();
	
	for (int j = 3; j <= n; j++) {
		// deep copy the list j times
		vector<vector<int>> temp;
		int oldLen = list->size();
		int index = 0;
		for (int k = 0; k < j; k++)
			for (int m = 0; m < oldLen; m++) {
				vector<int> cc;
				cc.clear();
				int lenz = (*list)[m].size();
				for (int z = 0; z < lenz; z++)
					cc.push_back((*list)[m][z]);
				temp.push_back(cc);
				index++;
			}
		*list = temp;
		// insert j-1 into list at each of j positions
		int len = list->size();
		index = 0;
		for (int k = 0; k < len; k += oldLen) {
			for (int m = 0; m < oldLen; m++) {
				vector<int>::iterator it;
				it = (*list)[k + m].begin();
				(*list)[k + m].insert(it + index, 1, j - 1);
			}
			index++;
		}
	}
	return;
}

int Permute2_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1) {
		for (int j = 0; j < setting[0]; j++) {
			output[j].value.clear();
			output[j].dimensions.clear();
		}
		return true;
	}

	bool stringType = input[0].stringType;
	int n;
	if( stringType )
		n = input[0].stringValue.size();
	else
		n = input[0].value.size();
	int outputSize = setting[0];
	if (outputSize < n)
		n = outputSize; // dont use all the inputs

	for (int j = 0; j < n; j++) {
		output[j].stringValue.clear();
		output[j].value.clear();
	}
	for (int j = n; j < outputSize; j++) {
		output[j].value.clear();
		output[j].stringValue.clear();
		output[j].dimensions.clear();
	}

	if (n == 1) {
		if (stringType) {
			output[0].stringValue.resize(1);
			output[0].stringValue[0] = input[0].stringValue[0];
		}
		else
		{
			output[0].value.resize(1);
			output[0].value[0] = input[0].value[0];
		}
		output[0].stringType = stringType;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = 1;
		return true;
	}
	vector<vector<int>> list;
	permutationList(this, n, &list);

	// copy the lists to outputs
	int len = list.size();
	for (int j = 0; j < n; j++) {
		if (stringType) {
			output[j].stringValue.resize(len);
			for (int k = 0; k < len; k++) 
				output[j].stringValue[k] = input[0].stringValue[list[k][j]];
		}
		else
		{
			output[j].value.resize(len);
			for (int k = 0; k < len; k++)
				output[j].value[k] = input[0].value[list[k][j]];
		}
		output[j].stringType = stringType;
		output[j].dimensions.resize(1);
		output[j].dimensions[0] = len;
	}

	return true;
}

bool Permute2_Component::onLoad(Sheet* pg) { 
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);
	
	return true;
}


Permute2_Component::Permute2_Component()
	: Component (L"Perm2", L"Permute", squareShape, 1, 1, 2, 1)
{
	dialogButtonList = permuteButtonList;
	setSetting(0, 2);
}



int Permute3_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1) {
		for (int j = 0; j < setting[0]; j++) {
			output[j].value.clear();
			output[j].stringValue.clear();
			output[j].dimensions.clear();
		}
		return true;
	}

	bool stringType = input[0].stringType;
	int n;
	if(stringType)
		n = input[0].stringValue.size();
	else
		n = input[0].value.size();

	int outputSize = setting[0];
	if (n <= outputSize)
		outputSize = n; // dont use all the outputs

	//for (int j = 0; j < n; j++) {
	//	output[j].stringType = stringType;
	//	output[j].stringValue.clear();
	//	output[j].value.clear();
	//}
	for (int j = 0; j < rightTerminalCount; j++) {
		output[j].stringType = stringType;
		output[j].stringValue.clear();
		output[j].value.clear();
		output[j].dimensions.resize(1);
		output[j].dimensions[0] = 0;
	}

	switch (outputSize) {
	case 1:
		for (int a = 0; a < n; a++)
			if( stringType )
				output[0].stringValue.push_back(input[0].stringValue[a]);
			else
				output[0].value.push_back(input[0].value[a]);
		break;
	case 2:
		for (int a = 0; a < n; a++) {
			for (int b = 0; b < n; b++) {
				if (a == b)
					continue;
				if (stringType) {
					output[0].stringValue.push_back(input[0].stringValue[a]);
					output[1].stringValue.push_back(input[0].stringValue[b]);
				}else{
					output[0].value.push_back(input[0].value[a]);
					output[1].value.push_back(input[0].value[b]);
				}
			}
		}
		break;
	case 3:
		for (int a = 0; a < n; a++) {
			for (int b = 0; b < n; b++) {
				if (a == b)
					continue;
				for (int c = 0; c < n; c++) {
					if ((a == c) || (b == c))
						continue;
					if (stringType) {
						output[0].stringValue.push_back(input[0].stringValue[a]);
						output[1].stringValue.push_back(input[0].stringValue[b]);
						output[2].stringValue.push_back(input[0].stringValue[c]);
					}else {
						output[0].value.push_back(input[0].value[a]);
						output[1].value.push_back(input[0].value[b]);
						output[2].value.push_back(input[0].value[c]);
					}
				}
			}
		}
		break;
	case 4:
		for (int a = 0; a < n; a++) {
			for (int b = 0; b < n; b++) {
				if (a == b)
					continue;
				for (int c = 0; c < n; c++) {
					if ((a == c) || (b == c))
						continue;
					for (int d = 0; d < n; d++) {
						if ((a == d) || (b == d) || (c == d))
							continue;
						if (stringType) {
							output[0].stringValue.push_back(input[0].stringValue[a]);
							output[1].stringValue.push_back(input[0].stringValue[b]);
							output[2].stringValue.push_back(input[0].stringValue[c]);
							output[3].stringValue.push_back(input[0].stringValue[d]);
						}else{
							output[0].value.push_back(input[0].value[a]);
							output[1].value.push_back(input[0].value[b]);
							output[2].value.push_back(input[0].value[c]);
							output[3].value.push_back(input[0].value[d]);
						}
					}
				}
			}
		}
		break;
	case 5:
		for (int a = 0; a < n; a++) {
			for (int b = 0; b < n; b++) {
				if (a == b)
					continue;
				for (int c = 0; c < n; c++) {
					if ((a == c) || (b == c))
						continue;
					for (int d = 0; d < n; d++) {
						if ((a == d) || (b == d) || (c == d))
							continue;
						for (int e = 0; e < n; e++) {
							if ((a == e) || (b == e) || (c == e) || (d == e))
								continue;
							if (stringType) {
								output[0].stringValue.push_back(input[0].stringValue[a]);
								output[1].stringValue.push_back(input[0].stringValue[b]);
								output[2].stringValue.push_back(input[0].stringValue[c]);
								output[3].stringValue.push_back(input[0].stringValue[d]);
								output[4].stringValue.push_back(input[0].stringValue[e]);
							}else{
								output[0].value.push_back(input[0].value[a]);
								output[1].value.push_back(input[0].value[b]);
								output[2].value.push_back(input[0].value[c]);
								output[3].value.push_back(input[0].value[d]);
								output[4].value.push_back(input[0].value[e]);
							}
						}
					}
				}
			}
		}
		break;
	}

	int len;
	if( stringType )
		len = output[0].stringValue.size();
	else
		len = output[0].value.size();

	for (int j = 0; j < outputSize; j++) {
		output[j].stringType = stringType;
		output[j].dimensions.resize(1);
		output[j].dimensions[0] = len;
	}

	return true;
}

bool Permute3_Component::onLoad(Sheet* pg) {
	int n = setting[0];
	height = pinToHeight(n);

	// right terminals are outputs
	buildOutputPins(n);
	for (int j = 0; j < n; j++)
		output[j].label = tomString(j);
	
	return true;
}


Permute3_Component::Permute3_Component()
	: Component (L"Perm3", L"r-permute", squareShape, 1, 1, 2, 1)
{
	dialogButtonList = permuteButtonList;
	setSetting(0, 2);
}

// *********************** fractions **********************************



void getFactors(int num, vector<int>*factors) {
	factors->clear();
	for (int j = 2; j < 1000; j++) { // safer than while for debug
		for (int k = 1; k < 100; k++) { // safer than while for debug
			if ((num % j) == 0) {
				factors->push_back(j);
				num /= j;
			}
			else
				break;
		}
		if (num == 1)
			break;
	}
	return;
}

void reduceFraction(int numIn, int denomIn, int* outWhole, int* outNum, int* outDenom) {
	*outWhole = 0;
	*outNum = 0;
	*outDenom = 0;

	if (numIn == 0)
		return;

	int sign = 1; // positive
	if ((numIn < 0) && (denomIn > 0)) sign = -1;
	if ((numIn > 0) && (denomIn < 0)) sign = -1;
	numIn = abs(numIn);
	denomIn = abs(denomIn);

	if (numIn >= denomIn) {
		if (denomIn == 0)
			*outWhole = 0;
		else
			*outWhole = floor(numIn / denomIn); // integer divide
		numIn -= *outWhole * denomIn;
	}
	vector<int> numFactors;
	vector<int> denomFactors;
	if (numIn > 0) {
		getFactors(numIn, &numFactors);
		getFactors(denomIn, &denomFactors);
		for (int j = 0; j < numFactors.size(); j++) {
			for (int k = 0; k < denomFactors.size(); k++) {
				if (numFactors[j] == denomFactors[k]) {
					numFactors[j] = 1;
					denomFactors[k] = 1;
					break;
				}
			}
		}
		*outNum = 1;
		for (int j = 0; j < numFactors.size(); j++)
			*outNum *= numFactors[j];
		*outDenom = 1;
		for (int k = 0; k < denomFactors.size(); k++)
			*outDenom *= denomFactors[k];
	}

	if (sign == -1) {
		if (*outWhole == 0)
			*outNum = -*outNum;
		else
			*outWhole = -*outWhole;
	}
}



int Fractions_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	for (int k = 0; k < 3; k++)
		output[k].value.resize(loops);
	setSimpleOutputDimensions();

	for (int k = 0; k < loops; k++) {
		int whole0 = floor(getLoopingValueD(0, k, 0));
		int num0   = floor(getLoopingValueD(1, k, 0));
		int denom0 = floor(getLoopingValueD(2, k, 0));
		int whole1 = floor(getLoopingValueD(3, k, 0));
		int num1   = floor(getLoopingValueD(4, k, 0));
		int denom1 = floor(getLoopingValueD(5, k, 0));

		if (denom0 == 0) {
			denom0 = 1;
			num0 = 0;
		}
		if (denom1 == 0) {
			denom1 = 1;
			num1 = 0;
		}
		int wholeOut, numOut = 0, denomOut = 0;

		if (z(type, L"AddFrac")) {
			int denom = denom0 * denom1;
			int num = num0 * denom1 + num1 * denom0;
			reduceFraction(num, denom, &wholeOut, &numOut, &denomOut);
			output[0].value[k] = wholeOut + whole0 + whole1;
		}
		if (z(type, L"MulFrac")) {
			int num = (num0 + whole0 * denom0) * (num1 + whole1 * denom1);
			int denom = denom0 * denom1;
			reduceFraction(num, denom, &wholeOut, &numOut, &denomOut);
			output[0].value[k] = wholeOut;
		}
		if (z(type, L"DivFrac")) {
			int num = (num0 + whole0 * denom0) * denom1;
			int denom = denom0 * (num1 + whole1 * denom1);
			reduceFraction(num, denom, &wholeOut, &numOut, &denomOut);
			output[0].value[k] = wholeOut;
		}

		output[1].value[k] = numOut;
		output[2].value[k] = denomOut;
	}

	return true;
}

Fractions_Component::Fractions_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS){}

AddFrac_Component::AddFrac_Component()
	: Fractions_Component (L"AddFrac", L"Add Fractions", squareShape, 6, 1, 3, 1)
{
	setLeftLabels(3, "Whole Part 0", "Numerator 0", "Denominator 0", "Whole Part 1", "Numerator 1", "Denominator 1");
	setRightLabels(3, "Whole Part", "Numerator", "Denominator");
	width = 70;
}
MulFrac_Component::MulFrac_Component()
	: Fractions_Component(L"MulFrac", L"Multiply Fractions", squareShape, 6, 1, 3, 1)
{
	setLeftLabels(3, "Whole Part 0", "Numerator 0", "Denominator 0", "Whole Part 1", "Numerator 1", "Denominator 1");
	setRightLabels(3, "Whole Part", "Numerator", "Denominator");
	width = 70;
}
DivideFractions_Component::DivideFractions_Component()
	: Fractions_Component(L"DivFrac", L"Divide Fractions", squareShape, 6, 1, 3, 1)
{
	setLeftLabels(3, "Whole Part 0", "Numerator 0", "Denominator 0", "Whole Part 1", "Numerator 1", "Denominator 1");
	setRightLabels(3, "Whole Part", "Numerator", "Denominator");
	width = 70;
}

// **************************** Reduce **********************************

int Reduce_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	for (int k = 0; k < 4; k++)
		output[k].value.resize(loops);
	setSimpleOutputDimensions();

	for (int k = 0; k < loops; k++) {
		
		int num = floor(getLoopingValueD(0, k, 0));
		int denom = floor(getLoopingValueD(1, k, 0));
	
		int wholeOut, numOut, denomOut;
		reduceFraction(num, denom, &wholeOut, &numOut, &denomOut);
	
		output[0].value[k] = wholeOut;
		output[1].value[k] = numOut;
		output[2].value[k] = denomOut;

		if (numOut != num)
			output[3].value[k] = 1;
		else
			output[3].value[k] = 0;
	}

	return 1;
}
Reduce_Component::Reduce_Component()
	: Component(L"Reduce", L"Reduce", squareShape, 2, 1, 4, 1)
{
	setLeftLabels(3, "Numerator", "Denominator");
	setRightLabels(3, "Whole Part", "Numerator", "Denominator", "Changed");
	width = 70;
}

// ******************* Factor *************************



vector<double> getFactors(int num) {
	vector<double> factors;
	for (int j = 2; j < 1000; j++) { // safer than while for debug
		for (int k = 1; k < 100; k++) { // safer than while for debug
			if ((num % j) == 0) {
				factors.push_back(j);
				num /= j;
			}
			else
				break;
		}
		if (num == 1)
			break;
	}
	return factors;
}


int Factor_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return clearOutputs();
	if (input[0].value[0] <= 0)
		return clearOutputs();

	int value = input[0].value[0];
	output[0].value = getFactors(value);
	setSimpleOutputDimensions();
	return 1;
}

Factor_Component::Factor_Component()
	: Component(L"Factor", L"Factor", 1, 1)
{
}

// *********************** Common Factor **********************************

int CommonFactor_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	int pins = input.size();
	for (int p = 0; p < pins + 1; p++)
		output[p].value.resize(loops);
	setSimpleOutputDimensions();

	int input;
	vector<vector<double>> factors;
	for (int l = 0; l < loops; l++) {
		factors.resize(pins);
		for (int p = 0; p < pins; p++) {
			input = abs(getLoopingValueD(p, l, 0));
			if (input == 0) {
				factors[p].clear();
				continue;
			}
			factors[p] = getFactors(input);
		}

		vector<int> foundAt;
		foundAt.resize(pins);
		int commonFactor = 1;

		for (int j = 0; j < factors[0].size(); j++) { // start with the first list
			foundAt[0] = j;
			bool found = true; // lets hope
			bool find2 = false;
			for (int n = 1; n < pins; n++) {      // look in the other lists - all must match
				if (factors[n].size()) {
					find2 = false;
					for (int k = 0; k < factors[n].size(); k++) {
						if (factors[0][j] == factors[n][k]) {
							find2 = true;
							foundAt[n] = k;
							break;
						}
					}
				}
				if (!find2) {
					found = false;
					break;
				}
			}

			if (found) { // they all match - remove them all
				commonFactor *= factors[0][foundAt[0]];
				for (int p = 0; p < pins; p++)
					if( factors[p].size() )
						factors[p][foundAt[p]] = 1;
			}
		}

		for (int p = 0; p < pins; p++) {
			if (getLoopingValueD(p, l, 0) == 0) {
				output[p].value[l] = 0;
				continue;
			}
			int out = 1;
			for (int j = 0; j < factors[p].size(); j++)
				out *= factors[p][j];
			if (getLoopingValueD(p, l, 1) > 0)
				output[p].value[l] = out;
			else
				output[p].value[l] = -out;
		}
		output[pins].value[l] = commonFactor;
	}

	return 1;
}
CommonFactor_Component::CommonFactor_Component()
		: Component(L"ComFactor", L"Common Factor", squareShape, 3, 1, 4, 1)
{
	setLeftLabels(3, "In 0", "In 1", "In 2");
	setRightLabels(3, "Out 0", "Out 1", "Out 2", "Common");
	width = 40;
	setSetting(0, 3); // inputs
}

// ******************************* Ellipse Path *********************************

int EllipsePath_Component::method2(Sheet* pg) {


	int ellipseLoops = inputsToLoopsN(5);
	if (ellipseLoops == 0)
		return clearOutputs();

	int pathLoops = input[5].value.size();
	if (pathLoops == 0)
		return clearOutputs();

	for (int j = 0; j < 2; j++) {
		output[j].value.clear();
		if (ellipseLoops == 1) {
			output[j].value.resize(pathLoops);
			output[j].dimensions.resize(1);
			output[j].dimensions[0] = pathLoops;
		}
		else {
			output[j].value.resize(pathLoops*ellipseLoops);
			output[j].dimensions.resize(2);
			output[j].dimensions[0] = ellipseLoops;
			output[j].dimensions[1] = pathLoops;
		}
	}

	for (int j = 2; j < 6; j++) {
		output[j].value.clear();
		output[j].value.resize(ellipseLoops);
		output[j].dimensions[0] = ellipseLoops;
	}

	int centerFocus = setting[0];

	int n = 0;
	for (int k = 0; k < ellipseLoops; k++) {

		double inputX = getLoopingValueD(0, k, 0);
		double inputY = getLoopingValueD(1, k, 0);
		double major = getLoopingValueD(2, k, 20);
		double minor = getLoopingValueD(3, k, 10);
		double orient = getLoopingValueD(4, k, 0);
		orient = rotateInputAngle(orient);

		double sinA = sin(-orient);
		double cosA = cos(-orient);
		double dX;
		double dY;
		double centerX;
		double centerY;

		if (centerFocus) {
			double c = sqrt(abs(major * major - minor * minor));
			if (major > minor) {
				dX = c * sin(-orient);
				dY = c * cos(-orient);
			}
			else {
				dX = c * cos(orient);
				dY = c * sin(orient);
			}
			centerX = inputX + dX;
			centerY = inputY + dY;
		}
		else {
			centerX = inputX;
			centerY = inputY;
		}

		if (centerFocus) {
			output[2].value[k] = centerX;  // center x
			output[3].value[k] = centerY;  // center y
			output[4].value[k] = centerX + dX;  // focus x1
			output[5].value[k] = centerY + dY;  // focus y1
		}
		else {
			double c = sqrt(abs(major * major - minor * minor));
			if (major > minor) {
				dX = c * sin(-orient);
				dY = c * cos(-orient);
			}
			else {
				dX = c * cos(orient);
				dY = c * sin(orient);
			}
			output[2].value[k] = centerX - dX;  // focus x0
			output[3].value[k] = centerY - dY;  // focus y0
			output[4].value[k] = centerX + dX;  // focus x1
			output[5].value[k] = centerY + dY;  // focus y1
		}

		for (int j = 0; j < pathLoops; j++) {
			double pathAngle = getLoopingValueD(5, j, 0);

			if ((rotation & 4) != 0)  
				pathAngle = -pathAngle;
			pathAngle -= PI_OVER_2;

			double px = minor * cos(pathAngle);
			double py = major * sin(pathAngle);

			double rx = px * cosA - py * sinA;
			double ry = px * sinA + py * cosA;

			output[0].value[n] = centerX + rx;
			output[1].value[n] = centerY - ry;
			n++;
		}
	}

	return 1;
}

bool EllipsePath_Component::onLoad(Sheet* pg) {
	if (setting[0] == 0) {
		input[0].label = "Center X";
		input[1].label = "Center Y";
		output[2].label = "Focus 0 X";
		output[3].label = "Focus 0 Y";
	}
	else {
		input[0].label = "Focus 0 X";
		input[1].label = "Focus 0 Y";
		output[2].label = "Center X";
		output[3].label = "Center Y";
	}
	return true;
}
void EllipsePath_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {

	if (setting[0] != radioValue) {
		setting[0] = radioValue;
		onLoad(pg);
		propagateValues(&pg->dgrm);
	}
}
void doubleEllipsePathDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> radioLabels{ "Ellipse on Center", "Ellipse on Focus" };
	int radioIndex = pg->dgrm.cL[i]->setting[0];

	buildMultipleValuesDialog(pg, i, {}, {}, 80,
		{}, {}, radioLabels, radioIndex, NULL);
}

std::vector<Button*>ellipsePathButtonList = { 
	new Button(3, 40, 85, 20, L"Options...", doubleEllipsePathDialog),
	new Button(3, 60, 85, 20, L"Rotation...", rotationActionGeneral) };

EllipsePath_Component::EllipsePath_Component()
	: Component(L"Ellipse", L"Elliptical Path", squareShape, 6, 1, 6, 1)
{
	dialogButtonList = ellipsePathButtonList;
	setLeftLabels(6, "Center X", "Center Y", "Major", "Minor", "Rotation", "Path Angle");
	setRightLabels(6, "Path X", "Path Y", "Focus 0 X", "Focus 0 Y", "Focus 1 X", "Focus 1 Y");
	width = 60;
	setSetting(0, 0); // center
}

// ********************************* Parabola Path *********************************

int ParabolaPath_Component::method2(Sheet* pg) {
	
	int focusLoops = inputsToLoopsN(4);
	if (focusLoops == 0)
		return clearOutputs();

	int pathLoops = input[4].value.size();
	if (pathLoops == 0)
		return clearOutputs();

	for (int j = 0; j < 2; j++) {
		output[j].value.clear();
		if (pathLoops == 1) {
			output[j].value.resize(focusLoops);
			output[j].dimensions.resize(1);
			output[j].dimensions[0] = focusLoops;
		}
		else {
			output[j].value.resize(pathLoops*focusLoops);
			output[j].dimensions.resize(2);
			output[j].dimensions[0] = focusLoops;
			output[j].dimensions[1] = pathLoops;
		}
	}
	
	for (int j = 2; j < 4; j++) {
		output[j].value.resize(focusLoops);
		output[j].dimensions[0] = focusLoops;
	}

	int n = 0;
	for (int k = 0; k < focusLoops; k++) {
		double inputX = getLoopingValueD(0, k, 0);
		double inputY = getLoopingValueD(1, k, 0);
		double focus = getLoopingValueD(2, k, 10);
		double orient = getLoopingValueD(3, k, 0);
		orient = rotateInputAngle(orient);

		if (focus == 0)
			continue;

		double sinA = sin(-orient);
		double cosA = cos(-orient);

		double fX = focus * sinA;
		double fY = focus * cosA;
		output[2].value[k] = inputX + fX;  // focus x0
		output[3].value[k] = inputY + fY;  // focus y0

		for (int j = 0; j < pathLoops; j++) {
			double px = getLoopingValueD(4, j, 0);
			double py = px * px / (-4 * focus);

			double rx = px * cosA - py * sinA;
			double ry = px * sinA + py * cosA;

			output[0].value[n] = inputX + rx;
			output[1].value[n] = inputY - ry;
			n++;
		}
	}
	
	return 1;
}
std::vector<Button*>parabolaPathButtonList = {
	new Button(3, 40, 85, 20, L"Rotation...", rotationActionGeneral) };

ParabolaPath_Component::ParabolaPath_Component()
	: Component(L"Parabola", L"Parabolic Path", squareShape, 5, 1, 4, 1)
{
	dialogButtonList = parabolaPathButtonList;
	setLeftLabels(6, "Vertex X", "Vertex Y", "Focus", "Orientation", "X Input");
	setRightLabels(4, "Path X", "Path Y", "Focus X", "Focus Y");
	width = 55;
	setSetting(0, 0); // set to vertex for expansion to set to focus later
}

