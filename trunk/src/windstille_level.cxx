//  $Id: windstille_level.cxx,v 1.14 2003/11/13 12:59:42 grumbel Exp $
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

#include <assert.h>
#include <iostream>
#include "config.h"
#include "globals.hxx"
#include "windstille_level.hxx"
#include "string_converter.hxx"
#include "lispreader.hxx"
#include "scm_helper.hxx"

WindstilleLevel::WindstilleLevel (const std::string& filename)
  : tilemap(0),
    background_tilemap(0)
{
  width  = 50;
  height = 50;
  parse_file (filename);
}

void 
WindstilleLevel::parse_file (const std::string& filename)
{
  diamond_map = 0;

  if (debug)
    std::cout << "Windstille Level: " << filename << std::endl;

  lisp_object_t* tree = lisp_read_from_file(filename.c_str());

  if (strcmp(lisp_symbol(lisp_car(tree)), "windstille-level") != 0)
    {
      std::cout << filename << ": not a Windstille Level file, type='" << lisp_symbol(lisp_car(tree)) << "'!" << std::endl;
    }
  else
    {
      LispReader reader(lisp_cdr(tree));

      parse_properties(reader.get("properties"));
      //parse_foreground_tilemap(tree->getLisp("foreground-tilemap"));
      parse_foreground_tilemap(reader.get("interactive-tilemap"));
      parse_background_tilemap(reader.get("background-tilemap"));
      parse_water(reader.get("water"));
      parse_diamond_map(reader.get("diamond-map"));
    }

  if (!diamond_map)
    {
      std::cout << "No diamond map in level file" << std::endl;
      diamond_map = new Field<int>(width * 2, height * 2);
    }
}

void
WindstilleLevel::parse_water(lisp_object_t* tree)
{
  if (tree)
    {
      for(lisp_object_t* cur = tree; cur != 0; cur = lisp_cdr(cur))
        {
          // read (water x y w h)
        }
    }
}

void
WindstilleLevel::parse_properties (lisp_object_t* tree)
{
  if (tree)
    {
      LispReader reader(tree);

      reader.read_int("width",  &width);
      reader.read_int("height", &height);
      //tree->get("name",   name);
      reader.read_string_vector("scripts", &scripts);
    }

  if (debug)
    std::cout << "WindstilleLevel: dimensions: " << width << "x" << height << std::endl;
}

void
WindstilleLevel::parse_background_tilemap (lisp_object_t* cur)
{
  background_tilemap = parse_tilemap(cur);
}

void
WindstilleLevel::parse_foreground_tilemap (lisp_object_t* cur)
{
  tilemap = parse_tilemap(cur);
}

Field<int>* 
WindstilleLevel::parse_tilemap (lisp_object_t* cur)
{
  Field<int>* field = new Field<int>(width, height);
  
  LispReader reader(cur);
  reader.read_int_vector("data", &field->get_vector());

  if ((static_cast<int>(field->get_vector().size()) != width*height))
    {
      std::cout << "Error: Size is " << field->get_vector().size() 
                << " should be " << width*height << std::endl;
      assert(0);
    }

  return field;
}

void
WindstilleLevel::parse_diamond_map(lisp_object_t* data)
{
  return;
#if 0
  diamond_map = new Field<int>(width * 2, height * 2);

  for(Field<int>::iterator i = diamond_map->begin(); i != diamond_map->end(); ++i)
    {
      *i = false;
    }
  
  int x = 0;
  int y = 0;

  while (!gh_null_p(data) && y < height*2)
    {
      (*diamond_map)(x, y) = gh_scm2int(gh_car(data));
              
      x += 1;

      if (x >= width*2)
        {
          x = 0;
          y += 1;
        }
              
      data = gh_cdr(data);
    }

  if (y != height*2)
    std::cout << "WindstilleLevel: Something went wrong: y=" << y << " height=" << height << std::endl;
#endif
}

void
WindstilleLevel::parse_gameobjects (lisp_object_t* cur)
{
}

/* EOF */
