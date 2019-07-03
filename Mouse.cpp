#include "stdafx.h"
#include "windowsx.h"
#include "structs.h"
#include "Mouse.h"
#include "FrameButtons.h"
#include "ButtonGroups.h"
#include "Sheet.h"
#include "buildComponent.h"
#include "Time.h"
#include "Text.h"
#include <string>
#include "ComponentButtons.h"
#include "ScreenDialog.h"
#include "StickyConnections.h"
#include "MethodComponents.h"
#include "Button.h"
#include "MST01.h"
#include "Fileio.h"

#pragma warning (disable: 4267)
#pragma warning (disable: 4244)

std::string debugString;
std::string keySearchText;


extern PATH findWireTerminal(int x, int y);



void enableDimButtons() {}
extern bool DoSelectedComponentButtons(Sheet* pg, POS aPos);

extern Button* magnetComponentsButton;
extern vector<int> connectedWireNumber;
extern Button* keyboardSearch;

void showCoordinates(HDC hDC, int x, int y, LPARAM lParam) {

	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);

	LPCTSTR aString = L"x = ";
	TextOut(hDC, x, y, aString, _tcslen(aString));
}





double calcLineLength( int x1, int y1, int x2, int y2) {
	double dx, dy;

	dx = x1 - x2;
	dy = y1 - y2;

	return sqrt(dx * dx + dy * dy);
}


int calculateDistancePoint2Line(int xp, int yp, int x1, int y1, int x2, int y2) {
	double m, b;
	double A, B, C;
	double dist;

	if (x2 == x1)    /* case of end points forming a vertical line */
	{
		if ((yp > y1) && (yp > y2)) /* point is below line segment */
		{
			/* pick the nearest point */
			if (y1 < y2) {
				/* y2 is below y1 and therefore closer */
				return calcLineLength(xp, yp, x2, y2);
			}
			else {
				/* y1 is below y2 and therefore closer */
				return calcLineLength(xp, yp, x1, y1);
			}
		}
		else if ((yp < y1) && (yp < y2)) /* point is above line segment */
		{
			/* pick the nearest point */
			if (y1 < y2) {
				/* y1 is above y2 and therefore closer */
				return calcLineLength(xp, yp, x1, y1);
			}
			else {
				/* y2 is above y1 and therefore closer */
				return calcLineLength(xp, yp, x2, y2);
			}
		}

		/* Point is vertically between the end points of the line segment */
		/* so we return the horizontal distance to line segment. */
		return abs(x1 - xp);
	}

	/* from 2 points on line get slope & intercept */
	m = (double)(y2 - y1) / (double)(x2 - x1);
	b = y1 - m * x1;

	/* convert to form: Ax + By + C = 0 */
	A = m;
	B = -1.0;
	C = b;

	/* calculate distance */
	dist = (A * xp + B * yp + C) / sqrt((A * A) + (B * B));
	if (dist < 0.0)
		dist = -dist;

	return dist;
}

bool checkWireSegment(Sheet* pg, int x, int y, int i, int j0, int j1) {
	if (abs(pg->dgrm.wL[i]->x[j0] - pg->dgrm.wL[i]->x[j1]) < TERMINAL_SIZE)  //  horizontal wire
	{
		if (x < pg->dgrm.wL[i]->x[j0] - TERMINAL_SIZE && x < pg->dgrm.wL[i]->x[j1] - TERMINAL_SIZE)
			return false;
		if (x > pg->dgrm.wL[i]->x[j0] + TERMINAL_SIZE && x > pg->dgrm.wL[i]->x[j1] + TERMINAL_SIZE)
			return false;
	}
	else {
		// are we between the end points of this segment? [j] to [j+1]?
		if (x < pg->dgrm.wL[i]->x[j0] && x < pg->dgrm.wL[i]->x[j1])
			return false;
		if (x > pg->dgrm.wL[i]->x[j0] && x > pg->dgrm.wL[i]->x[j1])
			return false;
	}
	if (abs(pg->dgrm.wL[i]->y[j0] - pg->dgrm.wL[i]->y[j1]) < TERMINAL_SIZE) // vertical wire
	{
		if (y < pg->dgrm.wL[i]->y[j0] - TERMINAL_SIZE && y < pg->dgrm.wL[i]->y[j1] - TERMINAL_SIZE)
			return false;
		if (y > pg->dgrm.wL[i]->y[j0] + TERMINAL_SIZE && y > pg->dgrm.wL[i]->y[j1] + TERMINAL_SIZE)
			return false;
	}
	else {
		// are we between the end points of this segment? [j] to [j+1]?
		if (y < pg->dgrm.wL[i]->y[j0] && y < pg->dgrm.wL[i]->y[j1])
			return false;
		if (y > pg->dgrm.wL[i]->y[j0] && y > pg->dgrm.wL[i]->y[j1])
			return false;
	}

	if (calculateDistancePoint2Line(x, y,
		pg->dgrm.wL[i]->x[j0], pg->dgrm.wL[i]->y[j0],
		pg->dgrm.wL[i]->x[j1], pg->dgrm.wL[i]->y[j1]) < TERMINAL_SIZE)
		return true;

	return false;
}



int MAG_DIST = 3 * TERMINAL_SIZE * TERMINAL_SIZE;

