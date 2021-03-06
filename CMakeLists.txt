cmake_minimum_required(VERSION 3.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

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

find_package(Python3 MODULE COMPONENTS Interpreter Development)
find_package(PyQt5)
find_package(SIP)
if(POLICY CMP0071)
  cmake_policy(SET CMP0071 OLD) # use MACOSX_RPATH
endif()
if (Python3_FOUND AND PYQT5_FOUND AND SIP_FOUND)
  include(SIPMacros)
  include_directories(${SIP_INCLUDE_DIR})
  set(SIP_INCLUDES ${PYQT5_SIP_DIR})
  set(SIP_EXTRA_OPTIONS ${PYQT5_SIP_FLAGS})
  separate_arguments(PYQT5_SIP_FLAGS)
  add_subdirectory(qtpvwidgets_py)
else()
  message(WARNING "Python module will not be built: SIP is missing.")
endif()
