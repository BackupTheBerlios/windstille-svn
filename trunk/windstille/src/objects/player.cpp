/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include "objects/player.hpp"

#include <boost/bind.hpp>

#include "collision/collision_engine.hpp"
#include "collision/stair_contact.hpp"
#include "engine/sector.hpp"
#include "objects/grenade.hpp"
#include "objects/laser_pointer.hpp"
#include "objects/pistol.hpp"
#include "screen/game_session.hpp"
#include "tile/tile.hpp"
#include "tile/tile_map.hpp"

static const int MAX_ENERGY = 16;
static const float WALK_SPEED = 128.0;
static const float RUN_SPEED = 256.0;

Player::Player () 
  : sprite(),
    jumping(),
    bomb_placed(),
    hit_count(),
    energy(),
    c_object(),
    laser_pointer(),
    contact(),
    weapon(),
    state(STAND),
    jump_foot(),
    reload_time(),
    z_pos()
{
  sprite = Sprite3D(Pathname("models/characters/jane/jane.wsprite"));
  pos.x = 320;
  pos.y = 200;
  name = "player";

  jumping = false;
  energy = MAX_ENERGY;

  hit_count = 0.0f;
  sprite.set_action("Stand");

  // collision detection init
  c_object = new CollisionObject(this, Rectf(-15, -120, 15, 0));

  c_object->set_pos(pos);
  c_object->set_velocity(velocity);
  
  c_object->sig_collision().connect(boost::bind(&Player::collision, this, _1));

  Sector::current()->get_collision_engine()->add(c_object);

  z_pos = 100.0f;

  contact = 0;
  weapon.reset(new Pistol());
  laser_pointer = ((Pistol*)weapon.get())->laser_pointer;
}

Player::~Player()
{
}

void
Player::draw (SceneContext& sc)
{
  if (1)
    { // draw the 'stand-on' tile
      sc.highlight().fill_rect(Rect(Point(int(pos.x)/32 * 32, (int(pos.y)/32 + 1) * 32),
                                    Size(32, 32)),
                               Color(1.0f, 0.0f, 0.0f, 0.5f), 10000.0f);
    }

  sprite.draw(sc.color(), pos, z_pos);

  Entity* obj = find_useable_entity();
  if (obj)
    {
      // FIXME: Highlight layer is the wrong place for this
      std::string use_str = "[" + obj->get_use_verb() + "]";
      sc.highlight().draw(use_str, obj->get_pos().x, obj->get_pos().y - 150, 1000);
    }
  
  // Draw weapon at the 'Weapon' attachment point
  Sprite3D::PointID id = sprite.get_attachment_point_id("Weapon");
  sc.push_modelview();
  sc.translate(pos.x, pos.y);
  sc.mult_modelview(sprite.get_attachment_point_matrix(id));
  weapon->draw(sc);
  sc.pop_modelview();

  if (laser_pointer->is_active())
    {
      sc.push_modelview();
      sc.translate(pos.x, pos.y - 80);
      laser_pointer->draw(sc);
      sc.pop_modelview();
    }
}

void
Player::start_listening()
{
  if (state == STAND_TO_LISTEN || state == LISTEN)
    return;

  set_stand_to_listen(false);
}

void
Player::stop_listening()
{
  if (state != LISTEN && state != STAND_TO_LISTEN)
    return;
  
  set_stand_to_listen(true);
}

void
Player::update(float /*delta*/)
{
}

void 
Player::update(const Controller& controller, float delta)
{
  weapon->update(delta);

  if (laser_pointer->is_active())
    laser_pointer->update(delta);

  if (fabsf(controller.get_axis_state(X2_AXIS, false)) > 0.25f ||
      fabsf(controller.get_axis_state(Y2_AXIS, false)) > 0.25f)
    {
      float angle = atan2f(controller.get_axis_state(Y2_AXIS, false),
                           controller.get_axis_state(X2_AXIS, false));

      laser_pointer->set_active(true);
      laser_pointer->set_angle(angle);
    }
  else
    {
      laser_pointer->set_active(false);
    }

  if (GameSession::current()->is_active())
    {
      switch(state)
        {
          case STAND:
          case WALK:
            update_walk_stand(controller);
            break;
          case RUN:
            update_run(controller);
            break;
          case DUCKING:
            update_ducking(controller);
            break;
          case DUCKED:
            update_ducked(controller);
            break;
          case TURNAROUND:
            update_turnaround(controller);
            break;
          case STAND_TO_LISTEN:
            update_stand_to_listen(controller);
            break;
          case LISTEN:
            update_listen(controller);
            break;
          case JUMP_BEGIN:
            update_jump_begin(controller);
            break;
          case JUMP_AIR:
            update_jump_air(controller);
            break;
          case JUMP_LAND:
            update_jump_land(controller);
            break;
          case JUMP_UP_BEGIN:
            update_jump_up_begin(controller);
            break;
          case JUMP_UP_AIR:
            update_jump_up_air(controller);
            break;
          case JUMP_UP_LAND:
            update_jump_up_land(controller);
            break;
          case PULL_GUN:
            update_pull_gun(controller);
            break;
          case STAIRS_DOWN:
          case STAIRS_UP:
            update_stairs(controller, delta);
            break;
          default:
            assert(false);
            break;
        }
    }

  // fall down
  velocity.y += GRAVITY * delta;

  sprite.update(delta);

  c_object->set_velocity (velocity);

  pos = c_object->get_pos();
}

