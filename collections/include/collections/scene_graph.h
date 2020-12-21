#pragma once
#include <optional>
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
  Node() : m_parent(nullptr),m_payload(std::nullopt) {}
  Node(Node<T>* parent, T p) : m_parent(parent),m_payload(p){}  
  Node(Node<T>* parent) : m_parent(parent), m_payload(std::nullopt){}
  
  Node<T>& operator=(const T& p){
    m_payload = p;
    return *this;
  }

  bool hasPayload(){
    return m_payload();
  }

  T* operator ->(){
    return &(*m_payload);
  }

  T& operator *(){
    return *m_payload;
  }

  const T* operator ->() const {
    return &(*m_payload);
  }

  const T& operator *() const {
    return *m_payload;
  }

  const T& data() const {
    return *m_payload;
  }

  T& data(){
    return *m_payload;
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
  glm::mat4                             m_transform = glm::mat4(1);
  std::vector<Node<T>*>                 m_children;
  Node<T>*                              m_parent;
  std::optional<T>                      m_payload;
};



template<typename T>
class Scene{
public:
  static constexpr node_ref Null_Ref = {0xffffffff};
  static constexpr node_ref Root_Ref = {0};
  typedef const T GlobalPositionType;
  typedef std::tuple<GlobalPositionType,glm::mat4> GlobalPosition;
  

  Scene(){
    Node<T> root = Node<T>(nullptr,{});
    m_nodes.push_back(std::move(root));
  }

  void update(){
    m_globalPositions.clear();
    traverseScene((m_nodes[0]),[&](Node<T>& node, const glm::mat4& parent) mutable {
      glm::mat4 globalPosition =  parent*node.transform();
      m_globalPositions.push_back(std::make_tuple(node.data(),globalPosition));  
      return globalPosition;  
    },glm::mat4(1)); 
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

  const auto& globals() const {
    return m_globalPositions;
  }
  
  const auto& nodes() const {
    return m_nodes;
  }

  void append(const Scene<T>& other){
    other.traverse([&](Node<T>& node,node_ref parent){
      return createNode(node.data(),parent); 
    },Root_Ref);
  }

  node_ref createNode(const T& data,const node_ref& parent = Root_Ref){
    node_ref ref = createNode(parent);
    Node<T>& node = get(ref);
    node = data; 
    return ref;
  }

  node_ref createNode(const node_ref& parent = Root_Ref){
    if(parent == Null_Ref){
      return Null_Ref;
    }
    Node<T>& parentNode  = m_nodes[parent.index];
    Node<T>  child       = Node<T>(&parentNode);
    m_nodes.push_back(std::move(child));

    node_ref childRef = {m_nodes.size() - 1};
    parentNode.addChild(m_nodes.at(childRef.index));
    return childRef;
  }

  template<typename F,typename Arg>
  void traverse(F&& function,const Arg& arg){
    Node<T>& root = get(Root_Ref);
    for(Node<T>* child : root.children()){
      traverseScene(*child,function,arg);
    }
  }

private:
  template<typename F,typename Arg>
  void traverseScene(Node<T>& node,F&& function, const Arg& arg){
    Arg res = function(node,arg);
    for(Node<T>* child : node.children()){
      traverseScene(*child,function,res);  
    }
  }
  
private:
  collections::stable_vector::StableVector<Node<T>> m_nodes;
  std::vector<GlobalPosition> m_globalPositions;
};

}
}
