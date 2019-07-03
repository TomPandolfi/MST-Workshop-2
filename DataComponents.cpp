#include "stdafx.h"
#include <cmath>
#include "structs.h"
#include "Sheet.h"
#include "DataComponents.h"
#include "FunctionComponents.h"
#include <time.h>

#pragma warning (disable: 4267)
#pragma warning (disable: 4244)


Version_Component::Version_Component()
	: Component(L"Version", L"Version", squareShape, 0, 1, 1, 1)
{
	output[0].value[0] = THIS_PROGRAM_VERSION;
	output[0].dimensions[0] = 1;
}

// ********************************** Planets ********************************

Planet::Planet(int _id, std::string _name, double _semi, double _sider, double _eccen, double _radius, double _incl, int _red, int _green, int _blue) {
	id =     _id;
	name =   _name;
	semi =   _semi;
	sider =  _sider;
	eccen =  _eccen;
	radius = _radius;
	incl =   _incl;
	red =    _red;
	green =  _green;
	blue =   _blue;
}


std::vector<Planet*>planetList{
	new Planet(0, "Sun", 0, 0, 0, 696000.0, 0.0, 255, 166, 54),
	new Planet(1, "Mercury", 57.91e6, 87.969, 0.206, 2420.0, 0.0, 181, 177, 176),
	new Planet(2, "Venus", 108.21e6, 224.700, 0.007, 6100.0, 23.0, 240, 216, 182),
	new Planet(3, "Earth", 149.60e6, 365.257, 0.017, 6378.0, 23.45, 131, 138, 182),
	new Planet(4, "Mars", 227.94e6, 686.980, 0.093, 3380.0, 23.98333, 192, 172, 139),
	new Planet(5, "Jupiter", 778.3e6, 4332.587, 0.048, 71350.0, 3.08333, 211, 212, 155),
	new Planet(6, "Saturn", 1427.0e6, 10759.20, 0.056, 60400.0, 26.73333, 216, 218, 169),
	new Planet(7, "Uranus", 2869.0e6, 30685.0, 0.047, 23800.0, 97.91667, 172, 200, 211),
	new Planet(8, "Neptune", 4498.0e6, 60188.0, 0.009, 22200.0, 28.8, 148, 170, 207),
	new Planet(9, "Pluto", 5900.0e6, 90700.0, 0.249, 3000.0, 0.0, 211, 202, 168)
};



int Planet_Component::method2(Sheet* pg) {
	
	if (input[0].drivenByComponent == -1)
		return clearOutputs();
	
	int len = planetList.size();
	int planets = input[0].value.size();
	int j, k;

	output[0].stringType = true;
	output[0].stringValue.resize(planets);
	output[0].dimensions[0] = planets;

	for (k = 1; k < rightTerminalCount; k++) {
		output[k].value.resize( planets);
		output[k].dimensions[0] = planets;
	}
	
	for (j = 0; j < planets; j++) {
		int planetID = floor(input[0].value[j]);

		if ((planetID < 0) || (planetID >= len)) {
			output[0].stringValue[j] = "";
			for (k = 1; k < rightTerminalCount; k++)
				output[k].value[j] = 0;
		}
		else {
			output[0].stringValue[j] = planetList[planetID]->name;
			output[1].value[j] = planetList[planetID]->semi;
			output[2].value[j] = planetList[planetID]->sider;
			output[3].value[j] = planetList[planetID]->eccen;
			output[4].value[j] = planetList[planetID]->radius;
			output[5].value[j] = planetList[planetID]->incl;
			output[6].value[j] = planetList[planetID]->red;
			output[7].value[j] = planetList[planetID]->green;
			output[8].value[j] = planetList[planetID]->blue;
		}
	}
	return true;
}


Planet_Component::Planet_Component()
	: Component(L"Planet", L"Planet", squareShape, 1, 1, 9, 1)
{
	setRightLabels(9, "Name", "Semimajor Axis (km)", "Sidreral Period (Days)", "Eccentricity",
	"Equatorial Radius (km)", "Inclination (deg)", "Red", "Green", "Blue");
	width = 65;
}

//*********************** Periodic table **************************

Element::Element(int _atomicNumber, std::string _name, std::string _symbol, double _density, double _AMU, double _melt, double _boil, int _neutrons, double _radius, int _radio){
	atomicNumber= _atomicNumber;
	name		= _name;
	symbol		= _symbol;
	density		= _density;
	AMU			= _AMU;
	melt		= _melt;
	boil		= _boil;
	neutrons	= _neutrons;
	radius		= _radius;
	radio		= _radio;
}					  


std::vector<Element*>elementList{
	new Element(1, "Hydrogen", "H", 0.08988, 1.00794, -259.14, -252.87, 0, 25, 0),  // [1, 2]),
	new Element(2, "Helium", "He", 0.1787, 4.0026, -272, -268.6, 2, 0, 0),  // [3, 4]),
	new Element(3, "Lithium", "Li", 0.53, 6.941, 180.54, 1347, 4, 145, 0),  // [6, 7]),
	new Element(4, "Beryllium", "Be", 1.8477, 9.01218, 1278, 2970, 5, 105, 0),  // [9]),
	new Element(5, "Boron", "B", 2.34, 10.811, 2300, 2550, 6, 85, 0),  // [10, 11]),
	new Element(6, "Carbon", "C", 2.62, 12.0107, 3500, 4827, 6, 70, 0),  // [12, 13]),
	new Element(7, "Nitrogen", "N", 1.2506, 14.0067, -209.9, -195.8, 7, 65, 0),  // [14, 15]),
	new Element(8, "Oxygen", "O", 1.429, 15.9994, -218.4, -183, 8, 60, 0),  // [16, 17, 18]),
	new Element(9, "Fluorine", "Fl", 1.696, 18.9984, -219.62, -188.14, 10, 50, 0),  // [19]),
	new Element(10, "Neon", "Ne", 0.901, 20.1797, -248.6, -246.1, 10, 0, 0),  // [20, 21, 22]),
	new Element(11, "Sodium", "Na", 0.971, 22.9898, 97.8, 552.9, 12, 180, 0),  // [23]),
	new Element(12, "Magnesium", "Mg", 1.738, 24.305, 650, 1107, 12, 150, 0),  // [24, 25, 26]),
	new Element(13, "Aluminum", "Al", 2.702, 26.9815, 660.37, 2467, 14, 125, 0),  // [27]),
	new Element(14, "Silicon", "Si", 2.329, 28.0855, 1410, 2355, 14, 110, 0),  // [28, 29, 30]),
	new Element(15, "Phosphorus", "P", 1.82, 30.9738, 44.1, 280, 16, 100, 0),  // [31]),
	new Element(16, "Sulfur", "S", 2.07, 32.066, 112.8, 444.6, 16, 100, 0),  // [32, 33, 34, 36]),
	new Element(17, "Chlorine", "Cl", 3.214, 35.453, -100.98, -34.6, 18, 100, 0),  // [35, 37]),
	new Element(18, "Argon", "Ar", 1.7824, 39.948, -189.3, -186, 22, 0, 0),  // [36, 38, 40]),
	new Element(19, "Potassium", "K", 0.862, 39.0983, 63.65, 774, 20, 220, 0),  // [39, 41]),
	new Element(20, "Calcium", "Ca", 1.55, 40.078, 839, 1484, 20, 180, 0),  // [40, 42, 43, 44, 46]),
	new Element(21, "Scandium", "Sc", 2.989, 44.9559, 1539, 2832, 24, 160, 0),  // [45]),
	new Element(22, "Titanium", "Ti", 4.54, 47.867, 1660, 3287, 26, 140, 0),  // [46, 47, 48, 49, 50]),
	new Element(23, "Vanadium", "V", 5.8, 50.9415, 1890, 3380, 28, 135, 0),  // [51]),
	new Element(24, "Chromium", "Cr", 7.19, 51.9961, 1857, 2672, 28, 140, 0),  // [50, 52, 53, 54]),
	new Element(25, "Manganese", "Mn", 7.43, 54.938, 1245, 1962, 30, 140, 0),  // [55]),
	new Element(26, "Iron", "Fe", 7.86, 55.845, 1535, 2750, 30, 140, 0),  // [54, 56, 57, 58]),
	new Element(27, "Cobalt", "Co", 8.9, 58.9332, 1495, 2870, 32, 135, 0),  // [59]),
	new Element(28, "Nickel", "Ni", 8.902, 58.6934, 1453, 2732, 31, 135, 0),  // [58, 60, 61, 62, 64]),
	new Element(29, "Copper", "Cu", 8.96, 63.546, 1083, 2567, 35, 135, 0),  // [63, 65]),
	new Element(30, "Zinc", "Zn", 7.133, 65.39, 419.58, 907, 35, 135, 0),  // [70]),
	new Element(31, "Gallium", "Ga", 5.907, 69.723, 29.78, 2403, 39, 130, 0),  // [69, 71]),
	new Element(32, "Germanium", "Ge", 5.323, 72.61, 937.4, 2830, 41, 125, 0),  // [70, 72, 73, 74]),
	new Element(33, "Arsenic", "As", 5.72, 74.9216, 817, 613, 42, 115, 0),  // [75]),
	new Element(34, "Selenium", "Se", 4.79, 78.96, 217, 684.9, 45, 115, 0),  // [74, 76, 77, 78, 80]),
	new Element(35, "Bromine", "Br", 3.119, 79.904, -7.2, 58.78, 45, 115, 0),  // [79, 81]),
	new Element(36, "Krypton", "Kr", 3.74, 83.8, -157.2, -153.4, 48, 0, 0),  // [78, 80, 82, 83, 84, 86]),
	new Element(37, "Rubidium", "Rb", 1.532, 85.4678, 38.89, 688, 48, 235, 0),  // [85]),
	new Element(38, "Strontium", "Sr", 2.54, 87.62, 769, 1384, 50, 200, 0),  // [84, 86, 87, 88]),
	new Element(39, "Yttrium", "Y", 4.469, 88.9059, 1523, 3337, 50, 180, 0),  // [89]),
	new Element(40, "Zirconium", "Zr", 6.49, 91.224, 1852, 4377, 51, 155, 0),  // [90, 91, 92, 94]),
	new Element(41, "Niobium", "Nb", 8.57, 92.9066, 2468, 4927, 52, 145, 0),  // [93]),
	new Element(42, "Molybdenum", "Mo", 10.22, 95.94, 2617, 4612, 54, 145, 0),  // [92, 94, 95, 96, 97, 98]),
	new Element(43, "Technetium", "Tc", 11.5, 98, 2200, 4877, 55, 135, 1),  // []),
	new Element(44, "Ruthenium", "Ru", 12.2, 101.07, 2250, 3900, 57, 130, 0),  // [96, 98, 99, 100, 101, 102, 104]),
	new Element(45, "Rhodium", "Rh", 12.41, 102.906, 1966, 3727, 58, 135, 0),  // [103]),
	new Element(46, "Palladium", "Pd", 12.02, 106.42, 1552, 2927, 60, 140, 0),  // [102, 104, 105, 106, 108, 110]),
	new Element(47, "Silver", "Ag", 10.5, 107.868, 961.93, 2212, 61, 135, 0),  // [107, 109]),
	new Element(48, "Cadmium", "Cd", 8.65, 112.411, 320.9, 765, 64, 155, 0),  // [106, 108, 110, 111, 112, 114]),
	new Element(49, "Indium", "In", 7.31, 114.818, 156.61, 2000, 66, 155, 0),  // [113]),
	new Element(50, "Tin", "Sn", 7.26, 118.71, 231.9, 2270, 69, 145, 0),  // [112, 114, 115, 116, 117, 118, 119, 120, 122, 124]),
	new Element(51, "Antimony", "Sb", 6.684, 121.76, 630, 1750, 71, 145, 0),  // [121, 123]),
	new Element(52, "Tellurium", "Te", 6.24, 127.6, 449.5, 989.8, 76, 140, 0),  // [120, 122, 124, 125, 126]),
	new Element(53, "Iodine", "I", 4.93, 126.9, 113.5, 184, 74, 140, 0),  // [127]),
	new Element(54, "Xenon", "Xe", 5.8971, 131.29, -111.9, -108.1, 77, 0, 0),  // [124, 126, 128, 129, 130, 131, 132, 134, 136]),
	new Element(55, "Cesium", "Cs", 1.873, 132.905, 28.5, 678.4, 78, 260, 0),  // [133]),
	new Element(56, "Barium", "Ba", 3.51, 137.327, 725, 1140, 81, 215, 0),  // [130, 132, 134, 135, 136, 137, 138]),
	new Element(57, "Lanthanum", "La", 6.7, 138.905, 920, 3469, 82, 195, 0),  // [139]),
	new Element(58, "Cerium", "Ce", 6.773, 140.116, 795, 3257, 82, 185, 0),  // [136, 138, 140, 142]),
	new Element(59, "Praseodymium", "Pr", 6.77, 140.908, 935, 2127, 82, 185, 0),  // [141]),
	new Element(60, "Neodymium", "Nd", 7.007, 144.24, 1010, 3127, 84, 185, 0),  // [142, 143, 145, 146, 148]),
	new Element(61, "Promethium", "Pm", 6.475, 145, NULL, NULL, 84, 185, 1),  // []),
	new Element(62, "Samarium", "Sm", 7.54, 150.36, 1072, 1900, 88, 185, 0),  // [144, 149, 150, 152, 154]),
	new Element(63, "Europium", "Eu", 5.259, 151.964, 822, 1597, 89, 185, 0),  // [151, 153]),
	new Element(64, "Gadolinium", "Gd", 7.895, 157.25, 1311, 3233, 93, 180, 0),  // [154, 155, 156, 157, 158, 160]),
	new Element(65, "Terbium", "Tb", 8.27, 158.925, 1360, 3041, 94, 175, 0),  // [159]),
	new Element(66, "Dysprosium", "Dy", 8.536, 162.5, 1412, 2562, 97, 175, 0),  // [156, 158, 160, 161, 162, 163, 164]),
	new Element(67, "Holmium", "Ho", 8.54, 164.93, 1470, 2720, 98, 175, 0),  // [165]),
	new Element(68, "Erbium", "Er", 8.795, 167.26, 1522, 2510, 99, 175, 0),  // [162, 164, 166, 167, 168, 170]),
	new Element(69, "Thulium", "Tm", 9.321, 168.934, 1545, 1727, 100, 175, 0),  // [169]),
	new Element(70, "Yttergium", "Yb", 6.98, 173.04, 824, 1466, 103, 175, 0),  // [168, 170, 171, 172, 173, 174, 176]),
	new Element(71, "Lutelium", "Lu", 9.85, 174.967, 1656, 3315, 104, 175, 0),  // [175]),
	new Element(72, "Hafnium", "Hf", 13.2, 178.49, 2150, 5400, 106, 155, 0),  // [176, 177, 178, 179, 180]),
	new Element(73, "Tantalum", "Ta", 16.654, 180.948, 2996, 5425, 108, 145, 0),  // [181]),
	new Element(74, "Tungsten", "W", 19.3, 183.84, 3410, 5660, 110, 135, 0),  // [180, 182, 183, 184, 186]),
	new Element(75, "Rhenium", "Re", 21.02, 186.207, 3180, 5627, 111, 135, 0),  // [185]),
	new Element(76, "Osmium", "Os", 22.4, 190.23, 3045, 5027, 114, 130, 0),  // [184, 187, 188, 189, 190, 192]),
	new Element(77, "Iridium", "Ir", 22.5, 192.217, 2410, 4527, 115, 135, 0),  // [191, 193]),
	new Element(78, "Platinum", "Pt", 21.45, 195.078, 1772, 3827, 117, 135, 0),  // [192, 194, 195, 196, 198]),
	new Element(79, "Gold", "Au", 19.32, 196.967, 1064.43, 2807, 118, 135, 0),  // [197]),
	new Element(80, "Mercury", "Hg", 13.456, 200.59, -38.87, 356.58, 121, 150, 0),  // [196, 198, 199, 200, 201, 202, 204]),
	new Element(81, "Thalium", "Tl", 11.85, 204.383, 303.5, 1457, 123, 190, 0),  // [203, 205]),
	new Element(82, "Lead", "Pb", 11.34, 207.2, 327.5, 1740, 125, 180, 0),  // [204, 206, 207, 208]),
	new Element(83, "Bismuth", "Bi", 9.8, 208.98, 271.3, 1560, 126, 160, 0),  // []),
	new Element(84, "Polonium", "Po", 9.4, 209, 254, 962, 125, 190, 1),  // []),
	new Element(85, "Astatine", "At", NULL, 210, 302, 337, 125, 0, 1),  // []),
	new Element(86, "Radon", "Rn", 9.73, 222, -71, -61.8, 136, 0, 1),  // []),
	new Element(87, "Francium", "Fr", NULL, 223, 27, 677, 136, 0, 1),  // []),
	new Element(88, "Radium", "Ra", 5, 226, 700, 1737, 138, 215, 1),  // []),
	new Element(89, "Actinium", "Ac", 10.07, 227, 1050, 3200, 138, 195, 1),  // []),
	new Element(90, "Thorium", "Th", 11.72, 232.038, 1750, 4790, 142, 180, 1),  // [232]),
	new Element(91, "Protactinium", "Pa", 15.4, 231.036, 1600, NULL, 140, 180, 1),  // []),
	new Element(92, "Uranium", "U", 18.95, 238.029, 1132, 3818, 146, 175, 1),  // []),
	new Element(93, "Neptunium", "Np", 20.45, 237, 640, 3902, 144, 175, 1),  // []),
	new Element(94, "Plutonium", "Pu", 19.84, 244, 639.5, 3235, 150, 175, 1),  // []),
	new Element(95, "Americium", "Am", 13.6, 243, 994, 2607, 148, 175, 1),  // []),
	new Element(96, "Curium", "Cm", 13.511, 247, 1340, NULL, 151, 0, 1),  // []),
	new Element(97, "Berkelium", "Bk", NULL, 247, NULL, NULL, 150, 0, 1),  // []),
	new Element(98, "Californium", "Cf", NULL, 251, NULL, NULL, 153, 0, 1),  // []),
	new Element(99, "Einsteinium", "Es", NULL, 252, NULL, NULL, 153, 0, 1),  // []),
	new Element(100, "Fermium", "Fm", NULL, 257, NULL, NULL, 157, 0, 1),  // []),
	new Element(101, "Mendelevium", "Md", NULL, 258, NULL, NULL, 157, 0, 1),  // []),
	new Element(102, "Nobelium", "No", NULL, 259, NULL, NULL, 157, 0, 1),  // []),
	new Element(103, "Lawrencium", "Lr", NULL, 262, NULL, NULL, 159, 0, 1),  // []),
	new Element(104, "Rutherfordium", "Rf", NULL, 261, NULL, NULL, 157, 0, 1),  // []),
	new Element(105, "Dubnium", "Db", NULL, 262, NULL, NULL, 157, 0, 1),  // []),
	new Element(106, "Seaborgium", "Sg", NULL, 263, NULL, NULL, 157, 0, 1),  // []),
	new Element(107, "Bohrium", "Bh", NULL, 262, NULL, NULL, 155, 0, 1),  // []),
	new Element(108, "Hassium", "Hs", NULL, 265, NULL, NULL, 157, 0, 1),  // []),
	new Element(109, "Meitnerium", "Mt", NULL, 266, NULL, NULL, 157, 0, 1),  // []),
	new Element(110, "Darmstadtium", "Ds", NULL, 281, NULL, NULL, 159, 0, 1),  // []),
	new Element(111, "Roentgenium", "Rg", NULL, 273, NULL, NULL, 162, 0, 1),  // []),
	new Element(112, "Copernicium", "Cn", NULL, 277, NULL, NULL, 165, 0, 1),  // []),
	new Element(113, "Ununtrium", "Uut", NULL, 286, NULL, NULL, 173, 0, 1),  // []),
	new Element(114, "Flerovium", "Fl", NULL, 289, NULL, NULL, 175, 0, 1),  // []),
	new Element(115, "Ununpentium", "Uup", NULL, 288, NULL, NULL, 173, 0, 1),  // []),
	new Element(116, "Livermorium", "Lv", NULL, 292, NULL, NULL, 176, 0, 1),  // []),
	new Element(117, "Ununseptium", "Uus", NULL, 292, NULL, NULL, 175, 0, 1),  // []),
	new Element(118, "Ununoctium", "Uuo", NULL, 293, NULL, NULL, 175, 0, 1)  // [])
};



