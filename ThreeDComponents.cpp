#include "stdafx.h"
#include "ThreeDComponents.h"
#include "MethodComponents.h"
#include "FunctionComponents.h"
#include "ScreenDialog.h"
#include <stdlib.h>
#include <GL/glut.h>
#include "Sheet.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

extern Sheet* page;


void matrixMultiply_4x4(double a[16], double b[16], double c[16]) {

	int z = 0;
	for (int ii = 0; ii < 4; ii++) {
		for (int jj = 0; jj < 4; jj++) {
			double sum = 0;
			for (int kk = 0; kk < 4; kk++) {
				double A = a[ii * 4 + kk];
				double B = b[kk * 4 + jj];
				sum += A * B;
			}
			c[z++] = sum;
		}
	}
}

void unitMatrix(double unitMat[16]) {
	for (int j = 0; j < 16; j++)
		unitMat[j] = 0;
	for (int j = 0; j < 4; j++)
		unitMat[j * 5] = 1;
}
void copyMatrix(double inMat[16], double outMat[16]) {
	for (int j = 0; j < 16; j++)
		outMat[j] = inMat[j];
}
void matrixRotationX(double angle, double rotMat[16]) {
	unitMatrix(rotMat);
	rotMat[5] = cos(angle);
	rotMat[6] = sin(angle);
	rotMat[9] = -rotMat[6];
	rotMat[10] = rotMat[5];
}

void matrixRotationY(double angle, double rotMat[16]) {
	unitMatrix(rotMat);
	rotMat[0] = cos(angle);
	rotMat[2] = -sin(angle);
	rotMat[8] = -rotMat[2];
	rotMat[10] = rotMat[0];
}

void matrixRotationZ(double angle, double rotMat[16]) {
	unitMatrix(rotMat);
	rotMat[0] = cos(angle);
	rotMat[1] = sin(angle);
	rotMat[4] = -rotMat[1];
	rotMat[5] = rotMat[0];
}
void matrixRotationXmult(double angle, double rotMat[16]) {
	double temp1Mat[16];
	double temp2Mat[16];
	matrixRotationX(angle, temp1Mat);
	matrixMultiply_4x4(temp1Mat, rotMat, temp2Mat);
	copyMatrix(temp2Mat, rotMat);
}
void matrixRotationYmult(double angle, double rotMat[16]) {
	double temp1Mat[16];
	double temp2Mat[16];
	matrixRotationY(angle, temp1Mat);
	matrixMultiply_4x4(temp1Mat, rotMat, temp2Mat);
	copyMatrix(temp2Mat, rotMat);
}
void matrixRotationZmult(double angle, double rotMat[16]) {
	double temp1Mat[16];
	double temp2Mat[16];
	matrixRotationZ(angle, temp1Mat);
	matrixMultiply_4x4(temp1Mat, rotMat, temp2Mat);
	copyMatrix(temp2Mat, rotMat);
}

void matrixTranslateXYZ(double inputX, double inputY, double inputZ, double transMat[16]) {
	unitMatrix(transMat);
	transMat[12] = inputX;
	transMat[13] = inputY;
	transMat[14] = inputZ;
}

void matrixTranslateXYZmult(double inputX, double inputY, double inputZ, double transMat[16]) {
	double temp1Mat[16];
	double temp2Mat[16];
	matrixTranslateXYZ(inputX, inputY, inputZ, temp1Mat);
	matrixMultiply_4x4(temp1Mat, transMat, temp2Mat);
	copyMatrix(temp2Mat, transMat);
}

void matrixScaleXYZ(double inputX, double inputY, double inputZ, double scaleMat[16]) {
	unitMatrix(scaleMat);
	scaleMat[0] = inputX;
	scaleMat[5] = inputY;
	scaleMat[10] = inputZ;
}

struct POS3
{
	float x;
	float y;
	float z;
};

// ********************************* Three D component *********************************************

void rotationActionGeneral3D(Sheet* pg, int i) {
	pg->modalDialogComponent = i;
	buildRotation3dDialogButtons(pg, i);
}

int ThreeD_Component::method2(Sheet* pg) {
	pg->update3Dview = true;
	return false;
}
ThreeD_Component::~ThreeD_Component() {
	page->update3Dview = true;
}

void ThreeD_Component::inputsTo3dLoops(int matBit) {
	matPin = -1;
	loops;
	if ((int)setting[0] & matBit) {
		matPin = leftTerminalCount - 1;
		loops = inputsToLoopsN(matPin); // skip the array pin
		if (loops == 0)
			return;
	}
	else
	{
		loops = inputsToLoops();
		if (loops == 0)
			return;
	}

	unitMatrix(inMat);
	if ((int)setting[0] & matBit) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}
}

void ThreeD_Component::getColorGroup(int group, int* pinptr, int k) {
	inputRed = getLoopingValueColorSplit(group, pinptr, k);
	inputGreen = getLoopingValueColorSplit(group, pinptr, k);
	inputBlue = getLoopingValueColorSplit(group, pinptr, k);
}
void ThreeD_Component::getPositionGroup(int group, int* pinptr, int k) {
	inputX = getLoopingValueD(group, pinptr, k, 0);
	inputY = getLoopingValueD(group, pinptr, k, 0);
	inputZ = getLoopingValueD(group, pinptr, k, 0);
}
void ThreeD_Component::getRotationGroup(int group, int* pinptr, int k) {
	
	inputRotateX = getLoopingValueD(group, pinptr, k, 0);
	int rot = rotation & 7;
	if ((rot & 4) == 0)
		inputRotateX = -inputRotateX;
	if (rot & 3)
		inputRotateX -= PI_OVER_2 * (rot & 3);
		
	inputRotateY = getLoopingValueD(group, pinptr, k, 0);
	rot = (rotation >> 3) & 7;
	if ((rot & 4) == 0)
		inputRotateY = -inputRotateY;
	if (rot & 3) 
		inputRotateY -= PI_OVER_2 * (rot & 3);

	inputRotateZ = getLoopingValueD(group, pinptr, k, 0);
	rot = (rotation >> 6) & 7;
	if ((rot & 4) == 0)
		inputRotateZ = -inputRotateZ;
	if (rot & 3)
		inputRotateZ -= PI_OVER_2 * (rot & 3);
}
void ThreeD_Component::getSizeGroup(int group, int* pinptr, int k, double def) {
	inputSizeX = getLoopingValueD(group, pinptr, k, def);
	inputSizeY = getLoopingValueD(group, pinptr, k, def);
	inputSizeZ = getLoopingValueD(group, pinptr, k, def);
}
ThreeD_Component::ThreeD_Component(LPCTSTR _type, LPCTSTR _label, int shapePath, int LTC, int LTS, int RTC, int RTS)
	: Component(_type, _label, shapePath, LTC, LTS, RTC, RTS) {
}

void normalize3fv(float v[3])
{
	float d = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (d == 0.0)
		return;

	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void normCrossProduct(float v1[3], float v2[3], float out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];

	normalize3fv(out);
}


// ********************************* Torus *********************************************


void Torus_Component::new3dOjbect() {

	inputsTo3dLoops(32);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);
		
		double inputRingRadius = getLoopingValueD(group, &pin, k, 40);
		double inputTubeRadius = getLoopingValueD(group, &pin, k, 20);
		group++;
		if (inputRingRadius <= 0)
			continue;
		if (inputTubeRadius <= 0)
			continue;
		
		getColorGroup(group++, &pin, k);

		float inputAlpha = getLoopingValueD(group, &pin, k, 1); // doesn't seem to work
		if (inputAlpha <= 0)
			continue;

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX, outMat);
		matrixRotationYmult(inputRotateY, outMat);
		matrixRotationZmult(inputRotateZ, outMat);
		glLoadMatrixd(outMat);
			
	
		vector<vector<POS3>> floatPoly;
		double	iAng, rCosI, jAng;
		double ringRadX = inputRingRadius;
		double ringRadY = inputRingRadius;
		double tubeRadR = inputTubeRadius;
		double tubeRadZ = inputTubeRadius;
		
		int numi = setting[1];
		int numj = setting[2];
		float X = setting[3] * (inputTubeRadius + inputRingRadius);
		float Y = setting[4] * (inputTubeRadius + inputRingRadius);
		float Z = setting[5] * inputTubeRadius;


		/* create the points for a torus */
		floatPoly.resize(numi);
		for (int ii = 0; ii < numi; ii++)
		{
			floatPoly[ii].resize(numj);
			for (int jj = 0; jj < numj; jj++)
			{
				iAng = ii * TWO_PI / numi;
				rCosI = tubeRadR * cos(iAng);
				jAng = jj * TWO_PI / numj;

				floatPoly[ii][jj].x = X + (float)((ringRadX + (rCosI)) * cos(jAng));
				floatPoly[ii][jj].y = Y + (float)((ringRadY + (rCosI)) * sin(jAng));
				floatPoly[ii][jj].z = Z + (float)(tubeRadZ * sin(iAng));
			}
		}

		/* here we go */
		
		/* build the solid body */
		float v1[3], v2[3], normal[3];

		glBegin(GL_QUADS);
		for (int ii = 0; ii < numi; ii++)
		{
			int iiP1 = (ii + 1) % numi;

			for (int jj = 0; jj < numj; jj++)
			{
				int jjP1 = (jj + 1) % numj;

				/*		 2--------3
				*        |        |
				*        |  \     |
				*     V2 |   V1   |
				*        v     \  |
				*        1--------0
				*/

				/* v2 is 1 - 2 */
				v2[0] = floatPoly[iiP1][jjP1].x - floatPoly[iiP1][jj].x;
				v2[1] = floatPoly[iiP1][jjP1].y - floatPoly[iiP1][jj].y;
				v2[2] = floatPoly[iiP1][jjP1].z - floatPoly[iiP1][jj].z;

				/* v1 is 0 - 2 */
				v1[0] = floatPoly[ii][jj].x - floatPoly[iiP1][jj].x;
				v1[1] = floatPoly[ii][jj].y - floatPoly[iiP1][jj].y;
				v1[2] = floatPoly[ii][jj].z - floatPoly[iiP1][jj].z;

				normCrossProduct(v1, v2, normal);
				glNormal3fv(normal);

				/*		11-------01
				*        |        |
				*        |        |
				*        |        |
				*       10-------00
				*/

				glVertex3f(floatPoly[ii][jj].x,     floatPoly[ii][jj].y,     floatPoly[ii][jj].z);		//	00
				glVertex3f(floatPoly[iiP1][jj].x,   floatPoly[iiP1][jj].y,   floatPoly[iiP1][jj].z);	//	10
				glVertex3f(floatPoly[iiP1][jjP1].x, floatPoly[iiP1][jjP1].y, floatPoly[iiP1][jjP1].z);	//	11
				glVertex3f(floatPoly[ii][jjP1].x,   floatPoly[ii][jjP1].y,   floatPoly[ii][jjP1].z);	//	01
			}
		}
		glEnd();

		glPopMatrix();
	}
	
}

void Torus_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // sides

	if (doubleValues[1] > 2)
		setting[2] = doubleValues[1]; // rings

	setting[3] = doubleValues[2]; // x offset
	setting[4] = doubleValues[3]; // y offset
	setting[5] = doubleValues[4]; // z offset

	pg->update3Dview = true;
}
void doubleClickTorus(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Sides:", "Number of Rings:" ,"X Offset:", "Y Offset:", "Z Offset:"};
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2],
	pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4], pg->dgrm.cL[i]->setting[5] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 60,
		{}, {}, {}, 0, NULL);
}

bool Torus_Component::onLoad(Sheet* pg) {
	if (setting.size() < 6) {
		setSetting(3, 0); // x offset
		setSetting(4, 0); // y offset
		setSetting(5, 0); // z offset
	}
	buildPinLabels();
	return false;
}


std::vector<Button*>torusButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickTorus),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

vector<int> Torus_Component::GetPinGroups() { return vector<int>{3, 3, 2, 3, 1, 1 }; }
vector<string> Torus_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",
		"X Rotation", "Y Rotation", "Z Rotation",
		"Ring Radius", "Tube Radius",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}

Torus_Component::Torus_Component()
	: ThreeD_Component(L"3TORUS", L"3D Torus", squareShape, 10, 1, 0, 1)
{
	dialogButtonList = torusButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, (long)(1 + 4)); // pin selection
	setSetting(1, 20); // sides
	setSetting(2, 20); // rings
	setSetting(3, 0); // x offset
	setSetting(4, 0); // y offset
	setSetting(5, 0); // z offset
}


// ********************************* Sphere *********************************************


void Sphere_Component::new3dOjbect() {

	inputsTo3dLoops(32);

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);

		float inputRadius = getLoopingValueD(group, &pin, k, 50);
		group++;
		getColorGroup(group++, &pin, k);

		float inputAlpha = getLoopingValueD(group, &pin, k, 1); // doesn't seem to work
		if (inputAlpha <= 0)
			continue;

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		
		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX, outMat);
		matrixRotationYmult(inputRotateY, outMat);
		matrixRotationZmult(inputRotateZ, outMat);
		float shiftX = setting[3] * inputRadius;
		float shiftY = setting[4] * inputRadius;
		float shiftZ = setting[5] * inputRadius;
		matrixTranslateXYZmult(shiftX, shiftY, shiftZ, outMat);
		glLoadMatrixd(outMat);


		GLUquadricObj* quadratic = gluNewQuadric();
		gluSphere(quadratic, inputRadius, setting[1], setting[2]);

		glPopMatrix();
#if 0
		/* create the points on a sphere */
		vector<vector<POS3>> floatPoly;
		double X = inputX;
		double Y = inputY;
		double Z = inputZ;
		double Sx = inputRadius;
		double Sy = inputRadius;
		double Sz = inputRadius;

		floatPoly.resize(setting[1] + 2);
		for (int jLat = 0; jLat < setting[1] + 2; jLat++)
		{
			double latAngle = (double)jLat * PI / (setting[1] + 1) - PI_OVER_2;
			double latCosine = cos(latAngle);

			floatPoly[jLat].resize(setting[2]);
			for (int jLon = 0; jLon < setting[2]; jLon++)
			{
				double lonAngle = (double)jLon * TWO_PI / setting[2];

				floatPoly[jLat][jLon].x = (float)(X + Sx * latCosine * cos(lonAngle));
				floatPoly[jLat][jLon].y = (float)(Y + Sy * sin(latAngle));
				floatPoly[jLat][jLon].z = (float)(Z + Sz * latCosine * sin(lonAngle));
			}
		}

		/* here we go */

		glMatrixMode(GL_MODELVIEW);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		float v1[3], v2[3], normal[3];

		/* North pole cap */

		glBegin(GL_TRIANGLES);
		for (int jLon = 0; jLon < setting[2]; jLon++)
		{
			int jLonP1 = jLon + 1;
			if (jLonP1 == setting[2])
				jLonP1 = 0; /* wrap back to prime meridian */

			/* v2 is 1 - 2 */
			v2[0] = floatPoly[1][jLonP1].x - floatPoly[1][jLon].x;
			v2[1] = floatPoly[1][jLonP1].y - floatPoly[1][jLon].y;
			v2[2] = floatPoly[1][jLonP1].z - floatPoly[1][jLon].z;

			/* v1 is 0 - 2 */
			v1[0] = floatPoly[0][jLon].x - floatPoly[1][jLon].x;
			v1[1] = floatPoly[0][jLon].y - floatPoly[1][jLon].y;
			v1[2] = floatPoly[0][jLon].z - floatPoly[1][jLon].z;

			normCrossProduct(v2, v1, normal);
			glNormal3fv(normal);

			glVertex3f(floatPoly[0][jLon].x, floatPoly[0][jLon].y, floatPoly[0][jLon].z);
			glVertex3f(floatPoly[1][jLon].x, floatPoly[1][jLon].y, floatPoly[1][jLon].z);
			glVertex3f(floatPoly[1][jLonP1].x, floatPoly[1][jLonP1].y, floatPoly[1][jLonP1].z);
		}
		glEnd();


		/* middle latitudes */

		glBegin(GL_QUADS);
		for (int jLat = 1; jLat < setting[1]; jLat++)
		{
			int jLatP1 = jLat + 1;

			for (int jLon = 0; jLon < setting[2]; jLon++)
			{
				int jLonP1 = jLon + 1;
				if (jLonP1 == setting[2])
					jLonP1 = 0; /* wrap back to prime meridian */

				/* v2 is 1 - 2 */
				v2[0] = floatPoly[jLatP1][jLonP1].x - floatPoly[jLatP1][jLon].x;
				v2[1] = floatPoly[jLatP1][jLonP1].y - floatPoly[jLatP1][jLon].y;
				v2[2] = floatPoly[jLatP1][jLonP1].z - floatPoly[jLatP1][jLon].z;

				/* v1 is 0 - 2 */
				v1[0] = floatPoly[jLat][jLon].x - floatPoly[jLatP1][jLon].x;
				v1[1] = floatPoly[jLat][jLon].y - floatPoly[jLatP1][jLon].y;
				v1[2] = floatPoly[jLat][jLon].z - floatPoly[jLatP1][jLon].z;

				normCrossProduct(v2, v1, normal);
				glNormal3fv(normal);

				glVertex3f(floatPoly[jLat][jLon].x,     floatPoly[jLat][jLon].y,     floatPoly[jLat][jLon].z);
				glVertex3f(floatPoly[jLatP1][jLon].x,   floatPoly[jLatP1][jLon].y,   floatPoly[jLatP1][jLon].z );
				glVertex3f(floatPoly[jLatP1][jLonP1].x, floatPoly[jLatP1][jLonP1].y, floatPoly[jLatP1][jLonP1].z);
				glVertex3f(floatPoly[jLat][jLonP1].x,   floatPoly[jLat][jLonP1].y,   floatPoly[jLat][jLonP1].z );
			}
		}
		glEnd();


		/* south pole cap */

		glBegin(GL_TRIANGLES);
		int jLat = setting[1];
		int jLatP1 = jLat + 1;

		for (int jLon = 0; jLon < setting[2]; jLon++)
		{
			int jLonP1 = jLon + 1;
			if (jLonP1 == setting[2])
				jLonP1 = 0; /* wrap back to prime meridian */

			/* v2 is 1 - 2 */
			v2[0] = floatPoly[jLat][jLonP1].x - floatPoly[jLatP1][jLon].x;
			v2[1] = floatPoly[jLat][jLonP1].y - floatPoly[jLatP1][jLon].y;
			v2[2] = floatPoly[jLat][jLonP1].z - floatPoly[jLatP1][jLon].z;

			/* v1 is 0 - 2 */
			v1[0] = floatPoly[jLat][jLon].x - floatPoly[jLatP1][jLon].x;
			v1[1] = floatPoly[jLat][jLon].y - floatPoly[jLatP1][jLon].y;
			v1[2] = floatPoly[jLat][jLon].z - floatPoly[jLatP1][jLon].z;

			normCrossProduct(v2, v1, normal);
			glNormal3fv(normal);

			glVertex3f(floatPoly[jLat][jLon].x,   floatPoly[jLat][jLon].y,   floatPoly[jLat][jLon].z );
			glVertex3f(floatPoly[jLatP1][jLon].x, floatPoly[jLatP1][jLon].y, floatPoly[jLatP1][jLon].z );
			glVertex3f(floatPoly[jLat][jLonP1].x, floatPoly[jLat][jLonP1].y, floatPoly[jLat][jLonP1].z );
		}
		glEnd();
