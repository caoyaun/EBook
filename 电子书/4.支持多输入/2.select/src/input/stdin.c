#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#include <config.h>
#include <input_manager.h>

T_INPUT g_tStdinOps;

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

	g_tStdinOps.fd = STDIN_FILENO;
}

static int getStdinInput(PT_INPUTEVENT ptInputEvent)
{
	struct timeval tv;
    fd_set fds;
	
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
	
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

	if (FD_ISSET(STDIN_FILENO, &fds))	//有数据，处理数据
	{
		ptInputEvent->time  = tv;
		ptInputEvent->type  = STDIN;
		ptInputEvent->value = (int)fgetc(stdin);

		return 0;
	}
	else
	{
		return -1;
	}
	
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

