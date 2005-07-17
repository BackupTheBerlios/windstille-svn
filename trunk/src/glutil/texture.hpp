#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <GL/gl.h>

class Texture
{
public:
  int orig_width;
  int orig_height;
  GLuint handle;

  ~Texture();

  /** returns the u texture-coordinate where the real image ends */
  float get_max_u() const;
  /** returns the v texture-coordinate where the real image ends */
  float get_max_v() const;

private:
  friend class TextureManager;
  Texture();
};

#endif