void
Player::set_stand()
{
  try_set_action("Stand");
  velocity = Vector2f(0, 0);
  state = STAND;
}

void
Player::update_walk_stand(const Controller& controller)
{
  if (controller.get_axis_state(Y_AXIS) > 0.5f)
    {
      TileMap* tilemap = Sector::current()->get_tilemap2();
      if (tilemap)
        {
          Point p(int(pos.x)/32, (int(pos.y)/32 + 1));
          unsigned int col = tilemap->get_pixel(p.x, p.y);

          if ((col & TILE_STAIRS) && ((get_direction() == WEST && (col & TILE_LEFT)) ||
                                      (get_direction() == EAST && (col & TILE_RIGHT))))
            {
              delete contact;
              contact = new StairContact(tilemap, p);

              std::cout << "Stair mode" << std::endl;
              state = STAIRS_DOWN;
              //c_object->get_check_domains() & (~CollisionObject::DOMAIN_TILEMAP));
              Sector::current()->get_collision_engine()->remove(c_object);
              z_pos = -10.0f;
              return;
            }
          else
            {
              set_ducking();
              return;
            }
        }
    }
  else if (controller.get_axis_state(Y_AXIS) < -0.5f)
    {
      TileMap* tilemap = Sector::current()->get_tilemap2();
      if (tilemap)
        {
          Point p(int(pos.x)/32 + ((get_direction() == WEST) ? -1 : +1), (int(pos.y)/32));
          unsigned int col = tilemap->get_pixel(p.x, p.y);

          if ((col & TILE_STAIRS) && ((get_direction() == EAST && (col & TILE_LEFT)) ||
                                      (get_direction() == WEST && (col & TILE_RIGHT))))
            {
              delete contact;
              contact = new StairContact(tilemap, p);

              state = STAIRS_UP;
              //c_object->get_check_domains() & (~CollisionObject::DOMAIN_TILEMAP));
              Sector::current()->get_collision_engine()->remove(c_object);
              z_pos = -10.0f;
              return;
            }
        }    
    }

  if (state == STAND)
    update_stand(controller);
  else
    update_walk(controller);
}

void
Player::update_stairs(const Controller& controller, float delta)
{
  assert(contact);

  if (controller.get_axis_state(X_AXIS) < -0.5f ||
      controller.get_axis_state(Y_AXIS) > 0.5f)
    {
      contact->advance(-WALK_SPEED * delta * 0.7f);
    }
  else if (controller.get_axis_state(X_AXIS) > 0.5f ||
           controller.get_axis_state(Y_AXIS) < -0.5f)
    {
      contact->advance(WALK_SPEED * delta * 0.7f);
    }

  velocity = Vector2f(0, 0);
  c_object->set_pos(contact->get_pos());

  if (!contact->is_active())
    {
      Sector::current()->get_collision_engine()->add(c_object);
      state = STAND;
    }
}

Entity*
Player::find_useable_entity()
{
  const std::vector<boost::shared_ptr<GameObject> >& objects = Sector::current()->get_objects();

  for (std::vector<boost::shared_ptr<GameObject> >::const_iterator i = objects.begin(); i != objects.end(); ++i)
    {
      Entity* object = dynamic_cast<Entity*>(i->get());

      if (object && object != this && object->is_useable())
        {
          //FIXME use proper collision detection
          if (object->get_pos().x > pos.x - 32 && object->get_pos().x < pos.x + 32 &&
              object->get_pos().y > pos.y - 256 && object->get_pos().y < pos.y + 256)
            {
              return object;
            }
        }
    }
  return 0;  
}

