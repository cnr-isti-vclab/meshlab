cmake_minimum_required (VERSION 2.6)

project(CoMISo)

# add our macro directory to cmake search path
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

include (ACGCommon)

acg_qt4 ()
# change to 0 if QT should not be used
set( WANT_COMISO_QT 1 )
if( QT4_FOUND)
  #message( WARNING " QT4 FOUND" )
  if( WANT_COMISO_QT )
    add_definitions (-DQT4_FOUND)
  #  message( WARNING " USING QT4" )
  endif ()
  set (COMISO_QT4_CONFIG_FILE_SETTINGS "#define COMISO_QT4_AVAILABLE 1" )
else()
  set (COMISO_QT4_CONFIG_FILE_SETTINGS "#define COMISO_QT4_AVAILABLE 0" )
endif ()

acg_get_version ()

include (ACGOutput)

set(COMISO_INCLUDE_DIRECTORIES "")
set(COMISO_LINK_DIRECTORIES "")
set(COMISO_LINK_LIBRARIES "")

FIND_PACKAGE( Boost 1.42.0 COMPONENTS system filesystem regex )
if(Boost_FOUND)
  set (COMISO_BOOST_CONFIG_FILE_SETTINGS "#define COMISO_BOOST_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS} )
#  list( APPEND COMISO_LINK_DIRECTORIES    ${Boost_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${Boost_LIBRARIES} )
else()
  set (COMISO_BOOST_CONFIG_FILE_SETTINGS "#define COMISO_BOOST_AVAILABLE 0" )
  message (STATUS "Boost not found!")
endif ()


find_package (GMM)
if (GMM_FOUND)
  set (COMISO_GMM_CONFIG_FILE_SETTINGS "#define COMISO_GMM_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${GMM_INCLUDE_DIR} )
else()
  set (COMISO_GMM_CONFIG_FILE_SETTINGS "#define COMISO_GMM_AVAILABLE 0" )
  message (FATAL_ERROR "GMM not found!")
endif ()

# We require cgal with its blas on windows
  find_package(CGAL)
if (CGAL_FOUND)
  set (COMISO_CGAL_CONFIG_FILE_SETTINGS "#define COMISO_CGAL_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${CGAL_INCLUDE_DIR} )
  list( APPEND COMISO_LINK_DIRECTORIES   ${CGAL_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${CGAL_LIBRARIES} )
else()
  set (COMISO_CGAL_CONFIG_FILE_SETTINGS "#define COMISO_CGAL_AVAILABLE 0" )
  message (STATUS "CGAL not found!")
endif()


find_package (BLAS)
if (BLAS_FOUND )
  set (COMISO_BLAS_CONFIG_FILE_SETTINGS "#define COMISO_BLAS_AVAILABLE 1" )
  
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${BLAS_INCLUDE_DIRS} )
  list( APPEND COMISO_LINK_DIRECTORIES   ${BLAS_LIBRARY_DIRS} )
  list( APPEND COMISO_LINK_LIBRARIES  ${BLAS_LIBRARIES} )
else()
  set (COMISO_BLAS_CONFIG_FILE_SETTINGS "#define COMISO_BLAS_AVAILABLE 0" )
  message (STATUS "BLAS not found!")
endif ()

find_package (ADOLC)
if (ADOLC_FOUND)
  set (COMISO_ADOLC_CONFIG_FILE_SETTINGS "#define COMISO_ADOLC_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${ADOLC_INCLUDE_DIR} )
  list( APPEND COMISO_LINK_DIRECTORIES   ${ADOLC_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${ADOLC_LIBRARIES} )
else ()
  set (COMISO_ADOLC_CONFIG_FILE_SETTINGS "#define COMISO_ADOLC_AVAILABLE 0" )
  message (STATUS "ADOLC not found!")
endif ()

find_package (SUITESPARSE)
if (SUITESPARSE_FOUND )
  set (COMISO_SUITESPARSE_CONFIG_FILE_SETTINGS "#define COMISO_SUITESPARSE_AVAILABLE 1" )
  
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${SUITESPARSE_INCLUDE_DIRS} )
  list( APPEND COMISO_LINK_DIRECTORIES   ${SUITESPARSE_LIBRARY_DIRS} )
  list( APPEND COMISO_LINK_LIBRARIES  ${SUITESPARSE_LIBRARIES} )
else ()
  message (STATUS "SUITESPARSE not found!")
  set (COMISO_SUITESPARSE_CONFIG_FILE_SETTINGS "#define COMISO_SUITESPARSE_AVAILABLE 0" )
endif ()
# special handling, since spqr is incorrect in several distributions
if(SUITESPARSE_SPQR_VALID)
  set (COMISO_SUITESPARSE_SPQR_CONFIG_FILE_SETTINGS "#define COMISO_SUITESPARSE_SPQR_AVAILABLE 1" )
else()
  message (STATUS "SUITESPARSE SPQR seems to be invalid!")
  set (COMISO_SUITESPARSE_SPQR_CONFIG_FILE_SETTINGS "#define COMISO_SUITESPARSE_SPQR_AVAILABLE 0" )
endif()

find_package (MPI)
if (MPI_FOUND )
  set (COMISO_MPI_CONFIG_FILE_SETTINGS "#define COMISO_MPI_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${MPI_INCLUDE_PATH} )
  list( APPEND COMISO_LINK_LIBRARIES  ${MPI_CXX_LIBRARIES} )
else ()
  message (STATUS "MPI not found!")
  set (COMISO_MPI_CONFIG_FILE_SETTINGS "#define COMISO_MPI_AVAILABLE 0" )
endif ()

find_package (PETSC)
if (PETSC_FOUND AND MPI_FOUND)
  set (COMISO_PETSC_CONFIG_FILE_SETTINGS "#define COMISO_PETSC_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${PETSC_INCLUDE_DIRS} )
  list( APPEND COMISO_LINK_DIRECTORIES ${PETSC_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${PETSC_LIBRARY} )
else ()
  message (STATUS "PETSC or dependency not found!")
  set (COMISO_PETSC_CONFIG_FILE_SETTINGS "#define COMISO_PETSC_AVAILABLE 0" )
endif ()


find_package (TAO)
if (TAO_FOUND AND PETSC_FOUND AND MPI_FOUND)
  set (COMISO_TAO_CONFIG_FILE_SETTINGS "#define COMISO_TAO_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${TAO_INCLUDE_DIRS} )
  list( APPEND COMISO_LINK_DIRECTORIES ${TAO_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${TAO_LIBRARY} )
else ()
  message (STATUS "TAO or dependency not found!")
  set (COMISO_TAO_CONFIG_FILE_SETTINGS "#define COMISO_TAO_AVAILABLE 0" )
endif ()

find_package (METIS)
if (METIS_FOUND )
  set (COMISO_METIS_CONFIG_FILE_SETTINGS "#define COMISO_METIS_AVAILABLE 1" )
  
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${METIS_INCLUDE_DIRS} )
  list( APPEND COMISO_LINK_DIRECTORIES   ${METIS_LIBRARY_DIRS} )
  list( APPEND COMISO_LINK_LIBRARIES  ${METIS_LIBRARIES} )
else()
  set (COMISO_METIS_CONFIG_FILE_SETTINGS "#define COMISO_METIS_AVAILABLE 0" )
  message (STATUS "METIS not found!")
endif ()

find_package (MUMPS)
if (MUMPS_FOUND )
  set (COMISO_MUMPS_CONFIG_FILE_SETTINGS "#define COMISO_MUMPS_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${MUMPS_INCLUDE_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES ${MUMPS_LIBRARY} )
else ()
  message (STATUS "MUMPS not found!")
  set (COMISO_MUMPS_CONFIG_FILE_SETTINGS "#define COMISO_MUMPS_AVAILABLE 0" )
endif ()

find_package (IPOPT)
if (IPOPT_FOUND)
  set (COMISO_IPOPT_CONFIG_FILE_SETTINGS "#define COMISO_IPOPT_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${IPOPT_INCLUDE_DIR} )
  list( APPEND COMISO_LINK_DIRECTORIES ${IPOPT_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES ${IPOPT_LIBRARY} )
  if ( IPOPT_HSL_LIBRARY_DIR )
	set (COMISO_HSL_CONFIG_FILE_SETTINGS "#define COMISO_HSL_AVAILABLE 1" )
  else ()
	set (COMISO_HSL_CONFIG_FILE_SETTINGS "#define COMISO_HSL_AVAILABLE 0" )
  endif()
else ()
  message (STATUS "IPOPT or dependency not found!")
  set (COMISO_IPOPT_CONFIG_FILE_SETTINGS "#define COMISO_IPOPT_AVAILABLE 0" )
  set (COMISO_HSL_CONFIG_FILE_SETTINGS "#define COMISO_HSL_AVAILABLE 0" )
endif ()

find_package (EIGEN3)
if (EIGEN3_FOUND )
  set (COMISO_EIGEN3_CONFIG_FILE_SETTINGS "#define COMISO_EIGEN3_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${EIGEN3_INCLUDE_DIR} )
else ()
  message (STATUS "EIGEN3 not found!")
  set (COMISO_EIGEN3_CONFIG_FILE_SETTINGS "#define COMISO_EIGEN3_AVAILABLE 0" )
endif ()

find_package (Taucs)
if (TAUCS_FOUND )
  set (COMISO_TAUCS_CONFIG_FILE_SETTINGS "#define COMISO_TAUCS_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${TAUCS_INCLUDE_DIR} )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${LAPACK_INCLUDE_DIR} )
  list( APPEND COMISO_LINK_DIRECTORIES    ${LAPACK_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${TAUCS_LIBRARY} )
  list( APPEND COMISO_LINK_LIBRARIES  ${LAPACK_LIBRARIES} )
else ()
  message (STATUS "TAUCS not found!")
  set (COMISO_TAUCS_CONFIG_FILE_SETTINGS "#define COMISO_TAUCS_AVAILABLE 0" )
endif ()

find_package (GUROBI)
if (GUROBI_FOUND )
  set (COMISO_GUROBI_CONFIG_FILE_SETTINGS "#define COMISO_GUROBI_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${GUROBI_INCLUDE_DIRS} )
#  list( APPEND COMISO_LINK_DIRECTORIES    ${GUROBI_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${GUROBI_LIBRARIES} )
else ()
  message (STATUS "GUROBI not found!")
  set (COMISO_GUROBI_CONFIG_FILE_SETTINGS "#define COMISO_GUROBI_AVAILABLE 0" )
endif ()

find_package (ARPACK)
if (ARPACK_FOUND )
  set (COMISO_ARPACK_CONFIG_FILE_SETTINGS "#define COMISO_ARPACK_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${ARPACK_INCLUDE_DIR} )
#  list( APPEND COMISO_LINK_DIRECTORIES    ${ARPACK_LIBRARY_DIR} )
  list( APPEND COMISO_LINK_LIBRARIES  ${ARPACK_LIBRARY} )
else ()
  message (STATUS "ARPACK not found!")
  set (COMISO_ARPACK_CONFIG_FILE_SETTINGS "#define COMISO_ARPACK_AVAILABLE 0" )
endif ()

find_package (CPLEX)
if (CPLEX_FOUND )
  set (COMISO_CPLEX_CONFIG_FILE_SETTINGS "#define COMISO_CPLEX_AVAILABLE 1" )
  list( APPEND COMISO_INCLUDE_DIRECTORIES ${CPLEX_INCLUDE_DIRS} )
  list( APPEND COMISO_LINK_LIBRARIES  ${CPLEX_LIBRARIES} )
  
  #enable c++ support
   add_definitions(-DIL_STD)
else ()
  message (STATUS "CPLEX not found!")
  set (COMISO_CPLEX_CONFIG_FILE_SETTINGS "#define COMISO_CPLEX_AVAILABLE 0" )
endif ()

include_directories (
  ..
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../
  ${CMAKE_CURRENT_BINARY_DIR}
  ${COMISO_INCLUDE_DIRECTORIES}
)

# generate dllexport macros on windows
if (WIN32)
  add_definitions(-DCOMISODLL)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
endif ()


link_directories (
  ${COMISO_LINK_DIRECTORIES}
)

# source code directories
set (directories 
  .
  Solver
  NSolver
  EigenSolver
  Config
  Utils
  QtWidgets
)

if (WIN32)
  add_definitions(
      -D_USE_MATH_DEFINES -DNOMINMAX
  )
endif ()

# collect all header,source and ui files
acg_append_files (headers "*.hh" ${directories})
acg_append_files (sources "*.cc" ${directories})
acg_append_files (ui "*.ui" ${directories})


macro (of_list_filter _list)
  if (WIN32)
   foreach (_element ${${_list}})
     if (_element MATCHES "gnuplot_i\\.(cc|hh)$")
       list (REMOVE_ITEM ${_list} ${_element})
     endif ()
   endforeach ()
  endif ()
endmacro ()

of_list_filter  ( headers )
of_list_filter  ( sources )


# remove template cc files from source file list
acg_drop_templates (sources)

if( QT4_FOUND)
# genereate uic and moc targets
acg_qt4_autouic (uic_targets ${ui})
acg_qt4_automoc (moc_targets ${headers})
endif()
acg_add_library (CoMISo SHARED ${uic_targets} ${sources} ${headers} ${moc_targets})

if (NOT APPLE)
	target_link_libraries (CoMISo 
	  ${QT_LIBRARIES}
	  ${COMISO_LINK_LIBRARIES}
	)
else(NOT APPLE)
	target_link_libraries (CoMISo
	  ${QT_LIBRARIES}
	  ${COMISO_LINK_LIBRARIES}
	)
endif(NOT APPLE)

# display results
acg_print_configure_header (COMISO "CoMISo")

# write config file
configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/Config/config.hh.in"
     "${CMAKE_CURRENT_SOURCE_DIR}/Config/config.hh" @ONLY IMMEDIATE)
     
     
     
#######################################################################
# Configure the examples last to be sure, that all configure files
# of the library are already there
#######################################################################

if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/factored_solver/CMakeLists.txt" )
	add_subdirectory (Examples/factored_solver)
endif()

if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/quadratic_solver/CMakeLists.txt" )
	add_subdirectory (Examples/quadratic_solver)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/test2/CMakeLists.txt" )
	add_subdirectory (Examples/test2)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_quadratic_example/CMakeLists.txt" )
	add_subdirectory (Examples/small_quadratic_example)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_factored_example/CMakeLists.txt" )
	add_subdirectory (Examples/small_factored_example)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/super_sparse_matrix/CMakeLists.txt" )
	add_subdirectory (Examples/super_sparse_matrix)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/eigen_solver/CMakeLists.txt" )
	add_subdirectory (Examples/eigen_solver)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_nsolver/CMakeLists.txt" )
	add_subdirectory (Examples/small_nsolver)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_eigenproblem/CMakeLists.txt" )
	add_subdirectory (Examples/small_eigenproblem)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_miqp/CMakeLists.txt" )
	add_subdirectory (Examples/small_miqp)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_nleast_squares/CMakeLists.txt" )
	add_subdirectory (Examples/small_nleast_squares)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_sparseqr/CMakeLists.txt" )
	add_subdirectory (Examples/small_sparseqr)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_quadratic_resolve_example/CMakeLists.txt" )
	add_subdirectory (Examples/small_quadratic_resolve_example)
endif()
if( EXISTS "${CMAKE_SOURCE_DIR}/Examples/small_cplex_soc/CMakeLists.txt" )
	add_subdirectory (Examples/small_cplex_soc)
endif()
