#pragma once
#include <inttypes.h>
#include <atomic>
#include <list>
#include <unordered_map>
#include <mutex>
#include <map>
#include "sync_que.h"

namespace collections{
namespace task_manager{

enum Status{
  Queued,
  Running,
  Finished,
  Invalid
};

struct TaskHandle{
  uint64_t handle;
  
  bool operator==(const TaskHandle& other) const {
    return handle == other.handle;
  }

  bool operator<(const TaskHandle& other) const {
    return handle < other.handle;
  }

  bool operator>(const TaskHandle& other) const {
    return handle > other.handle;
  }
};
 
template<typename _TASK,typename _RESULT>
class TaskManager{
public:
 
  struct Task{
    TaskHandle handle;
    Status status;
    _TASK task;
    _RESULT result;
  };

  static constexpr TaskHandle NULL_TASK = {0}; 

  TaskHandle schedule(const _TASK& t){
    std::lock_guard<std::mutex> lock(m_task_lock);
    
    Task task;
    task.handle.handle = m_handle_id++;
    task.status = Queued;
    task.task = t;

    m_task_status[task.handle] = task;
    m_pending_tasks.push_back(task.handle);
    
    std::cout << "scheduled " << t << std::endl;

    return task.handle;
  }
  
  Status pollStatus(const TaskHandle& handle){
    std::lock_guard<std::mutex> lock(m_task_lock);
    if(m_task_status.count(handle) == 0){
      return Invalid;
    }
    return m_task_status[handle].status;
  }

  void pollResult(const TaskHandle& handle, _RESULT& out){
    std::lock_guard<std::mutex> lock(m_task_lock);
    out = m_task_status[handle].result;
    m_task_status.erase(handle);
  }
  
  template<typename F>
  bool runTask(F&& function){
    Task task;
    {
      std::lock_guard<std::mutex> lock(m_task_lock);
      if(m_pending_tasks.size() == 0){
        return false;
      }
      TaskHandle handle = m_pending_tasks.front();
      m_pending_tasks.pop_front();
      task.task = m_task_status[handle].task;
      task.handle = handle;
      task.status = Running;
    }
    
    _RESULT result = function(task.task);
    
    {
      std::lock_guard<std::mutex> lock(m_task_lock);
      m_task_status[task.handle].result = result; 
      m_task_status[task.handle].status = Finished;
    }
    return true;
  }

private:
  std::mutex m_task_lock;
  std::list<TaskHandle> m_pending_tasks;
  std::map<TaskHandle,Task> m_task_status;
  std::atomic<uint64_t> m_handle_id = {1}; 
};

}
}
