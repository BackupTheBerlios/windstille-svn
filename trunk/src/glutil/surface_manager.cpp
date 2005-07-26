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
#include "physfs/physfs_sdl.hpp"

SurfaceManager::SurfaceManager* surface_manager = 0;

SurfaceManager::SurfaceManager()
{
}

SurfaceManager::~SurfaceManager()
{
  for(Surfaces::iterator i = textures.begin(); i != textures.end(); ++i) {
    delete i->second;
  }
}

const Surface*
SurfaceManager::get(const std::string& filename)
{
  Surfaces::iterator i = textures.find(filename);
  if(i != textures.end())
    return i->second;

  SDL_Surface* image = IMG_Load_RW(get_physfs_SDLRWops(filename), 1);
  if(!image) {
    std::ostringstream msg;
    msg << "Couldn't load image '" << filename << "' :" << SDL_GetError();
    throw std::runtime_error(msg.str());
  }

  std::cerr << filename << " loaded.\n";
  Surface* result;
  try {
    result = create(image);
  } catch(std::exception& e) {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  SDL_FreeSurface(image);
  textures.insert(std::make_pair(filename, result));
  
  return result;
}

static int power_of_two(int val) {
  int result = 1;
  while(result < val)
    result *= 2;
  return result;
}

Surface*
SurfaceManager::create(SDL_Surface* image)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();
  
  int texture_w = power_of_two(image->w);
  int texture_h = power_of_two(image->h);

  GLint maxt;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxt);
  if(texture_w > maxt || texture_h > maxt)
      throw std::runtime_error("Texture too big");

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
    throw std::runtime_error("Couldn't create texture: out of memory");
  SDL_SetAlpha(image, 0, 0);
  SDL_BlitSurface(image, 0, convert, 0);

  GLuint handle;
  glGenTextures(1, &handle);

  SDL_PixelFormat* format = convert->format;

  glBindTexture(GL_TEXTURE_2D, handle);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, convert->pitch/format->BytesPerPixel);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
      convert->w, convert->h, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, convert->pixels);

  assert_gl("creating texture");

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);

  assert_gl("setting texture parameters");

  SDL_FreeSurface(convert);
  
  Surface* result = new Surface(Texture(handle), Rect(0, 0, image->w, image->h), image->w, image->h);
  return result;
}

