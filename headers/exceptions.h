#include <iostream>
#include <exception>


#ifndef EXCEPTIONS
#define EXCEPTIONS

class myexception: public std::exception
{
  virtual const char* what() const throw()
  {
    return "My exception happened";
  }
} myex;

#endif
