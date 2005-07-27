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
#include "lisp/properties.hpp"
#include "lisp/parser.hpp"
#include "windstille_getters.hpp"
#include "globals.hpp"
#include "display/scene_context.hpp"
#include "tile_map.hpp"
#include "game_object.hpp"
#include "player.hpp"
#include "trigger.hpp"
#include "flashing_sign.hpp"
#include "test_object.hpp"
#include "sector.hpp"
#include "spawnpoint.hpp"
#include "sound/sound_manager.hpp"
#include "script_manager.hpp"
#include "collision/collision_engine.hpp"
#include "particles/particle_system.hpp"
#include "test_object.hpp"
#include "elevator.hpp"
#include "nightvision.hpp"
#include "character.hpp"
#include "badguy/hedgehog.hpp"
#include "badguy/spider_mine.hpp"
#include "box.hpp"
#include "useable_object.hpp"

Sector* Sector::current_ = 0;

Sector::Sector(const std::string& filename)
  : player(0)
{
  if (debug) std::cout << "Creating new Sector" << std::endl;
  collision_engine = new CollisionEngine();

  current_ = this;
  interactive_tilemap = 0;
  parse_file(filename);
  if (!interactive_tilemap)
    throw std::runtime_error("No interactive-tilemap available");

  // add interactive to collision engine
  collision_engine->add (new CollisionObject (interactive_tilemap));
}

Sector::~Sector()
{
  for(SpawnPoints::iterator i = spawn_points.begin();
      i != spawn_points.end(); ++i)
    delete *i;                                         
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    (*i)->unref();
  for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i)
    (*i)->unref();

  delete collision_engine;
}

void
Sector::parse_file(const std::string& filename)
{
  if (debug) std::cout << "Parsing " << filename << std::endl;
  using namespace lisp;
  
  std::auto_ptr<Lisp> root(Parser::parse(filename));
  Properties rootp(root.get());

  const Lisp* sector = 0;
  if(!rootp.get("windstille-sector", sector)) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a windstille-sector file";
    throw std::runtime_error(msg.str());
  }
  rootp.print_unused_warnings("sector");
  
  Properties props(sector);

  int version = 1;
  if(!props.get("version", version))
    std::cerr << "Warning no version specified in levelformat.\n";
  if(version > 1)
    std::cerr << "Warning: format version is newer than game.\n";
  props.get("name", name);
  props.get("music", music);
  props.get("init-script", init_script);
  props.get("ambient-color", ambient_light);
  
  PropertyIterator<const Lisp*> iter;
  props.get_iter("spawnpoint", iter);
  while(iter.next()) {
    spawn_points.push_back(new SpawnPoint(*iter));
  }

  const Lisp* objects = 0;
  if(props.get("objects", objects) == false)
    throw std::runtime_error("No objects specified");
  Properties pobjects(objects);
  iter = pobjects.get_iter();
  while(iter.next()) {
    add_object(iter.item(), *iter);
  }

  props.print_unused_warnings("sector");
  if (debug) std::cout << "Finished parsing" << std::endl;
}

void
Sector::add_object(const std::string& name, const lisp::Lisp* lisp)
{
  if(name == "tilemap") {
    TileMap* tilemap = new TileMap(lisp);
    add(tilemap);
    if (tilemap->get_name() == "interactive")
      interactive_tilemap = tilemap;
  } else if(name == "background") {
    // TODO
  } else if(name == "trigger") {
    add(new Trigger(lisp));
  } else if(name == "box") {
    add_entity(new Box(lisp));
  } else if(name == "elevator") {
    add_entity(new Elevator(lisp));
  } else if(name == "flashing-sign") {
    add(new FlashingSign(lisp));
  } else if(name == "character") {    
    add(new Character(lisp));
  } else if(name == "spider_mine") {
    add(new SpiderMine(lisp));
  } else if(name == "hedgehog") {
    add(new Hedgehog(lisp));
  } else if(name == "test-object") {
    add(new TestObject(lisp));
  } else if (name == "nightvision") {
    add(new Nightvision(lisp));
  } else if (name == "particle-system") {
    add(new ParticleSystem(lisp));
  } else if(name == "useable-object") {    
    add(new UseableObject(lisp));
  } else {
    std::cout << "Skipping unknown Object: " << name << "\n";
  }
}

