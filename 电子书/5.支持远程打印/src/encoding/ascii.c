#include <stdio.h>
#include <string.h>
#include <debug_manager.h>
#include <config.h>
#include <encoding_manager.h>

static int getGbkEncoding(unsigned int *pdwCode, unsigned char *pcFileCurPos, unsigned char *pcFileEndPos)
{
	unsigned char c1 = *pcFileCurPos;
	unsigned char c2 = *(pcFileCurPos + 1);
	
	if (c1 & 0x80)				//非字母
	{
		if((pcFileCurPos + 1) >= pcFileEndPos)
		{
			DEG_PRINTF("file end\n");
			return 0;
		}
		
		*pdwCode = c1 | (c2 << 8);
		
		return 2;
	}
	else						//字母
	{
		/* 越界了 */
		if(pcFileCurPos >= pcFileEndPos)
		{
			DEG_PRINTF("file end\n");
			return 0;
		}
		
		*pdwCode = *pcFileCurPos;
		return 1;
	}
	
}

static int isSupportGbk(unsigned char *pcFileHeadPos)
{
	/* 本程序只支持这几种编码方式，不是utf系列就是gbk */
	char pcFlagsUtf8[3] = {0xEF, 0xBB, 0xBF};
	char pcFlagsUtf16Le[2] = {0xFF, 0xFE};
	char pcFlagsUtf16Be[2] = {0xFE, 0xFF};
	
	if (memcmp(pcFlagsUtf8, pcFileHeadPos, 3) == 0)
	{
		return -1;					
	}
	if (memcmp(pcFlagsUtf16Le, pcFileHeadPos, 2) == 0)
	{
		return -1;					
	}
	if (memcmp(pcFlagsUtf16Be, pcFileHeadPos, 2) == 0)
	{
		return -1;					
	}

	return 0;
}

T_ENCODING g_tGbkOpr = {
	.name = "gbk",
	.isSupportThisEncoding = isSupportGbk,
	.getEncoding		   = getGbkEncoding,
	.file_head			   = 0,
};
int ascii_init(void)
{
	return register_encoding(&g_tGbkOpr);
}



