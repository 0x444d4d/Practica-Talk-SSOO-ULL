#pragma once

#include "./message.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


class socket_t {
  private:
    int fd_;
    
  public:

    socket_t( sockaddr_in &address );
    ~socket_t() {}

    int make_socket( void );

    void send_to( message_t message, sockaddr_in &address);
    void recieve_from( message_t message, sockaddr_in &address);

};


socket_t::socket_t( sockaddr_in &address ) {
  fd_ = make_socket();
  bind(fd_, reinterpret_cast<const sockaddr*>(&address), sizeof(address));
}


int socket_t::make_socket( void ) {
  int fd = socket( AF_INET, SOCK_DGRAM, 0 );
  if ( fd < 0 ) {
    std::cerr << "Error al crear el puerto" << std::endl;
    return 3;
  }

  return fd;
}


void socket_t::send_to( message_t message, sockaddr_in &address) {
  int result = sendto(fd_, &message, sizeof(message), 0, reinterpret_cast<const sockaddr*>(&address), sizeof(address));

  if ( result < 0 ) {
    std::cerr << "Fallo sendto" << std::endl;
    return;
  }
}


void socket_t::recieve_from( message_t message, sockaddr_in &address) {

  socklen_t src_len = sizeof(address); ///Cambiar esto

  int result = recvfrom(fd_, &message, sizeof(message), 0, reinterpret_cast< sockaddr*>(&address), &src_len);

  if ( result < 0 ) {
    std::cerr << "Falló rcvfrom()" << std::endl;
    return;
  }

  message.text[254] = '\0';
  std::string recieved;
  recieved.copy(message.text, sizeof(message.text) - 1, 0);
  std::cout << recieved << std::endl;


}
