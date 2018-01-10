#ifndef __DRAW_H
#define __DRAW_H

void system_init(char *pcFreeTypeFontFileName, char *pcGbkFontFileName, int iFontSizes);
void getConfig(char *pcDisName, char *pcFileName);
void showNextPage(void);
void showPrePage(void);
void closeDraw(void);

#endif
