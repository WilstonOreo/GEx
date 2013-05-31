#pragma once

#include "gex/prim.hpp"

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
        template<typename MODEL>
        struct SqrDistance<prim::Segment<MODEL>,
            prim::Segment<MODEL>,
            typename MODEL::scalar_type>
        {
          typedef prim::Segment<MODEL> segment_type;
          typename MODEL::scalar_type
          operator()(const segment_type& _a, const segment_type& _b)
          {
            typedef typename segment_type::point_type point_type;
            return SqrDistance<point_type,point_type>()(_a[1],_b[0]);
          }
        };

        /// Calculates squared distance between a segment and bounds
        template<typename MODEL>
        struct SqrDistance<prim::Segment<MODEL>,
            typename prim::Segment<MODEL>::bounds_type,
            typename MODEL::scalar_type>
        {
          typedef prim::Segment<MODEL> segment_type;
          typedef typename segment_type::bounds_type bounds_type;
          typename MODEL::scalar_type
          operator()(const segment_type& _a, const bounds_type& _b)
          {
            typedef typename segment_type::point_type point_type;
            return SqrDistance<point_type,bounds_type>()(_a[0],_b);
          }
        };

        /*********************************************
         * prim::LineString distances
         ********************************************/

        /// Calculates squared distance between two line strings
        template<typename MODEL>
        struct SqrDistance<prim::LineString<MODEL>,
            prim::LineString<MODEL>,
            typename MODEL::scalar_type>
        {
          typedef prim::LineString<MODEL> linestring_type;
          typename MODEL::scalar_type
          operator()(const linestring_type& _a, const linestring_type& _b)
          {
            typedef typename linestring_type::point_type point_type;
            return SqrDistance<point_type,point_type>()(_a.back(),_b.front());
          }
        };

        /// Calculates squared distance between a line string and bounds
        template<typename MODEL>
        struct SqrDistance<prim::LineString<MODEL>,
            typename prim::LineString<MODEL>::bounds_type,
            typename MODEL::scalar_type>
        {
          typedef prim::LineString<MODEL> linestring_type;
          typedef typename linestring_type::bounds_type bounds_type;

          typename MODEL::scalar_type
          operator()(const linestring_type& _a, const bounds_type& _b)
          {
            typedef typename linestring_type::point_type point_type;
            return SqrDistance<point_type,bounds_type>()(_a.back(),_b);
          }
        };
      }
    }
  }