int Period_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	int len = elementList.size();
	int elements = input[0].value.size();
	int j, k;

	output[0].stringType = true;
	output[0].stringValue.resize(elements);
	output[1].stringType = true;
	output[1].stringValue.resize(elements);
	for (k = 2; k < rightTerminalCount; k++) 
		output[k].value.resize(elements);

	for (k = 0; k < rightTerminalCount; k++) 
		output[k].dimensions[0] = elements;
	
	for (j = 0; j < elements; j++) {
		int atomicNumber = floor(input[0].value[j]);

		if ((atomicNumber < 1) || (atomicNumber >= len+1)) {
			output[0].stringValue[j] = "";
			output[1].stringValue[j] = "";
			for (k = 2; k < rightTerminalCount; k++)
				output[k].value[j] = 0;
		}
		else {
			int index = atomicNumber - 1;
			if (elementList[index]->atomicNumber != atomicNumber)
				return clearOutputs();

			output[0].stringValue[j] = elementList[index]->name;
			output[1].stringValue[j] = elementList[index]->symbol;
			output[2].value[j] = elementList[index]->density;
			output[3].value[j] = elementList[index]->AMU;
			output[4].value[j] = elementList[index]->melt;
			output[5].value[j] = elementList[index]->boil;
			output[6].value[j] = elementList[index]->neutrons;
			output[7].value[j] = elementList[index]->radius;
			output[8].value[j] = elementList[index]->radio;
		}
	}
	return true;
}

Period_Component::Period_Component()
	: Component(L"Period", L"Periodic", squareShape, 1, 1, 9, 1)
{
	setRightLabels(9, "Name", "Symbol", "Density", "AMU", "Melt", "Boil", "Neutrons", "Radius", "Radioactive");
	width = 35;
}

// ******************************** Sets **************************************

// ***************** Union
int Union_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // data A
		return clearOutputs();

	if (input[1].drivenByComponent == -1)    // data B
		return clearOutputs();

	if (input[0].stringType != input[1].stringType)
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();

	int jin;
	int k;
	bool found;

	if (input[0].stringType) {
		// first pull in all the items in A
		for (jin = 0; jin < input[0].stringValue.size(); jin++) {
			found = false; // check the output, we dont want duplicates
			for (k = 0; k < output[0].stringValue.size(); k++) {
				if (input[0].stringValue[jin] == output[0].stringValue[k]) {
					found = true;
					break;
				}
			}
			if (!found) // Not Found, add it to the output
				output[0].stringValue.push_back(input[0].stringValue[jin]);
		}
		// then pull in all the items in B
		for (jin = 0; jin < input[1].stringValue.size(); jin++) {
			found = false; // again no check for duplicates
			for (k = 0; k < output[0].stringValue.size(); k++) {
				if (input[1].stringValue[jin] == output[0].stringValue[k]) {
					found = true;
					break;
				}
			}
			if (!found) // no duplicates, add it to the output
				output[0].stringValue.push_back(input[1].stringValue[jin]);
		}
		output[0].stringType = true;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].stringValue.size();
	}
	else
	{
		// first pull in all the items in A
		for (jin = 0; jin < input[0].value.size(); jin++) {
			found = false;
			for (k = 0; k < output[0].value.size(); k++) {
				if (input[0].value[jin] == output[0].value[k]) {
					found = true;
					break;
				}
			}
			if (!found)  // add it to the output
				output[0].value.push_back(input[0].value[jin]);
		}
		// then pull in all the items in B
		for (jin = 0; jin < input[1].value.size(); jin++) {
			found = false;
			for (k = 0; k < output[0].value.size(); k++) {
				if (input[1].value[jin] == output[0].value[k]) {
					found = true;
					break;
				}
			}
			if (!found) // add it to the output
				output[0].value.push_back( input[1].value[jin]);
		}
		output[0].stringType = false;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].value.size();
	}
	
	return true;
}
Union_Component::Union_Component()
	: Component(L"Union", L"Union", 2, 1) {
	setLeftLabels(3, "A", "B");
}

// *********************** Intersection

int Intersection_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // data A
		return clearOutputs();

	if (input[1].drivenByComponent == -1)    // data B
		return clearOutputs();

	if (input[0].stringType != input[1].stringType)
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();

	bool found;
	int k;
	if (input[0].stringType) {

		for (int ja = 0; ja < input[0].stringValue.size(); ja++) {
			found = false; // search for it in B
			for (int jb = 0; jb < input[1].stringValue.size(); jb++) {
				if (input[0].stringValue[ja] == input[1].stringValue[jb]) {
					found = true; // ok, its in both A and B inputs
					break;
				}
			}
			if (found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].stringValue.size(); k++) {
					if (input[0].stringValue[ja] == output[0].stringValue[k]) {
						found = true;
						break;
					}
				}
				if (!found) // not a duplicate, add it to the output
					output[0].stringValue.push_back( input[0].stringValue[ja]);
			}
		}
		output[0].stringType = true;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].stringValue.size();
	}
	else
	{
		for (int ja = 0; ja < input[0].value.size(); ja++) {
			found = false;
			for (int jb = 0; jb < input[1].value.size(); jb++) {
				if (input[0].value[ja] == input[1].value[jb]) {
					found = true;
					break;
				}
			}
			if (found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].value.size(); k++) {
					if (input[0].value[ja] == output[0].value[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].value.push_back( input[0].value[ja]);
			}
		}
		output[0].stringType = false;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].value.size();
	}
	return true;

}
Intersection_Component::Intersection_Component()
	: Component(L"Inter", L"Intersection", 2, 1) {
	setLeftLabels(3, "A", "B");
}


// *************** Difference 
int Difference_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // data A
		return clearOutputs();

	if (input[1].drivenByComponent == -1)    // data B
		return clearOutputs();

	if (input[0].stringType != input[1].stringType)
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();

	bool found;
	int k;
	int ja, jb;

	if (input[0].stringType) {
		// first get all the items unique to A
		for (int ja = 0; ja < input[0].stringValue.size(); ja++) {
			found = false;
			for (jb = 0; jb < input[1].stringValue.size(); jb++) {
				if (input[0].stringValue[ja] == input[1].stringValue[jb]) {
					found = true;
					break;
				}
			}
			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].stringValue.size(); k++) {
					if (input[0].stringValue[ja] == output[0].stringValue[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].stringValue.push_back(input[0].stringValue[ja]);
			}
		}

		// then all all the items unique to B
		for (jb = 0; jb < input[1].stringValue.size(); jb++) {
			found = false;
			for (ja = 0; ja < input[0].stringValue.size(); ja++) {
				if (input[1].stringValue[jb] == input[0].stringValue[ja]) {
					found = true;
					break;
				}
			}

			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].stringValue.size(); k++) {
					if (input[1].stringValue[jb] == output[0].stringValue[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].stringValue.push_back( input[1].stringValue[jb]);
			}
		}
		output[0].stringType = true;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].stringValue.size();
	}
	else
	{
		// first get all the items unique to A
		for (int ja = 0; ja < input[0].value.size(); ja++) {
			found = false;
			for (jb = 0; jb < input[1].value.size(); jb++) {
				if (input[0].value[ja] == input[1].value[jb]) {
					found = true;
					break;
				}
			}
			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].value.size(); k++) {
					if (input[0].value[ja] == output[0].value[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].value.push_back( input[0].value[ja]);
			}
		}

		// then all all the items unique to B
		for (jb = 0; jb < input[1].value.size(); jb++) {
			found = false;
			for (ja = 0; ja < input[0].value.size(); ja++) {
				if (input[1].value[jb] == input[0].value[ja]) {
					found = true;
					break;
				}
			}

			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].value.size(); k++) {
					if (input[1].value[jb] == output[0].value[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].value.push_back( input[1].value[jb]);
			}
		}
		output[0].stringType = false;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].value.size();
	}
	return true;

}
Difference_Component::Difference_Component()
	: Component(L"SetDiff", L"Difference", 2, 1){
	setLeftLabels(3, "A", "B");
}

// *************** Only in A
int OnlyA_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // data A
		return clearOutputs();

	if (input[1].drivenByComponent == -1)    // data B
		return clearOutputs();

	if(input[0].stringType != input[1].stringType)
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();

	bool found;
	int k;
	if (input[0].stringType) {
		for (int ja = 0; ja < input[0].stringValue.size(); ja++) {
			found = false;
			for (int jb = 0; jb < input[1].stringValue.size(); jb++) {
				if (input[0].stringValue[ja] == input[1].stringValue[jb]) {
					found = true;
					break;
				}
			}
			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].stringValue.size(); k++) {
					if (input[0].stringValue[ja] == output[0].stringValue[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].stringValue.push_back( input[0].stringValue[ja]);
			}
		}
		output[0].stringType = true;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].stringValue.size();
	}
	else
	{
		for (int ja = 0; ja < input[0].value.size(); ja++) {
			found = false;
			for (int jb = 0; jb < input[1].value.size(); jb++) {
				if (input[0].value[ja] == input[1].value[jb]) {
					found = true;
					break;
				}
			}
			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].value.size(); k++) {
					if (input[0].value[ja] == output[0].value[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].value.push_back( input[0].value[ja]);
			}
		}
		output[0].stringType = false;
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].value.size();
	}
	return true;

}
OnlyA_Component::OnlyA_Component()
	: Component(L"OnlyA", L"Only in A", 2, 1)
{
	setLeftLabels(3, "A", "B");
}

