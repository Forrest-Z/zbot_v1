#include <ros/ros.h>  
#include <test_p/car_msg.h>
#include <geometry_msgs/Twist.h>
#include <math.h>

float ratio = 1000 ;   //转速转换比例，执行速度调整比例
float D = 272.5 ;    //两轮间距，单位是mm
char const_num=2;
float pi=3.14159;
float const_=180;
float linear_temp=0,angular_temp=0;
float limit_rot_speed=0;
float limit_vel_speed=0;

class vel_transform
{  
public:  
  vel_transform()
  {  
    //Topic you want to publish  
    pub_ = n_.advertise<test_p::car_msg>("car_vel", 20); //缓存大一些，防止处理速度不够导致数据丢失
  
    //Topic you want to subscribe  
    sub_ = n_.subscribe("smoother_cmd_vel", 20, &vel_transform::callback, this);
  }  
  
  void callback(const geometry_msgs::Twist & cmd_input)
  {
      test_p::car_msg output;
            // if(cmd_input.angular.z>0.15)            //limit  rotation speed 
            //   {
            //           limit_rot_speed=0.15;
            //   }
            //   else if(cmd_input.angular.z<-0.15)
            //   {
            //           limit_rot_speed=-0.15;
            //   }
            //   else
            //   {
                      limit_rot_speed=cmd_input.angular.z;
              // }
              // if(cmd_input.linear.x>0.07)
              // {
              //         limit_vel_speed=0.07;
              // }
              // else
              // {
                      limit_vel_speed=cmd_input.linear.x;
              // }
    		linear_temp=ratio*limit_vel_speed;
    		angular_temp=(limit_rot_speed*const_)/pi;
    		if ( ( (int)linear_temp) != 0 )     //转换前进速度到两轮速度,单位是mm/s
   		{
    			output.leftspeed = linear_temp ;
    			output.rightspeed = linear_temp ;
    		}
    		else if(((int)angular_temp)!=0)      //转换转动速度到两轮速度，单位是mm/s
   		{
    			output.leftspeed= - angular_temp* D/ (10*const_num) ;
    			output.rightspeed= angular_temp* D / (10*const_num)  ;
    		}
    		else                                                             //停止时两轮速度均为0
    		{
    			output.leftspeed = 0 ;
    			output.rightspeed = 0;
    		}

    //写入/cmd_vel发出的消息转换成左右轮速度的过程
    pub_.publish(output);
  }
  
private:

  ros::NodeHandle n_;   
  ros::Publisher pub_;  
  ros::Subscriber sub_;  
  
};//End of class vel_transform
  
int main(int argc, char **argv)  
{

  //Initiate ROS  
  ros::init(argc, argv, "vel_transfrom");  
  
  //Create an object of class SubscribeAndPublish that will take care of everything  
  vel_transform go_on;  //main 函数中
  
  ros::spin();  
  
  return 0;  
}
