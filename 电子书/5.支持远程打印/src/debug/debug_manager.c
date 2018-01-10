#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <config.h>
#include <debug_manager.h>

static PT_DEBUG g_ptDebugHead;
static int g_iDebugLevel = 8;

int register_debug(PT_DEBUG ptDebugOpr)
{
	PT_DEBUG ptDebugTemp;

	if (g_ptDebugHead == NULL)
	{
		g_ptDebugHead = ptDebugOpr;
	}
	else
	{
		ptDebugTemp = g_ptDebugHead;
			
		while(ptDebugTemp->next_debug)
		{
			ptDebugTemp = ptDebugTemp->next_debug;
		}

		ptDebugTemp->next_debug = ptDebugOpr;
	}

	return 0;
}

int debug_init(void)
{
	PT_DEBUG ptDebugTemp;
	int iError = 0;

	stdoutInit();
	netPrintInit();

	ptDebugTemp = g_ptDebugHead;
	
	while (ptDebugTemp)
	{
		if (ptDebugTemp->debugInit)
			iError |= ptDebugTemp->debugInit();

		ptDebugTemp = ptDebugTemp->next_debug;
	}

	return iError;
}

int debug_exit(void)
{
	PT_DEBUG ptDebugTemp;
	int iError = 0;
	
	ptDebugTemp = g_ptDebugHead;

	while (ptDebugTemp)
	{
		if (ptDebugTemp->debugExit)
			iError |= ptDebugTemp->debugExit();

		ptDebugTemp = ptDebugTemp->next_debug;
	}

	return iError;
}

void modifyChannel(char *pcCmd)
{
	int iControl;
	char *pcTmp;
	char cDebugNameBuf[128];
	PT_DEBUG ptDebugTemp;
	
	ptDebugTemp = g_ptDebugHead;

	/* 找到第一个等号位置的指针并且把等号之前的字符串拷到cDebugNameBuf */
	pcTmp = strchr(pcCmd, '=');
	strncpy(cDebugNameBuf, pcCmd, pcTmp - pcCmd);
	cDebugNameBuf[pcTmp - pcCmd] = '\0';
	
	iControl = pcTmp[1] - '0';
		
	while (ptDebugTemp)
	{
		if (strcmp(ptDebugTemp->name, cDebugNameBuf) == 0)
		{
			ptDebugTemp->isCanUse = iControl;
			return;
		}
		else
			ptDebugTemp = ptDebugTemp->next_debug;
	}
	
}

void modifyDebugLevel(char *pcCmd)
{
	char *pcTmp;

	pcTmp  = strchr(pcCmd, '=');
	g_iDebugLevel = pcTmp[1] - '0';
}

int debugPrint(const char *format, ...)
{
	PT_DEBUG ptDebugTemp;
	int iError = 0;
	int iDebugLevel;
	
	va_list tArg;
	int iNum;
	char strBuf[512];
	char *strTemp;

	strTemp = strBuf;

	ptDebugTemp = g_ptDebugHead;

	/* 把参数整合到一个字符串数组中 */
	va_start (tArg, format);
	iNum = vsprintf (strBuf, format, tArg);
	va_end (tArg);

	strBuf[iNum] = '\0';
	
	/* 处理打印级别 */
	if (strBuf[0] == '<' && strBuf[2] == '>')
	{
		if (strBuf[1] >= '0' && strBuf[1] <= '9')
		{
			iDebugLevel = strBuf[1] - '0';
			strTemp += 3;
		}
		else
		{
			iDebugLevel = APP_WARNING;
		}
	}
	else
	{
		iDebugLevel = APP_WARNING;
	}

	if (iDebugLevel <= g_iDebugLevel)
	{
		/* 把debug信息打印到所有isCanUse为1的终端上 */
		while(ptDebugTemp)
		{
			if (ptDebugTemp->isCanUse)
				iError |= ptDebugTemp->debugPrint(strTemp);
		
			ptDebugTemp = ptDebugTemp->next_debug;
		}
	}

	if (iError)
		return -1;

	return iNum;
}
