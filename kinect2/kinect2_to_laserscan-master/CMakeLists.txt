cmake_minimum_required(VERSION 2.8.3)
project(kinect2_to_laserscan)

find_package(catkin REQUIRED COMPONENTS
  message_filters
  nodelet
  roscpp
  sensor_msgs
  tf2
  tf2_ros
  tf2_sensor_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES kinect2_to_laserscan
  CATKIN_DEPENDS roscpp message_filters nodelet sensor_msgs tf2 tf2_ros
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(kinect2_to_laserscan src/kinect2_to_laserscan_nodelet.cpp)
target_link_libraries(kinect2_to_laserscan ${catkin_LIBRARIES})

add_executable(kinect2_to_laserscan_node src/kinect2_to_laserscan_node.cpp)
target_link_libraries(kinect2_to_laserscan_node kinect2_to_laserscan ${catkin_LIBRARIES})

install(TARGETS kinect2_to_laserscan kinect2_to_laserscan_node
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)
install(FILES nodelets.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)
