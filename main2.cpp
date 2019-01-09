#include <iostream>

#include "./socket.hpp"
#include "./message.hpp"

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
  message_t message;

  sockaddr_in loc_address= make_ip("local", 6001);
  sockaddr_in rem_address= make_ip(ip, 6000);

  socket_t soc_local( loc_address );

  while( 1 ) {
    soc_local.recieve_from( message, rem_address );
    print( std::cout, message );
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
  if (ip == "local")
    address.sin_addr.s_addr = htonl(INADDR_ANY);
  else
    inet_aton( ip.c_str(), &address.sin_addr);

  return address;
}
