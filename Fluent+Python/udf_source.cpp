﻿/************************************/
/* Copyright(c)**********************/
/* All rights reserved.**************/
/* Created on  2019-12-22 21:12:31***/
/* @author: 硫酸亚铜******************/
/************************************/

#include "udf.h"
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

/*socket的相关操作尽量单独写函数
  否则连接失败可能会卡死fluent*/
float FluentSocket(const std::string &sendMessage)
{
	WSADATA wsaData;
	/*用于通讯的套接字*/
	SOCKET s;
	/*python地址的相关*/
	SOCKADDR_IN pythonAddress;
	/*接收python发来的信息*/
	char recvInfo[1024];
	/*UDF接收发来数据的大小*/
	int returenValue;
	/*通讯接口*/
	int Port = 10002;
	/*用于格式化输出信息*/
	std::string msg;

	returenValue = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (returenValue != 0)
	{
		AfxMessageBox(TEXT("Socket初始化失败！"), MB_OK);
		return 0;
	}

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		AfxMessageBox(TEXT("获取套接字失败！"), MB_OK);
		::WSACleanup();
		return 0;
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
		return 0;
	}

	returenValue = ::send(s, sendMessage.c_str(), sendMessage.size(), 0);
	if (returenValue == SOCKET_ERROR)
	{
		msg = "Socket发送失败，错误码：";
		msg += std::to_string((long double)::WSAGetLastError());
		AfxMessageBox(TEXT(msg.c_str()), MB_OK);
		::closesocket(s);
		::WSACleanup();
		return 0;
	}

	returenValue = ::recv(s, recvInfo, sizeof(recvInfo), 0);
	if (returenValue == SOCKET_ERROR)
	{
		msg = "Socket接收失败，错误码：";
		msg += std::to_string((long double)::WSAGetLastError());
		AfxMessageBox(TEXT(msg.c_str()), MB_OK);
		::closesocket(s);
		::WSACleanup();
		return 0;
	}

	/*注意此操作，否则fluent里面会乱码*/
	recvInfo[returenValue] = '\0';
	return atof(recvInfo);
//	std::cout << recvInfo << std::endl;
	::closesocket(s);
	::WSACleanup();
}

DEFINE_ON_DEMAND(demo)
{
	float a;
	a=FluentSocket("1234");
	Message0("a=%f\n",a);
}