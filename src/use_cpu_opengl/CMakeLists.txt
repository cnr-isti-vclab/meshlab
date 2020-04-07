if(WIN32)

set(SOURCES
	main.cpp
)

set(RESOURCES 
	res.qrc
)

add_executable(UseCPUOpenGL ${SOURCES} ${RESOURCES})

target_link_libraries(
        UseCPUOpenGL
        PUBLIC Qt5::Core
               Qt5::Widgets
)

set_target_properties(UseCPUOpenGL PROPERTIES LINK_FLAGS "/MANIFESTUAC:level=\'requireAdministrator\'")

install(TARGETS UseCPUOpenGL DESTINATION ${MESHLAB_BIN_INSTALL_DIR})

endif()
