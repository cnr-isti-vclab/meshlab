find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)

add_library(edge_color_test_ply STATIC ../src/vcglib/wrap/ply/plylib.cpp)
target_link_libraries(edge_color_test_ply PUBLIC meshlab-common)

foreach(test_name edge_color_io edge_color_policy edge_color_render edge_color_ui)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE edge_color_test_ply)
    target_compile_definitions(${test_name} PRIVATE
        EDGE_COLOR_FIXTURE="${CMAKE_CURRENT_SOURCE_DIR}/fixtures/colored_edges.ply")
    set_target_properties(${test_name} PROPERTIES CXX_STANDARD 14)
    if(APPLE)
        target_compile_definitions(${test_name} PRIVATE GL_SILENCE_DEPRECATION)
    endif()
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# Rendering requires a desktop compatibility OpenGL context (Xvfb/Mesa works on Linux).
set_tests_properties(edge_color_render PROPERTIES SKIP_RETURN_CODE 77)

target_sources(edge_color_ui PRIVATE
    ../src/meshlab/additionalgui.cpp
    ../src/meshlab/additionalgui.h
    ../src/meshlab/ml_rendering_actions.cpp
    ../src/meshlab/ml_rendering_actions.h
    ../src/meshlab/ml_render_gui.cpp
    ../src/meshlab/ml_render_gui.h
    ../src/meshlab/dialogs/save_mesh_attributes_dialog.cpp
    ../src/meshlab/dialogs/save_mesh_attributes_dialog.h
    ../src/meshlab/dialogs/save_mesh_attributes_dialog.ui)
set_target_properties(edge_color_ui PROPERTIES AUTOMOC ON AUTOUIC ON)
target_include_directories(edge_color_ui PRIVATE ../src/meshlab)
target_link_libraries(edge_color_ui PRIVATE meshlab-common-gui)
set_tests_properties(edge_color_ui PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
if(TARGET io_base)
    add_dependencies(edge_color_ui io_base)
    target_compile_definitions(edge_color_ui PRIVATE EDGE_COLOR_IO_PLUGIN="$<TARGET_FILE:io_base>")
endif()
