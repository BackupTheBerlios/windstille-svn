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
#include "math/vector.hpp"
#include "color.hpp"
#include "lisp/properties.hpp"
#include "lisp/property_iterator.hpp"
#include "windstille_getters.hpp"
#include "spark_drawer.hpp"
#include "randomizer.hpp"

ParticleSystem::ParticleSystem(const lisp::Lisp* lisp)
{
  // Init some defaults
  randomizer = new PointRandomizer;
  drawer     = 0;
  x_pos      = 320.0f;
  y_pos      = 240.0f;
  z_pos      = 0;
  life_time  = 1.0f;

  gravity_x = 0.0f;
  gravity_y = -10.0f;

  cone_start = 0;
  cone_stop  = 2*M_PI;

  size_start = 1.0f;
  size_stop  = 1.0f;

  speed_start = 100.0;
  speed_stop  = 200.0f;

  color_start = Color(1.0f, 1.0f, 1.0f, 1.0f);
  color_stop  = Color(   0,    0,    0,    0);

  set_count(70);
  
  // Set stuff from Lisp
  lisp::Properties props(lisp);
  float p_lifetime;
  if (props.get("lifetime", p_lifetime))
    set_lifetime(p_lifetime);
  
  int p_count;
  if (props.get("count", p_count))
    set_count(p_count);

  props.get("count", z_pos);

  Vector p_gravity;
  if (props.get("gravity", p_gravity))
    set_gravity(p_gravity.x, p_gravity.y);

  float p_cycles;
  if (props.get("cycles",  p_cycles))
    set_cycles(p_cycles);

  Vector p_spawn_point;
  if (props.get("spawn-point", p_spawn_point))
    set_spawn_point(p_spawn_point.x, p_spawn_point.y);

  Vector p_pos;
  if (props.get("pos", p_pos))
    set_pos(p_pos.x, p_pos.y);
  
  Vector p_cone;
  if (props.get("cone", p_cone))
    set_cone(p_cone.x, p_cone.y);

  Vector p_size;
  if (props.get("size", p_size))
    set_size(p_size.x, p_size.y);

  Vector p_aspect;
  if (props.get("aspect", p_aspect))
    set_aspect(p_aspect.x, p_aspect.y);

  Color p_color;
  if (props.get("color", p_color))
    set_color(Color(p_color.r, p_color.g, p_color.b, p_color.a));

  Color p_fade_color;
  if (props.get("fade-color", p_fade_color))
    set_color(Color(p_fade_color.r, p_fade_color.g, p_fade_color.b, p_fade_color.a));

  Vector p_speed;
  if (props.get("speed", p_speed))
    set_speed(p_speed.x, p_speed.y);

  {
    const lisp::Lisp* drawer_lisp = 0;
    if (props.get("drawer", drawer_lisp))
      {
        lisp::Properties drawer_props(drawer_lisp);
        lisp::PropertyIterator<const lisp::Lisp*> iter = drawer_props.get_iter();
        while(iter.next()) {
          if (iter.item() == "surface-drawer") {
          
          } else if (iter.item() == "spark-drawer") {
            set_drawer(new SparkDrawer(*iter));
          } else {
            std::cout << "Unknown item: " << iter.item() << std::endl;
          }
        }
      }
  }

  const lisp::Lisp* distribution_lisp = 0;
  if (props.get("distribution", distribution_lisp))
    {
      lisp::Properties distribution_props(distribution_lisp);
      lisp::PropertyIterator<const lisp::Lisp*> iter = distribution_props.get_iter();
      if (iter.next()) {
        lisp::Properties prop(*iter);

        if (iter.item() == "point-distribution") {
          set_point_distribution();
        } else if (iter.item() == "line-distribution") {
          float x1, y1, x2, y2;
          prop.get("x1", x1);
          prop.get("y1", y1);
          prop.get("x2", x2);
          prop.get("y2", y2);
          
          set_line_distribution(x1, y1, x2, y2);
        } else {
          std::cout << "Unknown distribution: " << iter.item() << std::endl;
        }
      }
    }

  //props.get("point-distribution",   ); // void
  //props.get("line-distribution",   ); // 2xvector2
  //props.get("circle", ); // float
  //props.get("rect-distribution", ); // vector2
  props.print_unused_warnings("ParticleSystem");
}

ParticleSystem::ParticleSystem()
{
  randomizer = new PointRandomizer;
  drawer     = 0;
  x_pos      = 320.0f;
  y_pos      = 240.0f;
  z_pos      = 0;
  life_time  = 1.0f;

  gravity_x = 0.0f;
  gravity_y = -10.0f;

  cone_start = 0;
  cone_stop  = 2*M_PI;

  size_start = 1.0f;
  size_stop  = 1.0f;

  speed_start = 100.0;
  speed_stop  = 200.0f;

  color_start = Color(1.0f, 1.0f, 1.0f, 1.0f);
  color_stop  = Color(  0,   0,   0,   0);

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
ParticleSystem::set_color(const Color& color)
{
  color_start = color;
}

void
ParticleSystem::set_fade_color(const Color& color)
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
