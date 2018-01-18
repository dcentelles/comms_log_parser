cmake_minimum_required(VERSION 2.8.3)
project(comms_log_parser CXX)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
#set(CMAKE_AUTOMOC ON)
#SET(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5PrintSupport)


SET(PROJECT_INCLUDE_DIRS
    src/include
    src/include/comms_log_parser
)

add_definitions(-std=c++11)


set(HW_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include/comms_log_parser)
set(HW_GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/gui)

#https://stackoverflow.com/questions/19761767/qt-5-cmake-fails-with-undefined-reference-to-vtable-on-hello-world-with-inc-sr
qt5_wrap_cpp(hmi_SRC
    ${HW_HEADER_DIR}/dataplotwindow.h
    ${HW_HEADER_DIR}/jitterplotwindow.h
    ${HW_HEADER_DIR}/mainwindow.h
    ${HW_HEADER_DIR}/normalplot.h
    ${HW_HEADER_DIR}/qcustomplot.h
    )
qt5_wrap_ui(hmi_UI
    ${HW_GUI_DIR}/dataplotwindow.ui
    ${HW_GUI_DIR}/jitterplotwindow.ui
    ${HW_GUI_DIR}/mainwindow.ui
    ${HW_GUI_DIR}/normalplot.ui
    )

set(${PROJECT_NAME}_LIB ${PROJECT_NAME}_lib)

# Tell CMake to create the ${PROJECT_NAME} executable

add_library(${${PROJECT_NAME}_LIB}
        src/mainwindow.cpp
        src/dataregister.cpp
        src/dataplotwindow.cpp
        src/jitterplotwindow.cpp
        src/qcustomplot.cpp
        src/normalplot.cpp
        ${hmi_SRC}
        ${hmi_UI}
	)


set(QT_DEPS 
	Qt5::Widgets
        Qt5::Core
        Qt5::PrintSupport
	)
message("DETECTED QT VERSION: "${Qt5Core_VERSION})
message("DETECTED QT LIBRARIES: "${Qt5Core_INCLUDE_DIRS})
target_include_directories(${${PROJECT_NAME}_LIB} PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_libraries(${${PROJECT_NAME}_LIB}  ${QT_DEPS} )

add_executable(${PROJECT_NAME} 
        src/main.cpp
	)

target_link_libraries(${PROJECT_NAME} ${${PROJECT_NAME}_LIB})

get_target_property(QtCore_location Qt5::Core LOCATION)
message(STATUS "Qt5CoreLocation: " ${QtCore_location})


