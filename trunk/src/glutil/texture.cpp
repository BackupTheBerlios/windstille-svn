#include "texture.hpp"

Texture::Texture(GLuint handle_)
  : handle(handle_)
{
}

Texture::~Texture()
{
  //glDeleteTextures(1, &handle);
}

