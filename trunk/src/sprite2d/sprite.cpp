#include <config.h>

#include "sprite2d/sprite.hpp"

#include <math.h>
#include <GL/gl.h>
#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include "display/drawing_request.hpp"
#include "display/scene_context.hpp"
#include "sprite2d/data.hpp"
#include "sprite2d/manager.hpp"
#include "util.hpp"

Sprite::Sprite()
  : data(0)
{  
}

Sprite::Sprite(const std::string& filename)
  : data(sprite2d_manager->create_data(filename))
{
  current_action = data->actions[0];
  vflip    = false;
  frame    = 0;
  speed    = 1.0;
  pingpong = false;
  reverse  = false;
  alpha    = 0.0;
}

Sprite::Sprite(const sprite2d::Data* data)
  : data(data)
{
  current_action = data->actions[0];
  vflip = false;
  frame = 0;
  speed = 1.0;
  pingpong = false;
  reverse = false;
  alpha = 0.0;
}

Sprite::~Sprite()
{
}

void
Sprite::update(float elapsed_time)
{
  float step = elapsed_time * speed * current_action->speed;
  if(reverse)
    step = -step;

  frame = fmodf(frame + current_action->surfaces.size() + step,
      current_action->surfaces.size());
}

void
Sprite::set_action(const std::string& name)
{
  for(sprite2d::Data::Actions::const_iterator i = data->actions.begin();
      i != data->actions.end(); ++i) {
    const sprite2d::Action* action = *i;
    if(action->name == name) {
      current_action = action;
      pingpong = false;
      reverse = false;
      speed = 1.0;
      frame = 0;
      vflip = false;
      alpha = 0.0;
      return;
    }
  }

  std::ostringstream msg;
  msg << "No action '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

void
Sprite::set_vflip(bool vflip)
{
  this->vflip = vflip;
}

bool
Sprite::get_vflip() const
{
  return vflip;
}

void
Sprite::set_pingpong(bool pingpong)
{
  this->pingpong = pingpong;
}

bool
Sprite::get_pingpong() const
{
  return pingpong;
}

void
Sprite::set_speed(float speed)
{
  this->speed = speed;
}

float
Sprite::get_speed() const
{
  return speed;
}

void
Sprite::set_alpha(float alpha)
{
  this->alpha = alpha;
}

float
Sprite::get_alpha() const
{
  return alpha;
}

void
Sprite::draw(SceneContext& sc, const Vector& pos, float z_pos)
{
  Surface surface = current_action->surfaces[ static_cast<int> (frame) ];
  sc.color().draw(surface, pos.x, pos.y, z_pos, alpha);
}

Sprite::operator bool() const
{
  return data != 0;
}

void
Sprite::draw_light(SceneContext& sc, const Vector& pos, float z_pos)
{
  Surface surface = current_action->surfaces[ static_cast<int> (frame) ];
  sc.light().draw(surface, pos.x, pos.y, z_pos, alpha);
}

/* EOF */
