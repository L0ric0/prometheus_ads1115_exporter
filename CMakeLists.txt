cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

project(prometheus-ads1115-exporter VERSION 0.0.1)

option(COVERAGE OFF)
if(COVERAGE)
    include(CodeCoverage)
endif()

add_subdirectory(prometheus_ads1115_exporter)

### TESTS ###
add_subdirectory(tests)

### LIBS ###
add_subdirectory(libs/ads1115)

### DOCS ###
find_package(Doxygen OPTIONAL_COMPONENTS dot)

set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md")

doxygen_add_docs(doc ${CMAKE_SOURCE_DIR}/prometheus_ads1115_exporter)