POS snapComponentToTerminal(Sheet* pg, int dragComp, int posx, int posy) {
	int dist;
	int tx, ty;

	vector<int> lx;
	vector<int> ly;
	int lCount;

	vector<int> rx;
	vector<int> ry;
	int rCount;
	int j;
	POS result;

	if (dragComp >= 0) {
		// first get all the terminal positions of the component we are dragging - we check them not the mouse position
		lCount = pg->dgrm.cL[dragComp]->leftTerminalCount;
		for (int it = 0; it < lCount; it++) {
			lx.push_back(pg->dgrm.cL[dragComp]->leftTerminalX(it));
			ly.push_back(pg->dgrm.cL[dragComp]->leftTerminalY(it));
		}

		rCount = pg->dgrm.cL[dragComp]->rightTerminalCount;
		for (int it = 0; it < rCount; it++) {
			rx.push_back(pg->dgrm.cL[dragComp]->rightTerminalX(it));
			ry.push_back(pg->dgrm.cL[dragComp]->rightTerminalY(it));
		}

		// check for components

		for (int ic = 0; ic < pg->dgrm.cL.size(); ic++) {
			if (ic == dragComp)
				continue;

			// first see if a right terminal of the component we are dragging
			// is near a left terminal on some other component
			for (int it = 0; it < pg->dgrm.cL[ic]->leftTerminalCount; it++) {
				tx = pg->dgrm.cL[ic]->leftTerminalX(it);
				ty = pg->dgrm.cL[ic]->leftTerminalY(it);

				for (j = 0; j < rCount; j++) {
					dist = (rx[j] - tx) * (rx[j] - tx)
						+ (ry[j] - ty) * (ry[j] - ty);
					if (dist < MAG_DIST) {
						result.x = posx + tx - rx[j];
						result.y = posy + ty - ry[j];
						return result;
					}
				}
			}

			// then see if a left terminal of the component we are dragging
			// is near a right terminal on some other component
			for (int it = 0; it < pg->dgrm.cL[ic]->rightTerminalCount; it++) {
				tx = pg->dgrm.cL[ic]->rightTerminalX(it);
				ty = pg->dgrm.cL[ic]->rightTerminalY(it);

				for (j = 0; j < lCount; j++) {
					dist = (lx[j] - tx) * (lx[j] - tx)
						+ (ly[j] - ty) * (ly[j] - ty);
					if (dist < MAG_DIST) {
						result.x = posx + tx - lx[j]; 
						result.y = posy + ty - ly[j];
						return result;// modified: true
					}
				}
			}
		}


		// check for wires
		for (int iw = 0; iw < pg->dgrm.wL.size(); iw++) {

			// ignore connected wires - so we don't snap to them
			bool skipThisWire = false;
			for (int wc = 0; wc < connectedWireNumber.size(); wc++) {
				if (iw == connectedWireNumber[wc]) {
					skipThisWire = true;
					break;
				}
			}
			if (skipThisWire)
				continue;

			for (int e = 0; e < 2; e++) // only check the wire's end points
			{
				tx = pg->dgrm.wL[iw]->x[e];
				ty = pg->dgrm.wL[iw]->y[e];

				for (j = 0; j < lCount; j++) {
					dist = (lx[j] - tx) * (lx[j] - tx)
						+ (ly[j] - ty) * (ly[j] - ty);
					if (dist < MAG_DIST) {
						result.x = posx + tx - lx[j];
						result.y = posy + ty - ly[j];
						return result;// modified: true
					}
				}

				for (j = 0; j < rCount; j++) {
					dist = (rx[j] - tx) * (rx[j] - tx)
						+ (ry[j] - ty) * (ry[j] - ty);
					if (dist < MAG_DIST) {
						 // valient try but once it sticks it wont let go
						result.x = posx + tx - rx[j];
						result.y = posy + ty - ry[j];
						return result;// modified: true
					}
				}
			}
		}
	}
	result.x = posx;
	result.y = posy;
	return result;// modified: false
}



bool newWireBend = false;    // we need these...
int newWireBendMouseDownX;  //  so that we don't bend a wire...
int newWireBendMouseDownY;  //    when selecting it.
PATH findWirePathAndBendIt(Sheet* pg, int x, int y) {
	PATH result;
	newWireBend = false;
	for (int i = pg->dgrm.wL.size() - 1; i >= 0; i--) {
		if (pg->dgrm.wL[i]->ghost)
			continue;

		if (pg->dgrm.wL[i]->x.size() == 2) { // simple wire - no bends
			if (checkWireSegment(pg, x, y, i, 0, 1)) {
				pg->dgrm.wL[i]->x.push_back(x);
				pg->dgrm.wL[i]->y.push_back(y);
				pg->dgrm.wL[i]->pointSelected.push_back(false);
				newWireBend = true;
				newWireBendMouseDownX = x;
				newWireBendMouseDownY = y;
				result.w = i;
				result.t = 2;
				return result; //  { w: i, t : 2 };      // now [0] [2] [1]
			}
		}

		int k;
		int n;
		// bent wires go [0] [2] [3] ... [n] [1]
		if (pg->dgrm.wL[i]->x.size() > 2) {
			if (checkWireSegment(pg, x, y, i, 0, 2)) {
				// make room at front of list -- reverse ordering
				n = pg->dgrm.wL[i]->x.size();
				pg->dgrm.wL[i]->x.resize(n+1);
				pg->dgrm.wL[i]->y.resize(n+1);
				pg->dgrm.wL[i]->pointSelected.resize(n + 1);
				for (k = n - 1; k >= 2; k--) {
					pg->dgrm.wL[i]->x[k + 1] = pg->dgrm.wL[i]->x[k];
					pg->dgrm.wL[i]->y[k + 1] = pg->dgrm.wL[i]->y[k];
					pg->dgrm.wL[i]->pointSelected[k + 1] = pg->dgrm.wL[i]->pointSelected[k];
				}
				pg->dgrm.wL[i]->x[2] = x;
				pg->dgrm.wL[i]->y[2] = y;
				pg->dgrm.wL[i]->pointSelected[2] = false;
				newWireBend = true;
				newWireBendMouseDownX = x;
				newWireBendMouseDownY = y;
				result.w = i;
				result.t = 2;
				return result; // { w: i, t : 2 };
			}
			for (int j = 2; j < pg->dgrm.wL[i]->x.size() - 1; j++) {
				if (checkWireSegment(pg, x, y, i, j, j + 1)) {
					// make room in the middle of list -- reverse ordering
					n = pg->dgrm.wL[i]->x.size();
					pg->dgrm.wL[i]->x.resize(n + 1);
					pg->dgrm.wL[i]->y.resize(n + 1);
					pg->dgrm.wL[i]->pointSelected.resize(n + 1);
					for (k = n - 1; k >= j + 1; k--) {
						pg->dgrm.wL[i]->x[k + 1] = pg->dgrm.wL[i]->x[k];
						pg->dgrm.wL[i]->y[k + 1] = pg->dgrm.wL[i]->y[k];
						pg->dgrm.wL[i]->pointSelected[k + 1] = pg->dgrm.wL[i]->pointSelected[k];
					}
					pg->dgrm.wL[i]->x[j + 1] = x;
					pg->dgrm.wL[i]->y[j + 1] = y;
					pg->dgrm.wL[i]->pointSelected[j + 1] = false;
					newWireBend = true;
					newWireBendMouseDownX = x;
					newWireBendMouseDownY = y;
					result.w = i;
					result.t = j + 1;
					return result; // { w: i, t : j + 1 };
				}
			}
			if (checkWireSegment(pg, x, y, i, pg->dgrm.wL[i]->x.size() - 1, 1)) {
				n = pg->dgrm.wL[i]->x.size();
				pg->dgrm.wL[i]->x.resize(n + 1);
				pg->dgrm.wL[i]->y.resize(n + 1);
				pg->dgrm.wL[i]->pointSelected.resize(n + 1);
				pg->dgrm.wL[i]->x[n] = x;
				pg->dgrm.wL[i]->y[n] = y;
				pg->dgrm.wL[i]->pointSelected[n] = false;
				newWireBend = true;
				newWireBendMouseDownX = x;
				newWireBendMouseDownY = y;
				result.w = i;
				result.t = n;
				return result; // { w: i, t : n };
			}
		}
	}
	result.w = -1;
	result.t = -1;
	return result; // { w: -1, t : -1 };   // didn't find it
}


