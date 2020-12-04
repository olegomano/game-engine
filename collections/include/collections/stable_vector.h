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

  StableVector(){
    NodeType* n = new NodeType();
    m_allNodes[0]=n;
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
