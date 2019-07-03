#pragma once


void stopClock(Sheet* pg);
void resetClock(Sheet* pg);
void stepClock(Sheet* pg);
void startClock(Sheet*);

void displaySystemTime(Sheet* pg);

class Stop_Component :		public Component { 
public:	Stop_Component();
	int method2(Sheet*);
};
class Start_Component :		public Component { 
public:	Start_Component();
	int method2(Sheet*);
};
class Step_Component :		public Component { 
public:	Step_Component();
	int method2(Sheet*);
};
class Reset_Component :		public Component { 
public:	Reset_Component();
	int method2(Sheet*);
};
class Integrator_Component :		public FunctionNMO_Component { 
public:	Integrator_Component(); 
	double innerMethodNMO(Sheet*, double*, double*, double);
	void dial(Sheet*);
};
class Running_Component :	public Component { 
public:	Running_Component();
	int method2(Sheet* pg);
};
class TimeStep_Component : public Component {	
public:	TimeStep_Component();
		~TimeStep_Component();
	int method2(Sheet*);
};
class SquareWave_Component :	public Component { 
public:	SquareWave_Component();
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Sawtooth_Component :		public Component {
public:	Sawtooth_Component(); 
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class TriangleWave_Component :	public Component { 
public:	TriangleWave_Component(); 
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class SineWave_Component :		public Component {
public:	SineWave_Component(); 
	int method2(Sheet* pg);
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Pulse_Component :			public Component {
public:	Pulse_Component();
	int method2(Sheet* pg);
};
class TimeRand_Component : public Component {	
public:	TimeRand_Component();
	int method2(Sheet*);
};
class TrigRand_Component : public Component {	
public:	TrigRand_Component();
	int method2(Sheet*);
};
class OneShot_Component : public Component {
public:	OneShot_Component();
	int method2(Sheet*);
};
class Timer_Component : public Component {
public:	Timer_Component();
	int method2(Sheet*);
};
class Timer2_Component : public Component {
public:	Timer2_Component();
	int method2(Sheet*);
};
class ScalarCounter_Component : public Component {
public:	ScalarCounter_Component();
	int method2(Sheet*);
};