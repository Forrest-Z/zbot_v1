#include <ros/ros.h>  
#include <test_p/car_msg.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <math.h>
#include <test_p/odometry_data.h>
#include <nav_msgs/Odometry.h>
#include <iostream>

using namespace std;

float angle=0;      
float x_value=0;
float y_value=0;
float x_speed=0;
float th_speed=0;


  void callback(const test_p::odometry_data& odometry_input)
  {
      x_value=odometry_input.x;
      y_value=odometry_input.y;
      angle=odometry_input.th;
      x_speed=odometry_input.vel_x;
      th_speed=odometry_input.vel_th;
  }

int main(int argc, char **argv)  
{
      ros::init(argc, argv, "odometry_node");  
      ros::NodeHandle n_;   
      ros::Publisher odom_pub= n_.advertise<nav_msgs::Odometry>("odom", 20);  
      ros::Subscriber sub_=n_.subscribe("odom_data", 20, callback);  
      static tf::TransformBroadcaster odom_broadcaster;
      geometry_msgs::TransformStamped odom_trans;
      nav_msgs::Odometry odom;
      geometry_msgs::Quaternion odom_quat; 
      float covariance[36] =  {0.01, 0, 0, 0, 0, 0,  // covariance on gps_x
                                              0, 0.01, 0, 0, 0, 0,  // covariance on gps_y
                                              0, 0, 99999, 0, 0, 0,  // covariance on gps_z
                                              0, 0, 0, 99999, 0, 0,  // large covariance on rot x
                                              0, 0, 0, 0, 99999, 0,  // large covariance on rot y
                                              0, 0, 0, 0, 0, 0.01};  // large covariance on rot z 
      for(int i = 0; i < 36; i++)
      {
        odom.pose.covariance[i] = covariance[i];;
      }       

      ros::Rate loop_rate(10);
      while(ros::ok()) 
      {
   

              odom_quat = tf::createQuaternionMsgFromYaw(angle);
              //cout<<angle<<endl;
     
              odom_trans.header.stamp = ros::Time::now();
              odom_trans.header.frame_id = "odom";      //发布坐标变换的父子坐标系
              odom_trans.child_frame_id = "base_footprint";       

              odom_trans.transform.translation.x =x_value;// position_x.d;
              odom_trans.transform.translation.y = y_value;//;
              odom_trans.transform.translation.z = 0.0;
              odom_trans.transform.rotation = odom_quat;        

              odom_broadcaster.sendTransform(odom_trans);
              
              odom.header.stamp = ros::Time::now();     
              odom.header.frame_id = "odom";
              odom.child_frame_id = "base_footprint";       

              odom.pose.pose.position.x = x_value;//;        //里程计位置数据
              odom.pose.pose.position.y = y_value;//;
              odom.pose.pose.position.z = 0.0;
              odom.pose.pose.orientation = odom_quat;       

              odom.twist.twist.linear.x = x_speed;
              //odom.twist.twist.linear.y = odom_vy;
              odom.twist.twist.angular.z = th_speed;       

              odom_pub.publish(odom);

              ros::spinOnce();
              loop_rate.sleep();
      }
      //ros::spin();  
  
      return 0;  
}
