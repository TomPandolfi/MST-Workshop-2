#include "stdafx.h"
#include <vector>
#include "Sheet.h"
#include "Wire.h"
#include "StickyConnections.h"

#pragma warning (disable: 4267)



Term::Term(int in_index, int in_end) {
	index = in_index;
	end = in_end;
}

Connection::Connection(string in_name, int in_x, int in_y, vector<Term>in_term, int in_comp) {
	name = in_name;
	x = in_x;
	y = in_y;
	term = in_term;
	comp = in_comp;
}


vector<Connection>inputs;
vector<Connection>outputs;

void captureConnections(Diagram* dgrm, int i) {
	
	inputs.clear();
	vector<Term> terminals;

	int xx = dgrm->cL[i]->x - dgrm->cL[i]->width - 1 - TERMINAL_SIZE;
	int yy = dgrm->cL[i]->y - (TERMINAL_STEP * (dgrm->cL[i]->leftTerminalCount - 1) *dgrm->cL[i]->leftTerminalStep) / 2;
	for (int k = 0; k < dgrm->cL[i]->input.size(); k++) {
		// search for wires connected to this input terminal
		terminals.clear();
		for (int w = 0; w < dgrm->wL.size(); w++) {
			if (checkComponentInputToWire(dgrm, i, k, w, 0))
				terminals.push_back(Term(w, 0));
			else if (checkComponentInputToWire(dgrm, i, k, w, 1))
				terminals.push_back(Term(w, 1));
		}
		// search for simple components connected to this input terminal (no inputs, single output)
		int comp = -1;
		for (int c = 0; c < dgrm->cL.size(); c++) {
			if (c == i) continue;
			if (dgrm->cL[c]->leftTerminalCount != 0) continue;
			if (dgrm->cL[c]->rightTerminalCount != 1) continue;
			if (checkOverlap(dgrm, i, k, c, 0)) {
				comp = c;
				break; // can only have 1 output to an input
			}
		}
		inputs.push_back(Connection(dgrm->cL[i]->input[k].label, xx, yy, terminals, comp));

		yy += TERMINAL_STEP * dgrm->cL[i]->leftTerminalStep;
	}

	outputs.clear();
	xx = dgrm->cL[i]->x + dgrm->cL[i]->width + 1 + TERMINAL_SIZE;
	yy = dgrm->cL[i]->y - (TERMINAL_STEP * (dgrm->cL[i]->rightTerminalCount - 1) * dgrm->cL[i]->rightTerminalStep) / 2;
	for (int k = 0; k < dgrm->cL[i]->output.size(); k++) {
		// search for wires connected to this output terminal
		terminals.clear();
		for (int w = 0; w < dgrm->wL.size(); w++) {
			if (checkComponentOutputToWire(dgrm, i, k, w, 0))
				terminals.push_back(Term(w, 0));
			else if (checkComponentOutputToWire(dgrm, i, k, w, 1))
				terminals.push_back(Term(w, 1));
		}
		// search for simple components connected to this output terminal (single input, no outputs)
		int comp = -1;
		for (int c = 0; c < dgrm->cL.size(); c++) {
			if (c == i) continue;
			if (dgrm->cL[c]->rightTerminalCount != 0) continue;
			if (dgrm->cL[c]->leftTerminalCount != 1) continue;
			if (checkOverlap(dgrm, c, 0, i, k)) {
				comp = c;
				break; // can only have 1 output to component
			}
		}
		outputs.push_back(Connection(dgrm->cL[i]->output[k].label, xx, yy, terminals, comp));
		
		yy += TERMINAL_STEP * dgrm->cL[i]->rightTerminalStep;
	}
}

