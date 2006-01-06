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

#ifndef HEADER_CAMERA_HXX
#define HEADER_CAMERA_HXX

/** 
 * This class manages the virtual camera movement, it follows the
 * player, allows the player to watch around, might zoom out if
 * interesting stuff happens out of the screen and such
 */
class Camera
{
public:
  enum Mode { CAMERA_INACTIVE, CAMERA_FOLLOW_PLAYER, CAMERA_FOLLOW_PATH };

private:
  Mode mode;

  Vector pos;
  
  std::vector<Vector> path;
  float path_pos;

  static Camera* current_;
public:
  static Camera* current() { return current_; }

  Camera();

  void   update(float delta);
  Vector get_pos() const { return pos; }
  void   set_pos(float x, float y);

  void   set_mode(Mode mode_);

  /**
   * Set Camera to follow the given path
   */
  void   set_path(const std::vector<Vector>& path_);

private:
  Camera (const Camera&);
  Camera& operator= (const Camera&);
};

#endif

/* EOF */
