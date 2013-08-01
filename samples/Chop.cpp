#include <gex/prim.hpp>
#include "create.hpp"
#include <gex/algorithm/offset.hpp>
#include <gex/algorithm/within.hpp>
#include <gex/algorithm/for_each.hpp>
#include <gex/io.hpp>

using namespace gex;

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename MODEL, bool IS_CONST = true>
      struct Step
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(MODEL)

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
            segment_ref_type _segment, 
            const MARKERS& _markers, 
            SEGMENT_FUNCTOR sf,
            POINT_FUNCTOR pf)
        {
          pf(_segment.p0());
          if (!_markers.empty()) 
          {
            auto&& _v = _segment.p1() - _segment.p0();
            for (auto& _marker : _markers)
            {
              if (_marker < 0 || _marker >= 1) continue;
              auto&& _p = _segment.p0() + _v * _marker;
              sf(_p);
            }
          }
        }

        template<
        typename BEGIN,
                 typename END,
                 typename LENGTH,
                 typename MARKERS,
                 typename SEGMENT_FUNCTOR,
                 typename POINT_FUNCTOR>
        void operator()(
          BEGIN _begin, END _end,
          const LENGTH& _length,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          typedef prim::Segment<MODEL> segment_type;
          auto&& _invLength = 1.0 / _length;
          MARKERS _segmentMarkers;
          Scalar _d = 0;

          typedef std::pair<Point2,Point2> pointpair_type;
          ForEach<pointpair_type,true>()(_begin,_end,[&](const Point2& _p0, const Point2& _p1)
          {
            auto&& _segmentLength = (_p1 - _p0).length();
            auto&& _invSegmentLength = 1.0 / _segmentLength;
            auto&& _length = _segmentLength * _invLength;
            _segmentMarkers.clear();
            auto _lower = std::lower_bound(_markers.begin(),_markers.end(),_d),
                 _upper = std::lower_bound(_markers.begin(),_markers.end(),_d+_length);
            for (auto it = _lower ; it != _upper; ++it)
            {
              auto _marker = (*it - _d) / _length; 
              _segmentMarkers.push_back(_marker);
            }
            this->operator()(segment_type(_p0,_p1),_segmentMarkers,sf,pf);
            _d += _length;
          });
        }

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          linestring_ref_type _ls,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          this->operator()(_ls.begin(),_ls.end(),perimeter(_ls),_markers,sf,pf);
        }


        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          multilinestring_ref_type _mls,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          for_each<linestring_type>(_mls,[&](linestring_ref_type _ls)
          {
            this->operator()(_ls,_markers,sf,pf);
          });
        }

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          ring_ref_type _ring,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          this->operator()(_ring.begin(),_ring.end(),_ring.length(),_markers,sf,pf);
        }

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          polygon_ref_type _polygon,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          for_each<ring_type>(_polygon,[&](ring_ref_type _ring)
          {
            this->operator()(_ring,_markers,sf,pf);
          });
        }

        template<typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
        void operator()(
          multipolygon_ref_type _multiPolygon,
          const MARKERS& _markers,
          SEGMENT_FUNCTOR sf,
          POINT_FUNCTOR pf)
        {
          for_each<polygon_type>(_multiPolygon,[&](polygon_type _polygon)
          {
            this->operator()(_polygon,_markers,sf,pf);
          });
        }

      };
    }

    using functor::Step;

    template<typename PRIMITIVE, typename MARKERS, typename SEGMENT_FUNCTOR, typename POINT_FUNCTOR>
    void step(const PRIMITIVE& _prim, const MARKERS& _markers, SEGMENT_FUNCTOR sf, POINT_FUNCTOR pf)
    {
      typedef typename PRIMITIVE::model_type model_type;
      Step<model_type>()(_prim,_markers,sf,pf);
    }
    
    template<typename PRIMITIVE, typename MARKERS, typename SEGMENT_FUNCTOR>
    void step(const PRIMITIVE& _prim, const MARKERS& _markers, SEGMENT_FUNCTOR sf)
    {
      typedef typename PRIMITIVE::model_type model_type;
      Step<model_type>()(_prim,_markers,sf,[&](const typename PRIMITIVE::point_type& _p){});
    }
  }
}

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Chop {};

      template<typename MODEL>
      struct Chop<prim::Segment<MODEL>>
      {
        typedef prim::Segment<MODEL> segment_type;
        typedef segment_type output_type;
        typedef typename output_type::point_type point_type;

        template<typename MARKERS, typename OUTPUT>
        void operator()(
            const segment_type& _segment, 
            const MARKERS& _markers,
            OUTPUT& _out)
        {
          bool _mode = false;
          output_type _seg(_segment);
          step(_segment,_markers,
          [&](const point_type& _p)
          {
            _seg[_mode] = _p;
            if ((_seg.p1() - _seg.p0()).length() > 0)
            {
              _out.push_back(_seg);
              if (!_mode)
              {
                std::swap(_out.back()[0],_out.back()[1]);
              }
            }
            _mode = !_mode;
          },
          [&](const point_type& _p)
          {
            _seg.p1() = _p;
          });
          _seg[_mode] = _segment.p1();
          _out.push_back(_seg);
          if (!_mode)
          {
            std::swap(_out.back()[0],_out.back()[1]);
          }
        }
      };

      template<typename MODEL>
      struct Chop<prim::LineString<MODEL>>
      {
        typedef prim::LineString<MODEL> linestring_type;
        typedef linestring_type output_type;
        typedef typename output_type::point_type point_type;

        template<typename MARKERS, typename OUTPUT>
        void operator()(const linestring_type& _lineString,
            const MARKERS& _markers,
            OUTPUT& _out)
        {
          bool _mode = false;
          output_type _ls;
          step(_lineString,_markers,
          [&](const point_type& _p)
          {
            _ls.push_back(_p);
            if (!_ls.empty())
            {
              _out.push_back(_ls);
            }
            _ls.clear();
            _ls.push_back(_p);
          },
          [&](const point_type& _p)
          {
            _ls.push_back(_p);
          });
          if (!_ls.empty())
            _out.push_back(_ls);
        }
      };
      
      template<typename MODEL>
      struct Chop<prim::Ring<MODEL>>
      {
        typedef prim::Ring<MODEL> ring_type;
        typedef prim::LineString<MODEL> output_type;
        typedef typename output_type::point_type point_type;

        template<typename MARKERS, typename OUTPUT>
        void operator()(const ring_type& _ring,
            const MARKERS& _markers,
            OUTPUT& _out)
        {
          output_type _r;
          step(_ring,_markers,
          [&](const point_type& _p)
          {
            _r.push_back(_p);
            if (!_r.empty())
            {
              _out.push_back(_r);
            }
            _r.clear();
            _r.push_back(_p);
          },
          [&](const point_type& _p)
          {
            _r.push_back(_p);
          });

          if (_out.empty()) return;
          _out.front().insert(_out.front().begin(),_r.begin(),_r.end());
        }
      };

    }

    using functor::Chop;

    template<typename PRIMITIVE, typename MARKERS, typename OUTPUT>
    void chop(const PRIMITIVE& _primitive, const MARKERS& _markers, OUTPUT& _output)
    {
      Chop<PRIMITIVE>()(_primitive,_markers,_output);
    }

    template<typename PRIMITIVE, typename MARKERS>
    std::vector<typename Chop<PRIMITIVE>::output_type>
      chop(const PRIMITIVE& _primitive, const MARKERS& _markers)
    {
      std::vector<typename Chop<PRIMITIVE>::output_type> _output;
      chop(_primitive,_markers,_output);
      return _output;
    }
  }
}





