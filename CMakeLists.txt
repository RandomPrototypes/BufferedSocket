cmake_minimum_required(VERSION 3.16)

project(BufferedSocket VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)

set(LIB_SRC
	src/BufferedSocket.cpp
	src/DataPacket.cpp
)

set(LIB_INCLUDE
	include/BufferedSocket/BufferedSocket.h
	include/BufferedSocket/DataPacket.h
	include/BufferedSocket/BufferedSocketDefs.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(BufferedSocket SHARED ${LIB_SRC} ${LIB_INCLUDE})
target_compile_definitions(BufferedSocket PRIVATE "BUFFEREDSOCKET_EXPORTS")
set_target_properties(BufferedSocket PROPERTIES VERSION ${PROJECT_VERSION})

if(WIN32)
	target_link_libraries(BufferedSocket PRIVATE ws2_32)
endif()

install(TARGETS BufferedSocket  EXPORT bufferedsocket-config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION . PATTERN "*.h")	
install(EXPORT bufferedsocket-config DESTINATION lib/cmake/BufferedSocket)
export(TARGETS BufferedSocket FILE bufferedsocket-config.cmake)
