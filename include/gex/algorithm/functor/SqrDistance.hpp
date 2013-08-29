#pragma once

#include <gex/base.hpp>
#include <gex/prim/Segment.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      /** @brief Generic template functor for calculating the squared distance
       *  @detail Returns always inf
       */
      template<typename A, typename B, typename RESULT = typename A::scalar_type>
      struct SqrDistance
      {
        RESULT operator()(const A& _a, const B& _b)
        {
          return misc::inf<RESULT>();
        }
      };

      /*********************************************
       * base::Point distances
       ********************************************/
      /// Calculates squared distance between two points
      template<typename MODEL>
      struct SqrDistance<base::Point<MODEL>,base::Point<MODEL>,
          typename MODEL::scalar_type>
      {
        typedef base::Point<MODEL> point_type;
        typename MODEL::scalar_type
        operator()(const point_type& _a, const point_type& _b)
        {
          return (_a - _b).sqrLength();
        }
      };

      /// Calculates squared distance between a point and bounds
      template<typename MODEL>
      struct SqrDistance<base::Point<MODEL>,base::Bounds<MODEL>,
          typename MODEL::scalar_type>
      {
        typedef base::Point<MODEL> point_type;
        typedef base::Bounds<MODEL> bounds_type;

        typename MODEL::scalar_type
        operator()(const point_type& _a, const bounds_type& _b)
        {
          if (_b.inside(_a)) return 0.0;
          typedef typename base::Point<MODEL>::scalar_type scalar_type;

          scalar_type _minDist = misc::inf<scalar_type>();
          GEX_FOREACH_DIM_(_a,i)
          {
            scalar_type _d = (_a[i] < _b.min()[i]) ? _b.min()[i] : _b.max()[i];
            _d -= _a[i];
            _minDist = std::min(_d*_d,_minDist);
          }
          return _minDist;
        }
      };


      /*********************************************
       * prim::Segment distances
       ********************************************/

      /// Calculates squared distance between two segments
      template<typename POINT>
      struct SqrDistance<prim::Segment<POINT>,
          prim::Segment<POINT>,
          typename POINT::scalar_type>
      {
        typedef POINT point_type;
        typedef prim::Segment<point_type> segment_type;

        typename POINT::scalar_type
        operator()(const segment_type& _a, const segment_type& _b)
        {
          return SqrDistance<point_type,point_type>()(_a[1],_b[0]);
        }
      };

      /// Calculates squared distance between a segment and bounds
      template<typename POINT>
      struct SqrDistance<prim::Segment<POINT>,
          typename prim::Segment<POINT>::bounds_type,
          typename POINT::scalar_type>
      {
        typedef POINT point_type;
        typedef prim::Segment<point_type> segment_type;
        typedef typename segment_type::bounds_type bounds_type;

        typename point_type::scalar_type
        operator()(const segment_type& _a, const bounds_type& _b)
        {
          return SqrDistance<point_type,bounds_type>()(_a[0],_b);
        }
      };
    }
  }
}

