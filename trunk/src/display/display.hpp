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

#ifndef HEADER_DISPLAY_DISPLAY_HPP
#define HEADER_DISPLAY_DISPLAY_HPP

#include "math/rect.hpp"
#include "color.hpp"

/** FIXME: its called VDisplay instead of Display to avoid naming
    conflict with X11 header, should be renamed as soon as we got rid
    of ClanLib, which includes the X11 header in one of the
    headerfiles */
class VDisplay
{
private:
public:
  static void fill_rect(const Rectf& rect, const Color& color);
  static void draw_rect(const Rectf& rect, const Color& color);

  static void draw_line(const Vector& pos1, const Vector& pos2, const Color& color);

  static int  get_width();
  static int  get_height();
};

#endif

/* EOF */
