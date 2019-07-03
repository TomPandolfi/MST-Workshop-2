#pragma once

#include "Sheet.h"

void loadSolution_(Sheet*, char *string, bool select);

void readLiteralGeneric(Sheet* pg, int i, int mode);
void readTableGeneric(Sheet* pg, int i);

void copyDiagram(Diagram* srcDgrm, Diagram* dstDgrm);
void switchToSheet(Sheet* pg, int sheetNumber);
void syncVisibleSheet(Sheet* pg);
void deepCopyDiagram(Diagram* srcDgrm, Diagram* dstDgrm);

string setFileEnding(string fileName, string fileEnding); 
bool confirmExtension(string fileName, string fileEnding0, string fileEnding1, string fileEnding2);

bool checkIsDirty(Sheet* pg);
void clearAllDirty();
void setVisibleScreenDirty(string reasonForChange);
void clearAllSaveBuffers();
