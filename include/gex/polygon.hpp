#pragma once
#include <boost/polygon/polygon.hpp>
#include <gex/prim.hpp>

namespace gex
{
  /// Boost polygon wrapper
  namespace polygon
  {
    typedef boost::polygon::polygon_data<int> Ring;
    typedef boost::polygon::polygon_with_holes_data<int> Polygon;
    typedef boost::polygon::polygon_traits<Polygon>::point_type Point2;

    template<typename PRIMITIVE>
    struct Adapter {};

    template<typename PRIMITIVE>
    struct AdaptType {};

#define GEX_POLYGON_ADAPT_TYPE(in_type,out_type)\
      template<> struct AdaptType<in_type>\
      {\
        typedef out_type type;\
      };

    GEX_POLYGON_ADAPT_TYPE(gex::Point2,polygon::Point2)
    GEX_POLYGON_ADAPT_TYPE(polygon::Point2,gex::Point2)
    GEX_POLYGON_ADAPT_TYPE(gex::Ring,polygon::Ring)
    GEX_POLYGON_ADAPT_TYPE(polygon::Ring,gex::Ring)
    GEX_POLYGON_ADAPT_TYPE(gex::Polygon,polygon::Polygon)
    GEX_POLYGON_ADAPT_TYPE(polygon::Polygon,gex::Polygon)

    template<typename PRIMITIVE>
    void adapt(const PRIMITIVE& _in, const gex::Bounds2& _bounds, typename AdaptType<PRIMITIVE>::type& _out)
    { 
      Adapter<PRIMITIVE>()(_in,_bounds,_out);
    }

    template<typename PRIMITIVE>
    typename AdaptType<PRIMITIVE>::type adapt(const PRIMITIVE& _in, const gex::Bounds2& _bounds)
    {
      typename AdaptType<PRIMITIVE>::type _out;
      adapt<PRIMITIVE>(_in,_bounds,_out);
      return _out;
    }

    namespace 
    {
      static inline gex::Scalar size() { return 1 << 15; }

      static inline gex::Bounds2 bounds()
      {
        constexpr int _size = 1 << 15;
        return gex::Bounds2(gex::Point2(0,0),
                            gex::Point2(_size,_size));
      }

      template<typename POINT>
      POINT transform(const POINT& _p, const gex::Bounds2& _bounds)
      {
        POINT _tP = _p; // Transformed point
        _tP -= _bounds.min();
        auto&& _size = _bounds.size();
        _tP[0] *= size() / _size.x();
        _tP[1] *= size() / _size.y();
        return _tP;
      }

      template<typename POINT>
      POINT invTransform(const POINT& _p, const gex::Bounds2& _bounds)
      {
        POINT _tP = _p; // Transformed point
        auto&& _size = _bounds.size();
        _tP[0] *= _size.x() / size();
        _tP[1] *= _size.y() / size();
        _tP += _bounds.min();
        return _tP;
      }

      template<typename RING, typename BOUNDS>
      std::vector<polygon::Point2> points(const RING& _ring, const BOUNDS& _bounds)
      {
        std::vector<polygon::Point2> _points;
        _points.reserve(_ring.size());
        for (auto& _point : _ring)
        {
          _points.push_back(adapt(_point,_bounds));
        }
        return _points;
      }
    }

#define GEX_POLYGON_ADAPTER(primitive)\
      template<>\
      struct Adapter<primitive>\
      {\
        typedef primitive in_type;\
        typedef AdaptType<primitive>::type out_type;\
        inline void operator()(const in_type&, const Bounds2& _bounds, out_type&);\
      };\
      void Adapter<primitive>::operator()(const in_type& _in, const Bounds2& _bounds, out_type& _out)

    GEX_POLYGON_ADAPTER(gex::Point2)
    {
      auto&& _tP = transform(_in,_bounds);
      _out = boost::polygon::construct<polygon::Point2>(_tP.x(),_tP.y());
    }

    GEX_POLYGON_ADAPTER(polygon::Point2)
    {
      gex::Point2 _tP(_in.x(),_in.y());
      _out = invTransform(_tP,_bounds);
    }

    GEX_POLYGON_ADAPTER(gex::Ring)
    {
      auto&& _points = points(_in,_bounds);
      boost::polygon::set_points(_out,_points.begin(),_points.end());
    }

    GEX_POLYGON_ADAPTER(polygon::Ring)
    {
      _out.clear();
      _out.reserve(_in.size());
      for (auto& _p : _in)
        _out.push_back(adapt(_p,_bounds));
    }

    GEX_POLYGON_ADAPTER(gex::Polygon)
    {
      auto&& _points = points(_in.boundary(),_in.boundary().bounds());
      _out.set(_points.begin(),_points.end());
      
      std::vector<polygon::Ring> _holes;
      for (auto& _hole : _in.holes())
      {
        _holes.push_back(adapt(_hole,_bounds));
      }
      _out.set_holes(_holes.begin(),_holes.end());
    }

    GEX_POLYGON_ADAPTER(polygon::Polygon)
    {
      _out = gex::Polygon(adapt(_in.self_,_bounds));
      for (auto it = _in.begin_holes(); it != _in.end_holes(); ++it)
      {
        _out.holes().push_back(adapt(*it,_bounds));
      }
    }
  }
}


