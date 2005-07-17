#include "texture.hpp"

Texture::Texture()
{
}

Texture::~Texture()
{
  glDeleteTextures(1, &handle);
}

static int power_of_two(int val) {
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

float
Texture::get_max_u() const
{
  int newwidth = power_of_two(orig_width);
  return static_cast<float>(orig_width)/static_cast<float>(newwidth);
}

float Texture::get_max_v() const
{
  int newheight = power_of_two(orig_height);
  return static_cast<float>(orig_height)/static_cast<float>(newheight);
}

