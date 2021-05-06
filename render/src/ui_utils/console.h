#pragma once
#include <collections/ring_buffer.h>
#include <collections/sync_que.h>
#include <collections/sync_que_utils.h>
#include <collections/debug_print.h>
#include <functional>


namespace render{

class Console{ 
public:
  typedef std::function<void(const std::string&)> Listener;
  
  void push_log(const std::string& command){
  }
  
  void set_input_text(const std::string& command){
    m_input_text = command;
  } 

  void execute_input_text(){
    m_buffer.push_back(m_input_text);
    m_input_text = "";
  }

  const std::string& input_text() const {
    return m_input_text;
  }
  
  const auto& text() {
    collections::sync_que::drain_for(*debug::print::io::Debug_Buffer(),
        [&](const std::string& string){
          m_buffer.push_back(string);
        }, 3);
    return m_buffer;
  }

private:
  std::string m_input_text = "Default Text"; 
  collections::ring_buffer::RingBuffer<collections::ring_buffer::ArrayWrapper<std::string,32>> m_buffer;
};

}
