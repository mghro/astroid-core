cmake_minimum_required(VERSION 3.14)
project(cradle)

include(FetchContent)

FetchContent_Declare(preprocessor
    GIT_REPOSITORY https://github.com/mghro/astroid-preprocessor
    GIT_TAG master)
FetchContent_MakeAvailable(preprocessor)

# C++ sources
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE c_srcs CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
list(APPEND srcs ${c_srcs})

# C++ headers
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.hpp")
set(preprocessed_files ${headers})
file(GLOB_RECURSE c_headers CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h")
list(APPEND headers ${c_headers})
file(GLOB_RECURSE impls CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.ipp")

# Preprocess the API headers.
preprocess_header_files(
    generated_srcs generated_headers
    ACCOUNT_ID mgh
    TYPE_APP_ID cradle
    FUNCTION_APP_ID cradle
    NAMESPACE cradle
    INDEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/src/cradle/api_index.hpp"
    INPUT_FILES ${preprocessed_files})
list(APPEND srcs ${generated_srcs})
list(APPEND headers ${generated_headers}
    "${CMAKE_CURRENT_BINARY_DIR}/generated/src/cradle/api_index.hpp")

add_library(cradle STATIC ${srcs} ${headers} ${impls})
target_include_directories(cradle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# add_executable(generate_api generate_api.cpp)
# target_link_libraries(generate_api cradle ${MY_LIBRARIES})
