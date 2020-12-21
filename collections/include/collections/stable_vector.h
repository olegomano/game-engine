#pragma once
#include <array>

namespace collections{
namespace stable_vector{


template<typename T, uint32_t SIZE>
struct Node{
  static constexpr uint32_t MaximumSize = SIZE;
  std::array<T,SIZE> data;
  size_t size = 0;
  
  size_t push_back(const T& t){
    if(size == SIZE){
      return -1;
    }
    data[size++] = t;
    return size;
  }

  T& operator[](size_t indx){
    return data[indx];
  }

};

template<typename T>
class StableVector{
public:
  typedef Node<T,128> NodeType;
  static constexpr size_t Capacity = 128 * 128;
  
  class const_iterator{
  public:
    typedef const_iterator self_type;
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;
                
    const_iterator(const StableVector<T>& owner, uint32_t index) : m_owner(owner), m_index(index) { }

    self_type operator++() { 
      self_type i = *this; 
      m_index++; 
      return i; 
    }

    self_type operator++(int junk) { 
      m_index++; 
      return *this; 
    }
    
    const reference operator*() const { return (const reference)(*(m_owner.at(m_index))); }
    const pointer operator->()  const { return  (m_owner.at(m_index)); }
    bool operator==(const self_type& rhs) { return m_index == rhs.m_index; }
    bool operator!=(const self_type& rhs) { return m_index != rhs.m_index; }
  private:
    uint32_t m_index;
    const StableVector<T>& m_owner;

  };


  StableVector(){
    NodeType* n = new NodeType();
    m_allNodes[0]=n;
  }

  const_iterator begin() const {
    return const_iterator(*this,0); 
  }

  const_iterator end() const {
    return const_iterator(*this,m_nodeCount);
  }

  void push_back(const T& data){
    //std::cout << m_nodeCount << " " << Capacity << std::endl;
    assert(m_nodeCount < Capacity);
    uint32_t lastNodeIndex = m_nodeCount / NodeType::MaximumSize;
    if(m_allNodes[lastNodeIndex] == nullptr){
      NodeType* node = new NodeType();
      m_allNodes[lastNodeIndex] = node;
    }
    m_allNodes[lastNodeIndex]->push_back(data);
    ++m_nodeCount;
  }

  T& operator[](uint32_t indx){
    uint32_t nodeIndex = indx / NodeType::MaximumSize;
    uint32_t nodeOffset = indx % NodeType::MaximumSize;
    return m_allNodes[nodeIndex]->data[nodeOffset];
  }

  T* at(uint32_t indx){
    uint32_t nodeIndex = indx / NodeType::MaximumSize;
    uint32_t nodeOffset = indx % NodeType::MaximumSize;
    return &m_allNodes[nodeIndex]->data[nodeOffset];
  }

  const T* at(uint32_t indx) const {
    uint32_t nodeIndex = indx / NodeType::MaximumSize;
    uint32_t nodeOffset = indx % NodeType::MaximumSize;
    return &m_allNodes[nodeIndex]->data[nodeOffset];
  }

  size_t size() const {
    return m_nodeCount;
  }

  ~StableVector(){
    for(NodeType* ptr : m_allNodes){
      if(ptr != nullptr){
        delete ptr; 
      }
    } 
  }

private:
  std::array<NodeType*,128> m_allNodes = {nullptr};
  size_t                    m_nodeCount = 0;
};


}
}
