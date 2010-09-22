/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "display/surface_manager.hpp"

#include <sstream>
#include <stdexcept>

#include "display/software_surface.hpp"
#include "display/texture_packer.hpp"


SurfaceManager::SurfaceManager() :
  texture_packer(0), // (new TexturePacker(Size(2048, 2048))),
  surfaces()
{
}

SurfaceManager::~SurfaceManager()
{
#if 0 
  if (debug)
  {
    for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
    {
      //      if (i->second.use_count() > 1)
      std::cerr << "Warning: Surface '" << i->first << "' not released.\n";
    }
  }
#endif
}

SurfacePtr
SurfaceManager::get(const Pathname& filename)
{
  Surfaces::iterator i = surfaces.find(filename);

  if(i != surfaces.end())
  { // Surface in cache, return it
    return i->second;
  }
  else
  {
    SoftwareSurface software_surface(filename);

    if (texture_packer)
    {
      SurfacePtr result = texture_packer->upload(software_surface);
      surfaces.insert(std::make_pair(filename, result));
      return result;              
    }
    else
    {
      float maxu = 0.0f;
      float maxv = 0.0f;
      TexturePtr texture;

      try
      {
        texture = create_texture(software_surface, &maxu, &maxv);
      }
      catch(std::exception& e)
      {
        std::ostringstream msg;
        msg << "Couldn't create texture for '" << filename << "': " << e.what();
        throw std::runtime_error(msg.str());
      }
        
      SurfacePtr result = Surface::create(texture, Rectf(0.0f, 0.0f, maxu, maxv),
                                          Sizef(static_cast<float>(software_surface.get_width()),
                                                static_cast<float>(software_surface.get_height())));
      surfaces.insert(std::make_pair(filename, result));
      return result;
    }
  }
}

void
SurfaceManager::load_grid(const Pathname& filename,
                          std::vector<SurfacePtr>& out_surfaces,
                          int width, int height)
{
  SoftwareSurface image(filename);
  float maxu, maxv;

  TexturePtr texture;

  try
  {                                                                       
    texture = create_texture(image, &maxu, &maxv);
  }
  catch(std::exception& e)
  {
    std::ostringstream msg;
    msg << "Couldn't create texture for '" << filename << "': " << e.what();
    throw std::runtime_error(msg.str());                                      
  }

  for(int y = 0; y <= image.get_height() - height + 1; y += height)
  {
    for(int x = 0; x <= image.get_width() - width + 1; x += width)
    {
      float s_min_u = maxu * static_cast<float>(x) / static_cast<float>(image.get_width());
      float s_min_v = maxv * static_cast<float>(x) / static_cast<float>(image.get_height());
      float s_max_u = (maxu * (static_cast<float>(x + width)))  / static_cast<float>(image.get_width());
      float s_max_v = (maxv * (static_cast<float>(x + height))) / static_cast<float>(image.get_height());

      out_surfaces.push_back(Surface::create(texture, 
                                             Rectf(s_min_u, s_min_v, s_max_u, s_max_v), 
                                             Sizef(static_cast<float>(width),
                                                   static_cast<float>(height))));
    }
  }
}

TexturePtr
SurfaceManager::create_texture(const SoftwareSurface& image,
                               float* maxu, float* maxv)
{
  // OpenGL2.0 should be fine with non-power-of-two
  int texture_w = image.get_width();  //math::round_to_power_of_two(image.get_width());
  int texture_h = image.get_height(); //math::round_to_power_of_two(image.get_height());

  SoftwareSurface convert(texture_w, texture_h);

  image.blit(convert, 0, 0);

  TexturePtr texture = Texture::create(convert);
  
  *maxu = static_cast<float>(image.get_width())  / static_cast<float>(texture_w);
  *maxv = static_cast<float>(image.get_height()) / static_cast<float>(texture_h);

  return texture;
}

void
SurfaceManager::cleanup()
{
  //std::cout << "SurfaceManager: size: " << surfaces.size() << std::endl;
  for(Surfaces::iterator i = surfaces.begin(); i != surfaces.end(); ++i)
  {
    if (i->second.use_count() == 1)
    {
      //std::cout << "SurfaceManager: erasing a surface" << std::endl;
      surfaces.erase(i);
    }
  }
}

void
SurfaceManager::save_all_as_png() const
{
  texture_packer->save_all_as_png();
}

/* EOF */
