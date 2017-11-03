cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(SDHCALSim)

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 2 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package( ROOT REQUIRED )
#find_package( CLHEP REQUIRED )
find_package( LCIO REQUIRED )

FOREACH( pkg LCIO ROOT Geant4 )
	IF( ${pkg}_FOUND )
		INCLUDE_DIRECTORIES( SYSYEM ${${pkg}_INCLUDE_DIRS} )
		LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
		ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
	ENDIF()
ENDFOREACH()


ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++11" )

include( ${Geant4_USE_FILE} )
include_directories( ${PROJECT_SOURCE_DIR}/include )

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_library( MyLib ${sources} ${headers} )


file( GLOB APP_SOURCES ${PROJECT_SOURCE_DIR}/exec/*.cxx )
foreach( testsourcefile ${APP_SOURCES} )
	get_filename_component(testname ${testsourcefile} NAME_WE)
	add_executable( ${testname} ${testsourcefile} )
	target_link_libraries( ${testname} MyLib ${ROOT_COMPONENT_LIBRARIES} )
	install(TARGETS ${testname} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endforeach( testsourcefile ${APP_SOURCES} )
