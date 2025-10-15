cmake_minimum_required(VERSION 3.10)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include_directories(${FAUST_INCLUDE_DIR})
add_library(${PROJECT_NAME} SHARED ${FAUST_CPP_FILE})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FAUST_LIBRARY} Ws2_32)