bool modalDialogMouseIsDown = false;


void MouseDownEventListener(Sheet *pg, LPARAM lParam) {
	//showCoordinates(hWnd, 100, 100, lParam);

	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);
	
	// Modal dialogs are the ones that enable/disable the input pins or 
	// adjust the settings and literals
	int dialogHit = processModalDialogComponent(pg, rawpos.x, rawpos.y);
	if (dialogHit) {
		modalDialogMouseIsDown = true;
		unselectAllItems(pg);
		drawCanvas();
		return;
	}

	// Component buttons are the ones that popup around a component
	// there are basic buttons that all components have like
	// delete, duplicate, move to front, move to back
	// there are also buttons that are specific to each component
	// like set the number of rows and columns in a matrix
	int buttonHit = checkComponentButtonHit(pg, rawpos.x, rawpos.y);
	if (buttonHit != -1) {
		unselectAllItems(pg);
		drawCanvas();
		return;
	}

	// check the top row of buttons: Options, Controls, Functions, Drawings, Arrays 
	if (checkFrameButtons(pg, rawpos)) {
		if(pg->visibleButtonGroup != 0 )
			unselectAllItems(pg);
		drawCanvas();
		return;
	}
	
	// duplicate or delete a selection
	if (DoSelectedComponentButtons(pg, rawpos))
		return;

	// The group buttons follow the frame buttons in large groups
	// The Functions button pops up: Add, Subtract, Multiply, Divide, ...
	int newComponentIndex = checkGroupButtons(pg, rawpos);
	if( newComponentIndex >= 0 ) { // new component created
		deactivateAllButtons(pg);
		pg->mouseOffsetX = getComponentX(pg, newComponentIndex);
		pg->mouseOffsetY = getComponentY(pg, newComponentIndex);
		unselectAllItems(pg);
		drawCanvas();
		return;
	}
	if (newComponentIndex == -2) {// other action like save file or open 2d window
		newComponentIndex = -1;
		deactivateAllButtons(pg);
		unselectAllItems(pg);
		drawCanvas();
		return;
	}
	if (newComponentIndex == -3) {// subroutine poped up a dialog
		newComponentIndex = -1;
		//deactivateAllButtons(pg);
		//unselectAllItems(pg);
		drawCanvas();
		return;
	}

	// check the timer buttons. These are enabled when a time based component is on the screen
	int but = checkTimeButtons(pg, rawpos);
	if( but != -1) {

		switch (but) {
		case 0: // Reset
			stopClock(pg); // simple way to stop the clock when reset is hit
			resetClock(pg);
			break;
		case 1: // Step
			stepClock(pg);
			break;
		case 2: // Start
			startClock(pg);
			break;
		case 3: // Stop
			stopClock(pg); // simple way to stop the clock when reset is hit
			break;
		}

		//initAirWaves2(); // reset the airWaves2
		unselectAllItems(pg);
		propagateValues(&pg->dgrm);
		drawCanvas();
		//drawCanvas2();
		displaySystemTime(pg);

		return;
	}

	int posX = convertToLocalX(pg, rawpos.x);
	int posY = convertToLocalY(pg, rawpos.y);

	// ok its not a button, search for a wire first
	PATH combo = findWireTerminal(posX, posY);
	pg->draggingWire = combo.w;
	pg->dragTerminal = combo.t;
	if (pg->draggingWire != -1) {
		//saveState();
		unselectAllItems(pg);
		return;
	}
	else
	{
		// not a wire  -- search for a component
		pg->draggingComponent = findObject(pg, posX, posY, false);
		if (pg->draggingComponent >= 0) {
			// if it is selected - we are dragging the whole group
			if (pg->dgrm.cL[pg->draggingComponent]->selected) {
				pg->mouseOffsetX = posX;
				pg->mouseOffsetY = posY;
				pg->draggingSelectedComponents = true;
				return;
			}

			// check for a dial
			if (!componentIsGhost(pg->draggingComponent)) {
				if (!componentHasCrank(pg->hDC, pg->draggingComponent, posX, posY, true)) {
					//saveState();
					if (!pg->dgrm.cL[pg->draggingComponent]->selected)
						unselectAllItems(pg);
					// not a crank -- setup to drag it
					pg->mouseOffsetX = posX;
					pg->mouseOffsetY = posY;
					pg->mouseDownDX = posX - pg->dgrm.cL[pg->draggingComponent]->x;
					pg->mouseDownDY = posY- pg->dgrm.cL[pg->draggingComponent]->y;

					if(magnetWiresButton->isSelected)
						StartDraggingWires(pg, pg->draggingComponent);
					return;
				}
				else {
					//saveState();
					// we cranked something - remember which component and update
					pg->crankingComponent = pg->draggingComponent;
					pg->draggingComponent = -1;
					propagateValues(&pg->dgrm);
					unselectAllItems(pg);
					drawCanvas();
					return;
				}
			}
		}
		else // no object hit, 
		{
			// are we over a terminal?
			POS result = findComponentTerminal(posX, posY);
			if (result.x != -1) { // lets start a new wire
				pg->draggingWire = AddWire(pg, result.x, result.y, result.x, result.y);
				pg->dragTerminal = 1;
				unselectAllItems(pg);
				drawCanvas();
				return;
			}
			else // not over a terminal, are we over a wire?
			{
				if (allowWiresToBendButton->isSelected) {
					PATH path = findWirePathAndBendIt(pg, posX, posY);// check for wire path under mouse to add a bend
					if (path.w != -1) {
						// found a wire segment under the mouse
						pg->draggingWire = path.w;
						pg->dragTerminal = path.t; // the new terminal
						unselectAllItems(pg);
						return;
					}
				}
			}
		}
	}
	deactivateAllButtons(pg);

	//ok, nothing under the mouse - start a selectionRectangle
	pg->selectionRectangleInProgress = true;
	pg->selectionRectangleStartX = rawpos.x;
	pg->selectionRectangleStartY = rawpos.y;
	pg->selectionRectangleEndX = rawpos.x;
	pg->selectionRectangleEndY = rawpos.y;

	drawCanvas();
}

void MouseDownMiddleEventListener(Sheet* pg, LPARAM lParam) {
	
	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);

	int posX = convertToLocalX(pg, rawpos.x);
	int posY = convertToLocalY(pg, rawpos.y);

	// are we over a terminal?
	POS result = findComponentTerminal(posX, posY);
	if (result.x != -1) { // lets start a new wire
		pg->draggingWire = AddWire(pg, result.x, result.y, result.x, result.y);
		pg->dragTerminal = 1;
		unselectAllItems(pg);
		drawCanvas();
		return;
	}
	
}


