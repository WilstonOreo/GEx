#pragma once

#include "gex/algorithm/unify.hpp"
#include "gex/algorithm/intersection.hpp"
#include "gex/algorithm/for_each.hpp"
#include <gex/algorithm/within.hpp>

#include "detail/buildArc.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename POINT, typename RING>
        void generateJunction( const POINT& _p0,
                               const POINT& _p1,
                               const POINT& _p2,
                               typename POINT::scalar_type _offset,
                               typename POINT::scalar_type _limit,
                               RING& _ring)
        {
          typedef POINT point_type;
          typedef typename POINT::scalar_type scalar_type;
          typedef typename RING::vec_type vec_type;

          _ring.clear();
          if ((_p0 - _p1).length() < _limit)
            return;

          // Normals
          gex::base::Vec2d _n0 = getNormal(_p0,_p1).normalized(),
                           _n1 = getNormal(_p1,_p2).normalized();

          // Scaled normals;
          vec_type _sn0(_n0); _sn0 *= _offset;
          vec_type _sn1(_n1); _sn1 *= _offset;

          _ring.clear();
          _ring.push_back(_p0 + _sn1);
          double _a0 = std::atan2(_n1.y(),_n1.x()),
                 _a1 = std::atan2(_n0.y(),_n0.x());

          if (_offset > 0 && _a1 < _a0) _a1 += M_PI * 2.0;
          else if (_offset < 0 && _a1 > _a0) _a1 -= M_PI * 2.0;

          if (_offset < 0)
          {
            std::swap(_a0,_a1);
          }

          if (_a1 - _a0 < M_PI)
          {
            if (_offset < 0) std::swap(_a0,_a1);
            detail::buildArc(_p1,_a1,_a0,_offset,_limit,_ring);
          }
          else
          {
            gex::Segment _seg0(_p0 + _sn0,
                               _p1 + _sn0),
                         _seg1(_p1 + _sn1,
                               _p2 + _sn1);
            gex::Point2 _iPoint;
            if (util::lineSegmentIntersection(_seg0,_seg1,_iPoint))
            {
              _ring.push_back(_iPoint);
            }
            else
            {
              _ring.clear();
              _ring.push_back(_seg0.p0());
              _ring.push_back(_seg0.p1());
              _ring.push_back(_seg1.p0());
              _ring.push_back(_seg1.p1());
              _ring.update();
              return;
            }
          }
          _ring.push_back(_p2 + _sn1);
          _ring.push_back(_p2);
          _ring.push_back(_p1);
          _ring.push_back(_p0);
          _ring.update();
        }
      }

      template<typename PRIMITIVE>
      struct Offset
        {};


      template<typename MODEL>
      struct Offset<prim::Ring<MODEL>>
      {
        typedef prim::Ring<MODEL> ring_type;
        typedef prim::MultiPolygon<MODEL> multipolygon_type;
        typedef prim::Polygon<MODEL> polygon_type;
        typedef typename MODEL::scalar_type scalar_type;

        void operator()(const ring_type& _in, multipolygon_type& _output, 
            Scalar _offset,
            Scalar _eps = 0.0001)
        {
          typedef typename ring_type::point_type point_type;
          typedef typename ring_type::scalar_type scalar_type;
          typedef typename ring_type::vec_type vec_type;

          if (std::abs(_offset) <= _eps)
          {
            _output.emplace_back(_in);
            return;
          }


          auto _limit = _in.bounds().size().length() * _eps;
          ring_type _simplified;
          simplify(_in,_simplified,_limit);
          auto _it = _simplified.begin();
          ring_type _junction;
          _junction.reserve(48);
          polygon_type _connectionPolygon;
          _connectionPolygon.boundary().reserve(_in.size() * 4);

          multipolygon_type _unionOutput;
          _unionOutput.reserve(2);

          while (1)
          {
            if (_it == _simplified.end()-1) break;

            auto _p0 = *_it;
            point_type _p1, _p2;
            if (_it == _simplified.end()-2)
            {
              _p1 = _simplified.front();
              _p2 = _simplified[1];
            }
            else if (_it == _simplified.end()-3)
            {
              _p1 = *(_simplified.end()-2);
              _p2 = _simplified.front();
            }
            else
            {
              _p1 = *(_it+1);
              _p2 = *(_it+2);
            }
            detail::generateJunction(_p0,_p1,_p2,_offset,_limit,_junction);
            if (!_junction.empty())
            {
              correct(_junction);
              if (_connectionPolygon.boundary().empty())
              {
                _connectionPolygon.boundary() = _junction;
              }
              else
              {
                for (int _attempts = 1; _attempts < 6; ++_attempts)
                {
                  union_(_junction,_connectionPolygon,_unionOutput);
                  if (_unionOutput.size() == 1)
                  {
                    _connectionPolygon =_unionOutput[0];
                    _unionOutput.clear();
                    break;
                  }
                  else 
                  {
                    _unionOutput.clear();
                    double _move = _eps*_attempts*_attempts;
                    std::cout << _offset << " " << _move << std::endl;

                    for (auto& _p : _junction)
                      _p += (_p - _junction.front()) * (_move);
                    correct(_junction);
                  }
                }
                correct(_connectionPolygon);
                _unionOutput.clear();
              }
            }
            ++_it;
          }

          if (_offset > 0)
          {
            _output.push_back(_connectionPolygon.boundary());
            for (auto& _hole : _connectionPolygon.holes())
              if (disjoint(_hole,_simplified))
                _output.back().holes().emplace_back(_hole);
          }
          else
          {
            for (auto& _hole : _connectionPolygon.holes())
              if (within(_hole,_simplified))
                _output.emplace_back(_hole);
          }
        }
      };

      template<typename MODEL>
      struct Offset<prim::Polygon<MODEL>>
      {
        typedef prim::MultiPolygon<MODEL> multipolygon_type;

        void operator()(
          const prim::Polygon<MODEL>& _in,
          multipolygon_type& _out, Scalar _offset)
        {
          multipolygon_type _boundary, _holes;
          Offset<prim::Ring<MODEL>>()(_in.boundary(),_boundary,_offset);

          for (auto& _r : _in.holes())
          {
            multipolygon_type _newHoles;
            Offset<prim::Ring<MODEL>>()(_r,_newHoles,_offset);
            _holes.insert(_holes.end(),_newHoles.begin(),_newHoles.end());
          }
          if (_offset < 0)
            _holes = unify(_holes);
          difference(_boundary,_holes,_out);
        }
      };

      template<typename MODEL>
      struct Offset<prim::MultiPolygon<MODEL>>
      {
        typedef prim::MultiPolygon<MODEL> multipolygon_type;

        void operator()(const multipolygon_type& _in, multipolygon_type& _out, Scalar _offset)
        {
          for (auto& _p : _in)
          {
            multipolygon_type _offsetPolygons;
            Offset<Polygon>()(_p,_offsetPolygons,_offset);
            _out.insert(_out.end(),_offsetPolygons.begin(),_offsetPolygons.end());
          }
          if (_offset > 0)
          {
            _out = unify(_out);
          }
        }
      };
    }
  }
}
