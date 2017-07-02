project(web-game)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_CXX_FLAGS "-std=c++14")

#find_package(SDL2 REQUIRED)
add_subdirectory(deps/SDL2)


include_directories("src/")

#include_directories("deps/SDL2/include/")

#include_directories(${SDL2_INCLUDE_DIR})

file(GLOB_RECURSE SRC_LIST "src/*.cpp" "src/*.h")
add_executable(${PROJECT_NAME} ${SRC_LIST})
if(${EMSCRIPTEN})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_SDL=2")
endif()
target_link_libraries(${PROJECT_NAME} SDL2)

#file(COPY ./data DESINATION ${CMAKE_CURRENT_BINARY_DIR})
