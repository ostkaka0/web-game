project(web-game)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(GCC_COVERAGE_COMPILE_FLAGS "-s USE_SDL=2")
#SET(GCC_COVERAGE_LINK_FLAGS    "-s USE_SDL=2")

#find_package(SDL2 REQUIRED)
#set(glm_DIR /usr/share/cmake/glm)#find_package(glm REQUIRED)
add_subdirectory(deps/SDL2)
#add_subdirectory(deps/glm)

MESSAGE(${SDL2_SOURCE_DIR})

include_directories("src/" "${SDL2_SOURCE_DIR}/include" "deps/glm/")

#include_directories("deps/SDL2/include/")

#include_directories(${SDL2_INCLUDE_DIR})

file(GLOB_RECURSE SRC_LIST "src/*.h")
add_executable(${PROJECT_NAME} src/main.cpp ${SRC_LIST})
add_executable(${PROJECT_NAME}_cod_gen src/code_gen.cpp ${SRC_LIST})
if(${EMSCRIPTEN})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()
target_link_libraries(${PROJECT_NAME} SDL2)

#file(COPY ./data DESINATION ${CMAKE_CURRENT_BINARY_DIR})