void
Player::update_stand(const Controller& controller)
{ 
  if (controller.button_was_pressed(USE_BUTTON))
    {
      Entity* obj = find_useable_entity();
      if (obj)
        obj->use();
      return;
    }
    
  if (controller.button_was_pressed(JUMP_BUTTON)
      && controller.get_axis_state(Y_AXIS) > 0.5f) 
    {
      set_jump_up_begin();
    } 
  else if (controller.button_was_pressed(AIM_BUTTON))
    {
      if (0)
        {
          // TODO remove me later, just here for testing
          Grenade* grenade = new Grenade();
          grenade->set_pos(get_pos() + Vector2f(50, -300));
          grenade->set_velocity(Vector2f(20, -10));
          Sector::current()->add(grenade);
        }
      else
        {
          sprite.set_action("PullGun");
          state = PULL_GUN;
        }
    }
  else if (controller.get_axis_state(X_AXIS) < -0.5f) 
    {
      if (get_direction() == WEST)
        set_walk(WEST);
      else
        set_turnaround();
    }
  else if (controller.get_axis_state(X_AXIS) > 0.5f) 
    {
      if (get_direction() == EAST)
        set_walk(EAST);
      else
        set_turnaround();
    }
}

void
Player::set_walk(Direction direction)
{
  try_set_action("Walk");
  sprite.set_rot(direction == EAST);
  state = WALK;
  if (direction == EAST)
    velocity.x = WALK_SPEED;
  else
    velocity.x = -WALK_SPEED;

  // start walking sound
  // sound_source.reset(sound_manager->create_sound_source("sounds/steps_dirt.ogg"));
  // sound_source->set_looping(true);
  // sound_source->play();
}

void
Player::update_walk(const Controller& controller)
{
  if (fabsf(controller.get_axis_state(X_AXIS)) < 0.5f) // Hardcoded DEAD_ZONE, somewhat evil 
    {
      leave_walk();
      set_stand();
      return;
    }

  if ((get_direction() == WEST && controller.get_axis_state(X_AXIS) > 0.5f) ||
      (get_direction() == EAST && controller.get_axis_state(X_AXIS) < -0.5f))
    {
      leave_walk();
      set_turnaround();
      return;
    }
  
  if (controller.get_button_state(RUN_BUTTON))
    {
      leave_walk();
      set_run();
      return;
    }
}

void
Player::leave_walk()
{
  // stop walking sound
  // sound_source->stop();
}

void
Player::set_ducking()
{
  try_set_action("StandToDuck");
  sprite.set_next_action("Ducking");
  state = DUCKING;
  velocity.x = 0;
}

void
Player::update_ducking(const Controller& controller)
{
  // ducking
  if (sprite.switched_actions()) 
    {
      if (sprite.get_action() == "Ducking")
        set_ducked();
      else
        set_stand();
      return;
    }
  
  if (!(controller.get_axis_state(Y_AXIS) > 0.5f) && sprite.get_speed() > 0) 
    {
      sprite.set_speed(-1.0);
      sprite.set_next_action("Stand");
      state = STAND;
    } 
  else if (controller.get_axis_state(Y_AXIS) > 0.5f && sprite.get_speed() < 0) 
    {
      sprite.set_speed(1.0);
      sprite.set_next_action("Ducking");
    }
}

void
Player::set_ducked()
{
  assert(sprite.get_action() == "Ducking");
  state = DUCKED;
}

void
Player::update_ducked(const Controller& controller)
{
  if (!(controller.get_axis_state(Y_AXIS) > 0.5f))
    {
      state = DUCKING;
      sprite.set_action("StandToDuck", -1.0);
      sprite.set_next_action("Stand");
    }  
}

void
Player::set_turnaround()
{
  velocity.x = 0;
  try_set_action("Turn");
  sprite.set_next_action("Walk");
  sprite.set_next_rot(! sprite.get_rot());
  state = TURNAROUND;
}

void
Player::update_turnaround(const Controller& controller)
{
  if (sprite.switched_actions()) 
    {
      if (sprite.get_rot())
        {
          set_walk(EAST);
        }
      else {
        set_walk(WEST);
      }
    } 
  if ((sprite.get_rot() && controller.get_axis_state(X_AXIS) > 0.5f) ||
      (!sprite.get_rot() && controller.get_axis_state(X_AXIS) < -0.5f)) 
    {
      sprite.set_speed(-1.0);
      sprite.set_next_action("Walk");
      state = WALK;
    }
}

void
Player::set_stand_to_listen(bool backwards)
{
  try_set_action("StandtoListen", backwards ? -1.0f : 1.0f);
  if (!backwards) 
    {
      sprite.set_next_action("Listen");
      velocity = Vector2f(0, 0);
    } 
  else
    {
      sprite.set_next_action("Stand");
    }
  state = STAND_TO_LISTEN;
}

void
Player::update_stand_to_listen(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      if (sprite.get_action() == "Stand")
        set_stand();
      else
        set_listen();
    }
}

void
Player::set_listen()
{
  try_set_action("Listen");
  state = LISTEN;
}

