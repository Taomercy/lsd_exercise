#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "mychat_model.h"

Onlink* on_create_node(Online data)//创建用户链表节点
{
	Onlink* newnode = calloc(1,sizeof(Onlink));//申请空间
	newnode -> data = data;
	return newnode;
}


void onlink_insert_after(Onlink* head, Online data)//插入用户链表节点
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

	
	Onlink* newnode = on_create_node(data);//创建新节点
	//最后一个节点的指针域指向新节点
	head -> next = newnode;
}

Onlink* onlink_select_node(Onlink* head, char name[])//用户链表姓名查询当前节点
{
	if(head == NULL)
	{
		printf("用户链表头节点为空！\n");
		return NULL;
	}
	head = head->next;
	while(head != NULL)
	{
		if(0 == strcmp(head->data.name,name))
		{
			return head;
		}
		head = head->next;
	}
	return NULL;
}
Onlink* onlink_select_int_node(Onlink* head, int clientfd)//查找clientfd节点
{
	if(head == NULL)
	{
		printf("头节点为空!\n");
		return NULL;
	}
	head = head -> next;
	while(head != NULL)
	{
		if( head->data.clientfd == clientfd)
		{
			return head;
		}
		head = head -> next;
	}
	return NULL;
}

Onlink* onlink_select_front_node(Onlink* head,char* name)//查询用户链表前一节点
{
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return NULL;
	}

	//返回前一个节点
	Onlink* pre = head ;
	head = head -> next;
	while(head != NULL)
	{
		if(strcmp(head -> data.name,name) == 0)
		{
			return pre;
		}
		printf("in the onlink select\n");
		pre = head;
		head = head -> next;
	}
	return NULL;
}

void onlink_delete_node(Onlink* head,char* name)//删除用户链表节点
{
	if(NULL == head)
	{
		fprintf(stderr,"头节点为空\n");
		return ;
	}
	
	Onlink* pre = onlink_select_front_node(head,name);
	if(pre == NULL)
	{
		printf("没有此节点\n");
		return ;
	}
	Onlink* del = pre -> next;
	pre -> next = del -> next;
	free(del);
	del = NULL;
	printf("节点已删除\n");
}

void print_all(Onlink* head)
{
	if(head == NULL)
	{
		printf("头节点为空！\n");
		return;
	}
	head = head -> next;
	printf("在线用户信息：\n");
	while(head != NULL)
	{
//		printf("ip:%s\tclientfd:%d\tname:%s\tsecond:%d\n",head->data.ip,head->data.clientfd,head->data.name,head->data.second);
		printf("name:%s\tclientfd:%d\n",head->data.name,head->data.clientfd);
		head = head->next;
	}
}

