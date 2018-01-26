#ifndef MYCHAT_CTRL_H
#define MYCHAT_CTRL_H

void server_ctrl_1(int clientfd, Online net_message);//服务器 接收线程 1级控制层


void client_ctrl_1(int clientfd);//客户端 主线程 1级控制层
void client_ctrl_2(int clientfd, char* name);//客户端 主线程 2级控制层
#endif
