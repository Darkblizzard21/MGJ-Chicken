#include "Texture.h"

#include <glad/glad.h>

#include <cassert>
#include <filesystem>
#include <iostream>

#include "stb_image.h"

std::string resolvePath(const std::string& texture)
{
    auto basePath = std::filesystem::absolute(".");

    std::cout << "Loading rescource: " << texture << " from basepath: " << basePath << std::endl;

    std::vector<std::string> locations = {"", "assets", "../assets", "../../assets", "../../../assets"};
    for (const auto& loc : locations) {
        auto currentPath =  std::filesystem::absolute(basePath / loc / texture);
        if (std::filesystem::exists(currentPath)) {
            return currentPath.string();
        }
    }
    std::cout << "Could not resolve path for rescource: " << texture << " from basepath: " << basePath << std::endl;
    throw "";
}

Texture::Texture(const std::string& texture)
{
    // Generate new texture
    glGenTextures(1, &id);

    glBindTexture(GL_TEXTURE_2D, id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture mipmaps
    std::string    path = resolvePath(texture);
    int            width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        auto texFormat = GL_RGBA;
        if (nrChannels == 3) {
            texFormat = GL_RGB;
        }
        if (nrChannels == 2) {
            texFormat = GL_RG;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, texFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // free loaded texture to save memmory and prevent leak
    stbi_image_free(data);


}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::Bind(const unsigned int location)
{
    // at most 16 textures are supported by openGL
    assert(location < 16);
    // activate the texture unit first before binding texture
    glActiveTexture(GL_TEXTURE0 + location);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, id);
}
