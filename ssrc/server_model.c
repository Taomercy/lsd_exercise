#include "all_head.h"
#include "mychat_model.h"

void server_user_regist_judge(int clientfd, Cmsg core_message)//服务器用户名注册校验函数
{
	printf("注册名校验\n");
	Userlink* node = userlink_select_node(userlink,core_message.name);//返回查询用户的当前节点
	Cmsg msg2 = {};
	msg2.type = REG_J;
	if(node != NULL)
	{
	//	msg2.type = 1;
		strcpy(msg2.text,"此用户已存在");
	}
	else
	{
	//	msg2.type = 10;
		strcpy(msg2.text,"此用户名可用");
	}
	printf("正在发送用户名可用消息\n");
	if(write(clientfd,&msg2,sizeof(msg2)) <= 0)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}
}
void server_user_regist_pwdin(int clientfd, Cmsg core_message)//注册密码录入
{
//	printf("正在将信息插入链表\n");
	User data = {};
	printf("注册函数里的name:%s\n",core_message.name);
	strcpy(data.name,core_message.name);
	strcpy(data.password,core_message.password);
	userlink_insert_after(userlink,data);
	Cmsg msg5 = {};
	msg5.type = REG_P;
	strcpy(msg5.name,"注册成功");
	userlink_save(userlink);
	if(write(clientfd,&msg5,sizeof(msg5)) <= 0)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}

}

