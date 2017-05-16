class isClosed_exception{
public:
  isClosed_exception();
};

class winsock_exception{
public:
  winsock_exception(int code);
  int getErrorCode();
};
