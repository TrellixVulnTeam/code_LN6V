cmake_minimum_required(VERSION 2.8)

#ENV VARs
SET(QUEUE_DIR ${CMAKE_HOME_DIRECTORY}/../queue)

#INCLUDE DIR
INCLUDE_DIRECTORIES(${QUEUE_DIR}/src)

#SOURCE DIR
AUX_SOURCE_DIRECTORY(${QUEUE_DIR}/src SOURCE_QUEUE)

#LINK LIBRARY
SET(LIBRARIES -lpthread)

# DEBUG AND RELEASE
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++11 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -DOPEN_CORE -O0 -w -g")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -D_FILE_OFFSET_BITS=64 -D_LARGE_FILE -O3 -g")


IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    MESSAGE(STATUS "Debug Mode")
    MESSAGE(STATUS "Flags:" ${CMAKE_CXX_FLAGS_DEBUG} )
    ADD_DEFINITIONS(-DDEBUG)
    SET(BIN_POSTFIX "d")
ELSE (CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET (CMAKE_BUILD_TYPE "Release" CACHE STRING "build type" FORCE)
    MESSAGE(STATUS "Release Mode")
    MESSAGE(STATUS "Flags:" ${CMAKE_CXX_FLAGS_RELEASE})
    SET(BIN_POSTFIX "")
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Debug")

# EXECUTE
ADD_EXECUTABLE(Safe_queue_test${BIN_POSTFIX}
    ${SOURCE_QUEUE}
    )
TARGET_LINK_LIBRARIES(Safe_queue_test${BIN_POSTFIX}
    ${LIBRARIES})

SET(EXECUTABLE_OUTPUT_PATH ./)
