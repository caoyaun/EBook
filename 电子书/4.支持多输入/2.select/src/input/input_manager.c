#include <stdio.h>
#include <sys/select.h>
#include <input_manager.h>

#include <config.h>

PT_INPUT g_ptInputHead;
static fd_set g_tFdSet;
static int g_iMaxFd = -1;;

int register_input(PT_INPUT ptInputOpr)
{
	PT_INPUT ptInputTemp;

	if (g_ptInputHead == NULL)
	{
		g_ptInputHead = ptInputOpr;
	}
	else
	{
		ptInputTemp = g_ptInputHead;
			
		while(ptInputTemp->next_input)
		{
			ptInputTemp = ptInputTemp->next_input;
		}

		ptInputTemp->next_input = ptInputOpr;
	}

	return 0;

}

int getInput(PT_INPUTEVENT ptInputEvent)
{	
	PT_INPUT ptInputTemp;
	fd_set tFdSet = g_tFdSet;
	int iRet;

	ptInputTemp = g_ptInputHead;

	iRet = select (g_iMaxFd + 1, &tFdSet, NULL, NULL, NULL);
	if (iRet > 0)
	{
		while(ptInputTemp)
		{
			if (FD_ISSET(ptInputTemp->fd, &tFdSet))
			{
				iRet = ptInputTemp->getDeviceInput(ptInputEvent);
				if (iRet == 0)
					return 0;
				else 
					return -1;
			}
			else
				ptInputTemp = ptInputTemp->next_input;
		}
	}

	return -1;
}

void input_init(int x, int y)
{
	PT_INPUT ptInputTemp;
	
	FD_ZERO(&g_tFdSet);
	
	stdin_init();
	ts_init(x, y);
	
	ptInputTemp = g_ptInputHead;

	while (ptInputTemp)
	{
		FD_SET(ptInputTemp->fd, &g_tFdSet);
		
		if (g_iMaxFd < ptInputTemp->fd)
			g_iMaxFd = ptInputTemp->fd;

		if (ptInputTemp->next_input)
			ptInputTemp = ptInputTemp->next_input;
		else
			break;
	}
}

void input_exit(void)
{
	PT_INPUT ptInputTemp;
	
	ptInputTemp = g_ptInputHead;

	while (ptInputTemp)
	{
		if (ptInputTemp->deviceExit)
			ptInputTemp->deviceExit();

		if (ptInputTemp->next_input)
			ptInputTemp = ptInputTemp->next_input;
		else 
			break;
	}
}

