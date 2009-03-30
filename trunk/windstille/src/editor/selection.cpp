/*
**  Windstille - A Sci-Fi Action-Adventure Game
**  Copyright (C) 2009 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include "editor_window.hpp"
#include "selection.hpp"

Selection::Selection()
  : moving(false)
{
}

Selection::~Selection()
{
}

void
Selection::add(const ObjectModelHandle& object)
{
  objects.push_back(object);
  signal_changed();
}

void
Selection::remove(const ObjectModelHandle& object)
{
  Objects::iterator it = std::find(objects.begin(), objects.end(), object);
  if (it != objects.end())
    {
      objects.erase(it);
      signal_changed();
    }
  else
    {
      EditorWindow::current()->print("Selection:remove(): object not in selection");
    }
}

bool
Selection::empty() const
{
  return objects.empty();
}

void
Selection::clear()
{ 
  objects.clear(); 
  signal_changed();
}

bool
Selection::has_object(ObjectModelHandle object) const
{
  Objects::const_iterator it = std::find(objects.begin(), objects.end(), object);
  return it != objects.end();
}

bool
Selection::contains_parent(ObjectModelHandle object)
{
  ObjectModelHandle parent = object->get_parent();
  while (parent)
    {
      if (has_object(parent))
        return true;

      parent = parent->get_parent();
    }

  return false;
}

void
Selection::on_move_start()
{
  moving = true;

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (contains_parent(*i))
        {
          non_moveable_objects.insert(*i);
        }
    }

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (non_moveable_objects.find(*i) == non_moveable_objects.end())
        (*i)->on_move_start();
    }

  signal_changed();
}

void
Selection::on_move_update(const Vector2f& offset)
{
  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (non_moveable_objects.find(*i) == non_moveable_objects.end())
        (*i)->on_move_update(offset);
    }
}

void
Selection::on_move_end(const Vector2f& offset)
{
  moving = false;

  for(Objects::iterator i = objects.begin(); i != objects.end(); ++i)
    {
      if (non_moveable_objects.find(*i) == non_moveable_objects.end())
        (*i)->on_move_end(offset);
    }

  non_moveable_objects.clear();

  signal_changed();
}

SelectionHandle
Selection::clone() const
{
  SelectionHandle selection = Selection::create();
  std::map<ObjectModelHandle, ObjectModelHandle> parent_map;
  for(Objects::const_iterator i = objects.begin(); i != objects.end(); ++i)
    {
      ObjectModelHandle obj = (*i)->clone();
      parent_map[*i] = obj;
      selection->add(obj);
    }

  // Second pass to set the parents to the cloned objects
  for(Selection::iterator i = selection->begin(); i != selection->end(); ++i)
    {
      if ((*i)->get_parent())
        {
          std::map<ObjectModelHandle, ObjectModelHandle>::iterator it = parent_map.find((*i)->get_parent());
          
          if (it == parent_map.end())
            {
              // When the parent wasn't part of the selection, leave
              // the parent untouched
            }
          else
            {
              (*i)->set_parent(it->second);
            }
        }
    }

  return selection;
}

Rectf
Selection::get_bounding_box() const
{
  if (empty())
    {
      return Rectf();
    }
  else
    {
      Rectf rect = objects.front()->get_bounding_box();
      for(Objects::const_iterator i = objects.begin()+1; i != objects.end(); ++i)
        {
          rect = rect.grow((*i)->get_bounding_box());
        }
      return rect;
    }
}

void
Selection::add_control_points(std::vector<ControlPointHandle>& control_points)
{
  std::cout << "Adding ControlPoints" << std::endl;
  if (!empty())
    {
      const Rectf& rect = get_bounding_box();
      control_points.push_back(ControlPoint::create(Vector2f(rect.left, rect.top)));
      control_points.push_back(ControlPoint::create(Vector2f(rect.right, rect.top)));
      control_points.push_back(ControlPoint::create(Vector2f(rect.left, rect.bottom)));
      control_points.push_back(ControlPoint::create(Vector2f(rect.right, rect.bottom)));
    }
}

/* EOF */
