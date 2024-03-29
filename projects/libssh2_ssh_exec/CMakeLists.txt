cmake_minimum_required(VERSION 3.2)

SET(PNAME "testlibssh2sftp")

PROJECT(${PNAME})

INCLUDE(../cmake/ModuleImport.cmake)
LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
INCLUDE(../cmake/ModuleCompileOptions.cmake)

find_package(Libssh2 REQUIRED CONFIG)

ModuleSetCompileOptions()
ModuleSetWinCompilerFlags()

SET(UNIQS_VERSION_MAJOR "1")
SET(UNIQS_VERSION_MINOR "0")
SET(UNIQS_VERSION_PATCH "1")
SET(UNIQS_VERSION "${UNIQS_VERSION_MAJOR}.${UNIQS_VERSION_MINOR}.${UNIQS_VERSION_PATCH}")

MESSAGE(STATUS "VERSION: ${UNIQS_VERSION}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../cfg/)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include/sol/)

FILE(GLOB project_src_cpp "src/*.cpp")
FILE(GLOB project_src_h "src/*.h")
FILE(GLOB project_src_curr "./*.cpp")
FILE(GLOB project_src_curr_cpp "./*.cpp")
FILE(GLOB project_src_curr_h "./*.h")
FILE(GLOB cfg_src_cpp "cfg/*.cpp")
FILE(GLOB cfg_src_h "cfg/*.h")
FILE(GLOB project_include "include/*.h")

IF (WIN32)
    LIST(APPEND project_src_cpp)
ENDIF(WIN32)

ADD_EXECUTABLE(${PNAME} ${project_src_cpp} ${project_src_h} ${project_src_curr_cpp} ${project_src_curr_h} ${cfg_src_cpp} ${cfg_src_h} ${project_include})
target_link_libraries(${PNAME} Libssh2::libssh2)
