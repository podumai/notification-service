#include <boost/exception/all.hpp>
#include <exception>

namespace boost {

BOOST_NORETURN auto throw_exception(const std::exception& /* error */) -> void { std::terminate(); }

BOOST_NORETURN auto throw_exception(const std::exception& /* error */, const source_location& /* location */) -> void {
  std::terminate();
}

}  // namespace boost
