cmake_minimum_required(3.14)
project(cradle)

option(CRADLE_INCLUDE_GUI "Include GUI components" OFF)
option(CRADLE_INCLUDE_APP_CONFIG
    "Include platform-dependent storage of app data (requires wxWidgets)" OFF)
option(CRADLE_INCLUDE_STANDARD_IMAGE_IO
    "Include standard image file I/O (requires DevIL)" OFF)
option(CRADLE_INCLUDE_WEB_IO "Include web I/O (requires Curl)" OFF)

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
preprocess_cradle_header_files(
    ${CRADLE_SOURCE_DIR} generated_srcs generated_headers
    USE_INTERNAL_PREPROCESSOR
    ACCOUNT_ID <thinknode account ID>
    TYPE_APP_ID dosimetry
    FUNCTION_APP_ID dosimetry
    NAMESPACE cradle
    INDEX_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated/src/cradle/api_index.hpp"
    INPUT_FILES ${preprocessed_files})
list(APPEND srcs ${generated_srcs})
list(APPEND headers ${generated_headers}
    "${CMAKE_CURRENT_BINARY_DIR}/generated/src/cradle/api_index.hpp")

set(cradle_srcs ${srcs} ${headers} ${impls} ${cmake_files} ${preprocessor_files})

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
endif()
if (NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
endif()

set(included_libs )
if(CRADLE_INCLUDE_GUI)
    list(APPEND included_libs INCLUDE_SKIA INCLUDE_GLEW INCLUDE_WX)
endif()
if(CRADLE_INCLUDE_LUA)
    list(APPEND included_libs INCLUDE_LUA)
endif()
if(CRADLE_INCLUDE_PARETO_NAVIGATION)
    list(APPEND included_libs INCLUDE_CLP)
endif()
if(CRADLE_INCLUDE_STANDARD_IMAGE_IO)
    list(APPEND included_libs INCLUDE_DEVIL)
endif()
# OpenGL is considered a core requirement for now.
list(APPEND included_libs INCLUDE_OPENGL)
list(APPEND included_libs INCLUDE_ZLIB)
if(CRADLE_INCLUDE_WEB_IO)
    list(APPEND included_libs INCLUDE_CURL INCLUDE_OPENSSL INCLUDE_MSGPACK)
endif()
if(WIN32)
	list(APPEND included_libs INCLUDE_POSTGRESQL)
endif()

resolve_cradle_dependencies(MY ${included_libs})
include_directories(${MY_INCLUDE_DIRS})
# if (WIN32)
    include_directories("Unit_Tests/")
    include_directories("catch_unit_tests/")
# endif()

add_library(cradle STATIC ${cradle_srcs})
target_link_libraries(cradle ${MY_LIBRARIES})
set_property(TARGET cradle PROPERTY FOLDER "cradle")

# This has been moved to dosimetry in the rt_types_generate_api
# add_executable(generate_api generate_api.cpp)
# target_link_libraries(generate_api cradle ${MY_LIBRARIES})

add_executable(cradle_catch_unit_tests catch_unit_tests/catch_unit_tests.cpp ${catch_test_files} ${catch_test_srcs})
target_link_libraries(cradle_catch_unit_tests cradle ${MY_LIBRARIES})
set_property(TARGET cradle_catch_unit_tests PROPERTY FOLDER "cradle")

if (WIN32)
    add_executable(cradle_unit_tests_win32 Unit_Tests/win32/Unit_Tests_win32.cpp ${testfiles})
    target_link_libraries(cradle_unit_tests_win32 cradle ${MY_LIBRARIES})
    set_property(TARGET cradle_unit_tests_win32 PROPERTY FOLDER "cradle")
endif()