#endif
	}
	
}


void Sphere_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // sides

	if (doubleValues[1] > 2)
		setting[2] = doubleValues[1]; // rings

	setting[3] = doubleValues[2]; // x offset
	setting[4] = doubleValues[3]; // y offset
	setting[5] = doubleValues[4]; // z offset

	pg->update3Dview = true;
}
void doubleClickSphere(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Latitudes:", "Number of Longitudes:", 
		"X Offset:", "Y Offset:", "Z Offset:", };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2],
	pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4],pg->dgrm.cL[i]->setting[5]};

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 60,
		{}, {}, {}, 0, NULL);
}


bool Sphere_Component::onLoad(Sheet* pg) {
	if (setting.size() < 6) {
		setSetting(0, 3); // pin selection
		setSetting(1, 20); // latitudes
		setSetting(2, 20); // longitudes
		setSetting(3, 0); // x offset
		setSetting(4, 0); // y offset
		setSetting(5, 0); // z offset
	}
	buildPinLabels();
	return false;
}


std::vector<Button*>sphereButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickSphere) ,
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

vector<int> Sphere_Component::GetPinGroups() { return vector<int>{3, 3, 1, 3, 1, 1 }; }
vector<string> Sphere_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",
		"X Rotate", "Y Rotate", "Z Rotate",
		"Radius",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}

Sphere_Component::Sphere_Component()
	: ThreeD_Component(L"3SPHERE", L"3D Sphere", squareShape, 9, 1, 0, 1)
{
	dialogButtonList = sphereButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, (long)(1 + 4)); // pin selection
	setSetting(1, 20); // latitudes
	setSetting(2, 20); // longitudes
	setSetting(3, 0); // x offset
	setSetting(4, 0); // y offset
	setSetting(5, 0); // z offset
}


// ********************************* Cone *********************************************

void Cone_Component::new3dOjbect() {
	
	inputsTo3dLoops(32);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);

		float inputRadius = getLoopingValueD(group, &pin, k, 50);
		float inputHeight = getLoopingValueD(group, &pin, k, 100);
		if (inputRadius <= 0)
			continue;
		if (inputHeight <= 0)
			continue;
		group++;
		getColorGroup(group++, &pin, k);
		float inputAlpha = getLoopingValueD(group, &pin, k, 1);
		if (inputAlpha <= 0)
			continue;
		

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		
		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX + PI_OVER_2, outMat);
		matrixRotationYmult(inputRotateZ, outMat);
		matrixRotationZmult(inputRotateY, outMat);
		float shiftX = setting[3] * inputRadius;
		float shiftY = setting[5] * inputRadius;
		float shiftZ = (-setting[4] - 1) * inputHeight/2;
		matrixTranslateXYZmult(shiftX, shiftY, shiftZ, outMat);
		glLoadMatrixd(outMat);
		
		GLUquadricObj* quadratic1 = gluNewQuadric();
		gluCylinder(quadratic1, 0, inputRadius, inputHeight, setting[1], setting[2]);
		
		// bottom cover
		if (inputRadius > 0) {
			GLUquadricObj* quadratic3 = gluNewQuadric();
			glTranslatef(0, 0, inputHeight);
			gluDisk(quadratic3, 0, inputRadius, setting[1], 4);
		}

		glPopMatrix();
	}
}



void Cone_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // slices

	if (doubleValues[1] >= 2)
		setting[2] = doubleValues[1]; // stacks

	setting[3] = doubleValues[2]; // x offset
	setting[4] = doubleValues[3]; // y offset
	setting[5] = doubleValues[4]; // z offset

	pg->update3Dview = true;
}
void doubleClickCone(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Slices:", "Number of Stacks:", "X Offset:", "Y Offset:", "Z Offset:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2],
	pg->dgrm.cL[i]->setting[3], pg->dgrm.cL[i]->setting[4], pg->dgrm.cL[i]->setting[5] };

	
	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 60,
		{}, {}, {}, 0, NULL);
}
bool Cone_Component::onLoad(Sheet* pg) {
	if (setting.size() < 6) {
		setSetting(3, 0); // x offset
		setSetting(4, 0); // y offset
		setSetting(5, 0); // z offset
	}
	buildPinLabels();
	return false;
}


std::vector<Button*>coneButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickCone),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

vector<int> Cone_Component::GetPinGroups() { return vector<int>{3, 3, 2, 3, 1, 1}; }
vector<string> Cone_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",
		"X Rotate", "Y Rotate", "Z Rotate",
		"Radius", "Height",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]"  };
}

Cone_Component::Cone_Component()
	: ThreeD_Component(L"3CONE", L"3D Cone", squareShape, 10, 1, 0, 1)
{
	dialogButtonList = coneButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, 1L +4); // pin selection
	setSetting(1, 20); // slices
	setSetting(2, 20); // stacks
	setSetting(3, 0); // x offset
	setSetting(4, 0); // y offset
	setSetting(5, 0); // z offset
}


// ********************************* Cylinder *********************************************



void Cylinder_Component::new3dOjbect() {

	inputsTo3dLoops(32);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;
		
		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);

		float inputTopRadius = getLoopingValueD(group, &pin, k, 50);
		float inputBottomRadius = getLoopingValueD(group, &pin, k, 50);
		float inputHeight = getLoopingValueD(group, &pin, k, 100);
		if ((inputTopRadius <= 0) && (inputBottomRadius <= 0))
			continue;
		if (inputHeight <= 0)
			continue;
		group++;
		getColorGroup(group++, &pin, k);
		float inputAlpha = getLoopingValueD(group, &pin, k, 1);
		if (inputAlpha <= 0)
			continue;
	

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX + PI_OVER_2, outMat);
		matrixRotationYmult(inputRotateZ, outMat);
		matrixRotationZmult(inputRotateY, outMat);
		float shiftX = setting[4] * ((long)inputTopRadius + inputBottomRadius) / 2;
		float shiftY = setting[6] * ((long)inputTopRadius + inputBottomRadius) / 2;
		float shiftZ = (-setting[5]-1) * inputHeight/2;
		matrixTranslateXYZmult(shiftX, shiftY, shiftZ, outMat);
		glLoadMatrixd(outMat);
			

		GLUquadricObj *quadratic1 = gluNewQuadric();
		gluCylinder(quadratic1, inputTopRadius, inputBottomRadius, inputHeight, setting[1], setting[2]);
		if ((int)setting[3] & 1) {
			// top cover
			if (inputTopRadius > 0) {
				GLUquadricObj *quadratic2 = gluNewQuadric();
				gluDisk(quadratic2, 0, inputTopRadius, setting[1], 4);
			}
		}

		if ((int)setting[3] & 2) {
			// bottom cover
			if (inputBottomRadius > 0) {
				GLUquadricObj *quadratic3 = gluNewQuadric();
				glTranslatef(0, 0, inputHeight);
				gluDisk(quadratic3, 0, inputBottomRadius, setting[1], 4);
			}
		}
		glPopMatrix();
	}
}

void Cylinder_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // sides

	if (doubleValues[1] > 2)
		setting[2] = doubleValues[1]; // rings

	setting[4] = doubleValues[2]; // x offset
	setting[5] = doubleValues[3]; // y offset
	setting[6] = doubleValues[4]; // z offset

	setting[3] = (boolValues[0] ? 1L : 0) + (boolValues[1] ? 2 : 0);

	pg->update3Dview = true;
}
void doubleClickCylinder(Sheet* pg, int i) {
	pg->modalDialogComponent = i; 

	vector<string> doubleLabels = { "Number of Slices:", "Number of Stacks:", "X Offset:", "Y Offset:", "Z Offset:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2],
	pg->dgrm.cL[i]->setting[4], pg->dgrm.cL[i]->setting[5], pg->dgrm.cL[i]->setting[6] };
	
	vector<string> checkLabels = { "Close the Top", "Close the Bottom" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[3] & 1), (bool)((int)pg->dgrm.cL[i]->setting[3] & 2) };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, {}, 0, NULL);
}

bool Cylinder_Component::onLoad(Sheet* pg) {
	if (setting.size() < 7) {
		setSetting(4, 0); // x offset
		setSetting(5, 0); // y offset
		setSetting(6, 0); // z offset
	}
	buildPinLabels();
	return false;
}


std::vector<Button*>cylinderButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickCylinder) ,
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

vector<int> Cylinder_Component::GetPinGroups() { return vector<int>{3, 3, 3, 3, 1, 1}; }
vector<string> Cylinder_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",
		"X Rotation", "Y Position", "Z Position",
		"Top Radius", "Bottom Radius", "Height",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}

Cylinder_Component::Cylinder_Component()
	: ThreeD_Component(L"3CYLIND", L"3D Cylinder", squareShape, 10, 1, 0, 1)
{
	dialogButtonList = cylinderButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, 1L +4); // pin selection
	setSetting(1, 20); // slices
	setSetting(2, 20); // stacks
	setSetting(3, 3); // 1 for top cover, 2 for bottom cover, 3 for both
	setSetting(4, 0); // x offset
	setSetting(5, 0); // y offset
	setSetting(6, 0); // z offset
}


// ********************************* Tetrahedoron *********************************************

#if 0

void Tetra_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		// X position", "Y position", "Z position"
		float inputX = getLoopingValueD(0, &pin, k, 0);
		float inputY = getLoopingValueD(0, &pin, k, 0);
		float inputZ = getLoopingValueD(0, &pin, k, 0);

		double inputSize = 1.0;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 1); // pin 3
			if (inputSize <= 0)
				continue;
		}

		//"Red", "Green", "Blue", 
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		double inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work
	
		glPushMatrix();
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		glColor3d(inputRed, inputGreen, inputBlue);

		double transMat[16];
		matrixTranslateXYZ(inputX, inputY, inputZ, transMat); // glTranslatef(inputX, inputY, inputZ);
		double outMat[16];
		matrixMultiply_4x4(transMat, inMat, outMat);
		double scaleMat[16];
		matrixScaleXYZ(inputSize, inputSize, inputSize, scaleMat); // glScaled(inputSize, inputSize, inputSize);
		double outMat2[16];
		matrixMultiply_4x4(scaleMat, outMat, outMat2);
		glLoadMatrixd(outMat2);

		glutSolidTetrahedron();
		glPopMatrix();
	}
}

bool Tetra_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}



std::vector<Button*>tetraButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

vector<int> Tetra_Component::GetPinGroups() { return vector<int>{3, 1, 3, 1, 1 }; }
vector<string> Tetra_Component::GetPinLabels() {
	return vector<string>{
		"X position", "Y position", "Z position",
		"size",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}

Tetra_Component::Tetra_Component()
	: ThreeD_Component(L"3Tetra", L"3D Tetrahedron", squareShape, 8, 1, 0, 1)
{
	dialogButtonList = tetraButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, 3); // pin selection
}
#endif

// ********************************* Octahedoron *********************************************

#if 0
void Octahedron_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		// X position", L"Y position", "Z position"
		float inputX = getLoopingValueD(0, &pin, k, 0);
		float inputY = getLoopingValueD(0, &pin, k, 0);
		float inputZ = getLoopingValueD(0, &pin, k, 0);

		double inputSize = 1.0;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 1); // pin 3
			if (inputSize <= 0)
				continue;
		}

		//L"Red", L"Green", L"Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work

		glPushMatrix();
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		glColor3d(inputRed, inputGreen, inputBlue);

		double transMat[16];
		matrixTranslateXYZ(inputX, inputY, inputZ, transMat); // glTranslatef(inputX, inputY, inputZ);
		double outMat[16];
		matrixMultiply_4x4(transMat, inMat, outMat);
		double scaleMat[16];
		matrixScaleXYZ(inputSize, inputSize, inputSize, scaleMat); // glScaled(inputSize, inputSize, inputSize);
		double outMat2[16];
		matrixMultiply_4x4(scaleMat, outMat, outMat2);
		glLoadMatrixd(outMat2);

		glutSolidOctahedron();
		glPopMatrix();
	}
}

bool Octahedron_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}


std::vector<Button*>octaButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

vector<int> Octahedron_Component::GetPinGroups() { return vector<int>{3, 1, 3, 1, 1 }; }
vector<string> Octahedron_Component::GetPinLabels() {
	return vector<string>{
		"X position", "Y position", "Z position",
		"size",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}

Octahedron_Component::Octahedron_Component()
	: ThreeD_Component(L"3Octa", L"3D Octahedron", squareShape, 8, 1, 0, 1)
{
	dialogButtonList = octaButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, 3); // pin selection
}
#endif

// ********************************* Dodecahedron *********************************************

#if 0

void Dodecahedron_Component::new3dOjbect() {
	
	inputsTo3dLoops(16);

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		// X position", "Y position", "Z position"
		float inputX = getLoopingValueD(0, &pin, k, 0);
		float inputY = getLoopingValueD(0, &pin, k, 0);
		float inputZ = getLoopingValueD(0, &pin, k, 0);

		double inputSize = 1.0;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 1); // pin 3
			if (inputSize <= 0)
				continue;
		}

		//"Red", "Green", "Blue", 
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work

		glPushMatrix();
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		glColor3d(inputRed, inputGreen, inputBlue);

		double transMat[16];
		matrixTranslateXYZ(inputX, inputY, inputZ, transMat); // glTranslatef(inputX, inputY, inputZ);
		double outMat[16];
		matrixMultiply_4x4(transMat, inMat, outMat);
		double scaleMat[16];
		matrixScaleXYZ(inputSize, inputSize, inputSize, scaleMat); // glScaled(inputSize, inputSize, inputSize);
		double outMat2[16];
		matrixMultiply_4x4(scaleMat, outMat, outMat2);
		glLoadMatrixd(outMat2);

		glutSolidDodecahedron();
		glPopMatrix();
	}
}

bool Dodecahedron_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}


std::vector<Button*>dodecaButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

vector<int> Dodecahedron_Component::GetPinGroups() { return vector<int>{3, 1, 3, 1, 1}; }
vector<string> Dodecahedron_Component::GetPinLabels() {
	return vector<string>{
		"X position", "Y position", "Z position",
		"size",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]"  };
}
Dodecahedron_Component::Dodecahedron_Component()
	: ThreeD_Component(L"3Dodeca", L"3D Dodecahedron", squareShape, 8, 1, 0, 1)
{
	dialogButtonList = dodecaButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, 3); // pin selection
}
#endif


// ********************************* Icosahedoron *********************************************

#if 0

void Icosahedron_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		// X position", "Y position", "Z position"
		float inputX = getLoopingValueD(0, &pin, k, 0);
		float inputY = getLoopingValueD(0, &pin, k, 0);
		float inputZ = getLoopingValueD(0, &pin, k, 0);

		double inputSize = 1.0;
		if ((int)setting[0] & 2) {
			inputSize = getLoopingValueD(pin++, k, 1); // pin 3
			if (inputSize <= 0)
				continue;
		}

		//"Red", "Green", "Blue", 
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		double inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work

		glPushMatrix();
		//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		glColor3d(inputRed, inputGreen, inputBlue);

		double transMat[16];
		matrixTranslateXYZ(inputX, inputY, inputZ, transMat); // glTranslatef(inputX, inputY, inputZ);
		double outMat[16];
		matrixMultiply_4x4(transMat, inMat, outMat);
		double scaleMat[16];
		matrixScaleXYZ(inputSize, inputSize, inputSize, scaleMat); // glScaled(inputSize, inputSize, inputSize);
		double outMat2[16];
		matrixMultiply_4x4(scaleMat, outMat, outMat2);
		glLoadMatrixd(outMat2);

		glutSolidIcosahedron();
		glPopMatrix();
	}
}

bool Icosahedron_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}


std::vector<Button*>icosaButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

