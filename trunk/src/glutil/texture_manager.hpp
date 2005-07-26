#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <string>
#include <map>
#include <SDL.h>
#include <GL/gl.h>

class Texture;

/**
 * This class manages a map of image files to OpenGL textures.
 */
class TextureManager
{
public:
  TextureManager();
  ~TextureManager();

  /**
   * returns a texture containing the image specified with filename
   * The TextureManager keeps track of loaded Textures and might return a
   * reference to an existing texture.
   * Only textures with power of 2 dimensions are supported here. Use
   * SurfaceManager for images with other dimensions.
   * Note: Texture is a refcounted class, store it with Ref<Texture>
   */
  Texture* get(const std::string& filename);

  /**
   * Upload an SDL_Surface onto an OpenGL texture. The surface must have power
   * of 2 dimensions
   * */
  static GLuint create_texture_from_surface(SDL_Surface* surface);

private:
  friend class TextureManagerTexture;
  
  typedef std::map<std::string, Texture*> Textures;
  Textures textures;
};

extern TextureManager* texture_manager;

#endif

