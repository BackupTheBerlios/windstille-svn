#include <config.h>

#include "sprite2d/data.hpp"

#include <physfs.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include "util.hpp"
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/properties.hpp"
#include "windstille_getters.hpp"
#include "glutil/surface.hpp"
#include "glutil/surface_manager.hpp"

namespace sprite2d
{

Data::Data(const std::string& filename)
{
  if (PHYSFS_exists(filename.c_str()))
    {
      std::auto_ptr<lisp::Lisp> root (lisp::Parser::parse(filename));
      lisp::Properties rootp(root.get());
      const lisp::Lisp* sprite = 0;
      if(rootp.get("sprite", sprite) == false) {
        std::ostringstream msg;
        msg << "File '" << filename << "' is not a windstille sprite";
        throw std::runtime_error(msg.str());
      }
    
      std::string dir = dirname(filename);
      parse(dir, sprite);
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
Data::parse(const std::string& dir, const lisp::Lisp* lisp)
{
  lisp::Properties props(lisp);
  lisp::PropertyIterator<const lisp::Lisp*> iter;
  props.get_iter("action", iter);
  while(iter.next()) {
    actions.push_back(parse_action(dir, *iter));
  }
  props.print_unused_warnings("sprite");

  if(actions.size() == 0)
    throw std::runtime_error("Sprite contains no actions");
}

Action*
Data::parse_action(const std::string& dir, const lisp::Lisp* lisp)
{
  std::auto_ptr<Action> action (new Action);
  action->speed = 1.0;
  action->offset = Vector(0, 0);
 
  lisp::Properties props(lisp);
  props.get("name", action->name);
  props.get("speed", action->speed);
  props.get("offset", action->offset);
  
  const lisp::Lisp* ilisp = 0;
  if(props.get("images", ilisp)) {
    parse_images(action.get(), dir, ilisp);
  } else if(props.get("image-grid", ilisp)) {
    parse_image_grid(action.get(), dir, ilisp);
  }
  props.print_unused_warnings("sprite action");
  
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
                   const lisp::Lisp* lisp)
{
  for(size_t n = 1; n < lisp->get_list_size(); ++n) {
    std::string file = lisp->get_list_elem(n)->get_string();
    Surface surface = surface_manager->get(dir + "/" + file);
    action->surfaces.push_back(surface);
  }
}

void
Data::parse_image_grid(Action* action, const std::string& dir,
                       const lisp::Lisp* lisp)
{
  std::string filename;
  int x_size = -1;
  int y_size = -1;
 
  lisp::Properties props(lisp);

  props.get("file", filename);
  props.get("x-size", x_size);
  props.get("y-size", y_size);

  props.print_unused_warnings("action image-grid");

  if(filename == "" || x_size <= 0 || y_size <= 0)
    throw std::runtime_error("Invalid or too few data in image-grid");

  surface_manager->load_grid(dir + "/" + filename,
                             action->surfaces, x_size, y_size);
}
 
}
