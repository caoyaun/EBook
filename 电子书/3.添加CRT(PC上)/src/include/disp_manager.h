#ifndef __DISPMANGER_H
#define __DISPMANGER_H

typedef struct disp{
	char  *name;
	int xres;
	int yres;
	void (*show_pixel)(int x, int y, unsigned int iColor);
	void (*closeDisp)(void);
	struct disp *next_disp;
}T_DISP, *PT_DISP;

int register_disp(PT_DISP ptDispOpr);
PT_DISP find_displayer(char *pcDisName);
int fb_init(void);
int crt_init(void);
void display_init(void);

#endif