// remember if the user missed the component on mouse down
 // so the starting drag doesn't move the component.
 // Note: We don't have to do this for wires.


int canvasDragStartX;
int canvasDragStartY;

int canvasSelectStartX;
int canvasSelectStartY;
int canvasSelectEndX;
int canvasSelectEndY;

int modalDialogKeyboardEntry = -1; // enables keyboard entry and holds the button's index


bool handlePopupButtons(Sheet* pg, POS rawpos ) {
	if (checkGroupButtons(pg, rawpos) != -1) {
		//activateButtonGroup(visibleButtonGroup, false);
		deactivateAllButtons(pg);
		return true;
	}
	return false;
}


//var mouseMemory;
bool modalDialog_mouseDragEnable = false;
int modalDialog_mouseDragX = 0;
int modalDialog_mouseDragY = 0;

//var modalDialog_mouseDragButton;

int hoverOnComponent = -1;
int hoverOnTerminal = -1;

void MouseMoveEventListener(Sheet *pg, LPARAM lParam) {

	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);

	if (modalDialogMouseIsDown)
		mouseMoveInModalDialog(pg, rawpos.x, rawpos.y);
	if (pg->modalDialogComponent != -1)
		return;

	POS mouseMemory = rawpos;
	int rawPosX = convertToLocalX(pg, rawpos.x);
	int rawPosY = convertToLocalY(pg, rawpos.y);
	int posx = floor(rawPosX);
	int posy = floor(rawPosY);

	// mouse went down on a selected component 
	if (pg->draggingSelectedComponents) {
		int len = pg->dgrm.cL.size();
		for (int i = 0; i < len; i++) { // move all selected components
			if (pg->dgrm.cL[i]->selected) {
				dragComponent(pg, i, posx - pg->mouseOffsetX, posy - pg->mouseOffsetY);
			}
		}

		len = pg->dgrm.wL.size();
		for (int i = 0; i < len; i++) { // move all the selected ends and bends
			if (pg->dgrm.wL[i]->selected) {
				pg->dgrm.wL[i]->offsetPoints(posx - pg->mouseOffsetX, posy - pg->mouseOffsetY);
			}
		}
		pg->mouseOffsetX = posx;
		pg->mouseOffsetY = posy;
		hoverOnComponent = -1;
		hoverOnTerminal = -1;
		debugString = "";
		drawCanvas();
		return;
	}

	// we are drawing a selection rectangle
	if (pg->selectionRectangleInProgress) {
		pg->selectionRectangleEndX = rawpos.x;
		pg->selectionRectangleEndY = rawpos.y;
		drawCanvas();
		return;
	}

	//  we are dragging the canvas
	if (pg->draggingCanvas) {
		pg->cXo -= canvasDragStartX - rawpos.x;
		pg->cYo -= canvasDragStartY - rawpos.y;
		canvasDragStartX = rawpos.x;
		canvasDragStartY = rawpos.y;
		drawCanvas();
		return;
	}

	if (pg->draggingComponent >= 0) {
		if (magnetComponentsButton->isSelected) {
			pg->dgrm.cL[pg->draggingComponent]->x = posx - pg->mouseDownDX;
			pg->dgrm.cL[pg->draggingComponent]->y = posy - pg->mouseDownDY;
			POS pos = snapComponentToTerminal(pg, pg->draggingComponent, posx- pg->mouseDownDX, posy - pg->mouseDownDY);
			pg->dgrm.cL[pg->draggingComponent]->x = pos.x;
			pg->dgrm.cL[pg->draggingComponent]->y = pos.y;
		}else {
			dragComponent(pg, pg->draggingComponent, posx - pg->mouseOffsetX, posy - pg->mouseOffsetY);
			pg->mouseOffsetX = posx;
			pg->mouseOffsetY = posy;
		}
		
		if (magnetWiresButton->isSelected)
			PullWiresWithComponent(pg, pg->draggingComponent);
		hoverOnComponent = -1;
		hoverOnTerminal = -1;
		debugString = "";
		drawCanvas();
		return;
	}

	if (pg->crankingComponent >= 0) {
		doCrank(pg->hDC, pg->crankingComponent, posx, posy, false);
		propagateValues(&pg->dgrm);
		drawCanvas();
		hoverOnComponent = -1;
		hoverOnTerminal = -1;
		debugString = "";
		return;
	}

	if (pg->draggingWire >= 0) {
		dragWire(pg, posx, posy);
		drawCanvas();
		hoverOnComponent = -1;
		hoverOnTerminal = -1;
		debugString = "";
		return;
	}

	// DEBUG
	//debugString = tomString(posx) + "  " + tomString(posy);
	//showDebugString();
	//return;


	// if we are over a output terminal, show its dimensions and contents
	for (int c = 0; c < pg->dgrm.cL.size(); c++) {
		for (int t = 0; t < pg->dgrm.cL[c]->rightTerminalCount; t++) {
		
			int dist = TERMINAL_SIZE;
			int x1 = pg->dgrm.cL[c]->rightTerminalX(t);
			if (abs(posx - x1) > dist)
				continue;
			int y1 = pg->dgrm.cL[c]->rightTerminalY(t);
			if (abs(posy - y1) > dist)
				continue;

			if (hoverOnComponent == c)
				continue;
			hoverOnComponent = c;
			if (hoverOnTerminal == t)
				continue;
			hoverOnTerminal = t;
			
			// the component index
			debugString = tomString(c) + ") ";
			// get the dimensions
			debugString += "dim [";
			int size = pg->dgrm.cL[c]->output[t].dimensions.size();
			for (int v = 0; v < size; v++) {
				if (v == 0)
					debugString += tomString(pg->dgrm.cL[c]->output[t].dimensions[v]);
				else
					debugString += ",  " + tomString(pg->dgrm.cL[c]->output[t].dimensions[v]);
			}
			debugString += "]";

			if (!pg->dgrm.cL[c]->output[t].stringType)
			{   // get the values
				debugString += " value (";
				size = pg->dgrm.cL[c]->output[t].value.size();
				for (int v = 0; v < size; v++) {
					if (v == 0)
						debugString += tomString(pg->dgrm.cL[c]->output[t].value[v]);
					else
						debugString += ",  " + tomString(pg->dgrm.cL[c]->output[t].value[v]);
				}
				debugString += ")";
			}
			else
			{	// get the strings
				debugString += " string (";
				size = pg->dgrm.cL[c]->output[t].stringValue.size();
				for (int v = 0; v < size; v++) {
					if (v == 0)
						debugString += pg->dgrm.cL[c]->output[t].stringValue[v];
					else
						debugString += ",  " + pg->dgrm.cL[c]->output[t].stringValue[v];
				}
				debugString += ")";
			}

			showDebugString(); //drawCanvas();
			return;
		}
	}
	hoverOnComponent = -1;
	hoverOnTerminal = -1;
	debugString = "";
}
#if 0

