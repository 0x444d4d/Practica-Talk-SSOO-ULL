#include <iostream>

#include "./socket.hpp"

#include <string>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

sockaddr_in make_ip( const std::string& ip, int port);
void load_message( message_t message, std::string input );

int main(int argc, char *argv[]) {

  std::string ip;
  ip = "127.0.0.1";
  std::string input;
  message_t message;

  sockaddr_in loc_address= make_ip("local", 6000);
  sockaddr_in rem_address= make_ip(ip, 6001);

  socket_t soc_local( loc_address );

  std::cin >> input;
  std::cin.get();

  load_message( message, input);

  while (1) {
    soc_local.send_to( message, rem_address );
  }
 
  
  return 0;
}


void load_message( message_t message, std::string input ) {
  strncpy( message.text, input.c_str(), input.size() );
}

sockaddr_in make_ip(const std::string& ip, int port) {
  sockaddr_in address{};

  address.sin_family = AF_INET;
  address.sin_port = htons(port);

  return address;
}
