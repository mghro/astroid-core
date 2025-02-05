cmake_minimum_required(VERSION 3.16)

# Set VCPKG target triplet to use our mixed-linkage build.
if (WIN32)
    # This assumes we're doing a 64-bit build. It seems impossible to verify
    # that at this point in the CMake execution - it's possible after the call
    # to `project()`, but we need to set these variables before that - but we
    # could add an option to support 32-bit builds.
    set(VCPKG_TARGET_TRIPLET x64-windows-mixed)
    set(VCPKG_HOST_TRIPLET x64-windows-mixed)
endif()

project(astroid)

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

set(CMAKE_CXX_STANDARD 20)

# Enable a high level of compiler warnings and treat them as errors.
function(set_warning_levels target)
    if(IS_GCC OR IS_CLANG)
        target_compile_options(${target} PUBLIC -Wall -Werror -Wextra -pedantic)
        # Disable warnings that are too strict.
        # unused function parameters
        target_compile_options(${target} PUBLIC -Wno-unused-parameter)
        # Workaround for https://github.com/andreasbuhr/cppcoro/issues/77:
        # set(CMAKE_CXX_STANDARD 20) causes CMake to append -std=gnu++2a,
        # causing a built-in -Dlinux=1,
        # causing build failures when "namespace linux" is encountered.
        target_compile_options(${target} PUBLIC -Ulinux)
    elseif(IS_MSVC)
        # First strip out the old warning level.
        string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
        target_compile_options(${target} PUBLIC /W4 /WX)
        # Enable big object files.
        target_compile_options(${target} PUBLIC /bigobj)
        # Disable warnings that are too strict.
        # "unreferenced formal parameter"
        target_compile_options(${target} PUBLIC /wd4100)
        # "declaration hides previous local declaration"
        target_compile_options(${target} PUBLIC /wd4456)
        # "unreferenced local function has been removed"
        target_compile_options(${target} PUBLIC /wd4505)
        # Warnings about a derived class inheriting member functions via dominance
        # (which is correct and intended).
        target_compile_options(${target} PUBLIC /wd4250)
        # warnings about functions that are potentially insecure
        target_definitions(${target} PUBLIC /D_CRT_SECURE_NO_WARNINGS)
        target_definitions(${target} PUBLIC /D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
        # Also suppress linker warnings about missing .pdb files that seem to inevitably creep in.
        target_link_options(${target} PUBLIC /ignore:4099)
        # "operator '|': deprecated between enumerations of different types"
        # in cereal\external\rapidjson\document.h
        target_compile_options(${target} PUBLIC /wd5054)
        # The "unreachable code" warning is never to the point, and due to lack of
        # context impossible to fix.
        target_compile_options(${target} PUBLIC /wd4702)
    endif()
    if(IS_GCC)
        # Unclear what these warnings exactly mean to say, and whether they are valid.
        # Clang and MSVC don't complain.
        target_compile_options(${target} PUBLIC -Wno-subobject-linkage)
    endif()
endfunction()

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
  GIT_TAG        v2.13.9
)
FetchContent_MakeAvailable(catch)

add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)

# Download, extract, and setup Clipper.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.4.2")
    message(STATUS "Downloading Clipper")
    file(DOWNLOAD "https://sourceforge.net/projects/polyclipping/files/clipper_ver6.4.2.zip"
                  "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.4.2.zip"
                  TLS_VERIFY ON)
    message(STATUS "Extracting Clipper")
    file(ARCHIVE_EXTRACT
        INPUT "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.4.2.zip"
        DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.4.2")
endif()
set(CLIPPER_INCLUDE_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.4.2/cpp")
set(CLIPPER_SOURCE_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/clipper_ver6.4.2/cpp/clipper.cpp")
add_library(clipper STATIC  ${CLIPPER_SOURCE_FILES})
target_include_directories(clipper PUBLIC ${CLIPPER_INCLUDE_DIR})


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
    GIT_TAG c0b77d2571a8cfd2bfaa0713088726640694ad3f)
FetchContent_MakeAvailable(preprocessor)

# Preprocess the API headers.
preprocess_header_files(
    generated_srcs generated_headers
    ACCOUNT_ID mgh
    # TODO: Make this its own actual app...
    TYPE_APP_ID dosimetry
    FUNCTION_APP_ID dosimetry
    NAMESPACE astroid
    INDEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/src/astroid/api_index.hpp"
    INPUT_FILES ${preprocessed_files})
list(APPEND srcs ${generated_srcs})
list(APPEND headers ${generated_headers}
    "${CMAKE_CURRENT_BINARY_DIR}/generated/src/astroid/api_index.hpp")

find_package(Boost REQUIRED
    COMPONENTS date_time filesystem)
find_package(msgpack CONFIG REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(cradle REQUIRED)
find_package(ZLIB REQUIRED)

add_library(astroid STATIC ${srcs} ${headers} ${impls})
set_warning_levels(astroid)
target_include_directories(astroid PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/generated/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(astroid PUBLIC
    clipper
    cradle::cradle_typing
    Catch2::Catch2
    msgpackc-cxx
    Boost::boost
    Boost::filesystem
    Boost::date_time
    ZLIB::ZLIB)

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

# Add the unit test runner.
file(GLOB_RECURSE UNIT_TEST_FILES CONFIGURE_DEPENDS "unit_tests/*.cpp")
add_executable(unit_test_runner ${UNIT_TEST_FILES})
target_link_libraries(unit_test_runner astroid)
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
                             -E env ASTROID_DEPLOY_DIR=${PROJECT_BINARY_DIR}
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
