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
