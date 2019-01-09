#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <ctime>

struct message_t {
  char user[126];
  char time[50];
  char text[256];
};


void load_text ( std::string user, std::string time, std::string text, message_t& message ) {
//void load_text ( std::string user, std::time_t  time, std::string text, message_t& message ) {

  assert( user.size() < 126 );
  assert( time.size() < 50);
  assert( text.size() < 256);

  message.user[user.size()] = '\0';
  message.time[time.size()] = '\0';
  message.text[text.size()] = '\0';

  strncpy( message.user, user.c_str(), user.size() );
  strncpy( message.time, time.c_str(), time.size() );
  strncpy( message.text, text.c_str(), text.size() );
}


std::ostream& print ( std::ostream& os, message_t& message ) {
  
  os << message.user << ": " << message.time << std::endl;
  os << message.text;
  return os;
}


/*
std::ostream& operator <<( message_t& message, std::ostream os ) {
  print( message, os );
}
*/
