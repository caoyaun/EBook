#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <linux/fb.h>

#include <config.h>

#include <disp_manager.h>
#include <fonts_manager.h>
#include <encoding_manager.h>

typedef struct configure {
	PT_DISP disp_select;
	PT_FONT font_select;
	PT_ENCODING encoding_select;
}T_CONFIG;

typedef struct pageinfo {
	unsigned char *g_pcFileDataPos;
	struct pageinfo *nextPage;
	struct pageinfo *prePage;
}T_PAGEINFO, *PT_PAGEINFO;

static T_CONFIG g_tConfig;

static PT_PAGEINFO  g_ptCurPage;
static unsigned char *g_pcFileDataCurPos;
static unsigned char *g_pcFileDataHeadMem;
static unsigned char *g_pcFileDataTailMem;

void system_init(char *pcFreeTypeFontFileName, char *pcGbkFontFileName, int iFontSizes)
{

	/* 显示器初始化 */
	display_init();
	
	/* 编码初始化 */
	encoding_init();

	/* 字体初始化 */
	font_init(pcFreeTypeFontFileName, pcGbkFontFileName, iFontSizes);
}

void getConfig(char *pcDisName, char *pcFileName)
{
	int iFileFd;
	struct stat file_stat;
	
	/* 打开文本文件并且映射到内存 */
	iFileFd = open(pcFileName, O_RDONLY);
	fstat(iFileFd, &file_stat);

	g_pcFileDataHeadMem = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, iFileFd, 0);

	g_pcFileDataTailMem = g_pcFileDataHeadMem + file_stat.st_size;

	/* 根据传进来的显示器名称来选择显示器 */
	g_tConfig.disp_select = find_displayer(pcDisName);
	if (g_tConfig.disp_select == NULL)
	{
		DEG_PRINTF("g_tConfig.disp_select == NULL\n");
		return;
	}
	
	/* 根据文件头的信息判断文件是何种编码，并把g_pcFileMem加上文件头大小（指向数据头） */
	g_tConfig.encoding_select = find_support_encoding(g_pcFileDataHeadMem);
	if (g_tConfig.encoding_select == NULL)
	{
		DEG_PRINTF("g_tConfig.encoding_select == NULL\n");
		return;
	}
	g_pcFileDataHeadMem = g_pcFileDataHeadMem + g_tConfig.encoding_select->file_head;

	/* 根据编码格式选择字体 */
	if (strcmp(g_tConfig.encoding_select->name, "utf_8") == 0 		||	\
		strcmp(g_tConfig.encoding_select->name, "utf_16le") == 0	||  \
		strcmp(g_tConfig.encoding_select->name, "utf_16be") == 0)
	{
		g_tConfig.font_select = find_fonts("freetype");
		if (g_tConfig.font_select == NULL)
		{
			DEG_PRINTF("g_tConfig.font_select == NULL\n");
			return;
		}
	}	
	else if (strcmp(g_tConfig.encoding_select->name, "gbk") == 0)
	{
		g_tConfig.font_select = find_fonts("gbk");
		if (g_tConfig.font_select == NULL)
		{
			DEG_PRINTF("g_tConfig.font_select == NULL\n");
			return;
		}
	}
	else
	{
		DEG_PRINTF("g_tConfig.font_select == NULL\n");
		return;
	}
}

void cleanScreen(int iColor)
{
	int i,j;
	
	for(i = 0; i < g_tConfig.disp_select->yres; i++)
		for(j = 0; j < g_tConfig.disp_select->xres; j++)	
			g_tConfig.disp_select->show_pixel(j, i, iColor);
}

void showOneFont(void)
{	
	int i,j;
	int iLeft;
	int iTop;
	int iCount = 0;
	int iBit;
	unsigned char ucPixel = 0;

	iLeft = g_tConfig.font_select->CurOriginX + g_tConfig.font_select->offset_left;
	iTop  = g_tConfig.font_select->CurOriginY - g_tConfig.font_select->offset_top;

	if(g_tConfig.font_select->pixelBit == 8)	/* 如果一个像素用1byte表示 */
	{		
		for(i = iTop; i < (iTop + g_tConfig.font_select->rows); i++)
			for(j = iLeft, iBit = 7; j < (iLeft + g_tConfig.font_select->width); j++)
			{	
				if(g_tConfig.font_select->bitmap[(i - iTop) * g_tConfig.font_select->width + (j - iLeft)])
					g_tConfig.disp_select->show_pixel(j, i, FONTS_COLOR);
			}
	}
	else if(g_tConfig.font_select->pixelBit == 1)	/* 如果一个像素用1bit表示 */
	{		
		for(i = iTop; i < (iTop + g_tConfig.font_select->rows); i++)
		{

			/* 加这一句的目的是有一些字体虽然一行占了很多个字节，但是width还是比占用的少 */
			iCount = (i - iTop) * g_tConfig.font_select->pitch;
			
			for(j = iLeft, iBit = 7; j < (iLeft + g_tConfig.font_select->width); j++)
			{				
				if(iBit == 7)
				{
					ucPixel = g_tConfig.font_select->bitmap[iCount++];
				}
				
				if((ucPixel >> iBit) & 1)
					g_tConfig.disp_select->show_pixel(j, i, FONTS_COLOR);
	
				iBit--;
	
				if (iBit == -1)
					iBit = 7;
			}

		}
		
	}
	else
	{
		DEG_PRINTF("don't support %d pixelBit", g_tConfig.font_select->pixelBit);
		return;
	}
}

