#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>

#include <config.h>
#include <input_manager.h>

static void stdinDeviceInit(void)
{
	struct termios ttystate;
	
	//get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	
	//turn off canonical mode
	ttystate.c_lflag &= ~ICANON;
	//minimum of number input read.
	ttystate.c_cc[VMIN] = 1;
	
	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

static int getStdinInput(PT_INPUTEVENT ptInputEvent)
{
	struct timeval tv;
	
	gettimeofday(&tv, NULL);

	ptInputEvent->time  = tv;
	ptInputEvent->type  = STDIN;
	ptInputEvent->value = (int)fgetc(stdin);

	return 0;	
}


static int stdinExit(void)
{
	struct termios ttystate;
	
	//get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);
	
	//turn on canonical mode
	ttystate.c_lflag |= ICANON;
	
	//set the terminal attributes.
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

	return 0;
}

T_INPUT g_tStdinOps = {
	.name           = "stdin",
	.getDeviceInput = getStdinInput,
	.deviceExit     = stdinExit,
};

int stdin_init(void)
{
	stdinDeviceInit();
	return register_input(&g_tStdinOps);
}

