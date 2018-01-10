#include <stdio.h>
#include <encoding_manager.h>

#include <config.h>

PT_ENCODING g_ptEncodingHead;

int register_encoding(PT_ENCODING ptEncodingOpr)
{
	PT_ENCODING ptEncodingTemp;

	if (g_ptEncodingHead == NULL)
	{
		g_ptEncodingHead = ptEncodingOpr;
	}
	else
	{
		ptEncodingTemp = g_ptEncodingHead;
			
		while(ptEncodingTemp->next_encoding)
		{
			ptEncodingTemp = ptEncodingTemp->next_encoding;
		}

		ptEncodingTemp->next_encoding = ptEncodingOpr;
	}

	return 0;

}

PT_ENCODING find_support_encoding(unsigned char *pcFileHeadPos)
{
	int iError;
	PT_ENCODING ptEncodingTemp = g_ptEncodingHead;

	while(ptEncodingTemp)
	{
		iError = ptEncodingTemp->isSupportThisEncoding(pcFileHeadPos);
		if (iError == 0)			//表示是这种格式
			return ptEncodingTemp;
		else
			ptEncodingTemp = ptEncodingTemp->next_encoding;
	}
	
	return NULL;
}

void encoding_init(void)
{
	utf8_init();
	ascii_init();
	utf16Le_init();
	utf16Be_init();
}

