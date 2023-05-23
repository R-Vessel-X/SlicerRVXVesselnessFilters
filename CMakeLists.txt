cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(RVXVesselnessFilters)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://github.com/JonasLamy/Slicer3DVesselness")
set(EXTENSION_CATEGORY "Filtering.Vesselness")
set(EXTENSION_CONTRIBUTORS "Jonas Lamy")
set(EXTENSION_DESCRIPTION "Hessian Vesselness Filters extension")
set(EXTENSION_ICONURL "https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVXLogo.png")
set(EXTENSION_SCREENSHOTURLS "https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVX_vesselness_example.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

set(SUPERBUILD_TOPLEVEL_PROJECT inner)

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# SuperBuild setup
option(${EXTENSION_NAME}_SUPERBUILD "Build ${EXTENSION_NAME} and the projects it depends on." ON)
mark_as_advanced(${EXTENSION_NAME}_SUPERBUILD)
if(${EXTENSION_NAME}_SUPERBUILD)
    include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
    return()
endif()

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(FrangiVesselness)
add_subdirectory(JermanVesselness)
add_subdirectory(MeijeringVesselness)
add_subdirectory(OOFVesselness)
add_subdirectory(SatoVesselness)
add_subdirectory(ZhangVesselness)

# At the moment RORPO vessleness only works for UNIX due to OpenMP dependency
# The RORPO repository defines its SlicerCLI binding which is included here with
# consent from Odyssée Merveille, the creator and maintainer of the library.
if (UNIX)
  include(FetchContent)
  FetchContent_Declare(
    RORPO
    GIT_REPOSITORY https://github.com/path-openings/RORPO.git
    GIT_TAG master
    SOURCE_DIR rorpo-src
  )

  FetchContent_GetProperties(RORPO)
  if (NOT RORPO_POPULATED)
    FetchContent_Populate(RORPO)
  endif ()
  
  # Build RORPO with 3D Slicer binding option and add missing link to the Math library.
  set(PYTHON_BINDING OFF)
  set(3DSLICER_BINDING ON)
  link_libraries(m) 
  add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/rorpo-src
    ${CMAKE_CURRENT_BINARY_DIR}/rorpo-build
  )
endif ()

#-----------------------------------------------------------------------------
set(EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS)
set(${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS "${EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS}" CACHE STRING "List of external projects to install" FORCE)

#-----------------------------------------------------------------------------
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS}")
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
