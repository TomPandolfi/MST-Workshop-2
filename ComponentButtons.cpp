#include "stdafx.h"
#include "Button.h"
#include "buildComponent.h"
#include "ComponentButtons.h"


#pragma warning (disable: 4267)



int checkComponentButtonHit(Sheet* pg, int inX, int inY) 
{
	if (pg->activeDialogComponent == -1)
		return -1;
	inX -= pg->activeDialogOffsetX;
	inY -= pg->activeDialogOffsetY;

	int adc = pg->activeDialogComponent;

	// scan the basic buttons 
	for (int j = 0; j < 8; j++) {
		if (inX < pg->componentButtonList[j]->x)
			continue;
		if (inX > pg->componentButtonList[j]->x + pg->componentButtonList[j]->width)
			continue;
		if (inY < pg->componentButtonList[j]->y)
			continue;
		if (inY > pg->componentButtonList[j]->y + pg->componentButtonList[j]->height)
			continue;

		if(pg->componentButtonList[j]->action1)
			pg->componentButtonList[j]->action1(adc);
		if (pg->componentButtonList[j]->actionPageI)
			pg->componentButtonList[j]->actionPageI(pg, adc);
		return j;
	}

	// scan the component specific buttons
	int dialogButtonCount = pg->dgrm.cL[adc]->dialogButtonList.size();
	for (int j = 0; j < dialogButtonCount; j++) {
		if (inX < pg->dgrm.cL[adc]->dialogButtonList[j]->x)
			continue;
		if (inX > pg->dgrm.cL[adc]->dialogButtonList[j]->x + pg->dgrm.cL[adc]->dialogButtonList[j]->width)
			continue;
		if (inY < pg->dgrm.cL[adc]->dialogButtonList[j]->y)
			continue;
		if (inY > pg->dgrm.cL[adc]->dialogButtonList[j]->y + pg->dgrm.cL[adc]->dialogButtonList[j]->height)
			continue;

		if(pg->dgrm.cL[adc]->dialogButtonList[j]->action1)
			pg->dgrm.cL[adc]->dialogButtonList[j]->action1(adc);
		else if (pg->dgrm.cL[adc]->dialogButtonList[j]->actionPage)
			pg->dgrm.cL[adc]->dialogButtonList[j]->actionPage(pg);
		if (pg->dgrm.cL[adc]->dialogButtonList[j]->actionPageI)
			pg->dgrm.cL[adc]->dialogButtonList[j]->actionPageI(pg, adc);
	//	if (pg->dgrm.cL[adc]->dialogButtonList[j]->actionType)
			//pg->dgrm.cL[adc]->

		return j;
	}

	// missed
	pg->activeDialogComponent = -1;
	drawCanvas();
	return -1;
}


void loadComponentButtons(Sheet* pg) {
	int but = 0;
	pg->componentButtonList.push_back(new Button(3, -45, 85, 20, L"Delete", buttonBase_Delete_Action));
	pg->componentButtonList.push_back(new Button(3, -25, 85, 20, L"Details...", buttonBase_Details_Action));
	pg->componentButtonList.push_back(new Button(3, -5, 85, 20, L"Duplicate", buttonBase_Duplicate_Action));
	pg->componentButtonList.push_back(new Button(3, 15, 85, 20, L"Edit Label...", buttonBase_EditLabel_Action));

	pg->componentButtonList.push_back(new Button(-117, -45, 115, 20, L"Move To Top", buttonBase_MoveToTop_Action));
	pg->componentButtonList.push_back(new Button(-117, -25, 115, 20, L"Move Up", buttonBase_MoveUp_Action));
	pg->componentButtonList.push_back(new Button(-117, -5, 115, 20, L"Move Down", buttonBase_MoveDown_Action));
	pg->componentButtonList.push_back(new Button(-117, 15, 115, 20, L"Move To Bottom", buttonBase_MoveToBottom_Action));
}



