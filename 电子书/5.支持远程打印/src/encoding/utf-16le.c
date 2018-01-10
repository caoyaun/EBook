#include <stdio.h>
#include <string.h>
#include <debug_manager.h>
#include <config.h>
#include <encoding_manager.h>

static int getUtf16LeEncoding(unsigned int *pdwCode, unsigned char *pcFileCurPos, unsigned char *pcFileEndPos)				//返回拷贝字节数
{
	unsigned char c1 = *pcFileCurPos;
	unsigned char c2 = *(pcFileCurPos + 1);
	
	if((pcFileCurPos + 1) >= pcFileEndPos)
	{
		DEG_PRINTF("file end\n");
		return 0;
	}

	*pdwCode = c1 | (c2 << 8);
	
	return 2;
}

static int isSupportUtf16Le(unsigned char *pcFileHeadPos)
{
	char pcFlags[2] = {0xFF, 0xFE};
	
	if (memcmp(pcFlags, pcFileHeadPos, 2) == 0)
	{
		return 0;					//文件头有三个字节
	}

	return -1;
}

T_ENCODING g_tUtf16LeOpr = {
	.name = "utf_16le",
	.isSupportThisEncoding = isSupportUtf16Le,
	.getEncoding		   = getUtf16LeEncoding,
	.file_head			   = 2,
};
int utf16Le_init(void)
{
	return register_encoding(&g_tUtf16LeOpr);
}

