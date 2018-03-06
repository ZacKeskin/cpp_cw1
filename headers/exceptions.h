#include <iostream>
#include <exception>
#include <string>

#ifndef EXCEPTIONS
#define EXCEPTIONS

class myexception: public std::runtime_error
{
  public: 
    myexception(std::string error_msg = "Runtime exception caught. Terminating program."): runtime_error(error_msg){}
    
};

#endif
