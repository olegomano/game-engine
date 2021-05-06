#pragma once
#include <optional>
#include <vector>
#include <glm/glm.hpp>
#include <cgeom/transform.h>
#include <type_traits>
#include <functional>
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
  Node(node_ref ref= {0xffffffff}) : 
    m_ref(ref),
    m_parent(nullptr),
    m_payload(std::nullopt) {}
  
  Node(node_ref self, Node<T>* parent, T p) :
    m_ref(self),
    m_parent(parent),
    m_payload(p){}  
  Node(node_ref self, Node<T>* parent) :
    m_ref(self),
    m_parent(parent), 
    m_payload(std::nullopt){}
  
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

  node_ref ref() const {
    return m_ref;
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
  node_ref                              m_ref;
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
  typedef Node<T> node_type;
  typedef T data_type;
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
    Node<T> root = Node<T>(Root_Ref);
    m_nodes.push_back(std::move(root));
  }
  
  Scene(Scene<T>& other){
    Node<T> root = Node<T>(Root_Ref);
    m_nodes.push_back(std::move(root));
  
    other.traverse<node_ref>([&](Node<T>& node, const node_ref& ref) -> node_ref{
        if(node.hasPayload()){
          return createNode(node.data(),ref);
        }else{
          return createNode(ref);  
        }
    },Root_Ref);
   }

  void update(){
    m_globalPositions.clear();
    traverse<glm::mat4>([&](Node<T>& node, const glm::mat4& parent) -> glm::mat4 {
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
    other.traverse<node_ref>([&](Node<T>& node,const node_ref& parent) -> node_ref{
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
    Node<T>& parentNode  = get(parent);
    Node<T>  child       = Node<T>( {m_nodes.size()}, &parentNode);
    m_nodes.push_back(std::move(child));
    parentNode.addChild(m_nodes.at(child.ref().index));
 
    return child.ref();
  }
  
  template<typename _T>
  void morph_into(Scene<_T>& dst, std::function<_T(const T&)> transform){
    traverse<node_ref>([&](Node<T>& node, const node_ref& parent  ){
      if(node.hasPayload()){
        return dst.createNode( transform( node.data() ) , parent );
      }
      return dst.createNode(parent);
    },dst.Root_Ref);
  }

  template<typename _R>
  void traverse(std::function<_R(Node<T>&,const _R&)>const& function,const _R& args, node_ref start = Root_Ref) {
    struct stack_value{
      Node<T>* node = nullptr;
      _R arg;
      
      stack_value(){}
      stack_value(Node<T>* n, const _R& v):
        node(n),
        arg(v){}

      stack_value(const stack_value& sv):
        node(sv.node),
        arg(sv.arg){}
    };
    
    std::vector<stack_value> stack;
    stack.reserve(m_nodes.size());

    stack_value value(&get(start) ,args );
    stack.emplace_back( value );

    while(!stack.empty()){
      

      Node<T>* e = stack.back().node;
      const auto& a = stack.back().arg;
      stack.pop_back();

      auto pres = function(*e,a);
      //Node<T>*
      for(auto& child : e->children()){
        stack_value value(child,pres);
        stack.emplace_back(value);
      }
    }
  }
 
 private:
  collections::stable_vector::StableVector<Node<T>> m_nodes;
  std::vector<GlobalPosition> m_globalPositions;
};

}
}
