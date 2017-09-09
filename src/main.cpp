#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void error_callback(int error, const char* description)
{
    std::cerr << "GLFW ERROR: Code " << error << ": " << description << std::endl;
}

int main()
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        std::cerr << "INIT ERROR: Could not initialize GLFW" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        
    GLFWwindow* window = glfwCreateWindow(640, 480, "CHIP8-OpenGL", NULL, NULL);
    if (!window)
    {
        std::cerr << "INIT ERROR: Could not initialize window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}