#include <iostream>

#include "./socket.hpp"
//#include "./functions.hpp"

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

//PENDIENTE
//Fichero de configuracion por defecto.

//PENDIENTE
//Interceptar señales.

bool operator <( const sockaddr_in& lhs, const sockaddr_in& rhs );
bool operator !=( const sockaddr_in& lhs, const sockaddr_in& rhs );

void print_help(void);
void request_cancellation( std::thread& thread ); ///Solicita la cancelacion de un hilo y espera a su terminacion.
bool connect_to_server( std::string user, socket_t socket, sockaddr_in rem_address );
sockaddr_in make_ip( int port, const std::string& ip = "empty"); ///Crea un sockaddr_in a partir del puerto especificado.
std::string get_time( void );

//Funciones de los Hilos.
void send_m( sockaddr_in& rem_address, socket_t soc_local, std::string user, std::exception_ptr s_exeption );  ///Funcion del hilo de envio para cliente.
void receive_m( sockaddr_in& rem_address, socket_t soc_local, std::exception_ptr r_exception ); ///Funcion del hilo de recepcion para el cliente.
void resend_m( socket_t soc_local, std::set<sockaddr_in> connections,  std::exception_ptr r_exception ); ///Funcion del hilo de envio/reception del servidor.
void server_input( void );


//PENDIENTE
//Cambiar argumentos por struct.
void set_options( int argc, char **argv, bool& help, bool& server, std::string& ip, int& port, std::string& user);

//Variables globales.
std::atomic<bool> finish(false);

int main(int argc, char *argv[]) {

  bool help_option = false;
  bool server_option = false;
  std::string port_option( "talk_port" );
  std::string ip_rem( "talk_ip" );
  std::string user( "talk_username" );
  int port ( -1 );


  sockaddr_in loc_address;
  sockaddr_in rem_address;
  std::exception_ptr s_exception {}; 
  //Comenzamos el programa leyendo las opciones recividas.
  set_options( argc, argv, help_option, server_option, ip_rem, port, user ) ;

  //Codigo del modo servidor.
  //
  //
  if ( server_option ) {

    std::set<sockaddr_in> connections;

    if ( port == -1 ) {
      std::cout << "Puerto no especificado, se usará el 8000" << std::endl;
      port = 8000;
    }

    loc_address = make_ip ( port );
    socket_t soc_local( loc_address );

    std::thread resend ( &resend_m, std::ref(soc_local), std::ref(connections), std::ref(s_exception) ); ///Hilo que se encarga de reenviar los mensajes.
    std::thread controll ( &server_input ); ///Hilo que controla la salida del programa asi como otras funciones.

    controll.join();

    request_cancellation( resend );

  }
  //Codigo del modo cliente
  //
  //
  else {

    if ( port == -1 ) {
      std::cout << "Puerto del servidor no especificado, se usara el 8000 ";
      port = 8000;
    }

    //Si no se ha especificado up del servidor, se asigna una por defecto.
    //Si el puerto tampoco, se asignal el 8000.
    if ( ip_rem != "talk_ip" ) rem_address = make_ip ( port, ip_rem );
    else rem_address = make_ip ( port );
    loc_address = make_ip ( 0 );
    socket_t soc_local( loc_address );


    //PENDIENTE
    //Pasar try/catch al main completo.
    try {

      //PENDIENTE
      //Connect_to_server() devuelve 1 en caso de realizar la conexion correctamente.
      //Implementar un hilo para controlar el tiempo de espera.
      if ( connect_to_server(user, soc_local, rem_address ) ) {
        std::cout << "Conectado al servidor" << std::endl;
        fflush(stdout);
      }
      else {
        std::cout << "Error al conectarse al servidor" << std::endl;
        std::cout << "Tiempo de espera agotado" << std::endl;
        fflush(stdout);
        //PENDIENTE.
        //exit( no de error );
      }

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
      load_text( user, get_time(), text,  message);
      soc_local.send_to( message, rem_address );
    }
  }
  return;
}


void receive_m( sockaddr_in& rem_address, socket_t soc_local, std::exception_ptr r_exception ) {
  //PENDIENTE
  //Cerrar el cliente si el servidor envia /talk_quit controlando el usuario.
  //Controlar que los usuarios no se repitan.

  while (!finish) {
    message_t message;
    soc_local.recieve_from( message, rem_address );
    std::cout << message;
    fflush(stdout);
  }
  return;
}


void resend_m( socket_t soc_local, std::set<sockaddr_in> connections, std::exception_ptr r_exception ) {
    sockaddr_in rem_address {};
    message_t message;
  while (!finish) {

    soc_local.recieve_from( message, rem_address);

    if ( std::string(message.text) == "/talk_join" ) {
      connections.insert( rem_address );
      std::cout << "Se ha conectado el usuario: " << message.user << std::endl;
      load_text( message.user, message.time, "/talk_connected", message );
      soc_local.send_to( message, rem_address );
    }
    else {
      for (auto dir: connections) 
        if( dir != rem_address )
          soc_local.send_to( message, dir );
    }
  }
  return;
}


void server_input( void ) {
  while( !finish ) {
    std::string input;
    std::cin >> input;
    std::cin.ignore();

    if ( input == "/help" ) print_help();
    if ( input == "/quit" ) finish = true;
  }
}


void set_options( int argc, char **argv, bool& help, bool& server, std::string& ip, int& port, std::string& user) {

  int option;
  while ( ( option = getopt(argc, argv, "hsc:u:p:01") ) != -1 ) {
    switch (option) {
      case '0':
      case '1':
        // Recuerda que "option" contiene la letra de la opción.
        std::cout << option << std::endl;
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
      case 'c':
        std::cout << "Seleccionada ip:" << optarg << std::endl;
        ip = optarg;
        fflush(stdout);
        break;
      case 'p':
        // Esta opción recibe un argumento.
        // getopt() lo guarda en la variable global "optarg"
        std::cout << "Puerto seleccionado: " << optarg << std::endl;
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
    print_help();
    finish = true;
    exit(0);
  }

  if ( server == true ) {
    user = "Server";
  } 
  else if( user == "talk_username" ) {
    const char* aux = std::getenv("USER");
    if ( aux != nullptr ) user = aux;
  }
}


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
  std::cout << "Opciones de linea de comandos." << std::endl;
  std::cout << "-h : Muestra la ayuda" << std::endl;
  std::cout << "-p : Especifica el puerto local/remoto para servidor/cliente" << std::endl;
  std::cout << "-u : Especifica el nombre de usuario ( Solo cliente ) Si no se especifica se usan las variales de entorno" << std::endl;
  std::cout << "-s : Inicia el programa en modo servidor" << std::endl;
  std::cout << "Opciones en aplicacion" << std::endl;
  std::cout << "/help : mostrar ayuda." << std::endl;
  std::cout << "/quit : cerrar el cliente servidor" << std::endl;
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
