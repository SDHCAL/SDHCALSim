cmake_minimum_required(VERSION 3.9)

project(SDHCALSim VERSION "0.2.0" LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_SOURCE_DIR}/cmake")

if(DEFINED ENV{LCIO})
  set(CMAKE_PREFIX_PATH "$ENV{LCIO};${CMAKE_PREFIX_PATH}" CACHE PATH "CMAKE_PREFIX_PATH" FORCE)
  message(STATUS "LCIO environment found ${CMAKE_PREFIX_PATH}")
endif()

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(CMAKE_LINK_DEPENDS_NO_SHARED TRUE)

# guard against in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/bin" CACHE PATH "Default Install Prefix" FORCE)
  message(STATUS "Install Prefix set to : ${CMAKE_INSTALL_PREFIX}")
endif()

set(LIBRARY_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(RUNTIME_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}")
set(ARCHIVE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(INCLUDE_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(CMAKE_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/cmake")

set(CMAKE_MACOSX_RPATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${LIBRARY_OUTPUT_DIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(CMAKE_CONFIGURATION_TYPES)
  if(NOT "Coverage" IN_LIST CMAKE_CONFIGURATION_TYPES)
    list(APPEND CMAKE_CONFIGURATION_TYPES Coverage)
  endif()
  if(NOT "Profile" IN_LIST CMAKE_CONFIGURATION_TYPES)
    list(APPEND CMAKE_CONFIGURATION_TYPES Profile)
  endif()
else()
  set(allowableBuildTypes
      Debug
      MinSizeRel
      None
      RelWithDebInfo
      Release
      Coverage
      Profile)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowableBuildTypes}")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
  elseif(NOT CMAKE_BUILD_TYPE IN_LIST allowableBuildTypes)
    message(FATAL_ERROR "Invalid build type : ${CMAKE_BUILD_TYPE}")
  endif()
endif()

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

if(POLICY CMP0083)
  cmake_policy(SET CMP0083 NEW)
  include(CheckPIESupported)
  check_pie_supported(OUTPUT_VARIABLE output LANGUAGES CXX)
  if(NOT CMAKE_CXX_LINK_PIE_SUPPORTED)
    message(WARNING "PIE is not supported at link time: ${output}.\n" "PIE link options will not be passed to linker.")
  else()
    set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
    message(STATUS "PIE link options will be passed to linker.")
  endif()
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif()

# Optional IPO. Do not use IPO if it's not supported by compiler.
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "IPO is not supported: ${output}")
endif()

option(ENABLE_ALL_WARNINGS "Enable all warnings" TRUE)

include(CompilerOptions)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#

option(WITH_GEANT4_VIS "Build example with Geant4 Vis drivers" ON)
if(WITH_GEANT4_VIS)
  find_package(Geant4 REQUIRED vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

find_package(ROOT REQUIRED)
find_package(LCIO 2.11 REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)


file(DOWNLOAD "https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp" "${CMAKE_INSTALL_PREFIX}/include/json.hpp" SHOW_PROGRESS EXPECTED_HASH SHA256=3b5d2b8f8282b80557091514d8ab97e27f9574336c804ee666fda673a9b59926 STATUS state)
list(GET state 0 errorNumber)
if(NOT ${errorNumber} STREQUAL "0")
  list(GET state 1 error)
  message(WARNING "ERROR : ${error} \nCMake can't download the file \"https://github.com/nlohmann/json/releases/download/v3.7.3/json.hpp\" ! Do it by yourself and add it to \"${CMAKE_INSTALL_PREFIX}/include/json.hpp\" and rerun CMake !")
endif()

add_subdirectory(libs)

add_subdirectory(apps)
