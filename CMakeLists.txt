cmake_minimum_required (VERSION 2.8)


# possible values: Release, Debug, RelWithDebugInfo
# set(CMAKE_BUILD_TYPE RelWithDebugInfo)

project(ladspa.m.plugins)
set(PROJECT_VERSION "1")

set(CMAKE_INSTALL_PREFIX /usr)

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Florian Paul Schmidt (mista.tapas@gmx.net)")
set(CPACK_GENERATOR "DEB" "TGZ")
set(CPACK_DEBIAN_PACKAGE_NAME "ladspa.m.plugins-${PROJECT_VERSION}")
include(CPack)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Native CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Native."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

enable_testing()

set(CMAKE_VERBOSE_MAKEFILE off)

add_definitions(-DPROJECT_VERSION=\"${PROJECT_VERSION}\")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wall -pthread -fmax-errors=1")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -pg")
endif()

if (CMAKE_BUILD_TYPE MATCHES Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    add_definitions("-DNDEBUG")
endif()

if (CMAKE_BUILD_TYPE MATCHES RelWithDebugInfo)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")
    add_definitions("-DNDEBUG")
endif()

if (CMAKE_BUILD_TYPE MATCHES Native)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native -mfpmath=sse")
    add_definitions("-DNDEBUG")
endif()

if (CMAKE_BUILD_TYPE MATCHES NativeKvm)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native -mfpmath=sse -mno-avx")
    add_definitions("-DNDEBUG")
endif()

message(STATUS "The build type is: " ${CMAKE_BUILD_TYPE})

#
# TARGETS
# 

add_library(ladspa.m.osc-plugins-${PROJECT_VERSION} SHARED ladspa.m-osc-plugins.cc)

# 
# INSTALLATION
# 

install(TARGETS ladspa.m.osc-plugins-${PROJECT_VERSION} LIBRARY DESTINATION lib/ladspa)

