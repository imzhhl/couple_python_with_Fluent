/************************************************************************************/
/*
User-Defined Function for source identification using MCMC meathod
Author: Hongliang Zhang_WHU                                                                   
Date:   2022-07-20
Type:	2D
*/
/************************************************************************************/

#include "udf.h"
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#define	 di 0.05

float direct_no1_x; //x 坐标
float direct_no1_y; //y 坐标

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

/*源项宏实例*/
/*define source宏会自动循环计算域中所有的网格*/
/*define source宏在每个迭代步都会自动调用*/
DEFINE_SOURCE(direct_source_no1, c, t, dS, eqn)
{	
	 	
    real x[ND_ND];
   	C_CENTROID(x,c,t);   	
   	float source;
    
    /*定义污染源的位置*/
	if(x[0]>direct_no1_x - di&&x[0]<direct_no1_x + di&&x[1]>direct_no1_y - di &&x[1]<direct_no1_y + di )
	{
		source=1;
	}
	else
	{
		source=0;
	}
   	dS[eqn]=0;
    return source;
}

/*Define the diffusivity of biological particles in room air*/
DEFINE_DIFFUSIVITY(diffu_coef,c,t,i)
{
    return C_R(c,t) * 2.88e-05 + C_MU_EFF(c,t) / 0.7;
} 

DEFINE_ON_DEMAND(socket)
{
	float send_data;
	char *send_data_str;
	char *received_data_str;
	char send_data_str_temp[20];
	char char_x[3];//截取后字符串,x 坐标
    char char_y[3];//截取后字符串,y 坐标
	char temp[10];

	/*通过FluentSocket函数，发送数据到python (send_data_str)，接收python返回的数据 (received_data_str)*/ 
   	/*本实例中只需要接收数据即可，发送无意义数据2022*/
   	send_data = 2022;
    send_data_str = gcvt(send_data, 8, send_data_str_temp);

	received_data_str = FluentSocket(send_data_str);
	strcpy(temp,received_data_str);

	char_x[0] = temp[0];
	char_x[1] = temp[1];
	char_x[2] = temp[2];

	char_y[0] = temp[3];
	char_y[1] = temp[4];
	char_y[2] = temp[5];

	direct_no1_x = atof(char_x);
	direct_no1_y = atof(char_y);
	
	Message0("received_data=%s\n",received_data_str);
	Message0("x = %f\n",direct_no1_x);
	Message0("y = %f\n",direct_no1_y);
}
