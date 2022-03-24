**方法一：利用fluent中的corba接口**
参考小田老师：https://github.com/imzhhl/fluent_corba

**方法二：利用socket接口**
参考硫酸亚铜老师：https://www.cnblogs.com/liusuanyatong/p/12081218.html

**Python与MATLAB借助以上两种方法均与Fluent完成耦合计算**

# For MATLAB
## 方法一：利用MATLAB的ANSYS_aas包

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

## 方法二：利用socket函数

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
# For Python
## 方法一：利用fluent_corba包
参考链接为：https://github.com/ansys-dev/fluent_corba

本模块提供对ansys fluent中CORBA连接的Python支持，作为后续开发pyfluent的基础模块。使用模块可以发送TUI和Scheme脚本命令到fluent实例，并自动去执行和返回结果。

本模块依赖于ansys fluent提供的帮助文档，并使用来自于[omniORB](https://sourceforge.net/projects/omniorb/) 的编译库。

## 安装使用

预编译的二进制库目前只支持Windows x64平台的Python3.7、3.8版本，安装方法如下：
	
`pip install fluent_corba-0.2.0-cp37-none-win_amd64.whl`

`pip install fluent_corba-0.2.0-cp38-none-win_amd64.whl`

## 使用方法
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

## 方法二：利用socket接口完成FLuent UDf和python的通讯
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

while True:
    # 等待客户请求
    connection,address = sock.accept()
    #-----------------------------------------------------------------------------------------------------------------------
    #下面进行UDF的数据操作...

    send_data=3.14159
    send_function(data)
    
    received_dare = receive_function()
    print(f'{received_dare}')
    
    #UDF数据操作结束...
    # -----------------------------------------------------------------------------------------------------------------------
    # 关闭连接
    connection.close()
    
# 关闭服务器
sock.close()
```

