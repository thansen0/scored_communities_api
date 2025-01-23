cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)

project(ScoredCoAPI)

# Find libcurl package for ScoredCoApi header
find_package(CURL REQUIRED)

# library target for scored API and json headers
add_library(ApiHeaders INTERFACE)

# Specify include directories for the headers. This makes them available to
# targets. On your project you may place these in the include folder
target_include_directories(ApiHeaders INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# We want to make sure each file in the examples folder is built
# any .cpp file in examples/ will be turned into a stand-alone binary
file(GLOB EXAMPLES_FILES examples/*.cpp)

foreach(EXAMPLES_FILE ${EXAMPLES_FILES})
    # Set filename to TARGET_NAME
    get_filename_component(TARGET_NAME ${EXAMPLES_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${EXAMPLES_FILE})

    # link and build executable
    target_link_libraries(${TARGET_NAME} PRIVATE ApiHeaders CURL::libcurl)
endforeach()
