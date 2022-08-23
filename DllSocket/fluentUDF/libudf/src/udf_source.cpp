#include "udf.h"
#define	 di 0.05
#include <iostream>
#include <string.h>


float direct_no1_x; //x 坐标
float direct_no1_y; //y 坐标

/*源项宏实例*/
/*define source宏会自动循环计算域中所有的网格*/
/*define source宏在每个迭代步都会自动调用*/
DEFINE_SOURCE(direct_source_no1, c, t, dS, eqn)
{	
	 	
    real x[ND_ND];  		
   	double source;
	C_CENTROID(x,c,t);   
    
    /*定义污染源的位置*/
	if(x[0]>direct_no1_x - di&&x[0]<direct_no1_x + di&&x[1]>direct_no1_y - di &&x[1]<direct_no1_y + di )
	{
		source=1;
	}
	else
	{
		source=0;
	}
   	dS[eqn]=0;
    return source;
}

/*Define the diffusivity of biological particles in room air*/
DEFINE_DIFFUSIVITY(diffu_coef,c,t,i)
{
    return C_R(c,t) * 2.88e-05 + C_MU_EFF(c,t) / 0.7;
} 

DEFINE_ON_DEMAND(python_udf_socket)
{
	float send_data;
	char *send_data_str;
	char *received_data_str;
	char send_data_str_temp[20];
	char char_x[3];//截取后字符串,x 坐标
	char char_y[3];//截取后字符串,y 坐标
	char temp[10];
	extern char *FluentSocket(const std::string &sendMessage);
	
	/*通过FluentSocket函数，发送数据到python (send_data_str)，接收python返回的数据 (received_data_str)*/ 
	/*本实例中只需要接收数据即可，发送无意义数据2022*/
	send_data = 2022;
	send_data_str = gcvt(send_data, 8, send_data_str_temp);

	received_data_str = FluentSocket(send_data_str);
	strcpy(temp,received_data_str);

	char_x[0] = temp[0];
	char_x[1] = temp[1];
	char_x[2] = temp[2];

	char_y[0] = temp[3];
	char_y[1] = temp[4];
	char_y[2] = temp[5];

	direct_no1_x = atof(char_x);
	direct_no1_y = atof(char_y);

	Message0("x = %f\n",direct_no1_x);
	Message0("y = %f\n",direct_no1_y);
	Message0("\n");
}