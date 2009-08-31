/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2005 Ingo Ruhnke <grumbel@gmx.de>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**  
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**  
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "screen/particle_viewer.hpp"

#include <stdexcept>
#include <sstream>

#include "app/menu_manager.hpp"
#include "display/display.hpp"
#include "display/graphic_context_state.hpp"
#include "scenegraph/particle_system_drawable.hpp"
#include "input/controller.hpp"
#include "util/sexpr_file_reader.hpp"

ParticleViewer::ParticleViewer()
  : compositor(Display::get_size()),
    sc(),
    sg(),
    systems(),
    background(Pathname("images/greychess.sprite")),
    pos()
{
}

ParticleViewer::~ParticleViewer()
{
}

void
ParticleViewer::load(const Pathname& filename)
{
  std::cout << "ParticleViewer: loading " << filename << std::endl;

  // Cleanup
  systems.clear();
  
  FileReader root_reader = FileReader::parse(filename);
  if(root_reader.get_name() != "particle-systems") 
  {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a particle-system file";
    throw std::runtime_error(msg.str());
  }

  std::vector<FileReader> sections = root_reader.get_sections();
  for(std::vector<FileReader>::iterator i = sections.begin(); i != sections.end(); ++i)
    { 
      if (i->get_name() == "particle-system")
        {
          systems.push_back(boost::shared_ptr<ParticleSystem>(new ParticleSystem(*i)));
        }
    }

  std::cout << systems.size() << " particle systems ready to go" << std::endl;

  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
  {
    sg.add_drawable(boost::shared_ptr<Drawable>(new ParticleSystemDrawable(**i)));
  }
}
  
void
ParticleViewer::draw()
{
  sc.reset_modelview();

  for(float y = -background.get_width(); 
      y < static_cast<float>(Display::get_height()) + background.get_height(); 
      y += background.get_height())
  {
    for(float x = -background.get_width(); 
        x < static_cast<float>(Display::get_width()) + background.get_width(); 
        x += background.get_width())
    {
      sc.color().draw(background, Vector2f(x + fmodf(pos.x, background.get_width()),
                                           y + fmodf(pos.y, background.get_height())), -900);
    }
  }

  // Ambient lighting
  sc.light().fill_screen(Color(0.4f, 0.4f, 0.4f));
  
  GraphicContextState state(Display::get_width(), Display::get_height());
  state.set_pos(-pos);
  compositor.render(sc, &sg, state);
}

void
ParticleViewer::update(float delta, const Controller& controller)
{
  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->update(delta);

  pos.x -= controller.get_axis_state(X_AXIS) * delta * 400.0f;
  pos.y -= controller.get_axis_state(Y_AXIS) * delta * 400.0f;

  if (controller.button_was_pressed(PAUSE_BUTTON) ||
      controller.button_was_pressed(ESCAPE_BUTTON))
    {
      MenuManager::display_pause_menu();
    }
}

/* EOF */
