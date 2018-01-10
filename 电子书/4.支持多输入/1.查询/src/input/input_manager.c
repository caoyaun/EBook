#include <stdio.h>
#include <input_manager.h>

#include <config.h>

PT_INPUT g_ptInputHead;

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
	int iRet;
	
	ptInputTemp = g_ptInputHead;

	while (ptInputTemp)
	{
		iRet = ptInputTemp->getDeviceInput(ptInputEvent);
		if (iRet == 0)
			return 0;

		if (ptInputTemp->next_input)
			ptInputTemp = ptInputTemp->next_input;
		else 
			break;
	}

	return -1;
}

void input_init(int x, int y)
{
	stdin_init();
	ts_init(x, y);
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

