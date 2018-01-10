#include <fonts_manager.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <wchar.h>
#include <debug_manager.h>
#include <config.h>
#include <math.h>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library	  library;
FT_Face 	  face;

FT_GlyphSlot  slot;

static int get_freetype_font(PT_FONT ptFontOpr, int iCode)
{
	int iError;

	FT_Vector tPen;

	tPen.x = 0;
	tPen.y = 0;
	
	/*   */
	iError = FT_Load_Char(face, iCode, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);		/* 5. 加载并且渲染 FT_LOAD_MONOCHROME表示一个像素用1bit表示 */
	if (iError)
	{
		DEG_PRINTF("FT_Load_Char error!\n");
		return -1;
	}

	ptFontOpr->bitmap      = slot->bitmap.buffer;
	ptFontOpr->offset_left = slot->bitmap_left;
	ptFontOpr->offset_top  = slot->bitmap_top;
	ptFontOpr->width       = slot->bitmap.width;
	ptFontOpr->rows        = slot->bitmap.rows;

	ptFontOpr->pitch 	   = slot->bitmap.pitch;
	
	ptFontOpr->CurAdvanX   = slot->advance.x / 64;

	return 0;
}

static T_FONT g_tFreeTypeOpr = {
	.name           = "freetype",
	.pixelBit		= 1,
	.get_font_pixel = get_freetype_font,
};

static int freetype_font_init(char *pcFontFileName, int iFontSizes)
{
	int iError;
	
	g_tFreeTypeOpr.CurOriginX = 0;
	g_tFreeTypeOpr.CurOriginY = iFontSizes - 1;
	g_tFreeTypeOpr.iFontSizes = iFontSizes;

	iError = FT_Init_FreeType(&library);				  		 /* 1. initialize library */
	
	iError = FT_New_Face(library, pcFontFileName, 0, &face);   /* 2. create face object */
	
	slot = face->glyph;
	
	FT_Set_Pixel_Sizes(face, iFontSizes, 0);					  	 /* 3. set pixel sizes */
	
	return iError;
	
}

int freetype_init(char *pcFontFileName, int iFontSizes)
{
	int iError;

	iError = freetype_font_init(pcFontFileName, iFontSizes);
	if (iError)
		return iError;
	
	return register_fonts(&g_tFreeTypeOpr);
}