vector<int> Icosahedron_Component::GetPinGroups() { return vector<int>{ 3, 1, 3, 1, 1 }; }
vector<string> Icosahedron_Component::GetPinLabels() {
	return vector<string>{
		"X position", "Y position", "Z position",
		"size",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}
Icosahedron_Component::Icosahedron_Component()
	: ThreeD_Component(L"3Icosa", L"3D Icosahedron", squareShape, 8, 1, 0, 1)
{
	dialogButtonList = icosaButtonList;
	pinMaskIndex = 0;
	width = 40;
	setSetting(0, 3); // pin selection
}
#endif


// ********************************* Cube *********************************************


void Cube3D_Component::new3dOjbect() {
	
	inputsTo3dLoops(32);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);
		getSizeGroup(group++, &pin, k, 100);
		inputSizeX /= 2;
		inputSizeY /= 2;
		inputSizeZ /= 2;
		getColorGroup(group++, &pin, k);
		float inputAlpha = getLoopingValueD(group, &pin, k, 1); // doesn't seem to work
		if (inputAlpha <= 0)
			continue;

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX, outMat);
		matrixRotationYmult(inputRotateY, outMat);
		matrixRotationZmult(inputRotateZ, outMat);
		glLoadMatrixd(outMat);

		/* here we go */
		float offsetX = setting[1];
		float offsetY = setting[2];
		float offsetZ = setting[3];

		float posXplus = offsetX * inputSizeX + inputSizeX;
		float posYplus = offsetY * inputSizeY + inputSizeY;
		float posZplus = offsetZ * inputSizeZ + inputSizeZ;
		float posXminus = offsetX * inputSizeX - inputSizeX;
		float posYminus = offsetY * inputSizeY - inputSizeY;
		float posZminus = offsetZ * inputSizeZ - inputSizeZ;

		glBegin(GL_QUADS);

		glNormal3f(0.0F, 0.0F, 1.0F);		// front
		glVertex3d(posXplus,  posYplus, posZplus);
		glVertex3d(posXminus, posYplus, posZplus);
		glVertex3d(posXminus, posYminus, posZplus);
		glVertex3d(posXplus,  posYminus, posZplus);

		glNormal3f(0.0F, 0.0F, -1.0F);		// back
		glVertex3d(posXminus, posYminus, posZminus);
		glVertex3d(posXminus, posYplus,  posZminus);
		glVertex3d(posXplus,  posYplus,  posZminus);
		glVertex3d(posXplus,  posYminus, posZminus);

		glNormal3f(0.0F, 1.0F, 0.0F);		// top
		glVertex3d(posXplus,  posYplus, posZplus);
		glVertex3d(posXplus,  posYplus, posZminus);
		glVertex3d(posXminus, posYplus, posZminus);
		glVertex3d(posXminus, posYplus, posZplus);

		glNormal3f(0.0F, -1.0F, 0.0F);		// bottom
		glVertex3d(posXminus, posYminus, posZminus);
		glVertex3d(posXplus,  posYminus, posZminus);
		glVertex3d(posXplus,  posYminus, posZplus);
		glVertex3d(posXminus, posYminus, posZplus);

		glNormal3f(1.0F, 0.0F, 0.0F);		// right
		glVertex3d(posXplus, posYplus,  posZplus);
		glVertex3d(posXplus, posYminus, posZplus);
		glVertex3d(posXplus, posYminus, posZminus);
		glVertex3d(posXplus, posYplus,  posZminus);

		glNormal3f(-1.0F, 0.0F, 0.0F);		// left
		glVertex3d(posXminus, posYminus, posZminus);
		glVertex3d(posXminus, posYminus, posZplus);
		glVertex3d(posXminus, posYplus,  posZplus);
		glVertex3d(posXminus, posYplus,  posZminus);

		glEnd();

		glPopMatrix();
	}
}

bool Cube3D_Component::onLoad(Sheet* pg) {
	if (setting.size() < 4) {
		setSetting(1, 0); // x offset
		setSetting(2, 0); // y offset
		setSetting(3, 0); // z offset
	}
	buildPinLabels();
	return false;
}
void Cube3D_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	
	setting[1] = doubleValues[0]; 
	setting[2] = doubleValues[1];
	setting[3] = doubleValues[2];
	pg->update3Dview = true;

}
void doubleClickCube(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "X Offset:","Y Offset:","Z Offset:", };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1], pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		{}, {}, {}, 0,
		NULL);
}

vector<int> Cube3D_Component::GetPinGroups() { return vector<int>{ 3, 3, 3, 3, 1, 1 }; }
vector<string> Cube3D_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",
		"X Rotate", "Y Rotate", "Z Rotate",
		"X Size", "Y Size", "Z Size",
		"Red", "Green", "Blue",
		"Alpha",
		"[4x4]" };
}
std::vector<Button*>cubeButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) ,
	new Button(3, 60, 85, 20, L"Options...", doubleClickCube),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

Cube3D_Component::Cube3D_Component()
	: ThreeD_Component(L"3CUBE", L"3D Cube", squareShape, 11, 1, 0, 1)
{
	dialogButtonList = cubeButtonList;
	width = 40;
	setSetting(0, 1L + 4); // pin selection
	setSetting(1, 0); // x offset
	setSetting(2, 0); // y offset
	setSetting(3, 0); // z offset
	pinMaskIndex = 0;
}



// ******************************************** Rotate ***********************




int Rotate3D_Component::method2(Sheet* pg) {
	if (!checkValueInputs())
		return false;
	// be sure the input axis is a unit vector
	double ux = input[0].value[0];
	double uy = input[1].value[0];
	double uz = input[2].value[0];

	double length = sqrt(ux * ux + uy * uy + uz * uz);
	if (length <= 0)
		return false;

	ux /= length;
	uy /= length;
	uz /= length;

	double angle = input[3].value[0];
	if (angle == 0)
		return false;

	double C = cos(angle);
	double S = sin(angle);
	double t = 1.0 - C;

	double rotMat[16];
	rotMat[0] = t * ux * ux + C;
	rotMat[1] = t * ux * uy - S * uz;
	rotMat[2] = t * ux * uz + S * uy;
	rotMat[3] = 0;

	rotMat[4] = t * ux * uy + S * uz;
	rotMat[5] = t * uy * uy + C;
	rotMat[6] = t * uy * uz - S * ux;
	rotMat[7] = 0;

	rotMat[8] = t * ux * uz - S * uy;
	rotMat[9] = t * uy * uz + S * ux;
	rotMat[10] = t * uz * uz + C;
	rotMat[11] = 0;

	rotMat[12] = 0;
	rotMat[13] = 0;
	rotMat[14] = 0;
	rotMat[15] = 1;

	output[0].value.resize(16);

	if ((input[4].drivenByComponent == -1) ||
		(input[4].value.size() != 16)) {
		// no matrix input, just output the rotation
		for (int k = 0; k < 16; k++)
			output[0].value[k] = rotMat[k];
	}
	else
	{
		double inputMat[16];
		for (int k = 0; k < 16; k++)
			inputMat[k] = input[4].value[k];

		double outputMat[16];
		matrixMultiply_4x4(inputMat, rotMat, outputMat);

		for (int k = 0; k < 16; k++)
			output[0].value[k] = outputMat[k];
	}

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = 4;
	output[0].dimensions[1] = 4;

	return true;
}

Rotate3D_Component::Rotate3D_Component()
	: Component(L"3XYZROT", L"3D Rotate", squareShape, 5, 1, 1, 1)
{
	setLeftLabels(3, "Axis X", "Axis Y", "Axis Z", "Angle on Axis", "[4x4]");
	setRightLabels(3, "[4x4]");
	width = 40;
}

// ********************** X

int RotateX_Component::method2(Sheet* pg) {

	if (input[0].value.size() == 0)
		return clearOutputs();

	double rotMat[16];
	matrixRotationX(input[0].value[0], rotMat);
	output[0].value.resize(16);

	if ((input[1].drivenByComponent == -1) ||
		(input[1].value.size() != 16) )       {
		// no matrix input, just output the rotation
		for (int k = 0; k < 16; k++)
			output[0].value[k] = rotMat[k];
	}
	else
	{
		double inputMat[16];
		for (int k = 0; k < 16; k++)
			inputMat[k] = input[1].value[k];

		double outputMat[16];
		matrixMultiply_4x4(rotMat, inputMat, outputMat);
		for (int k = 0; k < 16; k++)
			output[0].value[k] = outputMat[k];
	}

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = 4;
	output[0].dimensions[1] = 4;

	return true; 
}

RotateX_Component::RotateX_Component()
	: Component(L"3ROTX", L"3D X Rotate", 2, 1)
{
	setLeftLabels(3, "Angle on X", "[4x4]");
	setRightLabels(3, "[4x4]");
	width = 45;
}

// ****************************** Y

int RotateY_Component::method2(Sheet* pg) {
	if (input[0].value.size() == 0)
		return clearOutputs();

	double rotMat[16];
	matrixRotationY(input[0].value[0], rotMat);
	output[0].value.resize(16);

	if ((input[1].drivenByComponent == -1) ||
		(input[1].value.size() != 16)) {
		// no matrix input, just output the rotation
		for (int k = 0; k < 16; k++)
			output[0].value[k] = rotMat[k];
	}
	else
	{
		double inputMat[16];
		for (int k = 0; k < 16; k++)
			inputMat[k] = input[1].value[k];

		double outputMat[16];
		matrixMultiply_4x4(rotMat, inputMat, outputMat);
		for (int k = 0; k < 16; k++)
			output[0].value[k] = outputMat[k];
	}

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = 4;
	output[0].dimensions[1] = 4;

	return true;
}

RotateY_Component::RotateY_Component()
	: Component(L"3ROTY", L"3D Y Rotate", 2, 1)
{
	setLeftLabels(3, "Angle on Y", "[4x4]");
	setRightLabels(3, "[4x4]");
	width = 45;
}

// ************************ Z

int RotateZ_Component::method2(Sheet* pg) {
	if (input[0].value.size() == 0)
		return clearOutputs();

	double rotMat[16];
	matrixRotationZ(input[0].value[0], rotMat);
	output[0].value.resize(16);

	if ((input[1].drivenByComponent == -1) ||
		(input[1].value.size() != 16)) {
		// no matrix input, just output the rotation
		for (int k = 0; k < 16; k++)
			output[0].value[k] = rotMat[k];
	}
	else
	{
		double inputMat[16];
		for (int k = 0; k < 16; k++)
			inputMat[k] = input[1].value[k];

		double outputMat[16];
		matrixMultiply_4x4(rotMat, inputMat, outputMat);
		for (int k = 0; k < 16; k++)
			output[0].value[k] = outputMat[k];
	}

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = 4;
	output[0].dimensions[1] = 4;

	return true;
}

RotateZ_Component::RotateZ_Component()
	: Component(L"3ROTZ", L"3D Z Rotate", 2, 1)
{
	setLeftLabels(3, "Angle on Z", "[4x4]");
	setRightLabels(3, "[4x4]");
	width = 45;
}

// ***************************** Scale *****************************

int Scale3D_Component::method2(Sheet* pg) {
	double inputX = getLoopingValueD(0, 0, 1);
	double inputY = getLoopingValueD(1, 0, 1);
	double inputZ = getLoopingValueD(2, 0, 1);

	double scaleMat[16];
	matrixScaleXYZ(inputX, inputY, inputZ, scaleMat);
	output[0].value.resize(16);

	if ((input[3].drivenByComponent == -1) ||
		(input[3].value.size() != 16)) {
		// no matrix input, just output the translation
		for (int k = 0; k < 16; k++)
			output[0].value[k] = scaleMat[k];
	}
	else
	{
		double inputMat[16];
		for (int k = 0; k < 16; k++)
			inputMat[k] = input[3].value[k];

		double outputMat[16];
		matrixMultiply_4x4(scaleMat, inputMat, outputMat);

		for (int k = 0; k < 16; k++)
			output[0].value[k] = outputMat[k];
	}

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = 4;
	output[0].dimensions[1] = 4;

	return true;
}

Scale3D_Component::Scale3D_Component()
	: Component(L"3XYZSCALE", L"3D Scale", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialog3XYZSHIFTButtonList; // shift and scale are the same
	setLeftLabels(3, "X", "Y", "Z", "[4x4]");
	setRightLabels(3, "[4x4]");
}

// *************************** Shift *********************************

int Shift3D_Component::method2(Sheet* pg) {
	
	double inputX = getLoopingValueD(0, 0, 0); 
	double inputY = getLoopingValueD(1, 0, 0); 
	double inputZ = getLoopingValueD(2, 0, 0);
	
	double transMat[16];
	matrixTranslateXYZ(inputX, inputY, inputZ, transMat);
	output[0].value.resize(16);

	if ((input[3].drivenByComponent == -1) ||
		(input[3].value.size() != 16)) {
		// no matrix input, just output the translation
		for (int k = 0; k < 16; k++)
			output[0].value[k] = transMat[k];
	}
	else
	{
		double inputMat[16];
		for (int k = 0; k < 16; k++)
			inputMat[k] = input[3].value[k];

		double outputMat[16];
		matrixMultiply_4x4(transMat, inputMat, outputMat);
		for (int k = 0; k < 16; k++)
			output[0].value[k] = outputMat[k];
	}

	output[0].dimensions.resize(2);
	output[0].dimensions[0] = 4;
	output[0].dimensions[1] = 4;

	return true;
}

Shift3D_Component::Shift3D_Component()
	: Component(L"3XYZSHIFT", L"3D Shift", squareShape, 4, 1, 1, 1)
{
	//dialogList = dialog3XYZSHIFTButtonList;// shift and scale are the same
	setLeftLabels(3, "X", "Y", "Z", "[4x4]");
	setRightLabels(3, "[4x4]");
}

// ****************************** Line Segment ********************************************



void Lines3D_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double posX0 = getLoopingValueD(pin++, k, 0);
		double posY0 = getLoopingValueD(pin++, k, 0);
		double posZ0 = getLoopingValueD(pin++, k, 0);

		double posX1 = getLoopingValueD(pin++, k, 0);
		double posY1 = getLoopingValueD(pin++, k, 0);
		double posZ1 = getLoopingValueD(pin++, k, 0);

		float inputLineWidth = 1.0;
		if ((int)setting[0] & 2) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}

		if (first) {
			glPushMatrix();
			glLoadMatrixd(inMat);
			glLineWidth(inputLineWidth);
			float Surface[4];
			Surface[0] = inputRed / 255;
			Surface[1] = inputGreen / 255;
			Surface[2] = inputBlue / 255;
			Surface[3] = inputAlpha;
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
			first = false;
		}
		glBegin(GL_LINE_STRIP);
		glVertex3f(posX0, posY0, posZ0);
		glVertex3f(posX1, posY1, posZ1);
		glEnd();
	}
	glPopMatrix();
}

std::vector<Button*>the3dLinesButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool Lines3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Lines3D_Component::GetPinGroups() { return vector<int>{ 3, 3, 1, 3, 1, 1 }; }
vector<string> Lines3D_Component::GetPinLabels() {
	return vector<string>{
		"X0", "Y0", "Z0",		// 0=Position  (1)
		"X1", "Y1", "Z1",		// 1=Position  (2)
		"Line Width",			// 2=Width     (4)
		"Red", "Green", "Blue",	// 3=color     (8)
		"Alpha",				// 4=alpha     (16)
		"[4x4]" };
}
Lines3D_Component::Lines3D_Component()
	: ThreeD_Component(L"3Lines", L"3D Line Segment", squareShape, 12, 1, 0, 0)
{
	dialogButtonList = the3dLinesButtonList;
	width = 40;
	setSetting(0, 7);
	pinMaskIndex = 0;
}

// *************************************************** Poly Line ***********************************************


void PolyLine3D_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;
	
		double posX = getLoopingValueD(pin++, k, 0);
		double posY = getLoopingValueD(pin++, k, 0);
		double posZ = getLoopingValueD(pin++, k, 0);

		float inputLineWidth = 1.0;
		if ((int)setting[0] & 2) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}
		if (first)
			glDisable(GL_LIGHTING);

		glColor3d(inputRed/255, inputGreen / 255, inputBlue / 255);

		if (first) {
			glPushMatrix();
			glLoadMatrixd(inMat);
			glLineWidth(inputLineWidth);
			
			glBegin(GL_LINE_STRIP);
			first = false;
		}
		
		glVertex3f(posX, posY, posZ);
	}
	if (!first) {
		glEnd();
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
		
}
std::vector<Button*>the3dPolylineButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool PolyLine3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> PolyLine3D_Component::GetPinGroups() { return vector<int>{ 3, 1, 3, 1, 1 }; }
vector<string> PolyLine3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",			// 0=Position  (1)
		"Line Width",			// 1=Width     (2)
		"Red", "Green", "Blue",	// 2=color     (4)
		"Alpha",				// 3=alpha     (8)
		"[4x4]" };
}
PolyLine3D_Component::PolyLine3D_Component()
	: ThreeD_Component(L"3Line", L"3D Poly Line", squareShape, 9, 1, 0, 0)
{
	//setLeftLabels(3, "X", "Y", "Z", "Width", "Red", "Green", "Blue", "Alpha", "[4x4]");
	dialogButtonList = the3dPolylineButtonList;
	width = 40;
	setSetting(0, 3);
	pinMaskIndex = 0;
}



// ******************************** Polygon *************************************

void Polygon3D_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double posX = 0;
		double posY = 0;
		double posZ = 0;
		if ((int)setting[0] & 1) {
			posX = getLoopingValueD(pin++, k, 0);
			posY = getLoopingValueD(pin++, k, 0);
			posZ = getLoopingValueD(pin++, k, 0);
		}

		float inputLineWidth = 1.0;
		if ((int)setting[0] & 2) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}

		if (first) {
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glLoadMatrixd(inMat);
			glLineWidth(inputLineWidth);
			glColor3d(inputRed / 255, inputGreen / 255, inputBlue / 255);
			glBegin(GL_LINE_LOOP);
			first = false;
		}

		glVertex3d(posX, posY, posZ);
	}
	glEnd();
	if (!first) {
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}

std::vector<Button*>the3dPolygonButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool Polygon3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> Polygon3D_Component::GetPinGroups() { return vector<int>{3, 1, 3, 1, 1}; }
vector<string> Polygon3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",				// 0=Position   (1)
		"Line Width",				// 1=width		(2)
		"Red", "Green", "Blue",		// 2=color      (4)
		"Alpha",					// 3=alpha      (8)
		"[4x4]"  };
}

Polygon3D_Component::Polygon3D_Component()
	: ThreeD_Component(L"3Polygon", L"3D Polygon", squareShape, 9, 1, 0, 0)
{
	dialogButtonList = the3dPolygonButtonList;
	width = 40;
	setSetting(0, 3);
	pinMaskIndex = 0;
}

// ***************************************** circle *******************************


