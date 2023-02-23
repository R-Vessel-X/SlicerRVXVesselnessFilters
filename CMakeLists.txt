cmake_minimum_required(VERSION 3.16.3...3.19.7 FATAL_ERROR)

project(RVesselXCLI)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://github.com/JonasLamy/Slicer3DVesselness")
set(EXTENSION_CATEGORY "Filtering.Vesselness")
set(EXTENSION_CONTRIBUTORS "Jonas Lamy")
set(EXTENSION_DESCRIPTION "Hessian Vesselness Filters extension")
set(EXTENSION_ICONURL "https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVXLogo.png")
set(EXTENSION_SCREENSHOTURLS "https://github.com/R-Vessel-X/SlicerRVXVesselnessFilters/raw/main/Screenshots/RVX_vesselness_example.png")
set(EXTENSION_DEPENDS "NA") # Specified as a list or "NA" if no dependencies

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(FrangiVesselness)
add_subdirectory(JermanVesselness)
add_subdirectory(MeijeringVesselness)
add_subdirectory(OOFVesselness)
add_subdirectory(SatoVesselness)
add_subdirectory(ZhangVesselness)
add_subdirectory(RORPOVesselness)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
