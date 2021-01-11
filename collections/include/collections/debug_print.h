#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iomanip>

namespace debug{
namespace print{


#define FILE_OUT

#ifdef FILE_OUT
static inline std::ofstream printFile("log.txt");
static inline std::ostream& stream = printFile; 
#else
static inline std::ostream& stream = std::cout; 
#endif


inline static std::string timestamp(){
  auto itt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::ostringstream ss;
  ss << std::put_time(gmtime(&itt), "%FT%TZ");
  return ss.str();
}

template<typename... T>
inline static void print_debug(T&&... args){
  std::string ts = timestamp();
  stream << "Debug:[" << ts << "]";
  ( (stream << args) , ... );
  stream << std::endl;
}

template<typename... T>
inline static void print_error(T&&... args){
  std::string ts = timestamp();
  stream << "Error:[" << ts << "]";
  ( (stream << args) , ... );
  stream << std::endl;
}



}
}

