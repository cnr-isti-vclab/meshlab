# Try to find the radium engine base folder
# Will define
# MESHLAB_SRC_DIR : the root of the Meshlab project
# MESHLAB_PLUGINS_SOURCE_DIR     : directory where meshlab plugins sources are
# MESHLAB_PROJECT_FILE           : path of the meshlab_full.pro file
# MESHLAB_EXTERNAL_DIR           : root dir of meshlab externals
# MESHLAB_EXTERNAL_INCLUDE_DIR   : path where external include files are installed
# MESHLAB_EXTERNAL_LIBRARIES_DIR : path where external libraries are installed


# MESHLAB_FOUND if found
if( NOT MESHLAB_SRC_DIR )
  find_path( MESHLAB_SRC_DIR NAMES meshlab_full.pro
    PATHS
    "${CMAKE_SOURCE_DIR}/../meshlab"
    "${CMAKE_SOURCE_DIR}/../Meshlab"
    "${CMAKE_SOURCE_DIR}/.."
    "${CMAKE_SOURCE_DIR}/../.."
    "${CMAKE_SOURCE_DIR}/../../.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../meshlab"
    "${CMAKE_CURRENT_SOURCE_DIR}/../Meshlab"
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../../.."
    PATH_SUFFIXES src meshlab/src
    DOC "The Meshlab src folder")
endif( NOT MESHLAB_SRC_DIR )

if ( MESHLAB_SRC_DIR )
  set ( MESHLAB_RELATIVE_PLUGINS_SOURCE_DIR "meshlabplugins" )
  set ( MESHLAB_PLUGINS_SOURCE_DIR          "${MESHLAB_SRC_DIR}/${MESHLAB_RELATIVE_PLUGINS_SOURCE_DIR}" )
  set ( MESHLAB_PROJECT_FILE                "${MESHLAB_SRC_DIR}/meshlab_full.pro" )
  set ( MESHLAB_EXTERNAL_DIR                "${MESHLAB_SRC_DIR}/external" )


  # Meshlab put include files and libraries in the following plateform dependent
  # folders:
  #  - includes:  linux-g++, macx, macx64, win32-gcc, win32-msvc2008
  #  - libraries: linux-g++, linux-g++-64, macx64 win32-gcc, win32-msvc2008, win32-msvc2015
  set (INC_PLATEFORM "linux-g++" )
  set (LIB_PLATEFORM "linux-g++" )

  message(AUTHOR_WARNING "Automatic plateform detection not ready yet. Using ${INC_PLATEFORM}")


  set ( MESHLAB_EXTERNAL_LIBRARIES_DIR "${MESHLAB_EXTERNAL_DIR}/lib/${LIB_PLATEFORM}" )
  set ( MESHLAB_EXTERNAL_INCLUDE_DIR   "${MESHLAB_EXTERNAL_DIR}/inc/${INC_PLATEFORM}" )

  unset ( INC_PLATEFORM )
  unset ( LIB_PLATEFORM )

  set ( MESHLAB_FOUND TRUE )

endif( MESHLAB_SRC_DIR )

if ( MESHLAB_FOUND )
  if( NOT MESHLAB_FIND_QUIETLY )
    message ( STATUS "Found Meshlab: ${MESHLAB_SRC_DIR}")
  endif( NOT MESHLAB_FIND_QUIETLY )
else( MESHLAB_FOUND )
  if( MESHLAB_FIND_REQUIRED )
    message( FATAL_ERROR "Could not find Meshlab src dir")
  endif( MESHLAB_FIND_REQUIRED )
endif( MESHLAB_FOUND )
