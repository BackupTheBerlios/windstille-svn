#ifndef __SPRITE_DATA_HPP__
#define __SPRITE_DATA_HPP__

#include <string>
#include <vector>
#include <GL/gl.h>
#include "math/vector.hpp"
#include "lisp/lisp.hpp"

namespace sprite2d
{
struct Action;
struct ActionImage;

class Data
{
public:
  Data(const std::string& filename);
  virtual ~Data();

  typedef std::vector<Action*> Actions;
  Actions actions;

private:
  void parse(const std::string& dir, const lisp::Lisp* lisp);
  
  Action* parse_action(const std::string& dir, const lisp::Lisp* lisp);
  void parse_images(Action* action, const std::string& dir, 
                    const lisp::Lisp* lisp);
  void parse_image_grid(Action* action, const std::string& dir,
                        const lisp::Lisp* lisp); 
};

struct ActionImage
{
  GLuint texture;
  float width;
  float height;
  float texcoords[8];
};

struct Action
{
  std::string name;
  float speed;
  Vector offset;
  typedef std::vector<ActionImage> Images;
  Images images;
};

}

#endif

