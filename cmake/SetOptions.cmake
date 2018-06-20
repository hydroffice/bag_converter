DEB_START("Options")

option(                 BAGCNV_SHARED "Build shared libraries"                OFF )
if( BAGCNV_SHARED )
    set(                BAGCNV_LIB_TYPE 					                  "SHARED" )
else()
    set(                BAGCNV_LIB_TYPE 					                  "STATIC" )
endif()
DEB(                    "library type: ${BAGCNV_LIB_TYPE}")

option(                 BAGCNV_MODE_VERBOSE "Build in verbose mode" 		  ON )
DEB(                    "verbose mode: ${BAGCNV_MODE_VERBOSE}")
if( BAGCNV_MODE_VERBOSE )
    add_definitions(    -DBAGCNV_VERBOSE )
endif()

 if( MSVC_VERSION EQUAL 1900 )
	option(             BAGCNV_USE_INTERNAL_BINS "Build against internal binaries" ON )
	DEB(                "internal binaries: ${BAGCNV_USE_INTERNAL_BINS}")
 endif()

DEB_END("Options")