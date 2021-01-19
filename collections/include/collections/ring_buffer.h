#pragma once
#include <array>
namespace collections{
namespace ring_buffer{

template<typename T, size_t SIZE>
struct ArrayWrapper{
  static constexpr size_t size = SIZE;
  typedef std::array<T,SIZE> array;
  typedef T type;
};

template<typename ARRAY>
class RingBuffer {
public:  
  typedef ARRAY ArrayInfo;
  static constexpr size_t SIZE = ARRAY::size;
  
  class const_iterator{
  public:
    typedef const_iterator self_type;
    typedef typename ARRAY::type  value_type;
    typedef typename ARRAY::type& reference;
    typedef typename ARRAY::type* pointer;
    typedef int difference_type;
    typedef std::forward_iterator_tag iterator_category;
                
    const_iterator(const RingBuffer<ARRAY>& owner, uint32_t pos) : m_owner(owner), m_index(pos){ }

    self_type operator++() { 
      self_type i = *this; 
      m_index++; 
      return i; 
    }

    self_type operator++(int junk) { 
      m_index++; 
      return *this; 
    }
    
    const reference operator*() const { return (const reference)(m_owner.at(m_index % SIZE)); }
    const pointer operator->()  const { return  m_owner.at(m_index % SIZE); }
    bool operator==(const self_type& rhs) { return m_index == rhs.m_index;}
    bool operator!=(const self_type& rhs) { return m_index != rhs.m_index; }
  private:
    uint32_t m_index;
    const RingBuffer<ARRAY>& m_owner;

  };
  
  const_iterator begin() const {
    return const_iterator(*this, m_pos); 
  }

  const_iterator end() const {
    return const_iterator(*this, m_pos + SIZE);
  }

  void push_back(const typename ARRAY::type& g){
    m_buffer[pos()] = g;
    m_pos++;
  }

  size_t pos() const {
    return m_pos % SIZE;
  }

  size_t size() const {
    return SIZE;
  }

  const typename ARRAY::type& at(size_t index) const {
    return m_buffer[index];
  }

  const typename ARRAY::type& operator[](size_t index) const {
    return m_buffer[ (pos() + index) % SIZE ];
  }

  const typename ARRAY::type* buffer() const {
    return &m_buffer[0];
  }

private:
  typename ARRAY::array m_buffer;
  size_t m_pos;
};

}
}
