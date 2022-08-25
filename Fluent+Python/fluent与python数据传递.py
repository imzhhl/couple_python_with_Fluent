import socket

#IPV4,TCP协议
sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#绑定ip和端口，bind接受的是一个元组
sock.bind(('127.0.0.1',10003))
#设置监听，其值阻塞队列长度，一共可以有5个客户端和1服务器连接
sock.listen(5)

def send_function(data): 
    # 发送数据
    connection.send(bytes(data, encoding="utf-8"))

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
    
    # 接收
    received_data = receive_function() 
    
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
    send_function(send_data)
     
    #UDF数据操作结束...
    # -----------------------------------------------------------------------------------------------------------------------
    # 关闭连接
    connection.close()
    
# 关闭服务器
sock.close()
