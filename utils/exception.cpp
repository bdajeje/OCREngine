#include "exception.hpp"

namespace utils {

Exception::Exception(std::string what)
  : _what{std::move(what)}
{

}

}
