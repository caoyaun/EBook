#include <fonts_manager.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <debug_manager.h>
#include <config.h>
#include <font_8x16.h>

unsigned char *pucHzkMem;

static void getChineseFont(struct font *ptFontOpr, int iCode)
{
	unsigned char ucHzkL;
	unsigned char ucHzkH;
	
	int iArea;
	int iWhere;
	
	int iHzkPos;
		
	ucHzkL = iCode & 0xff;
	ucHzkH = iCode >> 8;
	
	iArea    = ucHzkL - 0xa1;
	iWhere   = ucHzkH - 0xa1;
	
	iHzkPos = (iArea * 94 + iWhere) * 32;
	
	ptFontOpr->bitmap      = (unsigned char *)&pucHzkMem[iHzkPos];
	ptFontOpr->offset_left = 0;
	ptFontOpr->offset_top  = 15;
	ptFontOpr->width       = 16;
	ptFontOpr->rows        = 16;

	ptFontOpr->pitch 	   = 2;
	
	ptFontOpr->CurAdvanX   = 16;
}

static void getEnglishFont(struct font *ptFontOpr, int iCode)
{
	unsigned char *c_pos = (unsigned char *)&fontdata_8x16[iCode * 16];

	ptFontOpr->bitmap      = c_pos;
	ptFontOpr->offset_left = 0;
	ptFontOpr->offset_top  = 15;
	ptFontOpr->width       = 8;
	ptFontOpr->rows        = 16;

	ptFontOpr->pitch 	   = 1;
	
	ptFontOpr->CurAdvanX   = 8;
}

static int get_gbk_font(struct font *ptFontOpr, int iCode)
{	
	if (iCode & 0x80)		//如果是汉字
	{
		getChineseFont(ptFontOpr, iCode);
	}
	else					//如果是字母
	{
		getEnglishFont(ptFontOpr, iCode);
	}

	return 0;
}

static T_FONT g_tGbkFont = {
	.name           = "gbk",
	.pixelBit		= 1,
	.get_font_pixel = get_gbk_font,
};

static void gbkFontFileInit(char *pcGbkFontFileName)
{	
	int iHzkFd;
	
	struct stat tHzkStat;
	
	g_tGbkFont.CurOriginX = 0;
	g_tGbkFont.CurOriginY = 15;
	g_tGbkFont.iFontSizes = 16;
		
	iHzkFd = open(pcGbkFontFileName, O_RDONLY);
	if (iHzkFd < 0)
	{
		DEG_PRINTF("open %s fail!\n", pcGbkFontFileName);
		return;
	}
	
	if (fstat(iHzkFd, &tHzkStat))		//能获取文件的信息
	{
		DEG_PRINTF("get stat fail!\n");
		return;
	}
	
	pucHzkMem = mmap(NULL, tHzkStat.st_size, PROT_READ, MAP_SHARED, iHzkFd, 0);
	if (pucHzkMem < 0)
	{
		DEG_PRINTF("can't get mmap pucHzkMem!\n");
		return;
	}

}

int gbkFont_init(char *pcGbkFontFileName)
{
	gbkFontFileInit(pcGbkFontFileName);

	return register_fonts(&g_tGbkFont);
}

