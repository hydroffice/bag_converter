DEB_START("3rd Party")

if( WIN32 )
    string( REPLACE "\\" "/" APPDATA_FOLDER $ENV{LOCALAPPDATA} )
    set( CMAKE_PREFIX_PATH  ${CMAKE_SOURCE_DIR}/ext )
    DEB( "Search libraries with prefix path: ${CMAKE_PREFIX_PATH}" )
endif()

# BAG
find_library( BAG_LIB bag )
if(NOT BAG_LIB)
  message(FATAL_ERROR "BAG library not found")
endif()
DEB( "BAG: OK (${BAG_LIB})")

find_package( GDAL  REQUIRED )
if(${GDAL_FOUND})
	include_directories( ${GDAL_INCLUDE_DIR} )
	DEB( "GDAL: OK (${GDAL_LIBRARY})" )
endif()

DEB_END("3rd Party")