void Circle3D_Component::new3dOjbect() {

	inputsTo3dLoops(32);

	glDisable(GL_LIGHTING);
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double posX = 0;
		double posY = 0;
		double posZ = 0;
		if ((int)setting[0] & 1) {
			posX = getLoopingValueD(pin++, k, 0);
			posY = getLoopingValueD(pin++, k, 0);
			posZ = getLoopingValueD(pin++, k, 0);
		}

		double inputWidth = 100;
		double inputHeight = 100;
		if ((int)setting[0] & 2) {
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 100);
			if (inputWidth <= 0)
				continue;
			if (inputHeight <= 0)
				continue;
		}

		double inputLineWidth = 1.0;
		if ((int)setting[0] & 4) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 8) {
			inputRed = getLoopingValueD(pin++, k, 0);// pin 4
			inputGreen = getLoopingValueD(pin++, k, 0);// pin 5
			inputBlue = getLoopingValueD(pin++, k, 0);// pin 6
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 16) {
			inputAlpha = getLoopingValueD(pin++, k, 1);// pin 7
			if (inputAlpha <= 0)
				continue;
		}


		glPushMatrix();
		glLoadMatrixd(inMat);
			
		double radius = sqrt(inputLineWidth*inputLineWidth + inputHeight * inputHeight);
		double angleStep = TWO_PI / setting[1];

		glLineWidth(inputLineWidth);
		glColor3d(inputRed / 255, inputGreen / 255, inputBlue / 255);

		glBegin(GL_LINE_LOOP);
		switch ((int)setting[2]) {
		case 0:
			for (double angle = 0; angle < TWO_PI; angle += angleStep) {
				double z = inputWidth * sin(angle);
				double y = inputHeight * cos(angle);
				glVertex3f(posX, posY + y, posZ + z);
			}
			break;
		case 1:
			for (double angle = 0; angle < TWO_PI; angle += angleStep) {
				double x = inputWidth * sin(angle);
				double z = inputHeight * cos(angle);
				glVertex3f(posX + x, posY, posZ + z);
			}
			break;
		case 2:
			for (double angle = 0; angle < TWO_PI; angle += angleStep) {
				double x = inputWidth * sin(angle);
				double y = inputHeight * cos(angle);
				glVertex3f(posX + x, posY + y, posZ);
			}
			break;
		}
		glEnd();
		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
}

void Circle3D_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	if (doubleValues[0] > 3)
		setting[1] = doubleValues[0]; // steps

	if ((radioValue >= 0) && (radioValue <= 2))
		setting[2] = radioValue; // axis

}
void doubleClickCircle(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Steps:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1] };

	vector<string> radioLabels = { "Align on X axis", "Align on Y axis", "Align on Z axis" };
	int radioIndex = pg->dgrm.cL[i]->setting[2];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		{}, {}, radioLabels, radioIndex, NULL);
}
std::vector<Button*>the3dCircleButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickCircle) };

bool Circle3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> Circle3D_Component::GetPinGroups() { return vector<int>{3, 2, 1, 3, 1, 1}; }
vector<string> Circle3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",				// 0=Position   (1)
		"Width", "Height", 			// 1=size		(2)
		"Line Width",				// 2=width		(4)
		"Red", "Green", "Blue",		// 3=color      (8)
		"Alpha",					// 4=alpha      (16)
		"[4x4]"  };
}


Circle3D_Component::Circle3D_Component()
	: ThreeD_Component(L"3Circle", L"3D Circle", squareShape, 5, 1, 0, 1)
{
	dialogButtonList = the3dCircleButtonList;
	width = 40;
	setSetting(0, 3);
	setSetting(1, 24);  // steps
	setSetting(2, 2);	// axis
	pinMaskIndex = 0;
}

// ***************************** grid ************************************

void Grid3D_Component::new3dOjbect() {

	inputsTo3dLoops(64);

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = getLoopingValueD(pin++, k, 0);
		double inputY = getLoopingValueD(pin++, k, 0);
		double inputZ = getLoopingValueD(pin++, k, 0);

		double inputWidth = 100;
		double inputHeight = 100;
		double inputDepth = 100;
		if ((int)setting[0] & 2) {
			inputWidth = getLoopingValueD(pin++, k, 100);
			inputHeight = getLoopingValueD(pin++, k, 100);
			inputDepth = getLoopingValueD(pin++, k, 100);
		}

		double inputStepX = 10;
		double inputStepY = 10;
		double inputStepZ = 10;
		if ((int)setting[0] & 4) {
			inputStepX = getLoopingValueD(pin++, k, 10);
			inputStepY = getLoopingValueD(pin++, k, 10);
			inputStepZ = getLoopingValueD(pin++, k, 10);
		}

		if (inputWidth < 0) continue;
		if (inputStepX <= 0) continue;
		if (inputWidth / inputStepX > 100) continue;

		if (inputHeight < 0) continue;
		if (inputStepY <= 0) continue;
		if (inputHeight / inputStepY > 100) continue;

		if (inputDepth < 0) continue;
		if (inputStepZ <= 0) continue;
		if (inputDepth / inputStepZ > 100) continue;

		double inputLineWidth = 1.0;
		if ((int)setting[0] & 8) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 16) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 32) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		if (first) {
			glPushMatrix();
			glLoadMatrixd(inMat);
			glDisable(GL_LIGHTING);
			first = false;
		}

		double minX = inputX - inputWidth / 2;
		double maxX = inputX + inputWidth / 2;
		double minY = inputY - inputHeight / 2;
		double maxY = inputY + inputHeight / 2;
		double minZ = inputZ - inputDepth / 2;
		double maxZ = inputZ + inputDepth / 2;

		glLineWidth(inputLineWidth);
		glColor3d(inputRed / 255, inputGreen / 255, inputBlue / 255);

		// lines parallel to the x axis
		for (double y = minY; y <= maxY; y += inputStepY) 
			for (double z = minZ; z <= maxZ; z += inputStepZ) {
				glBegin(GL_LINE_STRIP);
				glVertex3f(minX, y, z);
				glVertex3f(maxX, y, z);
				glEnd();
			}
		
		// lines parallel to the y axis
		for (double x = minX; x <= maxX; x += inputStepX) 
			for (double z = minZ; z <= maxZ; z += inputStepZ) {
				glBegin(GL_LINE_STRIP);
				glVertex3f(x, minY, z);
				glVertex3f(x, maxY, z);
				glEnd();
			}
		
		// lines parallel to the z axis
		for (double x = minX; x <= maxX; x += inputStepX) 
			for (double y = minY; y <= maxY; y += inputStepY) {
				glBegin(GL_LINE_STRIP);
				glVertex3f(x, y, minZ);
				glVertex3f(x, y, maxZ);
				glEnd();
			}
	}
	if (!first) {
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}
Grid3D_Component::Grid3D_Component()
	: ThreeD_Component(L"3Grid", L"3D Grid", squareShape, 15, 1, 0, 1)
{
	setLeftLabels(3, "X", "Y", "Z", "Width", "Height", "Depth", "X step", "Y step", "Z step", "Line Width", "Red", "Green", "Blue", "Alpha", "[4x4]");
	width = 40;
	setSetting(0, 127);
}


// ************************* Axis *****************************


void Axis3D_Component::new3dOjbect() {

	inputsTo3dLoops(32);

	glDisable(GL_LIGHTING);

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputShaftSize = 100;
		if ((int)setting[0] & 2) {
			inputShaftSize = getLoopingValueD(pin++, k, 100);
			if (inputShaftSize < 0)
				continue;
		}

		double inputLineWidth = 1.0;
		if ((int)setting[0] & 4) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 8) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 16) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		
		glPushMatrix();
		glLoadMatrixd(inMat);
		
		double arrowHead = inputShaftSize / 10;

		glLineWidth(inputLineWidth);

		glColor3d(inputRed / 255, inputGreen / 255, inputBlue / 255);

		// arrow parallel to the x axis
		glBegin(GL_LINE_STRIP);
		glVertex3d(inputX - inputShaftSize, inputY, inputZ);
		glVertex3d(inputX + inputShaftSize, inputY, inputZ);

		glVertex3d(inputX + inputShaftSize - arrowHead, inputY + arrowHead / 2, inputZ); // X arrow head
		glVertex3d(inputX + inputShaftSize,             inputY,                 inputZ);
		glVertex3d(inputX + inputShaftSize - arrowHead, inputY - arrowHead / 2, inputZ);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex3f(inputX + inputShaftSize + arrowHead,             inputY + arrowHead, inputZ);// letter X
		glVertex3f(inputX + inputShaftSize + arrowHead + arrowHead, inputY - arrowHead, inputZ);
		glEnd(); glBegin(GL_LINE_STRIP);
		glVertex3f(inputX + inputShaftSize + arrowHead,             inputY - arrowHead, inputZ);
		glVertex3f(inputX + inputShaftSize + arrowHead + arrowHead, inputY + arrowHead, inputZ);
		glEnd();
			

		// arrow parallel to the y axis
		glBegin(GL_LINE_STRIP);
		glVertex3d(inputX, inputY - inputShaftSize, inputZ);
		glVertex3d(inputX, inputY + inputShaftSize, inputZ);

		glVertex3d(inputX + arrowHead / 2, inputY + inputShaftSize - arrowHead, inputZ); // Y arrow head
		glVertex3d(inputX,                 inputY + inputShaftSize,             inputZ);
		glVertex3d(inputX - arrowHead / 2, inputY + inputShaftSize - arrowHead, inputZ);
		glEnd();

		glBegin(GL_LINE_STRIP);
		glVertex3f(inputX + arrowHead / 2.0f, inputY + inputShaftSize + arrowHead + arrowHead + arrowHead, inputZ);// letter Y
		glVertex3f(inputX,                    inputY + inputShaftSize + arrowHead + arrowHead,             inputZ);
		glVertex3f(inputX,                    inputY + inputShaftSize + arrowHead,                         inputZ);
		glVertex3f(inputX,                    inputY + inputShaftSize + arrowHead + arrowHead,             inputZ);
		glVertex3f(inputX - arrowHead / 2.0f, inputY + inputShaftSize + arrowHead + arrowHead + arrowHead, inputZ);
		glEnd();

		// arrow parallel to the z axis
		glBegin(GL_LINE_STRIP);
		glVertex3d(inputX, inputY, inputZ - inputShaftSize);
		glVertex3d(inputX, inputY, inputZ + inputShaftSize);

		glVertex3d(inputX, inputY + arrowHead / 2, inputZ + inputShaftSize - arrowHead); // Z arrow head
		glVertex3d(inputX, inputY,                 inputZ + inputShaftSize            );
		glVertex3d(inputX, inputY - arrowHead / 2, inputZ + inputShaftSize - arrowHead);
		glEnd();
		
		glBegin(GL_LINE_STRIP);
		glVertex3f(inputX, inputY + arrowHead, inputZ + inputShaftSize + arrowHead);// letter Z
		glVertex3f(inputX, inputY + arrowHead, inputZ + inputShaftSize + arrowHead + arrowHead);
		glVertex3f(inputX, inputY - arrowHead, inputZ + inputShaftSize + arrowHead);
		glVertex3f(inputX, inputY - arrowHead, inputZ + inputShaftSize + arrowHead + arrowHead);
		glEnd();

		glPopMatrix();
	}
	glEnable(GL_LIGHTING);
}


std::vector<Button*>the3dAxisButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool Axis3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> Axis3D_Component::GetPinGroups() { return vector<int>{ 3, 1, 1, 3, 1, 1 }; }
vector<string> Axis3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",			// 0=Position    (1)
		"Arrow Size",				// 1=size        (2)
		"Line Width",				// 2=Anchors     (4)
		"Red", "Green", "Blue",	// 3=text color  (8)
		"Alpha",					// 4=alpha       (16)
		"[4x4]" };
}
Axis3D_Component::Axis3D_Component()
	: ThreeD_Component(L"3Axis", L"3D Axis", squareShape, 5, 1, 0, 0)
{
	dialogButtonList = the3dAxisButtonList;
	width = 40;
	setSetting(0, 1);
	pinMaskIndex = 0;
}

// ***************************** Arrow **********************************


void Arrow3D_Component::new3dOjbect() {

	inputsTo3dLoops(16);

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX0 = 0;
		double inputY0 = 0;
		double inputZ0 = 0;
		double inputX1 = 1;
		double inputY1 = 1;
		double inputZ1 = 1;
		if ((int)setting[0] & 1) {
			inputX0 = getLoopingValueD(pin++, k, 0);
			inputY0 = getLoopingValueD(pin++, k, 0);
			inputZ0 = getLoopingValueD(pin++, k, 0);
			inputX1 = getLoopingValueD(pin++, k, 1);
			inputY1 = getLoopingValueD(pin++, k, 1);
			inputZ1 = getLoopingValueD(pin++, k, 1);
		}

		double inputLineWidth = 1.0;
		if ((int)setting[0] & 2) {
			inputLineWidth = getLoopingValueD(pin++, k, 1);
			if (inputLineWidth <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		if (first) {
			glDisable(GL_LIGHTING);
			glPushMatrix();
			glLoadMatrixd(inMat);
			first = false;
		}
		double dx = inputX1 - inputX0;
		double dy = inputY1 - inputY0;
		double dz = inputZ1 - inputZ0;
		double fullLength = sqrt(dx * dx + dy * dy + dz * dz);
		
		glLineWidth(inputLineWidth);
		glColor3d(inputRed / 255, inputGreen / 255, inputBlue / 255);
		
		glBegin(GL_LINE_STRIP); //  shaft
		glVertex3d(inputX0, inputY0, inputZ0);
		glVertex3d(inputX1, inputY1, inputZ1);
		glEnd();

		double ax, ay;
		double bx, by;
		double deltaX, deltaY, deltaZ;
		double dxol, dyol, dzol;
		double abz;

		double arrowSize = fullLength/10;

		double arrowHead15 = arrowSize;
		///double arrowHead7 = 2 * arrowSize / 3;
		double arrowHead5 = arrowSize / 3;
		//double arrowHead2 = arrowSize / 6;

		deltaX = abs(inputX0 - inputX1);
		deltaY = abs(inputY0 - inputY1);
		deltaZ = abs(inputZ0 - inputZ1);
		fullLength = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
			
		dxol = (inputX0 - inputX1) / fullLength;
		dyol = (inputY0 - inputY1) / fullLength;
		dzol = (inputZ0 - inputZ1) / fullLength;

		ax = inputX1 + arrowHead15 * dxol + arrowHead5 * dyol - arrowHead5 * dzol;
		bx = inputX1 + arrowHead15 * dxol - arrowHead5 * dyol;

		ay = inputY1 - arrowHead5 * dxol + arrowHead15 * dyol;
		by = inputY1 + arrowHead5 * dxol + arrowHead15 * dyol + arrowHead5 * dzol;
			
		abz = inputZ1 + arrowHead15 * dzol;

		glBegin(GL_LINE_STRIP); // head
		glVertex3d(ax, ay, abz);
		glVertex3d(inputX1, inputY1, inputZ1);
		glVertex3d(bx, by, abz);
		glEnd();
	}
	if (!first) {
		glPopMatrix();
		glEnable(GL_LIGHTING);
	}
}


std::vector<Button*>the3dArrowButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool Arrow3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> Arrow3D_Component::GetPinGroups() { return vector<int>{6, 1, 3, 1, 1}; }
vector<string> Arrow3D_Component::GetPinLabels() {
	return vector<string>{
		"X0", "Y0", "Z0", "X1", "Y1", "Z1",	// 0=Position    (1)
		"Line Width",						// 1=Anchors     (2)
		"Red", "Green", "Blue",				// 2=text color  (4)
		"Alpha",							// 3=alpha       (8)
		"[4x4]"};
}
Arrow3D_Component::Arrow3D_Component()
	: ThreeD_Component(L"3Arrows", L"3D Arrow", squareShape, 11, 1, 0, 0)
{
	//nC->setLeftLabels(3, "X0", "Y0", "Z0", "X1", "Y1", "Z1", "Red", "Green", "Blue", "Alpha", "[4x4]");
	width = 40; 
	dialogButtonList = the3dArrowButtonList;
	setSetting(0, 17);
	pinMaskIndex = 0;
}


// *************************** lathe ********************************

typedef struct
{
	double x;
	double y;
	double z;
} DOUBLE_3PNT;
void normalize3fv(double v[3])
{
	double d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (d == 0.0)
		return;

	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}
void normCrossProduct(double v1[3], double v2[3], double out[3])
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];

	normalize3fv(out);
}

