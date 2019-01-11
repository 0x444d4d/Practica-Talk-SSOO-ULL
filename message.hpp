#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <ctime>

struct message_t {
  char user[32];
  char time[50]; //Ajustar size al tamaño del formato.
  char text[1024];
};


void load_text ( std::string user, std::string time, std::string text, message_t& message ) {

  assert( user.size() < 32 );
  assert( time.size() < 50);
  assert( text.size() < 1024);

  user.copy( message.user, user.size(), 0 );
  time.copy( message.time, time.size() - 1, 0 );
  text.copy( message.text, text.size(), 0 );

  message.user[user.size()] = '\0';
  message.time[time.size() - 1] = '\0';
  message.text[text.size()] = '\0';
}


std::ostream& print ( std::ostream& os, const message_t& message ) {
  
  return os << std::right << message.user << ": " << message.time << '\n' << message.text << std::endl << std::endl;
}


std::ostream& operator <<(std::ostream& os, const message_t& message ) {

  return print(os, message );
}

