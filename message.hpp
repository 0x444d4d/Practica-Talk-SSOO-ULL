#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>

struct message_t {
  char user[126];
  char text[1024];
};


/*
class message_t {
  private:
  char text[1024];

  public:
  void load_text( std::string input );
  std::ostream& print( std::ostream& os );

};


void message_t::load_text ( std::string input ) {
  strncpy( text, input.c_str(), input.size() );
}

std::ostream& message_t::print ( std::ostream& os ) {
  return os << text;
}
*/