void Lathe3D_Component::new3dOjbect() {

	if (!((int)setting[0] & 32)) // must have a polyline to lathe
		return;

	int rotationSteps = setting[1];
	if (rotationSteps < 3)
		return;

	int matPin = -1;
	int polyPin;
	int loops;
	if ((int)setting[0] & 64) {
		matPin = leftTerminalCount - 1;
		polyPin = matPin - 2;
	}
	else
	{
		polyPin = leftTerminalCount - 2;
	}
	loops = inputsToLoopsN(polyPin); // skip the polyline and matrix pins
	if (loops == 0)
		return;
	
	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 64) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}

	// one polygon for all lathed objects
	int polySize = input[polyPin].value.size();
	if (polySize < 2)
		return;
	if (polySize != input[polyPin + 1L].value.size())
		return;

	vector<double> inputPolyX;
	vector<double> inputPolyY;
	inputPolyX.resize(polySize);
	inputPolyY.resize(polySize);
	for (int j = 0; j < polySize; j++) {
		inputPolyX[j] = input[polyPin].value[j];
		inputPolyY[j] = input[polyPin + 1L].value[j];
	}

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);
		getSizeGroup(group++, &pin, k, 10);
		if (inputSizeX <= 0)
			continue;
		if (inputSizeY <= 0)
			continue;
		if (inputSizeZ <= 0)
			continue;
		getColorGroup(group++, &pin, k);
		float inputAlpha = getLoopingValueD(group, &pin, k, 1);
		if (inputAlpha <= 0)
			continue;

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		int axis = setting[2];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double xOffset = setting[4];
		double yOffset = setting[5];
		double zOffset = setting[6];

		doublePoly.resize(rotationSteps);
		for (int j2 = 0; j2 < rotationSteps; j2++)
		{
			double angle = j2 * TWO_PI / rotationSteps;
			double SinAngle = sin(angle);
			double CosAngle = cos(angle);

			doublePoly[j2].resize(polySize);
			for (int j1 = 0; j1 < polySize; j1++)
			{
				switch (axis)
				{
				case 0: // around X axis
					doublePoly[j2][j1].z = zOffset + inputSizeZ * inputPolyX[j1] * SinAngle;
					doublePoly[j2][j1].x = xOffset + inputSizeX * inputPolyY[j1]; // around X axis
					doublePoly[j2][j1].y = yOffset + inputSizeY * inputPolyX[j1] * CosAngle;
					break;

				case 1: // around Y axis
					doublePoly[j2][j1].x = xOffset + inputSizeX * inputPolyX[j1] * SinAngle;
					doublePoly[j2][j1].y = yOffset + inputSizeY * inputPolyY[j1]; // around Y axis
					doublePoly[j2][j1].z = zOffset + inputSizeZ * inputPolyX[j1] * CosAngle;
					break;

				case 2: // around Z axis
					doublePoly[j2][j1].y = yOffset + inputSizeY * inputPolyX[j1] * SinAngle;
					doublePoly[j2][j1].z = zOffset + inputSizeZ * inputPolyY[j1]; // around Z axis
					doublePoly[j2][j1].x = xOffset + inputSizeX * inputPolyX[j1] * CosAngle;
					break;
				}

			}
		}


		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX, outMat);
		matrixRotationYmult(inputRotateY, outMat);
		matrixRotationZmult(inputRotateZ, outMat);
		glLoadMatrixd(outMat);
			

		/* plot it */
		int j2p1;
		for (int j1 = 0; j1 < polySize - 1; j1++) // vertical along shape
		{
			for (int j2 = 0; j2 < rotationSteps; j2++)	// around the axis
			{
				j2p1 = j2 + 1;
				if (j2p1 == rotationSteps)
					j2p1 = 0;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[j2p1][j1 + 1L].x - doublePoly[j2p1][j1].x;
				v2[1] = doublePoly[j2p1][j1 + 1L].y - doublePoly[j2p1][j1].y;
				v2[2] = doublePoly[j2p1][j1 + 1L].z - doublePoly[j2p1][j1].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[j2][j1].x - doublePoly[j2p1][j1].x;
				v1[1] = doublePoly[j2][j1].y - doublePoly[j2p1][j1].y;
				v1[2] = doublePoly[j2][j1].z - doublePoly[j2p1][j1].z;

				normCrossProduct(v1, v2, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				glVertex3d(doublePoly[j2][j1].x,
					doublePoly[j2][j1].y,
					doublePoly[j2][j1].z);

				glVertex3d(doublePoly[j2][j1 + 1L].x,
					doublePoly[j2][j1 + 1L].y,
					doublePoly[j2][j1 + 1L].z);

				glVertex3d(doublePoly[j2p1][j1 + 1L].x,
					doublePoly[j2p1][j1 + 1L].y,
					doublePoly[j2p1][j1 + 1L].z);

				glVertex3d(doublePoly[j2p1][j1].x,
					doublePoly[j2p1][j1].y,
					doublePoly[j2p1][j1].z);

				glEnd();
			}
		}
		
		if ((int)setting[3] & 3) { // either end cap
			v1[0] = 0;
			v1[1] = 0;
			v1[2] = 0;
			v2[0] = 0;
			v2[1] = 0;
			v2[2] = 0;
			v1[axis] = 1;
			v2[axis] = -1;
			if ((int)setting[3] & 1) { // top cap
				glBegin(GL_POLYGON);
				glNormal3dv(v1);
				for (int j2 = 0; j2 < rotationSteps; j2++)
					glVertex3f(doublePoly[j2][0].x,
						doublePoly[j2][0].y,
						doublePoly[j2][0].z);
				glEnd();
			}
			if ((int)setting[3] & 2) { // bottom cap
				glBegin(GL_POLYGON);
				glNormal3dv(v2);
				for (int j2 = 0; j2 < rotationSteps; j2++)
					glVertex3f(doublePoly[j2][polySize - 1L].x,
						doublePoly[j2][polySize - 1L].y,
						doublePoly[j2][polySize - 1L].z);
				glEnd();
			}
		}
		glPopMatrix();
	}
	
}

void Lathe3D_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // steps

	setting[4] = doubleValues[1]; // x offset
	setting[5] = doubleValues[2]; // y offset
	setting[6] = doubleValues[3]; // z offset

	setting[3] = (boolValues[0] ? 1L : 0) + (boolValues[1] ? 2 : 0);

	if ((radioValue >= 0) && (radioValue <= 2))
		setting[2] = radioValue; // axis

}
void doubleClickLathe(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Steps:", "X Offset", "Y Offset", "Z Offset"};
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1],
	pg->dgrm.cL[i]->setting[4], pg->dgrm.cL[i]->setting[5], pg->dgrm.cL[i]->setting[6] };

	vector<string> checkLabels = { "Close the Top", "Close the Bottom" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[3] & 1), (bool)((int)pg->dgrm.cL[i]->setting[3] & 2) };

	vector<string> radioLabels = { "Align on X axis", "Align on Y axis", "Align on Z axis" };
	int radioIndex = pg->dgrm.cL[i]->setting[2];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex, NULL);
}
std::vector<Button*>the3dLatheButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickLathe),
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

bool Lathe3D_Component::onLoad(Sheet* pg) {
	if (setting.size() < 7) {
		setSetting(4, 0); // x offset
		setSetting(5, 0); // y offset
		setSetting(6, 0); // z offset
	}
	buildPinLabels();
	return false;
}

vector<int> Lathe3D_Component::GetPinGroups() { return vector<int>{3, 3, 3, 3, 1, 2, 1 }; }
vector<string> Lathe3D_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",	// 0=Position   (1)
		"X Rotation", "Y Rotation", "Z Rotation",	// 1=Position   (2)
		"X Size", "Y Size", "Z Size",				// 2=size		(4)
		"Red", "Green", "Blue",						// 3=color      (8)
		"Alpha",									// 4=alpha      (16)
		"X polyline", "Y polyline",					// 5=polygon    (32)
		"[4x4]"  };									// 6=matrix		(64)
}
Lathe3D_Component::Lathe3D_Component()
	: ThreeD_Component(L"3Lathe", L"3D Lathe", squareShape, 13, 1, 0, 0)
{
	dialogButtonList = the3dLatheButtonList;
	width = 40;
	setSetting(0, 1L +32);
	setSetting(1, 20); // lathe points
	setSetting(2, 0); // axis
	setSetting(3, 0); // endcap bits
	setSetting(4, 0); // x offset
	setSetting(5, 0); // y offset
	setSetting(6, 0); // z offset
	pinMaskIndex = 0;
}

// ****************************** Extrude *****************************************

// for tess
/*
void beginCB(GLenum which) {	
	glBegin(GL_POLYGON);
}
void endCB() { 
	glEnd(); 
}
void combineCB(GLdouble coords[3],
	GLdouble* vertex_data[4],
	GLfloat weight[4], GLdouble** dataOut)
{
	GLdouble* vertex;
	int i;

	vertex = (GLdouble*)malloc(6 * sizeof(GLdouble));
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	for (i = 3; i < 7; i++)
		vertex[i] = weight[0] * vertex_data[0][i]
		+ weight[1] * vertex_data[1][i]
		+ weight[2] * vertex_data[2][i]
		+ weight[3] * vertex_data[3][i];
	*dataOut = vertex;
}
void errorCB(GLenum errorCode) { 
	const GLubyte* estring;

	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}
void vertexCB(GLvoid* vertex)
{
	const GLdouble* pointer = (GLdouble*)vertex;
	glColor3dv(pointer + 3);
	glVertex3dv(pointer);
}
*/
void Extrude3D_Component::new3dOjbect() {

	if (!((int)setting[0] & 64)) // must have a polyline to lathe
		return;

	int depthSteps = setting[1];
	if (depthSteps < 1)
		return;
	
	int matPin = -1;
	int polyPin;
	int loops;
	if ((int)setting[0] & 128) {
		matPin = leftTerminalCount - 1;
		polyPin = matPin - 2;
	}
	else
	{
		polyPin = leftTerminalCount - 2;
	}
	loops = inputsToLoopsN(polyPin); // skip the polyline and matrix pins
	if (loops == 0)
		return;
	
	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 128) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}

	// one polygon for all lathed objects
	int polySize = input[polyPin].value.size();
	if (polySize < 2)
		return;
	if (polySize != input[polyPin + 1L].value.size())
		return;

	vector<double> inputPolyX;
	vector<double> inputPolyY;
	inputPolyX.resize(polySize);
	inputPolyY.resize(polySize);
	for (int j = 0; j < polySize; j++) {
		inputPolyX[j] = input[polyPin].value[j];
		inputPolyY[j] = input[polyPin + 1L].value[j];
	}


	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		getPositionGroup(group++, &pin, k);
		getRotationGroup(group++, &pin, k);
		getSizeGroup(group++, &pin, k, 10);
		if (inputSizeX <= 0)
			continue;
		if (inputSizeY <= 0)
			continue;
		if (inputSizeZ <= 0)
			continue;
		
		double inputDepth = getLoopingValueD(group, &pin, k, 1);
		if (inputDepth <= 0)
			continue;
		group++;
		getColorGroup(group++, &pin, k);
		float inputAlpha = getLoopingValueD(group, &pin, k, 1);
		if (inputAlpha <= 0)
			continue;

		int axis = setting[2];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double step = inputDepth / depthSteps;
		double xOffset = setting[4];
		double yOffset = setting[5];
		double zOffset = setting[6];

		doublePoly.resize(depthSteps);
		for (int j2 = 0; j2 < depthSteps; j2++)
		{
			doublePoly[j2].resize(polySize);
			double depth = j2 * step;
			for (int j1 = 0; j1 < polySize; j1++)
			{
				switch (axis)
				{
				case 0: // around X axis
					doublePoly[j2][j1].z = zOffset + inputSizeZ * inputPolyX[j1];
					doublePoly[j2][j1].x = xOffset + inputSizeX * depth; // along X axis
					doublePoly[j2][j1].y = yOffset + inputSizeY * inputPolyY[j1];
					break;

				case 1: // around Y axis
					doublePoly[j2][j1].x = xOffset + inputSizeX * inputPolyX[j1];
					doublePoly[j2][j1].y = yOffset + inputSizeY * depth; // along Y axis
					doublePoly[j2][j1].z = zOffset + inputSizeZ * inputPolyY[j1];
					break;

				case 2: // around Z axis
					doublePoly[j2][j1].y = yOffset + inputSizeY * inputPolyY[j1];
					doublePoly[j2][j1].z = zOffset + inputSizeZ * depth; // along Z axis
					doublePoly[j2][j1].x = xOffset + inputSizeX * inputPolyX[j1];
					break;
				}
			}
		}
		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;// doesn't seem to work
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
		
		glPushMatrix();
		double outMat[16];
		copyMatrix(inMat, outMat);
		matrixTranslateXYZmult(inputX, inputY, inputZ, outMat);
		matrixRotationXmult(inputRotateX, outMat);
		matrixRotationYmult(inputRotateY, outMat);
		matrixRotationZmult(inputRotateZ, outMat);
		glLoadMatrixd(outMat);
			

		/* plot the body */
		int j2p1;
		for (int j1 = 0; j1 < polySize - 1; j1++) // vertical along shape
		{
			for (int j2 = 0; j2 < depthSteps - 1; j2++)	// around the axis
			{
				j2p1 = j2 + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[j2p1][j1 + 1L].x - doublePoly[j2p1][j1].x;
				v2[1] = doublePoly[j2p1][j1 + 1L].y - doublePoly[j2p1][j1].y;
				v2[2] = doublePoly[j2p1][j1 + 1L].z - doublePoly[j2p1][j1].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[j2][j1].x - doublePoly[j2p1][j1].x;
				v1[1] = doublePoly[j2][j1].y - doublePoly[j2p1][j1].y;
				v1[2] = doublePoly[j2][j1].z - doublePoly[j2p1][j1].z;

				normCrossProduct(v1, v2, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				glVertex3d(doublePoly[j2][j1].x,
					doublePoly[j2][j1].y,
					doublePoly[j2][j1].z);

				glVertex3d(doublePoly[j2][j1 + 1L].x,
					doublePoly[j2][j1 + 1L].y,
					doublePoly[j2][j1 + 1L].z);

				glVertex3d(doublePoly[j2p1][j1 + 1L].x,
					doublePoly[j2p1][j1 + 1L].y,
					doublePoly[j2p1][j1 + 1L].z);

				glVertex3d(doublePoly[j2p1][j1].x,
					doublePoly[j2p1][j1].y,
					doublePoly[j2p1][j1].z);

				glEnd();
			}
		}

		// plot the endcaps?
		if (((int)setting[3] & 3) && // either end cap
			(inputPolyX[0] == inputPolyX[polySize - 1L]) &&	// must be a closed shape
			(inputPolyY[0] == inputPolyY[polySize - 1L])) {
			v1[0] = 0;
			v1[1] = 0;
			v1[2] = 0;
			v2[0] = 0;
			v2[1] = 0;
			v2[2] = 0;
			v1[axis] = 1;
			v2[axis] = -1;

			if ((int)setting[3] & 1) { // top cap
#if 0
				GLUtesselator* tess = gluNewTess();
				gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid(*) ())& beginCB);
				gluTessCallback(tess, GLU_TESS_END, (GLvoid(*) ())& endCB);
				gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid(*) ())& vertexCB);
				gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid(*) ())& combineCB);
				gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid(*) ())& combineCB);
				GLdouble rect[3];
				gluTessBeginPolygon(tess, NULL);
				gluTessBeginContour(tess);
				for (int j1 = 0; j1 < polySize; j1++) {
					rect[0] = doublePoly[0][j1].x;
					rect[1] = doublePoly[0][j1].y;
					rect[2] = doublePoly[0][j1].z;
					gluTessVertex(tess, rect, rect);
				}
				gluTessEndContour(tess);
				gluTessEndPolygon(tess);

				gluDeleteTess(tess);
#else
				glBegin(GL_POLYGON);
				glNormal3dv(v2);
				for (int j1 = 0; j1 < polySize; j1++)
					glVertex3d(doublePoly[0][j1].x,
						doublePoly[0][j1].y,
						doublePoly[0][j1].z);
				glEnd();
#endif
			}
			if ((int)setting[3] & 2) { // bottom cap
				glBegin(GL_POLYGON);
				glNormal3dv(v1);
				for (int j1 = 0; j1 < polySize; j1++)
					glVertex3d(doublePoly[depthSteps - 1L][j1].x,
						doublePoly[depthSteps - 1L][j1].y,
						doublePoly[depthSteps - 1L][j1].z);
				glEnd();
			}
		}
		glPopMatrix();
	}
	
}

void Extrude3D_Component::completion(Sheet* pg, double *doubleValues, bool *boolValues, int radioValue) {
	if (doubleValues[0] > 2)
		setting[1] = doubleValues[0]; // steps

	setting[4] = doubleValues[1]; // x offset
	setting[5] = doubleValues[2]; // y offset
	setting[6] = doubleValues[3]; // z offset

	setting[3] = (boolValues[0] ? 1L : 0) + (boolValues[1] ? 2 : 0);

	if ((radioValue >= 0) && (radioValue <= 2))
		setting[2] = radioValue; // axis

}
void doubleClickExtrude(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "Number of Depths:", "X Offset", "Y Offset", "Z Offset" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[1],
	pg->dgrm.cL[i]->setting[4], pg->dgrm.cL[i]->setting[5], pg->dgrm.cL[i]->setting[6] };

	vector<string> checkLabels = { "Close the Top", "Close the Bottom" };
	vector<bool> checkValues = { (bool)((int)pg->dgrm.cL[i]->setting[3] & 1), (bool)((int)pg->dgrm.cL[i]->setting[3] & 2) };

	vector<string> radioLabels = { "Align on X axis", "Align on Y axis", "Align on Z axis" };
	int radioIndex = pg->dgrm.cL[i]->setting[2];

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex, NULL);
}
std::vector<Button*>the3dExtrudeButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickExtrude) ,
	new Button(3, 80, 85, 20, L"Rotation...", rotationActionGeneral3D) };

bool Extrude3D_Component::onLoad(Sheet* pg) {
	if (setting.size() < 7) {
		setSetting(4, 0); // x offset
		setSetting(5, 0); // y offset
		setSetting(6, 0); // z offset
	}
	buildPinLabels();
	return false;
}

vector<int> Extrude3D_Component::GetPinGroups() { return vector<int>{3, 3, 3, 1, 3, 1, 2, 1}; }
vector<string> Extrude3D_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position", "Z Position",	// 0=Position   (1)
		"X Rotation", "Y Rotation", "Z Rotation",	// 1=Position   (2)
		"X Size", "Y Size", "Z Size",				// 2=size		(4)
		"Depth",									// 3=depth		(8)
		"Red", "Green", "Blue",						// 4=color      (16)
		"Alpha",									// 5=alpha      (32)
		"X polygon", "Y polygon",					// 6=polygon    (64)
		"[4x4]"  };									// 7=matrix		(128)
}

Extrude3D_Component::Extrude3D_Component()
	: ThreeD_Component(L"3Extrude", L"3D Extrude", squareShape, 16, 1, 0, 1)
{
	dialogButtonList = the3dExtrudeButtonList;
	width = 40;
	pinMaskIndex = 0;
	setSetting(0, 1L +8+64);
	setSetting(1, 10); // depth points
	setSetting(2, 0); // axis
	setSetting(3, 0); // endcap bits
	setSetting(4, 0); // x offset
	setSetting(5, 0); // y offset
	setSetting(6, 0); // z offset
}

// ****************************** Plane Surface ***********************************



