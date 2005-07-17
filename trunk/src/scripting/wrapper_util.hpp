#ifndef __WRAPPERUTIL_HPP__
#define __WRAPPERUTIL_HPP__

#include <squirrel.h>
#include <exception>
#include <sstream>
#include <string>

class SquirrelError : public std::exception
{
public:
  SquirrelError(HSQUIRRELVM v, const std::string& message) throw();
  virtual ~SquirrelError() throw();

  const char* what() const throw();
private:
  std::string message;
};

std::string squirrel2string(HSQUIRRELVM v, int i);
void print_squirrel_stack(HSQUIRRELVM v);

#endif
