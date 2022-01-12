cmake_minimum_required(VERSION 3.1)
project(OGDF-PROJECT CXX)

set(module_dir "${PROJECT_SOURCE_DIR}/cmake")
list(INSERT CMAKE_MODULE_PATH 0 "${module_dir}" )

# build type configuration
set(BUILD_TYPES Debug Release)
if(CMAKE_CONFIGURATION_TYPES)
  set(MULTICONFIG_BUILD 1)
  set(CMAKE_CONFIGURATION_TYPES ${BUILD_TYPES} CACHE STRING "" FORCE)
else()
  set(MULTICONFIG_BUILD 0)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build to be created." FORCE)
  endif()
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BUILD_TYPES})
endif()
set(CMAKE_CXX_STANDARD 11)

include(compiler-specifics)
include(group-files)
# include(coin)

find_package(COIN REQUIRED)
set(COIN_SOLVER "CLP" CACHE STRING "Linear program solver to be used by COIN.")
set_property(CACHE COIN_SOLVER PROPERTY STRINGS CLP CPX GRB)
set(COIN_SOLVER_IS_EXTERNAL 0)
# target_link_libraries(${YOUR_TARGET_NAME}
# PRIVATE
#     libCbc libCgl libOsiClp libOsi libClp libCoinUtils
# )
SET(COIN_INSTALL_INCLUDE_DIR ${COIN_INCLUDE_DIR})

include(ogdf)
target_link_libraries(OGDF PUBLIC ${COIN_LIBRARIES})
target_include_directories(OGDF SYSTEM PUBLIC ${COIN_INCLUDE_DIR})

find_package(Backward)
target_link_libraries(OGDF PUBLIC Backward::Backward)
target_include_directories(OGDF SYSTEM PUBLIC ${BACKWARD_INCLUDE_DIRS})

find_package(pugixml)
target_link_libraries(OGDF PUBLIC pugixml pugixml::pugixml)
get_target_property(PUGIXML_INCLUDE_DIRECTORIES pugixml::pugixml INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(OGDF SYSTEM PUBLIC ${PUGIXML_INCLUDE_DIRECTORIES})
message(STATUS "${PUGIXML_INCLUDE_DIRECTORIES}")

include(make-user-target)
include(tests)
include(examples)

include(doc)

add_custom_target(build-all DEPENDS OGDF tests examples)

# create autogen header
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/ogdf/basic/internal")
configure_file("${module_dir}/config_autogen.h.in" "${PROJECT_BINARY_DIR}/include/ogdf/basic/internal/config_autogen.h")

# target documentation
message(STATUS "The default target builds OGDF (and dependencies like COIN).")
if(DOXYGEN_FOUND AND DOC_INSTALL)
  message(STATUS "Because DOC_INSTALL is set, it also generates the OGDF documentation (in-source).")
endif()
message(STATUS "")
message(STATUS "Other important targets:")
if(DOXYGEN_FOUND)
  if (DOC_INSTALL)
    message(STATUS "        OGDF: build OGDF library only")
  endif()
  message(STATUS "         doc: build doxygen documentation (in-source)")
endif()
message(STATUS "       tests: build tests")
message(STATUS "    examples: build examples")
message(STATUS "   build-all: build OGDF, tests, examples")
