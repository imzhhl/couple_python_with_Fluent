# 方法一：利用MATLAB的ANSYS_aas包

# 方法二：利用socket函数

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
