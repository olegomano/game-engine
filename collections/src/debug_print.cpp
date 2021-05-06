#include <collections/debug_print.h>
#include <collections/sync_que.h>

debug::print::IOBuffer* debug_buffer = new debug::print::IOBuffer();
debug::print::IOBuffer* error_buffer = new debug::print::IOBuffer();


debug::print::IOBuffer* debug::print::io::Debug_Buffer(){
  return debug_buffer;
}

debug::print::IOBuffer* debug::print::io::Error_Buffer(){
  return error_buffer;
}