void server_user_login(int clientfd, Cmsg core_message, Online net_message)//用户登录
{
	chdir(first_path);
	Cmsg login_message = {};
	login_message.type = LOG;
//	printf("进入用户登录函数\n");
	print_all(onlink);
	Onlink* online_node = onlink_select_node(onlink,core_message.name);
//	printf("onlink:%p\n",onlink);
//	printf("遍历在线用户链表完成\n");
	if(online_node != NULL)
	{
		printf("此用户名已在线\n");
		if(strcmp(online_node->data.password,core_message.password) != 0)
		{
			strcpy(login_message.text,"密码错误");
			if(write(clientfd,&login_message,sizeof(Cmsg)) <= 0)
			{
				fprintf(stderr,"%s\n",strerror(errno));
				return;
			}
			return;
		}
		strcpy(login_message.text,"此账户已登录，将此在线账户成功代替");
		if(write(clientfd,&login_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
		Cmsg quit_message = {};
		quit_message.type = QUIT;
		strcpy(quit_message.text,"异地登录，自动下线");
		if(write(online_node->data.clientfd,&quit_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"%s\n",strerror(errno));
			return;
		}
		strcpy(online_node->data.name,core_message.name);
		strcpy(online_node->data.ip,net_message.ip);
		online_node->data.clientfd = clientfd;
		online_node->data.second = (int)time(NULL);
		chdir(core_message.name);
		char change_path[1024] = "";
		if(getcwd(change_path,sizeof(change_path)) == NULL)
		{
			fprintf(stderr,"%s\n",strerror(errno));
		}
		strcpy(online_node->data.path,change_path);
		printf("在线用户完成覆盖\n");
		print_all(onlink);
	}
	else
	{
		printf("无相同用户名在线\n");
		Userlink* node = userlink_select_node(userlink,core_message.name);
//		printf("遍历用户链表完成\n");
//		Cmsg login_message = {};
//		login_message.type = 2;
		if(node == NULL)
		{
			strcpy(login_message.text,"此用户不存在");
		}
		else
		{
			if(strcmp(node->data.password,core_message.password) == 0)
			{
				strcpy(login_message.text,"登录成功");
				printf("登录成功\n");
		//		printf("onlink:%p\n",onlink);
				strcpy(g_data.name,core_message.name);
				strcpy(g_data.password,core_message.password);

				//判断文件夹是否存在
				if(access(core_message.name,F_OK) != 0)
				{
					printf("创建此用户文件夹\n");
					umask(0000);
					if(mkdir(core_message.name,0777) == -1)
					{
						fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
						return;	
					}
				}
				chdir(core_message.name);//改变工作路径
				char user_path[1024] = "";
				if(getcwd(user_path,sizeof(user_path)) == NULL)
				{
					fprintf(stderr,"%s\n",strerror(errno));
				}
				strcpy(g_data.path,user_path);//将工作路径保存至在线用户链表
				onlink_insert_after(onlink,g_data);
				print_all(onlink);
				system("pwd");
				printf("进入此用户工作路径\n");
			}
			else
			{
				strcpy(login_message.text,"密码错误");
			}
		}
		printf("正在发送登录情况\n");
		if(write(clientfd,&login_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			return;
		}
	}

}


void server_user_shell(int clientfd, Cmsg core_message, char* pathname)//服务器shell命令
{
//	printf("进入shell命令函数\n");
	Onlink* user_node = onlink_select_node(onlink,core_message.name);
	if(user_node == NULL)
	{
		printf("此帐号未在线!\n");
		return;
	}
	chdir(user_node->data.path);//改变工作路径
	Cmsg shell_message = {};
	shell_message.type = SHELL;

	char str1[64] = "";
	char str2[64] = "";
	sscanf(core_message.shell,"%s %s",str1,str2);
	if(strcmp(str1,"cd") == 0)
	{
	//	printf("判断cd命令\n");
		if(strcmp(str2,"") == 0 || strcmp(str2,"~") == 0)
		{
			chdir(pathname);
			strcpy(shell_message.text,"返回根目录成功");
		}
		else if(strcmp(str2,"..") == 0)
		{
			char current_pathname[1024] = "";
			if(getcwd(current_pathname,sizeof(current_pathname)) == NULL)
			{
				fprintf(stderr,"%s\n",strerror(errno));
				return;
			}
			if(strcmp(pathname,current_pathname) == 0)
			{
				strcpy(shell_message.text,"权限不够，无法执行");
			}
			else
			{
				chdir("..");
				strcpy(shell_message.text,"返回上一级文件夹成功");
			}
		}
		else
		{
			if(access(str2,F_OK) != 0)
			{
				strcpy(shell_message.text,"此文件夹不存在");
			}
			else
			{
				chdir(str2);
				strcpy(shell_message.text,"进入文件夹成功");
			}
		}
		system("pwd");
	}
	else if(strcmp(str1,"mkdir") == 0)
	{
//		printf("判断mkdir命令\n");
		if(strcmp(str2,"") == 0)
		{
			strcpy(shell_message.text,"shell命令错误");
		}
		else
		{
			if(access(str2,F_OK) != 0)
			{
				umask(0000);
				if(mkdir(str2,0777) == -1)
				{
					fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
					return;	
				}
				strcpy(shell_message.text,"文件夹创建完成");
			}
			else
			{
				strcpy(shell_message.text,"此文件夹已存在，创建失败");
			}
		}
		system("pwd");
	}
	else
	{
//		printf("其他shell命令\n");
		char filename[1024] = "";
		sprintf(filename,"%s.txt",core_message.name);
		int fd = open(filename,O_WRONLY | O_CREAT | O_TRUNC,0664);
		if(fd == -1)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			return;
		}
		int save = dup(1);
		dup2(fd,1);
		system(core_message.shell);
		fflush(stdout);
		dup2(save,1);
		close(fd);
//		printf("shell命令重定向完成\n");
		int fd2 = open(filename,O_RDONLY);
		if(fd2 == -1)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			return;
		}

		char buffer[1024] = "";
		while(1)
		{
			if(read(fd2,buffer,1024)  > 0)
			{
				strcpy(shell_message.text,buffer);
				if(write(clientfd,&shell_message,sizeof(Cmsg)) <= 0)
				{
					fprintf(stderr,"%s\n",strerror(errno));
					return;	
				}
				strcpy(buffer,"");
			}
			else
			{
				strcpy(shell_message.text,buffer);
				if(write(clientfd,&shell_message,sizeof(Cmsg)) <= 0)
				{
					fprintf(stderr,"%s\n",strerror(errno));
					return;	
				}
				break;
			}
		}

		close(fd2);
		return;
	}
//	printf("发送消息\n");
//	printf("text:%s\n",shell_message.text);
	if(write(clientfd,&shell_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;	
	}
}

void server_user_check_online(int clientfd, Cmsg core_message)
{
//	printf("正在查看在线用户\n");
	char filename[1024] = "";
	sprintf(filename,"%s.txt",core_message.name);
	int fd = open(filename,O_WRONLY | O_CREAT | O_TRUNC,0664);
	if(fd == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}
	int save = dup(1);
	dup2(fd,1);
	print_all(onlink);
	fflush(stdout);
	dup2(save,1);
	close(fd);

	int fd2 = open(filename,O_RDONLY);
	if(fd2 == -1)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;
	}

	Cmsg check_message = {};
	check_message.type = CHECK;
	char buffer = '0';
	int i = 0;
	while(1)
	{
		if(read(fd2,&buffer,sizeof(char)) == 0)
		{
			break;
		}
		check_message.text[i] = buffer;
		++i;
	}

//	printf("发送在线用户消息\n");
	if(write(clientfd,&check_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"%s\n",strerror(errno));
		return;	
	}

	close(fd2);
}

void server_user_chat(int clientfd, Cmsg core_message)
{
	Cmsg chat_message = {};
	chat_message.type = CHAT;
	Onlink* chat_node = onlink_select_node(onlink,core_message.chat_name);
	if(chat_node == NULL)
	{
		strcpy(chat_message.text,"此用户不在线");
		if(write(clientfd,&chat_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			return;
		}
		return;
	}
	strcpy(chat_message.name,core_message.name);
	strcpy(chat_message.text,core_message.text);
	if(write(chat_node->data.clientfd,&chat_message,sizeof(Cmsg)) <= 0)
	{
		fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
		return;	
	}

}

void server_user_multichat(int clientfd, Cmsg core_message)
{
	Cmsg mulchat_message = {};
	mulchat_message.type = GROUP;
	strcpy(mulchat_message.name,core_message.name);
	strcpy(mulchat_message.text,core_message.text);
	Onlink* save = onlink;
	save = save->next;
	while(save != NULL)
	{
		if(save->data.clientfd == clientfd)
		{
			save = save->next;
			continue;
		}
		if(write(save->data.clientfd,&mulchat_message,sizeof(Cmsg)) <= 0)
		{
			fprintf(stderr,"文件名：%s\n函数名：%s\n行号：%d\n错误内容：%s\n",__FILE__,__func__,__LINE__,strerror(errno));
			return;			
		}
		save = save->next;
	}


}

void server_user_heart(int clientfd, Cmsg core_message)//心跳数据接收
{
//	printf("接收心跳包成功\n");
	printf("name:%s\ttime:%s\n",core_message.name,core_message.time);
	Onlink* node = onlink_select_node(onlink,core_message.name);
	if(node == NULL)
	{
		printf("查询在线链表错误\n");
		return;
	}
	node->data.second = core_message.second;
	return;
}

