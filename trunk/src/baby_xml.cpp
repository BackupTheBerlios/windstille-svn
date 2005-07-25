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

#include <stdexcept>
#include "baby_xml.hpp"

BabyXML::BabyXML(const std::string& text)
{
  State state = START;

  if (!text.empty())
    {
      Node node;
      
      for(int i = 0; i < int(text.size()); ++i)
        {
          switch(state)
            {
            case START: // looking for type the next node
              if (text[i] == '<')
                {
                  if (i+1 < int(text.size()) && text[i+1] == '/')
                    {
                      node = Node(Node::END_TAG);
                      i += 1;
                    }
                  else
                    {
                      node = Node(Node::START_TAG);
                    }
                }
              else if (text[i] == '&')
                {
                  node = Node(Node::ENTITY);
                }
              else
                {
                  node = Node(Node::TEXT);
                  node.content += text[i];
                }
              state = END;
              break;

            case END: // looking for end of current node
              if ((node.type == Node::START_TAG || node.type == Node::END_TAG) && text[i] == '>')
                {
                  nodes.push_back(node);
                  state = START;
                }
              else if (node.type == Node::TEXT && (text[i] == '<' || text[i] == '&'))
                {
                  nodes.push_back(node);
                  i -= 1;
                  state = START;
                }
              else if (node.type == Node::ENTITY && text[i] == ';')
                {
                  std::string entity;
                  if (node.content == "quot")
                    entity = "\"";
                  else if (node.content == "gt")
                    entity = ">";
                  else if (node.content == "lt")
                    entity = "<";
                  else
                    throw std::runtime_error("Unknown entity: '" + node.content + "'");

                  if (!nodes.empty() && nodes.back().type == Node::TEXT)
                    {
                      node = nodes.back();
                      nodes.pop_back();
                      node.content += entity;
                    }
                  else
                    {
                      node = Node(Node::TEXT, entity);
                    }
                }
              else
                {
                  node.content += text[i];
                }
              break;
            }
        }

      if (state == END)
        nodes.push_back(node);
    }
}

BabyXML::~BabyXML()
{
  
}

#ifdef TEST_BABY_XML
#include <iostream>

int main()
{
  BabyXML xml("<strong>Hello &lt;World&gt; <b>&quot;blabla&quot;</b> Blablub</strong>");

  for(BabyXML::iterator i = xml.begin(); i != xml.end(); ++i)
    {
      if (i->type == BabyXML::Node::START_TAG)
        std::cout << "STARTTAG: ";
      else if (i->type == BabyXML::Node::END_TAG)
        std::cout << "ENDTAG: ";
      else if (i->type == BabyXML::Node::TEXT)
        std::cout << "TEXT: ";

      std::cout << "'" << i->content << "'" << std::endl;
    }
}
#endif

/* EOF */
