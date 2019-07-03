#include "stdafx.h"
#include "structs.h"
#include "Sheet.h"



InnerSheet::InnerSheet() {
	cXo = 0;
	cYo = 0;
	cs = 1.0;
	isDirty = false;
};

Sheet::Sheet() {
	cXo = 0;
	cYo = 0;
	cs = 1.0; // cs is componentScale = 1.0;
	visibleSheet = 0; // main

	isDirty = false;

	canvas2Width = 100;
	canvas2Height = 100;
	canvas2Scale = 1.0;
	canvas2xOffset = 0;
	canvas2yOffset = 0;
	pCanvasWindow = NULL;

	timerID = 0;

	visibleButtonGroup = -1;

	modalDialogComponent = -1;
	activeDialogComponent = -1;

	mouseOffsetX = 0;
	mouseOffsetY = 0;

	activeDialogOffsetX = 0;
	activeDialogOffsetY = 0;
	draggingCanvas = false;

	selectionRectangleInProgress = false;
	draggingSelectedComponents = false;

	draggingComponent = -1;
	crankingComponent = -1;
	draggingWire = -1;		// -1=not dragging, other is wire index
	dragTerminal = 0;

	mouseDownDX = 0;
	mouseDownDY = 0;

	lastTrigMemory = 0; // 0 is radians


	//var crank2dComp = [-1, -1, -1, -1, -1];
	//var undoRingBuffer = [];
	//var attributionKeyQuidList = [];
	
	//	name : "", crank2dComp : crank2dComp, sL : sl,
	//	undoRingBuffer : undoRingBuffer, undoRingNext : 0, undoRingSize : 0, redoBuffer : '',
	//	attributionKeyQuidList : attributionKeyQuidList, currentAttributionKey : 0
}