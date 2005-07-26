#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <GL/gl.h>
#include "refcounter.hpp"

class Texture : public RefCounter
{
public:
  GLuint handle;

  Texture(GLuint handle);
  Texture() {}
  ~Texture();
};

#endif

