**方法1：利用fluent中的corba接口（可被方法3取缔）**
参考小田老师：https://github.com/imzhhl/fluent_corba

**方法2：利用socket接口**
参考硫酸亚铜老师：https://www.cnblogs.com/liusuanyatong/p/12081218.html

**方法3：利用官方接口pyFluent**
帮助文档链接：https://fluentdocs.pyansys.com/index.html


* Python与MATLAB借助以上三种方法可与Fluent完成耦合计算/自动化脚本

* UDF函数的socket接口为C++编写，Fluent不支持直接编译，需要外部编译（或将用于TCP/UDP数据交互的函数单独封装为动态链接库.dll，方法参考胡坤老师公众号：CFD之道），或者使用VC++UDF Studio插件：https://vcudfstudio.bitbucket.io/

# 💡For MATLAB
## 方法一：利用MATLAB的ANSYS_aas包
据说仅适用于R2014b-2018b，但亲测R2021a也可使用，安装时提示错误，无视即可
### 使用方法
通过以``-aas``批处理模式启动fluent，读取目录下的**aas_FluentId.txt**文件，然后通过CORBA连接到Fluent服务器发送TUI或者Scheme脚本命令。
```matlab
% 实例化ORB对象
orb = initialize_orb()
% 初始化加载工具箱
load_ansys_aas;
% 读取aaS_FluentId.txt, 连接到Fluent软件
iCoFluentUnit = actfluentserver(orb,'aaS_FluentId.txt');
% 获得scheme脚本控制器实例
iFluentTuiInterpreter = iCoFluentUnit.getSchemeControllerInstance(); 
% 执行Fluent TUI命令
fluentResult = iFluentTuiInterpreter.doMenuCommandToString('report summary') 
```

## 方法二：利用socket接口

