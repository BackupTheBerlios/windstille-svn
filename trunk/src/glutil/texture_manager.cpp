#include <config.h>

#include "texture_manager.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <SDL_image.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "util.hpp"
#include "globals.hpp"
#include "texture.hpp"
#include "physfs/physfs_sdl.hpp"

TextureManager::TextureManager* texture_manager = 0;

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
  if(debug)
  {
    for(Textures::iterator i = textures.begin(); i != textures.end(); ++i) {
      std::cerr << "Warning: Texture '" << i->first << "' not released.\n";
    }
  }
}

Texture
TextureManager::get(const std::string& filename)
{
  Textures::iterator i = textures.find(filename);
  if(i != textures.end())
    return i->second;

  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image) {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  Texture texture;
  try {
    texture = Texture(image);
  } catch(std::exception& e) {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  SDL_FreeSurface(image);
  
  textures.insert(std::make_pair(filename, texture));
  return texture;
}

/* EOF */
