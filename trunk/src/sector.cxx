//  $Id$
//
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include "lispreader.hxx"
#include "globals.hxx"
#include "display/scene_context.hxx"
#include "tile_map.hxx"
#include "gameobj.hxx"
#include "sector.hxx"

Sector* Sector::current_ = 0;

Sector::Sector(const std::string& filename)
{
  current_ = this;
  interactive_tilemap = 0;
  ambient_light = CL_Color(0, 0, 0);
  parse_file(filename);
  if (!interactive_tilemap)
    std::cout << "Error: Sector: No interactive-tilemap available" << std::endl;
}

Sector::~Sector()
{
}

void
Sector::parse_file(const std::string& filename)
{
  if (debug)
    std::cout << "Windstille Sector: " << filename << std::endl;

  lisp_object_t* tree = lisp_read_from_file(filename.c_str());

  if (tree && strcmp(lisp_symbol(lisp_car(tree)), "windstille-sector") != 0)
    {
      std::cout << filename << ": not a Windstille Sector file, type='" << lisp_symbol(lisp_car(tree)) << "'!" << std::endl;
    }
  else
    {
      LispReader reader(lisp_cdr(tree));

      reader.read_string("name",  &name);
      
      std::cout << "Sector Name: " << name << std::endl;

      std::vector<std::string> scripts;
      reader.read_string_vector("scripts", &scripts);

      std::vector<int> ambient_colors;
      reader.read_int_vector("ambient-color", &ambient_colors);
      if (ambient_colors.size() == 3)
        {
          ambient_light = CL_Color(ambient_colors[0],
                                   ambient_colors[1],
                                   ambient_colors[2]);
        }

      lisp_object_t* objects_ptr = 0;
      if (reader.read_lisp("objects", &objects_ptr))
        {
          while(!lisp_nil_p(objects_ptr))
            {
              lisp_object_t* data = lisp_car(objects_ptr);
              if (lisp_cons_p(data) && lisp_symbol_p(lisp_car(data)))
                {
                  std::string ident = lisp_symbol(lisp_car(data));
                  std::cout << "Object: " << ident << std::endl;

                  if (ident == "tilemap")
                    {
                      TileMap* tilemap = new TileMap(LispReader(lisp_cdr(data)));

                      std::cout << "TileMap: " << tilemap->get_name() << std::endl;

                      objects.push_back(tilemap);
                      if (tilemap->get_name() == "interactive")
                        interactive_tilemap = tilemap;
                    }
                  else if (ident == "background")
                    {
                    }
                  else
                    {
                      std::cout << "Sector: Unknown ident: " << ident << std::endl;
                    }
                }

              objects_ptr = lisp_cdr(objects_ptr);
            }
        }

      //parse_foreground_tilemap(reader.get("interactive-tilemap"));
      //parse_background_tilemap(reader.get("background-tilemap"));
    }

  lisp_free(tree);
}

void
Sector::draw(SceneContext& sc)
{
  sc.light().fill_screen(ambient_light);

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      //std::cout << *i << std::endl;
      (*i)->draw(sc);
    }
}

void
Sector::update(float delta)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->update(delta);
    }
}

void
Sector::add(GameObj* obj)
{
  // FIXME: This is not save to call in update(), should be changed accordingly
  objects.push_back(obj);
}

void
Sector::remove(GameObj* obj)
{
  // not implemented
}

int
Sector::get_width () const
{
  return interactive_tilemap->get_width() * TILE_SIZE;
}

int
Sector::get_height () const
{
  return interactive_tilemap->get_height() * TILE_SIZE;
}

/* EOF */
