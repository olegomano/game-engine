#pragma once
#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <cgeom/transform.h>
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

  bool hasPayload() const {
    return m_payload.has_value();
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
  
  cgeom::transform::Transform& transform(){
    return m_transform;
  }

  const cgeom::transform::Transform& transform() const {
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
  cgeom::transform::Transform           m_transform;
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
  
  template<typename Tuple> 
  static GlobalPositionType get_data(Tuple&& tuple){
    return std::get<GlobalPositionType>(tuple);
  }
  
  template<typename Tuple> 
  static const auto& get_transform(Tuple&& tuple){
    return std::get<glm::mat4>(tuple);
  }


  Scene(){
    Node<T> root = Node<T>(nullptr);
    m_nodes.push_back(std::move(root));
  }
  
  Scene(const Scene<T>& other){
    Node<T> root = Node<T>(nullptr);
    m_nodes.push_back(std::move(root));
    
    std::vector<const Node<T>*> other_node_parent;
    std::vector<node_ref> created_parent_ref;
    
    other_node_parent.push_back(&other.m_nodes[0]);
    created_parent_ref.push_back(Root_Ref);

    while(other_node_parent.size() > 0){
      const Node<T>* const parent_other = other_node_parent.back();
      node_ref parent_self = created_parent_ref.back();

      other_node_parent.pop_back(); 
      created_parent_ref.pop_back();

      for(int i = 0; i < parent_other->children().size(); i++){
        const Node<T>* const child = parent_other->children()[i];        
        other_node_parent.push_back(child);

        node_ref created_child = createNode(child->data(),parent_self);
        created_parent_ref.push_back(created_child);
      }
    }
  }

  void update(){
    m_globalPositions.clear();
    traverseScene((m_nodes[0]),[&](Node<T>& node, const glm::mat4& parent) mutable {
      glm::mat4 globalPosition =  parent*node.transform().transform();
      if(node.hasPayload()){
        m_globalPositions.push_back(std::make_tuple(
              node.data(),
              globalPosition)
            );  
      }
      return globalPosition;  
    },glm::mat4(1.0f)); 
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

  node_ref append(Scene<T>& other){
    node_ref root = createNode();
    other.traverse([&](Node<T>& node,node_ref parent){
      return createNode(node.data(),parent); 
    },root);
    return root;
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
  void traverse(F&& function,Arg&& arg){
    Node<T>& root = get(Root_Ref);
    for(Node<T>* child : root.children()){
      traverseScene(*child,function,arg);
    }
  }

private:
  template<typename F,typename Arg>
  void traverseScene(Node<T>& node,F&& function, Arg&& arg){
    auto res = function(node,arg);
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
