#include <config.h>

#include "sprite2d/data.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include "util.hpp"
#include "lisp/list_iterator.hpp"
#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "glutil/texture.hpp"
#include "glutil/texture_manager.hpp"

namespace sprite2d
{

Data::Data(const std::string& filename)
{
  std::auto_ptr<lisp::Lisp> root (lisp::Parser::parse(filename));
  const lisp::Lisp* sprite = root->get_lisp("sprite");
  if(sprite == 0) {
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
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "action") {
      actions.push_back(parse_action(dir, iter.lisp()));
    } else {
      std::cerr << "Skipping unknown tag '" << iter.item() << "' in sprite.\n";
    }
  }

  if(actions.size() == 0)
    throw std::runtime_error("Sprite contains no actions");
}

Action*
Data::parse_action(const std::string& dir, const lisp::Lisp* lisp)
{
  std::auto_ptr<Action> action (new Action);
  action->speed = 1.0;
  action->offset = Vector(0, 0, 0);
  
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "name") {
      action->name = iter.value().get_string();
    } else if(iter.item() == "speed") {
      action->speed = iter.value().get_float();
    } else if(iter.item() == "x-offset") {
      action->offset.x = iter.value().get_float();
    } else if(iter.item() == "y-offset") {
      action->offset.y = iter.value().get_float();
    } else if(iter.item() == "z-offset") {
      // do we use that for anything anyway?
      action->offset.z = iter.value().get_float();
    } else if(iter.item() == "images") {
      parse_images(action.get(), dir, iter.lisp());
    } else if(iter.item() == "image-grid") {
      parse_image_grid(action.get(), dir, iter.lisp());
    }
  }

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
  while(lisp && lisp->get_type() == lisp::Lisp::TYPE_CONS) {
    const lisp::Lisp* cur = lisp->get_car();
    std::string file = cur->get_string();
    const Texture* texture = texture_manager->get(dir + "/" + file);
    
    ActionImage image;
    image.texture = texture->handle;
    image.width = texture->orig_width;
    image.height = texture->orig_height;
    float* uvs = image.texcoords;
    uvs[0] = 0;
    uvs[1] = 0;
    uvs[2] = texture->get_max_u();
    uvs[3] = 0;
    uvs[4] = texture->get_max_u();
    uvs[5] = texture->get_max_v();
    uvs[6] = 0;
    uvs[7] = texture->get_max_v();
    action->images.push_back(image);

    lisp = lisp->get_cdr();
  }
}

void
Data::parse_image_grid(Action* action, const std::string& dir,
                       const lisp::Lisp* lisp)
{
  std::string filename;
  int x_size = -1;
  int y_size = -1;
  
  lisp::ListIterator iter(lisp);
  while(iter.next()) {
    if(iter.item() == "file") {
      filename = iter.value().get_string();
    } else if(iter.item() == "x-size") {
      x_size = iter.value().get_int();
    } else if(iter.item() == "y-size") {
      y_size = iter.value().get_int();
    } else {
      std::cerr << "Skipping unknown element '" << iter.item() 
                << "' in image-grid declaration.\n";
    }
  }

  if(filename == "" || x_size <= 0 || y_size <= 0)
    throw std::runtime_error("Invalid or too few data in image-grid");

  const Texture* texture = texture_manager->get(dir + "/" + filename);

  if(texture->orig_width % x_size != 0 || texture->orig_height % y_size != 0) {
    std::cerr << "Warning texture '" << filename
              << "' doesn't match a grid size.\n";
  }

  for(int y = 0; y <= texture->orig_height - y_size; y += y_size) {
    for(int x = 0; x <= texture->orig_width - x_size; x += x_size) {
      ActionImage image;
      image.texture = texture->handle;
      image.width = x_size;
      image.height = y_size;
      // TODO: check if (x + x_size - 1) is correct or (x + x_size)
      float min_u = (texture->get_max_u() * x) / static_cast<float>(texture->orig_width);
      float max_u = (texture->get_max_u() * (x + x_size)) / static_cast<float>(texture->orig_width);
      float min_v = (texture->get_max_v() * y) / static_cast<float>(texture->orig_height);
      float max_v = (texture->get_max_v() * (y + y_size)) / static_cast<float>(texture->orig_height);
      
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
