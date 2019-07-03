#pragma once
#include "stdafx.h"
#include <vector>
#include "structs.h"
#include "buildComponent.h"
#include "Wire.h"
#include "Button.h"
#include <GLFW/glfw3.h>

class Component;
class Button;
class CanvasWindow;


class Diagram {
public:
	std::vector<Component*> cL;
	std::vector<Wire*> wL;
};

class InnerSheet {
public:
	Diagram dgrm;
	std::vector<Diagram*> saveDiagramBuffer;
	int saveIndex = -1;
	int cXo; // window X offset
	int cYo; // window Y offset
	double cs; //  window scale
	bool isDirty;
	bool isTimeBased;
	InnerSheet();
	std::string	name;
};

class Sheet { // the page of code
public:
	int cXo; // window X offset
	int cYo; // window Y offset
	double cs; // window scale
	HWND hWnd; // the main window handle
	HDC hDC; // main window drawing context
	int workspaceLeft;
	int workspaceTop;
	int workspaceWidth;
	int workspaceHeight;

	HWND hWnd2; // 2D canvas 2 handle
	HDC hDC2; // 2D canvas 2 drawing context
	int canvas2Left;
	int canvas2Top;
	int canvas2Width;
	int canvas2Height;
	double canvas2Scale;
	double canvas2xOffset;
	double canvas2yOffset;
	int canvas2Red = 255;
	int canvas2Green = 255;
	int canvas2Blue = 255;

	Diagram dgrm;
	int visibleSheet;

	LPTSTR filePath;

	double systemTime;
	double timeStep; 
	UINT timerID;
	int visibleButtonGroup;
	bool draggingCanvas;

	int draggingComponent;
	int crankingComponent;

	CanvasWindow *pCanvasWindow;
	int threeDwindowHandle;

	bool isDirty;

	//	undoRingBuffer : undoRingBuffer, undoRingNext : 0, undoRingSize : 0, redoBuffer : '',

	int currentAttributionKey;
	Sheet();

	int modalDialogComponent;
	int activeDialogComponent;

	int mouseOffsetX;
	int mouseOffsetY;

	Button* stickyComponentsButton;
	Button* showAxisButton;
	Button* showGridButton;

	bool selectionRectangleInProgress;
	int selectionRectangleStartX;
	int selectionRectangleStartY;
	int selectionRectangleEndX;
	int selectionRectangleEndY;
	bool draggingSelectedComponents;

	int draggingWire;		// -1=not dragging, other is wire index
	int dragTerminal;
	int mouseDownDX;
	int mouseDownDY;

	int lastTrigMemory; // radians

	std::vector<Button*> buttonList;
	std::vector<int> buttonGroup;

	std::vector<Button*> activeDialogButtonList;
	std::vector<Button*> componentButtonList;

	std::string lastBussName;

	int activeDialogOffsetX;
	int activeDialogOffsetY;

	std::vector<Button*> frameTopButtons;
	std::vector<Button*> frameBottomButtons;
	int mainNameIndex;
	int subroutineNameIndex;

	GLFWwindow* GLFW_window = NULL;
	bool update3Dview = true;
	float canvas3Scale = 1;
	float canvas3xOffset = 0;
	float canvas3yOffset = 0;
	float canvas3zOffset = 0;
	double canvas3xRotate = 0;
	double canvas3yRotate = 0;
	double canvas3zRotate = 0;
	float canvas3Red = 0;
	float canvas3Green = 0;
	float canvas3Blue = 0;

	GLfloat light_ambient[MY_GL_MAX_LIGHTS][4];
	GLfloat light_diffuse[MY_GL_MAX_LIGHTS][4];
	GLfloat light_specular[MY_GL_MAX_LIGHTS][4];
	GLfloat light_position[MY_GL_MAX_LIGHTS][4];
	GLfloat light_direction[MY_GL_MAX_LIGHTS][3];
	GLfloat light_exponent[MY_GL_MAX_LIGHTS];
	GLfloat light_cutoff[MY_GL_MAX_LIGHTS];
	int light_bitmask[MY_GL_MAX_LIGHTS];
	bool light_enable[MY_GL_MAX_LIGHTS];

	int modelVersion;
};
