#pragma once
#include <glm/glm.hpp>
#include "stable_vector.h"

namespace collections{
namespace scene_graph{

struct node_ref{
  uint32_t index;
  
  bool operator==(const node_ref& rhs) const{
     return index == rhs.index;
  }
  bool operator!=(const node_ref& rhs) const {
    return index != rhs.index;
  }
};

template<typename T>
class Node{
public:
  Node() : m_parent(nullptr),m_payload({}) {}
  Node(Node<T>* parent, T p) : m_parent(parent),m_payload(p){}  

  T* operator ->(){
    return m_payload;
  }

  T& data(){
    return m_payload;
  }
  
  glm::mat4& transform(){
    return m_transform;
  }

  void addChild(Node<T>* child){
    m_children.push_back(child);
  }
  
  const auto& children() const {
    return m_children;
  }

  const Node<T>* parent() const {
    return m_parent;
  }

private:
  glm::mat4                             m_transform;
  std::vector<Node<T>*>                 m_children;
  Node<T>*                              m_parent;
  T                                     m_payload;
};

template<typename T>
class GlobalPosition{
  GlobalPosition(T* ptr) : m_data(ptr){

  }

  T* operator->(){
    return m_data;
  }
  
  const glm::mat4& transform() const {
    return m_transform;
  }
private:
  glm::mat4 m_transform;
  Node<T>*  m_data = nullptr;
};


template<typename T>
class Scene{
public:
  static constexpr node_ref Null_Ref = {0xffffffff};
  static constexpr node_ref Root_Ref = {0};
  
  Scene(){
    Node<T> root = Node<T>(nullptr,{});
    m_nodes.push_back(std::move(root));
  }

  void update(){
  
  }
  

  Node<T>& operator[](const node_ref& ref){
    return get(ref);
  }

  T& nodeData(const node_ref& node){
    return get(node).data(); 
  }
  
  Node<T>& get(const node_ref& ref){
    assert(ref != Null_Ref);
    return m_nodes[ref.index];
  }



  node_ref createNode(const T& data,const node_ref& parent = Root_Ref){
    if(parent == Null_Ref){
      return Null_Ref;
    }
    Node<T>& parentNode  = m_nodes[parent.index];
    Node<T>  child       = Node<T>(&parentNode,data);
    m_nodes.push_back(std::move(child));

    node_ref childRef = {m_nodes.size() - 1};
    parentNode.addChild(m_nodes.at(childRef.index));
    return childRef;
  }

private:
  template<typename F,typename Arg>
  void traverseScene(Node<T>& node,F&& function,Arg&& arg){
    auto res = function(node,arg);
    for(const node_ref& child : node.children()){
      traverseScene(get(child),function,res);  
    }
  }
  
private:
  collections::stable_vector::StableVector<Node<T>> m_nodes;
};

}
}