int OnlyB_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)    // data A
		return clearOutputs();

	if (input[1].drivenByComponent == -1)    // data B
		return clearOutputs();

	if (input[0].stringType != input[1].stringType)
		return clearOutputs();

	output[0].value.clear();
	output[0].stringValue.clear();

	bool found;
	int k;
	if (input[0].stringType) {
		for (int jb = 0; jb < input[1].stringValue.size(); jb++) {
			found = false;
			for (int ja = 0; ja < input[0].stringValue.size(); ja++) {
				if (input[1].stringValue[jb] == input[0].stringValue[ja]) {
					found = true;
					break;
				}
			}

			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].stringValue.size(); k++) {
					if (input[1].stringValue[jb] == output[0].stringValue[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].stringValue.push_back( input[1].stringValue[jb]);
			}
		}
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].stringValue.size();
	}
	else
	{
		for (int jb = 0; jb < input[1].value.size(); jb++) {
			found = false;
			for (int ja = 0; ja < input[0].value.size(); ja++) {
				if (input[1].value[jb] == input[0].value[ja]) {
					found = true;
					break;
				}
			}
			if (!found) {
				// see if its already in the output (keep the output from having duplicates)
				found = false;
				for (k = 0; k < output[0].value.size(); k++) {
					if (input[1].value[jb] == output[0].value[k]) {
						found = true;
						break;
					}
				}
				if (!found) // add it to the output
					output[0].value.push_back( input[1].value[jb]);
			}
		}
		output[0].dimensions.resize(1);
		output[0].dimensions[0] = output[0].value.size();
	}
	output[0].stringType = input[0].stringType;
	return true;

}
OnlyB_Component::OnlyB_Component()
	: Component(L"OnlyB", L"Only in B", 2, 1)
{
	setLeftLabels(3, "A", "B");
}


// ******************************** US Cities ***************************************

UsCity::UsCity(int _latdeg, int _latmin, char _lathem, int _londeg, int _lonmin, char _lonhem, string _city, string _state, int _timeZone) {
	
	city = _city;
	state = _state;
	timeZone = -_timeZone; // hours to UTC

	latitude = _latdeg + _latmin / 60.0;
	if (_lathem == 'S')
		latitude = -latitude;

	longitude = _londeg + _lonmin / 60.0;
	if (_lonhem == 'W')
		longitude = -longitude;
}
std::vector<UsCity*>usCityList{
	new UsCity(71, 18, 'N', 156, 46, 'W', "Barrow", "Alaska", 9),
	new UsCity(70, 12, 'N', 148, 31, 'W', "Deadhorse", "Alaska", 9),
	new UsCity(64, 51, 'N', 147, 43, 'W', "Fairbanks", "Alaska", 9),
	new UsCity(61, 13, 'N', 149, 54, 'W', "Anchorage", "Alaska", 9),
	new UsCity(58, 18, 'N', 134, 25, 'W', "Juneau", "Alaska", 9),
	new UsCity(57,  3, 'N', 135, 20, 'W', "Sitka", "Alaska", 9),
	new UsCity(51, 53, 'N', 176, 39, 'W', "Adak", "Alaska", 9),
	new UsCity(47, 37, 'N', 122, 20, 'W', "Seattle", "Washington", 8),
	new UsCity(46, 49, 'N', 100, 47, 'W', "Bismarck", "North Dakota", 6),
	//   setCity(46, 44, 'N', 117, 0, 'W', "Moscow", "Idaho", tz ),
	new UsCity(46, 36, 'N', 112,  2, 'W', "Helena", "Montana", 7),
	new UsCity(45, 31, 'N', 122, 41, 'W', "Portland", "Oregon", 8),
	new UsCity(44, 59, 'N', 93, 16, 'W', "Minneapolis", "Minnesota", 6),
	new UsCity(44, 22, 'N', 100, 20, 'W', "Pierre", "South Dakota", 6),
	new UsCity(44, 19, 'N', 69, 47, 'W', "Augusta", "Maine", 5),
	new UsCity(44, 15, 'N', 72, 34, 'W', "Montpelier", "Vermont", 5),
	new UsCity(43, 37, 'N', 116, 12, 'W', "Boise", "Idaho", 7),
	new UsCity(43, 12, 'N', 71, 32, 'W', "Concord", "New Hampshire", 5),
	new UsCity(43, 3, 'N', 87, 57, 'W', "Milwaukee", "Wisconsin", 6),
	new UsCity(42, 54, 'N', 78, 51, 'W', "Buffalo", "New York", 5),
	new UsCity(42, 21, 'N', 71, 4, 'W', "Boston", "Massachusetts", 5),
	new UsCity(42, 20, 'N', 83, 3, 'W', "Detroit", "Michigan", 5),
	new UsCity(41, 99, 'N', 89, 58, 'W', "Polo", "Illinois", 6),
	new UsCity(41, 53, 'N', 87, 38, 'W', "Chicago", "Illinois", 6),
	new UsCity(41, 49, 'N', 71, 25, 'W', "Providence", "Rhode Island", 5),
	new UsCity(41, 46, 'N', 72, 40, 'W', "Hartford", "Connecticut", 5),
	new UsCity(41, 35, 'N', 93, 37, 'W', "Des Moines", "Iowa", 6),
	new UsCity(41, 29, 'N', 81, 40, 'W', "Cleveland", "Ohio", 5),
	new UsCity(41, 9, 'N', 104, 48, 'W', "Cheyenne", "Wyoming", 7),
	// setCity(41, 5, 'N', 85, 8, 'W', "Fort Wayne", "Indiana", tz ),
	new UsCity(40, 49, 'N', 96, 41, 'W', "Lincoln", "Nebraska", 6),
	new UsCity(40, 46, 'N', 105, 1, 'W', "Fort Collins", "Colorado", 7),
	new UsCity(40, 45, 'N', 111, 53, 'W', "Salt Lake City", "Utah", 7),
	new UsCity(40, 43, 'N', 74, 4, 'W', "Jersey City", "New Jersey", 5),
	new UsCity(40, 40, 'N', 73, 56, 'W', "New York City", "New York", 5),
	new UsCity(40, 27, 'N', 80, 0, 'W', "Pittsburgh", "Pennsylvania", 5),
	new UsCity(40, 13, 'N', 74, 46, 'W', "Trenton", "New Jersey", 5),
	new UsCity(40, 1, 'N', 105, 17, 'W', "Boulder", "Colorado", 7),
	new UsCity(39, 59, 'N', 82, 59, 'W', "Columbus", "Ohio", 5),
	new UsCity(39, 57, 'N', 75, 10, 'W', "Philadelphia", "Pennsylvania", 5),
	new UsCity(39, 46, 'N', 86, 9, 'W', "Indianapolis", "Indiana", 5),
	new UsCity(39, 44, 'N', 104, 59, 'W', "Denver", "Colorado", 7),
	new UsCity(39, 32, 'N', 119, 49, 'W', "Reno", "Nevada", 8),
	new UsCity(39, 17, 'N', 76, 37, 'W', "Baltimore", "Maryland", 5),
	new UsCity(39, 10, 'N', 75, 32, 'W', "Dover", "Delaware", 5),
	new UsCity(39, 6, 'N', 84, 31, 'W', "Cincinnati", "Ohio", 5),
	new UsCity(39, 6, 'N', 94, 35, 'W', "Kansas City", "Missouri", 6),
	new UsCity(38, 54, 'N', 77, 2, 'W', "Washington", "District of Columbia", 5),
	new UsCity(38, 38, 'N', 90, 12, 'W', "St. Louis", "Missouri", 6),
	new UsCity(38, 33, 'N', 121, 28, 'W', "Sacramento", "California", 8),
	new UsCity(38, 21, 'N', 81, 38, 'W', "Charleston", "West Virginia", 5),
	new UsCity(38, 15, 'N', 85, 46, 'W', "Louisville", "Kentucky", 5),
	new UsCity(37, 47, 'N', 122, 25, 'W', "San Francisco", "California", 8),
	new UsCity(37, 41, 'N', 97, 20, 'W', "Wichita", "Kansas", 6),
	new UsCity(37, 12, 'N', 93, 17, 'W', "Springfield", "Missouri", 6),
	new UsCity(36, 51, 'N', 75, 59, 'W', "Virginia Beach", "Virginia", 5),
	new UsCity(36, 11, 'N', 115, 8, 'W', "Las Vegas", "Nevada", 8),
	new UsCity(36, 10, 'N', 86, 47, 'W', "Nashville", "Tennessee", 6),
	new UsCity(36, 8, 'N', 95, 56, 'W', "Tulsa", "Oklahoma", 6),
	new UsCity(35, 58, 'N', 83, 57, 'W', "Knoxville", "Tennessee", 5),
	new UsCity(35, 49, 'N', 78, 39, 'W', "Raleigh", "North Carolina", 5),
	new UsCity(35, 40, 'N', 105, 58, 'W', "Santa Fe", "New Mexico", 7),
	new UsCity(35, 29, 'N', 97, 32, 'W', "Oklahoma City", "Oklahoma", 6),
	new UsCity(35, 14, 'N', 80, 51, 'W', "Charlotte", "North Carolina", 5),
	new UsCity(35, 7, 'N', 89, 58, 'W', "Memphis", "Tennessee", 6),
	new UsCity(35, 7, 'N', 106, 37, 'W', "Albuquerque", "New Mexico", 7),
	new UsCity(34, 44, 'N', 92, 20, 'W', "Little Rock", "Arkansas", 6),
	new UsCity(34, 26, 'N', 119, 43, 'W', "Santa Barbara", "California", 8),
	new UsCity(34, 13, 'N', 77, 55, 'W', "Wilmington", "North Carolina", 5),
	new UsCity(34, 3, 'N', 118, 15, 'W', "Los Angeles", "California", 8),
	new UsCity(34, 0, 'N', 81, 3, 'W', "Columbia", "South Carolina", 5),
	new UsCity(33, 57, 'N', 117, 24, 'W', "Riverside", "California", 8),
	new UsCity(33, 45, 'N', 84, 23, 'W', "Atlanta", "Georgia", 5),
	new UsCity(33, 39, 'N', 86, 49, 'W', "Birmingham", "Alabama", 6),
	new UsCity(33, 27, 'N', 112, 4, 'W', "Phoenix", "Arizona", 7),
	new UsCity(32, 47, 'N', 79, 56, 'W', "Charleston", "South Carolina", 5),
	new UsCity(32, 47, 'N', 96, 48, 'W', "Dallas", "Texas", 6),
	new UsCity(32, 43, 'N', 117, 10, 'W', "San Diego", "California", 8),
	new UsCity(32, 38, 'N', 115, 33, 'W', "El Centro", "California", 8),
	new UsCity(32, 28, 'N', 93, 42, 'W', "Shreveport", "Louisiana", 6),
	new UsCity(32, 18, 'N', 90, 11, 'W', "Jackson", "Mississippi", 6),
	new UsCity(32, 13, 'N', 110, 56, 'W', "Tucson", "Arizona", 7),
	new UsCity(31, 47, 'N', 106, 25, 'W', "El Paso", "Texas", 7),
	new UsCity(30, 42, 'N', 88, 3, 'W', "Mobile", "Alabama", 6),
	new UsCity(30, 20, 'N', 81, 40, 'W', "Jacksonville", "Florida", 5),
	new UsCity(30, 15, 'N', 97, 45, 'W', "Austin", "Texas", 6),
	new UsCity(29, 58, 'N', 90, 3, 'W', "New Orleans", "Louisiana", 6),
	new UsCity(29, 46, 'N', 95, 23, 'W', "Houston", "Texas", 6),
	new UsCity(29, 25, 'N', 98, 30, 'W', "San Antonio", "Texas", 6),
	new UsCity(28, 25, 'N', 81, 18, 'W', "Orlando", "Florida", 5),
	new UsCity(27, 57, 'N', 82, 28, 'W', "Tampa", "Florida", 5),
	new UsCity(25, 47, 'N', 80, 13, 'W', "Miami", "Florida", 5),
	new UsCity(24, 34, 'N', 81, 47, 'W', "Key West", "Florida", 5),
	new UsCity(21, 19, 'N', 157, 50, 'W', "Honolulu", "Hawaii", 11),
	new UsCity(19, 42, 'N', 155, 5, 'W', "Hilo", "Hawaii", 11)
	//  setCity(18, 27, 'N', 66, 4, 'W', "San Juan", "Puerto Rico", tz ),
	//  setCity(18, 21, 'N', 64, 57, 'W', "Charlotte Amalie", "United States Virgin Islands", tz ),
	//  setCity(15, 11, 'N', 145, 45, 'E', "Saipan", "Northern Mariana Islands", tz ),
	//  setCity(13, 31, 'N', 144, 50, 'E', "Dededo", "Guam", tz ),
	//  setCity(13, 29, 'N', 144, 45, 'E', "Hagåtña", "Guam", tz ),
	//  setCity(14, 17, 'S', 170, 42, 'W', "Pago Pago", "American Samoa", tz ),
	//  setCity(77, 51, 'S', 166, 40, 'W', "McMurdo", "N/A", tz )
};
int UsCity_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	int len = usCityList.size();
	int cities = input[0].value.size();
	int j, k;


	for (k = 0; k < 3; k++) {
		output[k].value.resize(cities);
		output[k].dimensions[0] = cities;
	}
	for (k = 3; k < 5; k++) {
		output[k].stringType = true;
		output[k].stringValue.resize(cities);
		output[k].dimensions[0] = cities;
	}

	for (j = 0; j < cities; j++) {
		int cityID = floor(input[0].value[j]);

		if ((cityID < 0) || (cityID >= len)) {
			output[0].value[j] = 0;
			output[1].value[j] = 0;
			output[2].value[j] = 0;
			output[3].stringValue[j] = "";
			output[4].stringValue[j] = "";
		}
		else {
			output[0].value[j] = usCityList[cityID]->latitude;
			output[1].value[j] =usCityList[cityID]->longitude;
			output[2].value[j] =usCityList[cityID]->timeZone;
			output[3].stringValue[j] = usCityList[cityID]->city;
			output[4].stringValue[j] = usCityList[cityID]->state;
		}
	}
	return true;
}


UsCity_Component::UsCity_Component()
	: Component(L"UsCity", L"US Cities", squareShape, 1, 1, 5, 1)
{
	setRightLabels(3, "Latitude (deg)", "Longitude (deg)", "Time Zone", "City", "State");
	width = 45;
}


// ******************************** World Cities ***************************************


