cmake_minimum_required(VERSION 3.9.5)

project(FrangiVesselness)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ITK 5.2 REQUIRED)
include(${ITK_USE_FILE})

set(CMAKE_BUILD_TYPE Release)


# CLI11
set(MODULE_INCLUDE_DIRECTORIES
   "${PROJECT_SOURCE_DIR}/include"
  )

# GenerateCLP
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})


# Slicer macro

SEMMacroBuildCLI(
   NAME FrangiVesselness
   TARGET_LIBRARIES ${ITK_LIBRARIES}
   INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
   )


