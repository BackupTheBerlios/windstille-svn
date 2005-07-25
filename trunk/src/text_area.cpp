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

#include <ClanLib/gl.h>
#include <ClanLib/Display/display.h>
#include "ttf_font.hpp"
#include "text_area.hpp"
#include "text_area.hpp"

class TextAreaImpl
{
public:
  TTFFont* font;
  CL_Rectf rect;
  std::vector<std::string> words;
};

TextArea::TextArea(const CL_Rect& rect)
  : impl(new TextAreaImpl)
{
  impl->rect = rect;
}

TextArea::~TextArea()
{
  delete impl;
}

void
TextArea::set_text(const std::string& str)
{
  // Seperate the given str into words, words are seperated by either
  // ' ' or '\n', space is threaded as a word of its own
  // "Hello  World \n" => ("Hello", " ", " ", "World", " ", "\n")
  std::string word;
  for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
      if (*i == ' ' || *i == '\n')
        {
          if (word.empty())
            impl->words.push_back(std::string(1, *i));
          else
            {
              impl->words.push_back(word);
              impl->words.push_back(std::string(1, *i));
              word = "";
            }
        }
      else
        {
          word += *i;
        }
    }
}

void
TextArea::set_font(TTFFont* font)
{
  impl->font = font;
}

void
TextArea::draw()
{
  Color color(1.0f, 1.0f, 1.0f);

  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  impl->font->get_surface().bind();

  glPushMatrix();
  glTranslatef(impl->rect.left, impl->rect.top, 0);
  // Voodoo to get non-blurry fonts
  float mx = -0.375;
  float my = -0.375; 
  glBegin(GL_QUADS);
  glColor4f(color.r, color.g, color.b, color.a);

  int x_pos = 0;
  int y_pos = 0;
  for(std::vector<std::string>::const_iterator i = impl->words.begin(); i != impl->words.end(); ++i)
    {
    retry:
      int word_width = impl->font->get_width(*i);
      if (*i == "\n")
        {
          x_pos = 0;
          y_pos += impl->font->get_height();
        }
      else if (x_pos + word_width > impl->rect.get_width() && word_width <= impl->rect.get_width())
        {
          x_pos = 0;
          y_pos += impl->font->get_height();
          goto retry;
        }
      else
        {
          for(std::string::const_iterator j = i->begin(); j != i->end(); ++j)
            {
              const TTFCharacter& character = impl->font->get_character(*j);
      
              glTexCoord2f(character.uv.left, character.uv.top);
              glVertex2f(x_pos + character.pos.left + mx,
                         y_pos + character.pos.top  + my);

              glTexCoord2f(character.uv.right, character.uv.top);
              glVertex2f(x_pos + character.pos.right + mx, 
                         y_pos + character.pos.top   + my);

              glTexCoord2f(character.uv.right, character.uv.bottom);
              glVertex2f(x_pos + character.pos.right  + mx, 
                         y_pos + character.pos.bottom + my);

              glTexCoord2f(character.uv.left, character.uv.bottom);
              glVertex2f(x_pos + character.pos.left   + mx, 
                         y_pos + character.pos.bottom + my);

              x_pos += character.advance;
            }
        }
    }
  glEnd();
  glPopMatrix();
}

/* EOF */
