#include "ncurses_impl.h"

namespace render{
namespace ncurses{

Asset::Asset(const Asset& other) 
  : m_graph(other.m_graph), m_node(other.m_node){}

Asset::Asset(typename NCursesRender::SceneGraph& graph, 
             collections::scene_graph::node_ref node) : m_graph(graph),m_node(node){}
  

Asset::~Asset(){}

}
}
