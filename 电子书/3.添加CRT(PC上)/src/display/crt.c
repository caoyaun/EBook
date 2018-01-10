#include <stdlib.h>
#include <vga.h>
#include <vgagl.h>

#include <disp_manager.h>
#include <config.h>

static unsigned int g_acColor[256];
static unsigned int g_iCurCount;

static void crt_show_pixel(int x, int y, unsigned int iColor)
{
	int iRed, iBule, iGreen;
	int i;

	if ((x >= 320) || (y >= 240))
	{
		DEG_PRINTF("Over screen!\n");
		return;
	}
	
	if (g_iCurCount == 0)				//这段代码效率很低，做实验可以，实际不要这么做
	{
		g_acColor[0] = iColor;
		i = g_iCurCount;
		g_iCurCount++;
	}
	else
	{
		for (i = 0; i < g_iCurCount; i++)	//如果g_acColor中有这颜色
		{
			if (g_acColor[i] == iColor)
				break;
		}

		if (i >= g_iCurCount)				//如果g_acColor中没有这颜色,则添加
		{
			g_iCurCount++;
			
			if (g_iCurCount > 256)
			{
				g_iCurCount--；
				DEG_PRINTF("Palettecolor full!\n");
				return;
			}
			
			g_acColor[i] = iColor;
		}
	}
	
	iRed   = (iColor >> 16) & 0xff;
	iGreen = (iColor >> 8) & 0xff;
	iBule  = (iColor >> 0) & 0xff;
	
	gl_setpalettecolor(i, iRed >> 2, iGreen >> 2, iBule >> 2);	
	vga_setcolor(i);
	vga_drawpixel(x, y);
}

static void crtClose(void)
{
	vga_setmode(TEXT);
}

static T_DISP g_tCrtOpr = {
	.name       = "crt",
	.show_pixel = crt_show_pixel,	
	.closeDisp  = crtClose,
};

static void vgaInit(void)
{
	vga_init();
	vga_setmode(G320x240x256);
	g_tCrtOpr.xres = 320;
	g_tCrtOpr.yres = 240;
}

int crt_init(void)
{
	vgaInit();
	return register_disp(&g_tCrtOpr);
}

