message(STATUS "- levmar - using bundled source")
add_library(
	external-levmar STATIC
	"${LEVMAR_DIR}/compiler.h"
	"${LEVMAR_DIR}/lm.h"
	"${LEVMAR_DIR}/misc.h"
	"${LEVMAR_DIR}/Axb.c"
	"${LEVMAR_DIR}/lm.c"
	"${LEVMAR_DIR}/lmbc.c"
	"${LEVMAR_DIR}/lmblec.c"
	"${LEVMAR_DIR}/lmlec.c"
	"${LEVMAR_DIR}/misc.c")
target_include_directories(external-levmar PUBLIC ${LEVMAR_DIR})
set_property(TARGET external-levmar PROPERTY FOLDER External)
