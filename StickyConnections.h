#pragma once



class Term {
public:
	int index;
	int end;
	Term(int in_index, int in_end);
};



class Connection {
public:
	string name;
	int x;
	int y;
	vector<Term>term;
	int comp;
	Connection(string name, int x, int y, vector<Term>term, int comp);
};


void captureConnections(Diagram* dgrm, int i);
void updateConnections(Sheet* pg, int i);

void StartDraggingWires(Sheet* pg, int i);
void PullWiresWithComponent(Sheet* pg, int i);
void DisconnectWireDragging();

//void StartDraggingWires(Sheet* page, int initialX, int initialY);
void PullSelectedWires(Sheet* page, int currentX, int currentY);
