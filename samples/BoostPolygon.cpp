
#include <gex/polygon.hpp>
#include "create.hpp"


int main(int argc, char* argv[])
{
  using namespace gex;
  auto&& _circle = create::circle(Point2(0.0,0.0),8.0,42);
  gex::polygon::Ring _ring = gex::polygon::adapt(_circle,_circle.bounds());

  return EXIT_SUCCESS;
}
