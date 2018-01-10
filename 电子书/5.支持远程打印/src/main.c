#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <config.h>
#include <draw.h>
#include <input_manager.h>
#include <debug_manager.h>

/* ./show_book -s size -f freetype_filename -h hzk -d displayer bookname */

int main(int argc, char **argv)
{
	int iError;
	unsigned int iFontSize = 0;

	char acFreeTypeFile[128];
	char acHzkFile[128];
	char acDisPlay[128];
	char acBookName[128];

	int cOrder;

	T_INPUTEVENT tInputEvent;

	/* 先初始化debug打印，以后的打印信息都由debug模块处理 */
	iError = debug_init();
	if (iError)
	{
		printf("debug_init error!\n");
		return -1;
	}

	/* 判断参数 */
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
	
	debugPrint("input n: nextpage, p: prePage, q: quit\n");
	
	while(1)
	{
		iError = getInput(&tInputEvent);

		if (iError == 0)
		{
			cOrder = tInputEvent.value;
			
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
					input_exit();
					debug_exit();
					return 0;
					break;
				}
				default:
				{
					debugPrint("Please input n/p/q\n");
					break;
				}
			}

		}

	}
	
	return 0;
}
