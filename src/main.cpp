/*
Copyright (C) 2017 Kai-Uwe Zimdars

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chip8.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

// 3 floats positions, 2 floats texture coordinates
static const GLfloat quadBufferData[] = {
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f
};

static const GLuint quadIndices[] = {
    0, 1, 2,
    1, 3, 2
};

std::unique_ptr<uint32_t[]> ConvertToRGBA(uint8_t* display)
{
    std::unique_ptr<uint32_t[]> convertedDisplay(new uint32_t[64 * 32]);

    int k = 0;

    for (int i = 0; i < 256; i++)
    {
        for (uint8_t j = 0b10000000; j != 0; j = j >> 1)
        {
            if ((display[i] & j) != 0)
                convertedDisplay[k] = (uint32_t)0x00FFFFFF;
            else
                convertedDisplay[k] = (uint32_t)0x00000000;

            k++;
        } 
    }

    return convertedDisplay;
}

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
        
    GLFWwindow* window = glfwCreateWindow(640, 320, "CHIP8-OpenGL", NULL, NULL);
    if (!window)
    {
        std::cerr << "INIT ERROR: Could not initialize window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSwapInterval(1);
    
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

    GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadBufferData), quadBufferData, GL_STATIC_DRAW);

    GLuint eboID;
    glGenBuffers(1, &eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    std::string vertexCode, fragmentCode;
    std::ifstream vertexFile, fragmentFile;
    vertexFile.open("res/main.vshader");
    fragmentFile.open("res/main.fshader");
    std::stringstream vertexStream, fragmentStream;
    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();
    vertexFile.close();
    fragmentFile.close();
    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();

    const char* vertexCodeC = vertexCode.c_str();
    const char* fragmentCodeC = fragmentCode.c_str();

    GLuint vertexID, fragmentID;
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexCodeC, NULL);
    glCompileShader(vertexID);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
        std::cout << "Vertex Shader compilation failed: " << std::endl << infoLog << std::endl;
    }

    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentCodeC, NULL);
    glCompileShader(fragmentID);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
        std::cout << "Fragment Shader compilation failed: " << std::endl << infoLog << std::endl;
    }

    GLuint shaderID;
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexID);
    glAttachShader(shaderID, fragmentID);
    glLinkProgram(shaderID);

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);

    glUseProgram(shaderID);

    glUniform1i(glGetUniformLocation(shaderID, "display"), 0);

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    Chip8 chip;
    chip.Tick();

    auto imageData = ConvertToRGBA(chip.GetDisplay());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.get());

    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0)
        {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            lastTime += 1.0;
        }
        
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        chip.Tick();
        //if (chip.DisplayShouldUpdate())
        //{
            imageData = ConvertToRGBA(chip.GetDisplay());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.get());
        //}

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}