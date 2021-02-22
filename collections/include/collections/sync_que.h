#pragma once
#include <cinttypes>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include <cassert>
#include <iostream>

namespace collections{
namespace sync_que{

template<typename T,uint16_t SIZE>
class FreeList{
  struct State{
    uint16_t head;
    uint16_t tail;
    
    State(){}
    State(uint16_t head, uint16_t tail){
      this->head = head;
      this->tail = tail;
    }

    State(uint32_t i){
      *this = *((State*)(&i));
    }

    uint32_t toInt(){
      return *((uint32_t*)this);
    }


    operator uint32_t(){
      return *((uint32_t*)this);
    }

    operator uint32_t&(){
      return *((uint32_t*)this);
    }
  };

public:
  static constexpr uint16_t NULL_INDEX = -1;
  struct Node{
    T*       data;
    uint16_t handle;
  };
  
  FreeList(){
    for(int i = 0; i < SIZE - 1; i++){
      m_links[i].store(i + 1);
    }
    m_links[m_links.size() - 1].store(NULL_INDEX); 
    State s;
    s.head = 0;
    s.tail = SIZE - 1;
    m_listState.store(s.toInt());
  }

  uint16_t pop_front(){
    State old_state;
    uint32_t old_state_int;
    State new_state;
    
    uint16_t result;
    
    do{
      old_state = State(m_listState.load());
      old_state_int = old_state.toInt();
      result = NULL_INDEX;
      if(old_state.head == NULL_INDEX){ //empty 
        return result;
      }
      if(old_state.head == old_state.tail){//last element
        new_state.head = NULL_INDEX;
        new_state.tail = NULL_INDEX;
      }else{ //not empty
        new_state.head = m_links[old_state.head].load();
        new_state.tail = old_state.tail;
        if(new_state.head == NULL_INDEX){//HAZZARD, list is incosistent state, the node is mid update, retry
          old_state.head = SIZE; //forces us to fail while condition
          old_state.tail = SIZE; 
          old_state_int = old_state.toInt();
          continue; 
        }
      }
     }while(!m_listState.compare_exchange_strong(old_state_int,new_state.toInt()));
    
    result = old_state.head;
    m_links[result].store(NULL_INDEX);
    return result;
  }

  void push_back(uint16_t handle){
    State old_state;
    State new_state;
    uint32_t old_state_int;
    m_links[handle].store(NULL_INDEX);
    
    do{
      old_state = State(m_listState.load());
      old_state_int = old_state.toInt();
      if(old_state.tail == NULL_INDEX){ // empty
        new_state.head = handle;
        new_state.tail = handle;
      }else{
        new_state.head = old_state.head;
        new_state.tail = handle;
      }
    }while(!m_listState.compare_exchange_strong(old_state_int,new_state.toInt()));
    
    if(old_state.tail != NULL_INDEX){
      m_links[old_state.tail].store(handle);
    }
  }

  T& operator[](uint16_t index){
    return m_buffer[index];
  }

private:
 

private:
  //TODO: add padding between elements to reduce false sharing
  std::atomic_uint32_t m_listState;
  std::array<T,SIZE> m_buffer;
  std::array<std::atomic_uint16_t, SIZE> m_links;
};


template<typename _T, uint32_t _SIZE>
class FixedMpMc{
  static constexpr uint64_t TIMEOUT = -1;
  enum op{
    READ,
    WRITE
  };
public:
  FixedMpMc(){
    for(int i = 0; i < _SIZE; i++){
      uint16_t handle = m_queue.pop_front();
      m_queue[handle].handle = handle;
      m_freeList.insert( &(m_queue[handle]) );
    }
  }
  
  template<typename T>
  bool push_back(const T& t, uint64_t timeout_ms = 1000){
    FreeNode* node = m_freeList.remove();
    while( node == nullptr && waitFor<READ>(timeout_ms) != TIMEOUT){
      node = m_freeList.remove();
    }
    if(node == nullptr){
      return false;
    }

    node->data = t;
    m_queue.push_back(node->handle);
    return true;
  }

  bool pop_front(_T& out, uint64_t timeout_ms = 1000){
    uint16_t handle = m_queue.pop_front();
    while( handle == m_queue.NULL_INDEX && waitFor<WRITE>(timeout_ms) ){
      handle = m_queue.pop_front();
    }
    if(handle == m_queue.NULL_INDEX){
      return false;
    }
    out = m_queue[handle].data;
    m_freeList.insert(&(m_queue[handle]));
    return true;
  } 

  constexpr uint32_t capacity() const {
    return _SIZE;
  }

private:
  
  /**
   * MODIFIES THE INPUT VARIABLE TO DECREASE BY THE AMOUNT OF TIME THE FUNCTION
   * SPENT IN ITSELF, IF IT BECOMES NEGATIVE RETURNS -1
   *
   * waits for the specified operation to happen for atmost timeout time
   * returns the amount of time waiter, or -1 if timeout
   */
  template<op _op>
  uint64_t waitFor(uint64_t& timeout){
    if(timeout <= 0){
      return -1;
    }

    auto start = std::chrono::system_clock::now();
    std::cv_status timeoutStatus;
    if constexpr (_op == op::WRITE){
      std::unique_lock<std::mutex> lock(m_writeMutex);
      timeoutStatus = m_writeCV.wait_for(lock,std::chrono::milliseconds(timeout));
    }
    
    else if constexpr (_op == op::READ){
      std::unique_lock<std::mutex> lock(m_readMutex);
      timeoutStatus = m_readCV.wait_for(lock,std::chrono::milliseconds(timeout)); 
    }
    
    uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::system_clock::now() - start).count();
    
    if(timeoutStatus == std::cv_status::timeout){
      std::cout << "Timeout" << std::endl;
      return -1;
    }
    timeout -= time;
    return time; 
  }

private:
  struct FreeNode{
    _T data;
    uint16_t handle;
    std::atomic<FreeNode*> next = {nullptr};
  
    void insert(FreeNode* node){
      FreeNode* oldNext;
      node->next.store(nullptr);
      do{
        oldNext = next.load();
        node->next.store(oldNext);
      }
      while(!next.compare_exchange_strong(oldNext,node));
    }

    FreeNode* remove(){
      FreeNode* nowNext;
      do{
        nowNext = next.load();
        if(nowNext == nullptr){
          return nullptr;
        }
      }while(!next.compare_exchange_strong(nowNext,nowNext->next.load()));
      nowNext->next.store(nullptr);
      return nowNext;
     }
  };
   
  FreeList<FreeNode,_SIZE> m_queue;
  FreeNode m_freeList;


  std::condition_variable m_writeCV;
  std::mutex m_writeMutex;

  std::condition_variable m_readCV;
  std::mutex m_readMutex;
};


}
}
