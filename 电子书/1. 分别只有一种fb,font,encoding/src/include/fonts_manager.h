#ifndef __FONTSMANGER_H
#define __FONTSMANGER_H

typedef struct font{
	char  *name;
	int iFontSizes;			//字体大小
	
	unsigned char *bitmap;	//像素缓冲区
	int pixelBit;			//一个像素占多少位
	int pitch;				//一行占多少字节
	
	int offset_left;		//像素到原点的距离
	int offset_top;			//像素到原点的距离
	int width;				//像素宽度
	int rows;				//像素行数

	int CurOriginX;			//当前原点x的位置
	int CurOriginY;			//当前原点y的位置
	
	int CurAdvanX;			//当前字的x方向长度
	
	int (*get_font_pixel)(struct font *ptFontOpr, int iCode);
	struct font *next_font;
}T_FONT, *PT_FONT;

int register_fonts(PT_FONT ptFontOpr);
PT_FONT find_fonts(char *ptFontName);
int freetype_init(char *pcFreeTypeFontFileName, int iFontSizes);
void font_init(char *pcFontFileName, int iFontSizes);

#endif
