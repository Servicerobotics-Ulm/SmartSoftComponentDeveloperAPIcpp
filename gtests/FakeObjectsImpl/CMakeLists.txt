CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(FakeObjectsImpl)

FILE(GLOB SRCS ${PROJECT_SOURCE_DIR}/*.cpp)

ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRCS})
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} SmartSoft_CD_API)

# define include paths for the target
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)