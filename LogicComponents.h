#pragma once

#include "buildComponent.h"





class LogicComponent : public FunctionN_Component {
public:
	LogicComponent(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	virtual void dial(Sheet* pg);
};




class GT_Component :	public LogicComponent {public:	GT_Component();
	double innerMethodN(double*);
};
class LessThan_Component :	public LogicComponent {public:	LessThan_Component();
	double innerMethodN(double*);
};
class EQ_Component :	public LogicComponent {public:	EQ_Component();
	double innerMethodN(double*);
};
class GTE_Component :	public LogicComponent {public:	GTE_Component();
	double innerMethodN(double*);
};
class LessThanOrEqual_Component :	public LogicComponent {public:	LessThanOrEqual_Component();
	double innerMethodN(double*);	
};
class NEQ_Component :	public LogicComponent {public:	NEQ_Component();
	double innerMethodN(double*);	
};
class GTZ_Component :	public LogicComponent {public:	GTZ_Component();
	double innerMethodN(double*);	
};
class LessThanZero_Component :	public LogicComponent {public:	LessThanZero_Component();
	double innerMethodN(double*);
};
class EQZ_Component :	public LogicComponent {public:	EQZ_Component();
	double innerMethodN(double*);
};
class GTEZ_Component :	public LogicComponent {public:	GTEZ_Component(); 
	double innerMethodN(double*);
};
class LTEZ_Component :	public LogicComponent {public:	LTEZ_Component(); 
	double innerMethodN(double*);
};
class NEQZ_Component :	public LogicComponent {public:	NEQZ_Component(); 
	double innerMethodN(double*);
};
class And_Component :	public LogicComponent {public:	And_Component(); 
	double innerMethodN(double*);
};
class Nand_Component :	public LogicComponent {public:	Nand_Component();
	double innerMethodN(double*);
};
class Or_Component :	public LogicComponent {public:	Or_Component();
	double innerMethodN(double*);
};
class Nor_Component :	public LogicComponent {public:	Nor_Component();
	double innerMethodN(double*);
};
class Xor_Component :	public LogicComponent {public:	Xor_Component();
	double innerMethodN(double*);
};
class Not_Component :	public LogicComponent {public:	Not_Component();
	double innerMethodN(double*);
};


void logicSetInputCount(Sheet*, int, int);
void logicSetOutputCount(Sheet*, int, int);
void logicSetLogicsCount(Sheet*, int, int);

class Logic_Component : public Component {		public:	Logic_Component();
	int method2(Sheet*);
	bool onLoad(Sheet*);
	vector<vector<bool>> inputMemory;
	bool getInputVal(int n, int loop);
};
//class Logic2_Component : public Component {
//public:	Logic2_Component();
//	int method2(Sheet*);
//	bool onLoad(Sheet*);
//};
//

void dtoASetInputCount(Sheet*, int, int);
void dtoASetLogicsCount(Sheet*, int, int);

class DtoA_Component : public Component {		public:  DtoA_Component();
	int method2(Sheet*);
	bool onLoad(Sheet*);
	vector<vector<bool>> inputMemory;
	bool getInputVal(int n, int loop);
};
