# encoding: utf-8
# 导入fluent_corba下的CORBA接口类和其他必要模块
from fluent_corba import CORBA
import time
import pathlib
import os, sys
import subprocess

# 定义工作目录
workPath = pathlib.Path(r"E:\ZHHL_date\case22-02-23\p8-unstable")
aasFilePath = workPath/"aaS_FluentId.txt"
# 初始化orb环境
orb = CORBA.ORB_init()
# 获得fluent服务器会话实例
fluentUnit = orb.string_to_object(aasFilePath.open("r").read())
# 获得scheme脚本控制器实例
scheme = fluentUnit.getSchemeControllerInstance()

# 发送Scheme脚本读入case文件
# result = scheme.execScheme(r'(read-case "P8-unstable.cas")')

#更改UDF文件函数
def change_UDF_file(Lin):
    with open('E:\ZHHL_date\case22-02-23\p8-unstable\hendrik-unstable.c', 'r') as old_file:
        with open('E:\ZHHL_date\case22-02-23\p8-unstable\hendrik-unstable.c', 'r+') as new_file:
            current_line = 0
            # 定位到需要删除的行
            while current_line < (51 - 1):
                old_file.readline()
                current_line += 1
         
            seek_point = old_file.tell() # 当前光标在被删除行的行首，记录该位置       
            new_file.seek(seek_point, 0) # 设置光标位置            
            new_file.write(f'#define Lin {Lin}\n')  # 在该行设置新内容      
            old_file.readline() # 读需要删除的行，光标移到下一行行首      
            next_line = old_file.readline() # 被删除行的下一行读给 next_line
            # 连续覆盖剩余行，后面所有行上移一行
            while next_line:
                new_file.write(next_line)
                next_line = old_file.readline()
            new_file.truncate() # 写完最后一行后截断文件，因为删除操作，文件整体少了一行，原文件最后一行需要去掉 
            
Lins = [-4,-5,-10,-20,-30,-40,-50,-100,-200,-300,-400,-500,-1000,-2000,-3000]

for Lin in  Lins:
    #更改UDF文件
    change_UDF_file(Lin)   
        
    # 编译修改后的UDF并加载               
    scheme.doMenuCommand("/define/user-defined/compiled-functions compile , , yes hendrik-unstable.c , ,")
    scheme.doMenuCommand("/define/user-defined/compiled-functions load ,")
    
    # 设置迭代步数,并开始计算
    fluentUnit.setNrIterations(5000)
    fluentUnit.calculate()
    fluentUnit.saveCase(f"P8-unstable-L={Lin}")
    fluentUnit.saveData(f"P8-unstable-L={Lin}")
