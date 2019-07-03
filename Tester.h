#pragma once
//#include "buildComponent.h"

class Tester_2dPosition_Component :
	public Component {
public:	Tester_2dPosition_Component();
		int method2(Sheet*);
};

class Tester_3dPosition_Component :
	public Component {
public:	Tester_3dPosition_Component();
		int method2(Sheet*);
};


class Tester_2dColor_Component :
	public Component {
public:	Tester_2dColor_Component();
		int method2(Sheet*);
};

class Tester_2Phase_Component :
	public Component {
public:	Tester_2Phase_Component();
		int method2(Sheet*);
};

class Tester_3Phase_Component :
	public Component {
public:	Tester_3Phase_Component();
		int method2(Sheet*);
};

class Tester_Bit_Component :
	public Component {
public:	Tester_Bit_Component();
		int method2(Sheet*);
};