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

#ifndef HEADER_CONVERSATION_HXX
#define HEADER_CONVERSATION_HXX

#include <vector>
#include <string>

/** Displays a list of text strings from which the user can select
    one, used to form multiple-choice dialogs */
class Conversation
{
private:
  bool active;
  int selection;

  typedef std::vector<std::string> Choices;
  Choices choices;
  
  static Conversation* current_;
public:
  static Conversation* current() { assert(current_); return current_; }
  
  Conversation();

  void draw();
  void update(float delta);

  void add(const std::string& text);
  int  get_selection() const;
  void show();
};

#endif

/* EOF */
