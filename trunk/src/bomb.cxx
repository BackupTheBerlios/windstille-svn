//  $Id: bomb.cxx,v 1.4 2003/09/28 16:58:03 grumbel Exp $
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

#include "globals.hxx"
#include "igel.hxx"
#include "sector.hxx"
#include "bomb.hxx"

Bomb::Bomb(int x, int y)
  : sprite("bomb", resources),
    explo("explo", resources),
    light("bomblight", resources),
    highlight("bombhighlight", resources),
    explolight("explolight", resources),
   pos(x,
        (y/SUBTILE_SIZE+1)*SUBTILE_SIZE),
    count(2.0f),
    state(COUNTDOWN),
    exploded(false)
{
  light.set_blend_func(blend_src_alpha, blend_one);
  highlight.set_blend_func(blend_src_alpha, blend_one);
  explolight.set_blend_func(blend_src_alpha, blend_one);
}

Bomb::~Bomb()
{
}

void
Bomb::update(float delta)
{
  if (explo.is_finished())
    remove();

  if (state == EXPLODE)
    explo.update(delta);
  else
    sprite.update(delta);

  count -= delta;

  if (count < 0 && state != EXPLODE)
    {
      state = EXPLODE;
      count = 0;
      if (!exploded)
        {
          exploded = true;
          explode();
        }

    }
}

void
Bomb::draw(SceneContext& sc)
{
  if (state == EXPLODE)
    {
      explo.draw(pos.x, pos.y);
      sc.light().draw(explolight, pos.x, pos.y, 0);
      explolight.set_alpha(0.5);
      explolight.set_scale(.5, .5);
      sc.highlight().draw(explolight, pos.x, pos.y, 0);
      explolight.set_alpha(1.0);
      explolight.set_scale(1.0, 1.0);
    }
  else
    {
      sprite.draw(pos.x, pos.y);
      if (sprite.get_current_frame() == 0) {
        sc.light().draw(light, pos.x, pos.y, 0);
        sc.highlight().draw(highlight, pos.x, pos.y, 0);
      }
    }
}

void 
Bomb::explode()
{
  std::vector<GameObj*>* objs = Sector::current()->get_objects();
  for(std::vector<GameObj*>::iterator i = objs->begin(); i != objs->end(); ++i)
    {
      Igel* igel = dynamic_cast<Igel*>(*i);
      if (igel)
        {
          if (igel->get_pos().x > pos.x - 30 &&
              igel->get_pos().x < pos.x + 30 &&
              igel->get_pos().y > pos.y - 20 &&
              igel->get_pos().y < pos.y + 20)
          igel->die();
        }
    }
}

/* EOF */
