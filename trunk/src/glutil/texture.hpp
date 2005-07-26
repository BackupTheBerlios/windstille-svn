#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <GL/gl.h>

class Texture
{
public:
  GLuint handle;

  Texture(GLuint handle);
  ~Texture();
};

#endif

