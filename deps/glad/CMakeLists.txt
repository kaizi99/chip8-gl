# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 2.8)

# Your project's name
project(glad)

include_directories(include)
add_library(${PROJECT_NAME} src/glad.c)
target_include_directories(${PROJECT_NAME} INTERFACE include)