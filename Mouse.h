#pragma once

#include "Sheet.h"

void MouseDownEventListener(Sheet*, LPARAM lParam);
void MouseMoveEventListener(Sheet*, LPARAM lParam);
void MouseUpEventListener(Sheet*, LPARAM lParam);

void MouseDownRightEventListener(Sheet*, LPARAM lParam);
void MouseUpRightEventListener(Sheet*, LPARAM lParam);

void MouseDownMiddleEventListener(Sheet*, LPARAM lParam);

void KeyboardCharListener(Sheet*, WPARAM wParam);
void KeyboardKeydownListener(Sheet*, WPARAM wParam);

void deleteWire(Sheet* pg, int theWire);

int calculateDistancePoint2Line(int xp, int yp, int x1, int y1, int x2, int y2);