void updateConnections(Sheet* page, int i) {
	// first fix the inputs
	int xx =page->dgrm.cL[i]->x -page->dgrm.cL[i]->width - 1 - TERMINAL_SIZE;
	for (int k = 0; k < inputs.size(); k++) {
		if ((inputs[k].term.size() > 0) || (inputs[k].comp != -1)) {
			// find the terminal after the move
			bool foundIt = false;
			for (int newTerm = 0; newTerm <page->dgrm.cL[i]->input.size(); newTerm++) {
				if (page->dgrm.cL[i]->input[newTerm].label == inputs[k].name) {
					foundIt = true;
					//  move all the wires
					int yy =page->dgrm.cL[i]->y - (TERMINAL_STEP * (page->dgrm.cL[i]->leftTerminalCount - 1) *page->dgrm.cL[i]->leftTerminalStep) / 2;
					yy += newTerm * TERMINAL_STEP *page->dgrm.cL[i]->leftTerminalStep;
					for (int conn = 0; conn < inputs[k].term.size(); conn++) {
						// move the wire terminal to match the new pin
						int w = inputs[k].term[conn].index;
						int e = inputs[k].term[conn].end;
						page->dgrm.wL[w]->x[e] += xx - inputs[k].x;
						page->dgrm.wL[w]->y[e] += yy - inputs[k].y;
					}
					if (inputs[k].comp != -1) {
						int c = inputs[k].comp;
						page->dgrm.cL[c]->x += xx - inputs[k].x;
						page->dgrm.cL[c]->y += yy - inputs[k].y;
					}
					break;
				}
			}
			if (!foundIt) {
				// move the wire terminal to the left to show its disconnected
				for (int conn = 0; conn < inputs[k].term.size(); conn++) {
					// move the wire away from all pins
					int w = inputs[k].term[conn].index;
					int e = inputs[k].term[conn].end;
					page->dgrm.wL[w]->x[e] -= TERMINAL_STEP;
				}
				if (inputs[k].comp != -1) {
					int c = inputs[k].comp;
					page->dgrm.cL[c]->x -= TERMINAL_STEP;
				}
			}
		}
	}
	// now fix the outputs
	xx =page->dgrm.cL[i]->x +page->dgrm.cL[i]->width + 1 + TERMINAL_SIZE;
	for (int k = 0; k < outputs.size(); k++) {
		if ((outputs[k].term.size() > 0) || (outputs[k].comp != -1)) {
			// find the terminal after the move
			bool foundIt = false;
			for (int newTerm = 0; newTerm <page->dgrm.cL[i]->output.size(); newTerm++) {
				if (page->dgrm.cL[i]->output[newTerm].label == outputs[k].name) {
					foundIt = true;
					//  move all the wires
					int yy =page->dgrm.cL[i]->y - (TERMINAL_STEP * (page->dgrm.cL[i]->rightTerminalCount - 1) *page->dgrm.cL[i]->rightTerminalStep) / 2;
					yy += newTerm * TERMINAL_STEP *page->dgrm.cL[i]->rightTerminalStep;
					for (int conn = 0; conn < outputs[k].term.size(); conn++) {
						// move the wire terminal to match the new pin
						int w = outputs[k].term[conn].index;
						int e = outputs[k].term[conn].end;
						page->dgrm.wL[w]->x[e] += xx - outputs[k].x;
						page->dgrm.wL[w]->y[e] += yy - outputs[k].y;
					}
					if (outputs[k].comp != -1) {
						int c = outputs[k].comp;
						page->dgrm.cL[c]->x += xx - outputs[k].x;
						page->dgrm.cL[c]->y += yy - outputs[k].y;
					}
					break;
				}
			}
			if (!foundIt) {
				// move the wire terminal to the right to show its disconnected
				for (int conn = 0; conn < outputs[k].term.size(); conn++) {
					// move the wire away from all pins
					int w = outputs[k].term[conn].index;
					int e = outputs[k].term[conn].end;
					page->dgrm.wL[w]->x[e] += TERMINAL_STEP;
				}
				if (outputs[k].comp != -1) {
					int c = outputs[k].comp;
					page->dgrm.cL[c]->x += TERMINAL_STEP;
				}
			}
		}
	}
}


// ******************************** dragging wires ********************************

int connectedStartX;
int connectedStartY;
bool connected = false;

int connectedGroupStartX;
int connectedGroupStartY;
bool connectedGroup = false;


// shared with the following
void DisconnectWireDragging() {
	connected = false;
	connectedGroup = false;
}

// ******************************* dragging the wires connected to a single component ****************

vector<int> connectedWireNumber;
vector<int> connectedWireEnd;

void StartDraggingWires(Sheet* pg, int i) {
	connectedStartX = pg->dgrm.cL[i]->x;
	connectedStartY = pg->dgrm.cL[i]->y;

	connectedWireNumber.clear();
	connectedWireEnd.clear();
	connected = true;
	
	for (int it = 0; it < pg->dgrm.cL[i]->rightTerminalCount; it++) {
		for (int wn = 0; wn < pg->dgrm.wL.size(); wn++) {
			for (int wt = 0; wt < 2; wt++) {
				if (checkComponentOutputToWire(&pg->dgrm, i, it, wn, wt)) {
					connectedWireNumber.push_back(wn);
					connectedWireEnd.push_back(wt);
				}
			}
		}
	}

	for (int it = 0; it < pg->dgrm.cL[i]->leftTerminalCount; it++) {
		for (int wn = 0; wn < pg->dgrm.wL.size(); wn++) {
			for (int wt = 0; wt < 2; wt++) {
				if (checkComponentInputToWire(&pg->dgrm, i, it, wn, wt)) {
					connectedWireNumber.push_back(wn);
					connectedWireEnd.push_back(wt);
				}
			}
		}
	}
}

void PullWiresWithComponent(Sheet* page, int i) {
	if (!connected)
		return;

	// pull the wires with the component
	int deltaX = page->dgrm.cL[i]->x - connectedStartX;
	int deltaY = page->dgrm.cL[i]->y - connectedStartY;
	for (int k = 0; k < connectedWireNumber.size(); k++) {
		// move each wire
		int w = connectedWireNumber[k];
		int e = connectedWireEnd[k];
		page->dgrm.wL[w]->x[e] += deltaX;
		page->dgrm.wL[w]->y[e] += deltaY;
	}
	connectedStartX = page->dgrm.cL[i]->x;
	connectedStartY = page->dgrm.cL[i]->y;
}



// **************************** Dragging selected wires ****************************

void PullSelectedWires(Sheet* page, int currentX, int currentY) {
	if (!connectedGroup)
		return;

	int deltaX = currentX - connectedGroupStartX;
	int deltaY = currentY - connectedGroupStartY;

	// pull the wires 
	int len = page->dgrm.wL.size();
	for (int w = 0; w < len; w++) {
		if (page->dgrm.wL[w]->selected) {
			// move the whole wire
			for (int j = 0; j < page->dgrm.wL[w]->x.size(); j++) {
				page->dgrm.wL[w]->x[j] += deltaX;
				page->dgrm.wL[w]->y[j] += deltaY;
			}
		}
	}
	connectedGroupStartX = currentX;
	connectedGroupStartY = currentY;
}

