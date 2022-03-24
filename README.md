**方法一：利用fluent中的corba接口**

**方法二：利用socket接口**

**Fluent与MATLAB借助以上两种方法均可完成耦合计算**

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
