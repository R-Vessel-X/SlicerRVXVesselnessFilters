cmake_minimum_required(VERSION 3.9.5)

project(SatoVesselness)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ITK 5.2 REQUIRED)
include(${ITK_USE_FILE})


# GenerateCLP
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})


# Slicer macro

SEMMacroBuildCLI(
  NAME SatoVesselness
  TARGET_LIBRARIES ${ITK_LIBRARIES}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
)


