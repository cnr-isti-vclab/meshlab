######################################################################################
# ActionScript socket server builder
######################################################################################

add_executable(as3-server server/as3-server.c)
if(APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "-framework CoreFoundation -framework IOKit")
endif()
target_link_libraries(as3-server freenect m)
