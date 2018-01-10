#include <stdio.h>
#include <string.h>

#include <config.h>
#include <encoding_manager.h>

static int getUtf16BeEncoding(unsigned int *pdwCode, unsigned char *pcFileCurPos, unsigned char *pcFileEndPos)				//返回拷贝字节数
{
	unsigned char c1 = *pcFileCurPos;
	unsigned char c2 = *(pcFileCurPos + 1);
	
	if((pcFileCurPos + 1) >= pcFileEndPos)
	{
		DEG_PRINTF("file end\n");
		return 0;
	}

	*pdwCode = c2 | (c1 << 8);
	
	return 2;
}

static int isSupportUtf16Be(unsigned char *pcFileHeadPos)
{
	char pcFlags[2] = {0xFE, 0xFF};	

	if (memcmp(pcFlags, pcFileHeadPos, 2) == 0)
	{
		return 0;					
	}

	return -1;
}

T_ENCODING g_tUtf16BeOpr = {
	.name = "utf_8",
	.isSupportThisEncoding = isSupportUtf16Be,
	.getEncoding		   = getUtf16BeEncoding,
	.file_head			   = 2,
};
int utf16Be_init(void)
{
	return register_encoding(&g_tUtf16BeOpr);
}

