//  $Id: bonus_flyer.hxx,v 1.3 2003/09/10 08:25:29 grumbel Exp $
// 
//  Windstille - A Jump'n Shoot Game
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef BONUSFLYER_HXX
#define BONUSFLYER_HXX

#include <ClanLib/Display/sprite.h>
#include <ClanLib/Core/Math/vector2.h>
#include "gameobj.hxx"

class BonusFlyer : public GameObj
{
private:
  CL_Sprite sprite;
  CL_Vector2 pos;
  CL_Vector2 vel;
  float passed_time;

public:
  BonusFlyer (const CL_Vector2&);
  virtual ~BonusFlyer () {}
  void draw ();
  void update (float delta);
};

#endif

/* EOF */
