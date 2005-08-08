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

#ifndef HEADER_CONTROLLER_DEF_HXX
#define HEADER_CONTROLLER_DEF_HXX

#include <map>
#include <string>

enum AxisName
  { 
    X_AXIS, // used to run left/right
    Y_AXIS  // used to aim up/down
  };

enum ButtonName
  { 
    PRIMARY_BUTTON,   // used to ok a dialog or for running
    SECONDARY_BUTTON, // used to cancel a dialog or for jumping
    TERTIARY_BUTTON,  // used to cancel a dialog or for jumping
    PDA_BUTTON,       // display the PDA
    
    PAUSE_BUTTON,     // used to pause the game
    AIM_BUTTON,       // used to draw the gun and aim
  };

#define JUMP_BUTTON PRIMARY_BUTTON
#define RUN_BUTTON  SECONDARY_BUTTON
#define USE_BUTTON  TERTIARY_BUTTON
#define OK_BUTTON   PRIMARY_BUTTON
#define CANCEL_BUTTON SECONDARY_BUTTON

/** */
class ControllerDef
{
private:
  std::map<std::string, int> buttons;
  std::map<std::string, int> axes;

public:
  ControllerDef();
  ~ControllerDef();

  void add_button(const std::string& name, int id);
  void add_axis  (const std::string& name, int id);

  int         get_button_count()   const;
  int         get_axis_count()     const;
  int         get_keyboard_count() const;

  std::string button_id2name(int id) const;
  int         button_name2id(const std::string& name) const;

  std::string axis_id2name(int id) const;
  int         axis_name2id(const std::string& name) const;
};

#endif

/* EOF */
