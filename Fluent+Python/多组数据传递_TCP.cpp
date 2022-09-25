/************************************************************************************/
/*
python与fluent互相传递多组数据
python传递三个数据给fluent，fluent传递4个数据给python
Author: Hongliang Zhang_WHU                                                                   
Date:   2022-08-25
*/
/************************************************************************************/

#include "udf.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#pragma comment(lib, "ws2_32.lib")
#define	 di 0.05

float direct_no1_x; //x 坐标
float direct_no1_y; //y 坐标
char recvInfo[1024];/*接收python发来的信息*/

/*socket的相关操作尽量单独写函数否则连接失败可能会卡死fluent*/
char *FluentSocket(const std::string &sendMessage)
{
    WSADATA wsaData;
    /*用于通讯的套接字*/
    SOCKET s;
    /*python地址的相关*/
    SOCKADDR_IN pythonAddress;
    /*接收python发来的信息*/
    char recvInfo[1024];
    /*UDF接收发送数据的大小*/
    int returenValue;
    /*通讯接口*/
    int Port = 10003;
    /*用于格式化输出信息*/
    std::string msg;
 
    returenValue = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (returenValue != 0)
    {
        AfxMessageBox(TEXT("Socket初始化失败！"), MB_OK);
        return NULL;
    }
 
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        AfxMessageBox(TEXT("获取套接字失败！"), MB_OK);
        ::WSACleanup();
        return NULL;
    }
 
    pythonAddress.sin_family = AF_INET;
    /*设置端口*/
    pythonAddress.sin_port = htons(Port);
    /*设置地址*/
    pythonAddress.sin_addr.S_un.S_addr = ::inet_addr("127.0.0.1");
 
    if (::connect(s, (SOCKADDR *)&pythonAddress, sizeof(pythonAddress)) == SOCKET_ERROR)
    {
        msg = "Socket连接失败，错误码：";
        msg += std::to_string((long double)::WSAGetLastError());
        AfxMessageBox((TEXT(msg.c_str())), MB_OK);
        ::closesocket(s);
        ::WSACleanup();
        return NULL;
    }
 
    returenValue = ::send(s, sendMessage.c_str(), sendMessage.size(), 0);
    if (returenValue == SOCKET_ERROR)
    {
        msg = "Socket发送失败，错误码：";
        msg += std::to_string((long double)::WSAGetLastError());
        AfxMessageBox(TEXT(msg.c_str()), MB_OK);
        ::closesocket(s);
        ::WSACleanup();
        return NULL;
    }
 
    returenValue = ::recv(s, recvInfo, sizeof(recvInfo), 0);
    if (returenValue == SOCKET_ERROR)
    {
        msg = "Socket接收失败，错误码：";
        msg += std::to_string((long double)::WSAGetLastError());
        AfxMessageBox(TEXT(msg.c_str()), MB_OK);
        ::closesocket(s);
        ::WSACleanup();
        return NULL;
    }
 
    /*注意此操作，否则fluent里面会乱码*/
	recvInfo[returenValue] = '\0';
	return (recvInfo);
	::closesocket(s);
	::WSACleanup();
}

DEFINE_ON_DEMAND(python_udf_socket)
{
	double send_data_1, send_data_2, send_data_3, send_data_4; //定义将发送的4个数据
	char send_data_str_1[40], send_data_str_2[40], send_data_str_3[40], send_data_str_4[40]; //定义将发送的4个字符串（double转化而来）
	char space_str[] = " ";//用空格分割不同数据

	char *received_data_str;
	char *pEnd;
	char temp[40];

	double received_date_1, received_date_2, received_date_3;//接收的数据，3个

	/*通过FluentSocket函数，发送数据到python (send_data_str_1)，接收python返回的数据 (received_data_str)*/ 
   	/*本实例中使用单精度浮点数传递，保留小数点后6位*/

	send_data_1 = 1.12;
	send_data_2 = 10.23;
	send_data_3 = 0.45;
	send_data_4 = 0.00040;

	// 把双精度浮点数send_data转换为字符串，存放在send_data_str中
	sprintf(send_data_str_1,"%.6f",send_data_1);
	sprintf(send_data_str_2,"%.6f",send_data_2);
	sprintf(send_data_str_3,"%.6f",send_data_3);
	sprintf(send_data_str_4,"%.6f",send_data_4);

	//将send_data_str_1、send_data_str_2、send_data_str_3、send_data_str_4四个字符串连接在一起，存在send_data_str_total中，不同数据用空格分割
	strcat(send_data_str_1, space_str);
	strcat(send_data_str_1, send_data_str_2);
	strcat(send_data_str_1, space_str);	
	strcat(send_data_str_1, send_data_str_3);
	strcat(send_data_str_1, space_str);		
	strcat(send_data_str_1, send_data_str_4);	

	//通过socket接口传递数据
	received_data_str = FluentSocket(send_data_str_1);
	strcpy(temp, received_data_str);

	//将用空格隔开的字符串分割成双精度浮点数, python传递3个数给UDF
	received_date_1 = strtod(temp, &pEnd);
	received_date_2 = strtod(pEnd, &pEnd);
	received_date_3 = strtod(pEnd, &pEnd);

	Message0("\n");
	Message0("%.6f\n", received_date_1);
	Message0("%.6f\n", received_date_2);
	Message0("%.6f\n", received_date_3);	
	Message0("\n");
	Message0("%s\n", send_data_str_1);
}
