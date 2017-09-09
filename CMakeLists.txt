# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 2.8)

# Your project's name
project(chip8-gl)

# Add all files from the source/ folder to CMake
file(GLOB_RECURSE SRC
    "src/*.cpp"
)

add_subdirectory(deps/chip8-lib)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)

add_subdirectory(deps/glad)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} glfw glad chip8-lib)

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
)