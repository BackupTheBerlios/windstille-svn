/*  $Id$
**   __      __.__            .___        __  .__.__  .__
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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef HEADER_TTF_FONT_HXX
#define HEADER_TTF_FONT_HXX

#include <ClanLib/Core/Math/rect.h>
#include <string>

class TTFCharacter
{
public:
  /** The left-side bearing, i.e., the horizontal distance from the
      current pen position to the left border of the glyph bitmap. */
  int left;
  
  /* The top-side bearing, i.e., the vertical distance from the
     current pen position to the top border of the glyph bitmap. This
     distance is positive for upwards-y! */
  int top;
  
  int width;
  int height;

  /** The position of the character in a OpenGL texture, given in
      uv-coordinates */
  CL_Rectf uv;

  TTFCharacter(int left, int top, int width, int height, const CL_Rectf& uv);
};

class TTFFontImpl;

/** */
class TTFFont
{
public:
  static void init();
  static void deinit();

  TTFFont(const std::string& file, int size);
  ~TTFFont();

  int get_height() const;

  TTFCharacter get_character(int c);
  void draw(float x_pos, float y_pos, const std::string& str);
private:
  TTFFontImpl* impl;
};

#endif

/* EOF */
