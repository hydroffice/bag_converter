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

DEB_END("Options")