#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <map>
#include "exceptions.h"
using namespace std;

class tcp_base{
public:
  WSADATA data;
  SOCKET connect_socket;
  struct sockaddr_in dest;
  int result;

  tcp_base(){
    memset(&dest, 0, sizeof(dest));

    result = WSAStartup(MAKEWORD(2, 0), &data);
    if(result != 0)
      throw winsock_exception(GetLastError());
  }

  void send(char *pdata){
    result = ::send(tcp_base::connect_socket, pdata, sizeof(data), 0);;
    if (result)
      throw winsock_exception(GetLastError());
  }

  char* recv(){
    char *buff;
    result = ::recv(tcp_base::connect_socket, buff, sizeof(data), 0);
    return buff;
  }

  void connect_close(){
    result = closesocket(tcp_base::connect_socket);
    if(result)
      throw winsock_exception(GetLastError());
  }

  void system_close(){
    WSACleanup();
  }

};

class tcp_client : public tcp_base{
public:
  tcp_client(const char server_ip[], int port) : tcp_base(){
    //接続先の情報を設定
    tcp_base::dest.sin_port = htons(port);
    tcp_base::dest.sin_family = AF_INET;
    tcp_base::dest.sin_addr.s_addr = inet_addr(server_ip);

    tcp_base::connect_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
      throw winsock_exception(GetLastError());
  }

  void connect(){
    result = ::connect(tcp_base::connect_socket, (struct sockaddr *)&dest, sizeof(dest));
    if (result)
      throw winsock_exception(GetLastError());
  }

};

class tcp_server : public tcp_base{
public:
  struct sockaddr_in saver_data;
  SOCKET server_socket;
  tcp_server(int port) : tcp_base(){
      tcp_base::dest.sin_port = htons(port);
      tcp_base::dest.sin_family = AF_INET;
      tcp_base::dest.sin_addr.s_addr = htonl(INADDR_ANY);

      server_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (server_socket < 0)
        throw winsock_exception(GetLastError());

      result = ::bind(server_socket, (struct sockaddr *)&dest, sizeof(dest));
      if(result < 0)
        throw winsock_exception(GetLastError());

    }

  void listen(){
    result = ::listen(server_socket, 1);
    if(result < 0)
      throw winsock_exception(GetLastError());
    tcp_base::connect_socket = ::accept(server_socket, (struct sockaddr *)&dest, NULL);
    if(connect_socket == INVALID_SOCKET)
      throw winsock_exception(GetLastError());
  }

  void server_close(){
    result = closesocket(server_socket);
    if(result)
      throw winsock_exception(GetLastError());
  }

};
