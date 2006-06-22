/*  $Id$
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
** 
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#ifndef HEADER_SPRITE2DVIEW_HPP
#define HEADER_SPRITE2DVIEW_HPP

#include "display/scene_context.hpp"
#include "sprite2d/sprite.hpp"
#include "screen.hpp"

/**
 * A simple class to view 3d sprites and their different actions,
 * mostly usefull for debugging 
 */
class Sprite2DView : public Screen
{
private:
  SceneContext sc;
  std::vector<std::string> directory;
  std::vector<std::string> shuffle_directory;

  enum Mode { SLIDESHOW, MANUAL } mode;
  
  // Manual Mode Variables
  float  zoom;
  Vector pos;

  // Slideshow variables
  float width;
  float height;
  float aspect;

  int    index;
  Sprite sprite;
  
  Sprite new_sprite;
  float fadein;

  float  scale;
  float  offset;

  float display_time;
  bool show_thumbnail;
  bool ignore_delta;
  bool shuffle;
public:
  Sprite2DView();
  ~Sprite2DView();
  
  void adddir(const std::string& dir);

  void draw();
  void update(float delta, const Controller& controller);

  void update_slideshow(float delta, const Controller& controller);
  void update_manual(float delta, const Controller& controller);

  /** move \a i images forward in the directory */
  void next_image(int i = 1);
  void prev_image(int i = 1) { next_image(-i); }

  void set_sprite(const std::string& filename);
  void prepare_sprite(Sprite& sprite);
private:
  Sprite2DView (const Sprite2DView&);
  Sprite2DView& operator= (const Sprite2DView&);
};

#endif

/* EOF */
