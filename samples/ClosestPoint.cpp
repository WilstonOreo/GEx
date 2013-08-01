#include <gex/io.hpp>
#include <gex/prim.hpp>
#include <gex/misc.hpp>
#include "create/fromTxt.hpp"
#include <gex/algorithm/util.hpp>

#include <gex/algorithm/sqrDistance.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct ClosestPoint {};
      
      template<typename MODEL>
      struct ClosestPoint<prim::Segment<MODEL>>
      {
        typedef prim::Segment<MODEL> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _segment,
            const POINT& _point,
            POINT& _closestPoint)
        {
          auto& _p0 = _segment.p0();
          auto& _p1 = _segment.p1();
          auto&& _d = _p1 - _p0;
          auto&& _n = util::getNormal(_p0,_p1).normalized();
          auto _t = dot(_d,_point - _p0) / _d.sqrLength(); 
          std::cout << _t << std::endl;

          auto _bounds = _segment.bounds();
          if (_t <= 0) { _closestPoint = _p0; return; }
          if (_t >= 1) { _closestPoint = _p1; return; }
          _closestPoint = _p0 + _t * _d;
        }
      };

      namespace 
      {
        template<typename SUBPRIMITIVE,typename PRIMITIVE, typename POINT>
        void closest(const PRIMITIVE& _prim, const POINT& _point, POINT& _closestPoint)
        {
          typedef typename PRIMITIVE::model_type model_type;
          typedef typename model_type::scalar_type scalar_type;
          typedef prim::Segment<model_type> segment_type;
          
          auto _minDist = inf<scalar_type>();
          for_each<SUBPRIMITIVE>(_prim,[&](const SUBPRIMITIVE& _segment)
          {
            POINT _subPoint;
            ClosestPoint<SUBPRIMITIVE>()(_segment,_point,_subPoint);
            auto&& _dist = sqrDistance(_point,_subPoint);
            if (_dist < _minDist)
            {
              std::cout << _dist << ": " << _closestPoint << std::endl;
              _closestPoint = _subPoint;
              _minDist = _dist;
            }
          });
        }
      }

      template<typename MODEL>
      struct ClosestPoint<prim::LineString<MODEL>>
      {
        typedef prim::LineString<MODEL> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef prim::Segment<MODEL> segment_type;
          closest<segment_type>(_prim,_point,_closestPoint);
        }
      };

      template<typename MODEL>
      struct ClosestPoint<prim::Ring<MODEL>>
      {
        typedef prim::Ring<MODEL> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef prim::Segment<MODEL> segment_type;
          closest<segment_type>(_prim,_point,_closestPoint);
        }
      };

      template<typename MODEL>
      struct ClosestPoint<prim::Polygon<MODEL>>
      {
        typedef prim::Polygon<MODEL> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef prim::Ring<MODEL> ring_type;
          closest<ring_type>(_prim,_point,_closestPoint);
        }
      };

      template<typename MODEL>
      struct ClosestPoint<prim::MultiPolygon<MODEL>>
      {
        typedef prim::MultiPolygon<MODEL> primitive_type;

        template<typename POINT>
        void operator()(
            const primitive_type& _prim,
            const POINT& _point,
            POINT& _closestPoint)
        {
          typedef prim::Polygon<MODEL> polygon_type;
          closest<polygon_type>(_prim,_point,_closestPoint);
        }
      };
    }

    using functor::ClosestPoint;

    template<typename PRIMITIVE, typename POINT> 
    void closestPoint(
        const PRIMITIVE& _prim, 
        const POINT& _point,
        POINT& _closestPoint)
    {
      ClosestPoint<PRIMITIVE>()(_prim,_point,_closestPoint);
    }

    template<typename PRIMITIVE, typename POINT> 
    POINT closestPoint(
        const PRIMITIVE& _prim, 
        const POINT& _point)
    {
      POINT _closestPoint;
      closestPoint(_prim,_point,_closestPoint);
      return _closestPoint;
    }
  }
}

#include <iostream>
#include <iomanip>

int main(int argc, char* argv[])
{
  gex::io::SVG _svg;
  auto&& _rings = create::fromTxt("testcases.txt");

  if (_rings.empty()) return EXIT_FAILURE;

  gex::Bounds2 _bounds = _rings.front().bounds();

  for (auto& _ring : _rings)
  {
    _bounds.extend(_ring.bounds());
    std::cout << _ring.bounds() << " " << _bounds << std::endl;
  }

  _bounds.max().y() = _bounds.min().y() + _bounds.size().x();
  _svg.buffer().fit(_bounds);

  size_t _index = 0;
  for (auto& _ring : _rings)
  {
    _svg.clear();
    _svg.draw(_ring,"stroke:red;fill:none");

    gex::Point2 _point(4,2), _closestPoint;

    gex::algorithm::for_each<gex::Segment>(_ring,[&](const gex::Segment& _seg)
    {
      gex::algorithm::closestPoint(_seg,_point,_closestPoint);
      _svg.draw(_closestPoint,"stroke:blue;fill:none");
    });
    
    gex::algorithm::closestPoint(_ring,_point,_closestPoint);

    _svg.draw(_point,"stroke:green;fill:none");
    _svg.draw(_closestPoint,"stroke:yellow;fill:none");

    std::stringstream ss; ss << "ClosestPoint";
    ss << std::setw(4) << std::setfill('0') << _index << ".svg";
    _svg.buffer().write(ss.str());
    ++_index;
  }

  return EXIT_SUCCESS;
}
