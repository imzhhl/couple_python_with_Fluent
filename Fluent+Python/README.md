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
