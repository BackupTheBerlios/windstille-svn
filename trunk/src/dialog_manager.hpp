//  $Id: dialog_manager.hpp,v 1.2 2003/09/29 19:29:17 grumbel Exp $
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

#ifndef HEADER_DIALOG_MANAGER_HXX
#define HEADER_DIALOG_MANAGER_HXX

#include <vector>
#include <string>
#include <ClanLib/Display/sprite.h>
#include "globals.hpp"

class Dialog {
public:
  CL_Sprite portrait;
  std::string text;
  std::vector<std::string> answers;

  enum Alignment {
    VCENTER = 0x00,
    LEFT    = 0x01,
    RIGHT   = 0x02,
    HCENTER = 0x00,
    TOP     = 0x10,
    BOTTOM  = 0x20
  };
  int alignment;

  Dialog(int alignment, const std::string& portrait);
};

/** */
class DialogManager
{
private:
  typedef std::vector<Dialog> Dialogs;
  Dialogs dialogs;
  int current_dialog;
  int current_choice;

  static DialogManager* current_;
public:
  static DialogManager* current() { return current_; }

  DialogManager();

  void draw();
  void update(float delta);

  void add_dialog(int alignment, const std::string& portrait);
  void add_question(const std::string& text);
  void add_answer(const std::string& answer);
  int  dialog_answer() const {return current_choice;}
  int  dialog_answer();
  void remove_dialog();
  void clear();

private:
  DialogManager (const DialogManager&);
  DialogManager& operator= (const DialogManager&);
};

#endif

/* EOF */
