cmake_minimum_required(VERSION 3.14)
project(cradle)

include(FetchContent)

# Download/include CMake/Conan integration.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                  "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
                  TLS_VERIFY ON)
endif()
include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

# Invoke Conan.
conan_add_remote(
    NAME bincrafters
    INDEX 1
    URL https://api.bintray.com/conan/bincrafters/public-conan
    VERIFY_SSL True)
conan_cmake_run(
    REQUIRES
        boost_date_time/1.66.0@bincrafters/stable
        boost_format/1.66.0@bincrafters/stable
        boost_polygon/1.66.0@bincrafters/stable
        boost_uuid/1.66.0@bincrafters/stable
        clipper/6.4.2
        jsoncpp/1.9.4
        msgpack/3.3.0
    BASIC_SETUP
    BUILD missing)

add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)

# Download, extract, and setup Clipper.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0")
    message(STATUS "Downloading Clipper")
    file(DOWNLOAD "https://sourceforge.net/projects/polyclipping/files/Older%20versions/clipper_ver6.0.0.zip/download"
                  "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0.zip"
                  TLS_VERIFY ON)
    message(STATUS "Extracting Clipper")
    file(ARCHIVE_EXTRACT
        INPUT "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0.zip"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0")
endif()
set(CLIPPER_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0/cpp")
set(CLIPPER_SOURCE_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0/cpp/clipper.cpp")

# C++ sources
file(GLOB_RECURSE srcs CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE c_srcs CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
list(APPEND srcs ${c_srcs})

# C++ headers
file(GLOB_RECURSE headers CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
set(preprocessed_files ${headers})
file(GLOB_RECURSE c_headers CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
list(APPEND headers ${c_headers})
file(GLOB_RECURSE impls CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ipp")

# Fetch the Astroid preprocessor.
FetchContent_Declare(preprocessor
    GIT_REPOSITORY https://github.com/mghro/astroid-preprocessor
    GIT_TAG master)
FetchContent_MakeAvailable(preprocessor)

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

add_library(cradle STATIC ${srcs} ${headers} ${impls} ${CLIPPER_SOURCE_FILES})
target_include_directories(cradle PUBLIC
    ${CONAN_INCLUDE_DIRS}
    ${CLIPPER_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(cradle ${CONAN_LIBS})

# add_executable(generate_api generate_api.cpp)
# target_link_libraries(generate_api cradle ${MY_LIBRARIES})
