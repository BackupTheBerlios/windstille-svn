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
#include "particle_system.hxx"
#include "random.hxx"

class Randomizer 
{
public:
  virtual void set_pos(Particle& p) =0;
};

class PointRandomizer : public Randomizer {
public:
  void set_pos(Particle& particle) {
    particle.x = 0;
    particle.y = 0;
  }
};

class RectRandomizer : public Randomizer {
public:
  float width;
  float height;

  RectRandomizer(float width_, float height_) 
    : width(width_), height(height_) {}
 
  void set_pos(Particle& p) {
    p.x = rnd.drand(-width/2, width/2);
    p.y = rnd.drand(-height/2, height/2);
  }
};

class CircleRandomizer : public Randomizer  {
public:
  float radius;

  CircleRandomizer(float radius_) 
    : radius(radius_) {}

  void set_pos(Particle& p) {
    // FIXME: BROKEN!!!!!
    p.x = rnd.drand(-radius, radius);
    p.y = sqrt((radius*radius) - (p.x*p.x)) * rnd.drand(-1.0f, 1.0f);
  }
};

class LineRandomizer : public Randomizer {
public:
  float x1, y1;
  float x2, y2;

  LineRandomizer(float x1_, float y1_, float x2_, float y2_) 
    : x1(x1_), y1(y1_), x2(x2_), y2(y2_)
  {}

  void set_pos(Particle& p) {
    float l = rnd.drand();
    p.x = x1 + (x2-x1) * l;
    p.y = y1 + (y2-y1) * l;
  }
};

SurfaceDrawer::SurfaceDrawer(const CL_Surface& sur)
  : surface(sur)
{
}

SurfaceDrawer::~SurfaceDrawer() 
{
}
  
void
SurfaceDrawer::set_surface(const CL_Surface& sur)
{
  surface = sur;
  surface.set_alignment(origin_center);
}

void
SurfaceDrawer::draw(ParticleSystem& psys) 
{          
  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      // FIXME: use custom OpenGL here
      if (i->t != -1.0f)
        {
          float p = 1.0f - psys.get_progress(i->t);
          surface.set_color(CL_Color(int(psys.color_start.get_red()   * p + psys.color_stop.get_red()   * (1.0f - p)),
                                     int(psys.color_start.get_green() * p + psys.color_stop.get_green() * (1.0f - p)),
                                     int(psys.color_start.get_blue()  * p + psys.color_stop.get_blue()  * (1.0f - p)),
                                     int(psys.color_start.get_alpha() * p + psys.color_stop.get_alpha() * (1.0f - p))));

          surface.set_scale(psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start),
                            psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start));
          surface.set_angle(i->angle);
          surface.draw(int(psys.get_x_pos() + i->x),
                       int(psys.get_y_pos() + i->y));
        }
    }
}

void
SparkDrawer::draw(ParticleSystem& psys) 
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();
  
  //glBlendMode(GL_SRC_ALPHA, GL_SRC_ALPHA_MINUS_ONE);
  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA, GL_ONE );
  glBegin(GL_LINES);
  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      glColor4f(1.0, 1.0, 0, 1.0f-psys.get_progress(i->t));
      glVertex2f(i->x, i->y);
      glColor4f(0, 0, 0, 0);
      glVertex2f(i->x - i->v_x/10.0f, i->y - i->v_y/10.0f);
    }
  glEnd();
}

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
ParticleSystem::draw()
{
  if (drawer)
    {
      drawer->draw(*this);
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
  
}

void
ParticleSystem::set_cycles(float num)
{
  
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
