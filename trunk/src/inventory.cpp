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

#include <iostream>
#include <vector>
#include "sprite2d/sprite.hpp"
#include "input/input_manager.hpp"
#include "input/controller.hpp"
#include "inventory.hpp"

class InventoryItem
{
public:
  Sprite sprite;
  
  InventoryItem(const std::string& filename)
    : sprite(filename)
  {
  }
};

class InventoryImpl
{
public:
  int   moving;
  float add_angle;
  Sprite slot;
  Sprite slothighlight;
  typedef std::vector<InventoryItem> Items;
  Items items;
  int current_item;

  void update(float delta);
  void draw();
  
  void incr_current_item() { 
    if (current_item == int(items.size()) - 1)
      current_item = 0;
    else
      current_item += 1;
  }

  void decr_current_item() { 
    if (current_item == 0)
      current_item = items.size() - 1;
    else
      current_item -= 1;
  }
};

Inventory::Inventory()
  : impl(new InventoryImpl())
{
  impl->slot = Sprite("images/inventory/slot.sprite");
  impl->slothighlight = Sprite("images/inventory/slothighlight.sprite");
  impl->moving = false;
  impl->add_angle = 0.0f;
  impl->current_item = 0;

  impl->items.push_back(InventoryItem("images/inventory/flashlight.sprite"));
  impl->items.push_back(InventoryItem("images/inventory/stone.sprite"));
  impl->items.push_back(InventoryItem("images/inventory/pda.sprite"));
  impl->items.push_back(InventoryItem("images/inventory/granate.sprite"));
  impl->items.push_back(InventoryItem("images/inventory/keycard.sprite"));
}

Inventory::~Inventory()
{
}

void
InventoryImpl::draw()
{
  Vector pos = Vector(400, 300); // View::current()->screen_Player::currently->get_pos();

  int num_items = items.size();
  float step_angle = (2*M_PI) / num_items;

  for(int i = 0; i < int(items.size()); ++i)
    {
      Vector draw_pos = pos + Vector(128, 0).rotate(step_angle * i - M_PI/2 + add_angle);

      if (i == 0 && moving == 0)
        slothighlight.draw(draw_pos);
      else
        slot.draw(draw_pos);

      items[(i+current_item)%items.size()].sprite.draw(draw_pos - Vector(32,32));
    }
}

void
Inventory::draw()
{
  impl->draw();
}

void
Inventory::update(float delta)
{
  impl->update(delta);
}

void
InventoryImpl::update(float delta)
{
  Controller controller = InputManager::get_controller();
 
  float step_angle = (2*M_PI) / items.size();
  if (fabsf(add_angle) > step_angle)
    {
      if (moving == 1)
        decr_current_item();
      else if (moving == -1)
        incr_current_item();

      moving = 0;
      add_angle = 0;
    }

  if (controller.get_axis_state(X_AXIS) < -0.5f)
    {
      if (moving == 1)
        {
          add_angle = -step_angle + add_angle;
          decr_current_item();
        }

      moving = -1;
    }
  else if (controller.get_axis_state(X_AXIS) > 0.5f)
    {
      if (moving == -1)
        {
          add_angle = step_angle + add_angle;
          incr_current_item();
        }

      moving =  1;
    }

  if (moving == -1)
    {
      add_angle -= 3 * delta;
    }
  else if (moving == 1)
    {
      add_angle += 3 * delta;
    }
}

/* EOF */
