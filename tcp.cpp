#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#include <map>
#include <stdio.h>
#include <windows.h>
#include "tcp.h"
using namespace std;

tcp_base::tcp_base(){
  send_end_symbol = '^';
  memset(&dest, 0, sizeof(dest));

  result = WSAStartup(MAKEWORD(2, 0), &data);
  if(result != 0)
    throw winsock_exception(GetLastError());
  FD_ZERO(&readfds);
  FD_SET(connect_socket,&readfds);
}

void tcp_base::send(char cdata[]){
  if(!cdata){ return; }
  for(int i=0; cdata[i]; i++){
    if(cdata[i] != send_end_symbol){
      result = ::send(connect_socket, &cdata[i], sizeof(1), 0);
      if (result == SOCKET_ERROR)
      throw winsock_exception(GetLastError());
      Sleep(10);
    }
  }
  result = ::send(connect_socket, &send_end_symbol, sizeof(1), 0);
  if (result == SOCKET_ERROR)
    throw winsock_exception(GetLastError());
}

void tcp_base::recv(char in_recv[],int array_size){
  memset(in_recv,'\0',array_size);
  buff = '\0';
  memcpy(&fds,&readfds,sizeof(fd_set));
  select(0,&fds,NULL,NULL,NULL);
  for(int x = 0; buff != send_end_symbol && x <= array_size; x++){
    result = ::recv(connect_socket, &buff, sizeof(&buff), 0);
    if(result == SOCKET_ERROR)
      throw winsock_exception(GetLastError());
    if(buff != send_end_symbol)
      in_recv[x] = buff;
  }
}

void tcp_base::psend(char* pdata){
  if(!pdata){ return; }
    result = ::send(connect_socket, pdata, sizeof(pdata), 0);
  if (result == SOCKET_ERROR)
    throw winsock_exception(GetLastError());
}

void tcp_base::precv(char* in_recv,int var_size){
  memcpy(&fds,&readfds,sizeof(fd_set));
  select(0,&fds,NULL,NULL,NULL);
  result = ::recv(connect_socket, in_recv, sizeof(var_size), 0);
  if(result == SOCKET_ERROR)
    throw winsock_exception(GetLastError());
}

void tcp_base::connect_close(){
  result = closesocket(connect_socket);
  if(result)
    throw winsock_exception(GetLastError());
}

void tcp_base::system_close(){
  WSACleanup();
}

tcp_base::~tcp_base(){
  try{
    connect_close();
  }catch(winsock_exception e){/*stop exception*/}
}

tcp_client::tcp_client(const char server_ip[], int port) : tcp_base(){
  //接続先の情報を設定
  tcp_base::dest.sin_port = htons(port);
  tcp_base::dest.sin_family = AF_INET;
  tcp_base::dest.sin_addr.s_addr = inet_addr(server_ip);

  tcp_base::connect_socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if (socket < 0)
    throw winsock_exception(GetLastError());
}

tcp_client::~tcp_client(){
  try{
    system_close();
  }catch(winsock_exception e){/*stop exception*/}
}

void tcp_client::connect(){
  result = ::connect(tcp_base::connect_socket, (struct sockaddr *)&dest, sizeof(dest));
  if (result)
    throw winsock_exception(GetLastError());
}

tcp_server::tcp_server(int port) : tcp_base(){
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

tcp_server::~tcp_server(){
  try{
    server_close();
    system_close();
  }catch(winsock_exception e){/*stop exception*/}
}

void tcp_server::listen(){
  result = ::listen(server_socket, 1);
  if(result < 0)
    throw winsock_exception(GetLastError());
  tcp_base::connect_socket = ::accept(server_socket, (struct sockaddr *)&dest, NULL);
  if(connect_socket == INVALID_SOCKET)
    throw winsock_exception(GetLastError());
}

void tcp_server::server_close(){
  result = closesocket(server_socket);
  if(result)
    throw winsock_exception(GetLastError());
}
