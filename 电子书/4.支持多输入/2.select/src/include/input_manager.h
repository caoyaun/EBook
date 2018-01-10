#ifndef __INPUTMANGER_H
#define __INPUTMANGER_H

#include <sys/time.h>

#define STDIN			0
#define TOUCHSCREEN 	1

typedef struct input_event {
	struct timeval time;
	int type;
	int value;
}T_INPUTEVENT, *PT_INPUTEVENT;

typedef struct input_ops {
	char *name;
	int fd;
	int (*getDeviceInput)(PT_INPUTEVENT ptInputEvent);
	int (*deviceExit)(void);
	struct input_ops *next_input;
}T_INPUT, *PT_INPUT;

int stdin_init(void);
int ts_init(int x, int y);
int register_input(PT_INPUT ptInputOpr);
int getInput(PT_INPUTEVENT ptInputEvent);
void input_init(int x, int y);
void input_exit(void);


#endif
