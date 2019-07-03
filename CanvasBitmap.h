#pragma once

#include "buildComponent.h"
#include "CanvasComponents.h"

class Canvas_Bitmap_Component : public Canvas_Component {
public:	Canvas_Bitmap_Component();
		bool dial2d(Sheet*);
		bool onLoad(Sheet*);
		vector<int> GetPinGroups();
		vector<string> GetPinLabels();
		
};