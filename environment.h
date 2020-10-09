#pragma once
#include <GL/glew.h>
#include "opengl_shader.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Environment {

    private:

    float triangles_vertices[3*4*6] = {
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  -1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, 1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
    };

    const unsigned int triangles_indices[36] = { 
         0, 1, 2, 2, 3, 0,
         4, 5, 6, 6, 7, 4, 
         8, 9, 10, 10, 11, 8,
         12, 13, 14, 14, 15, 12,
         16, 17, 18, 18, 19, 16,
         20, 21, 22, 22, 23, 20
    };

    GLuint vbo;
    GLuint vao;
    GLuint ebo;


    public:

    Environment() {

        GLuint vbo, vao, ebo;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangles_vertices), triangles_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangles_indices), triangles_indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        this->vbo = vbo;
        this->vao = vao;
        this->ebo = ebo;
    }



    void render(shader_t& shader, GLuint texture) {

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        shader.use();

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

};