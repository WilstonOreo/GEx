#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/within.hpp>
#include <gex/io.hpp>

using namespace gex;

template<typename MODEL>
void offset(const prim::LineString<MODEL>& _lineString, prim::Polygon<MODEL>& _out)
{




}


int main(int argc, char* argv[])
{
  gex::io::SVG _svg;

  auto&& _circle = create::circle(gex::Point2(0,0),10);
  auto _bounds = _circle.bounds();
  //_circle = create::irregular(gex::Point2(),5,false,16);
  _circle = create::star(gex::Point2(),3,7,false,40);

  //_circle.pop_back();
  boost::geometry::correct(_circle);

  gex::MultiPolygon _offsetCircle;

  std::vector<double> _offsets;
  const int _num = 50;
  for (int i = 0; i < _num; ++i)
  {
    double _offset = 4*(i-_num/2)/(double)_num;
    _offset = _offset*_offset*_offset;
    std::cout << _offset << std::endl;
    _offsets.push_back(_offset);
  }

  auto _tmp = _offsets;
  std::reverse(_tmp.begin(),_tmp.end());
  _offsets.insert(_offsets.end(),_tmp.begin(),_tmp.end());


  int _index = 0;
  for (double _offset : _offsets)
  {
    gex::Ring _out;
    _offsetCircle.clear();
    _svg.clear(512,512);
    _svg.buffer().fit(_bounds);
    auto _limit = _circle.bounds().size().length() / 10000;
    /*
        for (int i = 0; i < _circle.size()-2; i++)
        {
          generateJunction(_circle[i+0],_circle[i+1],_circle[i+2],_offset,_limit,_out,_svg);

          _svg.draw(_circle,"stroke:red;fill:none");

          if (!_out.empty())
            _svg.draw(_out,"stroke:blue;fill:none");

          std::stringstream ss;
          ss << "offset_" << _offset << "_" << i << ".svg";
          _svg.buffer().write(ss.str());
        }
    */
    _svg.clear();
    _svg.draw(_circle,"stroke:red;fill:none");

    rawOffsetRing(_circle,_offset,_limit,_out,_svg);
    _svg.draw(_out,"stroke:blue;stroke-width:0.5;fill:none");

    //gex::Ring _bounds = create::circle(gex::Point2(0,0),10 + _offset);
    //gex::algorithm::intersection(_out,_bounds,_offsetCircle);



  //  _svg.draw(_out,"stroke:lime;stroke-width:4;fill:none");
    //  _offsetCircle.clear();
  //    gex::algorithm::offset(_circle,_offsetCircle,_offset);
    offsetRing(_circle,_offset,_offsetCircle,_svg);
    _svg.draw(_offsetCircle,"stroke:yellow;stroke-width:3;fill:none");
    std::stringstream ss;
    ss << "offset_" << std::setw(5) << std::setfill('0') << _index << ".svg";
    _svg.buffer().write(ss.str());
    ++_index;
  }

  return EXIT_SUCCESS;
}

