#pragma once
#include <string>
#include <array>
#include <GL/glew.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
	

class CubemapTextureLoader {

    private:

    CubemapTextureLoader() { }

    public:
 
    static GLuint load(const std::array<std::string, 6>&  files) {

        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

        int width, height, nrChannels;
        unsigned char *data; 
        GLuint index = 0;
        for (auto& file : files) {
            data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
            if (data != nullptr) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + index, 0, GL_RGB,
                                 width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            } else {
                throw std::runtime_error("error! can't load file: " + file);
            }
            ++index;
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture_id;
    }

};

class TextureLoader {

    private:

    TextureLoader() { }

    public:
 
    static GLuint load(const std::string& file) {

        GLuint texture_id;

        int width, height, nrChannels;
        unsigned char *data= stbi_load(file.c_str(), &width, &height, &nrChannels, STBI_rgb);

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return texture_id;
    }

};


