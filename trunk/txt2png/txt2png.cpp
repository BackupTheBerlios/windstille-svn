#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "ttf_font.hpp"

void render(Bitmap& canvas, TTFFont& font, const std::string& text)
{
  int start_x = 4;
  int start_y = 4+10;

  int page = 0;
  int x = start_x;
  int y = start_y; // <- refers to the baseline of the font!

  for(std::string::size_type i = 0; i < text.size(); ++i)
    {
      if (text[i] == '\n')
        {
          x = start_x;
          y += font.get_height();

          if (y >= canvas.get_height() - start_y + font.get_height())
            { // FIXME :Duplicate code
              y = start_y;
              x = start_x;

              std::ostringstream str;
              str << "out/page" << std::setfill('0') << std::setw(3) << page << ".pgm";
              std::cout << "Writing: " << str.str() << std::endl;
              canvas.write_pgm(str.str());
              canvas.clear();

              page += 1;
            }
        }
      else if (text[i] == '\r')
        {
          // ignore
        }
      else if (text[i] == '\t')
        {
          int cw = font.get_character(' ').advance * 8;
          x = start_x + ((((x-start_x)/cw) + 1) * cw);
        }
      else
        {
          const TTFCharacter& glyph = font.get_character(text[i]);

          canvas.blit(*glyph.bitmap, 
                      x + glyph.x_offset,
                      y + glyph.y_offset);

          x += glyph.advance;

          if (x >= canvas.get_width())
            {
              std::cout << "Illegal line break on page " << page << std::endl;
              x = start_x;
              y += font.get_height();              
            }

          if (y >= canvas.get_height() - start_y + font.get_height())
            {
              y = start_y;
              x = start_x;

              std::ostringstream str;
              str << "out/page" << std::setfill('0') << std::setw(3) << page << ".pgm";
              std::cout << "Writing: " << str.str() << std::endl;
              canvas.write_pgm(str.str());
              canvas.clear();

              page += 1;
            }
        }
    }
}

int main(int argc, char** argv)
{
  if (argc != 4)
    {
      std::cout << "Usage: " << argv[0] << " FONT SIZE TEXT" << std::endl;
    }
  else
    {
      std::string font_filename = argv[1];
      int font_size = atoi(argv[2]);
      std::string text_filename = argv[3];

      TTFFont::init();

      //std::cout << "Load font" << std::endl;
      TTFFont font(font_filename, font_size);

      //  std::cout << "Create bitmap" << std::endl;
      Bitmap  bitmap(480, 272);

      std::ifstream fin(text_filename.c_str());
      std::istreambuf_iterator<char> first(fin), last;
      std::string text(first, last); 

      //std::cout << "Render..." << std::endl;
      render(bitmap, font, text);
  
      TTFFont::deinit();
    }
  return 0;
}

/* EOF */
