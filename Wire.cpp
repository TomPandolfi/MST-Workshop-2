#include "stdafx.h"

#include "structs.h"
#include "wire.h"
#include "Sheet.h"
#include "buildComponent.h"
#include "Text.h"
#include "color.h"


#pragma warning (disable: 4267)

Wire::~Wire() {
	//x.clear();
	//y.clear();
	//pointSelected.clear();
}

// This is the Wire, it simply connects terminals
Wire::Wire( int startX, int startY, int endX, int endY) {

	drivenByComponent = -1;
	drivenByOutputTerminal = -1;

	x.push_back(startX);
	y.push_back(startY);
	pointSelected.push_back(false);

	x.push_back(endX);
	y.push_back(endY);
	pointSelected.push_back(false);

	ghost = NORMAL_GHOST; // this bit dims the wire and makes it not work
	selected = false;

}
Wire::Wire() {

	drivenByComponent = -1;
	drivenByOutputTerminal = -1;

	ghost = NORMAL_GHOST;
	selected = false;
}
Wire::Wire(Wire* oldWire) {
	int len = oldWire->x.size();
	x.resize(len);
	y.resize(len);
	pointSelected.resize(len);
	for (int j = 0; j < len; j++) {
		x[j] = oldWire->x[j];
		y[j] = oldWire->y[j];
		pointSelected[j] = oldWire->pointSelected[j];
	}
	selected = oldWire->selected;

	drivenByComponent = -1;
	drivenByOutputTerminal = -1;
}

void Wire::offset(int deltaX, int deltaY) {
	int len = x.size();
	for (int j = 0; j < len; j++) {
		x[j] += deltaX;
		y[j] += deltaY;
	}
}

void Wire::offsetPoints(int deltaX, int deltaY) {
	int len = pointSelected.size();
	for (int j = 0; j < len; j++) {
		if (pointSelected[j]) {
			x[j] += deltaX;
			y[j] += deltaY;
		}
	}
}


void Wire::moveWireEnd(int inx, int iny) {
	x[1] = inx;
	y[1] = iny;
}

void Wire::setPosition(int n, int inx, int iny) {
	x[n] = inx;
	y[n] = iny;
}



void Wire::draw(Sheet* pg) {

	int i;
	if (pointSelected.size() < 2)
		i = 0;
	
	COLORREF strokeStyle;
	switch (ghost) {
	case 3: // being learned - user must place it
		strokeStyle = 0x00FF00; // green lines
		break;
	case 4: // being learned - auto placement
		strokeStyle = 0xFFFF00; // yellow lines
		break;
	case 2: // invisible
		return;
	case 1: // gray
		strokeStyle = 0x808080; // ghostColor; // gray lines
		break;
	case 0: // normal
		break;
	}
	BeginPath(pg->hDC);
	// draw terminals  -- just the end points [0] and [1]
	Ellipse(pg->hDC, pg->cXo + x[0] - TERMINAL_SIZE, pg->cYo + y[0] - TERMINAL_SIZE,
		         pg->cXo + x[0] + TERMINAL_SIZE, pg->cYo + y[0] + TERMINAL_SIZE);

	Ellipse(pg->hDC, pg->cXo + x[1] - TERMINAL_SIZE, pg->cYo + y[1] - TERMINAL_SIZE,
		         pg->cXo + x[1] + TERMINAL_SIZE, pg->cYo + y[1] + TERMINAL_SIZE);

	// draw path [0] is the start, [1] is the end, [2]..[n] are between
	
	MoveToEx(pg->hDC, pg->cXo + x[0], pg->cYo + y[0], NULL);

	for (i = 2; i < x.size(); i++)
		LineTo(pg->hDC, pg->cXo + x[i], pg->cYo + y[i]);

	LineTo(pg->hDC, pg->cXo + x[1], pg->cYo + y[1]);
	EndPath(pg->hDC);

	bool drawBold = selected;
	if (!drawBold) {
		if (drivenByComponent != -1) {
			if (pg->dgrm.cL[drivenByComponent]->output[drivenByOutputTerminal].stringType) {
				if (pg->dgrm.cL[drivenByComponent]->output[drivenByOutputTerminal].stringValue.size() > 1)
					drawBold = true;
			}
			else {
				if (pg->dgrm.cL[drivenByComponent]->output[drivenByOutputTerminal].value.size() > 1)
					drawBold = true;
			}
		}
	}
	if(drawBold)
		SelectObject(pg->hDC, GetColorPen(0, ARRAY_THICK_LINE));
	else
		SelectObject(pg->hDC, GetColorPen(0, 1));
	StrokePath(pg->hDC);

	if(drawBold)
		SelectObject(pg->hDC, GetColorPen(0, 1));

	// DEBUG ONLY
#if 0
	BeginPath(hDC);
	for (i = 0; i < pointSelected.size(); i++) {
		if (pointSelected[i]) {
			MoveToEx(hDC, pg->cXo + x[i] + 10, pg->cYo + y[i] + 10, NULL);
			LineTo(hDC, pg->cXo + x[i] - 10, pg->cYo + y[i] - 10);
		}
	}
	EndPath(hDC);
	StrokePath(hDC);
#endif
}


