cmake_minimum_required(VERSION 2.8)
project(fc16)

file(GLOB_RECURSE fc16_SRC 
    "src/*.cpp")

include_directories("inc")

FIND_PACKAGE(Boost)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

ADD_DEFINITIONS("-std=c++0x")

add_executable(fc16 ${fc16_SRC})