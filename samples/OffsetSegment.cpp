#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/within.hpp>
#include <gex/io.hpp>
#include <boost/geometry/extensions/algorithms/buffer/buffer_inserter.hpp>



using namespace gex;


namespace gex
{
  namespace algorithm
  {
    template<typename MODEL, typename OUT>
    void offset(const prim::Segment<MODEL>& _segment, OUT& _out)
    {
      






    }
  }
}



int main(int argc, char* argv[])
{
  gex::io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  //_circle = create::irregular(gex::Point2(),5,false,16);
  _circle = create::star(gex::Point2(),3,7,false,40);

  _svg.buffer().fit(_bounds);

  
  offset();


  //_circle.pop_back();
  boost::geometry::correct(_circle);


  return EXIT_SUCCESS;
}

