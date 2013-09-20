#include <gex/base/EigenTypedefs.hpp>

using namespace gex;

int main(int ac, char**av)
{
  Vec3 _v(23,42,13);

  std::cout << _v << std::endl;
  std::cout << boost::lexical_cast<std::string>(_v) << std::endl;

  std::istringstream _iss("(1,2,3)");
  Vec3 _a;
  _iss >> _a;
  
  _iss.clear(); _iss.str("(4,5,6)");
  auto&& _b = boost::lexical_cast<gex::Vec3>(_iss.str());

  std::cout << _a << std::endl;
  std::cout << _b << std::endl;

  return EXIT_SUCCESS;
}
