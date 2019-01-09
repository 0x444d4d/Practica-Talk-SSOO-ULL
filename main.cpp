#include <iostream>

#include "./socket.hpp"

#include <string>
#include <cstring>
#include <ctime>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

sockaddr_in make_ip( const std::string& ip, int port);
void load_message( message_t message, std::string input );

int main(int argc, char *argv[]) {

  std::string ip;
  //ip = "127.0.0.1";
  std::string user;
  std::string time;
  std::string text;
  message_t message;


  sockaddr_in loc_address= make_ip("local", 6000);
  sockaddr_in rem_address= make_ip(ip, 6001);
  socket_t soc_local( loc_address );


  std::cout << "Introduzca el usuario: " << std::endl;
  std::cin >> user;
  std::cin.get();
  std::cout << "Introduzca el mensaje: " << std::endl;
  std::cin >> std::noskipws >> text;
  std::cin.get();


  std::time_t time1 = std::time(nullptr);
  time = std::asctime(std::localtime(&time1));


  load_text( user, time, text,  message);
  soc_local.send_to( message, rem_address );
  

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
