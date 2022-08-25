################################################################################
include(DownloadProject)

# With CMake 3.8 and above, we can hide warnings about git being in a
# detached head by passing an extra GIT_CONFIG option.
set(LIBIGL_EXTRA_OPTIONS TLS_VERIFY OFF)
if(NOT (${CMAKE_VERSION} VERSION_LESS "3.8.0"))
	list(APPEND LIBIGL_EXTRA_OPTIONS GIT_CONFIG advice.detachedHead=false)
endif()

# On CMake 3.6.3 and above, there is an option to use shallow clones of git repositories.
# The shallow clone option only works with real tags, not SHA1, so we use a separate option.
set(LIBIGL_BRANCH_OPTIONS)
if(NOT (${CMAKE_VERSION} VERSION_LESS "3.6.3"))
	# Disabled for now until we can make sure that it has no adverse effects
	# (Downside is that the eigen mirror is huge again)
	# list(APPEND LIBIGL_BRANCH_OPTIONS GIT_SHALLOW 1)
endif()

option(LIBIGL_SKIP_DOWNLOAD "Skip downloading external libraries" OFF)

# Shortcut functions
function(igl_download_project_aux name source)
	if(NOT LIBIGL_SKIP_DOWNLOAD)
		download_project(
			PROJ         ${name}
			SOURCE_DIR   "${source}"
			DOWNLOAD_DIR "${LIBIGL_EXTERNAL}/.cache/${name}"
			QUIET
			${LIBIGL_EXTRA_OPTIONS}
			${ARGN}
		)
	endif()
endfunction()

function(igl_download_project name)
	igl_download_project_aux(${name} "${LIBIGL_EXTERNAL}/${name}" ${ARGN})
endfunction()

################################################################################

## CGAL
function(igl_download_cgal)
	igl_download_project(cgal
		GIT_REPOSITORY https://github.com/CGAL/cgal.git
		GIT_TAG        f7c3c8212b56c0d6dae63787efc99093f4383415
	)
endfunction()

## CoMISo
function(igl_download_comiso)
	igl_download_project(CoMISo
		GIT_REPOSITORY https://github.com/libigl/CoMISo.git
		GIT_TAG        d60aa4759fba76b0b793b1efb090b7a771dd7c56
	)
endfunction()

## Cork
function(igl_download_cork)
	igl_download_project(cork
		GIT_REPOSITORY https://github.com/libigl/cork.git
		GIT_TAG        27ad8a285838f5a480d856429e39d3d56d4338f9
	)
endfunction()

## Eigen
set(LIBIGL_EIGEN_VERSION 3.3.7 CACHE STRING "Default version of Eigen used by libigl.")
function(igl_download_eigen)
	igl_download_project(eigen
		GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
		GIT_TAG        ${LIBIGL_EIGEN_VERSION}
		${LIBIGL_BRANCH_OPTIONS}
	)
endfunction()

## Embree
function(igl_download_embree)
	igl_download_project(embree
		GIT_REPOSITORY https://github.com/embree/embree.git
		GIT_TAG        v3.12.1
		${LIBIGL_BRANCH_OPTIONS}
	)
endfunction()

## glad
function(igl_download_glad)
	igl_download_project(glad
		GIT_REPOSITORY https://github.com/libigl/libigl-glad.git
		GIT_TAG        09b4969c56779f7ddf8e6176ec1873184aec890f
	)
endfunction()

## GLFW
function(igl_download_glfw)
	igl_download_project(glfw
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		GIT_TAG        3327050ca66ad34426a82c217c2d60ced61526b7
		${LIBIGL_BRANCH_OPTIONS}
	)
endfunction()

## ImGui
function(igl_download_imgui)
	igl_download_project(imgui
		GIT_REPOSITORY https://github.com/ocornut/imgui.git
		GIT_TAG        61b19489f1ba35934d9114c034b24eb5bff149e7 # 1.81 + patch for #1669
		${LIBIGL_BRANCH_OPTIONS}
	)
	igl_download_project(libigl-imgui
		GIT_REPOSITORY https://github.com/libigl/libigl-imgui.git
		GIT_TAG        7e1053e750b0f4c129b046f4e455243cb7f804f3
	)
endfunction()

## ImGuizmo
function(igl_download_imguizmo)
	igl_download_project(imguizmo
		GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
		GIT_TAG        a23567269f6617342bcc112394bdad937b54b2d7
		${LIBIGL_BRANCH_OPTIONS}
	)
endfunction()

## pybind11
function(igl_download_pybind11)
	igl_download_project(pybind11
		GIT_REPOSITORY https://github.com/pybind/pybind11.git
		GIT_TAG        2d0507db43cd5a117f7843e053b17dffca114107
	)
endfunction()

## stb_image
function(igl_download_stb)
	igl_download_project(stb
		GIT_REPOSITORY https://github.com/libigl/libigl-stb.git
		GIT_TAG        cd0fa3fcd90325c83be4d697b00214e029f94ca3
	)
endfunction()

## TetGen
function(igl_download_tetgen)
	igl_download_project(tetgen
		GIT_REPOSITORY https://github.com/jdumas/tetgen.git
		GIT_TAG        c63e7a6434652b8a2065c835bd9d6d298db1a0bc
	)
endfunction()

## TinyXML
function(igl_download_tinyxml2)
	igl_download_project(tinyxml2
		GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
		GIT_TAG        d175e9de0be0d4db75d0a8cf065599a435a87eb6
	)
endfunction()

## Triangle
function(igl_download_triangle)
	igl_download_project(triangle
		GIT_REPOSITORY https://github.com/libigl/triangle.git
		GIT_TAG        5a70326574b34d6a51d9eaf6a9f78813657ee108
	)
endfunction()

## Catch2
function(igl_download_catch2)
	igl_download_project(catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG        v2.11.0
	)
endfunction()

## Predicates
function(igl_download_predicates)
	igl_download_project(predicates
		GIT_REPOSITORY https://github.com/libigl/libigl-predicates.git
		GIT_TAG        488242fa2b1f98a9c5bd1441297fb4a99a6a9ae4
	)
endfunction()

################################################################################

## Test data
function(igl_download_test_data)
	igl_download_project_aux(test_data
		"${LIBIGL_EXTERNAL}/../tests/data"
		GIT_REPOSITORY https://github.com/libigl/libigl-tests-data
		GIT_TAG        19cedf96d70702d8b3a83eb27934780c542356fe
	)
endfunction()

## Tutorial data
function(igl_download_tutorial_data)
	igl_download_project_aux(tutorial_data
		"${LIBIGL_EXTERNAL}/../tutorial/data"
		GIT_REPOSITORY https://github.com/libigl/libigl-tutorial-data
		GIT_TAG        c1f9ede366d02e3531ecbaec5e3769312f31cccd
	)
endfunction()
