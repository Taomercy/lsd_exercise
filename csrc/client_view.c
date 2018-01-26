#include <stdio.h>

void client_view_1(void)//客户端 主线程 1级菜单
{
	printf("\033[0;34m----------MyChat--------\033[0;m\n");
	printf("\033[0;35m1.用户注册\033[0;m\n");
	printf("\033[0;35m2.用户登录\033[0;m\n");
	printf("\033[0;31m0.退出系统\033[0;m\n");
}

void client_view_2(char* name)//客户端 主线程 2级菜单
{
	printf("\033[0;34m----------MyChat--------\t\t\t用户名：%s\033[0;m\n",name);
	printf("\033[0;35m1.Shell命令\033[0;m\n");
	printf("\033[0;35m2.查看在线用户\033[0;m\n");
	printf("\033[0;35m3.单人聊天\033[0;m\n");
	printf("\033[0;35m4.多人聊天\033[0;m\n");
	printf("\033[0;31m0.退出登录\033[0;m\n");
}
