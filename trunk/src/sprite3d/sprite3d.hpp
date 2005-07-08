//  $Id$
// 
//  Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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
#ifndef HEADER_SPRITE3D_HPP
#define HEADER_SPRITE3D_HPP

#include <string>
#include <stdint.h>
#include "game_object.hpp"
#include "display/scene_context.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"

class SceneContext;
class Sprite3DData;
struct Action;
struct ActionFrame;

/**
 * This class is a 3d sprite. It's a set of textured meshed with different
 * animations (called actions) that are keyframe animated.
 */
class Sprite3D
{
public:
  ~Sprite3D();

  /**
   * You should call this every frame
   */
  void update(float elapsed_time);
  void draw(SceneContext& sc, const Vector& pos);
  
  /**
   * Instantly changes the action to the specified one
   */
  void set_action(const std::string& name);
  
  /**
   * Return the name of the currently active action
   */
  const std::string& get_action() const;

  /**
   * Set the next action and vflip value to be played after the current action
   * has finished (or reached the point to be defined by abort_at_marker)
   */
  void set_next_action(const std::string& name);
  void set_next_rot(bool rot);
  void set_next_speed(float speed);

  /**
   * Abort current action after a certain marker has been reached.
   * (This only works if set_next_action has been used before)
   */
  void abort_at_marker(const std::string& marker);

  /**
   * returns true if the current frame of the action is after a certain marker
   */
  bool after_marker(const std::string& marker) const;

  /**
   * returns true exactly once after actions have been switched after a
   * set_next_action call (another call to set_next_action resets this flag)
   */
  bool switched_actions();

  /**
   * The speed of the action is multiplied with this factor
   */
  void set_speed(float speed);
  float get_speed() const;

  /**
   * Rotate (or not rotate) the model 180 degree
   */
  void set_rot(bool rot = true);

  bool get_rot() const
  {
    return rot;
  }

private:
  friend class Sprite3DDrawingRequest;
  friend class Sprite3DManager;
  Sprite3D(const Sprite3DData* data);
  
  Sprite3D (const Sprite3D&);
  Sprite3D& operator= (const Sprite3D&);

  struct Frame {
    const ActionFrame* frame;
    float speed;
    bool rot;
  };

  void set_next_frame();
  void draw(CL_GraphicContext* gc, const Vector& pos, const Matrix& modelview);

  const Sprite3DData* data;
  const Action* current_action;
  int current_frame;
  float speed;
  bool reverse;
  bool rot;
  int last_frame;
  bool actions_switched;

  Frame frame1;
  Frame frame2;
  float blend_time;
  
  const Action* next_action;
  bool next_rot;
  float next_speed;
};

#endif
