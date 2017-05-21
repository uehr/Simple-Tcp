#include <iostream>
#include <exception>
using namespace std;

class winsock_exception : public exception{
public:
  int error_code;
  winsock_exception(int code):exception(){error_code = code;}
  int getErrorCode(){
    return error_code;
  }
};
