#include <config.h>

#include "texture_manager.hpp"

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

class TextureManagerTexture : public Texture
{
public:
  TextureManagerTexture(GLuint handle)
    : Texture(handle)
  { }
  ~TextureManagerTexture()
  {
    if(!texture_manager)
      return;
      
    for(TextureManager::Textures::iterator i = texture_manager->textures.begin();
        i != texture_manager->textures.end(); ++i)
    {
      if(i->second == this)
      {
        texture_manager->textures.erase(i);
        return;
      }
    }
  }
};

Texture*
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

  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();                                                
  state.setup_2d();

  GLuint handle;
  try {
    handle = create_texture_from_surface(image);
  } catch(std::exception& e) {
    SDL_FreeSurface(image);
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());
  }

  SDL_FreeSurface(image);
  
  Texture* result = new TextureManagerTexture(handle);
  textures.insert(std::make_pair(filename, result));
  return result;
}

static inline bool is_power_of_2(int v)
{
  return (v & (v-1)) == 0;
}

GLuint
TextureManager::create_texture_from_surface(SDL_Surface* image)
{
  const SDL_PixelFormat* format = image->format;
  if(!is_power_of_2(image->w) || !is_power_of_2(image->h))
    throw std::runtime_error("image has no power of 2 size");
  if(format->BitsPerPixel != 24 && format->BitsPerPixel != 32)
    throw std::runtime_error("image has not 24 or 32 bit color depth");

  glEnable(GL_TEXTURE_2D);

  GLuint handle;
  glGenTextures(1, &handle);
  assert_gl("creating texture handle.");

  try {
    GLint maxt;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxt);
    if(image->w > maxt || image->h > maxt)
        throw std::runtime_error("Texture size not supported");

    glBindTexture(GL_TEXTURE_2D, handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, image->pitch/format->BytesPerPixel);
    glTexImage2D(GL_TEXTURE_2D, 0, format->BytesPerPixel,
        image->w, image->h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, image->pixels);

    assert_gl("creating texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);

    assert_gl("setting texture parameters");
  }
  catch(...)
  {
    glDeleteTextures(1, &handle);
  }

  return handle;
}

