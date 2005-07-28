#ifndef __SPRITE2D_SPRITE_HPP__
#define __SPRITE2D_SPRITE_HPP__

#include <string>
#include "math/vector.hpp"
#include "math/matrix.hpp"

class CL_GraphicContext;
class SceneContext;

namespace sprite2d {
class Data;
struct Action;
} 

class Sprite
{
public:
  Sprite();
  Sprite(const std::string& filename);
  Sprite(const sprite2d::Data* data);
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

  void draw(CL_GraphicContext* gc, const Vector& pos, const Matrix& modelview);

  /** true if the Sprite is valid and usable, false if not */
  operator bool() const;
private:
  /** Pointer to the Sprites data which is shared among all sprites
      with are loaded from the same file */
  const sprite2d::Data* data;

  const sprite2d::Action* current_action;

  float frame;
  float speed;
  float alpha;

  bool pingpong;
  bool reverse;
  bool vflip;
};

#endif

