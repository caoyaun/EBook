#include <stdio.h>
#include <string.h>
#include <fonts_manager.h>

static PT_FONT g_ptFontHead;

int register_fonts(PT_FONT ptFontOpr)
{
	
	PT_FONT ptFontTemp;

	if (g_ptFontHead == NULL)
	{
		g_ptFontHead = ptFontOpr;
	}
	else
	{
		ptFontTemp = g_ptFontHead;
			
		while(ptFontTemp->next_font)
		{
			ptFontTemp = ptFontTemp->next_font;
		}

		ptFontTemp->next_font = ptFontOpr;
	}

	return 0;
}

PT_FONT find_fonts(char *ptFontName)
{
	PT_FONT ptFontTemp;

	ptFontTemp = g_ptFontHead;
	while(ptFontTemp)
	{
		if (strcmp(ptFontTemp->name, ptFontName) == 0)
		{
			return ptFontTemp;
		}
		else
			ptFontTemp = ptFontTemp->next_font;
	}

	return NULL;
	
}

void font_init(char *pcFreeTypeFontFileName, char *pcGbkFontFileName, int iFontSizes)
{
	freetype_init(pcFreeTypeFontFileName, iFontSizes);
	gbkFont_init(pcGbkFontFileName);
}
