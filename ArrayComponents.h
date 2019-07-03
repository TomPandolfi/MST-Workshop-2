#pragma once
#include "buildComponent.h"



bool crankJoinLimit_(Sheet* pg, Component *comp, int x, int y, bool check, int limit);



class Dimensions_Component :		public Component { 
public: Dimensions_Component(); 

	int method2(Sheet* pg); 
	bool crank(Sheet* pg, int, int, bool); 
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
};
class Table_Component :		public Component {
public:Table_Component();
	bool crank(Sheet* pg, int, int, bool);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Matrix_Component :	public Component {
public:Matrix_Component();
	bool crank(Sheet* pg, int, int, bool);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Count_Component :		public Component {
public:Count_Component();
	int method2(Sheet* pg);
};
class Sum_Component :		public Component {
public:Sum_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class OuterSum_Component :	public Component {
public:OuterSum_Component();
	int method2(Sheet* pg);
};
class OuterProd_Component : public Component {
public:OuterProd_Component();
	int method2(Sheet* pg);
};
class ProdA_Component :		public Component {
public:ProdA_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
};
class Ave_Component :		public Component {
public:Ave_Component();
	int method2(Sheet* pg);
};
class Median_Component :	public Component {
public:	Median_Component();
	int method2(Sheet* pg);
};
class Mode_Component :		public Component {
public:	Mode_Component();
	int method2(Sheet* pg);
};
class Stdev_Component :		public Component {
public:	Stdev_Component();
	int method2(Sheet* pg);
};
class Var_Component :		public Component {
public:	Var_Component();
	int method2(Sheet* pg);
};
class Correlation_Component :		public Component {
public:	Correlation_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
};
class Sums_Component :		public Component {
public:	Sums_Component();
	int method2(Sheet* pg);
};
class Differences_Component :		public Component {
public:	Differences_Component();
	int method2(Sheet* pg);
};
class ArrayRand_Component : public Component {
public:	ArrayRand_Component();
	int method2(Sheet* pg);
};
//class Match_Component :		public Component {
//public:	Match_Component();};

class MatMul_Component :	public Component {
public:	MatMul_Component();
	int method2(Sheet* pg);
};
//class MatInv_Component :	public Component {
//public:	MatInv_Component();};
class MatrixTranspose_Component :	public Component {
public:	MatrixTranspose_Component();
	int method2(Sheet* pg);
};
class MatrixDeterminent_Component :	public Component {
public:	MatrixDeterminent_Component();
	int method2(Sheet* pg);
};
class Mirror_Component :	public Component {
public:	Mirror_Component();
	int method2(Sheet* pg);
};
class Repeat_Component :	public Component {
public:	Repeat_Component(); 
	int method2(Sheet* pg);
};
class Duplicate_Component :		public Component {
public:	Duplicate_Component(); 
	int method2(Sheet* pg);
};
class Min_Component :		public Component {
public:	Min_Component(); 
	int method2(Sheet* pg);
};
class Max_Component :		public Component {
public:	Max_Component(); 
	int method2(Sheet* pg);
};
class Range_Component :		public Component {
public:	Range_Component(); 
	int method2(Sheet* pg);
};
class MaxIndex_Component :	public Component {
public:	MaxIndex_Component(); 
	int method2(Sheet* pg);
};
class MinIndex_Component :	public Component {
public:	MinIndex_Component(); 
	int method2(Sheet* pg);
};
class Small_Component :		public Component {
public:	Small_Component();
	int method2(Sheet* pg);
};
class Large_Component :		public Component {
public:	Large_Component(); 
	int method2(Sheet* pg);
};
class Roll_Component :		public Component {
public:	Roll_Component(); 
	int method2(Sheet* pg);
};
class FirstSet_Component :	public Component {
public:	FirstSet_Component();
	int method2(Sheet* pg);
};
class LastSet_Component :	public Component {
public:	LastSet_Component();
	int method2(Sheet* pg);
};
class SubSet_Component :	public Component {
public:	SubSet_Component(); 
	int method2(Sheet* pg);
};
class Excise_Component :	public Component {
public:	Excise_Component(); 
	int method2(Sheet* pg);
};
class RackZ_Component :		public Component {
public:	RackZ_Component(); 
	int method2(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class RackFacZ_Component :	public Component {
public:	RackFacZ_Component(); 
	int method2(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class RackOffZ_Component :	public Component {
public:	RackOffZ_Component(); 
	int method2(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Synth_Component :		public Component {
public:	Synth_Component(); 
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class PolySynth_Component : public Component {
public:	PolySynth_Component(); 
	int method2(Sheet* pg);
};
class Join_Component :		public Component {
public:	Join_Component(); 
	int method2(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
	void dial(Sheet* pg);
};
class InnerJoin_Component : public Component {
public:	InnerJoin_Component();
		int method2(Sheet* pg);
		bool crank(Sheet* pg, int, int, bool);
		bool onLoad(Sheet* pg);
		void dial(Sheet* pg);
};
class Split_Component :		public Component {
public:	Split_Component(); 
	int method2(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
	void dial(Sheet* pg);
};
class Gather_Component :	public Component { 
public:	Gather_Component(); 
	int method2(Sheet* pg);
	void innerGather(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Mem_Component :		public Component { 
public:	Mem_Component(); 
	int method2(Sheet* pg);
};
//class ListMem_Component :	public Component { 
//public:	ListMem_Component(); 
//virtual int method2(Sheet* pg);
//};
class Literal_Component :		public Component { 
public:	Literal_Component();
	bool crank(Sheet* pg, int, int, bool);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class SpliceIn_Component :  public Component { 
public:	SpliceIn_Component(); 
	int method2(Sheet* pg);
};
class SpliceOut_Component : public Component { 
public:	SpliceOut_Component(); 
	int method2(Sheet* pg);
};
class FunctionGenerator_Component : public Component {	
public:	FunctionGenerator_Component();
	int method2(Sheet* pg);
};
class Keep_Component :		public Component { 
public:	Keep_Component(); 
	int method2(Sheet* pg);
};
class KeepFirst_Component : public Component {		
public:	KeepFirst_Component();
	int method2(Sheet* pg);
};
class KeepLast_Component : public Component {		
public:	KeepLast_Component();
	int method2(Sheet* pg);
};
class GetAt_Component :		public Component { 
public:	GetAt_Component(); 
	int method2(Sheet* pg);
};
class PutAt_Component : public Component {
public:	PutAt_Component();
	int method2(Sheet* pg);
};
class GetUntil_Component : public Component {
public:	GetUntil_Component();
	int method2(Sheet* pg);
};
class MatrixGetAt_Component : public Component { 
public:	MatrixGetAt_Component();
	int method2(Sheet* pg);
};
class Interpolate_Component :	public Component { 
public:	Interpolate_Component(); 
	int method2(Sheet* pg);
};
class SJoin_Component :		public Component { 
public:	SJoin_Component(); 
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class MatrixJoin_Component :		public Component { 
public:	MatrixJoin_Component(); 
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class MatrixSplit_Component :	public Component { 
public:	MatrixSplit_Component(); 
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
};
class Expand_Component :	public Component {
public:	Expand_Component(); 
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool onLoad(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
};
class BitJoin_Component : public Component {
public:  BitJoin_Component(); 
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
};
class BitSplit_Component : public Component {
public:  BitSplit_Component(); 
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
};
class BitsToNumber_Component : public Component {
public:  BitsToNumber_Component();
	int method2(Sheet* pg);
};
class NToBit_Component : public Component { 
public:  NToBit_Component();
	int method2(Sheet* pg);
};
class Histogram_Component : public Component {
public:  Histogram_Component();
	int method2(Sheet* pg);
};
class LinearRegression_Component : public Component {
public:  LinearRegression_Component();
	int method2(Sheet* pg);
};
class LeastSquares_Component : public Component {
public:  LeastSquares_Component();
int method2(Sheet* pg);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class LeastSquaresN_Component : public Component {
public:  LeastSquaresN_Component();
int method2(Sheet* pg);
bool onLoad(Sheet* pg);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Buffer_Component : public Component {
public:  Buffer_Component();
int method2(Sheet* pg);
bool onLoad(Sheet* pg);
void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Str2dWid_Component : public Component {
public:  Str2dWid_Component();
	int method2(Sheet* pg);
};
class Spline_Component : public Component {
public:  Spline_Component();
	int method2(Sheet* pg);
};
class SplineXY_Component : public Component {
public:  SplineXY_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Arrange_Component : public Component {
public:  Arrange_Component();
	int method2(Sheet* pg);
};
class Merge_Component : public Component {
public:  Merge_Component();
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
};
class Shuffle_Component : public Component {
public:  Shuffle_Component();
	int method2(Sheet* pg);
};
class Deal_Component : public Component {
public:  Deal_Component();
	int method2(Sheet* pg);
	void dial(Sheet* pg);
	bool crank(Sheet* pg, int, int, bool);
	bool onLoad(Sheet* pg);
};
class FindIndex_Component : public Component {
public:  FindIndex_Component();
	int method2(Sheet* pg);
};
class FindBool_Component : public Component {
public:  FindBool_Component();
	int method2(Sheet* pg);
};
class Ravel_Component : public Component {
public:  Ravel_Component();
	int method2(Sheet* pg);
};
class Filter_Component : public Component {
public:  Filter_Component();
	int method2(Sheet* pg);
};
class MatrixSubset_Component : public Component {
public:  MatrixSubset_Component();
	int method2(Sheet* pg);
};
class MatrixExcise_Component : public Component {
public:  MatrixExcise_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class MatrixInversion_Component : public Component {
public:  MatrixInversion_Component();
	int method2(Sheet* pg);
};
class NumericalSort_Component : public Component {
public:  NumericalSort_Component();
	int method2(Sheet* pg);
};
class NumericalRank_Component : public Component {
public:  NumericalRank_Component();
	int method2(Sheet* pg);
};
class MatrixInterpolate_Component : public Component {
public:  MatrixInterpolate_Component();
	int method2(Sheet* pg);
};
class IndexToPosition_Component : public Component {
public:  IndexToPosition_Component();
	int method2(Sheet* pg);
}; 
class EliminateValue_Component : public Component {
public:  EliminateValue_Component();
	int method2(Sheet* pg);
};
class Replace_Component : public Component {
public:  Replace_Component();
	int method2(Sheet* pg);
};

class ReadTextFile_Component : public Component {
public:  ReadTextFile_Component();
	int method2(Sheet* pg);
};
class ReadDataFile_Component : public Component {
public:  ReadDataFile_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class WriteTextFile_Component : public Component {
public:  WriteTextFile_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class WriteDataFile_Component : public Component {
public:  WriteDataFile_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class GetFileInfo_Component : public Component {
public:  GetFileInfo_Component();
	int method2(Sheet* pg);
};
class DeleteFile_Component : public Component {
public:  DeleteFile_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class GetDir_Component : public Component {
public:  GetDir_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class CreateFldr_Component : public Component {
public:  CreateFldr_Component();
	int method2(Sheet* pg);
};

class RandReadFile_Component : public Component {
public:  RandReadFile_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class RandWriteFile_Component : public Component {
public:  RandWriteFile_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

