#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include <optional>

namespace collections{

template<typename SuperClass, typename T>
class Observable{
public:
  typedef std::function<void(const T& item, bool added)> UpdateListener;
  
  void push_back(const T& data){
    SuperClass* owner = (SuperClass*) this;
    m_items.push_back(data);
    
    owner->onItemAdded(data);
    for(auto& handler : m_listener){
      handler(data,true);
    }
  }

  void addUpdateHandler(const UpdateListener& listener){
    m_listener.push_back(listener);
  }

  const auto& _items() const {return m_items;}
private:
  std::vector<UpdateListener> m_listener;
  std::vector<T> m_items;
};


template<typename SuperClass, typename T>
class Handler{
public:
  void handle(const T& item, bool added){
    m_items.emplace(item->uuid(),item);
  }

  std::optional<T> get(uint32_t uuid){
    if(m_items.count(uuid)){
      return m_items[uuid];
    }
    return std::nullopt;
  }
private:
  std::unordered_map<uint32_t,T> m_items;
};


template<typename T, typename SuperClass_O, typename SuperClass_H>
struct connector{
  typedef Observable<SuperClass_O, T> ObservableType;
  typedef Handler<SuperClass_H, T> HandlerType;

  static 
  void connect(ObservableType& source, HandlerType& dst){
    source.addUpdateHandler( 
        std::bind(&HandlerType::handle, dst, 
          std::placeholders::_1,std::placeholders::_2) );
  }
};

#define TYPE(obj) typename std::decay<decltype(obj)>::type

#define CONNECT(source,dst,type) collections::connector<type,TYPE(source),TYPE(dst)>::connect(source,dst);



}
