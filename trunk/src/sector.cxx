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
#include <sstream>
#include <stdexcept>
#include "lisp/list_iterator.hpp"
#include "lisp/parser.hpp"
#include "globals.hxx"
#include "display/scene_context.hxx"
#include "tile_map.hxx"
#include "game_object.hxx"
#include "player.hxx"
#include "trigger.hxx"
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
  std::auto_ptr<lisp::Lisp> root(lisp::Parser::parse(filename));

  const lisp::Lisp* sector = root->get_lisp("windstille-sector");
  if(!sector) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }

  std::vector<std::string> scripts;
  std::vector<int> ambient_colors;
  
  lisp::ListIterator iter(sector);
  while(iter.next()) {
    if(iter.item() == "name") {
      name = iter.value().get_string();
    } else if(iter.item() == "scripts") {
      iter.value().get_vector(scripts);
    } else if(iter.item() == "ambient-color") {
      iter.lisp()->get_vector(ambient_colors);
      if(ambient_colors.size() != 3)
        throw std::runtime_error(
            "ambient-color contains has to contain exactly 3 values");
      ambient_light 
        = CL_Color(ambient_colors[0], ambient_colors[1], ambient_colors[2]);
    } else if(iter.item() == "objects") {
      lisp::ListIterator oiter(iter.lisp());
      while(oiter.next()) {
        parse_object(oiter.item(), oiter.lisp());
      }
    } else {
      std::cerr << "Skipping unknown tag '" << iter.item() << "' in sector\n";
    }
  }
}

void
Sector::parse_object(const std::string& name, const lisp::Lisp* lisp)
{
  if(name == "tilemap") {
    TileMap* tilemap = new TileMap(lisp);
    objects.push_back(tilemap);
    if (tilemap->get_name() == "interactive")
      interactive_tilemap = tilemap;
  } else if(name == "background") {
    // TODO
  } else if(name == "trigger") {
    Trigger* trigger = new Trigger(lisp);
    objects.push_back(trigger);
  } else {
    std::cout << "Skipping unknown Object: " << name << "\n";
  }
}

void
Sector::draw(SceneContext& sc)
{
  sc.light().fill_screen(ambient_light);

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      (*i)->draw(sc);
    }
}

void
Sector::update(float delta)
{
  Objects tmpobjects = objects;
  for(Objects::iterator i = tmpobjects.begin(); i != tmpobjects.end(); ++i)
    {
      (*i)->update(delta);
    }
}

void
Sector::add(GameObject* obj)
{
  // FIXME: This is not save to call in update(), should be changed accordingly
  objects.push_back(obj);
}

void
Sector::remove(GameObject* obj)
{
  (void) obj;
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