MATLAB为Server，UDF为Client, TCP连接(来自https://www.cnblogs.com/liusuanyatong/p/12081218.html)
```matlab
clc;
clear all;
close all;
s = tcpip('127.0.0.1',10001,'NetworkRole','server');
fopen(s);
data = fread(s)
sprintf('%s',data)
fwrite(s,'Hello FLuent')
fclsoe(s);
```
UDF如下：
```cpp
/************************************/
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
void FluentSocket(const std::string &sendMessage)
{
    WSADATA wsaData;
    /*用于通讯的套接字*/
    SOCKET s;
    /*MATLAB地址的相关*/
    SOCKADDR_IN matlabAddress;
    /*接收Matlab发来的信息*/
    char recvInfo[1024];
    /*UDF接收发送数据的大小*/
    int returenValue;
    /*通讯接口*/
    int Port = 10000;
    /*用于格式化输出信息*/
    std::string msg;
 
    returenValue = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (returenValue != 0)
    {
        AfxMessageBox(TEXT("Socket初始化失败！"), MB_OK);
        return;
    }
 
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
    {
        AfxMessageBox(TEXT("获取套接字失败！"), MB_OK);
        ::WSACleanup();
        return;
    }
 
    matlabAddress.sin_family = AF_INET;
    /*设置端口*/
    matlabAddress.sin_port = htons(Port);
    /*设置地址*/
    matlabAddress.sin_addr.S_un.S_addr = ::inet_addr("127.0.0.1");
 
    if (::connect(s, (SOCKADDR *)&matlabAddress, sizeof(matlabAddress)) == SOCKET_ERROR)
    {
        msg = "Socket连接失败，错误码：";
        msg += std::to_string(::WSAGetLastError());
        AfxMessageBox((TEXT(msg.c_str())), MB_OK);
        ::closesocket(s);
        ::WSACleanup();
        return;
    }
 
    returenValue = ::send(s, sendMessage.c_str(), sendMessage.size(), 0);
    if (returenValue == SOCKET_ERROR)
    {
        msg = "Socket发送失败，错误码：";
        msg += std::to_string(::WSAGetLastError());
        AfxMessageBox(TEXT(msg.c_str()), MB_OK);
        ::closesocket(s);
        ::WSACleanup();
        return;
    }
 
    returenValue = ::recv(s, recvInfo, sizeof(recvInfo), 0);
    if (returenValue == SOCKET_ERROR)
    {
        msg = "Socket接收失败，错误码：";
        msg += std::to_string(::WSAGetLastError());
        AfxMessageBox(TEXT(msg.c_str()), MB_OK);
        ::closesocket(s);
        ::WSACleanup();
        return;
    }
 
    /*注意此操作，否则fluent里面会乱码*/
    recvInfo[returenValue] = '\0';
    std::cout << recvInfo << std::endl;
 
    ::closesocket(s);
    ::WSACleanup();
}
 
DEFINE_ON_DEMAND(demo)
{
    FluentSocket("Hello Matlab!");
}
```

MATLAB为Server，UDF为Client, UDP连接(来自https://zhuanlan.zhihu.com/p/567815434）

UDF如下：
```c
#include "udf.h"
#include <stdio.h>
#include <winsock2.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")
int i = 0;
double data = 12.5;//传输数据 可以是多个数据 采用数组的方式
double fluentUDP(double temp1)   //每次传输调用一次
{
//1.初始化，使用socket()函数获取一个socket文件描述符
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//2.绑定本地的相关信息，如果不绑定，则系统会随机分配一个端口号
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;//使用IPv4地址
    local_addr.sin_addr.s_addr = inet_addr("xx.xxx.xxx.xxx");//本机IP地址
    local_addr.sin_port = htons(8590);//端口
    bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));//将套接字和IP、端口绑定
//3.发送数据到指定的ip和端口,'xx.xxx.xxx.xxx'表示目的ip地址，2589表示目的端口号 
    struct sockaddr_in sock_addr = {0};
    sock_addr.sin_family = AF_INET;                         // 设置地址族为IPv4
    sock_addr.sin_port = htons(4901);			// 设置地址的端口号信息
    sock_addr.sin_addr.s_addr = inet_addr("xx.xxx.xxx.xxx");//　设置IP地址
//4.等待接收对方发送的数据 阻塞型
    double recvbuf,sendbuf;
    sendbuf = temp1;
    struct sockaddr_in recv_addr;
    int nSize=sizeof(recv_addr);
    sendto(sockfd, (char *)&sendbuf, sizeof(sendbuf), 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    Message("sendbuf=%f\n",sendbuf);
    recvfrom(sockfd, (char *)&recvbuf, sizeof(recvbuf), 0,(struct sockaddr*)&recv_addr,&nSize);
    Message("recvbuf=%f\n",recvbuf);
    closesocket(sockfd);
    WSACleanup();//停止Winsock
    return recvbuf;
}

DEFINE_EXECUTE_AT_END(data_processing)
{
    #if RP_HOST
        i = i + 1;
        data = data + 12.5;
	double recvbuf;
	Message("Times=%d\n",i);
	recvbuf = fluentUDP(data);//fluent先将data发送给Matlab 然后阻塞等待接受处理好后的数据，可以是一个数或者数组
    #endif
}

```
MATLAB如下：
```matlab
%使用前需要先确定接收到数据的个数，修改 number_of_double
%按照数组的形式传输多个数据
%先关闭之前可能存在的UDP
clc;clear
delete(instrfindall);
%地址信息绑定
ip = 'xx.xxx.xxx.xxx';
local_port = 4901;
remote_port = 8590;
number_of_double = 1;%接收到数据的个数 这个是在simulink模块里面使用
%配置udp，打开连接
count = 0;
u = udp(ip,'RemotePort',remote_port,'LocalPort',local_port);
fopen(u);
while(1)
    %循环查询是否接收到数据
    bytes = u.BytesAvailable;
    if bytes > 0
        %接受数据部分
        count = count + 1
        receive = fread(u);
        matlab_receive_uint8 = uint8(receive)';
        simout_unpack = sim('udp_unpack');%调用simulink模块，将uint8组合为double数据类型
        matlab_receive_double = simout_unpack.matlab_receive_double.Data
        %处理数据部分
        disp('成功接收数据，开始处理')
        matlab_send_double = matlab_receive_double + 12.5;%可另外写一个function函数进行数据处理然后发送到fluent
        disp('成功处理数据，发送数据中...');
        %发送数据部分
        simout_pack = sim('udp_pack');%调用simulink模块，将double拆分为uint8数据类型
        matlab_send_uint8 = simout_pack.matlab_send_uint8.Data;
    	fwrite(u,matlab_send_uint8);
        disp('数据已发送完成');
    	disp('***********************');
    end
end
fclose(u);
delete(u);
clear u;

```
# 💡For Python
## 方法一：利用fluent_corba包

本模块提供对ansys fluent中CORBA连接的Python支持，作为后续开发pyfluent的基础模块。使用模块可以发送TUI和Scheme脚本命令到fluent实例，并自动去执行和返回结果。

本模块依赖于ansys fluent提供的帮助文档，并使用来自于[omniORB](https://sourceforge.net/projects/omniorb/) 的编译库。

### 安装使用

预编译的二进制库目前只支持Windows x64平台的Python3.7、3.8版本，安装方法如下：
	
`pip install fluent_corba-0.2.0-cp37-none-win_amd64.whl`

`pip install fluent_corba-0.2.0-cp38-none-win_amd64.whl`

### 使用方法
通过以``-aas``批处理模式启动fluent，读取目录下的**aas_FluentId.txt**文件，然后通过CORBA连接到Fluent服务器发送TUI或者Scheme脚本命令。

```python
# encoding: utf-8
import time
import pathlib
import os
import sys
from fluent_corba import CORBA
import subprocess

# 定义Fluent的启动位置，例如2020R1
ansysPath = pathlib.Path(os.environ["AWP_ROOT201"])
fluentExe = str(ansysPath/"fluent"/"ntbin"/"win64"/"fluent.exe")

# 定义工作目录
workPath = pathlib.Path(r"E:\Workdata\Fluent_Python")
aasFilePath = workPath/"aaS_FluentId.txt"
# 清除之前存在的aaS*.txt文件
for file in workPath.glob("aaS*.txt"):
    file.unlink()
# 启动Fluent软件
fluentProcess = subprocess.Popen(f'"{fluentExe}" 3ddp -aas', shell=True, cwd=str(workPath))
# 监控aaS_FluentId.txt文件生成，等待corba连接
while True:
    try:
        if not aasFilePath.exists():
            time.sleep(0.2)
            continue
        else:
            if "IOR:" in aasFilePath.open("r").read():
                break
    except KeyboardInterrupt:
        sys.exit()
        
# 初始化orb环境
orb = CORBA.ORB_init()
# 获得Fluent实例单元
fluentUnit = orb.string_to_object(aasFilePath.open("r").read())
scheme = fluentUnit.getSchemeControllerInstance()
print(scheme.execSchemeToString(r'(read-case "E:\Workdata\Fluent_Python\base-design.msh")'))
print(scheme.doMenuCommandToString("/mesh/check"))
```

## 方法二：利用socket接口
Python为Server，UDF为Client, TCP连接
```python
import socket

#IPV4,TCP协议
sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#绑定ip和端口，bind接受的是一个元组
sock.bind(('127.0.0.1',10002))
#设置监听，其值阻塞队列长度，一共可以有5个客户端和1服务器连接
sock.listen(5)

def send_function(data):
    # 将发送数据转化为String
    s=str(data)
    # 发送数据
    connection.send(bytes(s, encoding="utf-8"))

def receive_function():
    # 接收数据,并存入buf
    buf = connection.recv(40960)
    return(buf.decode('utf-8'))
    
def DNN(received_data):
    #神经网络函数……
    return 0

while True:
    # 等待客户请求
    connection,address = sock.accept()
    #-----------------------------------------------------------------------------------------------------------------------
    #下面进行UDF的数据操作...
    
    received_data = receive_function() 
    print(f'{received_data}')
    
    send_data= str(DNN(received_data))
    send_function(send_data)
     
    #UDF数据操作结束...
    # -----------------------------------------------------------------------------------------------------------------------
    # 关闭连接
    connection.close()
    
# 关闭服务器
sock.close()
```
Python为Server，UDF为Client, UDP连接
```python
import socket

#IPV4,TCP协议
sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)#ipv4,udp
#绑定ip和端口，bind接受的是一个元组
sock.bind(('127.0.0.1',54377))

while True:

    #-----------------------------------------------------------------------------------------------------------------------
    #下面进行UDF的数据操作...
    
    # 接收
    received_data,clientAddress=sock.recvfrom(40960)
    
    # 转化为数组
    received_data_str = received_data.split()
    received_data_float = list(map(float, received_data_str)) 
    print(f'{received_data_float[0]};{received_data_float[1]};{received_data_float[2]};{received_data_float[3]}')

    # 发送的数据用空格隔开，然后组合成字符串
    a = 10.2
    b = 1.1221
    c = 1002.12
    space = ' '
    send_data= str(a) + space + str(b) + space + str(c)
    
    # 发送
    sock.sendto(bytes(send_data, encoding = "utf8"),clientAddress)
     
    #UDF数据操作结束...
    # -----------------------------------------------------------------------------------------------------------------------
    # 关闭连接
    
# 关闭服务器
sock.close()
```

## 方法三：利用pyFluent包

Fluent 2022R2版本推出了pyFluent，这实际上是提供了一个利用python访问Fluent进程的工具，利用此工具可以实现利用python控制Fluent，可以实现在不启用Fluent GUI的情况下，完成参数设置、计算求解以及或数据结果输出的功能。

经过本人测试，这个方法非常好用，方法一可以淘汰了。
log：2022-09-20 在大量重复计算是，pyfluent会出现不识别tui命令的bug，希望后期版本有改进.

帮助文档链接：https://fluentdocs.pyansys.com/index.html
```python
import ansys.fluent.core as pyfluent
import numpy as np
import random
import time
import matplotlib.pyplot as plt
#%% 利用pyFluent包连接fluent,该包的使用仅限于Fluent 2022R2以后版本

# 定义工作目录
import_filename = r'F:\ZHHL\TE_Doctor\CASES\case220626\python_fluent\python\fluent16-uniformmesh-0814'
UDF_Path = r'F:\ZHHL\TE_Doctor\CASES\case220626\python_fluent\python\udf_source.c'

session = pyfluent.launch_fluent(version = "2d", precision='double',processor_count = 1, show_gui=False)

# 用工作站远程计算，连接现有的窗口
# session = pyfluent.launch_fluent(ip='192.168.31.230', port=63993, start_instance=False)

tui = session.solver.tui
root = session.solver.root

# 初始化patch
tui.solve.patch('air', [], 'uds-0', 'no', '0')

# fluent计算
tui.solve.set.equations('flow', 'no')
tui.solve.set.equations('ke', 'no')
tui.solve.set.equations('uds-0', 'yes')
tui.solve.iterate(10)

```
