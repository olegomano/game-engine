#pragma once

namespace render{


class Console{
public:
  bool poll(std::string& str);
  void push_text(const std::string command);
  bool push_command(const std::string command); 
  const auto& text() {return m_buffer;}
private:
  std::string m_command =  "";
  bool m_pendingCommand = false;
  collection::ring_buffer::RingBuffer<collections::ring_buffer::ArrayWrapper<std::string,128>> m_buffer;
};


}
