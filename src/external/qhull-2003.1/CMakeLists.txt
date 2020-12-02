if(ALLOW_SYSTEM_LIBS AND TARGET Qhull::libqhull)
	message(STATUS "- qhull - using system-provided library")
	add_library(external-qhull INTERFACE)
	target_link_libraries(external-qhull INTERFACE Qhull::libqhull)
	target_compile_definitions(external-qhull INTERFACE SYSTEM_QHULL)
	target_include_directories(external-qhull INTERFACE ${QHULL_libqhull_INCLUDE_DIR}/libqhull)
elseif(ALLOW_BUNDLED_LIBS)
	message(STATUS "- qhull - using bundled source")
	add_library(
		external-qhull STATIC
		"${QHULL_DIR}/src/geom2.c"
		"${QHULL_DIR}/src/global.c"
		"${QHULL_DIR}/src/io.c"
		"${QHULL_DIR}/src/io.h"
		"${QHULL_DIR}/src/mem.c"
		"${QHULL_DIR}/src/mem.h"
		"${QHULL_DIR}/src/merge.c"
		"${QHULL_DIR}/src/merge.h"
		"${QHULL_DIR}/src/poly.c"
		"${QHULL_DIR}/src/poly.h"
		"${QHULL_DIR}/src/poly2.c"
		"${QHULL_DIR}/src/qconvex.c"
		"${QHULL_DIR}/src/qhull.c"
		"${QHULL_DIR}/src/qhull.h"
		"${QHULL_DIR}/src/qset.c"
		"${QHULL_DIR}/src/qset.h"
		"${QHULL_DIR}/src/stat.c"
		"${QHULL_DIR}/src/stat.h"
		"${QHULL_DIR}/src/geom.c"
		"${QHULL_DIR}/src/geom.h"
		"${QHULL_DIR}/src/user.c"
		"${QHULL_DIR}/src/user.h")
	target_include_directories(external-qhull INTERFACE "${QHULL_DIR}/src")
	set_property(TARGET external-qhull PROPERTY FOLDER External)
	target_compile_options(external-qhull PRIVATE -w)
endif()
