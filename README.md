# 方法一：利用fluent_corba包
参考链接为：https://github.com/ansys-dev/fluent_corba

本模块提供对ansys fluent中CORBA连接的Python支持，作为后续开发pyfluent的基础模块。使用模块可以发送TUI和Scheme脚本命令到fluent实例，并自动去执行和返回结果。

本模块依赖于ansys fluent提供的帮助文档，并使用来自于[omniORB](https://sourceforge.net/projects/omniorb/) 的编译库。


# 方法二：利用socket接口完成FLuent UDf和python的通讯

参考链接为：https://www.cnblogs.com/liusuanyatong/p/12081218.html
