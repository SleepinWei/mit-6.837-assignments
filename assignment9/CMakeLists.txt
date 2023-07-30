project(particle_system)

file(GLOB SRC_LIST *.cpp) 
# ${PROJECT_SOURCE_DIR}/include/*.h)

# from the docs, this gives you `OpenGL::GL`
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_LIST})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/external)

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib/libfreeglut.lib)
target_link_libraries(${PROJECT_NAME} PUBLIC "-lopengl32 -lwinmm -lgdi32" ${OPENGL_glu_LIBRARY})