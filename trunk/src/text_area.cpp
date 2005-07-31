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
#include "baby_xml.hpp"
#include "text_area.hpp"

struct TextAreaCommand 
{
  enum Type { WORD, START, END } type;
  std::string content;
    
  TextAreaCommand(Type type_, const std::string& content_)
    : type(type_),
      content(content_)
  {}
};

class TextAreaImpl
{
public:
  TTFFont* font;
  Rectf rect;
  float passed_time;

  int v_space;
  std::vector<TextAreaCommand> commands;
};

TextArea::TextArea(const Rectf& rect)
  : impl(new TextAreaImpl)
{
  impl->rect    = rect;
  // FIXME: freetype might provide info for vspacing, not sure
  impl->v_space = 2;

  impl->passed_time = 0;
}

TextArea::~TextArea()
{
  delete impl;
}

void
TextArea::set_text(const std::string& str)
{
  BabyXML xml(str);

  for(BabyXML::iterator i = xml.begin(); i != xml.end(); ++i)
    {
      if (i->type == BabyXML::Node::START_TAG)
        {
          impl->commands.push_back(TextAreaCommand(TextAreaCommand::START, i->content));
        }
      else if (i->type == BabyXML::Node::END_TAG)
        {
          impl->commands.push_back(TextAreaCommand(TextAreaCommand::END, i->content));
        }
      else if (i->type == BabyXML::Node::TEXT)
        {      
          // Seperate the given str into words, words are seperated by either
          // ' ' or '\n', space is threaded as a word of its own
          // "Hello  World \n" => ("Hello", " ", " ", "World", " ", "\n")
          std::string word;
          for(std::string::const_iterator j = i->content.begin(); j != i->content.end(); ++j)
            {
              if (*j == ' ' || *j == '\n')
                {
                  if (word.empty())
                    {
                      impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, std::string(1, *j)));
                    }
                  else
                    {
                      impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, word));
                      impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, std::string(1, *j)));
                      word = "";
                    }
                }
              else
                {
                  word += *j;
                }
            }
          if (!word.empty())
            impl->commands.push_back(TextAreaCommand(TextAreaCommand::WORD, word));
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
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindTexture(GL_TEXTURE_2D, impl->font->get_texture().get_handle());

  glPushMatrix();
  glTranslatef(impl->rect.left, impl->rect.top, 0);
  // Voodoo to get non-blurry fonts
  float mx = -0.375;
  float my = -0.375; 
  glBegin(GL_QUADS);

  int x_pos = 0;
  int y_pos = 0;

  Color top_color    = Color(1.0f, 1.0f, 1.0f);
  Color bottom_color = Color(1.0f, 1.0f, 1.0f);
  bool is_small = false;
  bool is_large = false;
  float eat_time = impl->passed_time;
  bool sinus = false;
  for(std::vector<TextAreaCommand>::const_iterator i = impl->commands.begin(); i != impl->commands.end(); ++i)
    {
      switch (i->type)
        {
        case TextAreaCommand::START:
          if (i->content == "b")
            {
              top_color    = Color(1.0f, 0.0f, 0.0f);
              bottom_color = Color(0.8f, 0.0f, 0.0f);
            }
          else if (i->content == "i")
            {
              top_color    = Color(0.65f, 0.7f, 1.0f);
              bottom_color = Color(0.65f, 0.7f, 1.0f);
            }
          else if (i->content == "small")
            {
              is_small = true;
            }
          else if (i->content == "large")
            {
              is_large = true;
            }
          else if (i->content == "sleep")
            {
              eat_time -= 1.0f;
            }
          else if (i->content == "sin")
            {
              sinus = true;
            }
          break;

        case TextAreaCommand::END:
          if (i->content == "b" || i->content == "i")
            {
              top_color    = Color(1.0f, 1.0f, 1.0f);
              bottom_color = Color(1.0f, 1.0f, 1.0f);
            }
          else if (i->content == "small")
            {
              is_small = false;
            }
          else if (i->content == "large")
            {
              is_large = false;
            }
          else if (i->content == "sin")
            {
              sinus = false;
            }
          break;
          
        case TextAreaCommand::WORD:
        retry:
          int word_width;

          if (is_small)
            word_width = static_cast<int>(impl->font->get_width(i->content) * 0.6f);
          else if (is_large)
            word_width = static_cast<int>(impl->font->get_width(i->content) * 2.0f);
          else
            word_width = impl->font->get_width(i->content);
          
          if (i->content == "\n")
            {
              x_pos = 0;
              y_pos += impl->font->get_height() + impl->v_space;
            }
          else if (x_pos + word_width > impl->rect.get_width() && word_width <= impl->rect.get_width())
            {
              x_pos = 0;
              y_pos += impl->font->get_height() + impl->v_space;
              goto retry;
            }
          else
            {
              if (x_pos == 0 && i->content == " ")
                {
                  // ignore space at the beginning of a line
                }
              else
                {
                  for(std::string::const_iterator j = i->content.begin(); j != i->content.end() && eat_time > 0; ++j)
                    {
                      int x = x_pos;
                      int y = y_pos;

                      if (sinus) // FIXME: this could actually work per vertex
                        y += static_cast<int>(sin(impl->passed_time * 10.0f + x_pos / 15.0f) * 5.0f);

                      if (*j == '.' || *j == '\n')
                        eat_time -= 0.50f;
                      else
                        eat_time -= 0.05f;

                      const TTFCharacter& character = impl->font->get_character(*j);
                      
                      if (is_small)
                        {
                          float scale = 0.6f;
                          glColor4f(top_color.r, top_color.g, top_color.b, top_color.a);
                          glTexCoord2f(character.uv.left, character.uv.top);
                          glVertex2f(x + scale * character.pos.left + mx,
                                     y + scale * (character.pos.top  + my));

                          glTexCoord2f(character.uv.right, character.uv.top);
                          glVertex2f(x + scale * character.pos.right + mx, 
                                     y + scale * (character.pos.top) + my);

                          glColor4f(bottom_color.r, bottom_color.g, bottom_color.b, bottom_color.a);
                          glTexCoord2f(character.uv.right, character.uv.bottom);
                          glVertex2f(x + scale * character.pos.right  + mx, 
                                     y + scale * (character.pos.bottom) + my);

                          glTexCoord2f(character.uv.left, character.uv.bottom);
                          glVertex2f(x + scale * character.pos.left   + mx, 
                                     y + scale * (character.pos.bottom) + my);
                          x_pos += static_cast<int>(scale * character.advance);
                        }
                      else if (is_large)
                        {
                          float scale = 2.0f;
                          glColor4f(top_color.r, top_color.g, top_color.b, top_color.a);
                          glTexCoord2f(character.uv.left, character.uv.top);
                          glVertex2f(x + scale * character.pos.left + mx,
                                     y + (character.pos.top  + my));

                          glTexCoord2f(character.uv.right, character.uv.top);
                          glVertex2f(x + scale * character.pos.right + mx, 
                                     y + (character.pos.top) + my);

                          glColor4f(bottom_color.r, bottom_color.g, bottom_color.b, bottom_color.a);
                          glTexCoord2f(character.uv.right, character.uv.bottom);
                          glVertex2f(x + scale * character.pos.right  + mx, 
                                     y + (character.pos.bottom) + my);

                          glTexCoord2f(character.uv.left, character.uv.bottom);
                          glVertex2f(x + scale * character.pos.left   + mx, 
                                     y + (character.pos.bottom) + my);
                          x_pos += static_cast<int>(scale * character.advance);
                        }
                      else   
                        {
                          glColor4f(top_color.r, top_color.g, top_color.b, top_color.a);
                          glTexCoord2f(character.uv.left, character.uv.top);
                          glVertex2f(x + character.pos.left + mx,
                                     y + character.pos.top  + my);

                          glTexCoord2f(character.uv.right, character.uv.top);
                          glVertex2f(x + character.pos.right + mx, 
                                     y + character.pos.top   + my);

                          glColor4f(bottom_color.r, bottom_color.g, bottom_color.b, bottom_color.a);
                          glTexCoord2f(character.uv.right, character.uv.bottom);
                          glVertex2f(x + character.pos.right  + mx, 
                                     y + character.pos.bottom + my);

                          glTexCoord2f(character.uv.left, character.uv.bottom);
                          glVertex2f(x + character.pos.left   + mx, 
                                     y + character.pos.bottom + my);
                          x_pos += character.advance;
                        }
                    }
                }
            }
          break;
        }
    }
  glEnd();
  glPopMatrix();
}

void
TextArea::update(float delta)
{
  impl->passed_time += delta;
}

Rectf
TextArea::get_rect() const
{
  return impl->rect;
}

/* EOF */