void
Player::update_listen(const Controller& /*controller*/)
{
  // nothing
}

void
Player::set_run()
{
  try_set_action("Run");
  if (get_direction() == EAST)
    velocity.x = RUN_SPEED;
  else
    velocity.x = -RUN_SPEED;  
  state = RUN;

  // start running sound
  // sound_source.reset(sound_manager->create_sound_source("sounds/steps_dirt.ogg"));
  // sound_source->set_looping(true);
  // sound_source->play();
}

void
Player::update_run(const Controller& controller)
{
  if (!controller.get_button_state(RUN_BUTTON)) 
    {
      leave_run();
      set_walk(get_direction());
      return;
    }
  if (controller.get_button_state(JUMP_BUTTON)) 
    {
      leave_run();
      set_jump_begin();
      return;
    }
}

void
Player::leave_run()
{
  // sound_source->stop();
}

void
Player::set_jump_begin()
{
  if (sprite.before_marker("RightFoot")) 
    {
      sprite.set_next_action("JumpRightFoot");
      sprite.abort_at_marker("RightFoot");
      jump_foot = LEFT_FOOT;
    } 
  else if (sprite.before_marker("LeftFoot")) 
    {
      sprite.set_next_action("JumpLeftFoot");
      sprite.abort_at_marker("LeftFoot");
      jump_foot = RIGHT_FOOT;
    } 
  else 
    {
      sprite.set_next_action("JumpRightFoot");
      sprite.abort_at_marker("RightFoot");
      jump_foot = LEFT_FOOT;
    }
  state = JUMP_BEGIN;
}

void
Player::update_jump_begin(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      if (sprite.get_action() == "JumpLeftFoot") 
        {
          sprite.set_next_action("JumpLeftFootAir");
        } 
      else if (sprite.get_action() == "JumpRightFoot") 
        {
          sprite.set_next_action("JumpRightFootAir");
        } 
      else
        {
          set_jump_air();
          return;
        }
    }
}

void
Player::set_jump_air()
{
  velocity.y = -450;
  sprite.set_next_action("JumpLandSofttoRun");
  state = JUMP_AIR;
}

void
Player::update_jump_air(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      set_jump_land();
      return;
    }
}

void
Player::set_jump_land()
{
  sprite.set_next_action("Run");
  state = JUMP_LAND;
}

void
Player::update_jump_land(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      set_run();
      return;
    }
}

void
Player::set_jump_up_begin()
{
  sprite.set_next_action("JumpUp");
  state = JUMP_UP_BEGIN;
}

void
Player::update_jump_up_begin(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      set_jump_up_air();
      return;
    }
}

void
Player::set_jump_up_air()
{
  velocity.y = -400;
  sprite.set_next_action("JumpLandSofttoRun");
  state = JUMP_UP_AIR;
}

void
Player::update_jump_up_air(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      set_jump_up_land();
      return;
    }
}

void
Player::set_jump_up_land()
{
  sprite.set_next_action("Stand");
  state = JUMP_UP_LAND;
}

void
Player::update_jump_up_land(const Controller& /*controller*/)
{
  if (sprite.switched_actions()) 
    {
      set_stand();
      return;
    }
}

void
Player::update_pull_gun(const Controller& controller)
{
  if (!controller.get_button_state(AIM_BUTTON))
    {
      sprite.set_next_action("Stand");      
      state = STAND;
    }
}

Direction
Player::get_direction() const
{
  return sprite.get_rot() ? EAST : WEST;
}

void
Player::try_set_action(const std::string& name, float speed)
{
  if (sprite.get_action() != name)
    sprite.set_action(name, speed);
}

int
Player::get_energy() const
{
  return energy;
}

int
Player::get_max_energy() const
{
  return MAX_ENERGY;
}

void
Player::hit(int points)
{
  if (energy > 0 && hit_count <= 0)
    {
      energy -= points;
      //hit_count = 1.0f;

      if (energy <= 0)
        {
          //switch_movement_state(KILLED);
          hit_count = 0;
        }
    }
}

void
Player::collision(const CollisionData& data)
{
  // copy velocity, as "velocity" is the wanted velocity, whereas
  // cur_vel is the velocity in the current delta-frame
  Vector2f cur_vel = c_object->get_velocity(); 
  if (data.direction.y != 0)
    {
      cur_vel.y = 0;
      velocity.y = 0; // also reset vertical velocity
    }
  else
    {
      // do not reset horizontal velocity, as it's only set, when starting to go/run
      cur_vel.x = 0;
    }
  c_object->set_velocity (cur_vel);
}

void 
Player::set_pos(Vector2f pos)
{
  Entity::set_pos(pos);
  c_object->set_pos(pos);
}

/* EOF */