void
Sector::activate()
{
  commit_adds();
  commit_removes();

  sound_manager->play_music(music);
  script_manager->run_script_file(init_script);
}

void
Sector::spawn_player(const std::string& spawnpoint)
{
  const SpawnPoint* result = 0;
  for(SpawnPoints::iterator i = spawn_points.begin();
      i != spawn_points.end(); ++i) {
    const SpawnPoint* sp = *i;
    if(sp->name == spawnpoint) {
      result = sp;
      break;
    }
  }

  Vector spawnpos(320, 200);
  if(result == 0) {
    if(spawnpoint != "default") {
      std::cerr << "SpawnPoint '" << spawnpoint << "' not found.\n";
      spawn_player("default");
      return;
    }
  } else {
    spawnpos = result->pos;
  }

  if(!player) {
    player = new Player();
    add(player);
  }
  player->set_pos(spawnpos);
}

void
Sector::draw(SceneContext& sc)
{
  sc.light().fill_screen(ambient_light);

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if ((*i)->is_active())
        (*i)->draw(sc);
    }
}

void Sector::commit_adds()
{
  // Add new game objects
  for(Objects::iterator i = new_objects.begin(); i != new_objects.end(); ++i) {
    objects.push_back(*i);
  }
  new_objects.clear();
}

void Sector::update(float delta)
{
  commit_adds();

  collision_engine->update(delta);

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i) {
    if ((*i)->is_active())
      (*i)->update(delta);
  }
  commit_removes();
}

void
Sector::commit_removes()
{
  // remove objects
  for(Objects::iterator i = objects.begin(); i != objects.end(); ) {
    GameObject* object = *i;
    if(object->is_removable()) {
      if(object->get_name() != "") {
        remove_object_from_squirrel(object);
      }
      object->unref();

      i = objects.erase(i);
      continue;
    }

    ++i;
  }
}

void
Sector::add(GameObject* obj)
{
  new_objects.push_back(obj);
  obj->ref();
  if(obj->get_name() != "") {
    expose_object_to_squirrel(obj);
  }
}

void 
Sector::add_entity(Entity* ent)
{
  add(ent);
}


void
Sector::remove_object_from_squirrel(GameObject* object)
{
  script_manager->remove_object(object->get_name());
}

void
Sector::expose_object_to_squirrel(GameObject* object)
{
  // FIXME: Grumbel: I don't consider this brute-force exposing a good
  // idea, should be up to the scripter if we ones to keep a refrence
  // to an object or not
  FlashingSign* sign = dynamic_cast<FlashingSign*> (object);
  if(sign) {
    script_manager->expose_object(new Scripting::FlashingSign(sign),
                                  object->get_name(), true);
    return;
  }

  TestObject* tobj = dynamic_cast<TestObject*> (object);
  if(tobj) {
    script_manager->expose_object(new Scripting::TestObject(tobj),
                                  object->get_name(), true);
    return;
  }

  Player* player = dynamic_cast<Player*> (object);
  if(player) {
    script_manager->expose_object(new Scripting::Player(player),
                                  object->get_name(), true);
    return;
  }

  script_manager->expose_object(new Scripting::GameObject(object),
                                object->get_name(), true);
}

GameObject*
Sector::get_object(const std::string& name) const
{
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i) 
    {
      if ((*i)->get_name() == name)
        {
          return *i;
        }
    }
  return 0;
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

void
Sector::set_tilemap(TileMap* t)
{
  interactive_tilemap = t;
}

/* EOF */
