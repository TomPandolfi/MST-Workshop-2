#pragma once

#include "structs.h"
#include <vector>
#include <string>


class Sheet;

class Wire {
public:

	int drivenByComponent;
	int drivenByOutputTerminal;

	std::vector<int> x;
	std::vector<int> y;
	std::vector<bool> pointSelected;

	bool selected;
	int attributionKey;

	int ghost;
	
	Wire();
	~Wire();

	Wire(int startX, int startY, int endX, int endY);
	Wire(Wire* oldWire);

	void offset(int deltaX, int deltaY);
	void offsetPoints(int deltaX, int deltaY);
	void moveWireEnd( int x, int y);
	void setPosition(int n, int x, int y);
	void draw(Sheet* pg);
	bool neaten(int it, int* posX, int* posY);

	std::string toString();
};
