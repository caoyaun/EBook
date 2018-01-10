#include <stdio.h>
#include <string.h>

#include <config.h>
#include <encoding_manager.h>

/* 获得前导1的个数 */
static int getPreOneBit(unsigned char c)
{
	int iError = 0;
	int i;
	
	for(i = 7; i >= 0; i--)
	{
		if((c >> i) & 1)
			iError++;
		else
			break;
	}

	return iError;
}

static int getUtf8Encoding(unsigned int *pdwCode, unsigned char *pcFileCurPos, unsigned char *pcFileEndPos)				//返回拷贝字节数
{
	int iError;
	int iCodeTemp;
	int i;
	unsigned char c = *pcFileCurPos;

	if (c & 0x80)				//非字母
	{
		iError = getPreOneBit(c);

		/* 越界了 */
		if((pcFileCurPos + iError - 1) >= pcFileEndPos)
		{
			DEG_PRINTF("file end\n");
			return 0;
		}
		
		c <<= iError;		
		iCodeTemp = c >> iError;
		
		for(i = 1; i < iError; i++)
		{
			c = pcFileCurPos[i] & 0x3f;
			iCodeTemp = (iCodeTemp << 6) | c;
		}
		*pdwCode = iCodeTemp;
		
		return iError;
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

static int isSupportUtf8(unsigned char *pcFileHeadPos)
{
	char pcFlags[3] = {0xEF, 0xBB, 0xBF};

	if (memcmp(pcFlags, pcFileHeadPos, 3) == 0)
	{
		return 0;					//文件头有三个字节
	}

	return -1;
}

T_ENCODING g_tUtf8Opr = {
	.name = "utf_8",
	.isSupportThisEncoding = isSupportUtf8,
	.getEncoding		   = getUtf8Encoding,
	.file_head			   = 3,
};
int utf8_init(void)
{
	return register_encoding(&g_tUtf8Opr);
}
