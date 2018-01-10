#include <stdio.h>
#include <pthread.h>

#include <input_manager.h>

#include <config.h>

PT_INPUT g_ptInputHead;

static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tCond   = PTHREAD_COND_INITIALIZER;
	
static T_INPUTEVENT g_tInputEvent;

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
	pthread_mutex_lock(&g_tMutex);

	pthread_cond_wait(&g_tCond, &g_tMutex);
	*ptInputEvent = g_tInputEvent;
	
	pthread_mutex_unlock(&g_tMutex);
	
	return 0;
}

static void *inputThreadFunc(void *arg)
{
	T_INPUTEVENT tInputEvent;
	int iError;

	int (*getDeviceInput)(PT_INPUTEVENT ptInputEvent);
	getDeviceInput = (int (*)(PT_INPUTEVENT))arg;

	while (1)
	{
		/* 如果有数据，就唤醒主线程 */
		iError = getDeviceInput(&tInputEvent);
		if (iError == 0)
		{
			pthread_mutex_lock(&g_tMutex);
			
			g_tInputEvent = tInputEvent;
			pthread_cond_signal(&g_tCond);
			
			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}

void input_init(int x, int y)
{
	PT_INPUT ptInputTemp;
	
	stdin_init();
	ts_init(x, y);
	
	ptInputTemp = g_ptInputHead;

	while(ptInputTemp)
	{
		pthread_create(&ptInputTemp->iPthreadID, NULL, inputThreadFunc, ptInputTemp->getDeviceInput);

		ptInputTemp = ptInputTemp->next_input;
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

