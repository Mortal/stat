cmake_minimum_required(VERSION 2.6)

project(stat)

if(NOT WIN32) 
	add_definitions("-Wall -Wextra -std=c++0x")
endif(NOT WIN32)
if(NOT WIN32) 
	add_definitions("-DASCII")
endif(NOT WIN32)

if(WIN32)
	set(Boost_USE_STATIC_LIBS    ON)
endif(WIN32)
find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(stattest ALL stattest.cpp stat.cpp stat.h)
