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
#link_directories(${Boost_LIBRARY_DIRS})

# Ryan Pavlik's Git revision description helper
# http://stackoverflow.com/a/4318642
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/")
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_COMMIT)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/sysinfo.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/sysinfo.cpp" @ONLY)

add_executable(stattest
  stattest.cpp
  stat.cpp stat.h
  portability.cpp portability.h
  "${CMAKE_CURRENT_BINARY_DIR}/sysinfo.cpp" sysinfo.h)

#target_link_libraries(stattest ${Boost_LIBRARIES})
