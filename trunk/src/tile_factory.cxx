//  $Id: tile_factory.cxx,v 1.10 2003/09/22 18:37:05 grumbel Exp $
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <string>
#include <ClanLib/Core/System/system.h>
#include <ClanLib/Display/pixel_buffer.h>
#include <ClanLib/Display/pixel_format.h>
#include <ClanLib/Display/Providers/provider_factory.h>
#include <assert.h>
#include <iostream>
#include "globals.hxx"
#include "tile.hxx"
#include "tile_factory.hxx"

extern CL_ResourceManager* resources;

TileFactory* TileFactory::current_ = 0;

std::string TileFactory::tile_def_file = "tuxtiles.scm";

TileFactory::TileFactory (const std::string& filename)
{
  lisp_object_t* tree = lisp_read_from_file(filename.c_str());
  
  if (!(lisp_symbol_p(lisp_car(tree)) && 
        strcmp("windstille-tiles", lisp_symbol(lisp_car(tree))) == 0))
    {
      std::cout << "Not a Windstille Tile File!" << std::endl;
    }
  else
    {
      tree = lisp_cdr(tree);

      while (!lisp_nil_p(tree))
        {
          lisp_object_t* current = lisp_car(tree);
          
          if (lisp_cons_p(current))
            {
              lisp_object_t* name    = lisp_car(current);
              lisp_object_t* data    = lisp_cdr(current);
      
              if (strcmp(lisp_symbol(name), "tile") == 0)
                {
                  parse_tile(data);
                }
              else if (strcmp(lisp_symbol(name), "tiles") == 0)
                {
                  parse_tiles(data);
                }
              else
                {
                  std::cout << "WindstilleLevel: Unknown tag: " << lisp_symbol(name) << std::endl;
                }
            }
          else
            {
              std::cout << "WindstilleLevel: Not a pair!"  << std::endl;
            }
          tree = lisp_cdr(tree);
        }
    }

  lisp_free(tree);
}

void
TileFactory::parse_tiles(lisp_object_t* data)
{
  LispReader reader(data);

  int id = 1;

  if (!reader.read_int("id", &id))
    {
      std::cout << "Error: Id tag missing" << std::endl;
      return;
    }

  std::string filename;
  if (!reader.read_string("image", &filename))
    {
      std::cout << "Error: image tag missing" << std::endl;
      return;
    }

  std::vector<unsigned char> colmap;
  if (!reader.read_unsigned_char_vector("colmap", &colmap))
    {
      // fill with default
      colmap.push_back(255);
      colmap.push_back(255);
      colmap.push_back(255);
      colmap.push_back(255);

      colmap.push_back(255);
      colmap.push_back(255);
      colmap.push_back(255);
      colmap.push_back(255);
    }
  
  if (colmap.size() != 8)
    {
      std::cout << "Error: Size does not match: " << colmap.size() << std::endl;
    }

  CL_PixelBuffer* image = CL_ProviderFactory::load(filename);

  int num_tiles = (image->get_width()/64) * (image->get_height()/64);

  if ((id + num_tiles) >= int(tiles.size()))
    {
      tiles.resize(id + num_tiles + 1);
    }

  for (int y = 0; y < image->get_height(); y += TILE_SIZE)
    {
      for (int x = 0; x < image->get_width(); x += TILE_SIZE)
        {
          CL_PixelBuffer chopped_image(TILE_SIZE, TILE_SIZE,
                                       image->get_format().get_depth()*TILE_SIZE,
                                       image->get_format(), NULL);
          chopped_image.lock();
          image->convert(chopped_image.get_data(), 
                         chopped_image.get_format(), 
                         image->get_format().get_depth()*TILE_SIZE, 
                         CL_Rect(CL_Point(0, 0), CL_Size(TILE_SIZE, TILE_SIZE)),
                         CL_Rect(CL_Point(x, y), CL_Size(TILE_SIZE, TILE_SIZE)));
          chopped_image.unlock();

          std::cout << "id: " << id << " " << x << "x" << y << std::endl;

          tiles[id] = new Tile(chopped_image, 
                               CL_Color(255, 255, 255),
                               CL_Color(127, 127, 127), &*colmap.begin());
          tiles[id]->id = id;
          id += 1;
        }
    }

  delete image;
}

void
TileFactory::parse_tile(lisp_object_t* data)
{
  // FIXME: Move this to scripting and add a TileFactory::add()
  int id = 1;
  std::string image;
  CL_Color color(255, 255, 255, 255);
  CL_Color attribute_color(255, 255, 255, 255);
  unsigned char colmap[8];
  
  while (!lisp_nil_p(data))
    {
      lisp_object_t* current = lisp_car(data);
          
      if (lisp_cons_p(current))
        {
          lisp_object_t* name    = lisp_car(current);
          lisp_object_t* data    = lisp_cdr(current);

          if (strcmp(lisp_symbol(name), "id") == 0)
            {
              id = lisp_integer(lisp_car(data));
            }
          else if (strcmp(lisp_symbol(name), "color") == 0)
            {
              color = CL_Color(lisp_integer(lisp_list_nth(data, 0)),
                               lisp_integer(lisp_list_nth(data, 1)),
                               lisp_integer(lisp_list_nth(data, 2)),
                               lisp_integer(lisp_list_nth(data, 3)));
            }
          else if (strcmp(lisp_symbol(name), "attribute-color") == 0)
            {
              attribute_color = CL_Color(lisp_integer(lisp_list_nth(data, 0)),
                                         lisp_integer(lisp_list_nth(data, 1)),
                                         lisp_integer(lisp_list_nth(data, 2)),
                                         lisp_integer(lisp_list_nth(data, 3)));
            }
          else if (strcmp(lisp_symbol(name), "image") == 0)
            {
              image = lisp_string(lisp_car(data));
            }
          else if (strcmp(lisp_symbol(name), "colmap") == 0)
            {
              colmap[0] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[1] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[2] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[3] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[4] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[5] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[6] = lisp_integer(lisp_car(data));
              data = lisp_cdr(data);
              colmap[7] = lisp_integer(lisp_car(data));
            }
        }
      data = lisp_cdr(data);
    }

  if (0) // Debugging code
    {
      std::cout << "Tile: id     = " << id << "\n"
                << "      image  = " << image << "\n"
                << "      colmap = " 
                << int(colmap[0]) << ", "
                << int(colmap[1]) << ", "
                << int(colmap[2]) << ", "
                << int(colmap[3]) << ", "
                << int(colmap[4]) << ", "
                << int(colmap[5]) << ", "
                << int(colmap[6]) << ", "
                << int(colmap[7])
                << std::endl;
    }

  if (id >= int(tiles.size()))
    {
      tiles.resize(id+1);
    }
  tiles[id] = new Tile(image, color, attribute_color, colmap);
  tiles[id]->id = id;
}

Tile* 
TileFactory::create (int id)
{
  // FIXME: this might cause throuble for some games
  // id 0 is always the empty tile
  if (id == 0)
    { 
      return 0;
    }
  else
    {
      if (id > 0 && id < int(tiles.size()))
        return tiles[id];
      else
        return 0;
    }
}

void
TileFactory::init()
{
  assert(current_ == 0);
  current_ = new TileFactory(datadir + tile_def_file);
}

/** Destroy the default TileFactor*/
void
TileFactory::deinit()
{
  delete current_;
}

/* EOF */