canvas.addEventListener('mousemove', function(evt) {
	var rawpos = getMousePos(canvas, evt);
	mouseMemory = rawpos;
	var rawPosX = (rawpos.x / pg->cs) - pg->cXo;
	var rawPosY = (rawpos.y / pg->cs) - pg->cYo;
	var posx = floor(rawPosX);
	var posy = floor(rawPosY);
	var mes;
	var pos;

	if ((modalDialogComponent >= 0) && (modalDialog_mouseDragEnable)) {
		var text = modalDialog_mouseDragButton.keyboardText;
		var cursorPos = modalDialog_mouseDragButton.keyboardCursorPos
			//console.log( "start -----------------------");
			//console.log( "cursorPos " + cursorPos);
			//console.log( "modalDialog_mouseDragY " + modalDialog_mouseDragY);
			//console.log( "rawpos.y " + rawpos.y);
			var delta = modalDialog_mouseDragY - rawpos.y;
		//console.log( "delta " + delta);
		var useable = floor(delta / 10);
		//console.log( "useable " + useable);
		var newValue = parsedouble(text) + useable;
		var newText = newValue.tomString();
		modalDialog_mouseDragButton.keyboardText = newText;
		modalDialog_mouseDragY -= useable * 10;
		//console.log( "modalDialog_mouseDragY " + modalDialog_mouseDragY);
		drawCanvas();
	}

	if (draggingSelectionPhase == 2) {
		var dX = rawPosX - draggingSelectionStartX;
		var dY = rawPosY - draggingSelectionStartY;

		// shift the components
		for (var i = 0; i < componentsBeingDragged.length; i++) {
			var j = componentsBeingDragged[i];
			pg->dgrm.cL[j].x += dX;
			pg->dgrm.cL[j].y += dY;
		}
		for (i = 0; i < wiresBeingDragged.length; i++) {
			j = wiresBeingDragged[i];
			var jj = nodesBeingDragged[i];
			pg->dgrm.wL[j].x[jj] += dX;
			pg->dgrm.wL[j].y[jj] += dY;
		}

		drawCanvas();
		draggingSelectionStartX = rawPosX;
		draggingSelectionStartY = rawPosY;
	}
	else if (draggingComponent >= 0) {
		if (magnetComponentsButton.isSelected) {
			//console.log("START    ( " + posx + " , " + posy + " )" );
			pg->dgrm.cL[draggingComponent].setPosition(posx - mouseOffsetX, posy - mouseOffsetY);
			pos = snapComponentToTerminal(draggingComponent, posx, posy);

			//console.log(" RETURN    ( " + pos.x + " , " + pos.y + " )" );
			pg->dgrm.cL[draggingComponent].setPosition(pos.x - mouseOffsetX, pos.y - mouseOffsetY);
			//console.log(" New component pos ( " + pg->dgrm.cL[draggingComponent].x + " , " + pg->dgrm.cL[draggingComponent].y + " )" );
			//console.log(" because mouse offset is ( " + mouseOffsetX + " , " + mouseOffsetY + " )" );
		}
		else {
			pg->dgrm.cL[draggingComponent].setPosition(posx - mouseOffsetX, posy - mouseOffsetY);
		}

		if (stickyComponentsButton.isSelected && !playLessonButton.isSelected) {
			// pull the wires with the component
			var deltaX = pg->dgrm.cL[draggingComponent].x - connectedStartX;
			var deltaY = pg->dgrm.cL[draggingComponent].y - connectedStartY;
			for (var k = 0; k < connectedWireNumber.length; k++) {
				// move each wire
				var w = connectedWireNumber[k];
				var e = connectedWireEnd[k];
				pg->dgrm.wL[w].x[e] += deltaX;
				pg->dgrm.wL[w].y[e] += deltaY;
			}
			connectedStartX = pg->dgrm.cL[draggingComponent].x;
			connectedStartY = pg->dgrm.cL[draggingComponent].y;
			// drawCanvas();
		}

		drawCanvas();
	}
	else if (crankingComponent >= 0) {
		pg->dgrm.cL[crankingComponent].crank(pg, crankingComponent, posx, posy, false);

		//   debugShowTime( '2start ');
		propagateValues();
		//   debugShowTime( '2Propagate done     ');
		drawCanvas();
		//   debugShowTime( '2Draw Canvas done   ');
		drawCanvas2();
		//   debugShowTime( '2Draw Canvas 2 done ');
	}
	else if (draggingWire >= 0) {
		var result;

		// magnetic wires - if the mouse is close to a terminal, snap to it
		if (magnetWiresButton.isSelected)
		{
			if (dragTerminal < 2) {
				// its a terminal snap or neaten
				result = snapWireToTerminal(posx, posy);
				if (result.s)
					pg->dgrm.wL[draggingWire].setPosition(dragTerminal, result.x, result.y);
				else {
					result = neatenWire(draggingWire, dragTerminal, posx, posy);
					if (result.s)
						pg->dgrm.wL[draggingWire].setPosition(dragTerminal, result.x, result.y);
					else
						pg->dgrm.wL[draggingWire].setPosition(dragTerminal, posx, posy);
				}
			}
			else { // bends dont snap to terminals but can neaten
				result = neatenWire(draggingWire, dragTerminal, posx, posy);
				if (result.s)
					pg->dgrm.wL[draggingWire].setPosition(dragTerminal, result.x, result.y);
				else
					pg->dgrm.wL[draggingWire].setPosition(dragTerminal, posx, posy);
			}
		}
		else {
			pg->dgrm.wL[draggingWire].setPosition(dragTerminal, posx, posy);
			//console.log('68726 ' + draggingWire + "  " + dragTerminal + "  " + pg->dgrm.wL[draggingWire].x[0] + "  " + pg->dgrm.wL[draggingWire].y[0] );
		}
		drawCanvas();
	}
	else if (draggingCanvas) {
		// var mes = '  (delta: ' + floor(canvasDragStartX - pos.x).tomString() + ',  ' + floor(canvasDragStartY - pos.y).tomString() + ' )';
		// var mes = '  (start: ' + floor(canvasDragStartX).tomString() + ',  ' + floor(canvasDragStartY).tomString() + ' )';
		pg->cXo -= (canvasDragStartX - rawpos.x) / pg->cs;
		pg->cYo -= (canvasDragStartY - rawpos.y) / pg->cs;

		canvasDragStartX = rawpos.x;
		canvasDragStartY = rawpos.y;

		drawCanvas();

		/*   if (theTimer == null) {
		 if ((pg->cXo != 0) || (pg->cYo != 0)) {
		 ctx.scale(1/pg->cs, 1/pg->cs);
		 // display offset coordinates
		 ctx.clearRect(canvas.width / 2 - 30, canvas.height - 20, 200, 20);
		 mes = "  (offset: " + floor(pg->cXo).tomString() + ",  " + floor(pg->cYo).tomString() + " )";
		 ctx.fillText(mes, canvas.width / 2 - 29, canvas.height - 7);
		 ctx.scale(pg->cs, pg->cs);
		 }
		 } */
	}
	else if (selectingCanvas) {
		// draw a rectangle as we move
		canvasSelectEndX = rawpos.x;
		canvasSelectEndY = rawpos.y;

		if ((abs(canvasSelectStartX - canvasSelectEndX) > 20) &&
			(abs(canvasSelectStartY - canvasSelectEndY) > 20))
			stopClock();

		drawCanvas();

		var cSSX = (canvasSelectStartX / pg->cs);
		var cSSY = (canvasSelectStartY / pg->cs);

		var cSEX = (canvasSelectEndX / pg->cs);
		var cSEY = (canvasSelectEndY / pg->cs);

		ctx.strokeRect(cSSX, cSSY,
			cSEX - cSSX, cSEY - cSSY)
	}
	else {
		// if we are over an object, show it's note if it has one
		var temp = findObject(posx, posy, !playLessonButton.isSelected);
		if (temp >= 0) {
			if (showingComponentNote != temp) {

				// display component note
				if (pg->dgrm.cL[temp].hover == null) {
					mes = temp.tomString() + ") " + pg->dgrm.cL[temp].label;
					if (pg->dgrm.cL[temp].userNote)
						mes += ": " + pg->dgrm.cL[temp].userNote;
					//   mes = "ghost=" + pg->dgrm.cL[temp].ghost + " selected=" +  pg->dgrm.cL[temp].selected;
					showingComponentNote = temp; // reduce flicker
				}
				else {
					mes = pg->dgrm.cL[temp].hover(pg->dgrm.cL[temp], rawpos);
				}
				commonShowMessage(mes, true);
			}
		}
		else {
			showingComponentNote = -1;

			if (keyboardSearch.isSelected && (keySearchText.length > 0)) {
				commonClearMessage();
				commonShowMessage("Keyboard Search: " + keySearchText, false);
			}
			else
			{
				// here and in StartClock() to update while the clock is running
				commonClearMessage();
				showValueOfOutputTerminal(rawpos);
			}
		}

		// show mouse coords
		if (theTimer == null) {
			ctx.scale(1 / pg->cs, 1 / pg->cs);
			// display offset coordinates
			ctx.clearRect(canvas.width / 2 - 30, canvas.height - 20, 200, 20);
			mes = "mouse: " + floor(rawPosX) + ",  " + floor(rawPosY) +
				"  scale: " + funcRound(pg->cs, 2);
			ctx.fillText(mes, canvas.width / 2 - 29, canvas.height - 7);
			ctx.scale(pg->cs, pg->cs);

		}

	}
	}, false);

