#include "udf.h"
#include <stdio.h>
#include <winsock2.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

char *fluentUDP(char *sendmessage)   //每次传输调用一次，传输字符数组
{
	//1.初始化，使用socket()函数获取一个socket文件描述符
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	 SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	//2.绑定本地的相关信息，如果不绑定，则系统会随机分配一个端口号，40960表示本地端口号 
	struct sockaddr_in localAddress = {0};
	localAddress.sin_family = AF_INET;//使用IPv4地址
	localAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//本机IP地址
	localAddress.sin_port = htons(40960);//端口
	bind(sockfd, (struct sockaddr*)&localAddress, sizeof(localAddress));//将套接字和IP、端口绑定
	//3.发送数据到指定的ip和端口,'xx.xxx.xxx.xxx'表示目的ip地址，54377表示目的端口号 
	struct sockaddr_in pythonAddress = {0};
	pythonAddress.sin_family = AF_INET;                         // 设置地址族为IPv4
	pythonAddress.sin_port = htons(54377);			// 设置地址的端口号信息
	pythonAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//　设置IP地址
	//4.等待接收对方发送的数据 阻塞型
	static char recvbuf[1024];
	char	*sendbuf;
	sendbuf = sendmessage;
	struct sockaddr_in recv_addr;
	int nSize=sizeof(recv_addr);
	sendto(sockfd, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&pythonAddress, sizeof(pythonAddress));
//	Message0("sendbuf=%s\n",sendbuf);
	recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0,(struct sockaddr*)&recv_addr,&nSize);
//	Message0("recvbuf=%s\n",recvbuf);
	closesocket(sockfd);
	WSACleanup();//停止Winsock
	return recvbuf;
}

DEFINE_ON_DEMAND(data_processing)
{

	double send_data_1, send_data_2, send_data_3, send_data_4; //定义将发送的4个数据
	char send_data_str_1[40], send_data_str_2[40], send_data_str_3[40], send_data_str_4[40]; //定义将发送的4个字符串（double转化而来）
	char *space_str = " ";//用空格分割不同数据

	char *received_data_str; //接收的字符串
	char *pEnd;
	char temp[40];

	double received_date_1, received_date_2, received_date_3;//接收的数据，3个

	/*通过FluentSocket函数，发送数据到python (send_data_str)，接收python返回的数据 (received_data_str)*/ 
	/*本实例中使用单精度浮点数传递，保留小数点后6位*/

	send_data_1 = 1.12;
	send_data_2 = 10.23;
	send_data_3 = 0.45;
	send_data_4 = 0.00040;

	// 把双精度浮点数send_data转换为字符串，存放在send_data_str中
	sprintf(send_data_str_1,"%.6f",send_data_1);
	sprintf(send_data_str_2,"%.6f",send_data_2);
	sprintf(send_data_str_3,"%.6f",send_data_3);
	sprintf(send_data_str_4,"%.6f",send_data_4);

	//将send_data_str_1、send_data_str_2、send_data_str_3、send_data_str_4四个字符串连接在一起，存在send_data_str_1中，不同数据用空格分割
	strcat(send_data_str_1, space_str);
	strcat(send_data_str_1, send_data_str_2);
	strcat(send_data_str_1, space_str);	
	strcat(send_data_str_1, send_data_str_3);
	strcat(send_data_str_1, space_str);		
	strcat(send_data_str_1, send_data_str_4);

	//通过socket接口传递数据
	received_data_str = fluentUDP(send_data_str_1); //fluent先将data发送给python， 然后阻塞等待接受处理好后的数据
	strcpy(temp, received_data_str);

	//将用空格隔开的字符串分割成双精度浮点数, python传递3个数给UDF
	received_date_1 = strtod(temp, &pEnd);
	received_date_2 = strtod(pEnd, &pEnd);
	received_date_3 = strtod(pEnd, &pEnd);

	Message0("\nreceived_date: \n");
	Message0("%.6f\n", received_date_1);
	Message0("%.6f\n", received_date_2);
	Message0("%.6f\n", received_date_3);	
	Message0("\nsend_data\n");
	Message0("%s\n", send_data_str_1);
}
