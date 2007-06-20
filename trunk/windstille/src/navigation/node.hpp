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

#ifndef HEADER_NAVIGATION_NODE_HPP
#define HEADER_NAVIGATION_NODE_HPP

#include <vector>
#include "math/vector.hpp"

#include "segment_position.hpp"

/** */
class Node
{
private:
  Vector pos;
  
public:
  /** Segments connected to this node */
  typedef std::vector<SegmentPosition> Segments;
  Segments segments;

public:
  Node(const Vector& pos_);
  ~Node();

  Vector get_pos() const { return pos; }
  void   set_pos(const Vector& p) { pos = p; }

  /** Connect the given segment to the node, the position is used to
      mark the end of the segment that is actually connected */
  void add_segment(const SegmentPosition& segment);

  void remove_segment(Segment* segment);
private:
  Node(const Node&);
  Node& operator=(const Node&);
};

#endif

/* EOF */
