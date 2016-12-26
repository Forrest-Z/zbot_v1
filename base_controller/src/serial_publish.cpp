//订阅/car_vel主题,并进行串口通讯
#include "ros/ros.h"  //ros需要的头文件
#include "test_p/car_msg.h"   //调用消息所用到的头文件
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <test_p/odometry_data.h>

#include <string>        //以下为串口通讯需要的头文件
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <math.h>
#include "serial/serial.h"

using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

unsigned char temp0=0x01 ;   //右轮正符号
unsigned char temp1=0x02 ;   //右轮负符号
unsigned char temp2=0x03 ;   //左轮正符号
unsigned char temp3=0x04;    //左轮负符号
unsigned char temp4=0x05;    //左右轮速度均为0时
unsigned char temp5=0x06;
unsigned char data_terminal0=0x0d;
unsigned char data_terminal1=0x0a;

bool reset_odom=false;

unsigned char publish_data[10]={0};

string rec_buffer;

//发送给下位机的左右轮速度，里程计的坐标和方向
union floatData
{
     float d;
     unsigned char data[4];
 }rightdata,leftdata,position_x,position_y,oriention,vel_linear,vel_angular;

int count = 0;

 float odom_vx = 0;      //里程计要发布的速度信息
 float odom_vy = 0;
 float odom_vth = 0;

float lastPosition_x= 0 ;
float lastPosition_y= 0 ;
float lastOriention= 0 ;

float delta_x=0;
float delta_y=0;
float delta_th=0;
float dt=0;

void enumerate_ports()
{
	vector<serial::PortInfo> devices_found = serial::list_ports();

	vector<serial::PortInfo>::iterator iter = devices_found.begin();

	while( iter != devices_found.end() )
	{
		serial::PortInfo device = *iter++;
	}
}

void carCallback(const test_p::car_msg& msg)
{
	string port("/dev/ttyUSB0");
	unsigned long baud = 115200;
	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));

	// if(((int)msg.rightspeed==1)&&((int)msg.leftspeed==1))
	// {
	// 	cout<<"lala"<<endl;
	// 	for(int i=0;i++;i<8)
	// 	{
	// 		publish_data[i]==0x32;
	// 	}
	// 	reset_odom=true;
	// }
	// else
	// {
		rightdata.d=msg.rightspeed;
		leftdata.d=msg.leftspeed;

		for(int i=0;i<4;i++)
		{
			publish_data[i]=rightdata.data[i];
			publish_data[i+4]=leftdata.data[i];
		}
	// }
	publish_data[8]=data_terminal0;
	publish_data[9]=data_terminal1;

	my_serial.write(publish_data,10);
}

int main(int argc, char **argv)
{
	string port("/dev/ttyUSB1");
	unsigned long baud = 115200;
	serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));

	ros::init(argc, argv, "serial_publish");

	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("car_vel", 20, carCallback);
	ros::Publisher  odom_pub = n.advertise<test_p::odometry_data>("odom_data", 20);

  	ros::Time current_time, last_time;
  	test_p::odometry_data odom_data;

  	current_time = ros::Time::now();
  	last_time = ros::Time::now();

	while(ros::ok())
	{
		rec_buffer =my_serial.readline(25,"\n");
		const char *receive_data=rec_buffer.data();
		if(rec_buffer.length()==21)
		{
			for(int i=0;i<4;i++)
			{
				position_x.data[i]=receive_data[i];
				position_y.data[i]=receive_data[i+4];
				oriention.data[i]=receive_data[i+8];
				vel_linear.data[i]=receive_data[i+12];
				vel_angular.data[i]=receive_data[i+16];
			}
			current_time = ros::Time::now();
			position_x.d/=1000;
			position_y.d/=1000;
			vel_linear.d/=1000;
			//dt = (current_time - last_time).toSec();
			//delta_x=position_x.d-lastPosition_x;
			//delta_y=position_y.d-lastPosition_y;
			//delta_th=oriention.d-lastOriention;

			//odom_vx=delta_x/(cos(oriention.d)*dt);
			//odom_vy=(delta_y*cos(oriention.d)-delta_x*sin(oriention.d))/dt;
			//odom_vth=delta_th/dt;				

			cout<<vel_angular.d<<endl;
			odom_data.x=position_x.d;
			odom_data.y=position_y.d;
			odom_data.th=oriention.d;
			odom_data.vel_x=vel_linear.d;
			odom_data.vel_th=vel_angular.d;

			odom_pub.publish(odom_data);

			//last_time = current_time;
			//lastPosition_x=position_x.d;
			//lastPosition_y=position_y.d;
			//lastOriention=oriention.d;
		}
		else
		{
			// if((receive_data[0]==0x01)&&(receive_data[1]==0x01)&&(receive_data[1]==0x01))
			// {
			// 	//cout<<"command right ！"<<endl;
			// }
			if((receive_data[0]==0x00)&&(receive_data[1]==0x00)&&(receive_data[1]==0x00))
			{
				my_serial.write(publish_data,10);
				//cout<<"command send again !"<<endl;
			}
		}
		ros::spinOnce();  //callback函数必须处理所有问题时，才可以用到
	}
	return 0;
}
