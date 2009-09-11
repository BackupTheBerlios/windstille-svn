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

#include "editor/navigation_graph_model.hpp"

#include "editor/navgraph_edge_object_model.hpp"
#include "editor/navgraph_node_object_model.hpp"
#include "math/line.hpp"
#include "navigation/navigation_graph.hpp"

NavigationGraphModel::NavigationGraphModel(SectorModel& sector)
  : m_sector(sector),
    m_nodes(),
    m_edges()   
{
}

NavigationGraphModel::~NavigationGraphModel()
{
}
  
boost::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::create_node(const Vector2f& pos)
{
  boost::shared_ptr<NavGraphNodeObjectModel> node(new NavGraphNodeObjectModel(pos));
  m_nodes.push_back(node);
  return node;
}

boost::shared_ptr<NavGraphEdgeObjectModel>
NavigationGraphModel::create_edge(boost::shared_ptr<NavGraphNodeObjectModel> lhs, 
                                  boost::shared_ptr<NavGraphNodeObjectModel> rhs)
{
  boost::shared_ptr<NavGraphEdgeObjectModel> edge(new NavGraphEdgeObjectModel(lhs, rhs));
  m_edges.push_back(edge);
  return edge;
}

void
NavigationGraphModel::add_node(boost::shared_ptr<NavGraphNodeObjectModel> node)
{
  m_nodes.push_back(node);
}

void
NavigationGraphModel::add_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge)
{
  m_edges.push_back(edge);
}
 
struct EdgeHasNode
{
  boost::shared_ptr<NavGraphNodeObjectModel> m_node;

  EdgeHasNode(boost::shared_ptr<NavGraphNodeObjectModel> node)
    : m_node(node)
  {}

  bool operator()(boost::shared_ptr<NavGraphEdgeObjectModel> edge)
  {
    return 
      edge->get_lhs() == m_node ||
      edge->get_rhs() == m_node;
  }
};
 
void
NavigationGraphModel::remove_node(boost::shared_ptr<NavGraphNodeObjectModel> node)
{
  m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), node), m_nodes.end());
  m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(), EdgeHasNode(node)), m_edges.end());
}

void
NavigationGraphModel::remove_edge(boost::shared_ptr<NavGraphEdgeObjectModel> edge)
{
  m_edges.erase(std::remove(m_edges.begin(), m_edges.end(), edge), m_edges.end());
}

boost::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::find_closest_node(const Vector2f& pos, float radius) const
{
  boost::shared_ptr<NavGraphNodeObjectModel> node;
  float min_distance = radius;

  for(Nodes::const_iterator i = m_nodes.begin(); i != m_nodes.end(); ++i)
  {
    float current_distance = (pos - (*i)->get_world_pos()).length();
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      node = *i;
    }
  }

  return node;
}

boost::shared_ptr<NavGraphEdgeObjectModel>
NavigationGraphModel::find_closest_edge(const Vector2f& pos, float radius) const
{
  boost::shared_ptr<NavGraphEdgeObjectModel> edge;
  float min_distance = radius;

  for(Edges::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
  {
    float current_distance = Line((*i)->get_lhs()->get_world_pos(),
                                  (*i)->get_rhs()->get_world_pos()).distance(pos);
    if (current_distance < min_distance)
    {
      min_distance = current_distance;
      edge = *i;
    }
  }

  return edge;
}

std::vector<boost::shared_ptr<NavGraphEdgeObjectModel> >
NavigationGraphModel::find_edges(boost::shared_ptr<NavGraphNodeObjectModel> node) const
{
  std::vector<boost::shared_ptr<NavGraphEdgeObjectModel> > edges;
  
  for(Edges::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
  {
    if ((*i)->get_lhs() == node || (*i)->get_rhs() == node)
    {
      edges.push_back(*i);
    }
  }

  return edges;
}

boost::shared_ptr<NavGraphNodeObjectModel>
NavigationGraphModel::get_object_at(const Vector2f& pos, const SelectMask& select_mask) const
{
  for(Nodes::const_reverse_iterator i = m_nodes.rbegin(); i != m_nodes.rend(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()) &&
        (*i)->is_at(pos))
    {
      return *i;
    }
  }

  return boost::shared_ptr<NavGraphNodeObjectModel>();
}

SelectionHandle
NavigationGraphModel::get_selection(const Rectf& rect, const SelectMask& select_mask) const
{
  SelectionHandle selection = Selection::create();

  for(Nodes::const_reverse_iterator i = m_nodes.rbegin(); i != m_nodes.rend(); ++i)
  {
    if (select_mask.match((*i)->get_select_mask()) &&
        rect.contains((*i)->get_bounding_box()))
    {
      selection->add(*i);
    }
  }

  return selection;
}

void
NavigationGraphModel::write(FileWriter& writer) const
{
  writer.start_section("nodes");
  for(Nodes::const_reverse_iterator i = m_nodes.rbegin(); i != m_nodes.rend(); ++i)
  {
    (*i)->write(writer);
  }
  writer.end_section();

  writer.start_section("edges");
  for(Edges::const_reverse_iterator i = m_edges.rbegin(); i != m_edges.rend(); ++i)
  {
    (*i)->write_real(writer);
  }
  writer.end_section();
}

/* EOF */
