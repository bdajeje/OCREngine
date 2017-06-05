#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <exception>
#include <string>

namespace utils {

class Exception : public std::exception
{
  public:

	Exception(std::string what);

	const char* what() const noexcept { return _what.c_str(); }

  private:

	const std::string _what;
};

}

#endif // EXCEPTION_HPP
