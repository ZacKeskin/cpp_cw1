#include <iostream>
#include <exception>
using namespace std;

#ifndef EXCEPTIONS
#define EXCEPTIONS

class myexception: public exception
{
  virtual const char* what() const throw()
  {
    return "My exception happened";
  }
} myex;

#endif
