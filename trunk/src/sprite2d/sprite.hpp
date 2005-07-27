#ifndef __SPRITE2D_SPRITE_HPP__
#define __SPRITE2D_SPRITE_HPP__

#include <string>
#include "math/vector.hpp"
#include "math/matrix.hpp"

class CL_GraphicContext;
class SceneContext;

namespace sprite2d
{

class Data;
struct Action;

class Sprite
{
public:
  ~Sprite();

  void update(float elapsed_time);
  void draw(SceneContext& sc, const Vector& pos, float z_pos);
  void draw_light(SceneContext& sc, const Vector& pos, float z_pos);

  void set_action(const std::string& name);
  const std::string& get_action() const;

  void set_vflip(bool vflip);
  bool get_vflip() const;

  void set_pingpong(bool pingpong);
  bool get_pingpong() const;

  void set_speed(float speed);
  float get_speed() const;

  void set_alpha(float alpha);
  float get_alpha() const;

private:
  friend class SpriteDrawingRequest;
  friend class Manager;
  Sprite(const Data* data);
  Sprite(const Sprite& );
  Sprite& operator= (const Sprite& );

  void draw(CL_GraphicContext* gc, const Vector& pos, const Matrix& modelview);

  const Data* data;
  const Action* current_action;
  float frame;
  float speed;
  float alpha;

  bool pingpong;
  bool reverse;
  bool vflip;
};

}

#endif

