# CMake script that set the installation folder (default or user-defined)

DEB_START("Installation Folder")

# CMAKE_CROSSCOMPILING variable will be set only on project() command
# so we will try to detect crosscompiling by presense of CMAKE_TOOLCHAIN_FILE
if( NOT CMAKE_TOOLCHAIN_FILE )
    if( WIN32 )
        set( CMAKE_INSTALL_PREFIX "install" 	CACHE PATH "Installation Directory")
    else()
        set( CMAKE_INSTALL_PREFIX "/usr/local" 	CACHE PATH "Installation Directory")
    endif()
else() # any crosscompiling
    set( CMAKE_INSTALL_PREFIX "install" 		CACHE PATH "Installation Directory" )
endif()
DEB( "install prefix: ${CMAKE_INSTALL_PREFIX}")

DEB_END("Installation Folder")