WorldCity::WorldCity(string _city, string _country, int _latdeg, int _latmin, char _lathem, int _londeg, int _lonmin, char _lonhem, int _timeZone) {

	city = _city;
	country = _country;
	timeZone = -_timeZone; // hours to UTC

	latitude = _latdeg + _latmin / 60.0;
	if (_lathem == 'S')
		latitude = -latitude;

	longitude = _londeg + _lonmin / 60.0;
	if (_lonhem == 'W')
		longitude = -longitude;
}
std::vector<WorldCity*>worldCityList{
	new WorldCity("Aberdeen", "Scotland", 57, 9, 'N', 2, 9, 'W', 0),
	new WorldCity("Adelaide", "Australia", 34, 55, 'S', 138, 36, 'E', -9.5),  // next day -9.5 or 15.5
	new WorldCity("Algiers", "Algeria", 36, 50, 'N', 3, 0, 'E', -1),
	new WorldCity("Amsterdam", "Netherlands", 52, 22, 'N', 4, 53, 'E', -1),
	new WorldCity("Ankara", "Turkey", 9, 55, 'N', 32, 55, 'E', -2),
	new WorldCity("Asunción", "Paraguay", 25, 15, 'S', 57, 40, 'W', 4),
	new WorldCity("Athens", "Greece", 37, 58, 'N', 23, 43, 'E', -2),
	new WorldCity("Auckland", "New Zealand", 36, 52, 'S', 174, 45, 'E', -12),  // next day -12 or 12
	new WorldCity("Bangkok", "Thailand", 13, 45, 'N', 100, 30, 'E', -7),
	new WorldCity("Barcelona", "Spain", 41, 23, 'N', 2, 9, 'E', -1),
	new WorldCity("Beijing", "China", 39, 55, 'N', 116, 25, 'E', -8),  // next day -8 or 16
	new WorldCity("Belém", "Brazil", 1, 28, 'S', 48, 29, 'W', 3),
	new WorldCity("Belfast", "Northern Ireland", 54, 37, 'N', 5, 56, 'W', 0),
	new WorldCity("Belgrade", "Serbia", 44, 52, 'N', 20, 32, 'E', -1),
	new WorldCity("Berlin", "Germany", 52, 30, 'N', 13, 25, 'E', -1),
	new WorldCity("Birmingham", "England", 2, 25, 'N', 1, 55, 'W', 0),
	new WorldCity("Bogotá", "Colombia", 4, 32, 'N', 74, 15, 'W', 5),
	new WorldCity("Bombay", "India", 19, 0, 'N', 72, 48, 'E', -5.5),
	new WorldCity("Bordeaux", "France", 44, 50, 'N', 0, 31, 'W', -1),
	new WorldCity("Bremen", "Germany", 53, 5, 'N', 8, 49, 'E', -1),
	new WorldCity("Brisbane", "Australia", 27, 29, 'S', 153, 8, 'E', -10),  // next day -10 or 14
	new WorldCity("Bristol", "England", 51, 28, 'N', 2, 35, 'W', 0),
	new WorldCity("Brussels", "Belgium", 50, 52, 'N', 4, 22, 'E', -1),
	new WorldCity("Bucharest", "Romania", 44, 25, 'N', 26, 7, 'E', -2),
	new WorldCity("Budapest", "Hungary", 47, 30, 'N', 19, 5, 'E', -1),
	new WorldCity("Buenos Aires", "Argentina", 34, 35, 'S', 58, 22, 'W', 3),
	new WorldCity("Cairo", "Egypt", 30, 2, 'N', 31, 21, 'E', -2),
	new WorldCity("Calcutta", "India", 22, 34, 'N', 88, 24, 'E', -5.5),
	new WorldCity("Canton", "China", 23, 7, 'N', 113, 15, 'E', -8),  // next day -8 or 16
	new WorldCity("Cape Town", "South Africa", 33, 55, 'S', 18, 22, 'E', -2),
	new WorldCity("Caracas", "Venezuela", 10, 28, 'N', 67, 2, 'W', 4),
	new WorldCity("Cayenne", "French Guiana", 4, 49, 'N', 52, 18, 'W', 3),
	new WorldCity("Chihuahua", "Mexico", 28, 37, 'N', 106, 5, 'W', 7),
	new WorldCity("Chongqing", "China", 29, 46, 'N', 106, 34, 'E', -8),  // next day -8 or 16
	new WorldCity("Copenhagen", "Denmark", 55, 40, 'N', 12, 34, 'E', -1),
	new WorldCity("Córdoba", "Argentina", 31, 28, 'S', 64, 10, 'W', 3),
	new WorldCity("Dakar", "Senegal", 14, 40, 'N', 17, 28, 'W', 0),
	new WorldCity("Darwin", "Australia", 12, 28, 'S', 130, 51, 'E', -9.5),  // next day -9.5 or 15.5
	new WorldCity("Djibouti", "Djibouti", 11, 30, 'N', 43, 3, 'E', -3),
	new WorldCity("Dublin", "Ireland", 53, 20, 'N', 6, 15, 'W', 0),
	new WorldCity("Durban", "South Africa", 29, 53, 'S', 30, 53, 'E', -2),
	new WorldCity("Edinburgh", "Scotland", 55, 55, 'N', 3, 10, 'W', 0),
	new WorldCity("Frankfurt", "Germany", 50, 7, 'N', 8, 41, 'E', -1),
	new WorldCity("Georgetown", "Guyana", 6, 45, 'N', 58, 15, 'W', 4),
	new WorldCity("Glasgow", "Scotland", 55, 50, 'N', 4, 15, 'W', 0),
	new WorldCity("Guatemala City", "Guatemala", 14, 37, 'N', 90, 31, 'W', 6),
	new WorldCity("Guayaquil", "Ecuador", 2, 10, 'S', 79, 56, 'W', 5),
	new WorldCity("Hamburg", "Germany", 53, 33, 'N', 10, 2, 'E', -1),
	new WorldCity("Hammerfest", "Norway", 70, 38, 'N', 23, 38, 'E', -1),
	new WorldCity("Havana", "Cuba", 23, 8, 'N', 82, 23, 'W', 5),
	new WorldCity("Helsinki", "Finland", 60, 10, 'N', 25, 0, 'E', -2),
	new WorldCity("Hobart", "Tasmania", 42, 52, 'S', 147, 19, 'E', -10),  // next day -10 or 14
	new WorldCity("Hong Kong", "China", 22, 20, 'N', 114, 11, 'E', -8),  // next day -8 or 16
	new WorldCity("Iquique", "Chile", 20, 10, 'S', 70, 7, 'W', 4),
	new WorldCity("Irkutsk", "Russia", 52, 30, 'N', 104, 20, 'E', 16),
	new WorldCity("Jakarta", "Indonesia", 6, 16, 'S', 106, 48, 'E', -7),
	new WorldCity("Johannesburg", "South Africa", 26, 12, 'S', 28, 4, 'E', -2),
	new WorldCity("Kingston", "Jamaica", 17, 59, 'N', 76, 49, 'W', 5),
	new WorldCity("Kinshasa", "Congo", 4, 18, 'S', 15, 17, 'E', -1),
	new WorldCity("Kuala Lumpur", "Malaysia", 3, 8, 'N', 101, 42, 'E', -8),  // next day -8 or 16
	new WorldCity("La Paz", "Bolivia", 16, 27, 'S', 68, 22, 'W', 4),
	new WorldCity("Leeds", "England", 53, 45, 'N', 1, 30, 'W', 0),
	new WorldCity("Lima", "Peru", 12, 0, 'S', 77, 2, 'W', 5),
	new WorldCity("Lisbon", "Portugal", 38, 44, 'N', 9, 9, 'W', 0),
	new WorldCity("Liverpool", "England", 53, 25, 'N', 3, 0, 'W', 0),
	new WorldCity("London", "England", 51, 32, 'N', 0, 5, 'W', 0),
	new WorldCity("Lyons", "France", 45, 45, 'N', 4, 50, 'E', -1),
	new WorldCity("Madrid", "Spain", 40, 26, 'N', 3, 42, 'W', -1),
	new WorldCity("Manchester", "England", 53, 30, 'N', 2, 15, 'W', 0),
	new WorldCity("Manila", "Philippines", 14, 35, 'N', 120, 57, 'E', -8),  // next day -8 or 16
	new WorldCity("Marseilles", "France", 43, 20, 'N', 5, 20, 'E', -1),
	new WorldCity("Mazatlán", "Mexico", 23, 12, 'N', 106, 25, 'W', 7),
	new WorldCity("Mecca", "Saudi Arabia", 21, 29, 'N', 39, 45, 'E', -3),
	new WorldCity("Melbourne", "Australia", 37, 47, 'S', 144, 58, 'E', -10),  // next day -10 or 14
	new WorldCity("Mexico City", "Mexico", 19, 26, 'N', 99, 7, 'W', 6),
	new WorldCity("Milan", "Italy", 45, 27, 'N', 9, 10, 'E', -1),
	new WorldCity("Montevideo", "Uruguay", 34, 53, 'S', 56, 10, 'W', 3),
	new WorldCity("Moscow", "Russia", 55, 45, 'N', 37, 36, 'E', -3),
	new WorldCity("Munich", "Germany", 48, 8, 'N', 11, 35, 'E', -1),
	new WorldCity("Nagasaki", "Japan", 32, 48, 'N', 129, 57, 'E', -9),  // next day -9 or 15
	new WorldCity("Nagoya", "Japan", 35, 7, 'N', 136, 56, 'E', -9),  // next day -9 or 15
	new WorldCity("Nairobi", "Kenya", 1, 25, 'S', 36, 55, 'E', -3),
	new WorldCity("Nanjing (Nanking)", "China", 32, 3, 'N', 118, 53, 'E', -8),  // next day -8 or 16
	new WorldCity("Naples", "Italy", 40, 50, 'N', 14, 15, 'E', -1),
	new WorldCity("New Delhi", "India", 28, 35, 'N', 77, 12, 'E', -5.5),
	new WorldCity("Newcastle-on-Tyne", "England", 54, 58, 'N', 1, 37, 'W', 0),
	new WorldCity("Odessa", "Ukraine", 46, 27, 'N', 30, 48, 'E', -2),
	new WorldCity("Osaka", "Japan", 34, 32, 'N', 135, 30, 'E', -9),  // next day -9 or 15
	new WorldCity("Oslo", "Norway", 59, 57, 'N', 10, 42, 'E', -1),
	new WorldCity("Panama City", "Panama", 8, 58, 'N', 79, 32, 'W', 5),
	new WorldCity("Paramaribo", "Suriname", 5, 45, 'N', 55, 15, 'W', 3),
	new WorldCity("Paris", "France", 48, 48, 'N', 2, 20, 'E', -1),
	new WorldCity("Perth", "Australia", 31, 57, 'S', 115, 52, 'E', -8),  // next day -8 or 16
	new WorldCity("Plymouth", "England", 50, 25, 'N', 4, 5, 'W', 0),
	new WorldCity("Port Moresby", "Papua New Guinea", 9, 25, 'S', 147, 8, 'E', -10),  // next day -10 or 14
	new WorldCity("Prague", "Czech Republic", 50, 5, 'N', 14, 26, 'E', -1),
	new WorldCity("Rangoon", "Myanmar", 16, 50, 'N', 96, 0, 'E', -6.5),
	new WorldCity("Reykjavík", "Iceland", 64, 4, 'N', 21, 58, 'W', 0),
	new WorldCity("Rio de Janeiro", "Brazil", 22, 57, 'S', 43, 12, 'W', 3),
	new WorldCity("Rome", "Italy", 41, 54, 'N', 12, 27, 'E', -1),
	new WorldCity("Salvador", "Brazil", 12, 56, 'S', 38, 27, 'W', 3),
	new WorldCity("Santiago", "Chile", 33, 28, 'S', 70, 45, 'W', 4),
	new WorldCity("St. Petersburg", "Russia", 59, 56, 'N', 30, 18, 'E', -3),
	new WorldCity("São Paulo", "Brazil", 23, 31, 'S', 46, 31, 'W', 3),
	new WorldCity("Shanghai", "China", 31, 10, 'N', 121, 28, 'E', -8),  // next day -8 or 16
	new WorldCity("Singapore", "Singapore", 1, 14, 'N', 103, 55, 'E', -8),  // next day -8 or 16
	new WorldCity("Sofia", "Bulgaria", 42, 40, 'N', 23, 20, 'E', -2),
	new WorldCity("Stockholm", "Sweden", 59, 17, 'N', 18, 3, 'E', -1),
	new WorldCity("Sydney", "Australia", 34, 0, 'S', 151, 0, 'E', -10),  // next day -10 or 14
	new WorldCity("Tananarive", "Madagascar", 18, 50, 'S', 47, 33, 'E', -3),
	new WorldCity("Teheran", "Iran", 35, 45, 'N', 51, 45, 'E', -3.5),
	new WorldCity("Tokyo", "Japan", 35, 40, 'N', 139, 45, 'E', -9),  // next day -9 or 15
	new WorldCity("Tripoli", "Libya", 32, 57, 'N', 13, 12, 'E', -2),
	new WorldCity("Venice", "Italy", 45, 26, 'N', 12, 20, 'E', -1),
	new WorldCity("Veracruz", "Mexico", 19, 10, 'N', 96, 10, 'W', 6),
	new WorldCity("Vienna", "Austria", 48, 14, 'N', 16, 20, 'E', -1),
	new WorldCity("Vladivostok", "Russia", 43, 10, 'N', 132, 0, 'E', -10),  // next day -10 or 14
	new WorldCity("Warsaw", "Poland", 52, 14, 'N', 21, 0, 'E', -1),
	new WorldCity("Wellington", "New Zealand", 41, 17, 'S', 174, 47, 'E', -12),  // next day -12 or 12
	new WorldCity("Zürich", "Switzerland", 47, 21, 'N', 8, 31, 'E', -1)
};

int WorldCity_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1)
		return clearOutputs();

	int len = worldCityList.size();
	int cities = input[0].value.size();
	int j, k;

	
	for (k = 0; k < 3; k++) {
		output[k].value.resize(cities);
		output[k].dimensions[0] = cities;
	}
	for (k = 3; k < 5; k++) {
		output[k].stringType = true;
		output[k].stringValue.resize(cities);
		output[k].dimensions[0] = cities;
	}

	for (j = 0; j < cities; j++) {
		int cityID = floor(input[0].value[j]);

		if ((cityID < 0) || (cityID >= len)) {
			for (k = 0; k < 3; k++) 
				output[k].value[j] = 0;
			for (k = 3; k < 5; k++) 
				output[k].stringValue[j] = "";
		}
		else {
			output[0].value[j] = worldCityList[cityID]->latitude;
			output[1].value[j] = worldCityList[cityID]->longitude;
			output[2].value[j] = worldCityList[cityID]->timeZone;
			output[3].stringValue[j] = worldCityList[cityID]->city;
			output[4].stringValue[j] = worldCityList[cityID]->country;
		}
	}
	return true;
}

WorldCity_Component::WorldCity_Component()
	: Component(L"WorldCity", L"World Cities", squareShape, 1, 1, 5, 1)
{
	setRightLabels(3, "Latitude (deg)", "Longitude (deg)", "Time Zone", "City", "Country");
	width = 45;
}

// ************************** day of week *****************************
int DayOfWeek_Component::method2(Sheet *pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	
	time_t tim = time(0);   // get time now
	struct tm buf;
	localtime_s(&buf, &tim);

	int defaultYear = buf.tm_year + 1900;
	int defaultMonth = buf.tm_mon + 1;
	int defaultDay = buf.tm_mday;
	
	int y, m, d;
	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	output[0].value.resize(loops);

	for (int j = 0; j < loops; j++) {
		y = getLoopingValueD( 0, j, defaultYear);
		m = getLoopingValueD(1, j, defaultMonth);
		d = getLoopingValueD(2, j, defaultDay);

		y -= m < 3;
		if (m < 0)
			output[0].value[j] = -1;
		else if( m > 11 )
			output[0].value[j] = -1;
		else
			output[0].value[j] = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
	}
	
	return 1;
}


DayOfWeek_Component::DayOfWeek_Component()
	: Component(L"DayOfWeek", L"Day Of Week", 3, 1)
{
	setLeftLabels(3, "Year", "Month", "Day");
}



