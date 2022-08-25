# Check the OS type.

# CMake does not distinguish Linux from other Unices.
STRING (REGEX MATCH "Linux" PROJECT_OS_LINUX ${CMAKE_SYSTEM_NAME})
# Nor *BSD
STRING (REGEX MATCH "BSD" PROJECT_OS_BSD ${CMAKE_SYSTEM_NAME})
# Or Solaris. I'm seeing a trend, here
STRING (REGEX MATCH "SunOS" PROJECT_OS_SOLARIS ${CMAKE_SYSTEM_NAME})

# Windows is easy (for once)
IF (WIN32)
    SET (PROJECT_OS_WIN TRUE BOOL INTERNAL)
ENDIF (WIN32)

# Check if it's an Apple OS
IF (APPLE)
    # Check if it's OS X or another MacOS (that's got to be pretty unlikely)
    STRING (REGEX MATCH "Darwin" PROJECT_OS_OSX ${CMAKE_SYSTEM_NAME})
    IF (NOT PROJECT_OS_OSX)
        SET (PROJECT_OS_MACOS TRUE BOOL INTERNAL)
    ENDIF (NOT PROJECT_OS_OSX)
ENDIF (APPLE)

# QNX
IF (QNXNTO)
    SET (PROJECT_OS_QNX TRUE BOOL INTERNAL)
ENDIF (QNXNTO)

IF (PROJECT_OS_LINUX)
    MESSAGE (STATUS "Operating system is Linux")
ELSEIF (PROJECT_OS_BSD)
    MESSAGE (STATUS "Operating system is BSD")
ELSEIF (PROJECT_OS_WIN)
    MESSAGE (STATUS "Operating system is Windows")
ELSEIF (PROJECT_OS_OSX)
    MESSAGE (STATUS "Operating system is Apple MacOS X")
ELSEIF (PROJECT_OS_MACOS)
    MESSAGE (STATUS "Operating system is Apple MacOS (not OS X)")
ELSEIF (PROJECT_OS_QNX)
    MESSAGE (STATUS "Operating system is QNX")
ELSEIF (PROJECT_OS_SOLARIS)
    MESSAGE (STATUS "Operating system is Solaris")
ELSE (PROJECT_OS_LINUX)
    MESSAGE (STATUS "Operating system is generic Unix")
ENDIF (PROJECT_OS_LINUX)

MESSAGE (STATUS "Got System Processor ${CMAKE_SYSTEM_PROCESSOR}")

# 32 or 64 bit Linux
IF (PROJECT_OS_LINUX)
    # Set the library directory suffix accordingly
    IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        SET (PROJECT_PROC_64BIT TRUE BOOL INTERNAL)
        MESSAGE (STATUS "Linux x86_64 Detected")
    ELSEIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "ppc64")
        MESSAGE (STATUS "Linux ppc64 Detected")
        SET (PROJECT_PROC_64BIT TRUE BOOL INTERNAL)
    ENDIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
ENDIF (PROJECT_OS_LINUX)
