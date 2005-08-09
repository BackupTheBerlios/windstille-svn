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

#include "globals.hpp"
#include "player.hpp"
#include "sector.hpp"
#include "hedgehog.hpp"
#include "lisp/properties.hpp"
#include "game_session.hpp"
#include "view.hpp"
#include "windstille_getters.hpp"
#include "display/vertex_array_drawing_request.hpp"
#include "random.hpp"
#include "swarm.hpp"

Swarm::Swarm(const lisp::Lisp* lisp)
{
  int count = 100;
  turn_speed = 7.0f;

  lisp::Properties props(lisp);
  props.get("name",  name);
  props.get("pos",   pos);
  props.get("count", count);
  props.get("turn-speed", turn_speed);
  props.print_unused_warnings("swarm");

  agents.resize(count);

  for(Agents::iterator i = agents.begin(); i != agents.end(); ++i)
    {
      i->pos.x = pos.x + rnd.drand(-100, 100);
      i->pos.y = pos.y + rnd.drand(-100, 100);

      i->angle = rnd.drand(-M_PI, M_PI);
      i->speed = rnd.drand(50.0f, 200.0f);

      i->max_speed = rnd.drand(150.0f, 200.0f);

      i->last_pos = i->pos;
    }
}

void
Swarm::draw(SceneContext& sc)
{
  VertexArrayDrawingRequest* array = new VertexArrayDrawingRequest(Vector(0, 0), 
                                                                   1000.0f, sc.highlight().get_modelview());

  array->set_mode(GL_LINES);
  array->set_blend_func(GL_ONE, GL_ZERO);

  Color begin_color(1.0f, 1.0f, 1.0f);
  Color end_color(0.0f, 0.0f, 0.0f);

  for(Agents::const_iterator i = agents.begin(); i != agents.end(); ++i)
    {
      array->color(begin_color);
      array->vertex(i->last_pos.x, i->last_pos.y);

      array->color(end_color);
      array->vertex(i->pos.x, i->pos.y);
    }

  sc.highlight().draw(array);
}

void
Swarm::update(float delta)
{
  int x, y;
  SDL_GetMouseState(&x, &y);
  
  target = GameSession::current()->get_view()->screen_to_world(Vector(x, y));

  for(Agents::iterator i = agents.begin(); i != agents.end(); ++i)
    {
      i->last_pos = i->pos;

      float dx = target.x - i->pos.x;
      float dy = target.y - i->pos.y;

      float target_angle   = atan2f(dy, dx);
      float relative_angle = target_angle - i->angle;
      

      if (sqrt(dx*dx + dy*dy) > 50.0f) // swarm range
        {
          if (fabs(relative_angle) < 1.0f)
            {
              //i->angle += rnd.drand(-1.0f, 1.0f) * delta;
              if (i->speed < i->max_speed) 
                i->speed += 10.0f * delta;
            }
          else
            {
              if (fmod(relative_angle, 2*M_PI) > 0 && fmod(relative_angle, 2*M_PI) < M_PI)
                i->angle += turn_speed * delta;
              else
                i->angle -= turn_speed * delta;
            }
        }
      else
        {
          //i->angle += rnd.drand(-15.0f, 15.0f) * delta;
          //i->speed += 150.0f - fabs(i->angle);
        }

      i->pos.x += i->speed * cos(i->angle) * delta;
      i->pos.y += i->speed * sin(i->angle) * delta;
    }
}

/* EOF */
