#include <config.h>

#include "surface_manager.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <SDL_image.h>
#include <ClanLib/GL/opengl_state.h>
#include <ClanLib/Display/display.h>
#include <ClanLib/Display/display_window.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "util.hpp"
#include "globals.hpp"
#include "texture.hpp"
#include "surface.hpp"
#include "texture_manager.hpp"
#include "physfs/physfs_sdl.hpp"

SurfaceManager::SurfaceManager* surface_manager = 0;

SurfaceManager::SurfaceManager()
{
}

SurfaceManager::~SurfaceManager()
{
  if(debug)
  {
    for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
    {
      std::cerr << "Warning: Surface '" << i->first << "' not released.\n";
    }
  }
}

Surface*
SurfaceManager::get(const std::string& filename)
{
  Surfaces::iterator i = surfaces.find(filename);
  if(i != surfaces.end())
    return i->second;

  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  float maxu, maxv;
  GLuint handle;
  try
  {
    create_texture(image, handle, maxu, maxv);
  }
  catch(std::exception& e)
  {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  Surface* result = new Surface();
  result->texture = handle;
  result->width = image->w;
  result->height = image->h;
  float* texcoords = result->texcoords;
  texcoords[0] = 0;
  texcoords[1] = 0;
  texcoords[2] = maxu;
  texcoords[3] = 0;
  texcoords[4] = maxu;                                                      
  texcoords[5] = maxv;
  texcoords[6] = 0;
  texcoords[7] = maxv;

  SDL_FreeSurface(image);
  surfaces.insert(std::make_pair(filename, result));
  
  return result;
}

void
SurfaceManager::load_grid(const std::string& filename,
                          std::vector< Ref<Surface> >& surfaces,
                          int width, int height)
{
  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image)
  {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  float maxu, maxv;
  GLuint handle;
  try
  {                                                                       
    create_texture(image, handle, maxu, maxv);
  }
  catch(std::exception& e)
  {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());                                      
  }

  for(int y = 0; y <= image->h - height + 1; y += height)
  {
    for(int x = 0; x <= image->w - width + 1; x += width)
    {
      Surface* surface = new Surface();
      surface->texture = handle;
      surface->width = width;
      surface->height = height;
      
      float s_min_u = maxu * x / static_cast<float>(image->w);
      float s_max_u = (maxu * (x + width)) / static_cast<float>(image->w);
      float s_min_v = maxv * x / static_cast<float>(image->h);
      float s_max_v = (maxv * (x + height)) / static_cast<float>(image->h);
      float* texcoords = surface->texcoords;
      texcoords[0] = s_min_u;
      texcoords[1] = s_min_v;
      texcoords[2] = s_max_u;
      texcoords[3] = s_min_v;
      texcoords[4] = s_max_u;
      texcoords[5] = s_max_v;
      texcoords[6] = s_min_u;
      texcoords[7] = s_max_v;
      surfaces.push_back(surface);  
    }
  }
  SDL_FreeSurface(image);
}

static int power_of_two(int val) {
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

void
SurfaceManager::create_texture(SDL_Surface* image, GLuint& handle,
                               float& maxu, float& maxv)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  int texture_w = power_of_two(image->w);
  int texture_h = power_of_two(image->h);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
      texture_w, texture_h, 32,
      0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
      texture_w, texture_h, 32,
      0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  if(convert == 0) 
  {
    throw std::runtime_error("Couldn't create texture: out of memory");
  }
  SDL_SetAlpha(image, 0, 0);
  SDL_BlitSurface(image, 0, convert, 0);

  try 
  {
    handle = TextureManager::create_texture_from_surface(convert);
  }
  catch(...) 
  {
    SDL_FreeSurface(convert);
    throw;
  }
  SDL_FreeSurface(convert);
  
  maxu = static_cast<float>(image->w)/static_cast<float>(texture_w);
  maxv = static_cast<float>(image->h)/static_cast<float>(texture_h);
}

