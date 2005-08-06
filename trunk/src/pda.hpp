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

#ifndef HEADER_PDA_HPP
#define HEADER_PDA_HPP

class TextArea;

class Dialog_Entry
{
public:
  Dialog_Entry(const std::string& arg_character, const std::string& arg_text);
  
  std::string character;
  std::string text;
};

/** */
class PDA
{
public:
  PDA();

  void draw();
  void update(float delta);
  bool is_active() const {return active;}
  void set_active(bool arg_active) {active = arg_active;}
  void add_dialog(const std::string& text);

private:
  void show_inventory();
  void show_missions();
  void show_dialogs();
  
  TextArea* text_area;
  bool active;
  std::vector<Dialog_Entry> dialogs;
  std::string new_text;  
  std::string old_text;
  enum pda_state { PDA_INVENTORY, PDA_MISSIONS, PDA_DIALOGS } state;
};

#endif

/* EOF */
