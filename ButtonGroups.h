#pragma once

#if MEMORY_CHECK
void startMemCheck();
void endMemCheck();
#endif


HANDLE getDataFromClipboard();

void LoadButtonGroups(Sheet* pg);
void activateButtonGroup(Sheet* pg, int visibleGroup);
void drawGroupButtons(Sheet* pg, HDC hDC);
int checkGroupButtons(Sheet *pg, POS rawpos);
void deactivateAllButtons(Sheet *pg);
COLORREF GetButtonBackgroundColor(Sheet *pg, LPCTSTR componentName);
void deleteAll(Sheet* pg);
void innerDeleteAll(Sheet* pg);

int checkGroupButtonsAndShowDetails(Sheet* pg, POS rawpos);

void dimButtonsBasedKeySearch(Sheet* pg, string keySearchText);
void undimAllButtons(Sheet* pg);
void updateSheetButtonText(Sheet* pg, int sheet, string theName);

extern Button* magnetWiresButton;
extern Button* magnetComponentsButton;
//extern Button* stickyComponentsButton;
//extern Button* startWiresFromTerminalsButton;
extern Button* allowWiresToBendButton;
extern Button* enable3dDefaultLightingButton;
extern Button* showGraphPaperButton;
extern Button* keyboardSearch;

void copyStringToClipboard(string aString);

