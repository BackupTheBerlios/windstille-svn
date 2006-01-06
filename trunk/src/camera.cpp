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

#include "player.hpp"
#include "sector.hpp"
#include "script_manager.hpp"
#include "display/display.hpp"
#include "camera.hpp"

Camera* Camera::current_ = 0;

/**
 * Simple linear interpolation to move along a given vector path
 * FIXME: Could add curves and different speed per vertex
 */
Camera::PathPoint interpolate_path(const std::vector<Camera::PathPoint>& path, float length)
{
  float length_so_far = 0.0f;
  for(std::vector<Camera::PathPoint>::size_type i = 0; i < path.size()-1; ++i)
    {
      Vector segment = path[i+1].pos - path[i].pos;
      float  segment_length = segment.length();

      if (length_so_far + segment_length > length)
        {
          float factor = (length - length_so_far) / segment_length;
          return Camera::PathPoint(path[i].pos + segment * factor,
                                   (factor * path[i+1].zoom) + ((1.0f - factor) * path[i].zoom));
        }
      length_so_far += segment_length;
    }
  return path.back();
}

Camera::Camera()
  : pos(0, 0), zoom(1.0f)
{
  path_pos = 0;
  current_ = this;
  mode     = CAMERA_FOLLOW_PLAYER;
}

void
Camera::update(float delta)
{
  switch (mode)
    {
    case CAMERA_INACTIVE:
      // do nothing
      break;

    case CAMERA_FOLLOW_PLAYER:
      {
        int hscroll_threshold = 100;
        int vscroll_threshold = 150;

        Vector tpos = Player::current()->get_pos();

        float dist = tpos.x - pos.x;
        if (dist > hscroll_threshold)
          pos.x = tpos.x - hscroll_threshold;
        else if (dist < - hscroll_threshold)
          pos.x = tpos.x + hscroll_threshold;

        dist = tpos.y - pos.y;
        if (dist > vscroll_threshold)
          pos.y = tpos.y - vscroll_threshold;
        else if (dist < -vscroll_threshold)
          pos.y = tpos.y + vscroll_threshold;

        int start_x = Display::get_width()/2;
        int end_x   = Sector::current()->get_width() - Display::get_width()/2;

        int start_y = Display::get_height()/2;
        int end_y   = Sector::current()->get_height() - Display::get_height()/2;

        if (pos.x < start_x)
          pos.x = start_x;

        if (pos.y < start_y)
          pos.y = start_y;

        if (pos.x > end_x)
          pos.x = end_x;

        if (pos.y > end_y)
          pos.y = end_y;
      }
      break;

    case CAMERA_FOLLOW_PATH:
      {
        assert(!path.empty());
        path_pos += delta * 50.0f;

        PathPoint p = interpolate_path(path, path_pos);
        if (p == path.back())
          {
            script_manager->fire_wakeup_event(ScriptManager::CAMERA_DONE);
            set_mode(CAMERA_INACTIVE);
          }
        set_pos(p.pos.x, p.pos.y);
        set_zoom(p.zoom);
      }
      break;
    }
}

void
Camera::set_pos(float x, float y)
{
  // Casting here isn't really necessary, but should about some
  // pixel-jitter when scrolling with subpixel values and pixel
  // precise images
  pos.x = static_cast<int>(x + Display::get_width()/2);
  pos.y = static_cast<int>(y + Display::get_height()/2);
}

void
Camera::set_zoom(float zoom_)
{
  zoom = zoom_;
}

float
Camera::get_zoom() const
{
  return zoom;
}

void
Camera::set_path(const std::vector<Camera::PathPoint>& path_)
{
  path     = path_;
  path_pos = 0;
  mode     = CAMERA_FOLLOW_PATH;
}

void
Camera::set_mode(Mode mode_)
{
  mode = mode_;
}

/* EOF */