#endif

void MouseUpEventListener(Sheet* pg, LPARAM lParam) {

	if (modalDialogMouseIsDown) {
		mouseUpInModalDialog(pg);
		modalDialogMouseIsDown = false;
	}

	if (pg->selectionRectangleInProgress) {
		int minX = min(pg->selectionRectangleStartX, pg->selectionRectangleEndX) - pg->cXo;
		int maxX = max(pg->selectionRectangleStartX, pg->selectionRectangleEndX) - pg->cXo;
		int minY = min(pg->selectionRectangleStartY, pg->selectionRectangleEndY) - pg->cYo;
		int maxY = max(pg->selectionRectangleStartY, pg->selectionRectangleEndY) - pg->cYo;

		// check components in the rectangle
		int len = pg->dgrm.cL.size();
		for (int i = 0; i < len; i++) {
			pg->dgrm.cL[i]->selected = false;
			if (pg->dgrm.cL[i]->x < minX) continue;
			if (pg->dgrm.cL[i]->x > maxX) continue;
			if (pg->dgrm.cL[i]->y < minY) continue;
			if (pg->dgrm.cL[i]->y > maxY) continue;
			pg->dgrm.cL[i]->selected = true;
		}

		// check wires that have one end or the other in the rectangle
		len = pg->dgrm.wL.size();
		// the start of the wire
		for (int i = 0; i < len; i++) {
			pg->dgrm.wL[i]->selected = false;
			pg->dgrm.wL[i]->pointSelected[0] = false;
			if (pg->dgrm.wL[i]->x[0] < minX) continue;
			if (pg->dgrm.wL[i]->x[0] > maxX) continue;
			if (pg->dgrm.wL[i]->y[0] < minY) continue;
			if (pg->dgrm.wL[i]->y[0] > maxY) continue;
			pg->dgrm.wL[i]->selected = true;
			pg->dgrm.wL[i]->pointSelected[0] = true;
		}
		// the end
		for (int i = 0; i < len; i++) {
			pg->dgrm.wL[i]->pointSelected[1] = false;
			if (pg->dgrm.wL[i]->x[1] < minX) continue;
			if (pg->dgrm.wL[i]->x[1] > maxX) continue;
			if (pg->dgrm.wL[i]->y[1] < minY) continue;
			if (pg->dgrm.wL[i]->y[1] > maxY) continue;
			pg->dgrm.wL[i]->selected = true;
			pg->dgrm.wL[i]->pointSelected[1] = true;
		}
		// the bends
		for (int i = 0; i < len; i++) {
			for (int j = 2; j < pg->dgrm.wL[i]->pointSelected.size(); j++) {
				pg->dgrm.wL[i]->pointSelected[j] = false;
				if (pg->dgrm.wL[i]->x[j] < minX) continue;
				if (pg->dgrm.wL[i]->x[j] > maxX) continue;
				if (pg->dgrm.wL[i]->y[j] < minY) continue;
				if (pg->dgrm.wL[i]->y[j] > maxY) continue;
				pg->dgrm.wL[i]->pointSelected[j] = true;
			}
		}

		pg->selectionRectangleInProgress = false;
		drawCanvas();
	}

	pg->draggingSelectedComponents = false;

	if (pg->modalDialogComponent != -1)
		return;

	bool needToAnalyze = false;
	bool needToPropagate = false;

	deleteDoubleBuffer();

	if (pg->draggingComponent != -1) {
		if (pg->dgrm.cL.size() == 0)
			pg->isDirty = false;
		else
			setVisibleScreenDirty("Dragged component");// pg->isDirty = true;
		needToAnalyze = true;
		pg->draggingComponent = -1;
	}
	
	DisconnectWireDragging();

	if (pg->crankingComponent >= 0) {
		doMouseUp(pg->crankingComponent);
		pg->crankingComponent = -1;
		needToPropagate = true;
	}
	if (pg->draggingWire != -1) {
		POS rawpos;
		rawpos.x = GET_X_LPARAM(lParam);
		rawpos.y = GET_Y_LPARAM(lParam);
		int posX = convertToLocalX(pg, rawpos.x);
		int posY = convertToLocalY(pg, rawpos.y);
		
		if (pg->dragTerminal < 2) { // we are not bending a wire
			// end point must be on a terminal
			POS result = findComponentTerminal(posX, posY);
			if (result.x == -1) {
				destroyWire(pg);
				remoteAnalyzeConnections();
				drawCanvas();
			}
			else
			{
				finishWire(pg, result.x, result.y);

				if (destroyShortWire(pg)) {
					pg->draggingWire = -1;
					drawCanvas();
				}
				else
					needToAnalyze = true;
			}
		}
		pg->draggingWire = -1;
	}
	
	for (int i = 0; i < pg->dgrm.cL.size(); i++) {
		if (pg->dgrm.cL[i]->ghost)
			continue;

		if (pg->dgrm.cL[i]->type == L"Button") {
			if (pg->dgrm.cL[i]->setting[2] == 0) { // momentary
				for (int j = 0; j < pg->dgrm.cL[i]->setting[0]; j++) {
					if (pg->dgrm.cL[i]->output[0].value[j]) {
						pg->dgrm.cL[i]->output[0].value[j] = 0;
						needToPropagate = true;
					}
				}
			}
		}
	}

	if (needToAnalyze) {
		remoteAnalyzeConnections();
		drawCanvas();
	}
	else if (needToPropagate) {
		propagateValues(&pg->dgrm);
		drawCanvas();
	}
}

