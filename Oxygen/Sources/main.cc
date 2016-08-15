// Oxygen - main.cc
//
// Copyright (C) Jack Maloney 2016
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Based off Glitter. https://github.com/Polytonic/Glitter

// Local Headers
#include "glitter.h"
#include "oxygen.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

using namespace oxygen;

int main() {

    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow =
        glfwCreateWindow(mWidth, mHeight, "Oxygen", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    gladLoadGL();
    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    vao vertexArrayObject;
    vertexArrayObject.bind();

    GLuint vbo;
    glGenBuffers(1, &vbo);
    float vertices[] = {
        -0.5f, 0.5f,  1.0f,
        0.0f,  0.0f, // Top-left
        0.5f,  0.5f,  0.0f,
        1.0f,  0.0f, // Top-right
        0.5f,  -0.5f, 0.0f,
        0.0f,  1.0f, // Bottom-right
        -0.5f, -0.5f, 0.0f,
        0.5f,  0.5f // Bottom-left
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    GLuint elements[] = {0, 1, 2, 2, 3, 0};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
                 GL_STATIC_DRAW);

    const char* vertexSource = "#version 150\n"
                               "in vec2 in_position;\n"
                               "in vec3 in_color;\n"
                               "out vec3 color;\n"
                               "void main() {\n"
                               "gl_Position = vec4(in_position, 0.0, 1.0);\n"
                               "color = in_color; }\n";
    char buffer[512];
    shader vertexShader = shader(vertexSource, GL_VERTEX_SHADER);
    GLint status = vertexShader.compile(buffer, 512);
    handleShaderCompileErrors(status, buffer);

    const char* fragmentSource =
        "#version 150\n"
        "in vec3 color;\n"
        "out vec4 outColor;\n"
        "void main() { outColor = vec4(color, 1.0); }\n";
    shader fragmentShader = shader(fragmentSource, GL_FRAGMENT_SHADER);
    status = fragmentShader.compile(buffer, 512);
    handleShaderCompileErrors(status, buffer);

    program shaderProgram = program();
    shaderProgram.addShader(vertexShader);
    shaderProgram.addShader(fragmentShader);
    status = shaderProgram.link(buffer, 512);
    shaderProgram.use();
    handleShaderCompileErrors(status, buffer);

    shaderProgram.setAttributeArray("in_position", 2, GL_FLOAT, GL_FALSE,
                                    5 * sizeof(float), 0);

    shaderProgram.setAttributeArray("in_color", 3, GL_FLOAT, GL_FALSE,
                                    5 * sizeof(float),
                                    (void*) (2 * sizeof(float)));

    GLint uniColor = shaderProgram.getUniformLocation("color");

    std::cout << glGetError() << std::endl;

    // Rendering Loop
    while (glfwWindowShouldClose(mWindow) == false) {
        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(mWindow, true);
        }

        // Background Fill Color
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
    glDeleteBuffers(1, &vbo);
    glfwTerminate();
    return EXIT_SUCCESS;
}