//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#include <math.h>
#include <ClanLib/gl.h>
#include "particle_system.hpp"
#include "display/scene_context.hpp"
#include "randomizer.hpp"

ParticleSystem::ParticleSystem()
{
  randomizer = new PointRandomizer;
  drawer     = 0;
  x_pos      = 320.0f;
  y_pos      = 240.0f;
  life_time  = 1.0f;

  gravity_x = 0.0f;
  gravity_y = -10.0f;

  cone_start = 0;
  cone_stop  = 2*M_PI;

  size_start = 1.0f;
  size_stop  = 1.0f;

  speed_start = 100.0;
  speed_stop  = 200.0f;

  color_start = CL_Color(255, 255, 255, 255);
  color_stop  = CL_Color(  0,   0,   0,   0);

  set_count(70);
}

ParticleSystem::~ParticleSystem()
{
  delete randomizer;
  delete drawer;
}

void
ParticleSystem::set_drawer(Drawer* drawer_)
{
  delete drawer;
  drawer = drawer_;
}
  
void
ParticleSystem::draw(SceneContext& sc)
{
  if (drawer)
    {
      drawer->draw(sc, *this);
    }
  else
    {
      std::cout << "ParticleSystem: No drawer set" << std::endl;
    }
}

void
ParticleSystem::spawn(Particle& particle)
{
  randomizer->set_pos(particle);

  particle.x   += spawn_x_pos;
  particle.y   += spawn_y_pos;

  float direction = rnd.drand(cone_start, cone_stop);
  float speed     = rnd.drand(speed_start, speed_stop);
  particle.v_x    = cos(direction) * speed;
  particle.v_y    = sin(direction) * speed;

  particle.angle = rnd.drand(360);

  particle.t   = std::min(std::max(0.0f, particle.t - life_time), life_time);
}

void
ParticleSystem::update(float delta)
{
  for(Particles::iterator i = particles.begin(); i != particles.end(); ++i)
    {
      if (i->t > life_time)
        {
          spawn(*i);
        }
      else
        {
          i->t += delta;
          
          i->x += i->v_x * delta;
          i->y += i->v_y * delta;

          i->v_x += gravity_x;
          i->v_y += gravity_y;
        }
    }
}

void
ParticleSystem::set_count(int num)
{
  particles.resize(num);

  for(Particles::iterator i = particles.begin(); i != particles.end(); ++i)
    {
      //i->t = -1.0f;
      spawn(*i);
      i->t = (life_time * (float(i - particles.begin())/particles.size()));
    }
}
  
void
ParticleSystem::set_bunching(float factor)
{
  (void) factor; 
}

void
ParticleSystem::set_cycles(float num)
{
  (void) num;
}

void
ParticleSystem::set_pos(float x, float y)
{
  x_pos = x;
  y_pos = y;
}

void
ParticleSystem::set_spawn_point(float x, float y)
{
  spawn_x_pos = x;
  spawn_y_pos = y;
}

void
ParticleSystem::set_point_distribution()
{
  delete randomizer;
  randomizer = new PointRandomizer();
}

void
ParticleSystem::set_line_distribution(float x1, float y1,
                                      float x2, float y2)
{
  delete randomizer;
  randomizer = new LineRandomizer(x1, y1, x2, y2);
}

void
ParticleSystem::set_circle_distribution(float radius)
{
  delete randomizer;
  randomizer = new CircleRandomizer(radius);
}

void
ParticleSystem::set_rect_distribution(float width, float height)
{
  delete randomizer;
  randomizer = new RectRandomizer(width,  height);
}

void
ParticleSystem::set_cone(float start_angle, float stop_angle)
{
  cone_start = start_angle * M_PI/180.0f;
  cone_stop  = stop_angle  * M_PI/180.0f;
}

void
ParticleSystem::set_gravity(float arg_gravity_x, float arg_gravity_y)
{
  gravity_x = arg_gravity_x;
  gravity_y = arg_gravity_y;
}

void
ParticleSystem::set_lifetime(float time)
{
  life_time = time;
}

void
ParticleSystem::set_size(float from, float to)
{
  size_start = from;
  size_stop  = to;
}

void
ParticleSystem::set_aspect(float from, float to)
{
  (void) from;
  (void) to;
}

void
ParticleSystem::set_color(const CL_Color& color)
{
  color_start = color;
}

void
ParticleSystem::set_fade_color(const CL_Color& color)
{
  color_stop = color;
}

void
ParticleSystem::set_speed(float from, float to)
{
  speed_start = from;
  speed_stop  = to;
}

float
ParticleSystem::get_progress(float t)
{
  return std::max(0.0f, std::min(1.0f, t/life_time));
}

/* EOF */
