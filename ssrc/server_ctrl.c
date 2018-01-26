#include "all_head.h"
#include "mychat_model.h"

void server_ctrl_1(int clientfd, Online net_message)//服务器 客户端处理线程1级控制层
{
	char pathname[1024] = "";
	char root[1024] = "";
	if(getcwd(pathname,sizeof(pathname)) == NULL)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;
	}
	chdir(first_path);
	system("pwd");
	while(1)
	{
		sleep(1);
		Cmsg core_message = {};
//		printf("receive from client...\n");
		int read_err = read(clientfd,&core_message,sizeof(Cmsg));
		if(read_err == 0)
		{
			fprintf(stderr,"ID:%d\t断开链接！\n",clientfd);
			break;
		}
		else if(read_err < 0)
		{
			fprintf(stderr,"套接字错误！\n");
			break;
		}
/*		printf("数据包内容：\n");
		printf("type:%ld\n",core_message.type);
		printf("name:%s\n",core_message.name);
		printf("password:%s\n",core_message.password);
		printf("shell:%s\n",core_message.shell);
		printf("text:%s\n",core_message.text);*/
		switch(core_message.type)
		{
			case REG_J://校验用户名是否可用
				printf("进行用户名校验\n");
				server_user_regist_judge(clientfd,core_message);
				break;
			case REG_P://用户名可用是进行信息录入操作
				printf("进行密码录入\n");
				server_user_regist_pwdin(clientfd,core_message);
				break;
			case LOG://用户登录
				printf("进入登录函数\n");
				server_user_login(clientfd,core_message,net_message);
				break;
			case SHELL://shell命令
				server_user_shell(clientfd,core_message,root);
				getcwd(root,sizeof(root));
				Onlink* node = onlink_select_node(onlink,core_message.name);
				strcpy(node->data.path,root);
				break;
			case CHECK://查看在线用户
				server_user_check_online(clientfd,core_message);
				break;
			case HEART://心跳数据接收
				server_user_heart(clientfd,core_message);
				break;
			case CHAT://单人聊天
				server_user_chat(clientfd,core_message);
				break;
			case GROUP://多人聊天
				server_user_multichat(clientfd,core_message);
				break;
			case QUIT:
				print_all(onlink);
				onlink_delete_node(onlink,core_message.name);
				chdir(pathname);
				system("pwd");
				printf("退出此用户工作路径\n");
				break;
			case EXIT:
				printf("clientfd:%d,断开链接！\n",clientfd);
				return;
			default:
				printf("输入错误！\n");
				break;
		}
	}
}
