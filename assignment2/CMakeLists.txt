project(raytracer)

file(GLOB SRC_LIST "*.cpp")

add_executable(${PROJECT_NAME} ${SRC_LIST})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)