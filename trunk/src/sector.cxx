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

Sector::Sector(const std::string& filename)
{
  parse_file(filename);
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

  if (strcmp(lisp_symbol(lisp_car(tree)), "windstille-sector") != 0)
    {
      std::cout << filename << ": not a Windstille Sector file, type='" << lisp_symbol(lisp_car(tree)) << "'!" << std::endl;
    }
  else
    {
      LispReader reader(lisp_cdr(tree));

      reader.read_string("name",  &name);
      
      std::vector<std::string> scripts;
      reader.read_string_vector("scripts", &scripts);
      
      //parse_foreground_tilemap(reader.get("interactive-tilemap"));
      //parse_background_tilemap(reader.get("background-tilemap"));
    }

  lisp_free(tree);
}

void
Sector::draw(SceneContext& sc)
{
  // FIXME: Make baselight configurable
  sc.light().fill_screen(CL_Color(100, 100, 200));

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
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
  
}

void
Sector::remove(GameObj* obj)
{
  
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
