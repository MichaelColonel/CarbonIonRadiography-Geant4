#----------------------------------------------------------------------------
# Setup the project
#----------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(CarbonIonRadiography)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#--------------------------------------------------------------------------
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
option(WITH_ROOT "Build example with ROOT support" ON)

if(WITH_GEANT4_UIVIS)
  find_package(Geant4 COMPONENTS ui_all vis_all REQUIRED)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#----------------------------------------------------------------------------
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#----------------------------------------------------------------------------
file(GLOB sources
	${PROJECT_SOURCE_DIR}/src/*.cc
	${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB headers
	${PROJECT_SOURCE_DIR}/include/*.hh
	${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#----------------------------------------------------------------------------
add_executable(cir-run cir.cc ${sources} ${headers})

#----------------------------------------------------------------------------
# Find ROOT variables if the variable GEANT4_USE_ROOT is set
#----------------------------------------------------------------------------
if(WITH_ROOT)
 find_package(ROOT)
endif()

if(ROOT_FOUND)
 add_definitions(-DGEANT4_USE_ROOT)
 include_directories(${ROOT_INCLUDE_DIR})
 target_link_libraries(cir-run ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
else()
 target_link_libraries(cir-run ${Geant4_LIBRARIES})
endif()

#----------------------------------------------------------------------------
# Gnu Scientific Library - GSL
#----------------------------------------------------------------------------

find_package(PkgConfig)

pkg_check_modules(GSL REQUIRED gsl)
if(GSL_FOUND)
 include_directories(${GSL_INCLUDE_DIRS})
 target_link_libraries(cir-run ${GSL_LIBRARIES})
endif()

pkg_check_modules(TREC REQUIRED trec)
if(TREC_FOUND)
 include_directories(${TREC_INCLUDE_DIRS})
 target_link_libraries(cir-run ${TREC_LIBRARIES})
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build CarbonIonRadiography. This is so that we can run the executable
# directly because it relies on these scripts being in the current
# working directory.
#----------------------------------------------------------------------------
set(CarbonIonRadiography_SCRIPTS
  visu.mac
  novisu.mac
  init.mac
  cir_visu.mac
  cir_novisu.mac
  RunMe.sh
  )

foreach(_script ${CarbonIonRadiography_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#----------------------------------------------------------------------------
add_custom_target(CarbonIonRadiography DEPENDS cir-run)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#----------------------------------------------------------------------------
install(TARGETS cir-run DESTINATION bin )
