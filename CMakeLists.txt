cmake_minimum_required(VERSION 3.14)
project(cradle)

# Detect the compiler.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(IS_CLANG true)
else()
    set(IS_CLANG false)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(IS_GCC true)
else()
    set(IS_GCC false)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(IS_MSVC true)
else()
    set(IS_MSVC false)
endif()

set(CMAKE_CXX_STANDARD 17)

# Determine if we're being built through FetchContent.
if (${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    set(is_fetched FALSE)
else()
    set(is_fetched TRUE)
endif()

include(FetchContent)

# Include Catch.
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.0
)
FetchContent_MakeAvailable(catch)

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
        boost_asio/1.66.0@bincrafters/stable
        boost_assign/1.66.0@bincrafters/stable
        boost_date_time/1.66.0@bincrafters/stable
        boost_format/1.66.0@bincrafters/stable
        boost_polygon/1.66.0@bincrafters/stable
        boost_uuid/1.66.0@bincrafters/stable
        jsoncpp/1.9.4
        msgpack/3.3.0
    BASIC_SETUP
    BUILD missing)

add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)

# Download, extract, and setup Clipper.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.0.0")
    message(STATUS "Downloading Clipper")
    file(DOWNLOAD "https://sourceforge.net/projects/polyclipping/files/Older%20versions/clipper_ver6.0.0.zip"
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

# Don't use CONFIGURE_DEPENDS if we're being built as a dependency.
if (is_fetched)
    set(glob_options )
else()
    set(glob_options CONFIGURE_DEPENDS)
endif()

# C++ sources
file(GLOB_RECURSE srcs ${glob_options}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE c_srcs ${glob_options}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
list(APPEND srcs ${c_srcs})

# C++ headers
file(GLOB_RECURSE headers ${glob_options}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
set(preprocessed_files ${headers})
file(GLOB_RECURSE c_headers ${glob_options}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
list(APPEND headers ${c_headers})
file(GLOB_RECURSE impls ${glob_options}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ipp")

# Fetch the Astroid preprocessor.
FetchContent_Declare(preprocessor
    GIT_REPOSITORY https://github.com/mghro/astroid-preprocessor
    GIT_TAG 0.2.0)
FetchContent_MakeAvailable(preprocessor)

# Preprocess the API headers.
preprocess_header_files(
    generated_srcs generated_headers
    ACCOUNT_ID mgh
    # TODO: Make this its own actual app...
    TYPE_APP_ID dosimetry
    FUNCTION_APP_ID dosimetry
    NAMESPACE cradle
    INDEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/src/cradle/api_index.hpp"
    INPUT_FILES ${preprocessed_files})
list(APPEND srcs ${generated_srcs})
list(APPEND headers ${generated_headers}
    "${CMAKE_CURRENT_BINARY_DIR}/generated/src/cradle/api_index.hpp")

# The rest of this is concerned with testing, which we don't want to do if
# we're just building as a dependency.
if (is_fetched)
    return()
endif()

# Set build options for instrumenting test coverage.
enable_testing()
if(IS_CLANG AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Enabling gcov support")
    add_compile_options(-DLLVM_USE_LINKER=gold -fprofile-instr-generate
                        -fcoverage-mapping)
    string(APPEND CMAKE_EXE_LINKER_FLAGS
           " -fprofile-instr-generate -fcoverage-mapping")
endif()

add_library(cradle STATIC ${srcs} ${headers} ${impls} ${CLIPPER_SOURCE_FILES})
target_include_directories(cradle PUBLIC
    ${CONAN_INCLUDE_DIRS}
    ${CLIPPER_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(
    cradle PUBLIC ${CONAN_DEFINES} ${CONAN_COMPILE_DEFINITIONS})
target_link_directories(cradle PUBLIC ${CONAN_LIB_DIRS})
target_link_libraries(cradle PUBLIC ${CONAN_LIBS} Catch2::Catch2)

# Add the unit test runner.
file(GLOB_RECURSE UNIT_TEST_FILES CONFIGURE_DEPENDS "unit_tests/*.cpp")
add_executable(unit_test_runner ${UNIT_TEST_FILES})
target_link_libraries(unit_test_runner cradle)
target_include_directories(unit_test_runner
    PRIVATE ${PROJECT_SOURCE_DIR}/unit_tests)

# Add a target for running the unit tests.
add_custom_target(
    unit_tests
    # Create a fresh 'unit-testing' directory within the build dir and run the
    # tests with that.
    COMMAND ${CMAKE_COMMAND} -E remove_directory unit-testing
    COMMAND ${CMAKE_COMMAND} -E make_directory unit-testing
    COMMAND ${CMAKE_COMMAND} -E chdir unit-testing ${CMAKE_COMMAND}
                             -E env CRADLE_DEPLOY_DIR=${PROJECT_BINARY_DIR}
                             $<TARGET_FILE:unit_test_runner>
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS unit_test_runner)

# On Linux debug builds, the proper CMake test associated with the unit tests
# includes test coverage reporting.
if(IS_CLANG AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    string(REPLACE clang llvm-profdata LLVM_PROFDATA $ENV{CC})
    string(REPLACE clang llvm-cov LLVM_COV $ENV{CC})
    add_custom_target(
        unit_test_coverage
        COMMAND ${CMAKE_COMMAND} --build . --target unit_tests
        COMMAND ${LLVM_PROFDATA} merge -sparse unit-testing/default.profraw
                -o default.profdata
        COMMAND ${LLVM_COV} show -instr-profile=default.profdata
                $<TARGET_FILE:unit_test_runner>
                >${PROJECT_SOURCE_DIR}/coverage.txt
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
    add_test(
        NAME unit_test_coverage
        COMMAND ${CMAKE_COMMAND} --build . --target unit_test_coverage
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
else()
    add_test(
        NAME unit_tests
        COMMAND ${CMAKE_COMMAND} --build . --target unit_tests
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