void PlaneSurface3D_Component::new3dOjbect() {

	if (!((int)setting[0] & 16)) // must have a dataset make a surface
		return;

	int matPin = -1;
	int dataPin;
	int loops;
	if ((int)setting[0] & 32) {
		matPin = leftTerminalCount - 1;
		dataPin = matPin - 1;
	}
	else
	{
		dataPin = leftTerminalCount - 1;
	}
	loops = inputsToLoopsN(dataPin); // skip the polyline and matrix pins
	if (loops == 0)
		return;
	
	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}

	// one data set for all surface objects
	int polySize = input[dataPin].value.size();
	if (polySize < 4)
		return;
	if(input[dataPin].dimensions.size() != 2)
		return;
	int dataSizeX = input[dataPin].dimensions[0];
	if( dataSizeX < 2 )
		return;
	int dataSizeY = input[dataPin].dimensions[1];
	if( dataSizeY < 2 )
		return;

	vector<vector<double>> inputData;
	inputData.resize(dataSizeY);
	int j3 = 0;
	for (int jy = 0; jy < dataSizeY; jy++) {
		inputData[jy].resize(dataSizeX);
		for (int jx = 0; jx < dataSizeX; jx++) {
			inputData[jy][jx] = input[dataPin].value[j3++];
		}
	}


	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputSizeX = 1;
		double inputSizeY = 1;
		double inputSizeZ = 1;
		if ((int)setting[0] & 2) {
			inputSizeX = getLoopingValueD(pin++, k, 1);
			inputSizeY = getLoopingValueD(pin++, k, 1);
			inputSizeZ = getLoopingValueD(pin++, k, 1);
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		int axis = setting[1];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double stepX = inputSizeX / dataSizeX;
		double stepY = inputSizeY / dataSizeY;

		doublePoly.resize(dataSizeY);
		for (int jy = 0; jy < dataSizeY; jy++)
		{
			double y = jy * stepY;
			doublePoly[jy].resize(dataSizeX);
			for (int jx = 0; jx < dataSizeX; jx++)
			{
				double x = jx * stepX;
				doublePoly[jy][jx].x = inputX + x;
				doublePoly[jy][jx].y = inputY + y;
				doublePoly[jy][jx].z = inputZ + inputSizeZ * inputData[jy][jx]; // data on Z axis
			}
		}


		glPushMatrix();
		glLoadMatrixd(inMat);
		

		/* plot it */
		int jxp1;
		int jyp1;
		for (int jy = 0; jy < dataSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < dataSizeX - 1; jx++)	
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v1, v2, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);

				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);

				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
		}
		glPopMatrix();
	}
	
}


std::vector<Button*>the3dPlaneSurfaceButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool PlaneSurface3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> PlaneSurface3D_Component::GetPinGroups() { return vector<int>{3, 3, 3, 1, 1, 1}; }
vector<string> PlaneSurface3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",					// 0=Position   (1)
		"X size", "Y size", "Z size",	// 1=size		(2)
		"Red", "Green", "Blue",			// 2=color      (4)
		"Alpha",							// 3=alpha      (8)
		"Surface Level",					// 4=data       (16)
		"[4x4]"  };
}
PlaneSurface3D_Component::PlaneSurface3D_Component()
	: ThreeD_Component(L"3PlSurf", L"3D Plane Surface", squareShape, 12, 1, 0, 1)
{
	dialogButtonList = the3dPlaneSurfaceButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
}


// ****************************** Plane Surface 2 ***********************************

typedef struct
{
	GLfloat color[4];
} GL_FLOAT_4;


void PlaneSurface3D2_Component::new3dOjbect() {

	if ( !((int)setting[0] & 8) && // must have a level dataset 
		 !((int)setting[0] & 16)  )   // or a color dataset make a surface
		return;

	int matrixPin = -1;
	int levelPin;
	int colorPin;
	int loops;
	int lastPin = leftTerminalCount - 1;

	if ((int)setting[0] & 32)	// matrix
		matrixPin = lastPin--;
	if ((int)setting[0] & 16) {	// color
		colorPin = lastPin - 2;
		lastPin -= 3;
	}
	if ((int)setting[0] & 8)	// level 
		levelPin = lastPin--;

	loops = inputsToLoopsN(lastPin); // skip the level, color and matrix pins
	if (loops == 0)
		return;

	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matrixPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matrixPin, z, inMat[z]);
	}

	int levelSize;
	int levelSizeX = 0;
	int levelSizeY = 0;
	if ((int)setting[0] & 8) {	// level 
		// one level set for all surface objects
		levelSize = input[levelPin].value.size();
		if (levelSize < 4)
			return;
		if (input[levelPin].dimensions.size() != 2)
			return;
		levelSizeX = input[levelPin].dimensions[0];
		if (levelSizeX < 2)
			return;
		levelSizeY = input[levelPin].dimensions[1];
		if (levelSizeY < 2)
			return;
	}

	int colorSize;
	int colorSizeX;
	int colorSizeY;
	if ((int)setting[0] & 16) {	// color
		// one color set for all surface objects
		colorSize = input[colorPin].value.size();
		if (colorSize < 4)
			return;
		if (input[colorPin].dimensions.size() != 2)
			return;
		colorSizeX = input[colorPin].dimensions[0];
		if (colorSizeX < 2)
			return;
		colorSizeY = input[colorPin].dimensions[1];
		if (colorSizeY < 2)
			return;

		for (int k = 1; k < 3; k++) {
			if (colorSize != input[(long)colorPin + k].value.size())
				return;
			if (2 != input[(long)colorPin+k].dimensions.size())
				return;
			if (colorSizeX != input[(long)colorPin+k].dimensions[0])
				return;
			if (colorSizeY != input[(long)colorPin + k].dimensions[1])
				return;
		}
	}

	if (((int)setting[0] & 8) && ((int)setting[0] & 16)) {	// level and color
		if (colorSize != levelSize < 4)
			return;
		if (colorSizeX != levelSizeX)
			return;
		if (colorSizeY != levelSizeY)
			return;
	}

	if (! ((int)setting[0] & 8)) {	// no level, use color
		levelSize = colorSize;
		levelSizeX = colorSizeX;
		levelSizeY = colorSizeY;
	}

	vector<vector<GL_FLOAT_4>> floatColor;
	if ((int)setting[0] & 16) {	// build color surface
		
		floatColor.resize((long)colorSizeY);
		int j4 = 0;
		for (int jy = colorSizeY-1; jy >= 0; jy--) {
			floatColor[jy].resize((long)colorSizeX);
			for (int jx = 0; jx < colorSizeX; jx++) {
				floatColor[jy][jx].color[0] = input[colorPin].value[j4] / 256;
				floatColor[jy][jx].color[1] = input[colorPin+1L].value[j4] /256;
				floatColor[jy][jx].color[2] = input[colorPin+2L].value[j4] / 256;
				floatColor[jy][jx].color[3] = 1;
				j4++;
			}
		}
	}

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputSizeX = 1;
		double inputSizeY = 1;
		double inputSizeZ = 1;
		if ((int)setting[0] & 2) {
			inputSizeX = getLoopingValueD(pin++, k, 1);
			inputSizeY = getLoopingValueD(pin++, k, 1);
			inputSizeZ = getLoopingValueD(pin++, k, 1);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 4) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		int axis = setting[1];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];

		double stepX = inputSizeX / levelSizeX;
		double stepY = inputSizeY / levelSizeY;
		double xOffset = stepX * levelSizeX / 2;
		double yOffset = stepY * levelSizeY / 2;

		doublePoly.resize(levelSizeY);
		int j3 = 0;
		for (int jy = levelSizeY - 1; jy >= 0; jy--) 
		{
			double y = jy * stepY;
			doublePoly[jy].resize(levelSizeX);
			for (int jx = 0; jx < levelSizeX; jx++)
			{
				double x = jx * stepX;
				doublePoly[jy][jx].x = inputX + x - xOffset;
				doublePoly[jy][jx].y = inputY + y - yOffset;
				if ((int)setting[0] & 8) // level data
					doublePoly[jy][jx].z = inputZ + inputSizeZ * input[levelPin].value[j3++]; // data on Z axis
				else
					doublePoly[jy][jx].z = inputZ; 
			}
		}
		
		if (first) {
			float Surface[4];
			Surface[0] = 1;
			Surface[1] = 1;
			Surface[2] = 1;
			Surface[3] = 1;// doesn't seem to work
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
			glLoadMatrixd(inMat);
			first = false;
		}
		
		glClear(GL_COLOR_BUFFER_BIT);

		/* plot it */
		int jxp1;
		int jyp1;

		for (int jy = 0; jy < levelSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < levelSizeX - 1; jx++)
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v1, v2, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jy][jx]);
				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);


				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jy][jxp1]);
				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);


				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jyp1][jxp1]);
				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jyp1][jx]);
				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
			j3++;
		}
		j3++;
	}
	glPopMatrix();
}


std::vector<Button*>the3dPlaneSurface2ButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool PlaneSurface3D2_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> PlaneSurface3D2_Component::GetPinGroups() { return vector<int>{3, 3, 1, 1, 3, 1 }; }
vector<string> PlaneSurface3D2_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",					// 0=Position   (1)
		"X size", "Y size", "Z size",	// 1=size		(2)
		"Alpha",							// 2=alpha      (4)
		"Surface Level",					// 3=data       (8)
		"Surface Red", "Surface Green", "Surface Blue",			// 4=color      (16)
		"[4x4]"  };
}
PlaneSurface3D2_Component::PlaneSurface3D2_Component()
	: ThreeD_Component(L"3PlSurf2", L"3D Plane Surface 2", squareShape, 12, 1, 0, 1)
{
	dialogButtonList = the3dPlaneSurface2ButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
}


// **************************** Sphere Surface ********************************


void SphereSurface3D_Component::new3dOjbect() {

	if (!((int)setting[0] & 16)) // must have a dataset make a surface
		return;

	int matPin = -1;
	int dataPin;
	int loops;
	if ((int)setting[0] & 32) {
		matPin = leftTerminalCount - 1;
		dataPin = matPin - 1;
	}
	else
	{
		dataPin = leftTerminalCount - 1;
	}
	loops = inputsToLoopsN(dataPin); // skip the dataset and matrix pins
	if (loops == 0)
		return;
	
	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}

	// one data set for all surface objects
	int polySize = input[dataPin].value.size();
	if (polySize < 9)
		return;
	if (input[dataPin].dimensions.size() != 2)
		return;
	int dataSizeX = input[dataPin].dimensions[0];
	if (dataSizeX < 3)
		return;
	int dataSizeY = input[dataPin].dimensions[1];
	if (dataSizeY < 3)
		return;

	vector<vector<double>> inputData;
	inputData.resize(dataSizeY);
	int j3 = 0;
	for (int jy = 0; jy < dataSizeY; jy++) {
		inputData[jy].resize(dataSizeX);
		for (int jx = 0; jx < dataSizeX; jx++) {
			inputData[jy][jx] = input[dataPin].value[j3++];
		}
	}

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputRadius = 1;
		if ((int)setting[0] & 2) {
			inputRadius = getLoopingValueD(pin++, k, 1);
			if (inputRadius <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		int axis = setting[1];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double stepX = TWO_PI / dataSizeX;	// longitude
		double stepY = PI / (dataSizeY-1L);		// latitude

		doublePoly.resize(dataSizeY);
		for (int jy = 0; jy < dataSizeY; jy++)
		{
			double ySin = sin(jy * stepY);
			double yCos = cos(jy * stepY);
			doublePoly[jy].resize(dataSizeX+1L);
			for (int jx = 0; jx < dataSizeX; jx++)
			{
				double xSin = sin(jx * stepX);
				double xCos = cos(jx * stepX);
				double radius = inputRadius + inputData[jy][jx];
				doublePoly[jy][jx].x = inputX + radius * xSin * ySin;
				doublePoly[jy][jx].y = inputY + radius * xCos * ySin;
				doublePoly[jy][jx].z = inputZ - radius * yCos;
			}
			doublePoly[jy][dataSizeX].x = doublePoly[jy][0].x;
			doublePoly[jy][dataSizeX].y = doublePoly[jy][0].y;
			doublePoly[jy][dataSizeX].z = doublePoly[jy][0].z;
		}
		

		glPushMatrix();
		glLoadMatrixd(inMat);

		/* plot it */
		int jxp1;
		int jyp1;
		for (int jy = 0; jy < dataSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < dataSizeX; jx++)
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v2, v1, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);

				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);

				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
		}
		glPopMatrix();
	}
	
}


std::vector<Button*>the3dSphereSurfaceButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool SphereSurface3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> SphereSurface3D_Component::GetPinGroups() { return vector<int>{3, 1, 3, 1, 1, 1}; }
vector<string> SphereSurface3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",			// 0=Position   (1)
		"Radius",				// 1=size		(2)
		"Red", "Green", "Blue",	// 2=color      (4)
		"Alpha",				// 3=alpha      (8)
		"Surface Level",			// 4=data       (16)
		"[4x4]"  };
}
SphereSurface3D_Component::SphereSurface3D_Component()
	: ThreeD_Component(L"3SphSurf", L"3D Sphere Surface", squareShape, 12, 1, 0, 1)
{
	dialogButtonList = the3dSphereSurfaceButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
}

// *************************** Cylinder Surface ******************************************


void CylinderSurface3D_Component::new3dOjbect() {

	if (!((int)setting[0] & 16)) // must have a dataset make a surface
		return;

	int matPin = -1;
	int dataPin;
	int loops;
	if ((int)setting[0] & 32) {
		matPin = leftTerminalCount - 1;
		dataPin = matPin - 1;
	}
	else
	{
		dataPin = leftTerminalCount - 1;
	}
	loops = inputsToLoopsN(dataPin); // skip the dataset and matrix pins
	if (loops == 0)
		return;
	
	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}

	// one data set for all surface objects
	int polySize = input[dataPin].value.size();
	if (polySize < 9)
		return;
	if (input[dataPin].dimensions.size() != 2)
		return;
	int dataSizeX = input[dataPin].dimensions[0];
	if (dataSizeX < 3)
		return;
	int dataSizeY = input[dataPin].dimensions[1];
	if (dataSizeY < 3)
		return;

	vector<vector<double>> inputData;
	inputData.resize(dataSizeY);
	int j3 = 0;
	for (int jy = 0; jy < dataSizeY; jy++) {
		inputData[jy].resize(dataSizeX);
		for (int jx = 0; jx < dataSizeX; jx++) {
			inputData[jy][jx] = input[dataPin].value[j3++];
		}
	}


	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputRadius = 1;
		double inputHeight = 1;
		if ((int)setting[0] & 2) {
			inputRadius = getLoopingValueD(pin++, k, 1);
			if (inputRadius <= 0)
				continue;
			inputHeight = getLoopingValueD(pin++, k, 1);
			if (inputHeight <= 0)
				continue;
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}
		
		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		int axis = setting[1];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double stepX = TWO_PI / dataSizeX;	// longitude
		double stepY = inputHeight / dataSizeY;

		doublePoly.resize(dataSizeY);
		for (int jy = 0; jy < dataSizeY; jy++)
		{
			double y = jy * stepY;
			
			doublePoly[jy].resize(dataSizeX + 1L);
			for (int jx = 0; jx < dataSizeX; jx++)
			{
				double xSin = sin(jx * stepX);
				double xCos = cos(jx * stepX);
				double radius = inputRadius + inputData[jy][jx];
				doublePoly[jy][jx].x = inputX + radius * xSin;
				doublePoly[jy][jx].y = inputY + radius * xCos;
				doublePoly[jy][jx].z = inputZ + y;
			}
			doublePoly[jy][dataSizeX].x = doublePoly[jy][0].x;
			doublePoly[jy][dataSizeX].y = doublePoly[jy][0].y;
			doublePoly[jy][dataSizeX].z = doublePoly[jy][0].z;
		}
		

		glPushMatrix();
		glLoadMatrixd(inMat);
		
		/* plot it */
		int jxp1;
		int jyp1;
		for (int jy = 0; jy < dataSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < dataSizeX; jx++)
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v2, v1, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);

				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);

				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
		}
		glPopMatrix();
	}
	
}


std::vector<Button*>the3dCylinderSurfaceButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool CylinderSurface3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> CylinderSurface3D_Component::GetPinGroups() { return vector<int>{ 3, 2, 3, 1, 1, 1 }; }
vector<string> CylinderSurface3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",			// 0=Position   (1)
		"Radius", "Height", 	// 1=size		(2)
		"Red", "Green", "Blue",	// 2=color      (4)
		"Alpha",				// 3=alpha      (8)
		"Surface Level",		// 4=data       (16)
		"[4x4]"  };
}
CylinderSurface3D_Component::CylinderSurface3D_Component()
	: ThreeD_Component(L"3CylSurf", L"3D Cylinder Surface", squareShape, 12, 1, 0, 1)
{
	dialogButtonList = the3dCylinderSurfaceButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
//	double matrix[16];
//	float surface[4];
	vector<vector<DOUBLE_3PNT>>	doublePoly;
}


// ************************* Surface ******************************


void Surface3D_Component::new3dOjbect() {

	if (((int)setting[0] & 16) == 0) // must have the datasets make a surface
		return;

	int matPin = -1;
	int dataPin0;
	int loops;
	if ((int)setting[0] & 32) {
		matPin = leftTerminalCount - 1;
		dataPin0 = matPin - 3;
	}
	else
	{
		dataPin0 = leftTerminalCount - 3;
	}
	loops = inputsToLoopsN(dataPin0); // skip the polyline and matrix pins
	if (loops == 0)
		return;
	
	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matPin, z, inMat[z]);
	}

	// one data set for all surface objects
	int dataSize = input[dataPin0].value.size();
	if (dataSize < 4)
		return;
	if (input[dataPin0].dimensions.size() != 2)
		return;
	int dataSizeX = input[dataPin0].dimensions[0];
	if (dataSizeX < 2)
		return;
	int dataSizeY = input[dataPin0].dimensions[1];
	if (dataSizeY < 2)
		return;
	for (int d = dataPin0+1; d < dataPin0+3; d++) {
		if (dataSize != input[d].value.size())
			return;
		if (input[d].dimensions.size() != 2)
			return;
		if (dataSizeX != input[d].dimensions[0])
			return;
		if (dataSizeY != input[d].dimensions[1])
			return;
	}

	vector<vector<double>> inputData0;
	vector<vector<double>> inputData1;
	vector<vector<double>> inputData2;
	inputData0.resize(dataSizeY);
	inputData1.resize(dataSizeY);
	inputData2.resize(dataSizeY);
	int j3 = 0;
	for (int jy = 0; jy < dataSizeY; jy++) {
		inputData0[jy].resize(dataSizeX);
		inputData1[jy].resize(dataSizeX);
		inputData2[jy].resize(dataSizeX);
		for (int jx = 0; jx < dataSizeX; jx++) {
			inputData0[jy][jx] = input[dataPin0].value[j3];
			inputData1[jy][jx] = input[dataPin0+1L].value[j3];
			inputData2[jy][jx] = input[dataPin0+2L].value[j3++];
		}
	}

	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputSizeX = 1;
		double inputSizeY = 1;
		double inputSizeZ = 1;
		if ((int)setting[0] & 2) {
			inputSizeX = getLoopingValueD(pin++, k, 1);
			inputSizeY = getLoopingValueD(pin++, k, 1);
			inputSizeZ = getLoopingValueD(pin++, k, 1);
		}

		//"Red", "Green", "Blue"
		double inputRed = 255;
		double inputGreen = 255;
		double inputBlue = 255;
		if ((int)setting[0] & 4) {
			inputRed = getLoopingValueD(pin++, k, 0);
			inputGreen = getLoopingValueD(pin++, k, 0);
			inputBlue = getLoopingValueD(pin++, k, 0);
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 8) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		float Surface[4];
		Surface[0] = inputRed / 255;
		Surface[1] = inputGreen / 255;
		Surface[2] = inputBlue / 255;
		Surface[3] = inputAlpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);

		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double stepX = inputSizeX / dataSizeX;
		double stepY = inputSizeY / dataSizeY;

		doublePoly.resize(dataSizeY);
		for (int jy = 0; jy < dataSizeY; jy++)
		{
			doublePoly[jy].resize(dataSizeX);
			for (int jx = 0; jx < dataSizeX; jx++)
			{
				doublePoly[jy][jx].x = inputX + inputSizeX * inputData0[jy][jx];
				doublePoly[jy][jx].y = inputY + inputSizeY * inputData1[jy][jx];
				doublePoly[jy][jx].z = inputZ + inputSizeZ * inputData2[jy][jx];
			}
		}


		glPushMatrix();
		glLoadMatrixd(inMat);

		/* plot it */
		int jxp1;
		int jyp1;
		for (int jy = 0; jy < dataSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < dataSizeX - 1; jx++)
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v1, v2, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);

				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);

				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
		}
		glPopMatrix();
	}
	
}


