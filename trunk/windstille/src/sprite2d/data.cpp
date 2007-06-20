/*  $Id: windstille.hpp 1460 2007-06-18 04:03:31Z grumbel $
**   __      __ __             ___        __   __ __   __
**  /  \    /  \__| ____    __| _/_______/  |_|__|  | |  |   ____
**  \   \/\/   /  |/    \  / __ |/  ___/\   __\  |  | |  | _/ __ \
**   \        /|  |   |  \/ /_/ |\___ \  |  | |  |  |_|  |_\  ___/
**    \__/\  / |__|___|  /\____ /____  > |__| |__|____/____/\___  >
**         \/          \/      \/    \/                         \/
**  Copyright (C) 2005,2007 Matthias Braun <matze@braunis.de>,
**                          Ingo Ruhnke <grumbel@gmx.de>
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

#include <config.h>

#include "sprite2d/data.hpp"

#include <physfs.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include "util.hpp"
#include "display/surface.hpp"
#include "display/surface_manager.hpp"

namespace sprite2d {

static bool has_suffix(const std::string& str, const std::string& suffix)
{
  if (str.length() >= suffix.length())
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
  else
    return false;
}

static std::string tolowercase(const std::string& str)
{
  std::string out;
  for(std::string::const_iterator i = str.begin();  i != str.end(); ++i)
    out += tolower(*i);
  return out;
}

Data::Data(const std::string& filename)
{
  if (PHYSFS_exists(filename.c_str()))
    {
      if (has_suffix(filename, ".sprite"))
        {
          FileReader reader = FileReader::parse(filename);

          if(reader.get_name() != "sprite") {
            std::ostringstream msg;
            msg << "File '" << filename << "' is not a windstille sprite";
            throw std::runtime_error(msg.str());
          }
    
          std::string dir = dirname(filename);
          parse(dir, reader);
        }
      else if (has_suffix(tolowercase(filename), ".png") || has_suffix(tolowercase(filename), ".jpg"))
        {
          if (PHYSFS_exists(filename.c_str()))
            {
              std::auto_ptr<Action> action(new Action);
              action->name   = "default";
              action->speed  = 1.0;
              action->scale  = 1.0f;
              action->offset = Vector(0, 0);
              action->surfaces.push_back(Surface(filename));
              actions.push_back(action.release());
            }
          else
            {
              throw std::runtime_error("Couldn't find '" + filename + "'");
            }
        }
      else
        {
          throw std::runtime_error("Sprite filename has unknown suffix: '" + filename + "'");
        }
    }
  else if (filename.length() > std::string(".sprite").length())
    { // If sprite file is not found, we search for a file with the
      // same name ending in .png
      std::string pngfile = filename.substr(0, filename.length() - std::string(".sprite").length()) + ".png";

      if (PHYSFS_exists(pngfile.c_str()))
        {
          std::auto_ptr<Action> action(new Action);
          action->name   = "default";
          action->speed  = 1.0;
          action->scale  = 1.0f;
          action->offset = Vector(0, 0);
          action->surfaces.push_back(Surface(pngfile));
          actions.push_back(action.release());
        }
      else
        {
          throw std::runtime_error("Couldn't find '" + filename + "'");
        }
    }
  else
    {
      throw std::runtime_error("Couldn't find '" + filename + "'");
    }
}

Data::~Data()
{
  for(Actions::iterator i = actions.begin(); i != actions.end(); ++i)
    delete *i;
}

void
Data::parse(const std::string& dir, FileReader& reader)
{
  std::vector<FileReader> sections = reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    actions.push_back(parse_action(dir, *i));
  
  if(actions.size() == 0)
    throw std::runtime_error("Sprite contains no actions");
}

Action*
Data::parse_action(const std::string& dir, FileReader& reader)
{
  std::auto_ptr<Action> action (new Action);
  action->speed = 1.0;
  action->scale = 1.0f;
  action->offset = Vector(0, 0);
 
  reader.get("name", action->name);
  reader.get("speed", action->speed);
  reader.get("scale", action->scale);
  reader.get("offset", action->offset);
  
  FileReader grid_reader;
  std::vector<std::string> image_files;
  if(reader.get("images", image_files))
    {
      //parse_images(action.get(), dir, images);

      for(std::vector<std::string>::iterator file = image_files.begin(); file != image_files.end(); ++file)
        {
          action->surfaces.push_back(surface_manager->get(dir + "/" + *file));
        }
    }
  else if(reader.get("image-grid", grid_reader)) 
    {
      std::string filename;
      int x_size = -1;
      int y_size = -1;
      
      grid_reader.get("file", filename);
      grid_reader.get("x-size", x_size);
      grid_reader.get("y-size", y_size);

      grid_reader.print_unused_warnings("action image-grid");

      if(filename == "" || x_size <= 0 || y_size <= 0)
        throw std::runtime_error("Invalid or too few data in image-grid");
      
      surface_manager->load_grid(dir + "/" + filename,
                                 action->surfaces, x_size, y_size);
    }
  reader.print_unused_warnings("sprite action");
  
  if(action->name == "")
    throw std::runtime_error("No Name defined for action");
  if(action->surfaces.size() == 0) {
    std::ostringstream msg;
    msg << "Action '" << action->name << "' contains no images";
    throw std::runtime_error(msg.str());
  }
  return action.release();
}

void
Data::parse_images(Action* action, const std::string& dir,
                   FileReader& reader)
{

}

void
Data::parse_image_grid(Action* action, const std::string& dir,
                       FileReader& reader)
{

}
 
} // namespace sprite2d

/* EOF */
