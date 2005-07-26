#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <string>
#include <map>
#include <SDL.h>

#include <ClanLib/Display/pixel_buffer.h>

class Surface;

class SurfaceManager
{
public:
  SurfaceManager();
  ~SurfaceManager();

  /** returns a texture containing the image specified with filename
   * Note: You must NOT delete the returned Texture
   */
  const Surface* get(const std::string& filename);

  Surface* create(SDL_Surface* surface);
  
  typedef std::map<std::string, Surface*> Surfaces;
  Surfaces textures;
};

extern SurfaceManager* surface_manager;

#endif

