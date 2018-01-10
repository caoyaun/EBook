#ifndef __DEBUG_MANAGER_H
#define __DEBUG_MANAGER_H


#define APP_EMERG	0	/* system is unusable			*/
#define APP_ALERT	1	/* action must be taken immediately	*/
#define APP_CRIT	2	/* critical conditions			*/
#define APP_ERR	    3	/* error conditions			*/
#define APP_WARNING	4	/* warning conditions			*/
#define APP_NOTICE	5	/* normal but significant condition	*/
#define APP_INFO	6	/* informational			*/
#define APP_DEBUG	7	/* debug-level messages			*/

typedef struct debug {
	char *name;
	int isCanUse;
	int (*debugInit)(void);
	int (*debugExit)(void);
	int (*debugPrint)(char *debugData);
	struct debug *next_debug;
}T_DEBUG, *PT_DEBUG;

int debugPrint(const char *format, ...);
int debug_exit(void);
int debug_init(void);
int register_debug(PT_DEBUG ptDebugOpr);

int stdoutInit(void);
int netPrintInit(void);

void modifyChannel(char *pcCmd);
void modifyDebugLevel(char *pcCmd);

#endif
