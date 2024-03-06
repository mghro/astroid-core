cmake_minimum_required(VERSION 2.6)
project(astroid_tests)

include(../cmake/UseCradle.cmake)

set(ASTROID_INCLUDE_UNIT_TESTING ON)
set(ASTROID_INCLUDE_GUI OFF)
set(ASTROID_INCLUDE_PARETO_NAVIGATION ON)
set(ASTROID_INCLUDE_DATABASE ON)
set(ASTROID_INCLUDE_STANDARD_IMAGE_IO ON)
set(ASTROID_INCLUDE_APP_CONFIG ON)
set(ASTROID_INCLUDE_DICOM_IO ON)
set(ASTROID_INCLUDE_SCRIPTING ON)
add_astroid(astroid "${CMAKE_CURRENT_SOURCE_DIR}/..")

enable_testing()

file (GLOB_RECURSE test_files RELATIVE ${CMAKE_SOURCE_DIR} "*.cpp")

foreach(test_file ${test_files})
    get_filename_component(path ${test_file} PATH)
    get_filename_component(basename ${test_file} NAME_WE)
    string(REPLACE "/" "__" test_name "${path}/${basename}")
    add_executable(${test_name} ${test_file})
    use_astroid(${test_name} astroid)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
