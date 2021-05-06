#pragma once
#include <cstdint>
#include <array>
#include <mutex>
#include <condition_variable>
#include <chrono>
namespace collections{
namespace sync_que{

template<typename T,uint32_t SIZE_T>
class SimpleQue{
  static constexpr uint64_t TIMEOUT = -1;
public:
  typedef T type;
  bool push_back(const T& data,uint64_t timeout = 0){
    uint64_t remaining_time = timeout;
    do{
      {
      std::lock_guard<std::mutex> lock(m_lock);
      if( (m_tail >= m_head) && (m_tail - m_head) < SIZE_T){
        m_buffer[m_tail%SIZE_T] = data;
        ++m_tail;
        m_write_signal.notify_one();
        return true;
      }
      }
      remaining_time = wait_for(remaining_time, m_read_signal,m_read_mutex);
    }while(remaining_time != TIMEOUT);
    return false;
  }


  bool pop_front(T& out, uint64_t timeout = 0){
    uint64_t remaining_time = timeout;
    do{
      {
      std::lock_guard<std::mutex> lock(m_lock);
      if(m_head < m_tail){
        out = m_buffer[m_head%SIZE_T];
        ++m_head;
        m_read_signal.notify_one();
        return true;
      }
      }
      remaining_time = wait_for(remaining_time, m_write_signal,m_write_mutex);
    }while(remaining_time != TIMEOUT);
    return false; 
  }

  constexpr uint32_t capacity() const  {
    return SIZE_T;
  }  
private:
  uint64_t wait_for(uint64_t timeout, std::condition_variable& signal, std::mutex& lock){
    if(timeout == 0){
      return TIMEOUT;
    }
    auto now = std::chrono::high_resolution_clock::now();
    
    std::unique_lock<std::mutex> guard(lock);
    std::cv_status status = signal.wait_for(guard,std::chrono::milliseconds(timeout));
  
    if(status == std::cv_status::timeout){
      return TIMEOUT;
    }
    uint64_t delta = std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::high_resolution_clock::now() - now).count();
    if(delta > timeout){
      return 0;
    }
    return timeout;
  }

  std::array<T,SIZE_T> m_buffer;
  uint32_t m_head = 0;
  uint32_t m_tail = 0;
  
  std::condition_variable m_read_signal;
  std::mutex m_read_mutex;
  std::condition_variable m_write_signal; 
  std::mutex m_write_mutex;
  std::mutex m_lock;

};


}
}
