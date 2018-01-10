#include <stdio.h>

#include <stdlib.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>

#include <config.h>
#include <input_manager.h>
#include <tslib.h>

static struct tsdev *ptTs;
static int g_iXres, g_iYres;

static void tsDeviceInit(void)
{
	char *pcTsDevice=NULL;

	if( (pcTsDevice = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
		/* 以阻塞方式打开 */
			ptTs = ts_open(pcTsDevice, 0);
	} 
	else 
	{
		ptTs = ts_open("/dev/input/event2", 0);
	}
	
	if (!ptTs) {
		DEG_PRINTF("ts_open fail!\n");
		return;
	}

	if (ts_config(ptTs)) {
		DEG_PRINTF("ts_config fail!\n");
		return;
	}

	

}

static int getTsInput(PT_INPUTEVENT ptInputEvent)
{		
	int iRet;
	int iDelta;
	struct timeval tv;
	struct ts_sample tDownSamp;
	struct ts_sample tUpSamp;
	
	iRet = ts_read(ptTs, &tDownSamp, 1);
	
	gettimeofday(&tv, NULL);
	
	if ((iRet == 1) && (tDownSamp.pressure > 0))
	{
		do
		{
			iRet = ts_read(ptTs, &tUpSamp, 1);
			
		}while(tUpSamp.pressure > 0);

		iDelta = tUpSamp.x - tDownSamp.x;

		if (iDelta > g_iXres/5)				//前一页
		{
			ptInputEvent->time	= tv;
			ptInputEvent->type	= TOUCHSCREEN;
			ptInputEvent->value = 'p';
		}
		else if (iDelta < (0 - g_iXres/5))	//后一页
		{
			ptInputEvent->time	= tv;
			ptInputEvent->type	= TOUCHSCREEN;
			ptInputEvent->value = 'n';
		}
		else
		{
			return -1;
		}

		return 0;
	}
	
	return -1;
}

static int TsExit(void)
{
	return 0;
}

T_INPUT g_tTsOps = {
	.name			= "touchscreen",
	.getDeviceInput = getTsInput,
	.deviceExit 	= TsExit,
};

int ts_init(int x, int y)
{
	tsDeviceInit();
	g_iXres = x;
	g_iYres = y;
		
	return register_input(&g_tTsOps);
}

