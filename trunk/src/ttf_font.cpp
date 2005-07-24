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

#include <iostream>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_IMAGE_H
#include FT_GLYPH_H

#include "ttf_font.hpp"

class TTFFontImpl
{
public:
  static FT_Library      library; /* the FreeType library            */
};

FT_Library TTFFontImpl::library;

TTFFont::TTFFont(const std::string& file, unsigned int size)
  : impl(new TTFFontImpl())
{
  // FIXME: Use FT_NewMemory_Face and physfs
  FT_Face face;
  if (FT_New_Face(TTFFontImpl::library, file.c_str(), 0, &face))
    {
      throw std::runtime_error("Couldn't load font: '" + file + "'");
    }
  
  // FIXME: font size in what units?
  //FT_Set_Char_Size( face, 0, size << 6, 0, 0 );
  FT_Set_Pixel_Sizes( face, size, size);
  std::cout << face->num_faces << std::endl;
  std::cout << face->face_index << std::endl;
  std::cout << face->face_flags << std::endl;
  std::cout << face->style_flags << std::endl;
  std::cout << face->family_name << std::endl;
  std::cout << face->style_name << std::endl;
  std::cout << face->num_charmaps << std::endl;
  std::cout << face->num_fixed_sizes << std::endl;

  FT_Select_Charmap(face,  FT_ENCODING_UNICODE);

  for(int glyph_index = 'M'; glyph_index <= 'M'; glyph_index += 1)
    {
      if (FT_Load_Char( face, glyph_index, FT_LOAD_RENDER))
        {
          throw std::runtime_error("couldn't load char");
        }

      FT_Glyph        glyph;                                         
      FT_BitmapGlyph  glyph_bitmap;    
      if (FT_Get_Glyph( face->glyph, &glyph ))
        {
          std::cout << "Something went wrong1" << std::endl;
        }

      if ( glyph->format != FT_GLYPH_FORMAT_BITMAP )
        {                                                              
          FT_Error error = FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL,  
                                               0, 1 );
          if ( error )
            throw std::runtime_error( "could not convert glyph" );
        }

      glyph_bitmap = (FT_BitmapGlyph)glyph;

      int pitch = glyph_bitmap->bitmap.pitch;
      
      std::cout << "\n\nCharacter: " << glyph_index << " '" << (char)glyph_index << "' "  
                << "Size: " << glyph_bitmap->bitmap.width << "x" << glyph_bitmap->bitmap.rows << std::endl;
      //std::cout << "pitch vs width: " << pitch << " vs " << glyph_bitmap->bitmap.width << std::endl;

      for(int y = 0; y < glyph_bitmap->bitmap.rows; ++y)
        {
          for(int x = 0; x < glyph_bitmap->bitmap.width; ++x)
            {
              std::cout.width(3);
              std::cout << int(glyph_bitmap->bitmap.buffer[pitch * y + x]) << " ";
            }  
          std::cout << std::endl;
        }
      FT_Done_Glyph( glyph );
    }
  FT_Done_Face(face);
}

TTFFont::~TTFFont()
{
}

Character
TTFFont::get_character(int c)
{
  return Character();
}

void
TTFFont::draw(const std::string& str)
{
  
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
