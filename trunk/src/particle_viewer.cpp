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
#include "particle_viewer.hpp"

ParticleViewer::ParticleViewer()
{
  background = Sprite("images/greychess.sprite");
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

  for(int y = 0; y < 600; y += background.get_height())
    for(int x = 0; x < 800; x += background.get_width())
      sc.color().draw(background, Vector(x, y), -900);

  sc.translate(400, 600);
  sc.light().fill_screen(Color(0.2, 0.2, 0.2));
  //sc.light().fill_screen(Color(1.0, 1.0, 1.0));
  sc.color().fill_screen(Color(0.0, 0.0, 0.0));

  for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->draw(sc);

  sc.render();
}

void
ParticleViewer::update(float delta, const Controller& controller)
{
for(Systems::iterator i = systems.begin(); i != systems.end(); ++i)
    (*i)->update(delta);
}

/* EOF */
