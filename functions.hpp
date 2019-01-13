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

bool operator<( const sockaddr_in& lhs, const sockaddr_in& rhs ) {

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