/* 把g_ptCurPage显示出来 */
void showOnepage(void)
{
	unsigned int dwCode;
	int iError;
	int iXEnd;

	g_pcFileDataCurPos = g_ptCurPage->g_pcFileDataPos;
	
	g_tConfig.font_select->CurOriginX = 0;
	g_tConfig.font_select->CurOriginY = g_tConfig.font_select->iFontSizes - 1;

	/* 显示前一页前先清屏 */
	cleanScreen(PAPER_COLOR);

	while(1)
	{
		/* 1. 获取编码 */
		iError = g_tConfig.encoding_select->getEncoding(&dwCode, g_pcFileDataCurPos, g_pcFileDataTailMem);
		g_pcFileDataCurPos += iError;

		/* 如果越界了 */
		if(iError == 0)
		{
			return;
		}

		switch(dwCode)
		{
			case '\n':				//换行
			{
				g_tConfig.font_select->CurOriginX = 0;
				if (g_tConfig.font_select->rows - g_tConfig.font_select->offset_top > 0)
					g_tConfig.font_select->CurOriginY = g_tConfig.font_select->CurOriginY + g_tConfig.font_select->rows - g_tConfig.font_select->offset_top + g_tConfig.font_select->iFontSizes;
				else
					g_tConfig.font_select->CurOriginY = g_tConfig.font_select->CurOriginY + g_tConfig.font_select->iFontSizes;
					
				continue;
				break;
			}
			case '\t':				//tab变为空格
			{
				dwCode = 20;		
				break;
			}
			case '\r':				//windows下的换行符(和\n同时出现，不用管)
			{
				continue;
				break;
			}
		}

		/* 2. 根据编码获取字体点阵 */
		g_tConfig.font_select->get_font_pixel(g_tConfig.font_select, dwCode);

		/* 3. 显示 */
		iXEnd = g_tConfig.font_select->CurOriginX + g_tConfig.font_select->CurAdvanX;
		if (iXEnd > g_tConfig.disp_select->xres)    												//换行
		{
			g_tConfig.font_select->CurOriginX = 0;
			if (g_tConfig.font_select->rows - g_tConfig.font_select->offset_top > 0)
				g_tConfig.font_select->CurOriginY = g_tConfig.font_select->CurOriginY + g_tConfig.font_select->rows - g_tConfig.font_select->offset_top + g_tConfig.font_select->iFontSizes;
			else
				g_tConfig.font_select->CurOriginY = g_tConfig.font_select->CurOriginY + g_tConfig.font_select->iFontSizes;
		}
		if (g_tConfig.font_select->CurOriginY > (g_tConfig.disp_select->yres - 1))					//换页
		{
			g_tConfig.font_select->CurOriginX = 0;
			g_tConfig.font_select->CurOriginY = g_tConfig.font_select->iFontSizes - 1;
			
			g_pcFileDataCurPos -= iError;
			
			return;
		}

		showOneFont();
		
		g_tConfig.font_select->CurOriginX = g_tConfig.font_select->CurOriginX + g_tConfig.font_select->CurAdvanX;		//重设x原点
	}
}

void showNextPage(void)
{	
	PT_PAGEINFO ptPageInfoTemp;
	
	if (g_ptCurPage == NULL)							//第一次显示
	{		
		g_ptCurPage = malloc(sizeof(T_PAGEINFO));
		if (g_ptCurPage == NULL)
		{
			DEG_PRINTF("malloc g_ptCurPage fail\n");
			return;
		}	
		g_ptCurPage->g_pcFileDataPos = g_pcFileDataHeadMem;
		g_ptCurPage->nextPage = NULL;
		g_ptCurPage->prePage = NULL;
	}	
	else if(g_ptCurPage->nextPage != NULL)				//下一页不是空的
	{
		g_ptCurPage = g_ptCurPage->nextPage;
	}
	else if(g_pcFileDataCurPos >= g_pcFileDataTailMem)	//后面没有内容了
	{
		DEG_PRINTF("it is tail!\n");
		return;
	}
	else											    //创建新页
	{		
		ptPageInfoTemp = g_ptCurPage;
		g_ptCurPage = malloc(sizeof(T_PAGEINFO));
		if (g_ptCurPage == NULL)
		{
			DEG_PRINTF("malloc g_ptCurPage fail\n");
			return;
		}	
		g_ptCurPage->g_pcFileDataPos = g_pcFileDataCurPos;
		g_ptCurPage->nextPage = NULL;
		g_ptCurPage->prePage = ptPageInfoTemp;
		ptPageInfoTemp->nextPage = g_ptCurPage;
	}

	showOnepage();
}

void showPrePage(void)
{	
	if ((g_ptCurPage == NULL) || (g_ptCurPage->prePage == NULL))
	{
		DEG_PRINTF("showPrePage error, g_ptCurPage/g_ptCurPage->prePage == NULL\n");
		return;
	}
	else 
	{		
		g_ptCurPage = g_ptCurPage->prePage;
		showOnepage();
	}
}
