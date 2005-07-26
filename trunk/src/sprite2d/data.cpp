#include <config.h>

#include "sprite2d/data.hpp"

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
  action->offset = Vector(0, 0, 0);
 
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
  if(action->images.size() == 0) {
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
    const Surface* surface = surface_manager->get(dir + "/" + file);
    ActionImage image;
    image.surface      = surface;
    image.width        = surface->get_width();
    image.height       = surface->get_height();
    image.texcoords[0] = surface->get_uv().left;
    image.texcoords[1] = surface->get_uv().top;
    image.texcoords[2] = surface->get_uv().right;
    image.texcoords[3] = surface->get_uv().top;
    image.texcoords[4] = surface->get_uv().right;
    image.texcoords[5] = surface->get_uv().bottom;
    image.texcoords[6] = surface->get_uv().left;
    image.texcoords[7] = surface->get_uv().bottom;
    action->images.push_back(image);
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

  const Surface* surface = surface_manager->get(dir + "/" + filename);

  if(surface->get_width() % x_size != 0 || surface->get_height() % y_size != 0) {
    std::cerr << "Warning texture '" << filename
              << "' doesn't match a grid size.\n";
  }

  for(int y = 0; y <= surface->get_height() - y_size; y += y_size) {
    for(int x = 0; x <= surface->get_width() - x_size; x += x_size) {
      ActionImage image;
      image.surface = surface;
      image.width   = x_size;
      image.height  = y_size;

      // TODO: check if (x + x_size - 1) is correct or (x + x_size)
      float min_u = (surface->get_uv().right * x) / static_cast<float>(surface->get_width());
      float max_u = (surface->get_uv().right * (x + x_size)) / static_cast<float>(surface->get_width());
      float min_v = (surface->get_uv().bottom * y) / static_cast<float>(surface->get_height());
      float max_v = (surface->get_uv().bottom * (y + y_size)) / static_cast<float>(surface->get_height());
      
      float* uvs = image.texcoords;
      uvs[0] = min_u;
      uvs[1] = min_v;
      uvs[2] = max_u;
      uvs[3] = min_v;
      uvs[4] = max_u;
      uvs[5] = max_v;
      uvs[6] = min_u;
      uvs[7] = max_v;

      action->images.push_back(image);
    }
  }
}
 
}