void deleteWire(Sheet* pg, int theWire) {
	//int n = pg->dgrm.wL.size();
	//for (int w = theWire; w < n - 1; w++)
	//	pg->dgrm.wL[w] = pg->dgrm.wL[w + 1];
	//pg->dgrm.wL.resize(n - 1);

	pg->dgrm.wL.erase(pg->dgrm.wL.begin() + theWire);

	if (pg->dgrm.cL.size() == 0)
		pg->isDirty = false;
	else
		setVisibleScreenDirty("Deleted wire");// pg->isDirty = true;

	//programChangedDefinition = "Deleted a wire";
	//removeWireFromLesson(theWire);
	//if (!autoUpdateSheet*Button.isSelected)
	//	debug_hover_depth = 0; // stop debugging Sheet*s
}

int findWirePath(Sheet* pg, int x, int y) {
	
	for (int i = pg->dgrm.wL.size() - 1; i >= 0; i--) {
		if (pg->dgrm.wL[i]->ghost)
			continue;

		if (pg->dgrm.wL[i]->x.size() == 2) { // simple wire - no bends
			if (checkWireSegment(pg, x, y, i, 0, 1))
				return i;
		}

		// bent wires go [0] [2] [3] ... [n] [1]
		if (pg->dgrm.wL[i]->x.size() > 2) {
			if (checkWireSegment(pg, x, y, i, 0, 2))
				return i; 
			for (int j = 2; j < pg->dgrm.wL[i]->x.size() - 1; j++) {
				if (checkWireSegment(pg, x, y, i, j, j + 1))
					return i;
			}
			if (checkWireSegment(pg, x, y, i, pg->dgrm.wL[i]->x.size() - 1, 1))
				return i;
		}
	}
	return -1; // didn't find it
}

vector<LPCTSTR> logicTypes{ L"Switch", L"Button", L"Running", 
L"2dButton", L"2dButton2", L"InPoly", L"InRange",
L"And", L"Or", L"Nand", L"Nor", L"Xor", L"Not", L"Logic", L"Flip", L"DFlip",
L"LT", L"GT", L"EQ", L"GTE", L"LTE", L"NEQ", L"GTZ", L"LTZ", L"GTEZ", L"LTEZ", L"NEQZ" };
bool checkForLogicOutput(Sheet* pg, int ic, int ot) {

	for( int j = 0; j < logicTypes.size(); j++)
		if( z(pg->dgrm.cL[ic]->type, logicTypes[j] ))
			return true;
	return false;
}

