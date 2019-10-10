cmake_minimum_required(VERSION 3.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)


project(qepicspv)
set(VERSION 1.0)


find_package(EPICS COMPONENTS ca REQUIRED)
include_directories(${EPICS_INCLUDE_DIRS})
link_directories(${EPICS_LIB_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})


add_subdirectory(qtpv)
add_subdirectory(qtpvwidgets)

find_package(Python3 COMPONENTS Interpreter Development)
find_package(PyQt5)
find_package(SIP)
cmake_policy(SET CMP0071 OLD)
if (Python3_FOUND AND PYQT5_FOUND AND (SIP_EXECUTABLE OR SIP_INCLUDE_DIRS) )
  include_directories(${SIP_INCLUDE_DIRS})
  add_subdirectory(qtpvwidgets_py)
else()
  message(WARNING "Python module will not be built: SIP is missing.")
endif()
