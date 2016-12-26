#include <ros/ros.h>  
#include <test_p/car_msg.h>
#include <geometry_msgs/Twist.h>
#include <math.h>

float ratio = 1000.0f ;   //转速转换比例，执行速度调整比例
float D = 0.2680859f ;    //两轮间距，单位是mm
char const_num=2;
float pi=3.14159f;
float const_=180.0f;
float linear_temp=0,angular_temp=0;

class vel_transform
{  
public:  
  vel_transform()
  {  
    //Topic you want to publish  
    pub_ = n_.advertise<test_p::car_msg>("car_vel", 20); //缓存大一些，防止处理速度不够导致数据丢失
  
    //Topic you want to subscribe  
    sub_ = n_.subscribe("cmd_vel", 20, &vel_transform::callback, this);
  }  
  
  void callback(const geometry_msgs::Twist & cmd_input)
  {
      test_p::car_msg output;

             angular_temp = 0.5f*cmd_input.angular.z*D ;

             linear_temp =cmd_input.linear.x ;
    		// linear_temp = ratio*limit_vel_speed ;

    		output.leftspeed = linear_temp-angular_temp ;
    		output.rightspeed = linear_temp+angular_temp ;

              output.leftspeed*=ratio;
              output.rightspeed*=ratio;

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
