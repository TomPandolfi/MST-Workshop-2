#pragma once
#include "buildComponent.h"



class Add_Component :			public FunctionN_Component { 
public:	Add_Component();
	double innerMethodN(double *a);
};
class Subtract_Component :			public FunctionN_Component { 
public:	Subtract_Component();
	double innerMethodN(double *a);
};
class Mul_Component :			public FunctionN_Component {
public:	Mul_Component();
	double innerMethodN(double *a);
};
class Divide_Component :			public FunctionN_Component { 
public:	Divide_Component();
	double innerMethodN(double *a);
};
class Inv_Component :			public FunctionN_Component { 
public:	Inv_Component();
	double innerMethodN(double *a);
};
class Neg_Component :			public FunctionN_Component { 
public:	Neg_Component();
	double innerMethodN(double *a);
};
class Floor_Component :			public FunctionN_Component { 
public:	Floor_Component();
	double innerMethodN(double *a);
};
class Ceiling_Component :			public FunctionN_Component { 
public:	Ceiling_Component();
	double innerMethodN(double *a);
};
class Exp_Component :			public FunctionN_Component { 
public:	Exp_Component();
	double innerMethodN(double *a);
};
class Pow_Component :			public FunctionN_Component { 
public:	Pow_Component();
	double innerMethodN(double *a);
};
class Log_Component :			public FunctionN_Component { 
public:	Log_Component();
	double innerMethodN(double *a);
};
class Sign_Component :			public FunctionN_Component { 
public:	Sign_Component();
	double innerMethodN(double *a);
};
class Ave2_Component :			public FunctionN_Component { 
public:	Ave2_Component();
	double innerMethodN(double *a);
};
class Abs_Component :			public FunctionN_Component { 
public:	Abs_Component(); 
	double innerMethodN(double *a);
};
class Min2_Component :			public FunctionN_Component { 
public:	Min2_Component(); 
	double innerMethodN(double *a);
};
class Max2_Component :			public FunctionN_Component { 
public:	Max2_Component();
	double innerMethodN(double *a);
};
class Mod_Component :			public FunctionN_Component {
public:	Mod_Component(); 
	double innerMethodN(double *a);
};
class SquareRoot_Component :			public FunctionN_Component {
public:	SquareRoot_Component();
	double innerMethodN(double *a);
};
class Square_Component :		public FunctionN_Component {
public:	Square_Component();
	double innerMethodN(double *a);
};
class Cube_Component :			public FunctionN_Component {
public:	Cube_Component(); 
	double innerMethodN(double *a);
};


class Sine_Component :			public FunctionCN_Component { 
public:	Sine_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};
class Cosine_Component :			public FunctionCN_Component { 
public:	Cosine_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};
class Tan_Component :			public FunctionCN_Component { 
public:	Tan_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};
class ArcSin_Component :			public FunctionCN_Component { 
public:	ArcSin_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};
class ArcCos_Component :			public FunctionCN_Component {
public:	ArcCos_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};
class ArcTan_Component :			public FunctionCN_Component {
public:	ArcTan_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};
class ArcTan2_Component :			public FunctionCN_Component { 
public:	ArcTan2_Component(Sheet* pg);
	double innerMethodCN(Component, double *a);
};


class BitAnd_Component :		public FunctionN_Component { 
public:	BitAnd_Component();
	double innerMethodN(double *a);
};
class BitOr_Component :			public FunctionN_Component { 
public:	BitOr_Component(); 
	double innerMethodN(double *a);
};
class BitNot_Component :		public FunctionN_Component { 
public:	BitNot_Component(); 
	double innerMethodN(double *a);
};
class BitXor_Component :		public FunctionN_Component { 
public:	BitXor_Component(); 
	double innerMethodN(double *a);
};
class InRange_Component :		public FunctionN_Component { 
public:	InRange_Component();
	double innerMethodN(double *a);
};
class Fade_Component :			public FunctionN_Component { 
public:	Fade_Component();
	double innerMethodN(double *a);
};
class Fact_Component :			public FunctionN_Component { 
public:	Fact_Component();
	double innerMethodN(double *a);
};


class Polynomial_Component :	public Component {
public:	Polynomial_Component(); 
	int method2(Sheet* pg);
};
class PolyInteg_Component :		public Component { 
public:	PolyInteg_Component(); 
	int method2(Sheet* pg);
};
class PolyDeriv_Component :		public Component { 
public:	PolyDeriv_Component(); 
	int method2(Sheet* pg);
};


class Dot2Prod_Component :		public FunctionN_Component { 
public:	Dot2Prod_Component(); 
	double innerMethodN(double *a);
};
class Dot3Prod_Component :		public FunctionN_Component {
public:	Dot3Prod_Component(); 
	double innerMethodN(double *a);
};

class CrossProduct_Component :		public Component { 
public:	CrossProduct_Component(); 
	int method2(Sheet* pg);
};

class Flat2dDist_Component :		public FunctionN_Component { 
public:	Flat2dDist_Component(); 
	double innerMethodN(double *a);
};
class SphericalDistance_Component :	public FunctionN_Component { 
public:	SphericalDistance_Component(); 
	double innerMethodN(double *a);
};
class ThreeDDist_Component :	public FunctionN_Component { 
public:	ThreeDDist_Component(); 
	double innerMethodN(double *a);
};

class Counter_Component :		public Component { 
public:	Counter_Component(); 
	int method2(Sheet* pg); 
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
//class SCount_Component :		public FunctionN_Component { 
//public:	SCount_Component(); 
//virtual double innerMethodN(double *a);
//};

class Slope_Component :			public Component { 
public:	Slope_Component(); 
	int method2(Sheet* pg);
};
class IntersectLines_Component :		public Component { 
public:	IntersectLines_Component();
	int method2(Sheet* pg);
};
class IntersectCircles_Component :	public Component { 
public:	IntersectCircles_Component();
	int method2(Sheet* pg);
};
class IntersectLineCircle_Component :	public Component { 
public:	IntersectLineCircle_Component(); 
	int method2(Sheet* pg);
};

class Scale_Component :			public FunctionN_Component { 
public:	Scale_Component(); 
	double innerMethodN(double *a);
};
class LogScale_Component :		public FunctionN_Component {
public:	LogScale_Component();
	double innerMethodN(double *a);
};

class Permute_Component :		public Component {
public:	Permute_Component();
	int method2(Sheet* pg); 
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Permute2_Component :		public Component { 
public:	Permute2_Component();  
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
};
class Permute3_Component :		public Component { 
public:	Permute3_Component(); 
	int method2(Sheet* pg); 
	bool onLoad(Sheet* pg);
};
class AddFrac_Component :		public Fractions_Component {
public:	AddFrac_Component(); 
};
class MulFrac_Component :		public Fractions_Component { 
public:	MulFrac_Component();
};
class DivideFractions_Component :		public Fractions_Component { 
public:	DivideFractions_Component();
};

class Reduce_Component : public Component {
public:	Reduce_Component();
	int method2(Sheet* pg);
};
class Factor_Component : public Component {
public:	Factor_Component();
	int method2(Sheet* pg);
};
class CommonFactor_Component : public Component {
public:	CommonFactor_Component();
	int method2(Sheet* pg);
};
class EllipsePath_Component : public Component {
public:	EllipsePath_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class ParabolaPath_Component : public Component {
public:	ParabolaPath_Component();
	int method2(Sheet* pg);
};