std::string Wire::toString() {

	// write the Component details
	std::string result;

	result += "<W> ";

	if (x.size()) {
		result += "<x>";
		for (int j = 0; j < x.size(); j++) {
			result += tomString(x[j]);
			result += " ";
		}
		result += "</x> ";
	}
	if (y.size()) {
		result += " <y>";
		for (int j = 0; j < y.size(); j++) {
			result += tomString(y[j]);
			result += " ";
		}
		result += "</y> ";
	}
	if (ghost)
		result += "<g>1</g> ";

	result += "</W> \n";

	return result;
}


// ************************* Neaten wire *********************************

int NEATNESS_BAND = 8;

bool inNeatnessBand(int a, int b) {
	return abs(a - b) < NEATNESS_BAND;
}

bool Wire::neaten(int it, int* posXptr, int* posYptr) {
	bool horizontal = false;
	bool vertical = false;
	int ot;         // other terminal
	int itm, itp;   // terminal minus, terminal plus

	if (it == 0) {
		/* first point */
		if (x.size() == 2)
			ot = 1;
		else
			ot = 2;
		horizontal = inNeatnessBand(*posXptr, x[ot]);
		vertical = inNeatnessBand(*posYptr, y[ot]);

		if (horizontal == vertical) {
			return false;
		}

		if (horizontal) {
			*posXptr = x[ot];
			return true;
		}
		else if (vertical) {
			*posYptr = y[ot];
			return true;
		}
	}
	else if (it == 1) // remember last point on the wire (terminal) is [1]
	{
		/* last point */
		if (x.size() == 2)
			ot = 0;
		else
			ot = x.size() - 1;
		horizontal = inNeatnessBand(*posXptr, x[ot]);
		vertical = inNeatnessBand(*posYptr, y[ot]);

		if (horizontal == vertical) {
			return false;
		}
		if (horizontal) {
			*posXptr = x[ot];
			return true;
		}
		else if (vertical){
			*posYptr = y[ot];
			return true;
		}
	}
	else {
		/* some internal bend */
		itm = it - 1;
		itp = it + 1;

		if (it == 2)
			itm = 0;
		if (it == x.size() - 1)
			itp = 1;

		int newX = *posXptr;
		int newY = *posYptr;

		if (inNeatnessBand(*posXptr, x[itm]))
			newX = x[itm];
		else if (inNeatnessBand(*posXptr, x[itp]))
			newX = x[itp];

		if (inNeatnessBand(*posYptr, y[itm]))
			newY = y[itm];
		else if (inNeatnessBand(*posYptr, y[itp]))
			newY = y[itp];

		*posXptr = newX;
		*posYptr = newY;
		return true;
	}
	return false;
}

