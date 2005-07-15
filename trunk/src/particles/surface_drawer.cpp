//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "particle_system.hpp"
#include "surface_drawer.hpp"

SurfaceDrawer::SurfaceDrawer(const CL_Surface& sur)
  : surface(sur)
{
}

SurfaceDrawer::~SurfaceDrawer() 
{
}
  
void
SurfaceDrawer::set_surface(const CL_Surface& sur)
{
  surface = sur;
  surface.set_alignment(origin_center);
}

void
SurfaceDrawer::draw(SceneContext& sc, ParticleSystem& psys) 
{          
  for(ParticleSystem::Particles::iterator i = psys.begin(); i != psys.end(); ++i)
    {
      // FIXME: use custom OpenGL here
      if (i->t != -1.0f)
        {
          float p = 1.0f - psys.get_progress(i->t);
          surface.set_color(CL_Color(int(psys.color_start.get_red()   * p + psys.color_stop.get_red()   * (1.0f - p)),
                                     int(psys.color_start.get_green() * p + psys.color_stop.get_green() * (1.0f - p)),
                                     int(psys.color_start.get_blue()  * p + psys.color_stop.get_blue()  * (1.0f - p)),
                                     int(psys.color_start.get_alpha() * p + psys.color_stop.get_alpha() * (1.0f - p))));

          surface.set_scale(psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start),
                            psys.size_start + psys.get_progress(i->t)*(psys.size_stop - psys.size_start));
          surface.set_angle(i->angle);
          
          sc.color().draw(surface, 
                          psys.get_x_pos() + i->x,
                          psys.get_y_pos() + i->y);
        }
    }
}

/* EOF */
