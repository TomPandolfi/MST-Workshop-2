#pragma once

#include "stdafx.h"
#include "structs.h"
#include <vector>



class Planet {
public:
	int     id;
	std::string name;
	double   semi;
	double   sider;
	double   eccen;
	double   radius;
	double   incl;
	int     red;
	int     green;
	int     blue;

	Planet(int _id, std::string _name, double _semi, double _sider, double _eccen, double _radius, double _incl, int _red, int _green, int _blue);
};

class PlanetList {
	std::vector<Planet> planets;
};

// ************* 

class Element {
public:
	int     atomicNumber;
	std::string name;
	std::string symbol;
	double   density;
	double   AMU;
	double   melt;
	double   boil;
	int     neutrons;
	double   radius;
	int     radio;

	Element(int _atomicNumber, std::string _name, std::string _symbol, double _density, double _AMU, double _melt, double _boil, int _neutrons, double _radius, int _radio);
};

class ElementList {
	std::vector<Element> elements;
};

class Planet_Component : public Component { public:	Planet_Component();
	int method2(Sheet* pg);
};
class Period_Component : public Component { public:	Period_Component();
	int method2(Sheet* pg);
};

class Union_Component :			public Component { public:	Union_Component();
	int method2(Sheet* pg);
};
class Intersection_Component :		public Component { public:	Intersection_Component(); 
	int method2(Sheet* pg);
};
class Difference_Component :	public Component { public:	Difference_Component();
	int method2(Sheet* pg);
};
class OnlyA_Component :			public Component { public:	OnlyA_Component();
	int method2(Sheet* pg);
};
class OnlyB_Component :			public Component { public:	OnlyB_Component();
	int method2(Sheet* pg);
};

class UsCity {
public:
	double latitude;
	double longitude;
	string city;
	string state;
	int timeZone;

	UsCity(int _latdeg, int _latmin, char _lathem, int _londeg, int _lonmin, char _lonhem, string _city, string _state, int _timeZone);
};
class UsCity_Component : public Component {public:	UsCity_Component();
		virtual int method2(Sheet* pg);
}; 

class WorldCity {
public:
	double latitude;
	double longitude;
	string city;
	string country;
	int timeZone;

	WorldCity(string _city, string _country, int _latdeg, int _latmin, char _lathem, int _londeg, int _lonmin, char _lonhem, int _timeZone);
};
class WorldCity_Component : public Component {public:	WorldCity_Component();
	int method2(Sheet* pg);
};

class DayOfWeek_Component : public Component {public:	DayOfWeek_Component();
	int method2(Sheet* pg);
};

class DaysInMonth_Component : public Component {public:	DaysInMonth_Component();
	int method2(Sheet* pg);
}; 

class RealTimeClock_Component : public Component {public:	RealTimeClock_Component();
	int method2(Sheet* pg);
};
class Version_Component : public Component {public:	Version_Component();
};

class GraphBaseWords_Component : public Component {
public:	GraphBaseWords_Component();
	int method2(Sheet* pg);
};

class Concatenate_Component : public Component {
public:	Concatenate_Component();
	int method2(Sheet* pg);
};


class MorseCode_Component : public Component {
public:	MorseCode_Component();
	int method2(Sheet* pg);
};


class MIDI_Pitch_Component : public Component {
public:	MIDI_Pitch_Component();
	int method2(Sheet* pg);
};

class InstrumentHarmonics {
public:
	string name;
	int count;
	vector<int> harmonic;
	vector<double> amplitude;
	InstrumentHarmonics(string _name, int _count, vector<int> _harmonic, vector<double> _amplitude);
};
class InstrumentHarmonics_Component : public Component {
public:	InstrumentHarmonics_Component();
	int method2(Sheet* pg);
};