int DaysInMonth_Component::method2(Sheet* pg) {

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();
	//setOutputMatrixSizes_(pg, i, result.dims);

	time_t t = time(0);   // get time now
	struct tm buf;
	localtime_s( &buf, &t);

	int defaultYear = buf.tm_year + 1900;
	int defaultMonth = buf.tm_mon + 1;
	
	int year, month;
	output[0].value.resize(loops);

	for (int j = 0; j < loops; j++) {
		year = getLoopingValueD(0, j, defaultYear);
		month = getLoopingValueD(1, j, defaultMonth);

		switch (month-1) {
			// 30 days has...
		case 9:// sep
		case 4:// apr
		case 6:// june
		case 11:// nov
			output[0].value[j] = 30;
			break;
			// all the rest have 31
		case 1:// january
		case 3:// mar
		case 5:// may
		case 7:// july
		case 8:// aug
		case 10:// oct
		case 12:// dec
			output[0].value[j] = 31;
			break;
			// except the second month
		case 2:// february
			if (year % 100 == 0) {
				if (year % 400 == 0)
					output[0].value[j] = 29; // leap century
				else
					output[0].value[j] = 28; // regular century
			}
			else if (year % 4 == 0)
				output[0].value[j] = 29; // leap year
			else
				output[0].value[j] = 28; // regular year
			break;
		default:
			output[0].value[j] = 0;
			break;
		}
	}
	return 1;
}

DaysInMonth_Component::DaysInMonth_Component()
	: Component(L"DaysInMon", L"Days In Month", 2, 1)
{
	setLeftLabels(3, "Year", "Month");
}

// ************** Real Time clock *********************

int RealTimeClock_Component::method2(Sheet* pg) {

	output[0].value[0] = time(0);   // get time now
	return 1;
}
RealTimeClock_Component::RealTimeClock_Component()
	: Component(L"Clock", L"RT Clock", triangleShape, 0, 0, 1, 1)
{
	timeBased = true;
	//output[0].dimensions[0] = 1;
}

// ************** Concatenate *********************

int Concatenate_Component::method2(Sheet* pg) {

	output[0].stringValue.resize(1);
	output[0].stringValue[0] = "";
	output[0].stringType = true;

	// combine elements on the left input into a single right outputs
	if (input[0].drivenByComponent == -1)
		return true;

	string stringSeg; // optional input

	if (input[1].drivenByComponent == -1)
		stringSeg = "";
	else if (input[1].stringType)
		stringSeg = input[1].stringValue[0];
	else {
		for( int k = 0; k < input[1].value.size(); k++)
			stringSeg += input[1].value[k];
	}

	int n = input[0].stringValue.size();

	string outputString = "";

	for (int j = 0; j < n; j++) {
		if (j > 0)
			outputString += stringSeg;
		if (input[0].stringValue[j].size())
			outputString += input[0].stringValue[j];
	}
	output[0].stringValue[0] = outputString;

	return 1;
}
Concatenate_Component::Concatenate_Component()
	: Component(L"Concat", L"Concatenate", 2, 1)
{
	setLeftLabels(3, "Input", "Separator");
}


// ************** Morse Code *********************

vector<vector<char>> theMorseLetters = {
{1,3},			// A
{3, 1, 1, 1},	// B
{3, 1, 3, 1},	// C
{3, 1, 1},		// D
{1},			// E
{1, 1, 3, 1},	// F
{3, 3, 1},		// G
{1, 1, 1, 1},	// H
{1, 1},			// I
{1, 3, 3, 3},	// J
{3, 1, 3},		// K
{1, 3, 1, 1},	// L
{3, 3},			// M
{3, 1},			// N
{3, 3, 3},		// O
{1, 3, 3, 1},	// P
{3, 3, 1, 3},	// Q
{1, 3, 1},		// R
{1, 1, 1},		// S
{3},			// T
{1, 1, 3},		// U
{1, 1, 1, 3},	// V
{1, 3, 3},		// W
{3, 1, 1, 3},	// X
{3, 3, 1, 1}	// Z
};
vector<vector<char>> theMorseNumbers = {
{3, 3, 3, 3, 3},// 0
{1, 3, 3, 3, 3},// 1
{1, 3, 3, 3, 3},// 2
{1, 1, 3, 3, 3},// 3
{1, 1, 1, 3, 3},// 4
{1, 1, 1, 1, 3},// 5
{3, 1, 1, 1, 1},// 6
{3, 3, 1, 1, 1},// 7
{3, 3, 3, 1, 1},// 8
{3, 3, 3, 3, 1} // 9
};

vector<char> getTheCode(char theChar) {
	if ((theChar >= 'A') && (theChar <= 'Z'))
		return theMorseLetters[theChar - 'A'];
	if ((theChar >= 'a') && (theChar <= 'z'))
		return theMorseLetters[theChar - 'a'];
	if ((theChar >= '0') && (theChar <= '9'))
		return theMorseNumbers[theChar - '0'];
	return {};
}
int MorseCode_Component::method2(Sheet* pg) {

	if (input[0].drivenByComponent == -1) {
		memory[3] = 0;
		return clearOutputs();
	}
	if (! input[0].stringType) {
		memory[3] = 0;
		return clearOutputs();
	}
	if (input[0].stringValue.size() == 0) {
		memory[3] = 0;
		return clearOutputs();
	}
	if (pg->timerID == NULL) {
		memory[3] = 0;
		return clearOutputs();
	}
		
	if (!memory[3]) {
		if (input[3].value[0]) {
			if (memory[4] == 0) {
				memory[0] = 0;	// reset string
				memory[1] = 0;	// reset morse code
				memory[2] = 0;  // time for next event
				memory[3] = 1;
				output[0].value[0] = 0;
				output[1].value[0] = 0;
			}
			memory[4] = 1;
		}
		else 
			memory[4] = 0;
	}
	if (!memory[3])
		return 0;

	// ok we are running
	double rate = getLoopingValueD(1, 0, 10); // 10 wpm default
	if (rate < 1)
		rate = 1;
	double ditTime = 60.0 / (50.0 * rate);
	double delay = getLoopingValueD(2, 0, 0); 
	if (delay < 0)
		delay = 0;

	if (pg->systemTime < memory[2]) // time for a change?
		return 0; // not yet

	char theChar = input[0].stringValue[0][memory[0]];
	vector<char> theCode = getTheCode(theChar);
	int theCodeSize = theCode.size();

	if (memory[1] >= theCodeSize) { // this char is done
		memory[1] = 0;
		memory[0]++; 
		int strlen = input[0].stringValue[0].size();
		if (memory[0] >= strlen) { // end - shut down
			output[0].value[0] = 0;
			memory[3] = 0;
			output[1].value[0] = 1;
			return 1;
		}

		// get the next character
		theChar = input[0].stringValue[0][memory[0]];
		theCode = getTheCode(theChar);
		memory[1] = 0;
		output[0].value[0] = 0; // keep it low
		if( theChar == ' ')
			memory[2] = pg->systemTime + (4+delay) * ditTime - pg->timeStep; // plus time for a space char
		else
			memory[2] = pg->systemTime + 2 * ditTime - pg->timeStep; // plus time for before next char
		return 1;
	}

	//if (theCodeSize > 0) {
		if (output[0].value[0]) {
			// output is high
			output[0].value[0] = 0; // lower it
			memory[2] = pg->systemTime + 1 * ditTime - pg->timeStep/3; // time between dots and dashes
			memory[1]++;
		}
		else {
			// output is low
			output[0].value[0] = 1; // raise it
			memory[2] = pg->systemTime + theCode[memory[1]] * ditTime - pg->timeStep / 3; // time for a dot or dash
		}
	//}
	//else {
	//	// its a space
	//	output[0].value[0] = 0; // keep it low
	//	memory[2] = pg->systemTime + (delay-1) * ditTime - pg->timeStep; // time for a word space
	//	memory[1]++;
	//}
	return 1;
}
MorseCode_Component::MorseCode_Component()
	: Component(L"Morse", L"MorseCode", squareShape, 4, 1, 2, 1)
{
	setLeftLabels(3, "Input", "Rate", "Delay", "Trigger");
	setRightLabels(3, "Code", "Done");
	timeBased = true;
	setMemory(0, 0); // the index of the current input character
	setMemory(1, 0); // the index of the current morse dot or dash
	setMemory(2, 0); // delay for next event
	setMemory(3, 0); // true if we are running
	setMemory(4, 0); // trigger edge detection
	width = 40;
}


// ************** Graph Base Words *********************

