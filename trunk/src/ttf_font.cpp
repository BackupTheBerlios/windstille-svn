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

#include <assert.h>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <ClanLib/gl.h>
#include <ClanLib/Display/pixel_buffer.h>
#include <ClanLib/Display/Providers/provider_factory.h>
#include <ClanLib/Display/pixel_format.h>
#include <ClanLib/Display/display.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "physfs/physfs_stream.hpp"
#include "blitter.hpp"
#include "ttf_font.hpp"

TTFCharacter::TTFCharacter(const CL_Rect& pos_,
                           const CL_Rectf& uv_, 
                           int advance_)
  : pos(pos_),
    uv(uv_), 
    advance(advance_)
{
  
}

class TTFFontImpl
{
public:
  /// the global FreeType library handle
  static FT_Library      library;

  /** Array of characters available in this font, current limited to
      256 characters, no full unicode */
  std::vector<TTFCharacter> characters;

  int size;

  /** OpenGL Texture which holds all the characters */
  CL_OpenGLSurface surface;
};

FT_Library TTFFontImpl::library;

void 
blit_ftbitmap(CL_PixelBuffer target, const FT_Bitmap& brush, int x_pos, int y_pos)
{
  target.lock();
  
  int start_x = std::max(0, -x_pos);
  int start_y = std::max(0, -y_pos);
  
  int end_x = std::min(brush.width, target.get_width()  - x_pos);
  int end_y = std::min(brush.rows,  target.get_height() - y_pos);

  unsigned char* target_buf = static_cast<unsigned char*>(target.get_data());

  int target_pitch = target.get_pitch();

  for (int y = start_y; y < end_y; ++y)
    for (int x = start_x; x < end_x; ++x)
      {
        int target_pos = (y + y_pos) * target_pitch + 4*(x + x_pos);
        int brush_pos  = y * brush.pitch + x;
            
        target_buf[target_pos + 0] = brush.buffer[brush_pos];
        target_buf[target_pos + 1] = 255;
        target_buf[target_pos + 2] = 255;
        target_buf[target_pos + 3] = 255;
      }
    
  target.unlock();
}

TTFFont::TTFFont(const std::string& filename, int size)
  : impl(new TTFFontImpl())
{
  assert(size > 0);

  impl->size = size;

  IFileStream fin(filename);
  std::istreambuf_iterator<char> first(fin), last;
  std::vector<char> buffer(first, last); 

  FT_Face face;
  std::cout << "Buffer size: " << buffer.size() << std::endl;
  if (FT_New_Memory_Face(TTFFontImpl::library, reinterpret_cast<FT_Byte*>(&*buffer.begin()), buffer.size(), 0, &face))
    {
      throw std::runtime_error("Couldn't load font: '" + filename + "'");
    }
  
  FT_Set_Pixel_Sizes( face, size, size);

  FT_Select_Charmap(face,  FT_ENCODING_UNICODE);

  // FIXME: should calculate texture size, based on font size
  CL_PixelBuffer pixelbuffer(1024, 1024, 1024*4, CL_PixelFormat::rgba8888);

  int x_pos = 1;
  int y_pos = 1;

  // We limit ourself to 256 characters for the momemnt
  for(int glyph_index = 0; glyph_index < 256; glyph_index += 1)
    {
      if (FT_Load_Char( face,  glyph_index, FT_LOAD_RENDER))
        {
          // FIXME: happens when character is not in font, should be handled more gentle
          throw std::runtime_error("couldn't load char");
        }
      
      blit_ftbitmap(pixelbuffer, face->glyph->bitmap, x_pos, y_pos);
      generate_border(pixelbuffer, x_pos, y_pos, 
                      face->glyph->bitmap.width, face->glyph->bitmap.rows);

      CL_Rect pos(CL_Point(face->glyph->bitmap_left,  -face->glyph->bitmap_top), 
                  CL_Size (face->glyph->bitmap.width, face->glyph->bitmap.rows));

      CL_Rectf uv(x_pos/float(pixelbuffer.get_width()),
                  y_pos/float(pixelbuffer.get_height()),
                  (x_pos + face->glyph->bitmap.width)/float(pixelbuffer.get_width()),
                  (y_pos + face->glyph->bitmap.rows)/float(pixelbuffer.get_height()));
      
      impl->characters.push_back(TTFCharacter(pos, uv,
                                              face->glyph->advance.x >> 6));

      // we leave a one pixel border around the letters which we fill with generate_border
      x_pos += face->glyph->bitmap.width + 2;
      if (x_pos + size + 2 > pixelbuffer.get_width())
        {
          y_pos += size + 2;
          x_pos = 1;
        }

      if (y_pos + size + 2 > pixelbuffer.get_height())
        throw std::runtime_error("Font Texture to small");
    }
  FT_Done_Face(face);

  //CL_ProviderFactory::save(pixelbuffer, "/tmp/packfont.png");
  impl->surface = CL_OpenGLSurface(pixelbuffer);
}

TTFFont::~TTFFont()
{
}

TTFCharacter
TTFFont::get_character(int c)
{
  assert(c >= 0 && c < 256);
  return impl->characters[c];
}

int
TTFFont::get_height() const
{
  return impl->size;
}

void
TTFFont::draw(float x_pos, float y_pos, const std::string& str, const Color& color)
{
  CL_OpenGLState state(CL_Display::get_current_window()->get_gc());
  state.set_active();
  state.setup_2d();

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  impl->surface.bind();

  // Voodoo to get non-blurry fonts
  float mx = -0.375;
  float my = -0.375; 
  glBegin(GL_QUADS);
  glColor4f(color.r, color.g, color.b, color.a);
  for(std::string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
      TTFCharacter& character = impl->characters[*i];
      
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
  glEnd();
}

void
TTFFont::init()
{
  FT_Error  error;
  
  error = FT_Init_FreeType( &TTFFontImpl::library );
  if ( error )
    throw std::runtime_error( "could not initialize FreeType" );
}

void
TTFFont::deinit()
{
  FT_Done_FreeType( TTFFontImpl::library );
}

/* EOF */
