#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NETPRINT_PORT		8888

/*  Usage: 
 *  ./netprintAPP start ip
 *	./netprintAPP stdout=1 				//开启stdout
 *	./netprintAPP stdout=0 				//关闭stdout
 *	./netprintAPP netprint=1 			//开启netprint
 *	./netprintAPP netprint=0 			//关闭netprint
 *	./netprintAPP debuglevel=[0-9] 		//修改打印级别
 */
int main(int argc, char **argv)
{
	int iSockfd;
	int iNum;
	socklen_t tCliAddrLen;
	char recvBuf[512];
	struct sockaddr_in tServAddr;
	struct sockaddr_in tClientAddr;
	
	if(argc != 3)
	{
		printf("Usgae:\n");
		printf("./netprintAPP start ip\n");
		printf("./netprintAPP stdout=0 ip\n");
		printf("./netprintAPP stdout=1 ip\n");
		printf("./netprintAPP netprint=0 ip\n");
		printf("./netprintAPP netprint=1 ip\n");
		printf("./netprintAPP debuglevel=[0-9] ip\n");
		return -1;
	}

	
	/* 创建一个UDP的socket连接 */
	iSockfd = socket(PF_INET, SOCK_DGRAM, 0);

	/* 变量servaddr清零 */
	bzero(&tServAddr, sizeof(tServAddr));
	tServAddr.sin_family = AF_INET;
	tServAddr.sin_port = htons(NETPRINT_PORT);
	tServAddr.sin_addr.s_addr = inet_addr(argv[2]);

	if (strncmp(argv[1], "start", 5) == 0)
	{
		sendto(iSockfd, argv[1], 5, 0, (struct sockaddr *)&tServAddr, sizeof(tServAddr));
		while (1)
		{
			iNum = recvfrom(iSockfd, recvBuf, 512, 0, (struct sockaddr *)&tClientAddr, &tCliAddrLen);
			if (iNum <= 0)
			{
				printf("recvfrom error");
			}
			recvBuf[iNum] = '\0';
			printf("%s", recvBuf);
		}
	}
	else
	{
		sendto(iSockfd, argv[1], strlen(argv[1]), 0, (struct sockaddr *)&tServAddr, sizeof(tServAddr));
	}
	

	close(iSockfd);

	return 0;
}

