#pragma once

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

#include "./socket.hpp"


bool operator <( const sockaddr_in& lhs, const sockaddr_in& rhs );
bool operator !=( const sockaddr_in& lhs, const sockaddr_in& rhs );

void print_help(void);
void request_cancellation( std::thread& thread ); ///Solicita la cancelacion de un hilo y espera a su terminacion.
sockaddr_in make_ip( int port, const std::string& ip = "empty"); ///Crea un sockaddr_in a partir del puerto especificado.
std::string get_time( void );
bool connect_to_server( std::string user, socket_t socket, sockaddr_in rem_address );
