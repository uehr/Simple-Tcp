class tcp_base{
public:
  tcp_base();
  void send(char *pdata);
  char* recv();
  void connect_close();
  void system_close();
};

class tcp_client : public tcp_base{
public:
  tcp_client(const char server_ip[], int port);
  void connect();
};

class tcp_server : public tcp_base{
public:
  tcp_server(int port);
  void listen();
  void server_close();
};
