#include <fstream>
#include <gex/prim.hpp>
#include <boost/geometry/io/wkt/wkt.hpp>
#include <gex/algorithm/for_each.hpp>

namespace create
{
  using gex::Ring;
  using gex::Polygon;

  template<typename FILENAME>
  std::vector<Ring> fromTxt(const FILENAME& _filename) 
  {
    std::ifstream _is(_filename);
    std::string _line;
    std::vector<Ring> _rings;
    while (std::getline(_is, _line))
    {
      Polygon _polygon;
      boost::geometry::read_wkt(_line,_polygon);  
      gex::algorithm::for_each<Ring>(_polygon,[&](const Ring& _ring)
      {
        _rings.push_back(_ring);
        _rings.back().update();
        //boost::geometry::correct(_rings.back());
      });
    }
    return _rings;
  }
}
