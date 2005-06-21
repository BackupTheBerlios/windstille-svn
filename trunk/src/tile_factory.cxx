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
      
              if (strcmp(lisp_symbol(name), "tiles") == 0)
                {
                  parse_tiles(data);
                }
              else
                {
                  std::cout << "TileFactory: Unknown tag: " << lisp_symbol(name) << std::endl;
                }
            }
          else
            {
              std::cout << "TileFactory: Not a pair!"  << std::endl;
            }
          tree = lisp_cdr(tree);
        }
    }

  lisp_free(tree);
}

void
TileFactory::parse_tiles(lisp_object_t* data)
{
  assert(data);
  LispReader reader(data);

  int id = 1;

  if (!reader.read_int("id", &id))
    {
      std::cout << "Error: Id tag missing" << std::endl;
      return;
    }

  std::string filename;
  if (!reader.read_string("color-image", &filename))
    {
      std::cout << "Error: image tag missing" << std::endl;
      return;
    }

  std::string highlight_filename;
  reader.read_string("highlight-image", &highlight_filename);

  std::vector<int> colmap;
  reader.read_int_vector("colmap", &colmap);
 
  CL_PixelBuffer image = CL_ProviderFactory::load(filename);
  CL_PixelBuffer hl_image;

  if (!highlight_filename.empty())
    hl_image = CL_ProviderFactory::load(highlight_filename);

  int num_tiles = (image.get_width()/TILE_SIZE) * (image.get_height()/TILE_SIZE);

  if (int(colmap.size()) != num_tiles)
    {
      std::cout << "Error: TileFactor: not enough colmap information for tiles" << std::endl;
    }

  if ((id + num_tiles) >= int(tiles.size()))
    {
      tiles.resize(id + num_tiles + 1);
    }

  // FIMXE: Tiles should share one OpenGL texture
  for (int y = 0; y < image.get_height(); y += TILE_SIZE)
    {
      for (int x = 0; x < image.get_width(); x += TILE_SIZE)
        {
          CL_PixelBuffer chopped_image(TILE_SIZE, TILE_SIZE,
                                       image.get_format().get_depth()*TILE_SIZE,
                                       image.get_format(), NULL);
          chopped_image.lock();
          image.convert(chopped_image.get_data(), 
                        chopped_image.get_format(), 
                        image.get_format().get_depth()*TILE_SIZE, 
                        CL_Rect(CL_Point(0, 0), CL_Size(TILE_SIZE, TILE_SIZE)),
                        CL_Rect(CL_Point(x, y), CL_Size(TILE_SIZE, TILE_SIZE)));
          chopped_image.unlock();

          CL_PixelBuffer hl_chopped_image;

          if (hl_image)
            {
              hl_chopped_image = CL_PixelBuffer(TILE_SIZE, TILE_SIZE,
                                                hl_image.get_format().get_depth()*TILE_SIZE,
                                                hl_image.get_format(), NULL);
              hl_chopped_image.lock();
              hl_image.convert(hl_chopped_image.get_data(), 
                               hl_chopped_image.get_format(), 
                               hl_image.get_format().get_depth()*TILE_SIZE, 
                               CL_Rect(CL_Point(0, 0), CL_Size(TILE_SIZE, TILE_SIZE)),
                               CL_Rect(CL_Point(x, y), CL_Size(TILE_SIZE, TILE_SIZE)));
              hl_chopped_image.unlock();
            }

          tiles[id] = new Tile(chopped_image, hl_chopped_image,
                               colmap[y/TILE_SIZE * image.get_width()/TILE_SIZE + x/TILE_SIZE]);
          tiles[id]->id = id;
          id += 1;
        }
    }
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