string GraphBaseWordsListA = "whichtheretheiraboutwouldtheseotherwordscouldwritefirstwaterafterwhererightthinkthreeyears\
placesoundgreatagainstilleverysmallfoundthoseneverundermightwhilehouseworldbelowaskedgoinglargeuntilalongshallbeing\
oftenearthbegansincestudynightlightabovepaperpartsyoungstorypointtimesheardwholewhitegivenmeansmusicmilesthingtoday\
laterusingmoneylinesordergroupamonglearnknownspacetableearlytreesshorthandsstateblackshownstoodfrontvoicekindsmakes\
comesclosepowerlivedvoweltakenbuiltheartreadyquiteclassbringroundhorseshowspiecegreenstandbirdsstartrivertriedleast\
fieldwhosegirlsleaveaddedcolorthirdhoursmovedplantdoingnamesformsheavyideascriedcheckfloorbeginwomanaloneplanespell\
watchcarrywroteclearnamedbookschildglasshumantakespartybuildseemsbloodsidessevenmouthsolvenorthvaluedeathmaybehappy\
tellsgiveslooksshapelivesstepsareassensespeakforceoceanspeedwomenmetalsouthgrassscalecellslowersleepwrongpagesships\
needsrockseightmajorleveltotalaheadreachstarsstoresighttermscatchworksboardcoversongsequalstonewavesguessdancespoke\
breakcauseradioweekslandsbasiclikedtradefreshfinalfightmeantdrivespentlocalwaxesknowstrainbreadhomesteethcoastthick\
browncleanquietsugarfactssteelforthrulesnotesunitspeacemonthverbsseedshelpssharpvisitwoodschiefwallscrosswingsgrown\
casesfoodscropsfruitstickwantsstagesheepnounsplaindrinkbonesapartturnsmovestouchanglebasedrangemarkstiredolderfarms\
spendshoesgoodschairtwicecentsemptyalikestylebrokepairscountenjoyscoreshorerootspaintheadsshookserveangrycrowdwheel\
quickdresssharealivenoisesolidclothsignshillstypesdrawnworthtruckpianoupperlovedusualfacesdrovecabinboatstownsproud\
courtmodelprimefiftyplansyardsprovetoolspricesheetsmellboxesraisematchtruthroadsthrewenemylunchchartscenegraphdoubt\
guidewindsblockgrainsmokemixedgameswagonsweettopicextraplatetitleknifefencefallscloudwheatplaysenterbroadsteamatoms\
presslyingbasisclocktastegrowsthankstormagreebraintracksmilefunnybeachstockhurrysavedsorrygianttrailofferoughtrough\
dailyavoidkeepsthrowallowcreamlaughedgesteachframebellsdreammagicoccurendedchordfalseskillholesdozenbraveappleclimb\
outerpitchrulerholdsfixedcostscallsblankstafflaboreatenyouthtoneshonorglobegasesdoorspoleslooseapplytearsexactbrush\
chestlayerwhaleminorfaithtestsjudgeitemsworrywastehopedstripbegunasidelakesbounddepthcandyeventworseawareshellrooms\
ranchimagesnakealouddriedlikesmotorpoundkneesreferfullychainshirtflourdropsspiteorbitbanksshootcurvetribetightblind\
sleptshadeclaimfliesthemequeenfifthunionhencestrawentryissuebirthfeelsangerbriefrhymegloryguardflowsfleshownedtrick\
yourssizesnotedwidthburstroutelungsunclebearsroyalkingsfortytrialcardsbrassoperachoseownervaporbeatsmousetoughwires\
metertowerfindsinnerstuckarrowpoemslabelswingsolartrulytensebeanssplitrisesweighhotelstemsprideswunggradedigitbadly\
bootspilotsalessweptluckyprizestovetubesacreswoundsteepslidetrunkerrorporchslaveexistfacedminesmarryjuiceracedwaved\
goosetrustfewerfavormillsviewsjointeagerspotsblendringsadultindexnailshornsballsflameratesdrilltraceskinswaxedseats\
stuffratiomindsdirtysillycoinshellotripsleadsriflehopesbasesshinebenchmoralfiresmealsshakeshopscyclemovieslopecanoe\
teamsfolksfiredbandsthumbshoutcanalhabitreplyruledfevercrustshelfwalksmidstcrackprinttalescoachstifffloodverseawake\
rockymarchfaultswiftfaintcivilghostfeastbladelimitgermsreadsducksdairyworstgiftslistsstopsrapidbrickclawsbeadsbeast\
skirtcakeslionsfrogstriesnervegrandarmedtreathoneymoistlegalpennycrownshocktaxessixtyaltarpullssportdrumstalksdying\
datesdrankblowsleverwagesproofdrugstankssingstailspauseherdsarosehatedcluesnovelshameburntracesflashwearyheelstoken\
coatsspareshinyalarmdimessixthclerkmercysunnyguestfloatshonepipeswormsbillssweatsuitssmartupsetrainssandyrainyparks\
sadlyfancyriderunitybunchrollscrashcraftnewlygateshatchpathsfundswidergracegravetidesadmitshiftsailspupiltigerangel\
cruelagentdramaurgedpatchnestsvitalswordblameweedsscrewvocalbaconchalkcargocrazyactedgoatsarisewitchlovesqueerdwell\
backsropesshotsmerryphonecheekpeaksidealbeardeaglecreekcriesashesstallyieldmayoropensinputfleettoothcubicwivesburns\
poetsapronspearorgancliffstamppasteruralbakedchasesliceslantknocknoisysortsstayswipedblownpiledclubscheerwidowtwist\
tenthhidescommasweepspoonsterncreptmapledeedsridesmuddycrimejellyridgedriftdustydeviltempohumorsendsstealtentswaist\
rosesreignnoblecheapdenselinengeesewovenpostshiredwrathsaladbowedtiressharkbeltsgraspblastpolarfungitendspearlloads\
jokesveinsfrosthearsloseshostsdiverphasetoadsalerttasksseamscoralfocusnakedpuppyjumpsspoilquartmacrofearsflungspark\
vividbrooksteerspraydecayportssocksurbangoalsgrantminusfilmstunesshaftfirmsskiesbridewreckflockstarehobbybondsdared\
fadedthiefcrudepantsflutevotestonalradarwellsskullhairsarguewearsdollsvotedcavescaredbroomscentpanelfairyolivebends\
prismlampscablepeachruinsrallyschwalambssellscoolsdraftcharmlimbsbrakegazedcubesdelaybeamsfetchranksarrayharshcamel\
vinespicksnavalpurserigidcrawltoastsoilssaucebasinpondstwinswristfluidpoolsbrandstalkrobotreedshoofsbusessheergrief\
bloomdweltmeltsrisenflagskneltfiberroofsfreedarmorpilesaimedalgaetwigslemonditchdrunkrestschillslainpaniccordstuned\
crispledgedivedswampclungstolemoldsyarnslivergaugebreedstoolgullsawokegrossdiaryrailsbellytrendflaskstakefrieddraws\
actorhandybowlshastescopedealsknotsmoonsessaythumphangsblissdealtgainsbombsclownpalmsconesroasttidalboredchantacids\
doughcampssworeloverhooksmalescocoapunchawardreinsninthnoseslinksdrainfillsnylonlunarpulseflownelbowfatalsitesmoths\
meatsfoxesminedatticfierymountusageswearsnowyrustyscaretrapsrelaxreactvalidrobinceasegillspriorsaferpolioloyalswell\
saltymarshvagueweavemoundsealsmulesvirusscoutacutewindystoutfoldsseizehillyjoinspluckstacklordsdunesburrohawkstrout\
feedsscarfhallscoalstowelsoulselectbuggypumpsloansspinsfilesoxidepainsphotorivalflatssyruprodeosandsmoosepintscurly\
comiccloakonionclamsscrapdidstcouchcodesfailsouncelodgegreetgypsyutterpavedzonesfoursalleytilesblesscrestelderkills\
yeasterectbuglemedalroleshoundsnailalteranklerelayloopszerosbitesmodesdebtsrealmgloverayonswimspokedstrayliftsmaker\
lumpsgrazedreadbarnsdocksmastspourswharfcurseplumprobesseekscedarcurlsjollymythscagesgloomlockspedalbeetscrowsanode\
slashcreeprowedchipsfistswinescaresvalvenewermotelivorynecksclampbargebluesalienfrownstrapcrewsshackgonnasavesstump\
ferryidolscooksjuicyglarecartsalloybulbslawnslastsfuelsoddlycranefiledweirdshawlslipstroopboltssuitesleekquilttramp\
blazeatlasodorsscrubcrabsprobelogicadobeexilerebelgrindstingspineclingdesksgroveleapsproseloftyagonysnaretusksbulls\
moodshumidfinerdimlyplankchinapinesguiltsacksbracequotelathegailyfontsscalpadoptfoggyfernsgramsclumpperchtumorteens\
crankfablehedgegenessoberboasttractcigaruniteowingthighhaikuswishdikeswedgebootheasedfrailcoughtombsdartsfortschoir\
pouchpinchhairybuyertorchvigorwaltzheatsherbsusersflintclickmadambleakbluntaidedlacksmaskswadedrisksnursechaossewed\
curedampleleasesteaksinksmeritbluffbathegleambonuscoltsshearglandsilkyskatebirchanvilsledsgroanmaidsmeetsspeckhymns\
hintsdrownbosomslickquestcoilsspiedsnowssteadsnackplowsblondtamedthornwaitsgluedbanjoteasearenabulkycarvestuntwarms\
shadyrazorfollyleafynotchfoolsotterpearsflushgenusachedfivesflapsspoutsmotefumesadaptcuffstastystoopclipsdiskssniff\
lanesbriskimplydemonsuperfurryragedgrowltextshardystungtypedhateswisertimidserumbeaksrotorcastsbathsglideplotstrait\
resinslumslyricpuffsdecksbroodmournaloftabusewhirledgedovaryquackheapsslangawaitcivicsaintbevelsonarauntspacksfroze\
toniccorpsswarmfrankrepaygauntwiredniececelloneedychuckstonymediasurgehurtsrepelhuskydatedhuntsmistsexertdriesmates\
swornbakerspiceoasisboilsspursdovessneakpacescolonsiegestrumdriercacaohumusbalespipednastyrinseboxershrubamusetacks\
citedslungdeltaladenlarvarentsyellsspoolspillcrushjewelsnapsstainkickstyingslitsratedeeriesmashplumszebraearnsbushy\
scarysquadtutorsilksslabsbumpsevilsfangssnoutperilpivotyachtlobbyjeansgrinsviolalinercometscarschopsraidseaterslate\
skipssolesmistyurineknobssleethollypestsforksgrilltrayspailsbornetenorwarescarolwoodycanonwakeskittyminerpollsshaky\
nasalscornchesstaxiscrateshylytulipforgenymphbudgelowlyabidedepotoasesassesshedsfudgepillsrivetthinegroomlankyboost\
brothheavegravybeechtimedquailinertgearschickhingetrashclashsighsrenewboughdwarfslowsquillshavesporesixeschunkmadly\
pacedbraidfuzzymottospiesslackmucusmagmaawfuldiscseraseposedassetcidertapertheftchurnsatinslotstaxedbullyslothshale\
treadrakedcurdsmanoraislebulgeloinsstairtapesleansbunkssquattowedlancepanessakesheirscastedummyporesfaunacrookpoise\
epochriskywarnsflingberrygrapeflankdragssquidpeltsicingironyironsbarkswhoopchokedietswhipstallydozedtwinekitesbikes\
ticksriotsroarsvaultloomsscoldblinkdandypupaesievespikeductslendspizzabrinkwidenplumbpaganfeatsbisonsoggyscoopargon\
nudgeskiffambersexesrousesaltshitchexaltleashdinedchutesnortgustsmeloncheatreefsllamalassodebutquotaoathspronemixes\
raftsdivesstaleinletflickpintobrowsuntiebatchgreedchorestirsblushonsetbarbsvoltsbeigeswooppaddylacedshovejerkypoppy\
leaksfaresdodgegodlysquawaffixbrutenicerunduesnarlmergedosesshowydaddyroostvasesswirlpettycoldscurrycobragenieflare\
messycoressoaksripenwhineaminoplaidspinymowedbatonpeersvowedpiousswansexitsafootplugsidiomchiliritesserfscleftberth\
grubsannexdizzyhastylatchwaspsmirthbaronpleadaloofagingpixelbaredmummyhotlyaugerbuddychapsbadgestarkfairsgullymumps\
emeryfillyovensdronegauzeidiotfussyannoyshankgougebleedelvesropedunfitbaggymowerscantgrabsfleaslousyalbumsawedcooky\
murkyinferburlywageddingybrinekneelcreakvanessmokyspurtcombseasellaceshumpsrumoraromahordeswissleaptopiumslimeafire\
pansymaressoapshuskssnipshazellinedcafesnaivewrapssizedpiersbesetagiletongssteedfraudbootyvalordownywittymossypsalm\
scubatourspolkamilkygaudyshrugtuftswildslasertrussharescreedlilacsirentarrybribeswinemutedflipscuressinewboxedhoops\
gaspshoodsnicheyuccaglowssewerwhackfusesgownsdroopbuckspangsmailswhiskhavenclaspslingstinturgeschamppietychirppleat\
possesunupmenushowlsquakeknackplazafiendcakedbangseruptpokeroldencrampvoterposesmanlyslumpfinedgripsgapedpurgehiked\
maizefluffstrutsloopprowlroachcocksblanddialsplumeslapssoupsdullywillsfoamssolosskiereavestotemfusedlatexveilsmused\
mainsmyrrhracksgallsgnatsboutssisalshutshosesdrylyhoverglossseepsdenimputtyguppyleakyduskyfilthoboesspansfowlsadorn\
glazehauntdaresobeysbakesabysssmeltgangsachestrawlclapsundidspicyhoistfadesvicaracornpussygruffmustytartssnuffhunch\
trucetweeddryerlosersheafmoleslapsetawnyvexedautoswagerdomessheenclangspadesowedbroilslylystudsgruntdonorslugsaspen\
homercroaktithehaltsaverthavochoganglintruddyjeepsflakyladletauntsnorefinespropsprunepesosradiipokestileddaisyheron\
villafarcebindscitesfixesjerkslividwakedinkedboomschewslickshyenascofflustysonicsmithushertucksvigilmoltssectsspars\
dumpsscalywispssoresmincepandaflieraxlespliedboobypatiorabbipetalpolyptintsgratetrolltollsrelicphonybleatflawsflake\
snagsaptlydrawlulcersoapybossymonkscragscagedtwangdinertapedcadetgridsspawnguilenoosemoresgirthslimyaidesspasmburrs\
alibilymphsaucymuggyliterjokedgoofyexamsenactstorkluredtoxicomensnearscovetwrungforumvenommoodyaldersassyflairguild\
prayswrenshaulsstavetiltspecksstompgalestemptcapesmesasomitstepeeharrywringevokelimesclucklungehighscanesgiddylithe\
vergekhakiqueueloathfoyeroutdofareddetercrumbastirspirejumpyextolbuoysstubslucidthongaforewhiffmaximhullsclogsslats\
jiffyarborcinchigloogoodygazesdowelcalmsbitchscowlgulpscodedwavermasonlobesebonyflailislesclodsdazedadeptoozedsedan\
clayswartsketchskunkmanesadoresneermangofiordfloraroomyminksthawswattsfreerexultplushpaledtwainclinkscamppawedgrope\
bravogablestinkseverwanedrarerregalwardsfawnsbabesunifyamendoakengladevisorheftyninesthrobpecanbuttspencesillsjails\
flyersabernomadmiterbeepsdomedgulfscurbsheathmoorsaortalarkstangywrylycheepragesevadeluresfreakvoguetunicslamsknits\
dumpymaniaspitsfirthhikestrotsnosedclankdogmabloatbalsagraftmiddystilekeyedfinchspermchaffwilesamigocopraamisseying\
twirllurchpopeschinssmocktinesguisegritsjunksshoalcachetapiratolldeitytoilsspreemocksscansshornrevelravenhoaryreels\
scuffmimicweedycornytruerrougeemberfloestorsowipesedictsulkyrecurgroinbastekinkssurerpiggymoldyfrancliarsineptgusty\
facetjettyequipleperslinksoarscaterdowrysidedyearndecoytabooovalshealspleasberetspiltgaylyroverendowpygmycaratabbey\
ventswakenchimpfumedsodasvinylcloutwadesmitessmirkboresbunnysurlyfrockforaypurermilksquerymiredblarefrothgruelnavel\
palerpuffycasksgrimederbymammagavelteddyvomitmoansallotdeferwieldviperlouseerredhewedabhorwrestwaxenadageardorstabs\
poredrondolopedfishybiblehiresfoalsfeudsjambsthudsjeerskneadquirkrugbyexpelgreysrigoresterlyresabackglueslotuslurid\
rungshutchthymevalettommyyokesepicstrillpikesozonecaperchimefreesfamedleechsmiteneigheroderobedhoardsalveconicgawky\
crazejacksgloatmushyrumpsfetuswincepinksshalttootsglenscooedrustsstewsshredparkachugswinksclotsshrewbooedfilmyjuror\
dentsgummygrayshookybuttedogiepoledreamsfifesspankgayertepidspooktaintflirtroguespikyopalsmisercockycoylybalmyslosh\
brawlaphidfakedhydrabragschideyanksallayvideoaltoseasesmetedchasmlongsexceltaffyimpelsavorkoalaquaysdawnsproxyclove\
duetsdregstardybriargrimyultrameatyhalvewailssuedemauveenvoyarsoncovesgooeybrewssofaschumsamazezoomsabbothalosscour\
suingcribssagasenemawordyharpscoupemolarflopsweepsmintsashenfeltsaskewmunchmeweddivanvicesjumboblobsblotsspunkacrid\
topazcubedclansfleesslursgnawsweldsfordsemitsagatepumasmendsdarksdukespliescannyhootsoozeslamedfoulsclefsnicksmated\
skimsbrunttubertingefatesdittythinsfretseiderbayoumulchfastsamassdampsmornsfriarpalsyvistacroonconchuddertacosskits\
mikesquitspreenasteradderelegypulpyscowsbaledhovellavascraveopticweltsbustsknaverazedshinstotesscootdearscrockmutes\
trimsskeindotedshunsveersfakesyokedwooedhackssprigwandslullsseerssnobsnookspinedperkymooedfrilldinesboozetripeprong\
dripsodderleveeanticsidlepithycorksyelpsjokerfleckbuffsscramtiersbogeydolediratevalescopedhailseludebulksairedvying\
stagsstrewcoccipactsscabssilosdustsyodeltersejadedbaserjibesfoilsswaysforgoslayspreystreksquellpeeksassaylurkseject\
boarstritebelchgnashwanesluteswhimsdosedchewysnipeumbrateemsdozeskelpsuppedbrawndopedshushrindsslushmoronvoilewoken\
fjordsheikjestskayakslewstotedsanerdrapepattyravessulfagristskiedvixencivetvouchtiarahomeymopedruntssergekinkyrills\
cornsbratspriesamblefriesloonstsarsdatummuskypigmygnomeravelovuleicilylikenlemurfrayssiltssiftsplodsrampstressearls\
dudeswaivekaratjoltspeonsbeershornypaleswreaklairslynchstankswoonidlerabortblitzensueatonebingoroveskiltsscaldadios\
cynicdullsmemoselfindalespeelspealsbaressinuscronesablehindsshirkenrolwiltsroamsdupedcystsmittssafesspatscoopsfilet\
knellrefitcoveypunkskilnsfitlyabatetalcsheedsduelswanlyruffsgausslapeljauntwhelpcleatgauzydirgeeditswormymoatssmear\
prodsbowelfriskvestsbayedraspstamesdelveembedbefitwafercedednovasfeignspewslarchhuffsdolesmamashulkspriedbrimsirked\
aspicswipemealyskimpbluerslakedowdypenisbrayspupasegretflunkphloxgripepeonydouseblursdarnsslunkleftschatsinanevials\
stiltrinkswoofswowedbongsfrondingotevictsingeshyerfliedslopsdoltsdrooldellswhelkhippyfetedethercocoshivesjibedmazes\
triossirupsquablathsleerspastariftslopesaliaswhirsdicedslagslodesfoxedidledprowsplaitmaltschafecowertoyedchefskeels\
stiesraceretudesuckssulksmicasczarscopseailedablerrabidgoldscroupsnakyvisaspallsmopesbonedwispyravedswapsjunkydoily\
pawnstamerpoachbaitsdamnsgumbodauntprankhunksbuxomhereshonksstowsunbaridlesroutssagesgoadsremitcopesdeigncullsgirds\
haveslucksstunkdodosshamssnubsiconsusurpdoomshellssoledcomaspavesmathsperkslimpswombsblurbdaubscokessoursstunscased\
mustscoedscowedapingzonedrummyfetesskulkquaffrajahdeansreapsgalastillsrovedkudostonedparedscullvexespuntssnoopbails\
dameshazesloresmartsvoidsamebarakesadzesharmsrearssatyrswillhexescolicleekshurlsyowlsiviesplopsmuskspapawjellsbused\
cruetbidedfilchzestsrookslaxlyrendsloamsbaskssirescarpspokeyflitsmusesbawlsshuckvilerlispspeepssorerlollsprudediked\
flossflogsscumsdopesbogiepinkyleafstubasscadslowedyesesbikedqualmevenscanedgawkswhitswoolyglutsrompsbestsduncecrony\
joisttunasbonermallsparchaverscramsparesdallybigotkalesflaysleachgushypoochhugerslyergolfsmiresfluesloafsarcedacnes\
neonsfiefsdintsdazespoutscoredyulesliltsbeefsmuttsfellscowlsspudslamesjaweddupesdeadsbylawnoonsniftycluedvireogapes";

