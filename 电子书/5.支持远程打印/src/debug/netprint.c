#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <config.h>
#include <debug_manager.h>

#define NETPRINTBUF_SIZE	(1024*16)
#define NETPRINT_PORT		8888

static int g_iSocketServer;
static struct sockaddr_in g_tServAddr;
static struct sockaddr_in g_tClientAddr;

static char *g_pcNetPrintBuf;
static pthread_t g_tNetPrintSendThreadID;
static pthread_t g_tNetPrintRecvThreadID;

static int g_iNetPrintWritePos = 0;		//环形缓冲区的读写指针
static int g_iNetPrintReadPos  = 0;

static int g_iDebugPrintStartFlag = 0;

static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tCond   = PTHREAD_COND_INITIALIZER;

static int isEmpty(void)
{
	return g_iNetPrintWritePos == g_iNetPrintReadPos;
}

static int isFull(void)
{
	return (g_iNetPrintWritePos + 1) % NETPRINTBUF_SIZE == g_iNetPrintReadPos;
}

static int saveChar(char cTmp)
{
	if (!isFull())
	{
		g_pcNetPrintBuf[g_iNetPrintWritePos] = cTmp;
		g_iNetPrintWritePos = (g_iNetPrintWritePos + 1) % NETPRINTBUF_SIZE;
		return 0;
	}
	else
	{
		return -1;
	}
}

static int getChar(char *cTmp)
{
	if (!isEmpty())
	{
		*cTmp = g_pcNetPrintBuf[g_iNetPrintReadPos];
		g_iNetPrintReadPos = (g_iNetPrintReadPos + 1) % NETPRINTBUF_SIZE;
		return 0;
	}
	else
	{
		return -1;
	}
		
}

static void *NetPrintSendThreadFunc(void *arg)
{
	int iNum;
	int i;
	char sendBuf[512];
	char cVal;
	
	
	while (1)
	{		
		pthread_mutex_lock(&g_tMutex);
		
		pthread_cond_wait(&g_tCond, &g_tMutex);
		
		pthread_mutex_unlock(&g_tMutex);
		
		if (g_iDebugPrintStartFlag && !isEmpty())   
		{	
			i = 0;
			while (i < 512 && !getChar(&cVal))
			{
				sendBuf[i] = cVal;
				i++;
			}
			
			sendBuf[i] = '\0';
			iNum = sendto(g_iSocketServer, sendBuf, i, 0, (struct sockaddr *)&g_tClientAddr, sizeof(struct sockaddr));
			if(iNum <= 0)
			{
				debugPrint("sendto error!\n");
			}
		}
	}
	
	return NULL;
}

static void *NetPrintRecvThreadFunc(void *arg)
{
	char recvBuf[512];
	int iNum;
	socklen_t tSockAddrLen;
	struct sockaddr_in tClientAddr;

	tSockAddrLen = sizeof(struct sockaddr);
	
	while(1)
	{
		iNum = recvfrom(g_iSocketServer, recvBuf, 512, 0, (struct sockaddr *)&tClientAddr, &tSockAddrLen);
		
		if (iNum <= 0)
		{
			debugPrint("recvfrom error!\n");
		}
		
		recvBuf[iNum] = '\0';
		
		/* 
		 *	recvBuf = "start"				//开启发送debug信息
		 *	recvBuf = "stdout=1"			//开启stdout
		 *	recvBuf = "stdout=0"			//关闭stdout
		 *	recvBuf = "netprint=1"			//开启netprint
		 *	recvBuf = "netprint=0"			//关闭netprint
		 *	recvBuf = "debuglevel=[0-9]"	//修改打印级别
		 */
		
		if (strncmp(recvBuf, "start", 5) == 0)
		{
			g_tClientAddr = tClientAddr;
			g_iDebugPrintStartFlag = 1;
			debugPrint("ip = %s\n",  inet_ntoa(tClientAddr.sin_addr));
		}
		else if ((strncmp(recvBuf, "stdout=", 7) == 0) || (strncmp(recvBuf, "netprint=", 9) == 0))
		{
			modifyChannel(recvBuf);
		}
		else if (strncmp(recvBuf, "debuglevel=", 11) == 0)
		{
			modifyDebugLevel(recvBuf);
		}
		else
		{
			debugPrint("understand cmd!\n");
		}

	}

	return NULL;
}

static int netPrintDebugInit(void)
{
	/* 创建一个UDP连接的socket */
	g_iSocketServer = socket(PF_INET, SOCK_DGRAM, 0);
	
	bzero(&g_tServAddr, sizeof(g_tServAddr));
	g_tServAddr.sin_family = AF_INET;
	g_tServAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	g_tServAddr.sin_port = htons(NETPRINT_PORT);
	/* 绑定servaddr到创建的socket上 */
	bind(g_iSocketServer, (struct sockaddr *)&g_tServAddr, sizeof(g_tServAddr));

	/* 分配缓冲区 */
	g_pcNetPrintBuf = malloc(NETPRINTBUF_SIZE);
	if (g_pcNetPrintBuf == NULL)
	{
		debugPrint("malloc error!\n");
		return -1;
	}

	/* 创建两个线程，一个是发送线程，另一个是接受线程 */
	pthread_create(&g_tNetPrintSendThreadID, NULL, NetPrintSendThreadFunc, NULL);
	pthread_create(&g_tNetPrintRecvThreadID, NULL, NetPrintRecvThreadFunc, NULL);

	return 0;
}

static int netPrintDebugExit(void)
{
	free(g_pcNetPrintBuf);
	close(g_iSocketServer);
	
	return 0;
}

static int netPrintDebugPrint(char *debugData)
{
	int i;
	int iError;
	
	for (i = 0; i < strlen(debugData); i++)
	{
		iError = saveChar(debugData[i]);
		if (iError)							//缓冲区满了
			break;
	}

	pthread_mutex_lock(&g_tMutex);
	
	pthread_cond_signal(&g_tCond);
	
	pthread_mutex_unlock(&g_tMutex);
	
	return i;
}

static T_DEBUG g_tNetPrintDebug = {
	.name       = "netprint",
	.isCanUse   = 1,
	.debugInit  = netPrintDebugInit,
	.debugExit  = netPrintDebugExit,
	.debugPrint = netPrintDebugPrint,
};

int netPrintInit(void)
{
	return register_debug(&g_tNetPrintDebug);
}

