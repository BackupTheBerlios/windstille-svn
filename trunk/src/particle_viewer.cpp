/*  $Id$
**   __      __ __             ___        __   __ __   __
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
**  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
**  02111-1307, USA.
*/

#include "lisp/lisp.hpp"
#include "lisp/parser.hpp"
#include "lisp/properties.hpp"
#include "file_reader.hpp"
#include "input/controller.hpp"
#include "screen_manager.hpp"
#include "gui/gui_manager.hpp"
#include "particle_viewer.hpp"

// Components
#include "gui/button.hpp"
#include "gui/slider.hpp"
#include "gui/root_component.hpp"
#include "gui/grid_component.hpp"
#include "gui/tab_component.hpp"
#include "gui/list_view.hpp"
#include "gui/text_view.hpp"
#include "gui/automap.hpp"

ParticleViewer::ParticleViewer()
{
  background = Sprite("images/greychess.sprite");
  show_gui = false;

  using namespace GUI;

  manager = new GUIManager();

  GridComponent* grid = new GridComponent(Rectf(350, 20, 450, 120), 1, 3, manager->get_root());
  grid->set_padding(4);

  gravity_slider  = new Slider(grid);
  velocity_slider = new Slider(grid);
  count_slider    = new Slider(grid);


  gravity_slider->set_range(-10, 10);
  gravity_slider->set_step(1);
  velocity_slider->set_pos(1);

  velocity_slider->set_range(-500, 500);
  velocity_slider->set_step(10);
  velocity_slider->set_pos(200);

  count_slider->set_range(1, 200);
  count_slider->set_step(10);
  count_slider->set_pos(50);

  grid->pack(gravity_slider,  0, 0);
  grid->pack(velocity_slider, 0, 1);
  grid->pack(count_slider,    0, 2);


  manager->get_root()->set_child(grid);
}

ParticleViewer::~ParticleViewer()
{
}

void
ParticleViewer::load(const std::string& filename)
{
  std::cout << "ParticleViewer: loading " << filename << std::endl;
  using namespace lisp;
  
  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    delete *i;
  systems.clear();
  
  std::auto_ptr<Lisp> root(Parser::parse(filename));
  Properties rootp(root.get());

  const Lisp* sector = 0;
  if(!rootp.get("particle-systems", sector)) {
    std::ostringstream msg;
    msg << "'" << filename << "' is not a particle-system file";
    throw std::runtime_error(msg.str());
  }
  rootp.print_unused_warnings("sector");
  
  Properties props(sector);
  PropertyIterator<const Lisp*> iter = props.get_iter();
  while(iter.next()) {
    if (iter.item() == "particle-system")
      {
        FileReader reader(*iter);
        systems.push_back(new ParticleSystem(reader));
      }
  }

  std::cout << systems.size() << " particle systems ready to go" << std::endl;
}
  
void
ParticleViewer::draw()
{
  sc.reset_modelview();

  for(int y = -background.get_width(); y < 600 + background.get_height(); y += background.get_height())
    for(int x = -background.get_width(); x < 800 + background.get_width(); x += background.get_width())
      sc.color().draw(background, Vector(x + int(pos.x) % background.get_width(),
                                         y + int(pos.y) % background.get_height()), -900);

  sc.translate(400 + pos.x, 600 + pos.y);
  sc.light().fill_screen(Color(0.4, 0.4, 0.4));
  //sc.light().fill_screen(Color(1.0, 1.0, 1.0));
  sc.color().fill_screen(Color(0.0, 0.0, 0.0));

  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->draw(sc);

  sc.render();

  if (show_gui)
    {
      manager->draw();
    }
}

void
ParticleViewer::update(float delta, const Controller& controller)
{
  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->update(delta);

  if (!show_gui)
    {
      pos.x += controller.get_axis_state(X_AXIS) * delta * 100.0f;
      pos.y += controller.get_axis_state(Y_AXIS) * delta * 100.0f;

      if (controller.button_was_pressed(OK_BUTTON))
        {
          show_gui = true;
          std::cout << "Show GUI" << std::endl;       
        }
    }
  else
    {
      if (!manager->get_root()->is_active())
        {
          std::cout << "Hide GUI" << std::endl;
          show_gui = false;
        }
      else
        {
          manager->update(delta, controller);
        }
    }

  //systems[3]->set_count(count_slider->get_pos());
  //systems[3]->set_velocity(velocity_slider->get_pos(), velocity_slider->get_pos());
}

/* EOF */
