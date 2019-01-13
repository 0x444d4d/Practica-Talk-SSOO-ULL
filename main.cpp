#include <iostream>

#include "./socket.hpp"

#include <atomic>
#include <string>
#include <cstring>
#include <ctime>
#include <csignal>
#include <thread>
#include <exception>
#include <set>
#include <pthread.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <arpa/inet.h>



sockaddr_in make_ip( int port, const std::string& ip = "empty"); ///Crea un sockaddr_in a partir del puerto especificado.
void load_message( message_t message, std::string input ); ///Carga la cadena indicada en el mensaje.
void request_cancellation( std::thread& thread ); ///Solicita la cancelacion de un hilo y espera a su terminacion.

//Funciones de los Hilos.
void send_m( sockaddr_in& rem_address, socket_t soc_local, std::string user, std::exception_ptr s_exeption );  ///Funcion del hilo de envio para cliente.
void receive_m( sockaddr_in& rem_address, socket_t soc_local, std::exception_ptr r_exception ); ///Funcion del hilo de recepcion para el cliente.
void resend_m( socket_t soc_local, std::set<sockaddr_in> connections,  std::exception_ptr r_exception ); ///Funcion del hilo de envio/reception del servidor.

//Sobrecarga para ordenar los sockaddr_in en el set de conexiones.
bool operator<( const sockaddr_in& lhs, const sockaddr_in& rhs ) {

  if ( lhs.sin_addr.s_addr == rhs.sin_addr.s_addr )
    return ( lhs.sin_port < rhs.sin_port );
  else
    return ( lhs.sin_addr.s_addr < rhs.sin_addr.s_addr );
}


//PENDIENTE
//
//
//
//Cambiar argumentos por struct.
void set_options( int argc, char **argv, bool& help, bool& server, int& port, std::string& user);

//Variables globales.
std::atomic<bool> finish(false);

int main(int argc, char *argv[]) {

  bool help_option = false;
  bool server_option = false;
  std::string port_option( "talk_port" );
  std::string ip_rem( "talk_ip" );
  std::string user( "talk_username" );
  int port_loc ( -1 ), port_rem ( -1 );


  sockaddr_in loc_address;
  sockaddr_in rem_address;
  std::exception_ptr s_exception {}; 
  //Comenzamos el programa leyendo las opciones recividas.
  set_options( argc, argv, help_option, server_option, port_loc, user ) ;

  //Codigo del modo servidor.
  //
  //
  if ( server_option ) {

    std::set<sockaddr_in> connections;

    if ( port_loc == -1 ) {
      std::cout << "Puerto no especificado, se usará el 6000" << std::endl;
      port_loc = 6000;
    }

    loc_address = make_ip ( port_loc );
    socket_t soc_local( loc_address );

    std::thread resend ( &resend_m, std::ref(soc_local), std::ref(connections), std::ref(s_exception) );

    resend.join();

    request_cancellation( resend );

  }
  //Codigo del modo cliente
  //
  //
  else {

    if ( port_rem == -1 ) {
      std::cout << "Puerto del servidor: ";
      std::cin >> port_rem;
      std::cin.ignore();
    }

  //DIRECCIONES
    loc_address = make_ip (0);
    rem_address = make_ip (/*ip_rem,*/ port_rem);
    socket_t soc_local( loc_address );


  //HILOS

    try {

      std::thread envio ( &send_m, std::ref(rem_address), std::ref(soc_local), std::ref(user), std::ref(s_exception) );
      std::thread receptor ( &receive_m, std::ref(rem_address), std::ref(soc_local), std::ref(s_exception) );

      envio.join();

      request_cancellation( receptor );

      if ( s_exception ) std::rethrow_exception( s_exception );
      
    } catch (std::bad_alloc& e) {
      std::cerr << "Mytalk: Memoria insuficiente" << std::endl;
      return 1;
    } catch ( std::system_error& e ) {
      std::cerr << "Mytalk: " << e.what() << std::endl;
      return 2;
    } catch( const std::exception& e) {
      s_exception = std::current_exception();
    }


  }
  return 0;
}




void send_m( sockaddr_in& rem_address, socket_t soc_local, std::string user, std::exception_ptr s_exception) {

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

  return;
}


void receive_m( sockaddr_in& rem_address, socket_t soc_local, std::exception_ptr r_exception ) {

  while (!finish) {
    message_t message;
    //pthread_testcancel();
    soc_local.recieve_from( message, rem_address );
    std::cout << message;
    fflush(stdout);
  }

  return;
}


void resend_m( socket_t soc_local, std::set<sockaddr_in> connections, std::exception_ptr r_exception ) {
  while (!finish) {
    sockaddr_in rem_address {};
    message_t message;
    soc_local.recieve_from( message, rem_address);
    connections.insert( rem_address );

    for (auto dir: connections)
      soc_local.send_to(message, dir );

  }
  return;
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


void request_cancellation( std::thread& thread ) {

  pthread_cancel( thread.native_handle() );
  thread.join();
}


void set_options( int argc, char **argv, bool& help, bool& server, int& port, std::string& user) {

  int option;
  while ( ( option = getopt(argc, argv, "hsu:p:01") ) != -1 ) {
    switch (option) {
      case '0':
      case '1':
        // Recuerda que "c" contiene la letra de la opción.
        std::cout << option << std::endl;
        //std::printf("opción %c\n", option);
          break;
      case 'h':
        std::cout << "Manual de ayuda\n";
        help = true;
        break;
      case 's':
        std::cout << "Modo servidor activado\n";
        server = true;
        break;
      case 'u':
        std::cout << "Usuario: " << optarg << std::endl;
        user = std::string( optarg );
        break;
      case 'p':
        // Esta opción recibe un argumento.
        // getopt() lo guarda en la variable global "optarg"
        std::printf( "Puerto seleccionado '%s'\n", optarg );
        port = atoi( optarg );
        break;
      case '?':
        // c == '?' cuando la opción no está en la lista
        // No hacemos nada porque getopt() se encarga de
        // mostrar el mensaje de error.
        break;
      default:
        // Si "c" vale cualquier otra cosa, algo fue mal con
        // getopt(). Esto nunca debería pasar.

        // Usamos fprintf() porque los errores siempre deben
        // imprimirse por la salida de error: stderr o cerr.
        std::fprintf(stderr, "?? getopt devolvió código de error 0%o ??\n", option);
    }

  }
  if (optind < argc) {
    std::cout << "-- argumentos no opción --\n";
    for (; optind < argc; ++optind)
      std::cout << "argv[" << optind << "]: " << argv[optind] << '\n';
    }

  if ( help == true ) {
    //PENDIENTE
    //Aqui escribir la informacion del funcionamiento
    finish = true;
    std::cout << "\t Ayuda de Talk" << std::endl;

    std::cin.get();
    exit(0);
  }
  if ( server == true ) {
    user = "Server";
  } 
  else if( user == "talk_username" ) {
    std::cout << "Introduzca el usuario: " << std::endl;
    std::cin >> user;
    std::cin.get();
  }
}
