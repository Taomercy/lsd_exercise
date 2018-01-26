#ifndef MYCHAT_MODEL_H
#define	MYCHAT_MODEL_H
#include <pthread.h>

typedef struct user
{
	char name[32];//姓名
	char password[32];//密码
	char r_time[32];//注册时间
}User;
typedef struct userlink{
	User data;
	struct userlink* next;
}Userlink;
/******************************************/
typedef struct online
{
	char ip[1024];
	int clientfd;
	char name[1024];
	char password[1024];
	char path[1024];
	pthread_t pth_id;//服务端客户线程编号
	int second;//当前时间秒数
}Online;
typedef struct onlink
{
	Online data;
	struct onlink* next;
}Onlink;
/************************************************/
Userlink* userlink;//服务器全局用户链表
Onlink* onlink;//服务器全局在线用户链表
Online g_data;//服务器全局客户端信息
char first_path[1024];//服务器全局根目录
pthread_t pthread_heart;//客户端心跳线程id
/*************************************************/
typedef struct cmsg//核心信息结构体
{
	long type;
	char name[128];
	char password[128];
	char shell[128];
	char chat_name[128];//聊天对象姓名
	char path[1024];
	int second;//当前秒数
	char time[128];//当前时间
	char text[1024];
}Cmsg;
/**********************************************/
enum//枚举数据包类型
{
	EXIT = 0,
	REG_J = 1,//判断注册用户名是否可用
	LOG = 2,//登录
	SHELL = 3,//shell命令
	CHECK = 4,//查看在线用户
	CHAT = 5,//聊天
	GROUP = 6,//群聊
	HEART = 7,//心跳
	REG_P = 10,//注册用户名可用时完成密码信息录入操作
	QUIT = 11,//异地登录OR退出登录

};
/*********************************************************/
typedef struct queue_message//消息队列
{
	long type;
	char text[1024];
}Qmsg;
int msgqid;
/********************************************/

typedef struct clientfd_msg//将名字传入线程结构体
{
	int clientfd;
	char name[128];
}Nmsg;

char* get_array(char* str ,char* filename,char* value);//IP,port获取函数：成功返回str，失败返回NULL


/******************************用户链表函数**********************************/
Userlink* user_create_node(User data);//创建用户链表节点
void userlink_insert_after(Userlink* head, User data);//插入用户链表节点
Userlink* userlink_select_front_node(Userlink* head,char* name);//查询用户链表前一节点
Userlink* userlink_select_node(Userlink* userlink, char name[]);//用户链表姓名查询当前节点
void userlink_delete_node(Userlink* head,char* name);//删除用户链表节点
void userlink_load(Userlink* head);//加载用户链表
void userlink_save(Userlink* head);//保存用户链表
void userlink_free(Userlink* head);//释放用户链表
void userlink_printf_all(Userlink* head);//打印所有

/*****************************在线用户链表函数*******************************/
Onlink* on_create_node(Online data);//创建用户链表节点
void onlink_insert_after(Onlink* head, Online data);//插入用户链表节点
Onlink* onlink_select_node(Onlink* head, char name[]);//用户链表姓名查询当前节点
Onlink* onlink_select_front_node(Onlink* head,char* name);//查询用户链表前一节点
Onlink* onlink_select_int_node(Onlink* head, int clientfd);//查询用户数字类型信息当前节点
void onlink_delete_node(Onlink* head,char* name);//删除用户链表节点
void print_all(Onlink* head);
/*****************************服务器函数*************************************/
void server_create(char* ip, char* port);//服务器创建函数
void* deal_heart(void* arg);//心跳包处理线程
void* server_receive(void* arg);//服务器数据接收线程
void msg_send(Cmsg msg3);//消息队列发送消息
void server_user_regist_judge(int clientfd, Cmsg core_message);//服务器用户名注册校验函数
void server_user_regist_pwdin(int clientfd, Cmsg core_message);//注册密码录入
void server_user_login(int clientfd, Cmsg core_message, Online net_message);//服务器用户登录
void server_user_shell(int clientfd, Cmsg core_message, char* pathname);//服务器shell命令
void server_user_check_online(int clientfd, Cmsg core_message);//服务器查看在线用户
void server_user_chat(int clientfd, Cmsg core_message);//服务器单人聊天
void server_user_multichat(int clientfd, Cmsg core_message);//服务器多人聊天
void server_user_heart(int clientfd, Cmsg core_message);//心跳数据接收

/*****************************客户端函数*************************************/
void enter_wait(void);//按回车以继续
void msgq_create(void);//创建消息队i列
void client_create(char* ip, char* port);//客户端创建
void* client_receive(void* arg);//客户端数据接收线程，并同步到主线程
void client_user_exit(int clientfd,char* name);//客户端退出
void client_user_regist(int clientfd);//客户端用户注册函数
void client_user_login(int clientfd, char* name);//客户端用户登录函数
void* client_heart(void* arg);//客户端心跳包发送线程
void client_user_shell(int clientfd, char* name);//客户端shell命令
void client_user_quit(int clientfd, char* name);//客户端退出登录
void client_user_check_online(int clientfd,char* name);//客户端查看在线用户
void client_user_chat(int clientfd,char* name);//客户端单人聊天
void client_user_multichat(int clientfd,char* name);//客户端多人聊天









#endif
