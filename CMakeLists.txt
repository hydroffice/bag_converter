option(                     BAGCNV_DEBUG_CMAKE "Debug the CMake files" ON )         # CMake file debugging
set(					    CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}					${CMAKE_CURRENT_SOURCE_DIR}/cmake )
include(                    cmake/GenericFunctions.cmake )	                        # Add some generic useful functions
cmake_minimum_required(     VERSION 3.8.0 )
DEB(                        "CMake: ${CMAKE_VERSION} [${CMAKE_GENERATOR}]" )

include( 				    cmake/SetInstallationFolder.cmake )                     # Set the installation folder (default or user-defined)
project(                    bag_converter CXX )
DEB(                        "project: ${CMAKE_PROJECT_NAME}" )
DEB(                        "source dir: ${CMAKE_SOURCE_DIR}" )
DEB(                        "binary dir: ${CMAKE_BINARY_DIR}" )

include( 				    cmake/SetConfiguration.cmake )	                        # Set the configuration: e.g Debug/Release, output folders
include( 				    cmake/GetVersion.cmake )	                        	# Detect version
include( 				    cmake/SetOptions.cmake )	                        	# Declare options
include( 				    cmake/DetectEnvironment.cmake )                         # Detect the building environment (e.g. setup the compiler)
include( 				    cmake/Find3rdParty.cmake )                              # Find external libraries

include_directories(        ${CMAKE_SOURCE_DIR}/ext/include )                   	# Include external headers
include_directories(        ${CMAKE_SOURCE_DIR}/src )                       		# Include library headers

add_subdirectory(           src )
