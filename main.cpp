#include <iostream>

#include "./socket.hpp"

#include <atomic>
#include <string>
#include <cstring>
#include <ctime>
#include <thread>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

std::atomic<bool> finish(false);

sockaddr_in make_ip( const std::string& ip, int port);
void load_message( message_t message, std::string input );

//Funciones de los Hilos.
void send_m( sockaddr_in& rem_address, socket_t soc_local, std::string user );
void receive_m( sockaddr_in& rem_address, socket_t soc_local );


int main(int argc, char *argv[]) {

  std::string ip_loc, ip_rem;
  std::string user;
  int port_loc, port_rem;


  std::cout << "Introduzca el usuario: " << std::endl;
  std::cin >> user;
  std::cin.get();

  std::cout << "Introduzca la ip y el puerto local: ";
  std::cin >> ip_loc;
  std::cin.ignore();
  std::cin >> port_loc;
  std::cin.ignore();

  std::cout << "Introduzca la ip y el puerto del servidor: ";
  std::cin >> ip_rem;
  std::cin.ignore();
  std::cin >> port_rem;
  std::cin.ignore();

//DIRECCIONES
  sockaddr_in loc_address = make_ip (ip_loc, port_loc);
  sockaddr_in rem_address = make_ip (ip_rem, port_rem);
  socket_t soc_local( loc_address );


//HILOS
  std::thread envio ( &send_m, std::ref(rem_address), std::ref(soc_local), std::ref(user) );
  std::thread receptor ( &receive_m, std::ref(rem_address), std::ref(soc_local) );

  while (!finish);

  envio.join();
  receptor.join();

  return 0;
}


void send_m( sockaddr_in& rem_address, socket_t soc_local, std::string user ) {

  while(!finish) {
    message_t message;
    std::string time;
    std::string text;

    std::getline(std::cin, text);

    if (text == "/quit") finish = true;
    else {
      std::time_t time1 = std::time(nullptr);
      time = std::asctime(std::localtime(&time1));

      load_text( user, time, text,  message);
      soc_local.send_to( message, rem_address );
    }
  }
}


void receive_m( sockaddr_in& rem_address, socket_t soc_local ) {

  while (!finish) {
    message_t message;
    soc_local.recieve_from( message, rem_address );
    std::cout << message;
    fflush(stdout);
  }
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