string GraphBaseWordsListB = "metescutermaimsdrollcupidmaulssedgepapaswheysekinglootshiltsmeowsbeausdicespeppyriperfogeygistsyogasgiltsskewscedes\
zealsalumsokayselopegrumpwaftssootsblimpheftsmullshosedcressdoffsruderpixiewaifsoustspucksbiersgulchsuetshoboslints\
branstealsgarbspeweehelmsturfsquipswendsbanesnapesicierswatsbagelhexedogresgonergildspyreslardsbidespagedtalonflout\
medicvealsputtsdirksdotestippyblurtpithsacingbarerwhetsgaitswoolsdunksherosswabsdirtsjuteshempssurfsokapichowsshoos\
dusksparrydecalfurlsciliasearsnovaemurkswarpsslueslamersarisweanspurrsdillstogasnewtsmeanybuntsrazesgoonswicksruses\
vendsgeodedrakejudosloftspulpslaudsmucksvisesmochaoiledromanethylgottafuguesmackgourdbumpyradixfattyboraxcubitcacti\
gammafocalavailpapalgollyeliteversabillyadieuannumhowdyrhinonormsbobbyaxiomsetupyolksternsmixergenreknollabodejunta\
gorgecomboalphaovertkindaspeltpricknoblyephodaudiomodalveldtwartyflukebonnybreamrosinbollsdoersdownsbeadymotifhumph\
fellamouldcrepekernsalohaglyphazureriserblestlocuslumpyberylwannabriertunerrowdymuraltimercanstkrillquothlemmetriad\
tenonamplydeepspadreleantpaceroctaldollytranssumacfoamylollygiverquipucodexmannaunwedvodkafernysalondupleboronrevue\
crieralackinterdillywhistcultsspakeresetloessdecormoververveethicgamutlingodunnoalignsissyincurreedyavantpiperwaxer\
calyxbasilcoonsseinepineylemmatramswinchwhirrsaithionicheadyharemtummysallyshieddrossfaradsavertildejingobowerserif\
factobelleinsetboguscavedfortesootybongotovescredobasalyellaaglowgleangustohymenethosterrabrashscripswashalephtinny\
itchywantatricejowlsgongsgardeborictwillsowerhenryawashlibelspurnsabrerebutpenalobesesonnyquirtmebbetacitgreekxenon\
hullopiquerogernegrohadstgeckobegetuncutaloeslouisquintclunkrapedsalvodiodemateyhertzxylemkioskapacecawedpeterwench\
cohossortagambabytestangonuttyaxialalecknatalclompgoredsireebandygunnyrunicwhizzrupeefatedwiperbardsbrinystaidhocks\
ochreyummygentssoupyroperswathcameoedgerspategimmeebbedbrevethetadeemsdykesservotellytabbytaresblocswelchghoulvitae\
cumindinkybronctaborteenycomerborersiredprivymammydearygyrosspritcongaquirethugsfurorblokerunesbawdycadretoxinannul\
eggedanionnodespickysteinjelloauditechosfagotletupeyriefountcapedaxonsamuckbanalriledpetitumbermilerfibreagavebated\
bilgevitrofeintpudgymatermanicumpedpeskystrepslurppylonpureecarettempsnewelyawnsseedytreedcoupsrangybradsmangyloner\
circatibiaafoulmommytitercarnekookymotesamitysuavehippocurvysambanewsyaniseimamstulleawayslivenhallowalesoptedcanto\
idyllbodescuriowrackhikerchiveyokeldottydemurcuspsspecsquadslaitytonerdecrywritssauteclackaughtlogostipsynattyducal\
bidetbulgymetrelustsunarygoethbalersitedshieshaspsbrungholedswanklookymeleehuffyloamypimpstitanbingeshuntfemurlibra\
sederhonedannascoypushimszowiejihadsavvynadirbassomonicmanedmousyomegalaverprimapicasfoliomeccarealstrothtestybalky\
crimpchinkabetssplatabacivauntcutiepastymoraylevisrattyisletjoustmotetviralnukesgradscomfyvoilawoozybluedwhompsward\
metroskeetchineaeriebowietubbyemirscoatiunzipslobstrikefunkyducatdeweyskoalwadisoomphtakerminimgetupstoicsynodrunty\
flybybrazeinlayvenueloutspeatyorlonhumpyradonbeautraspyunfedcricknappyvizoryipesrebusdivotkiwisvetchsquibsitarkiddo\
dyerscottamatzolagerzebuscrassdachakneeddictafakirknurlrunnyunpinjulepglobsnudessushitackystokekaputbutchhulascroft\
achoogeniinodaloutgospielviolsfetidcageyfudgyepoxyleggyhankylapisfelonbeefycootsmelbacaddyseguebetelfrizzdrearkooks\
turbohoagymoulthelixzonalariasnoseypaeanlaceybannsswainfryerretchtenetgigaswhinyogledrumenbegotcruseabutsrivenbalks\
sinessigmaabaseennuigoresunsetaugursatedodiumlatindingsmoirescionhennakrautdicksliferprigsbebopgagesgazerfannygibes\
auraltempihoochrapessnuckhartstechsemendninnyguavascarpliegetuftysepiatomescarobemceepramsposerversohubbajoulebaize\
blipsscrimcubbyclavewinosrearmlienslumenchumpnannytrumpfichuchomphomospurtymaserwooshpatsyshillrusksavastswamiboded\
ahhhhlobednatchshishtansysnootpayeralthosappylaxerhubbyaegisrilesdittojazzydingoquasiseptapeakylorryheerdbittypayee\
seamyapsesimbuebeliecharyspoofphylaclimebabelwackysumpsskidskhanscryptinurenonceoutenfairehooeyanolekazoocalvelimbo\
argotduckyfakervibesgassyunlitnervyfemmebiterficheboorsgaffesaxesrecapsynchfaciediceyouijahewerlegitgurusedifytweak\
carontyposrerunpollysurdshamzanullshaterleftymogulmafiadebugpatesblabssplaytaluspornomoolanixedkilossnidehorsygesso\
jaggytrovenixescreelpateriotascadgeskyedhokumfurzeankhscurienutsyhilumremixangstburlsjimmyveinytrystcodonbefoggamed\
flumeaxmandoozylubesrheasbozosbutylkellymynahjocksdonutavianwurstchockquashqualshayedbombecushyspacypukedleerythews\
prinkamensteslaintrofiverfrumpcaposopinecodernamerjowlypukeshaledchardduffsbruinreusewhangtoonsfratssiltytelexcutup\
niseineatodecafsoftybimboadlibloonyshoedaguespeevenowaygameysargereranepactpottyconedupendnarcoikatswhorljinkstizzy\
weepypositmargeveganclopsnumbsreeksrubesrowerbipedtiffshocushammybuncofixittykeschawsyuckyhokeyresewmavenadmanscuzz\
slogssousenachomimedmeldsboffodebitpinupvagusgulagrandybosuneducefaxesauraspestoantsybetasfizzydorkysnitsmoxiethane\
mylarnobbygamingoutyessesgoyimpaneddruidjadesrehabgofertzarsoctethomedsockodorksearedantedelidefazesoxbowdowsesitus\
macawsconedrilyhypersalsamoochgatedunjamlipidmitrevenalknishritzydivastorusmangedimerrecutmesonwinedfendsphagefiats\
caulkcavilpantyroansbilkshonesbotchestopsullysoothgeldsaholdraperpagerfixerinfixhickstuxesplebetwitsabashtwixtwacko\
primpnablagirtsmiffsemotexeroxrebidshahsruttygroutgriftdeifybiddykopeksemisbriesacmespitonhussytortsdiscowhoreboozy\
gibedvampsamoursoppygonzodurstwadertutuspermscattyglitzbrigsnerdsbarmygizmoowletsayermollsshardwhopscompscorercolas\
mattedroidploysvapidcairndeismmixupyikesprosyrakerflubswhishreifycrapsshagsclonehazedmachorectorefixdramsbikeraquas\
porkydoyenexudegoofsdivvynoelsjivedhulkycagerharpyoldievivasadmixcodaszilchdeistorcasretropilafparserantszingytoddy\
chiffmicroveepsgirlynexusdemosbibbsanteslulusgnarlzippyiviedepeeswimpstrompgrailyoyospoufshalesroustcabalrawerpampa\
moseykefirburgsunmetcuspyboobsboonshypesdynesnardslanaiyogissepalquarktokedprateayinshawedswigsvitastokerdoperbossa\
lintyfoistmondostashkayostwerpzestycaponwimpyrewedfungotarotfroshkabobpinkoredidmimeoheisttarpslamassutradinarwhams\
bustyspaysmambonabobprepsodourcabbyconkssluffdadoshouriswartbalmsgutsyfaxedegadspushyretryagoradrubsdaffychitsmufti\
karmalottotoffsburpsdeucezingskappacladsdoggyduperscamsoglermimesthroezetaswaledpromoblatsmuffsoinksviandcosetfinks\
faddyminissnafusaunausurymuxescrawsstatscondocoxesloopydormsascotdippyexecsdopeyenvoiumptygismofazedstropjivesslims\
batikpingssonlyleggopekoeprawnluauscampyoodleprexypromstoutsoglestweettoadynaiadhidernukedfatsoslutsobitsnarcstyros\
deliswooerhypedposetbywaytexasscrodavowsfutontortetuplecaromkebabtampsjiltsdualsartsyrepromodemtopedpsychsickoklutz\
tarnscoxeddrayscloysandedpikeraimersuraslimosflackhapaxdutchmuckyshirekliegstaphlayuptokesaxingtoperduvetcowryprofs\
blahsaddlesudsybattycoifssuetygabbyhaftapitasgoudadeicetaupetopesduchynitrocarnylimeyoralshirertaxerroilsrubleelate\
dolorwryersnotsquaiscokedgimelgorseminasgoestagapemantajingsiliacadmenoffencillsoffallottabolasthwapalwayboggydonna\
locosbelayglueybitsymimsyhilarouttavroomfetalrathsrenaldyadscrocsviresculpakivasfeistteatsthatsyawlswhensabacaohhhh\
aphisfustyeclatperdumaystexeatmollysuprawetlyplasmbuffasemenpukkataguaparasstoatseccocartehautemolalshadsformaovoid\
pionsmodusbuenorheumscurfparerephahdoestsprueflamsmoltodiethchoosmikedbronxgoopyballyplumymoonymortsyournbipodspume\
algalambitmuchospueddozerharumgroatskintlaudethrumpappyoncetrimedgiguelimedpleinredlyhumpflitesseestgrebeabsitthanx\
pshawyawpsplatspayedarealtilthyousegwinetheeswatsalentospitzyawedgipsyspratcornuamahsblowywahoolubramecumwhooocoqui\
sabraedemamradsdicotastrokitedouzeldidosgratabonneaxmenklunksummalavespurlsyawnytearymasselargobazarpssstsylphlulab\
toquefugitplunkortholucrecoochwhiptfolkytyreswheeecorkyinjunsolondidotkerfsrayedwassachilebegatnippylitremagnarebox\
hydromilchbrentgyveslazedfeuedmavisinaptbaulkcasusscrumwisedfossadowerkyriebhoysscusefeuarohmicjusteukasebeauxtusky\
oratemustalardyintraquiffepsomneathochertaredhommemezzocormspsoasbeakyterryinfraspivstuansbellibergsanimaweirsmahua\
scopsmansetitrecuriakebobcycadtalkyfuckstapisamidedolcesloesjakesrusseblashtuttiprutapangablebstenchswarfheremmissy\
mersepawkylimenvivrechertunseetirosbrackfootswelshfosseknopsileumnoirefirmapodgylairdthunkshuterowanshojipoesyuncap\
famesgleescostaturpsforessolumimagobyresfonduconeypolisdictukraalsherdmumbowrothcharsunboxvacuosluedweesthadeswiled\
syncsmuserexconhoarssibylpassejoeyslotsaleptashaysbocksenduedarernonesileusplashbusbywhealbuffoyobbobilespoxesrooty\
licittercebromohayeydweebimbedsaranbruitpunkysoftsbiffsloppyagarsaquaelivrebiomebundsshewsdiemsginnydegumpolosdesex\
unmandungyvitamwedgyglebeapersridgyroidswifeyvapeswhoasbunkoyolkyulnasreekybodgebrantdavitdequelikerjennytactsfulls\
treapligneackedrefryvoweraarghchurlmommagaolswhumparrasmarlstilergrogsmemesmidistidedhalerducestwinyposteunrigprise\
drabsquidsfacerspierbaricgeoidremaptriergunksstenostomaairerovatetorahapiansmutspocksyurtsexurbdefognuderboskynimbi\
mothyjoyedlabiapardsjammybiglyfaxerhoppynurbscotesdishyvisedcelebpismocasaswithsdodgyscudimungsmuonsureasioctlunhip\
kronesagerverstexpatgronkuvulashawmbilgybraescentowebbylippygamiclordymazedtingsshoatfaerywirerdiazocarerratergreps\
rentezlotyviersunaptpoopsfecalkepistaxoneyerswontsspinastoaeyentapooeyburetjapanbedewhaftsselfsoaredherbypryeroakum\
dinkstittysepoypenesfuseewineygimpsnihilrillegiberouselumiakcuppyhamesshitsazinegladstacetbumphcoyerhonkygamergooky\
waspysedgybentsvariadjinnjuncopubicwilcolazesidylslupusrivessnoodschmospazzfinisnoterpavanorbedbatespipetbaddygoers\
shakostetssebumseethlobarraverajugaricedveldsdribsvilledhowsunsewhalmakronalimbyjiffstreysbaudspffftmimerplebscaner\
jibercuppawashychuffunarmyukkystyeswakerflaksmacesrimesgimpyguanoliraskapokscudsbwanaoringaiderprierklugymontegolem\
velarfirerpietaumbelcampounpegfoveaabeambosonaskergothsvocabvinedtrowstikisloperindieboffsspanggrapytaterichorkilty\
lochssupesdegasflicstorsibethsweberresawlawnycovenmujikreletthermheighshnortruedzayinliestbarfsbassiqophsroilyflabs\
punnyokrashanksdipsonerfsfaunscallapseudlurermagusobeahatriatwinkpalmypockypendsrectaplonkslawskeensnicadponesinker\
whewsgroksmoststrewsulnargyppycocasexposeructoilervacuadreckdaterarumstubalvoxeldixitbeeryassailadesactinghotibuzzy\
meadsgrodyribbyclewscremeemailpyxiekulakboccirivedduddyhoperlapinwonkspetriphialfugalholonboomyduomomusosshierhayer\
porgyhivedlithofistystagyluvyamariasmogsasanayogicslomofawnyamineweftsgonadtwirpbravaplyerfermilogesniterrevetunate\
gyvedtottyzappyhonergirosdicercalksluxesmonadcruftquoinfumerampedshlepvincayahoovulvazooeydryadnixiemoperiambslunes\
nudielimnswealsnohowmiaowgoutsmynasmazerkikesoxeyestoupjujusdebarpubestaelsdefunrandsblearpavergoosysprogoleostoffy\
pawermacedcritsklugetubedsahibganefscatssputavanedacnedtaxolplinkowethtribsresayboulethoushaplyglansmaxisbezelantis\
porksquoitalkydglarybeamyhexadbonkstecumkerbsfilarfrierreduxabuzzfadershoercouthtruesguyedgoonybookyfuzeshurlygenet\
hodadcalixfilerpawlsiodicuterohengeunsaylierspiingwealdsexedfolicpoxedcuntsanilekithsbeckstattyplenarebarabledtoyer\
attarteaksaioliawinganentfecesredipwistspratsmesnemutersmurfowestbahtslossyftpedhunkyhoerssliersicksfatlydelfthiver\
himbopengobusksloxeszonksiliumaportikonsmulctreevecivvycannabarfykaiakscudoknoutgaperbhangpeaseuterilasespatenrasae\
axelsstoasombrestyligunkyhazerkenafahoysammosweenyurgerkudzuparenbolosfetornittytechyliethsomasdarkyvilligluonjanes\
cantsfartssoclejinnsruingslilyricerhaddawoweericesnertscaulsswiveliltymicksaritypashafinifoinkyguttytetrawiseswolds\
baldspicotwhatsshikibungssnarflegosdungsstogybermstangsvailsroodsmorelswareelanslatusgulesrazerdoxiebuenaoversgutta\
zincsnateskirkstikesdoneejerrymohelcederdogesunmapfoliarawlysnarktopoiceilsimmixyoresdiestbubbapompsforkyturdylawzy\
poohswortsglomsbeanomuleybarkytunnyauricfunksgaffscordycurdylisletoricsoyasremanmungycarpyapishoatengappyauraebract\
rookyaxledburrysizerproemturfyimpromashymiensnonnyoliosgrooksatesagleycorgidashydoserdildoapsosxoredlakerplayaselah\
maltydulsefrigsdemitwhosorialssawerspicsbedimsnugsfaninazoicicerssuerswizenkoinetoposshirrriferferalladedlasedturds\
swedeeastscozenunhitpallyaitchsedumcoperruchegeeksswagsetextalginoffedninjaholerdotertoterbesotdicutmacerpeenspewit\
redoxpoleryecchflukydoethtwatscrudsbebugbiderstelehexerwestsgluerpilauabaftwhelmlacerinodetabusgatorcuingreflyluted\
cukesbairnbightarsescrumploggyblinispoortoyonharkswazoofennynaveskeyertufasmorphrajastypalspiffoxlipunbanmussyfinny\
rimerloginmolascirrihuzzaagoneunsexunwonpeatstoilezombidewednookyalkylixnaydoveyholeycuberamylspodiachinoapneaprims\
lycrajohnsprimofatwaeggerhempysnookhyingfuzedbarmscrinkmootsyerbarhumbunarcdirermungeelandnareswriernoddyatiltjukes\
enderthensunfixdoggozooksdiddyshmoobruskprestcurerpastskelpyboccekickytaroslingsdickynerdyabendstelabiggylavedbaldy\
pubisgookswonkystiedhyposassedspumyosierroblerumbabiffypupal";
int sizeOfA = GraphBaseWordsListA.size();
int sizeOfB = GraphBaseWordsListB.size();
int GraphBaseWords_Component::method2(Sheet* pg) {

	output[0].stringValue.resize(1);
	output[0].stringValue[0] = "";
	output[0].stringType = true;

	int n = input[0].value.size();
	output[0].stringValue.resize(n);

	for (int j = 0; j < n; j++) {
		int k = input[0].value[j] * 5;
		if (k < 0)
			output[0].stringValue[j] = "";
		else if (k < sizeOfA)
			output[0].stringValue[j] = GraphBaseWordsListA.substr(k, 5);
		else if (k < sizeOfA + sizeOfB)
			output[0].stringValue[j] = GraphBaseWordsListB.substr(k - sizeOfA, 5);
		else
			output[0].stringValue[j] = "";
	}
	output[0].dimensions[0] = n;

	return 1;
}
GraphBaseWords_Component::GraphBaseWords_Component()
	: Component(L"GraphBaseWords", L"GraphBase Words", 1, 1)
{
	setLeftLabels(3, "Index");
}


