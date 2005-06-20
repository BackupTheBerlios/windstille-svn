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
#include "globals.hxx"
#include "windstille_level.hxx"
#include "lispreader.hxx"

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

      reader.read_int("width",  &width);
      reader.read_int("height", &height);
      reader.read_string_vector("scripts", &scripts);

      //parse_foreground_tilemap(tree->getLisp("foreground-tilemap"));
      parse_foreground_tilemap(reader.get("interactive-tilemap"));
      parse_background_tilemap(reader.get("background-tilemap"));
      parse_water(reader.get("water"));
    }

  lisp_free(tree);
}

void
WindstilleLevel::parse_water(lisp_object_t* tree)
{
  if (tree)
    {
      for(lisp_object_t* cur = tree; cur != 0; cur = lisp_cdr(cur))
        {
#if 0
          // read (water x y w h)
          lisp_object_t* data = lisp_car(cur);
          
          lisp_integer(lisp_list_nth(data, 1));
          lisp_integer(lisp_list_nth(data, 2));
          lisp_integer(lisp_list_nth(data, 3));
          lisp_integer(lisp_list_nth(data, 4));
#endif
        }
    }
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
      std::cout << "WindstilleLevel: Error: Size is " << field->get_vector().size() 
                << " should be " << width*height << std::endl;
      assert(0);
    }

  return field;
}

void
WindstilleLevel::parse_gameobjects (lisp_object_t* cur)
{
}

/* EOF */
