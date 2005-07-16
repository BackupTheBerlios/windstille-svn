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

#include "globals.hpp"
#include "random.hpp"
#include "nightvision.hpp"

Nightvision::Nightvision(const lisp::Lisp* lisp)
  : nightvision("nightvision", resources),
    noise("noise", resources)
{
  name = "nightvision";
}

Nightvision::~Nightvision()
{
}

void
Nightvision::draw(SceneContext& sc)
{
  // reset the modelview, so we can draw in screen coordinates
  sc.light().push_modelview();
  sc.light().set_modelview(CL_Matrix4x4(true));

  // try to stay above everything else with large z value
  nightvision.set_alpha(1.0f);
  nightvision.set_blend_func(blend_src_alpha, blend_one_minus_src_alpha);
  sc.light().draw(nightvision, 0, 0, 10000);

  // FIXME: Use raw OpenGL here and offset the texture coordinates
  noise.set_scale(5.0f, 5.0f);
  noise.set_blend_func(blend_dest_color, blend_zero);
  sc.light().draw(noise, rnd.drand(-300, 300), rnd.drand(-300, 300), 11000);

  sc.light().pop_modelview();

  sc.highlight().push_modelview();
  sc.highlight().set_modelview(CL_Matrix4x4(true));
  nightvision.set_alpha(0.5f);
  nightvision.set_blend_func(blend_src_alpha, blend_one);
  sc.highlight().draw(nightvision, 0, 0, 10000);
  sc.highlight().pop_modelview();
}

void
Nightvision::update(float delta)
{
}

/* EOF */
