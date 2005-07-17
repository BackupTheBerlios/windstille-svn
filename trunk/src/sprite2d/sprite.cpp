#include <config.h>

#include "sprite2d/sprite.hpp"

#include <math.h>
#include <GL/gl.h>
#include <ClanLib/gl.h>
#include <ClanLib/display.h>
#include "display/drawing_request.hpp"
#include "display/scene_context.hpp"
#include "sprite2d/data.hpp"
#include "util.hpp"

namespace sprite2d
{

Sprite::Sprite(const Data* data)
  : data(data)
{
  current_action = data->actions[0];
  vflip = false;
  frame = 0;
  speed = 1.0;
  pingpong = false;
  reverse = false;
  alpha = 0.0;
}

Sprite::~Sprite()
{
}

void
Sprite::update(float elapsed_time)
{
  float step = elapsed_time * speed * current_action->speed;
  if(reverse)
    step = -step;

  frame = fmodf(frame + current_action->images.size() + step,
      current_action->images.size());
}

void
Sprite::set_action(const std::string& name)
{
  for(Data::Actions::const_iterator i = data->actions.begin();
      i != data->actions.end(); ++i) {
    const Action* action = *i;
    if(action->name == name) {
      current_action = action;
      pingpong = false;
      reverse = false;
      speed = 1.0;
      frame = 0;
      vflip = false;
      alpha = 0.0;
      return;
    }
  }

  std::ostringstream msg;
  msg << "No action '" << name << "' defined";
  throw std::runtime_error(msg.str());
}

void
Sprite::set_vflip(bool vflip)
{
  this->vflip = vflip;
}

bool
Sprite::get_vflip() const
{
  return vflip;
}

void
Sprite::set_pingpong(bool pingpong)
{
  this->pingpong = pingpong;
}

bool
Sprite::get_pingpong() const
{
  return pingpong;
}

void
Sprite::set_speed(float speed)
{
  this->speed = speed;
}

float
Sprite::get_speed() const
{
  return speed;
}

void
Sprite::set_alpha(float alpha)
{
  this->alpha = alpha;
}

float
Sprite::get_alpha() const
{
  return alpha;
}

class SpriteDrawingRequest : public DrawingRequest
{
private:
  Sprite* sprite;
  
public:
  SpriteDrawingRequest(Sprite* sprite, const Vector& pos,
      const Matrix& modelview)
    : DrawingRequest(pos, modelview), sprite(sprite)
  {
  }

  void draw(CL_GraphicContext* gc)
  {
    sprite->draw(gc, pos, modelview);
  }
};

void
Sprite::draw(CL_GraphicContext* gc, const Vector& pos, const Matrix& modelview)
{
  static const float rectvertices[12] 
    = { 0, 0, 0,
        1, 0, 0,
        1, 1, 0, 
        0, 1, 0 };

  CL_OpenGLState state(gc);
  state.set_active();
  state.setup_2d();

  glPushMatrix();
  glMultMatrixd(modelview);
  glTranslatef(pos.x, pos.y, 0);
  if(vflip)
    glRotatef(180, 0, 1.0, 0);
  
  // TODO: move alot of this state management to DrawingContext class...
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if 1
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
#endif
  glColor4f(1.0, 1.0, 1.0, alpha);

  const ActionImage& image 
    = current_action->images[ static_cast<int> (frame) ];
  glScalef(image.width, image.height, 1.0);

  glBindTexture(GL_TEXTURE_2D, image.texture);

  // TODO find out which of the 2 is faster
#if 1
  glVertexPointer(3, GL_FLOAT, 0, rectvertices);
  glTexCoordPointer(2, GL_FLOAT, 0, image.texcoords);
  glDrawArrays(GL_QUADS, 0, 4);
#else
  glBegin(GL_QUADS);
  for(int i = 0; i < 4; ++i) {
    glVertex3f(rectvertices[i*3], rectvertices[i*3+1], rectvertices[i*3+2]);
    glTexCoord2d(image.texcoords[i*2], image.texcoords[i*2+1]);
  }
  glEnd();
#endif

  glPopMatrix();

  assert_gl("drawing 2d sprite");
}

void
Sprite::draw(SceneContext& sc, const Vector& pos)
{
  sc.color().draw(
    new SpriteDrawingRequest(this, pos, sc.color().get_modelview()));
}

}
