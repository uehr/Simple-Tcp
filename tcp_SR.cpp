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
  struct sockaddr_in dest;
  int result;
  bool isClose = false;

  void close_check(){
    if(isClose)
      throw isClosed_exception();
  }

  tcp_base(){
    memset(&dest, 0, sizeof(dest));

    result = WSAStartup(MAKEWORD(2, 0), &data);
    if(result < 0)
      throw winsock_exception(GetLastError());
  }

  virtual void send(char *pdata){close_check();}

  virtual char* recv(){close_check();}

  virtual void close(){
    result = WSACleanup();
    if(result)
    throw winsock_exception(GetLastError());
    isClose = true;
  }

};

class tcp_client : public tcp_base{
public:
  SOCKET csocket;
  tcp_client(const char server_ip[], int port) : tcp_base(){
    //接続先の情報を設定
    tcp_base::dest.sin_port = htons(port);
    tcp_base::dest.sin_family = AF_INET;
    tcp_base::dest.sin_addr.s_addr = inet_addr(server_ip);

    csocket = socket(AF_INET, SOCK_STREAM, 0);
    if (csocket < 0)
      throw winsock_exception(GetLastError());
  }

  void connect(){
    close_check();
    result = ::connect(csocket, (struct sockaddr *)&dest, sizeof(dest));
    if (result)
      throw winsock_exception(GetLastError());
  }

  void send(char *pdata){
    close_check();
    result = ::send(csocket, pdata, sizeof(data), 0);;
    if (result)
      throw winsock_exception(GetLastError());
  }

  char* recv(){
    close_check();
    char *buff;
    result = ::recv(csocket, buff, sizeof(data), 0);
    return buff;
  }

  void close(){
    result = closesocket(csocket);
    if(result)
      throw winsock_exception(GetLastError());
  }

};

class tcp_server : public tcp_base{
public:
  #define sockets_route for(map<SOCKET,struct sockaddr_in>::iterator socket_itr = route_target.begin(); socket_itr != route_target.end(); ++socket_itr)

  map<SOCKET,struct sockaddr_in> active_sockets,
                                 route_target,
                                 accepted_sockets;
  fd_set fds,readfds;

  tcp_server(int port[]) : tcp_base(){
    for(int i = 0; i < sizeof(port)/sizeof(int); i++){
      struct sockaddr_in connect_data;

      connect_data.sin_port = htons(port[i]);
      connect_data.sin_family = AF_INET;
      connect_data.sin_addr.s_addr = htonl(INADDR_ANY);

      SOCKET ssocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (ssocket < 0)
        throw winsock_exception(GetLastError());

      result = ::bind(ssocket, (struct sockaddr *)&connect_data, sizeof(connect_data));
      if(result < 0)
        throw winsock_exception(GetLastError());

      active_sockets[ssocket] = connect_data;

      FD_ZERO(&readfds);
      FD_SET(ssocket,&readfds);
    }

  }

  void listen(){
    route_target = active_sockets;
    sockets_route{
      result = ::listen(socket_itr->first, 1);
      if(result < 0)
        throw winsock_exception(GetLastError());
    }
    while(1){
      select(0, &fds, NULL, NULL, NULL);
      sockets_route{
        if (FD_ISSET(socket_itr->first, &fds))
        accepted_sockets[socket_itr->first] = socket_itr->second;
      }
    }
  }

  void send_all(char *pdata){
    route_target = connect_wait();
    sockets_route{
      result = ::send(socket_itr->first, pdata, sizeof(data), 0);
      if (result)
      throw winsock_exception(GetLastError());
    }
  }

  void send_one(char *pdata,int connect_port){
    route_target = connect_wait();
    sockets_route{
      if(ntohl(socket_itr->second.sin_port) == connect_port){
        result = ::send(socket_itr->first, pdata, sizeof(data), 0);
        if (result)
          throw winsock_exception(GetLastError());
      }
    }
  }

  char* recv(){
    char *recv_data;
    route_target = active_sockets;
    sockets_route{
      result = ::recv(socket_itr->first, recv_data, sizeof(data), 0);
      return recv_data;
    }
    thread recv_thread([](){
      while(1){
        select(0, &fds, NULL, NULL, NULL);
        sockets_route{
          if (FD_ISSET(socket_itr->first, &fds))
          result = ::recv(socket_itr->first, recv_data, sizeof(data), 0);
        }
      }
    });
  }

  void close(){
    sockets_route{
      result = closesocket(socket_itr->first);
      if(result)
        throw winsock_exception(GetLastError());
    }
  }

};