// ************************ MIDI Frequency *************************

double MidiFreq[128] = { 8.18, 8.66,9.18,9.72,10.3,10.91,11.56,12.25,12.98,
	13.75,14.57,15.43,16.35,17.32,18.35,19.45,20.6,21.83,23.12,24.5,25.96,
	27.5,29.14,30.87,32.7,34.65,36.71,38.89,41.2,43.65,46.25,49,51.91,
	55,58.27,61.74,65.41,69.3,73.42,77.78,82.41,87.31,92.5,98,103.83,
	110,116.54,123.47,130.81,138.59,146.83,155.56,164.81,174.61,185,196,207.65,
	220,233.08,246.94,261.63,277.18,293.66,311.13,329.63,349.23,369.99,392,415.3,
	440,466.16,493.88,523.25,554.37,587.33,622.25,659.26,698.46,739.99,783.99,830.61,
	880,932.33,987.77,1046.5,1108.73,1174.66,1244.51,1318.51,1396.91,1479.98,1567.98,1661.22,
	1760,1864.66,1975.53,2093,2217.46,2349.32,2489.02,2637.02,2793.83,2959.96,3135.96,3322.44,
	3520,3729.31,3951.07,4186.01,4434.92,4698.64,4978.03,5274.04,5587.65,5919.91,6271.93,6644.88,
	7040,7458.62,7902.13,8372.02,8869.84,9397.27,9956.06,10548.08,11175.3,11839.82,12543.85 };

string MidiName[128] = { "C","C# / Db","D","D# / Eb","E","F","F# / Gb","G","G# / Ab",
"A","A# / Bb","B","C","C# / Db","D","D# / Eb","E","F","F# / Gb","G","G# / Ab",
"A0","A#0 / Bb0","B0","C1","C#1 / Db1","D1","D#1 / Eb1","E1","F1","F#1 / Gb1","G1","G#1 / Ab1",
"A1","A#1 / Bb1","B1","C2","C#2 / Db2","D2","D#2 / Eb2","E2","F2","F#2 / Gb2","G2","G#2 / Ab2",
"A2","A#2 / Bb2","B2","C3","C#3 / Db3","D3","D#3 / Eb3","E3","F3","F#3 / Gb3","G3","G#3 / Ab3",
"A3","A#3 / Bb3","B3","C4","C#4 / Db4","D4","D#4 / Eb4","E4","F4","F#4 / Gb4","G4","G#4 / Ab4",
"A4","A#4 / Bb4","B4","C5","C#5 / Db5","D5","D#5 / Eb5","E5","F5","F#5 / Gb5","G5","G#5 / Ab5",
"A5","A#5 / Bb5","B5","C6","C#6 / Db6","D6","D#6 / Eb6","E6","F6","F#6 / Gb6","G6","G#6 / Ab6",
"A6","A#6 / Bb6","B6","C7","C#7 / Db7","D7","D#7 / Eb7","E7","F7","F#7 / Gb7","G7","G#7 / Ab7",
"A7","A#7 / Bb7","B7","C8","C#8 / Db8","D8","D#8 / Eb8","E8","F8","F#8 / Gb8","G8","G#8 / Ab8",
"A8","A#8 / Bb8","B8","C9","C#9 / Db9","D9","D#9 / Eb9","E9","F9","F#9 / Gb9","G9" };

int MIDI_Pitch_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	output[1].stringValue.resize(loops);
	output[1].stringType = true;

	for (int j = 0; j < loops; j++) {
		int midi = getLoopingValueD(0, j, 60); // middle c = C4
		if (midi < 0) {
			output[0].value[j] = 0;
			output[1].stringValue[j] = "";
		}
		else if (midi > 127) {
			output[0].value[j] = 0;
			output[1].stringValue[j] = "";
		}
		else {
			output[0].value[j] = MidiFreq[midi];
			output[1].stringValue[j] = MidiName[midi];
		}
	}
	copyDimensions(0, 0);
	copyDimensions(0, 1);

	return 1;
}
MIDI_Pitch_Component::MIDI_Pitch_Component()
	: Component(L"MidiPitch", L"MIDI Pitch", squareShape, 1, 1, 2, 1)
{
	setLeftLabels(3, "MIDI");
	setRightLabels(3, "Pitch", "Name");
	width = 40;
}

// ************************ Instrument Harmonics *************************
InstrumentHarmonics::InstrumentHarmonics(string _name, int _count, vector<int> _harmonic, vector<double> _amplitude){

	name = _name;
	count = _count;
	harmonic = _harmonic;
	amplitude = _amplitude;
}
std::vector<InstrumentHarmonics*>InstrumentHarmonicsList{
	new InstrumentHarmonics("Clarinet 156 Hz", 14,
	{ 1,   2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14   },
	{ 1.0, 0.04, 0.99, 0.12, 0.53, 0.11, 0.26, 0.05, 0.24, 0.07, 0.02, 0.03, 0.02, 0.03 }),
	new InstrumentHarmonics("Clarinet 233 Hz",    14,
	{ 1,   2,   3,    4,    5,    6,    7,    8,   9,    10,   11,   12,   13   },
	{ 1.0, 0.8, 0.82, 0.07, 0.29, 0.35, 0.06, 0.1, 0.03, 0.01, 0.02, 0.02, 0.01 }),
	new InstrumentHarmonics("Clarinet 698 Hz",    5,
	{ 1,   2,    3,    4,    6    },
	{ 1.0, 0.42, 0.04, 0.01, 0.01 }),
	new InstrumentHarmonics("Euphonium 58 Hz",    16,
	{ 1,    2,    3,    4,    5,    6,    7,    8,    9,    10,   11,   12,   13,   14,   15,   16   },
	{ 0.17, 0.85, 0.91, 0.97, 0.81, 0.48, 0.41, 1.00, 0.46, 0.16, 0.08, 0.23, 0.18, 0.11, 0.02, 0.01 }),
	new InstrumentHarmonics("Euphonium 175 Hz",	6,
	{ 1,   2,    3,   4,    5,    8    },
	{ 1.0, 0.89, 0.5, 0.15, 0.01, 0.01 }),
	new InstrumentHarmonics("Trombone 155 Hz",    9,
	{ 1,    2,   3,   4,    5,    6,    7,    8,    9    },
	{ 0.78, 0.9, 1.0, 0.05, 0.08, 0.03, 0.12, 0.09, 0.01 }),
	new InstrumentHarmonics("Trombone 175 Hz",    8,
	{ 1,    2,    3,   4,   5,    6,    7,    9    },
	{ 0.73, 0.45, 1.0, 0.3, 0.25, 0.07, 0.02, 0.01 }),
	new InstrumentHarmonics("Trombone 233 Hz",    6,
	{ 1,   2,    3,    4,    5,    6    },
	{ 1.0, 0.52, 0.11, 0.01, 0.04, 0.02}),
	new InstrumentHarmonics("Saxophone 262 Hz",	5,
	{ 1,    2,   3,   4,   5    },
	{ 0.52, 1.0, 0.4, 0.4, 0.01 }),
	new InstrumentHarmonics("Saxophone 330 Hz",	5,
	{ 1,   2,    3,   4,   7    },
	{ 1.0, 0.81, 0.3, 0.1, 0.01 }),
	new InstrumentHarmonics("Saxophone 392 Hz",	6,
	{ 1,   2,    3,    4,    6,    7    },
	{ 1.0, 0.38, 0.14, 0.02, 0.01, 0.01 }),
	new InstrumentHarmonics("Flute 247 Hz",		10,
	{ 1,   2,    3,    4,    5,    6,    7,    8,    9,    10   },
	{ 1.0, 0.65, 0.61, 0.15, 0.09, 0.02, 0.02, 0.01, 0.01, 0.01 }),
	new InstrumentHarmonics("Flute 349 Hz",		5,
	{ 1,   2,    3,   4,    5,    6,    8,    11   },
	{ 1.0, 0.61, 0.1, 0.24, 0.11, 0.09, 0.02, 0.01 }),
	new InstrumentHarmonics("Flute 440 Hz",		6,
	{ 1,   2,    3,    4,    5,    6    },
	{ 1.0, 0.77, 0.18, 0.11, 0.05, 0.02 }),
	new InstrumentHarmonics("Bassoon 110 Hz",		15,
	{ 1,    2,    3,    4,    5,   6,    7,    8,    9,    10,   11,   12,   13,   16,   17   },
	{ 0.19, 0.19, 0.34, 0.61, 1.0, 0.29, 0.05, 0.15, 0.09, 0.01, 0.12, 0.03, 0.01, 0.03, 0.02 }),
	new InstrumentHarmonics("Bassoon 123 Hz",		13,
	{ 1,    2,   3,    4,   5,    6,    7,    8,    9,    10,   11,   12,   14   },
	{ 0.07, 0.3, 0.55, 1.0, 0.13, 0.11, 0.15, 0.01, 0.01, 0.06, 0.01, 0.01, 0.02 }),
	new InstrumentHarmonics("Bassoon 247 Hz",	6,
	{ 1,    2,   3,    4,    5,    6,    7,    16   },
	{ 0.45, 1.0, 0.34, 0.03, 0.05, 0.03, 0.02, 0.01}),
	new InstrumentHarmonics("Square Wave",    8,
	{ 1,   3,      5,   7,      9,       11,     13,     15    },
	{ 1.0, 0.3333, 0.2, 0.1428, 0.1111,  0.0909, 0.0769, 0.0666 }),
	new InstrumentHarmonics("Saw Tooth",      8,
	{ 1,   2,   3,      4,    5,   6,      7,      8     },
	{ 1.0, 0.5, 0.3333, 0.25, 0.2, 0.1667, 0.1428, 0.125 }),
	new InstrumentHarmonics("Triangle Wave",  5,
	{ 1,   3,      5,    7,      9,     },
	{ 1.0, 0.1111, 0.04, 0.0204, 0.0123 })
};
int InstrumentHarmonics_Component::method2(Sheet* pg) {

	if (!checkValueInputs())
		return clearOutputs();

	output[0].value.resize(0);
	output[1].stringValue.resize(1);
	output[1].stringType = true;

	int count = 0;
	
	int midi = getLoopingValueD(0, 0, 0);
	if (midi < 0) {
		output[1].stringValue[0] = "";
	}
	else if (midi >= InstrumentHarmonicsList.size()) {
		output[1].stringValue[0] = "";
	}
	else {
		count = InstrumentHarmonicsList[midi]->count;
		output[1].stringValue[0] = InstrumentHarmonicsList[midi]->name;

		int listSize = InstrumentHarmonicsList[midi]->harmonic.size();
		int lastHarm = InstrumentHarmonicsList[midi]->harmonic[listSize - 1];
		int thisHarm = 0;
		for (int j = 1; j <= lastHarm; j++) {
			if (j == InstrumentHarmonicsList[midi]->harmonic[thisHarm]) {
				output[0].value.push_back(InstrumentHarmonicsList[midi]->amplitude[thisHarm]);
				thisHarm++;
			}
			else
				output[0].value.push_back(0);
		}
	}
	
	output[0].dimensions[0] = output[1].value.size();
	output[1].dimensions[0] = 1;

	return 1;
}

InstrumentHarmonics_Component::InstrumentHarmonics_Component()
	: Component(L"InstHarm", L"Instrument Harmonics", squareShape, 1, 1, 2, 1)
{
	setLeftLabels(3, "Index");
	setRightLabels(3, "Harmonics", "Name");
	width = 50;
}