#pragma once

double reasonableValue(double in);
//bool commonCrankSld_(Sheet* pg, int i, int x, int y, bool check);
void commonDialGrid(Sheet* pg, Component *comp);
void commonDialCon(Sheet* pg, Component *comp);
bool commonCrankTable(Sheet* pg, Component *comp, int x, int y, bool check);

double overScale(double value, double scale);
void drawResizeBox(Sheet* pg, Component *comp);

double detentValue(double inputValue, double detentValue);

// for the functions these are the inner workings

double funcDetent(double value, double detent);


void dialNull_(Sheet*, int);

void dialGenericLogic_(Sheet*, int);




class Round_Component :			public FunctionN_Component { 
public:	Round_Component();
	double innerMethodN(double *a);
};
class Precision_Component :		public FunctionN_Component { 
public:	Precision_Component(); 
	double innerMethodN(double *a);
};
class Detent_Component :		public FunctionN_Component {
public:	Detent_Component();
	double innerMethodN(double *a);
};
class Reasonable_Component :	public FunctionN_Component {
public:	Reasonable_Component();
	double innerMethodN(double *a);
};
class Limit_Component :			public FunctionN_Component {
public:	Limit_Component(); 
	double innerMethodN(double *a);
};
class Flip_Component :			public FunctionNMO_Component {
public:	Flip_Component(); 
	double innerMethodNMO(Sheet*, double*, double*, double);
};
class DFlipFlop_Component :			public FunctionNMO_Component {
public:	DFlipFlop_Component(); 
	double innerMethodNMO(Sheet*, double*, double*, double);
};
class TriggeredSum_Component :		public FunctionNMO_Component {
public:	TriggeredSum_Component();
	double innerMethodNMO(Sheet*, double*, double*, double);
};

