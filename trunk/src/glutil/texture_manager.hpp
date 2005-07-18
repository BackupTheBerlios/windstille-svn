#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#include <string>
#include <map>
#include <SDL.h>

#include <ClanLib/Display/pixel_buffer.h>

class Texture;

class TextureManager
{
public:
  TextureManager();
  ~TextureManager();

  /** returns a texture containing the image specified with filename
   * Note: You must NOT delete the returned Texture
   */
  const Texture* get(const std::string& filename);

private:
  Texture* create(SDL_Surface* surface);
  
  typedef std::map<std::string, Texture*> Textures;
  Textures textures;
};

extern TextureManager* texture_manager;

#endif

