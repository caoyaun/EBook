#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <config.h>
#include <draw.h>

/* ./show_book -s size -f freetype_filename -h hzk -d displayer bookname */

int main(int argc, char **argv)
{
	int iError;
	unsigned int iFontSize = 0;

	char acFreeTypeFile[128];
	char acHzkFile[128];
	char acDisPlay[128];
	char acBookName[128];

	char cOrder;
	
	while((iError = getopt(argc, argv, "s:f:h:d:")) != -1)
	{
		switch(iError)
		{
			case 's':
			{
				/* 字体大小 */
				iFontSize = strtoul(optarg, NULL, 0);
				break;
			}
			case 'f':
			{
				/* 字体矢量文件名 */
				strcpy(acFreeTypeFile, optarg);
				break;
			}
			case 'h':
			{
				/* 汉字库文件名 */
				strcpy(acHzkFile, optarg);
				break;
			}
			case 'd':
			{
				/* 显示器名称 */
				strcpy(acDisPlay, optarg);
				break;
			}
			case ':':
			{
				/* 缺少选项 */
				DEG_PRINTF("Please input ./show_book -s size -f freetype_filename -h hzk -d displayer bookname\n");
				break;
			}
		}
		
	}

	/* 文本文件名 */
	strcpy(acBookName, argv[optind]);

	/* 做一些初始化 */
	system_init(acFreeTypeFile, acHzkFile, iFontSize);
	getConfig(acDisPlay, acBookName);
	
	/* 显示第一页 */
	showNextPage();
	
	while(1)
	{
		printf("input n: nextpage, p: prePage, q: quit\n");
		cOrder = getchar();

		switch(cOrder)
		{
			case 'n':
			{
				showNextPage();
				break;
			}
			case 'p':
			{
				showPrePage();
				break;
			}
			case 'q':
			{
				return 0;
				break;
			}
			default:
			{
				printf("Please input n/p/q\n");
				break;
			}
		}
	}
	
	return 0;
}
