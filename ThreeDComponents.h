#pragma once

#include "buildComponent.h"

void unitMatrix(double unitMat[16]);
void matrixRotationX(double angle, double rotMat[16]);
void matrixRotationY(double angle, double rotMat[16]);
void matrixRotationZ(double angle, double rotMat[16]);
void matrixRotationXMult(double angle, double rotMat[16]);
void matrixRotationYMult(double angle, double rotMat[16]);
void matrixRotationZMult(double angle, double rotMat[16]);


class ThreeD_Component : public Component {
public:
	ThreeD_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS);
	~ThreeD_Component();
	int method2(Sheet* pg);
	void inputsTo3dLoops(int matBit);
	int matPin;
	int loops;
	double inMat[16];
	void getColorGroup(int group, int* pinptr, int k);
	double inputRed;
	double inputGreen;
	double inputBlue;
	void getPositionGroup(int group, int* pinptr, int k);
	double inputX;
	double inputY;
	double inputZ;
	void getRotationGroup(int group, int* pinptr, int k);
	double inputRotateX;
	double inputRotateY;
	double inputRotateZ;
	void getSizeGroup(int group, int* pinptr, int k, double def);
	double inputSizeX;
	double inputSizeY;
	double inputSizeZ;
};





class Torus_Component :		 public ThreeD_Component {
public: Torus_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Sphere_Component : public ThreeD_Component {
public: Sphere_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Cone_Component :		 public ThreeD_Component {
public: Cone_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class Cube3D_Component :	 public ThreeD_Component {
public: Cube3D_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Cylinder_Component :		 public ThreeD_Component {
public: Cylinder_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
#if 0
class Tetra_Component : public ThreeD_Component {
public: Tetra_Component();
		void new3dOjbect();
		bool onLoad(Sheet* pg);
		vector<int> GetPinGroups();
		vector<string> GetPinLabels();
};
class Octahedron_Component :	 public ThreeD_Component {
public: Octahedron_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Dodecahedron_Component :	 public ThreeD_Component {
public: Dodecahedron_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Icosahedron_Component : public ThreeD_Component {
public: Icosahedron_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
#endif
class Rotate3D_Component : public Component { 
public: Rotate3D_Component(); 
	int method2(Sheet* pg);
};
class RotateX_Component :	 public Component { 
public: RotateX_Component(); 
	int method2(Sheet* pg);
};
class RotateY_Component :  public Component { 
public: RotateY_Component(); 
	int method2(Sheet* pg);
};
class RotateZ_Component :	 public Component { 
public: RotateZ_Component(); 
	int method2(Sheet* pg);
};
class Scale3D_Component :		 public Component {
public: Scale3D_Component(); 
	int method2(Sheet* pg);
};
class Shift3D_Component :	 public Component { 
public: Shift3D_Component();
	int method2(Sheet* pg);
};
class PolyLine3D_Component : public ThreeD_Component {
	public: PolyLine3D_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Lines3D_Component :		 public ThreeD_Component {
public: Lines3D_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Polygon3D_Component :	 public ThreeD_Component {
public: Polygon3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Circle3D_Component :	 public ThreeD_Component {
public: Circle3D_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Grid3D_Component :	 public ThreeD_Component {
	public: Grid3D_Component(); 
	void new3dOjbect();
};
class Axis3D_Component :		 public ThreeD_Component {
public: Axis3D_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Arrow3D_Component :		 public ThreeD_Component {
public: Arrow3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Lathe3D_Component :		 public ThreeD_Component {
public: Lathe3D_Component(); 
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class Extrude3D_Component :	 public ThreeD_Component {
public: Extrude3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};
class PlaneSurface3D_Component :		public ThreeD_Component {
public: PlaneSurface3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class SphereSurface3D_Component :	 public ThreeD_Component {
public: SphereSurface3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class CylinderSurface3D_Component :		public ThreeD_Component {
public: CylinderSurface3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class Surface3D_Component :			public ThreeD_Component {
public: Surface3D_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};

class PlaneSurface3D2_Component :	public ThreeD_Component {
public: PlaneSurface3D2_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class SphereSurface3D2_Component : public ThreeD_Component {
public: SphereSurface3D2_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class CylinderSurface3D2_Component : public ThreeD_Component {
public: CylinderSurface3D2_Component();
	void new3dOjbect();
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};
class View3D_Component : public ThreeD_Component {
public: View3D_Component();
	void new3dOjbect();
};
class Light3D_Component : public Component {
public: Light3D_Component(Sheet* pg);
		~Light3D_Component();
	int method2(Sheet* pg);
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
	void completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue);
};

class GetDirection3D_Component : public Component {
public: GetDirection3D_Component(Sheet* pg);
	int method2(Sheet* pg);
};

class Bridge3D_Component : public Component {
public: Bridge3D_Component(Sheet* pg);
	int method2(Sheet* pg);
};

class ViewControl3D_Component : public Component {
public:	ViewControl3D_Component();
		~ViewControl3D_Component();
	int method2(Sheet*);
	bool dial2d(Sheet*);
	bool onLoad(Sheet* pg);
	vector<int> GetPinGroups();
	vector<string> GetPinLabels();
};