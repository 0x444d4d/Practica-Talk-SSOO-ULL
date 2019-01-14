#include "./functions.hpp"

bool operator <( const sockaddr_in& lhs, const sockaddr_in& rhs ) {

  if ( lhs.sin_addr.s_addr == rhs.sin_addr.s_addr )
    return ( lhs.sin_port < rhs.sin_port );
  else
    return ( lhs.sin_addr.s_addr < rhs.sin_addr.s_addr );
}

bool operator !=( const sockaddr_in& lhs, const sockaddr_in& rhs ) {
  if ( lhs.sin_addr.s_addr == rhs.sin_addr.s_addr )
    if ( lhs.sin_port == rhs.sin_port ) 
      return false;
  return true;

}

void print_help( void ) {
  std::cout << "-h : Muestra la ayuda" << std::endl;
  std::cout << "-p : Especifica el puerto local/remoto para servidor/cliente" << std::endl;
  std::cout << "-u : Especifica el nombre de usuario ( Solo cliente ) Si no se especifica se usan las variales de entorno" << std::endl;
  std::cout << "-s : Inicia el programa en modo servidor" << std::endl;
}

void request_cancellation( std::thread& thread ) {

  pthread_cancel( thread.native_handle() );
  thread.join();
}

sockaddr_in make_ip( int port, const std::string& ip) {

  sockaddr_in address{};

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (ip == "empty")
    address.sin_addr.s_addr = htonl(INADDR_ANY);
  else
    inet_aton( ip.c_str(), &address.sin_addr);

  return address;
}

std::string get_time( void ) {
  std::time_t time = std::time(nullptr);
  return std::asctime(std::localtime(&time));
}

bool connect_to_server( std::string user, socket_t socket, sockaddr_in rem_address ) {
  message_t message;

  load_text( user, get_time(), std::string("/talk_join"), message );
  socket.send_to( message, rem_address );

  socket.recieve_from(message, rem_address);
  if ( std::string(message.text) == std::string("/talk_connected") ) return true;
  return false;
}
