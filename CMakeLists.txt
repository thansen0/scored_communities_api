cmake_minimum_required(VERSION 3.10) # Or a suitable minimum version

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

# Create an executable target 
add_executable(two_pages_no_login examples/two_pages_no_login.cpp)
add_executable(print_json_no_login examples/print_json_no_login.cpp)
add_executable(view_user_no_login examples/view_user_no_login.cpp)

# Link the executable with the header library
target_link_libraries(two_pages_no_login ApiHeaders CURL::libcurl)
target_link_libraries(print_json_no_login ApiHeaders CURL::libcurl)
target_link_libraries(view_user_no_login ApiHeaders CURL::libcurl)