void MouseDownRightEventListener(Sheet* pg, LPARAM lParam)
{
	if (pg->modalDialogComponent != -1)
		return;
	

	POS rawpos;
	rawpos.x = GET_X_LPARAM(lParam);
	rawpos.y = GET_Y_LPARAM(lParam);
	POS mouseMemory = rawpos;
	int rawPosX = convertToLocalX(pg, rawpos.x);
	int rawPosY = convertToLocalY(pg, rawpos.y);
	int posx = floor(rawPosX);
	int posy = floor(rawPosY);


	if (checkGroupButtonsAndShowDetails(pg, rawpos) >= 0)
		return;

	// if we are dragging a wire. lets bend it here
	if (pg->draggingWire != -1) {
		if (allowWiresToBendButton->isSelected) {
			//  add point to end of path
			pg->dgrm.wL[pg->draggingWire]->x.push_back(posx);
			pg->dgrm.wL[pg->draggingWire]->y.push_back(posy);
			pg->dgrm.wL[pg->draggingWire]->pointSelected.push_back(false);
		}
		return;
	}

	// check for right click on a component - pop up menu buttons
	int hitComponent = findObject(pg, posx, posy, false);
	if (hitComponent >= 0) {
		//saveState();
		//pg->dgrm.cL[hitComponent].doubleClick(hitComponent);
		doubleClickDialog(hitComponent);
		return;
	}

	// check for right click on wire terminal
	PATH hitWire = findWireTerminal(posx, posy);
	if (hitWire.w >= 0) {
		if (hitWire.t > 1) {
			//saveState();
			// we hit a bend - remove just the bend
			int points = pg->dgrm.wL[hitWire.w]->x.size();
			for (int pt = hitWire.t; pt < points - 1; pt++) {
				pg->dgrm.wL[hitWire.w]->x[pt] = pg->dgrm.wL[hitWire.w]->x[pt + 1];
				pg->dgrm.wL[hitWire.w]->y[pt] = pg->dgrm.wL[hitWire.w]->y[pt + 1];
				pg->dgrm.wL[hitWire.w]->pointSelected[pt] = pg->dgrm.wL[hitWire.w]->pointSelected[pt + 1];
			}
			pg->dgrm.wL[hitWire.w]->x.resize(points - 1);
			pg->dgrm.wL[hitWire.w]->y.resize(points - 1);
			pg->dgrm.wL[hitWire.w]->pointSelected.resize(points - 1);

			analyzeConnections(&pg->dgrm);
			drawCanvas();
			drawCanvas2();
			return;
		}
	}
	
	// check for wire path under mouse - and delete it
	int theWire = findWirePath(pg, posx, posy);
	if (theWire != -1) {
		//saveState();
		deleteWire(pg, theWire);
		analyzeConnections(&pg->dgrm);
		drawCanvas();
		drawCanvas2();
		return;
	}

	// check for mouse down on an input terminal
	int dist = TERMINAL_SIZE * TERMINAL_SIZE;
	bool done = false;
	for (int ic = 0; ic < pg->dgrm.cL.size(); ic++) {
		if (pg->dgrm.cL[ic]->ghost)
			continue;
		for (int it = 0; it < pg->dgrm.cL[ic]->leftTerminalCount; it++) {
			int dx = pg->dgrm.cL[ic]->leftTerminalX(it) - posx;
			int dy = pg->dgrm.cL[ic]->leftTerminalY(it) - posy;

			if (dx * dx + dy * dy < dist) {
				// ask for constant
				double result = 0;
				//		saveState();
						// add a 'Con' connected to this terminal
				POS pos;
				pos.x = pg->dgrm.cL[ic]->leftTerminalX(it) - 42; // based on the width of the constant and terminal size
				pos.y = pg->dgrm.cL[ic]->leftTerminalY(it);
				pos.x = (pos.x + pg->cXo) * pg->cs;
				pos.y = (pos.y + pg->cYo) * pg->cs;
				AddComponent(pg, L"Con", pos, pg->visibleSheet);
				pg->modalDialogComponent = pg->draggingComponent;

				//buildConDialogButtons(draggingComponent);
				doubleClickConstant(pg, pg->draggingComponent);

				pg->draggingComponent = -1;

				analyzeConnections(&pg->dgrm);
				propagateValues(&pg->dgrm);
				drawCanvas();
				drawCanvas2();

				done = true;
				break;
			}
			if (done)
				break;
		}
		if (done)
			break;

		// check for mouse down on an output terminal
		for (int ot = 0; ot < pg->dgrm.cL[ic]->rightTerminalCount; ot++) {
			int dx = pg->dgrm.cL[ic]->rightTerminalX(ot) - posx;
			int dy = pg->dgrm.cL[ic]->rightTerminalY(ot) - posy;
	
			if (dx * dx + dy * dy < dist) {
				// place a display
			//	saveState();
				// add a 'Dis' or 'Grid' -   connected to this terminal
				POS pos;
				if (checkForLogicOutput(pg, ic, ot))
					pos.x = pg->dgrm.cL[ic]->rightTerminalX(ot) + 22; // based on the width of the LED and terminal size
				else if (pg->dgrm.cL[ic]->output[ot].dimensions.size() == 2)
					pos.x = pg->dgrm.cL[ic]->rightTerminalX(ot) + 67; // based on the width of the grid and terminal size
				else
					pos.x = pg->dgrm.cL[ic]->rightTerminalX(ot) + 37; // based on the width of the display and terminal size
				pos.y = pg->dgrm.cL[ic]->rightTerminalY(ot);
				pos.x = (pos.x + pg->cXo) * pg->cs;
				pos.y = (pos.y + pg->cYo) * pg->cs;
				if (checkForLogicOutput(pg, ic, ot))
					AddComponent(pg, L"LED", pos, pg->visibleSheet);
				else if (pg->dgrm.cL[ic]->output[ot].dimensions.size() == 2)
					AddComponent(pg, L"Grid", pos, pg->visibleSheet);
				else
					AddComponent(pg, L"Dis", pos, pg->visibleSheet);
				pg->draggingComponent = -1;
	
				analyzeConnections(&pg->dgrm);
				propagateValues(&pg->dgrm);
				drawCanvas();
				drawCanvas2();
	
				done = true;
				break;
			}
			if (done)
				break;
		}
		if (done)
			break;
	}
	if (done)
		return;

	// hit nothing - lets drag the canvas!
	pg->draggingCanvas = true;
	canvasDragStartX = rawpos.x;
	canvasDragStartY = rawpos.y;
}

void MouseUpRightEventListener(Sheet* pg, LPARAM lParam)
{
	pg->draggingCanvas = false;
}

void KeyboardCharListener(Sheet* pg, WPARAM e) {

	if (modalDialogKeyboardEntry >= 0) {
		if (keyboardDialogChar(pg, e))
			return;
	}
	else if (pg->modalDialogComponent >= 0) {
		if (keyboardDialogKeydown(pg, e))
			return;
	}

	// dim buttons based on key search
	if (keyboardSearch->isSelected &&
		(pg->modalDialogComponent < 0))
	{
		if (e == 8) { // delete last character
			if (keySearchText.size() > 0) {
				keySearchText.resize(keySearchText.size() - 1);
			}
		}
		if ((e == 13) || (e == 27)) { // delete all characters
			keySearchText.clear();
			undimAllButtons(pg);
			drawCanvas();
			return;
		}
		if (((e >= 65) && (e <= 90))  || // A through Z
			((e >= 97) && (e <= 122)) || // a through z
			((e >= 48) && (e <= 57))  || // 0 through 9
			(e == 32)) { // space
			keySearchText += e;
		}
		
		dimButtonsBasedKeySearch(pg, keySearchText);

		drawCanvas();
			//			commonShowMessage("Keyboard Search: " + keySearchText, false);
		return;
		
	}
	//var needsUpdate = false;
	//for (j = 0; j < pg->dgrm.cL.length; j++)
	//	if (pg->dgrm.cL[j].type == 'Key')
	//		if (methodKey_(pg, j, e)) {
	//			needsUpdate = true;
	//			thisFunctionHandledIt = true;
	//		}
	//
	//// send key to the selected 2d string
	//if (selected2dStringComponent != -1)
	//	if (key2dString(e))
	//		thisFunctionHandledIt = true;
	//
	////console.log( "Needs update is " + needsUpdate);
	//if (needsUpdate) {
	//	propagateValues();
	//	drawCanvas();
	//	drawCanvas2();
	//}

	//if ((e.which == 27) && (e.type == 'keydown')) {// esacape key
	//	handleFindComponent();
	//	thisFunctionHandledIt = true;
	//}
	
	return;
}


void KeyboardKeydownListener(Sheet* pg, WPARAM e) {

	if (modalDialogKeyboardEntry >= 0) {
		if (keyboardDialogKeydown(pg, e))
			return;
	}
}
