DEB_START("Configuration")

set( LIBRARY_OUTPUT_PATH  "${CMAKE_BINARY_DIR}/out" )     # Where will be put all the libraries
set( EXECUTABLE_OUTPUT_PATH  "${CMAKE_BINARY_DIR}/out" )  # Where will be put all the executables
DEB( "output: ${LIBRARY_OUTPUT_PATH} (libs), ${EXECUTABLE_OUTPUT_PATH} (exec)")
	
set_property( GLOBAL PROPERTY USE_FOLDERS  ON )           # Turn on using solution folders
set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS  ON)                # Shared libraries on MSVC without explicit necessity to add export macros or .def file

if( NOT CMAKE_SIZEOF_VOID_P GREATER 0 ) # Break in case of popular CMake configuration mistakes
    message( FATAL_ERROR "Error in bitness of target platform. If you are crosscompiling, check your CMake toolchain file")
endif()

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}") # Warning in case of in-source build
    message( WARNING "The source directory is the same as binary directory. \"make clean\" may damage the source tree")
endif()

if(MSVC)
    set( CMAKE_USE_RELATIVE_PATHS ON CACHE INTERNAL "" FORCE )
endif()

include( GenerateExportHeader )  # Function for generation of export macros for libraries

set( CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo"  CACHE STRING "Build Configurations" FORCE )
if( DEFINED CMAKE_BUILD_TYPE )
    set_property( CACHE CMAKE_BUILD_TYPE  PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES} )
endif()
if(CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
    DEB( "configuration types: ${CMAKE_CONFIGURATION_TYPES}" )
else()
    DEB( "configuration: ${CMAKE_BUILD_TYPE})" )
endif()

DEB_END("Configuration")