std::vector<Button*>the3dSurfaceButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool Surface3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> Surface3D_Component::GetPinGroups() { return vector<int>{3, 3, 3, 1, 3, 1}; }
vector<string> Surface3D_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",						// 0=Position   (1)
		"X size", "Y size", "Z size",		// 1=size		(2)
		"Red", "Green", "Blue",				// 2=color      (4)
		"Alpha",							// 3=alpha      (8)
		"X Surface", "Y Surface", "Z Surface",// 4=data       (16)
		"[4x4]"  };
}
Surface3D_Component::Surface3D_Component()
	: ThreeD_Component(L"3Surf", L"3D Surface", squareShape, 8, 1, 0, 1)
{
	dialogButtonList = the3dSurfaceButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
}


// **************************** Sphere Surface 2 ********************************


void SphereSurface3D2_Component::new3dOjbect() {

	if ( !((int)setting[0] & 8) &&	// must have EITHER a level dataset 
		 !((int)setting[0] & 16)  ) // OR a color dataset make a surface
		return;

	int matrixPin = -1;
	int levelPin;
	int colorPin;
	int loops;
	int lastPin = leftTerminalCount - 1;

	if ((int)setting[0] & 32)	// matrix
		matrixPin = lastPin--;
	if ((int)setting[0] & 16) {	// color
		colorPin = lastPin - 2;
		lastPin -= 3;
	}
	if ((int)setting[0] & 8)	// level 
		levelPin = lastPin--;

	loops = inputsToLoopsN(lastPin); // skip the level, color and matrix pins
	if (loops == 0)
		return;

	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matrixPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matrixPin, z, inMat[z]);
	}
	
	int levelSize;
	int levelSizeX = 0;
	int levelSizeY = 0;
	if ((int)setting[0] & 8) {	// level 
		// one level set for all surface objects
		levelSize = input[levelPin].value.size();
		if (levelSize < 4)
			return;
		if (input[levelPin].dimensions.size() != 2)
			return;
		levelSizeX = input[levelPin].dimensions[0];
		if (levelSizeX < 2)
			return;
		levelSizeY = input[levelPin].dimensions[1];
		if (levelSizeY < 2)
			return;
	}

	int colorSize;
	int colorSizeX = 0;
	int colorSizeY = 0;
	if ((int)setting[0] & 16) {	// color
		// one color set for all surface objects
		colorSize = input[colorPin].value.size();
		if (colorSize < 4)
			return;
		if (input[colorPin].dimensions.size() != 2)
			return;
		colorSizeX = input[colorPin].dimensions[0];
		if (colorSizeX < 2)
			return;
		colorSizeY = input[colorPin].dimensions[1];
		if (colorSizeY < 2)
			return;

		for (int k = 1; k < 3; k++) {
			if (colorSize != input[(long)colorPin + k].value.size())
				return;
			if (2 != input[(long)colorPin + k].dimensions.size())
				return;
			if (colorSizeX != input[(long)colorPin + k].dimensions[0])
				return;
			if (colorSizeY != input[(long)colorPin + k].dimensions[1])
				return;
		}
	}

	if (((int)setting[0] & 8) && ((int)setting[0] & 16)) {	// level and color
		if (colorSize != levelSize < 4)
			return;
		if (colorSizeX != levelSizeX)
			return;
		if (colorSizeY != levelSizeY)
			return;
	}

	if (!((int)setting[0] & 8)) {	// no level, use color
		levelSize = colorSize;
		levelSizeX = colorSizeX;
		levelSizeY = colorSizeY;
	}

	vector<vector<GL_FLOAT_4>> floatColor;
	if ((int)setting[0] & 16) {	// build color surface

		floatColor.resize(colorSizeY);
		int j4 = 0;
		//for (int jy = colorSizeY - 1; jy >= 0; jy--) {
		for (int jy = 0; jy < colorSizeY; jy++) {
			floatColor[jy].resize(colorSizeX+1L);
			for (int jx = 0; jx < colorSizeX; jx++) {
				floatColor[jy][jx].color[0] = input[colorPin].value[j4] / 256;
				floatColor[jy][jx].color[1] = input[(long)colorPin + 1L].value[j4] / 256;
				floatColor[jy][jx].color[2] = input[colorPin + 2L].value[j4] / 256;
				floatColor[jy][jx].color[3] = 1;
				j4++;
			}
			floatColor[jy][levelSizeX].color[0] = floatColor[jy][0].color[0];
			floatColor[jy][levelSizeX].color[1] = floatColor[jy][0].color[1];
			floatColor[jy][levelSizeX].color[2] = floatColor[jy][0].color[2];
			floatColor[jy][levelSizeX].color[3] = floatColor[jy][0].color[3];
		}
	}


	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputRadius = 1;
		if ((int)setting[0] & 2) {
			inputRadius = getLoopingValueD(pin++, k, 1);
			if (inputRadius <= 0)
				continue;
		}

		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 4) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}
		
		int axis = setting[1];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];

		double stepX = TWO_PI / levelSizeX;		// longitude
		double stepY = PI / (levelSizeY - 1L);	// latitude

		doublePoly.resize(levelSizeY);
		int j3 = 0;
		for (int jy = 0; jy < levelSizeY; jy++)
		{
			double ySin = sin(jy * stepY);
			double yCos = cos(jy * stepY);
			doublePoly[jy].resize(levelSizeX + 1L);
			for (int jx = 0; jx < levelSizeX; jx++)
			{
				double xSin = sin(jx * stepX);
				double xCos = cos(jx * stepX);
				double radius;
				if ((int)setting[0] & 8) // level data
					radius = inputRadius + input[levelPin].value[j3++];
				else
					radius = inputRadius;
				doublePoly[jy][jx].x = inputX + radius * xSin * ySin;
				doublePoly[jy][jx].y = inputY + radius * xCos * ySin;
				doublePoly[jy][jx].z = inputZ - radius * yCos;
			}
			doublePoly[jy][levelSizeX].x = doublePoly[jy][0].x;
			doublePoly[jy][levelSizeX].y = doublePoly[jy][0].y;
			doublePoly[jy][levelSizeX].z = doublePoly[jy][0].z;
		}
		
		if (first) {
			float Surface[4];
			Surface[0] = 1;
			Surface[1] = 1;
			Surface[2] = 1;
			Surface[3] = 1;// doesn't seem to work
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
			glLoadMatrixd(inMat);
			first = false;
		}

		glClear(GL_COLOR_BUFFER_BIT);
		//glColor3d(inputRed, inputGreen, inputBlue);

		/* plot it */
		int jxp1;
		int jyp1;
		for (int jy = 0; jy < levelSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < levelSizeX; jx++)
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v2, v1, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jy][jx]);
				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jy][jxp1]);
				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jyp1][jxp1]);
				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jyp1][jx]);
				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
		}
	}
	glPopMatrix();
}


std::vector<Button*>the3dSphereSurface2ButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool SphereSurface3D2_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> SphereSurface3D2_Component::GetPinGroups() { return vector<int>{3, 1, 1, 1, 3, 1}; }
vector<string> SphereSurface3D2_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",				// 0=Position   (1)
		"Radius",					// 1=size		(2)
		"Alpha",					// 2=alpha      (4)
		"Surface Level",				// 3=data       (8)
		"Surface Red", "Surface Green", "Surface Blue",	// 4=color      (16)
		"[4x4]"  };
}
SphereSurface3D2_Component::SphereSurface3D2_Component()
	: ThreeD_Component(L"3SphSurf2", L"3D Sphere Surface 2", squareShape, 12, 1, 0, 1)
{
	dialogButtonList = the3dSphereSurface2ButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
}


// *************************** Cylinder Surface 2 ******************************************


void CylinderSurface3D2_Component::new3dOjbect() {

	if ( !((int)setting[0] & 8) && // must have a level dataset 
		 !((int)setting[0] & 16)  )  // or a color dataset make a surface
		return;

	int matrixPin = -1;
	int levelPin;
	int colorPin;
	int loops;
	int lastPin = leftTerminalCount - 1;

	if ((int)setting[0] & 32)	// matrix
		matrixPin = lastPin--;
	if ((int)setting[0] & 16) {	// color
		colorPin = lastPin - 2;
		lastPin -= 3;
	}
	if ((int)setting[0] & 8)	// level 
		levelPin = lastPin--;

	loops = inputsToLoopsN(lastPin); // skip the level, color and matrix pins
	if (loops == 0)
		return;

	// setup the matrix
	double inMat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if ((int)setting[0] & 32) {
		if (!checkMatrixInput(matrixPin))
			return;
		for (int z = 0; z < 16; z++)
			inMat[z] = getLoopingValueD(matrixPin, z, inMat[z]);
	}

	int levelSize;
	int levelSizeX = 0;
	int levelSizeY = 0;
	if ((int)setting[0] & 8) {	// level 
		// one level set for all surface objects
		levelSize = input[levelPin].value.size();
		if (levelSize < 4)
			return;
		if (input[levelPin].dimensions.size() != 2)
			return;
		levelSizeX = input[levelPin].dimensions[0];
		if (levelSizeX < 2)
			return;
		levelSizeY = input[levelPin].dimensions[1];
		if (levelSizeY < 2)
			return;
	}

	int colorSize;
	int colorSizeX;
	int colorSizeY;
	if ((int)setting[0] & 16) {	// color
		// one color set for all surface objects
		colorSize = input[colorPin].value.size();
		if (colorSize < 4)
			return;
		if (input[colorPin].dimensions.size() != 2)
			return;
		colorSizeX = input[colorPin].dimensions[0];
		if (colorSizeX < 2)
			return;
		colorSizeY = input[colorPin].dimensions[1];
		if (colorSizeY < 2)
			return;

		for (int k = 1; k < 3; k++) {
			if (colorSize != input[colorPin + k].value.size())
				return;
			if (2 != input[colorPin + k].dimensions.size())
				return;
			if (colorSizeX != input[colorPin + k].dimensions[0])
				return;
			if (colorSizeY != input[colorPin + k].dimensions[1])
				return;
		}
	}
	
	if (((int)setting[0] & 8) && ((int)setting[0] & 16)) {	// level and color
		if (colorSize != levelSize < 4)
			return;
		if (colorSizeX != levelSizeX)
			return;
		if (colorSizeY != levelSizeY)
			return;
	}

	if (!((int)setting[0] & 8)) {	// no level, use color
		levelSize = colorSize;
		levelSizeX = colorSizeX;
		levelSizeY = colorSizeY;
	}

	vector<vector<GL_FLOAT_4>> floatColor;
	if ((int)setting[0] & 16) {	// build color surface

		floatColor.resize(colorSizeY);
		int j4 = 0;
		for (int jy = colorSizeY - 1; jy >= 0; jy--) {
			floatColor[jy].resize(colorSizeX + 1);
			for (int jx = 0; jx < colorSizeX; jx++) {
				floatColor[jy][jx].color[0] = input[colorPin].value[j4] / 256;
				floatColor[jy][jx].color[1] = input[colorPin + 1].value[j4] / 256;
				floatColor[jy][jx].color[2] = input[colorPin + 2].value[j4] / 256;
				floatColor[jy][jx].color[3] = 1;
				j4++;
			}
			floatColor[jy][levelSizeX].color[0] = floatColor[jy][0].color[0];
			floatColor[jy][levelSizeX].color[1] = floatColor[jy][0].color[1];
			floatColor[jy][levelSizeX].color[2] = floatColor[jy][0].color[2];
			floatColor[jy][levelSizeX].color[3] = floatColor[jy][0].color[3];
		}
	}

	bool first = true;
	for (int k = 0; k < loops; k++) {
		int pin = 0;

		double inputX = 0;
		double inputY = 0;
		double inputZ = 0;
		if ((int)setting[0] & 1) {
			inputX = getLoopingValueD(pin++, k, 0);
			inputY = getLoopingValueD(pin++, k, 0);
			inputZ = getLoopingValueD(pin++, k, 0);
		}

		double inputRadius = 1;
		double inputHeight = 1;
		if ((int)setting[0] & 2) {
			inputRadius = getLoopingValueD(pin++, k, 1);
			if (inputRadius <= 0)
				continue;
			inputHeight = getLoopingValueD(pin++, k, 1);
			if (inputHeight <= 0)
				continue;
		}
		
		float inputAlpha = 1.0; // doesn't seem to work
		if ((int)setting[0] & 4) {
			inputAlpha = getLoopingValueD(pin++, k, 1);
			if (inputAlpha <= 0)
				continue;
		}

		int axis = setting[1];
		vector<vector<DOUBLE_3PNT>>	doublePoly;
		double	v1[3], v2[3], normal[3];
		double stepX = TWO_PI / levelSizeX;	// longitude
		double stepY = inputHeight / levelSizeY;

		doublePoly.resize(levelSizeY);
		int j3 = 0;
		for (int jy = 0; jy < levelSizeY; jy++)
		{
			double y = jy * stepY;

			doublePoly[jy].resize(levelSizeX + 1);
			for (int jx = 0; jx < levelSizeX; jx++)
			{
				double xSin = sin(jx * stepX);
				double xCos = cos(jx * stepX);
				double radius;
				if ((int)setting[0] & 8) // level data
					radius = inputRadius + input[levelPin].value[j3++];
				else
					radius = inputRadius;
				doublePoly[jy][jx].x = inputX + radius * xSin;
				doublePoly[jy][jx].y = inputY + radius * xCos;
				doublePoly[jy][jx].z = inputZ + y;
			}
			doublePoly[jy][levelSizeX].x = doublePoly[jy][0].x;
			doublePoly[jy][levelSizeX].y = doublePoly[jy][0].y;
			doublePoly[jy][levelSizeX].z = doublePoly[jy][0].z;
		}
		
		if (first) {
			float Surface[4];
			Surface[0] = 1;
			Surface[1] = 1;
			Surface[2] = 1;
			Surface[3] = 1;// doesn't seem to work
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Surface);
			glLoadMatrixd(inMat);
			first = false;
		}

		glClear(GL_COLOR_BUFFER_BIT);
		//glColor3d(inputRed, inputGreen, inputBlue);

		/* plot it */
		int jxp1;
		int jyp1;
		for (int jy = 0; jy < levelSizeY - 1; jy++)
		{
			jyp1 = jy + 1;
			for (int jx = 0; jx < levelSizeX; jx++)
			{
				jxp1 = jx + 1;

				/* v2 is 2 - 3 */
				v2[0] = doublePoly[jyp1][jx].x - doublePoly[jy][jx].x;
				v2[1] = doublePoly[jyp1][jx].y - doublePoly[jy][jx].y;
				v2[2] = doublePoly[jyp1][jx].z - doublePoly[jy][jx].z;

				/* v1 is 0 - 3 */
				v1[0] = doublePoly[jy][jxp1].x - doublePoly[jy][jx].x;
				v1[1] = doublePoly[jy][jxp1].y - doublePoly[jy][jx].y;
				v1[2] = doublePoly[jy][jxp1].z - doublePoly[jy][jx].z;

				normCrossProduct(v2, v1, normal);

				glBegin(GL_POLYGON);

				glNormal3dv(normal);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jy][jx]);
				glVertex3d(doublePoly[jy][jx].x,
					doublePoly[jy][jx].y,
					doublePoly[jy][jx].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jy][jxp1]);
				glVertex3d(doublePoly[jy][jxp1].x,
					doublePoly[jy][jxp1].y,
					doublePoly[jy][jxp1].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jyp1][jxp1]);
				glVertex3d(doublePoly[jyp1][jxp1].x,
					doublePoly[jyp1][jxp1].y,
					doublePoly[jyp1][jxp1].z);

				if ((int)setting[0] & 16) 	// color
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (GLfloat*)&floatColor[jyp1][jx]);
				glVertex3d(doublePoly[jyp1][jx].x,
					doublePoly[jyp1][jx].y,
					doublePoly[jyp1][jx].z);

				glEnd();
			}
		}
	}
	glPopMatrix();
}


std::vector<Button*>the3dCylinderSurface2ButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool CylinderSurface3D2_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}

vector<int> CylinderSurface3D2_Component::GetPinGroups() { return vector<int>{3, 2, 1, 1, 3, 1}; }
vector<string> CylinderSurface3D2_Component::GetPinLabels() {
	return vector<string>{
		"X", "Y", "Z",				// 0=Position   (1)
		"Radius", "Height", 		// 1=size		(2)
		"Alpha",					// 2=alpha      (4)
		"Surface Level",				// 3=data       (8)
		"Surface Red", "Surface Green", "Surface Blue",	// 4=color      (16)
		"[4x4]"  };
}
CylinderSurface3D2_Component::CylinderSurface3D2_Component()
	: ThreeD_Component(L"3CylSurf2", L"3D Cylinder Surface 2", squareShape, 11, 1, 0, 1)
{
	dialogButtonList = the3dCylinderSurface2ButtonList;
	width = 40;
	setSetting(0, 63);
	setSetting(1, 0); // axis maybe future??
	pinMaskIndex = 0;
	//double matrix[16];
	//float surface[4];
	vector<vector<DOUBLE_3PNT>>	doublePoly;
}



