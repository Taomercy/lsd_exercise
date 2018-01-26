#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mychat_model.h"

Userlink* user_create_node(User data)//创建用户链表节点
{
	Userlink* newnode = calloc(1,sizeof(Userlink));//申请空间
	newnode -> data = data;
	return newnode;
}


void userlink_insert_after(Userlink* head, User data)//插入用户链表节点
{
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return;
	}
	while(head -> next != NULL)
	{
		head = head -> next;
	}

	
	Userlink* newnode = user_create_node(data);//创建新节点
	//最后一个节点的指针域指向新节点
	head -> next = newnode;
}

Userlink* userlink_select_node(Userlink* userlink, char name[])//用户链表姓名查询当前节点
{
	if(userlink == NULL)
	{
		printf("用户链表头节点为空！\n");
		return NULL;
	}
	userlink = userlink->next;
	while(userlink != NULL)
	{
		if(0 == strcmp(userlink->data.name,name))
		{
			return userlink;
		}
		userlink = userlink->next;
	}
	return NULL;
}

Userlink* userlink_select_front_node(Userlink* head,char* name)//查询用户链表前一节点
{
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return NULL;
	}

	//返回前一个节点
	Userlink* pre = head ;
	head = head -> next;
	while(head != NULL)
	{
		if(strcmp(head -> data.name,name) == 0)
		{
			return pre;
		}
		printf("in the userlink select\n");
		pre = head;
		head = head -> next;
	}
	return NULL;
}

void userlink_delete_node(Userlink* head,char* name)//删除用户链表节点
{
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return ;
	}
	
	Userlink* pre = userlink_select_front_node(head,name);
	if(pre == NULL)
	{
		printf("没有此节点\n");
		return ;
	}
	Userlink* del = pre -> next;
	pre -> next = del -> next;
	free(del);
	del = NULL;
}

void userlink_load(Userlink* userlink)//用户链表加载函数
{
	if(userlink == NULL)
	{
		printf("用户链表头节点为空！\n");
		return;
	}
	FILE* fp = fopen("userlink.bin","rb");
	if(fp == NULL)
	{
		system("touch userlink.bin");
		return ;
	}
	User data;
	memset(&data,0,sizeof(User));
	while(1)
	{
		int res = fread(&data,sizeof(User),1,fp);
		if(res < 1)
		{
			break;
		}
		userlink_insert_after(userlink,data);
	}
	printf("用户数据读取成功！\n");
	fclose(fp);
	fp = NULL;
}

void userlink_save(Userlink* userlink)//用户链表保存函数
{
	if(userlink == NULL)
	{
		printf("用户链表头节点为空！\n");
		return;
	}
	FILE* fp = fopen("userlink.bin","wb");
	if(fp == NULL)
	{
		printf("打开文件失败！\n");
		return;
	}
	userlink = userlink->next;
	while(userlink != NULL)
	{
		fwrite(&userlink->data,sizeof(User),1,fp);
		userlink = userlink->next;
	}
	printf("用户数据保存成功！\n");
	fclose(fp);
	fp = NULL;
}

void userlink_free(Userlink* head)//释放用户链表
{	
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return ;
	}

	while(head != NULL)
	{
		Userlink* save = head -> next;
		free(head);
		head = save;		
	}
}

void userlink_printf_all(Userlink* head)//打印所有
{
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return ;
	}
	printf("姓名\t密码\n");
	head = head -> next;
	while(head != NULL)
	{
		printf("%s\t%s\n",head -> data.name,head -> data.password);	
		head = head -> next;
	}
}
