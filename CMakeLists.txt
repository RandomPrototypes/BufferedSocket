cmake_minimum_required(VERSION 3.16)

project(BufferedSocket)

add_executable(BufferedSocket BufferedSocket.cpp BufferedSocket.h test/test.cpp)

if(WIN32)
	target_link_libraries(BufferedSocket PRIVATE ws2_32)
endif()
