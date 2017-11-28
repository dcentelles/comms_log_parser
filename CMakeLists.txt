cmake_minimum_required(VERSION 2.8.3)
project(comms_log_parser CXX)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  std_srvs

)

catkin_package(
   CATKIN_DEPENDS roscpp rospy std_msgs std_srvs
)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

SET(CMAKE_AUTOUIC ON) 

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5PrintSupport)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_definitions(-std=c++11)

set(${PROJECT_NAME}_LIB ${PROJECT_NAME}_lib)

# Tell CMake to create the ${PROJECT_NAME} executable

add_library(${${PROJECT_NAME}_LIB}
	mainwindow.cpp
	dataregister.cpp
        dataplotwindow.cpp
        qcustomplot.cpp
        normalplot.cpp
	)


set(QT_DEPS 
	Qt5::Widgets
        Qt5::Core
        Qt5::PrintSupport
	)

target_link_libraries(${${PROJECT_NAME}_LIB}  ${QT_DEPS}  ${catkin_LIBRARIES})

add_executable(${PROJECT_NAME} 
	main.cpp
	)

target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIB})

get_target_property(QtCore_location Qt5::Core LOCATION)
message(STATUS "Qt5CoreLocation: " ${QtCore_location})
