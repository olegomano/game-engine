#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include "sync_que.h"

namespace debug{
namespace print{



typedef collections::sync_que::FixedMpMc<std::string,1024> IOBuffer;
struct io{
  static IOBuffer* Debug_Buffer();
  static IOBuffer* Error_Buffer();
};

inline static std::string timestamp(){
  auto itt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::ostringstream ss;
  ss << std::put_time(gmtime(&itt), "%FT%TZ");
  return ss.str();
}

#define print_system(...) __print_system__(__FILE__,__func__,__LINE__,__VA_ARGS__)

template<typename... T>
inline static void __print_system__(const char* file,const char* func, int line,T&&... args){
  std::stringstream stream;
  stream << timestamp();
  stream << " [" << file << ":" << func << "." << line << "] ";
  ( (stream << args) , ... );
  stream << std::endl;
  io::Debug_Buffer()->push_back(stream.str(),0);
  std::cout << stream.str() << std::endl;
}


template<typename... T>
inline static void print_debug(T&&... args){
  std::stringstream stream;
  stream << "Debug:[" << timestamp() << "] ";
  ( (stream << args) , ... );
  stream << std::endl;

  io::Debug_Buffer()->push_back(stream.str(),0);
}



template<typename... T>
inline static void print_error(T&&... args){
  std::stringstream stream;
  stream << "Error:[" << timestamp() << "] ";
  ( (stream << args) , ... );
  stream << std::endl;

  io::Error_Buffer()->push_back(stream.str(),0);
}



}
}

