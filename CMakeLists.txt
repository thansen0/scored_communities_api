cmake_minimum_required(VERSION 3.18)

project(scored_communities_api LANGUAGES CXX)

# ----- Options ---------------------------------------------------------------
# Build examples only when this repo is the top-level, unless explicitly toggled.
set(_default_build_examples OFF)
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(_default_build_examples ON)
endif()
option(SCOREDCOAPI_BUILD_EXAMPLES "Build scored_communities_api examples" ${_default_build_examples})

# C++ standard as a usage requirement for consumers (since we're header-only).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----- Dependencies ----------------------------------------------------------
find_package(CURL REQUIRED)

# ----- Library target (header-only) ------------------------------------------
add_library(scoredcoapi INTERFACE)

target_compile_options(scoredcoapi INTERFACE -Wall -Wextra -pedantic -Wno-comment)

# Public (interface) usage requirements:
#  - consumers get include dir
target_include_directories(scoredcoapi
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include> # for install() rules
)
#  - consumers get CURL::libcurl
target_link_libraries(scoredcoapi
  INTERFACE
    CURL::libcurl
)
#  - consumers get c+20 requirement
target_compile_features(scoredcoapi
  INTERFACE
    cxx_std_20
)

# Provide a namespaced alias for nicer consumption.
add_library(scoredco::api ALIAS scoredcoapi)

# ----- Examples (optional) ---------------------------------------------------
if (SCOREDCOAPI_BUILD_EXAMPLES)
  file(GLOB EXAMPLES_FILES CONFIGURE_DEPENDS examples/*.cpp)
  foreach(EXAMPLES_FILE IN LISTS EXAMPLES_FILES)
    get_filename_component(TARGET_NAME "${EXAMPLES_FILE}" NAME_WE)
    add_executable(${TARGET_NAME} "${EXAMPLES_FILE}")
    target_link_libraries(${TARGET_NAME} PRIVATE scoredco::api)
    # If examples need extra warnings or flags, add them here.
  endforeach()
endif()

# ----- (Optional) Install/export scaffold ------------------------------------
# Enables `find_package(scored_communities_api)` support.
include(GNUInstallDirs)
install(TARGETS scoredcoapi
  EXPORT scoredcoapiTargets
)
install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT scoredcoapiTargets
  NAMESPACE scoredco::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/scored_communities_api
)
