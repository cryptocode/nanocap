# - Try to find json.hpp
# Once done, this will define
# 
#  JSONForModernCPP_FOUND - system has json.hpp
#  JSONForModernCPP_INCLUDE_DIRS - the json.hpp include directories

find_path(JSONForModernCPP_INCLUDE_DIRS NAMES nlohmann/json.hpp
    PATHS /nanocap/external/JSONForModernCPP/include)

add_library(JSONForModernCPP INTERFACE IMPORTED)
set_target_properties(JSONForModernCPP PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${JSONForModernCPP_INCLUDE_DIRS}")

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set JSONForModernCPP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(JSONForModernCPP DEFAULT_MSG JSONForModernCPP_INCLUDE_DIRS)

mark_as_advanced(JSONForModernCPP_INCLUDE_DIRS)
