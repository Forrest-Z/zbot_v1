cmake_minimum_required(VERSION 2.8.3)
project(keyboard_teleop_zbot)

find_package(catkin REQUIRED)

catkin_package()

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
catkin_install_python(PROGRAMS
   keyboard_teleop_zbot.py
   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