int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  using gex::Point2;

  auto&& _circle = create::circle(gex::Point2(0,0),10,false,42);
  auto _bounds = _circle.bounds();
  _svg.buffer().fit(_bounds);
  
  _circle = create::circle(gex::Point2(0,0),8,false,7);
  
  /*//  _circle = create::irregular(gex::Point2(),5,false,16);
    _circle = create::star(gex::Point2(),4,7,false,40);
  */

  typedef std::vector<double> Markers;

  Segment _segment(Point2(-5,-5),Point2(5,5));
  _svg.draw(_segment,"stroke:green");
  gex::algorithm::step(_segment,Markers( {0.0,0.25,0.5,0.75,0.9,1.0}),[&](const Point2& _p)
  {
    _svg.draw(_p,"stroke:red");
  });

  auto&& _segments = gex::algorithm::chop(_segment,Markers({0.25,0.5,0.75,0.9}));
  for (size_t i = 0; i < _segments.size(); ++i)
  {
    auto& _seg = _segments[i];
  
    gex::Vec2 _offset(0.5,0.0);
    if (i % 2) 
    {
      _offset = -_offset;
    }

    _seg.p0() += _offset;
    _seg.p1() += _offset;

    _svg.draw(_seg,"stroke:orange");
  }
  std::cout << _segments.size() << std::endl;

  Markers _markers({0.25,0.3333,0.5,0.75,0.9,0.95});
  _svg.draw(_circle,"stroke:green;fill:none");

  size_t _markerId = 0;

  gex::algorithm::step(_circle,_markers,[&](const Point2& _p)
  {
    _svg.buffer().text(_p,_markers[_markerId],"stroke:orange;fill:none");
    _svg.draw(_p,"stroke:orange;fill:none");
    _markerId++;
  },
  [&](const Point2& _p)
  {
    _svg.draw(_p,"stroke:blue;fill:none");
  });

  auto&& _lineStrings = gex::algorithm::chop(_circle,_markers);
  std::cout << _lineStrings.size() << std::endl;
  for (size_t i = 0; i < _lineStrings.size(); ++i)
  {
    auto& _lineString = _lineStrings[i];
    gex::Vec2 _offset(0.5,0.0);
    if (i % 2) 
    {
      _offset = -_offset;
    }
    
    for (auto& _p : _lineString) 
      _p += _offset;

    _svg.draw(_lineString,"stroke:orange;fill:none");
  }
  _svg.buffer().write("Chop.svg");

  return EXIT_SUCCESS;
}

