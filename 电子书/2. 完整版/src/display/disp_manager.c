#include <stdio.h>
#include <disp_manager.h>
#include <string.h>
#include <config.h>

PT_DISP g_ptDisplayHead;

int register_disp(PT_DISP ptDispOpr)
{
	PT_DISP ptDispTemp;
	
	if (g_ptDisplayHead == NULL)
	{
		g_ptDisplayHead = ptDispOpr;
	}
	else
	{
		ptDispTemp = g_ptDisplayHead;
			
		while(ptDispTemp->next_disp)
		{
			ptDispTemp = ptDispTemp->next_disp;
		}

		ptDispTemp->next_disp = ptDispOpr;
	}
	
	return 0;
}

PT_DISP find_displayer(char *pcDisName)
{
	PT_DISP ptDispTemp;

	ptDispTemp = g_ptDisplayHead;
		
	while(ptDispTemp)
	{
		if (strcmp(ptDispTemp->name, pcDisName) == 0)
		{
			return ptDispTemp;
		}
		else
			ptDispTemp = ptDispTemp->next_disp;
	}

	return NULL;
}

void display_init(void)
{
	fb_init();
}
