#pragma once
#include <iostream>
#include <fstream>

namespace debug{
namespace print{


static inline std::ofstream printFile("log.txt");
#define FILE_OUT

#ifdef FILE_OUT
static inline std::ostream& stream = printFile; 
#else
static inline std::ostream& stream = std::cout; 
#endif

template<typename... T>
inline static void print_debug(T&&... args){
  ( (stream << args) , ... );
  stream << std::endl;
}




}
}
