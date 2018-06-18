set( 					TARGET_NAME  			"bag_converter" )
set( 					TARGET_DEBUG_POSTFIX 	"_d" )
add_executable( 		bag_converter 		  	bag_converter.cpp
												helper.h helper.cpp
												converter.h converter.cpp	
												)
set_target_properties( 	bag_converter		    PROPERTIES 
												OUTPUT_NAME ${TARGET_NAME}
												DEBUG_POSTFIX ${TARGET_DEBUG_POSTFIX} )		
target_link_libraries( 	bag_converter 		    ${BAG_LIB}  ${GDAL_LIBRARY} )
add_custom_command(     TARGET bag_converter   	POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/metadata ${CMAKE_BINARY_DIR}/out/${CMAKE_CFG_INTDIR}/metadata )
add_custom_command(     TARGET bag_converter   	POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ext/configdata ${CMAKE_BINARY_DIR}/out/${CMAKE_CFG_INTDIR}/configdata )
add_custom_command(     TARGET bag_converter    POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/metadata ${CMAKE_CURRENT_BINARY_DIR}/metadata )  							
add_custom_command(     TARGET bag_converter    POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/ext/configdata ${CMAKE_CURRENT_BINARY_DIR}/configdata )  												

install( TARGETS bag_converter                	RUNTIME DESTINATION .
												LIBRARY DESTINATION .
												ARCHIVE DESTINATION . 
												)	
install( DIRECTORY ${CMAKE_SOURCE_DIR}/ext/configdata 	DESTINATION . )		
install( DIRECTORY ${CMAKE_SOURCE_DIR}/src/metadata 	DESTINATION . )												

set( INST_DIR  "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_PREFIX}" )
set( CMAKE_INSTALL_DEBUG_LIBRARIES  OFF )
set( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS  ON )
set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION  ${INST_DIR} )
include( InstallRequiredSystemLibraries )
install( CODE 	"
			include(BundleUtilities)
			if(\"\${CMAKE_INSTALL_CONFIG_NAME}\" STREQUAL \"Release\")
				fixup_bundle( \"${INST_DIR}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}\" \"\" \"\")
			endif()
			"
		)
