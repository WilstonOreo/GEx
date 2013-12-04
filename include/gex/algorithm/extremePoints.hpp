#pragma once

#include <gex/base/Curve.hpp>
#include <gex/io.hpp>
#include <gex/io/svg/RandomColor.hpp>
#include <gex/algorithm/step.hpp>
#include <gex/algorithm/reverse.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename RING, typename LINESTRINGS>
        void get_convex_linestrings(const RING& _ring, LINESTRINGS& _lineStrings, bool _flipped)
        {
          typedef typename RING::point_type point_type;
          typedef base::Curve<point_type> curve_type;
          Scalar _lastDet = 0.0;
          bool _first = true;
          prim::LineString<point_type> _ls; 
          for_each<curve_type>(_ring,[&](const curve_type& _curve)
          {
            auto&& _det = _curve.determinant();
            if (_flipped) _det = -_det;
            if (_det <= 0) 
              _ls.push_back(_curve.p1());
            
            if (_first || (_lastDet <= 0 && _det > 0))
            {
              _first = false;
              if (!_ls.empty())
              _lineStrings.push_back(_ls);
              _ls.clear();
            }
            
            _lastDet = _det;
          });
        }
      }

      template<typename PRIMITIVE>
      struct ExtremePoints {};

      template<typename POINT>
      struct ExtremePoints<prim::Ring<POINT>>
      {
        template<typename RING, typename POINTS>
        void operator()(const RING& _ring, POINTS& _points,
            gex::io::SVG& _svg)
        {
          typedef typename RING::point_type point_type;
          prim::MultiLineString<point_type> _lineStrings;

          std::vector<double> _markers({0.5});

          for (auto& _lineString : _lineStrings)
          {
            POINT _extremePoint;
            bool _found = false;
            step(_lineString,_markers,[&](const POINT& _p)
            {
              if ((sqrDistance(_p,_lineString.front()) < 0.001) ||
                  (sqrDistance(_p,_lineString.back()) < 0.001) || _found) return;
              _found = true;
              _extremePoint = _p;
              _svg.draw(_p,"stroke:orange;fill:none");
            });
            if (!_found) continue;

            _svg.draw(_extremePoint,"stroke:orange;fill:none");
            _svg.draw(_lineString,"stroke:"+io::svg::randomColor()+";fill:none");
          }
        }
      };
    }

    using functor::ExtremePoints;

    template<typename RING, typename POINTS>
    void extremePoints(const RING& _ring, const POINTS& _points,
        gex::io::SVG& _svg) 
    {
      ExtremePoints<RING>()(_ring,_points,_svg);
    }
  }
}


