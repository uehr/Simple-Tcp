#include <winsock2.h>
#include "exceptions.h"
class tcp_base{
protected:
  WSADATA data;
  SOCKET connect_socket;
  fd_set fds, readfds;
  struct sockaddr_in dest;
  int result;
  char send_end_symbol,buff;
public:
  tcp_base();
  void send(char cdata[]);
  void recv(char in_recv[],int array_size);
  void psend(char* pdata);
  void precv(char* in_recv,int var_size);
  void connect_close();
  void system_close();
  ~tcp_base();
};

class tcp_client : public tcp_base{
public:
  tcp_client(const char server_ip[], int port);
  ~tcp_client();
  void connect();
};

class tcp_server : public tcp_base{
private:
  struct sockaddr_in saver_data;
  SOCKET server_socket;
public:
  tcp_server(int port);
  ~tcp_server();
  void listen();
  void server_close();
};
