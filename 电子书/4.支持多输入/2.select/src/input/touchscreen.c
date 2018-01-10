#include <stdio.h>

#include <stdlib.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/select.h>

#include <config.h>
#include <input_manager.h>
#include <tslib.h>

T_INPUT g_tTsOps;

static struct tsdev *ptTs;
static struct timeval tPreTime;
static int g_iXres, g_iYres;

static void tsDeviceInit(void)
{
	char *pcTsDevice=NULL;

	if( (pcTsDevice = getenv("TSLIB_TSDEVICE")) != NULL ) 
	{
			ptTs = ts_open(pcTsDevice, O_NONBLOCK);
	} 
	else 
	{
		ptTs = ts_open("/dev/input/event2", O_NONBLOCK);
	}
	
	if (!ptTs) {
		DEG_PRINTF("ts_open fail!\n");
		return;
	}

	if (ts_config(ptTs)) {
		DEG_PRINTF("ts_config fail!\n");
		return;
	}

	g_tTsOps.fd = ts_fd(ptTs);

}

static int isOver500ms(struct ts_sample *tSamp)
{
	int iTime;
	
	iTime = tSamp->tv.tv_sec * 1000 + tSamp->tv.tv_usec / 1000;
	iTime = iTime - (tPreTime.tv_sec * 1000 + tPreTime.tv_usec / 1000); 
	
	if (iTime > 500)
		return 1;
	else 
		return 0;
}

static int getTsInput(PT_INPUTEVENT ptInputEvent)
{		
	int ret;
	struct ts_sample tSamp;

	ret = ts_read(ptTs, &tSamp, 1);
	
	if (ret < 0) 
	{
		DEG_PRINTF("ts_read fail!\n");
		return -1;
	}

	if (ret != 1)
		return -1;

	if (isOver500ms(&tSamp))
	{
		tPreTime = tSamp.tv;		
		ptInputEvent->type = TOUCHSCREEN;

		if (tSamp.x > (g_iXres * 2 / 3))
		{
			ptInputEvent->value = 'n';
		}
		else if (tSamp.x < (g_iXres * 1 / 3))
		{
			ptInputEvent->value = 'p';
		}
		else
		{
			ptInputEvent->value = ' ';
		}
		
		return 0;
	}
	else
	{
		return -1;
	}
	
	return 0;
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

