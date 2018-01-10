#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include <linux/fb.h>

#include <disp_manager.h>
#include <config.h>

struct fb_info {
	int dot_len;
	int line_width;
	struct fb_var_screeninfo var;
	unsigned char *fb_mem;
};

static struct fb_info g_tFbInfo;

static int fb_info_get(void)
{	
	int iRet;
	int iScreenSize;
	int iFbFd;
	
	iFbFd = open(DISPLAY_DEVICE, O_RDWR);
	if (iFbFd < 0)
	{
		DEG_PRINTF("open DISPLAY_DEVICE fail!\n");
		return -1;
	}
	
	iRet = ioctl(iFbFd, FBIOGET_VSCREENINFO, &g_tFbInfo.var);
	if (iRet != 0)
	{
		DEG_PRINTF("can't get lcd info!\n");
		return -1;
	}
	
	iScreenSize = g_tFbInfo.var.xres * g_tFbInfo.var.yres * g_tFbInfo.var.bits_per_pixel / 8;
	g_tFbInfo.dot_len    = g_tFbInfo.var.bits_per_pixel / 8;
	g_tFbInfo.line_width = g_tFbInfo.var.xres * g_tFbInfo.var.bits_per_pixel / 8;  

	g_tFbInfo.fb_mem  = mmap(NULL, iScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, iFbFd, 0);
	if (g_tFbInfo.fb_mem < 0)
	{
		DEG_PRINTF("can't mmap fb_mem!\n");
		return -1;
	}

	return 0;
}

void fb_show_pixel(int x, int y, unsigned int iColor)
{
	int iFbShowAddr = g_tFbInfo.line_width * y + g_tFbInfo.dot_len * x;
	unsigned char  *ucFb8  = g_tFbInfo.fb_mem + iFbShowAddr;
	unsigned short *ucFb16 = (unsigned short *)ucFb8;
	unsigned int   *ucFb32 = (unsigned int   *)ucFb8;
	unsigned short usColor16;
	unsigned int iRed, iGreen, iBlue;

	if ((x >= g_tFbInfo.var.xres) || (y >= g_tFbInfo.var.yres))
	{
		DEG_PRINTF("Over screen!\n");
		return;
	}
	
	switch(g_tFbInfo.var.bits_per_pixel)
	{
		case 8:
		{
			*ucFb8 = (unsigned char)iColor;
			break;
		}
		case 16:		/* red:green:blue比例565 只要最高位 */
		{
			iRed   = (iColor >> 16) & 0xff;
			iGreen = (iColor >> 8) & 0xff;
			iBlue  = (iColor >> 0) & 0xff;
			usColor16 = ((iRed >> 3) << 11) | ((iGreen >> 2) << 5) | ((iBlue >> 3) << 0);
			*ucFb16   = usColor16;
			break;
		}
		case 32:
		{
			*ucFb32 = iColor;
			break;
		}
		default:
		{
			DEG_PRINTF("can't support %d\n", g_tFbInfo.var.bits_per_pixel);
			break;
		}
	}

}

static void fb_close(void)
{
	return;
}

static T_DISP g_tFbOpr = {
	.name       = "fb",
	.show_pixel = fb_show_pixel,	
	.closeDisp  = fb_close,
};


int fb_init(void)
{
	int iError;
	
	iError = fb_info_get();
	if (iError)
		return iError;

	g_tFbOpr.xres = g_tFbInfo.var.xres;
	g_tFbOpr.yres = g_tFbInfo.var.yres;
	
	return register_disp(&g_tFbOpr);
}
