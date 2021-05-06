#pragma once
#include <collections/pipe.h>
#include <collections/ring_buffer.h>
#include <string>
namespace render{


class ReplConsole{
public:
  enum Status{
    READY_FOR_INPUT,
    RUNNING
  };

  typedef collections::task_manager::TaskManager<std::string,std::string> TaskManager;
  typedef typename collections::task_manager::TaskHandle TaskHandle;
  
  ReplConsole(TaskManager& manager) : m_manager(manager){}

 
  /**
   * Updates the state of the console
   * returns the new status of the console
   */
  Status poll(){
    auto status = m_manager.pollStatus(m_current_task);
    switch(status){
      case collections::task_manager::Invalid:
        return READY_FOR_INPUT;
      case collections::task_manager::Finished:
        {
        std::string out;
        m_manager.pollResult(m_current_task,out);
        m_output_buffer.push_back(out);
        }
        return READY_FOR_INPUT;
      default:
        return RUNNING;
    }
 }


  const std::string& line() const {
    return m_buffer;
  }

  void setLine(const std::string& line){
    m_buffer = line;
  }

  void execute(){
    m_current_task = m_manager.schedule(m_buffer);
    m_buffer = "";
  }

  auto& out(){
    return m_output_buffer;
  }


private:
  TaskManager& m_manager;
  TaskHandle m_current_task = TaskManager::NULL_TASK;
  std::string m_buffer;

  collections::ring_buffer::RingBuffer<collections::ring_buffer::ArrayWrapper<std::string,64>> m_output_buffer;
};

}