// *************************** View 3D ******************************************


//void View3D_Component::new3dOjbect() {
//		
//	//global_threeDScale = getLoopingValueD(0, 0, 0);
//	//global_EyeY = getLoopingValueD(1, 0, 0);
//	//global_EyeZ = getLoopingValueD(2, 0, 3);
//	//global_CenterX = getLoopingValueD(3, 0, 0);
//	//global_CenterY = getLoopingValueD(4, 0, 0);
//	//global_CenterZ = getLoopingValueD(5, 0, 0);
//	//global_UpX = getLoopingValueD(6, 0, 0);
//	//global_UpY = getLoopingValueD(7, 0, 1);
//	//global_UpZ = getLoopingValueD(8, 0, 0);
//	//
//}
//
//
//View3D_Component::View3D_Component()
//	: ThreeD_Component(L"3View", L"3D View", squareShape, 9, 1, 0, 1)
//{
//	width = 40;
//	setLeftLabels(4, "Eye X", "Eye Y", "Eye Z", "Center X", "Center Y", "Center Z", "Up X", "Up Y", "Up Z");
//}


// *************************** Light 3D ******************************************

int findNextAvailableLight(Sheet* pg) {
	vector < bool> usedLights;
	usedLights.resize(MY_GL_MAX_LIGHTS);
	for (int j = 0; j < MY_GL_MAX_LIGHTS; j++)
		usedLights[j] = false;
	usedLights[0] = true;

	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		if (pg->dgrm.cL[i]->type == L"3Light") {
			int j = pg->dgrm.cL[i]->setting[1];
			usedLights[j] = true;
		}
	}

	for (int j = 0; j < MY_GL_MAX_LIGHTS; j++)
		if (!usedLights[j])
			return j;

	return -1; // all are already used
}
void clamp(GLfloat* value, GLfloat min, GLfloat max, int count) {
	for (int j = 0; j < count; j++) {
		if (value[j] < min)
			value[j] = min;
		if (value[j] > max)
			value[j] = max;
	}
}

int Light3D_Component::method2(Sheet* pg) {

	int light = setting[1]; 
	if (light < 1)
		return 0;
	if (light >= MY_GL_MAX_LIGHTS)
		return 0;

	int pin = 0;
	if ((int)setting[0] & 1) {
		pg->light_ambient[light][0] = getLoopingValueD(pin++, 0, 0);
		pg->light_ambient[light][1] = getLoopingValueD(pin++, 0, 0);
		pg->light_ambient[light][2] = getLoopingValueD(pin++, 0, 0);
		pg->light_ambient[light][3] = getLoopingValueD(pin++, 0, 1);
		//clamp(ambient, 0.0, 1.0, 4);
	}

	if ((int)setting[0] & 2) {
		pg->light_diffuse[light][0] = getLoopingValueD(pin++, 0, 0);
		pg->light_diffuse[light][1] = getLoopingValueD(pin++, 0, 0);
		pg->light_diffuse[light][2] = getLoopingValueD(pin++, 0, 0);
		pg->light_diffuse[light][3] = getLoopingValueD(pin++, 0, 1);
		//clamp(diffuse, 0.0, 1.0, 4);
	}

	if ((int)setting[0] & 4) {
		pg->light_specular[light][0] = getLoopingValueD(pin++, 0, 0);
		pg->light_specular[light][1] = getLoopingValueD(pin++, 0, 0);
		pg->light_specular[light][2] = getLoopingValueD(pin++, 0, 0);
		pg->light_specular[light][3] = getLoopingValueD(pin++, 0, 1);
		//clamp(specular, 0.0, 1.0, 4);
	}

	if ((int)setting[0] & 8) {
		pg->light_position[light][0] = getLoopingValueD(pin++, 0, 0);
		pg->light_position[light][1] = getLoopingValueD(pin++, 0, 0);
		pg->light_position[light][2] = getLoopingValueD(pin++, 0, 1);
		pg->light_position[light][3] = getLoopingValueD(pin++, 0, 0);
	}

	if ((int)setting[0] & 16) {
		pg->light_direction[light][0] = getLoopingValueD(pin++, 0, 0);
		pg->light_direction[light][1] = getLoopingValueD(pin++, 0, 0);
		pg->light_direction[light][2] = getLoopingValueD(pin++, 0, 1);
	}

	if ((int)setting[0] & 32) {
		pg->light_exponent[light] = getLoopingValueD(pin++, 0, 0);
	}

	if ((int)setting[0] & 64) {
		pg->light_cutoff[light] = getLoopingValueD(pin++, 0, 0);
	}

	pg->light_bitmask[light] = setting[0];
	pg->light_enable[light] = true;
	pg->update3Dview = true;

	return 0;
}
Light3D_Component::~Light3D_Component() {
	int light = setting[1];
	if (light < GL_LIGHT0)
		return;
	if (light >= MY_GL_MAX_LIGHTS)
		return;
	page->light_enable[light] = false;
	page->update3Dview = true;
}

void Light3D_Component::completion(Sheet* pg, double* doubleValues, bool* boolValues, int radioValue) {
	
	setting[1] = radioValue + 1; // light number

	onLoad(pg);
}
void doubleClickLight3D(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels;
	vector<double> doubleValues;

	vector<string> checkLabels;
	vector<bool> checkValues;

	vector<string> radioLabels = { "Light 1", "Light 2", "Light 3", "Light 4", "Light 5", "Light 6", "Light 7", "Light 8", "Light 9" };
	int radioIndex = pg->dgrm.cL[i]->setting[1] - 1;

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, radioLabels, radioIndex, NULL);
}
std::vector<Button*>the3dLightButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClickLight3D) };

bool Light3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	pg->update3Dview = true;
	return false;
}

vector<int> Light3D_Component::GetPinGroups() { return vector<int>{4, 4, 4, 4, 3, 1, 1 }; }
vector<string> Light3D_Component::GetPinLabels() {
	return vector<string>{
		"Ambient Red", "Ambient Green", "Ambient Blue", "Ambient Alpha",	// 0=GL_AMBIENT			(1)
		"Diffuse Red", "Diffuse Green", "Diffuse Blue", "Diffuse Alpha",	// 1=GL_DIFFUSE			(2)
		"Specular Red", "Specular Green", "Specular Blue", "Specular Alpha",// 2=GL_SPECULAR		(4)
		"Position X", "Position Y", "Position Z", "Position W",				// 3=GL_POSITION		(8)
		"Direction X", "Direction Y", "Direction Z", 						// 4=GL_SPOT_DIRECTION	(16)
		"Spot Exponent",													// 5=GL_SPOT_EXPONENT	(32)
		"Spot Cutoff"  };
}
Light3D_Component::Light3D_Component(Sheet* pg)
	: Component(L"3Light", L"3D Light", squareShape, 4, 1, 0, 1)
{
	dialogButtonList = the3dLightButtonList;
	width = 40;
	pinMaskIndex = 0;
	setSetting(0, 1);
	setSetting(1, 1);// findNextAvailableLight(pg)); // sequence of lights from 0 to GL_MAX_LIGHTS - 1.
}

// **************************** 3D Get Direction ****************************

double clamp(double x) {
	return min(max(x, -1), 1);
}
int GetDirection3D_Component::method2(Sheet* pg) {
	if (!checkMatrixInput(0))
		return clearOutputs();
	double mat[16] = { 1,0,0,0,   0,1,0,0,  0,0,1,0, 0,0,0,1 };
	if (input[0].drivenByComponent != -1) {
		for (int z = 0; z < 16; z++)
			mat[z] = getLoopingValueD(0, z, mat[z]);
	}
	
	// output the angles
	double m11 = mat[0], m12 = mat[4], m13 = mat[8];
	double m21 = mat[1], m22 = mat[5], m23 = mat[9];
	double m31 = mat[2], m32 = mat[6], m33 = mat[10];

	double ax;
	double ay = asin(clamp(m13));
	double az;
	if (abs(m13) < 0.99999) {
		ax = atan2(-m23, m33);
		az = atan2(-m12, m11);
	}
	else {
		ax = atan2(m32, m22);
		az = 0;
	}
	
	output[0].value[0] = ax;
	output[1].value[0] = ay;
	output[2].value[0] = az;

	// output a matrix with no rotation
	output[3].value.resize(16);
	output[3].dimensions.resize(2);
	output[3].dimensions[0] = 4;
	output[3].dimensions[1] = 4;
	for (int k = 0; k < 16; k++)
		output[3].value[k] = 0;

	// calculate the scales
	double scale[3];
	for (int k = 0; k < 3; k++)
		scale[k] = sqrt(mat[k] * mat[k] + mat[k + 4] * mat[k + 4] + mat[k + 8] * mat[k + 8]);
	// finish the matrix
	output[3].value[0] = scale[0];
	output[3].value[5] = scale[1];
	output[3].value[10] = scale[2];
	output[3].value[12] = mat[12];
	output[3].value[13] = mat[13];
	output[3].value[14] = mat[14];
	output[3].value[15] = mat[15];

	return 1;
}
GetDirection3D_Component::GetDirection3D_Component(Sheet* pg)
	: Component(L"3UNROT", L"3D Get Direction", squareShape, 1, 1, 4, 1)
{
	setLeftLabels(3, "[4x4]");
	setRightLabels(3, "X angle", "Y angle", "Z angle", "[4x4]");
	width = 40;// calcComponentWidth();
}

// ********************************** 3D Bridge ***************************
int Bridge3D_Component::method2(Sheet* pg) {

	output[3].value.resize(16);
	output[3].dimensions.resize(2);
	output[3].dimensions[0] = 4;
	output[3].dimensions[1] = 4;

	double x0 = getLoopingValueD(0, 0, 0);
	double y0 = getLoopingValueD(1, 0, 0);
	double z0 = getLoopingValueD(2, 0, 0);
						   
	double x1 = getLoopingValueD(3, 0, 0);
	double y1 = getLoopingValueD(4, 0, 0);
	double z1 = getLoopingValueD(5, 0, 0);

	double xc = (x0 + x1) / 2;
	double yc = (y0 + y1) / 2;
	double zc = (z0 + z1) / 2;

	double dx = x0 - x1;
	double dy = y0 - y1;
	double dz = z0 - z1;
	double xSize = sqrt(dx * dx + dy * dy + dz * dz);
	output[0].value[0] = xSize;

	// first shift
	double translationMatrix[16];
	matrixTranslateXYZ(xc, yc, zc, translationMatrix);

	double azimuth = atan2(dy, -dx);
	output[1].value[0] = azimuth;

	double rotationMatrixZ[16];
	matrixRotationZ( -azimuth, rotationMatrixZ);

	double resultMatrix1[16];
	matrixMultiply_4x4(translationMatrix, rotationMatrixZ, resultMatrix1);

	double hypotinuse = sqrt(dx * dx + dy * dy);
	double elevation = atan2(-dz, hypotinuse);
	output[2].value[0] = elevation;

	double rotationMatrixY[16];
	matrixRotationY(-elevation, rotationMatrixY);

	double resultMatrix2[16];
	matrixMultiply_4x4(resultMatrix1, rotationMatrixY, resultMatrix2);

	
	for (int k = 0; k < 16; k++)
		output[3].value[k] = resultMatrix2[k];

	return 1;
}
Bridge3D_Component::Bridge3D_Component(Sheet* pg)
	: Component(L"3BRIDGE", L"3D Bridge", squareShape, 6, 1, 4, 1)
{
	setLeftLabels(3, "X0 position", "Y0 position", "Z0 position", "X1 position", "Y1 position", "Z1 position");
	setRightLabels(3, "X size", "Azamuth", "Elevation", "[4x4]");
	width = 60;// calcComponentWidth(nC);
}

// ************************* View Control 3D ***************************************


int ViewControl3D_Component::method2(Sheet* pg) {
	if (pg->GLFW_window == NULL)
		return false;
	if (!checkValueInputs())
		return false;

	int result = 0;
	int pin = 0;

	double inputScale = 1;
	if ((int)setting[0] & 1) 
		inputScale = getLoopingValueD(pin++, 0, 1);
	double oldScale = pg->canvas3Scale;
	if (input[0].drivenByComponent != -1) {
		if (input[0].value[0] < 0.000000001)
			pg->canvas3Scale = 0.000000001;
		else
			pg->canvas3Scale = inputScale;
	}
	else
		pg->canvas3Scale = 1;
	if (pg->canvas3Scale != oldScale)
		result |= 4;


	double inputXoffset = 0;
	double inputYoffset = 0;
	double inputZoffset = 0;
	if ((int)setting[0] & 2) {
		inputXoffset = getLoopingValueD(pin++, 0, 0);
		inputYoffset = getLoopingValueD(pin++, 0, 0);
		inputZoffset = getLoopingValueD(pin++, 0, 0);
	}
	if (pg->canvas3xOffset != inputXoffset) {
		pg->canvas3xOffset = inputXoffset;
		result |= 4;
	}
	if (pg->canvas3yOffset != inputYoffset) {
		pg->canvas3yOffset = inputYoffset;
		result |= 4;
	}
	if (pg->canvas3zOffset != inputZoffset) {
		pg->canvas3zOffset = inputZoffset;
		result |= 4;
	}

	double inputXrotate = 0;
	double inputYrotate = 0;
	double inputZrotate = 0;
	if ((int)setting[0] & 4) {
		inputXrotate = getLoopingValueD(pin++, 0, 0);
		inputYrotate = getLoopingValueD(pin++, 0, 0);
		inputZrotate = getLoopingValueD(pin++, 0, 0);
	}
	inputXrotate *= RADIANS_TO_DEGREES;
	inputYrotate *= RADIANS_TO_DEGREES;
	inputZrotate *= RADIANS_TO_DEGREES;
	if (pg->canvas3xRotate != inputXrotate) {
		pg->canvas3xRotate = inputXrotate;
		result |= 4;
	}
	if (pg->canvas3yRotate != inputYrotate) {
		pg->canvas3yRotate = inputYrotate;
		result |= 4;
	}
	if (pg->canvas3zRotate != inputZrotate) {
		pg->canvas3zRotate = inputZrotate;
		result |= 4;
	}
	
	float inputRed = 0;
	float inputGreen = 0;
	float inputBlue = 0;
	if ((int)setting[0] & 8) {
		inputRed =   getLoopingValueD(pin++, 0, 0);
		inputGreen = getLoopingValueD(pin++, 0, 0);
		inputBlue =  getLoopingValueD(pin++, 0, 0);
	}
	inputRed /= 255;
	inputGreen /= 255;
	inputBlue /= 255;

	if (pg->canvas3Red != inputRed) {
		pg->canvas3Red = inputRed;
		result |= 4;
	}
	if (pg->canvas3Green != inputGreen) {
		pg->canvas3Green = inputGreen;
		result |= 4;
	}
	if (pg->canvas3Blue != inputBlue) {
		pg->canvas3Blue = inputBlue;
		result |= 4;
	}
	
	int width;
	int height;
	glfwGetWindowSize(pg->GLFW_window, &width, &height );
	if (output[0].value[0] != width) {
		output[0].value[0] = width;
		result |= 1;
	}
	if (output[1].value[0] != height) {
		output[1].value[0] = height;
		result |= 1;
	}

	if (result & 4)
		pg->update3Dview = true;
	return result & 3;
}
bool ViewControl3D_Component::dial2d(Sheet* pg) {
/*	if (pg->pCanvasWindow == NULL)
		return false;*/
	return method2(pg);
}
ViewControl3D_Component::~ViewControl3D_Component() {
	if ((page->canvas3Scale == 1) &&
		(page->canvas3xOffset == 0) &&
		(page->canvas3yOffset == 0) &&
		(page->canvas3zOffset == 0) &&
		(page->canvas3xRotate == 0) &&
		(page->canvas3yRotate == 0) &&
		(page->canvas3zRotate == 0) &&
		(page->canvas3Red == 0)     &&
		(page->canvas3Green == 0)   &&
		(page->canvas3Blue == 0)    )
		return;

	page->canvas3Scale = 1;
	page->canvas3xOffset = 0;
	page->canvas3yOffset = 0;
	page->canvas3zOffset = 0;

	page->canvas3xRotate = 0;
	page->canvas3yRotate = 0;
	page->canvas3zRotate = 0;

	page->canvas3Red = 0;
	page->canvas3Green = 0;
	page->canvas3Blue = 0;

	// for some reason method will be called next - override the input values
	int pin = 0;
	if ((int)setting[0] & 1)
		if (input[pin].value.size())
			input[pin++].value[0] = 1;

	if ((int)setting[0] & 2) {
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
	}
	if ((int)setting[0] & 4) {
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
	}
	if ((int)setting[0] & 8) {
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
		if (input[pin].value.size())
			input[pin++].value[0] = 0;
	}

	page->update3Dview = true;
	
	return;
}

std::vector<Button*>the3dViewButtonList = { 
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons) };

bool ViewControl3D_Component::onLoad(Sheet* pg) {
	buildPinLabels();
	return false;
}
vector<int> ViewControl3D_Component::GetPinGroups() { return vector<int>{ 1, 3, 3, 3 }; }
vector<string> ViewControl3D_Component::GetPinLabels() {
	return vector<string>{
		"Scale Factor",						// 0=Position    (1)
		"X Offset", "Y Offset", "Z Offset",	// 1=offset      (2)
		"X Rotate", "Y Rotate", "Z Rotate",	// 2=Rotate      (4)
		"Red", "Green", "Blue",				// 3=back color  (8)
	 };
}
ViewControl3D_Component::ViewControl3D_Component()
	: Component(L"3dDisp", L"3D View Control", squareShape, 7, 1, 2, 1)
{
	dialogButtonList = the3dViewButtonList;
	pinMaskIndex = 0;
	setSetting(0, 4);
	setRightLabels(3, "Width", "Height");
	width = 50;
}
