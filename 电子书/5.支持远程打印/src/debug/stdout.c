#include <stdio.h>
#include <config.h>
#include <debug_manager.h>

static int stdoutDebugPrint(char *debugData)
{
	printf("%s", debugData);
	
	return 0;
}

static T_DEBUG g_tStdoutDebug = {
	.name       = "stdout",
	.isCanUse   = 1,
	.debugPrint = stdoutDebugPrint,
};

int stdoutInit(void)
{
	return register_debug(&g_tStdoutDebug);
}
