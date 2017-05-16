class tcp_base{
private:
  void close_check();
public:
  tcp_base(const char server_ip[], int port);
  void connect();
  void send(char *pdata);
  void recv();
  void close();
};
