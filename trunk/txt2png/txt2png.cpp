#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "command_line.hpp"
#include "ttf_font.hpp"

struct Options {
  std::string font_file;
  int font_size;
  std::string output_file;
  std::string text_file;
  std::string type;
  std::string title;
  bool status;
  int canvas_width;
  int canvas_height;
  int text_x;
  int text_y;
  int text_w;
  int text_h;
  bool area_set;
  bool break_lines;
  bool info_mode;

  Options()
  {
    font_file = "VeraMono.ttf";
    font_size = 10;
    output_file = "out/page";
    type = "jpg";
      
    status = false;
      
    canvas_width  = 480;
    canvas_height = 272;
      
    text_x = -1;
    text_y = -1;
    text_w = -1;
    text_h = -1;
    area_set = false;
    
    break_lines = false;
    info_mode   = false;
  }
};

Options options;

void render(Bitmap& canvas, TTFFont& font, const std::string& text, 
            int start_x, int start_y,
            int text_width, int text_height)
{
  int page = 0;
  int x = 0;
  int y = 0; // <- refers to the baseline of the font!

  for(std::string::size_type i = 0; i < text.size(); ++i)
    {
      if (text[i] == '\n')
        {
          x = 0;
          y += font.get_height();

          if (y >= text_height)
            { // FIXME :Duplicate code
              y = 0;
              x = 0;
              
              std::ostringstream tstr;
              if (!options.title.empty())
                tstr << options.title << " ";

              tstr << "Page "
                   << std::setfill('0') << std::setw(2) << page << "/21 - " 
                   << std::setw(3) << 100 * page / 21 << "%";
              render(canvas, font, tstr.str(),
                     canvas.get_width()/2 - font.get_width(tstr.str())/2, 
                     canvas.get_height() - font.get_height() + font.get_character('M').bitmap->get_height()+2,
                     canvas.get_width(), canvas.get_height());
              canvas.invert(0, canvas.get_height() - font.get_height(),
                            canvas.get_width(), canvas.get_height());

              std::ostringstream str;
              str << "out/page" << std::setfill('0') << std::setw(3) << page << ".jpg";
              std::cout << "Writing: " << str.str() << std::endl;
              canvas.write_jpg(str.str());
              canvas.clear();

              page += 1;
            }
        }
      else if (text[i] == '\r')
        {
          // ignore so both DOS and Unix files look fine
        }
      else if (text[i] == '\t')
        {
          int cw = font.get_character(' ').advance * 8;
          x = (x/cw + 1) * cw;
        }
      else
        {
          const TTFCharacter& glyph = font.get_character(text[i]);

          canvas.blit(*glyph.bitmap, 
                      x + glyph.x_offset + start_x,
                      y + glyph.y_offset + start_y);

          x += glyph.advance;

          if (x >= text_width)
            {
              std::cout << "Illegal line break on page " << page << std::endl;
              x = 0;
              y += font.get_height();              
            }

          if (y >= text_height)
            {
              y = 0;
              x = 0;

              canvas.invert(0, canvas.get_height() - font.get_height(),
                            canvas.get_width(), canvas.get_height());


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

void parse_args(int argc, char** argv, Options& options)
{
  CommandLine argp;

  argp.set_help_indent(24);
  argp.add_usage ("[OPTION]... TEXTFILE");
  argp.add_doc   ("text2image converts ascii text files to images");

  argp.add_group("Options:");
  argp.add_option('f', "font",      "FONT",    "Use FONT for rendering the text, must be a ttf font (default: VerdanaMono.ttf)");
  argp.add_option('s',  "size",     "NUM",     "Set the font size to NUM (default: 12)");
  argp.add_option('o', "output",    "FILE",    "Write the rendered image to file");
  argp.add_option('t',  "type",     "TYPE",    "Use TYPE format when writing the file, can be 'jpg' or 'pgm'");
  argp.add_option('l',  "status-line",   "",        "Add a statusbar to the rendering output");
  argp.add_option('a',  "area", "WxH+X+Y","limit text rendering to the given area");
  argp.add_option('n',  "title",    "NAME",    "Use NAME as title in the status line");
  argp.add_option('c',  "canvas",   "WxH",      "Use a canvas for size WxH+X+Y");
  argp.add_option('b',  "break",    "",        "Break long lines");
  argp.add_option('i',  "info",     "",        "Print information about the text");
  argp.add_option('h',  "help",     "",        "Display this help");
  
  argp.parse_args(argc, argv);

  while (argp.next())
    {
      switch (argp.get_key())
        {
        case 'f': // font
          options.font_file = argp.get_argument();
          break;

        case 'o': // output
          options.output_file = argp.get_argument();
          break;

        case 't': // type
          if (argp.get_argument() != "jpg" || argp.get_argument() != "pgm")
            {
              std::cerr << "Only 'pgm' or 'jpg' allowed as format type options" << std::endl;
              exit(EXIT_FAILURE);              
            }
          options.type = argp.get_argument();
          break;

        case 'a': // area
          if (sscanf(argp.get_argument().c_str(), "%dx%d+%d+%d",
                     &options.text_w,
                     &options.text_h,
                     &options.text_x,
                     &options.text_y) != 4)
            {
              std::cerr << "Expected WIDTHxHEIGHT+X+Y as argument" << std::endl;
              exit(EXIT_FAILURE);
            }

          options.area_set = true;
          break;

        case 'c': // canvas
          if (sscanf(argp.get_argument().c_str(), "%dx%d", &options.canvas_width, &options.canvas_height) != 2)
            {
              std::cerr << "Expected WIDTHxHEIGHT as argument" << std::endl;
              exit(EXIT_FAILURE);
            }
          break;

        case 'i': // info
          options.info_mode = true;
          break;

        case 'n': // title
          options.title = argp.get_argument();
          break;

        case 'b': // break
          options.break_lines = true;
          break;

        case 'l': // status-line
          options.status = true;
          break;

        case 's': // size
          if (sscanf(argp.get_argument().c_str(), "%d", &options.font_size) != 1)
            {
              std::cerr << "Expected Integer as argument for --size" << std::endl;
              exit(EXIT_FAILURE);
            }

          break;

        case 'h':
          argp.print_help();
          exit(EXIT_SUCCESS);
          break;

        case CommandLine::REST_ARG:
          if (options.text_file.empty())
            {
              options.text_file = argp.get_argument();
            }
          else
            {
              std::cerr << "Only one text file can be given" << std::endl;
              exit(EXIT_FAILURE); 
            }
          break;

        default: 
          std::cerr << "Unknown argument: " << argp.get_key() << std::endl;
          exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv, options);

  if (options.text_file.empty())
    {
      std::cout << "Usage: " << argv[0] << " [OPTION]... TEXTFILE" << std::endl;
    }
  else
    {
      TTFFont::init();

      //std::cout << "Load font" << std::endl;
      TTFFont font(options.font_file, options.font_size);

      //  std::cout << "Create bitmap" << std::endl;
      Bitmap  bitmap(options.canvas_width, options.canvas_height);

      std::ifstream fin(options.text_file.c_str());
      std::istreambuf_iterator<char> first(fin), last;
      std::string text(first, last); 

      //std::cout << "Render..." << std::endl;

      if (options.area_set)
        render(bitmap, font, text, options.text_x, options.text_y, options.text_w, options.text_h);
      else
        render(bitmap, font, text,
               4, font.get_height(), bitmap.get_width() - 4, bitmap.get_height() - font.get_height()*2);
  
      TTFFont::deinit();
    }

  return 0;
}

/* EOF */
