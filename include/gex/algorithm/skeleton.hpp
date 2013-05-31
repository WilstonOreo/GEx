#pragma once

#include "gex/prim.hpp"
#include "gex/cgal.hpp"
#include <CGAL/create_straight_skeleton_2.h>

namespace gex
{
    namespace algorithm
    {
      struct StraightSkeleton
      {
        TBD_PROPERTY_REF(std::vector<Point2>,vertices)
        TBD_PROPERTY_REF(std::vector<Segment>,edges)
      };

      namespace functor
      {
        template<typename PRIMITIVE>
        struct Skeleton
        {
        };

        template<>
        struct Skeleton<Ring>
        {
          void operator()(const Ring& _ring, StraightSkeleton& _skeleton)
          {
            auto _cgalRing = cgal::adapt(_ring);
            typedef CGAL::Straight_skeleton_2<cgal::Traits> Ss;
            typedef std::shared_ptr<Ss> SsPtr;

     //       auto iss = CGAL::create_interior_straight_skeleton_2(_ring);
      //      for (auto it = iss->vertices_begin(); it != iss->vertices_end(); ++it)
            {
           //   _skeleton.vertices().push_back(*it);
            }
          }
        };

        template<>
        struct Skeleton<Polygon>
        {
          void operator()(const Polygon& _polygon, StraightSkeleton& _skeleton)
          {
          }
        };

      }
    }
  }

namespace gex
{
    namespace algorithm
    {
      using functor::Skeleton;

      template<typename PRIMITIVE>
      void skeleton(const PRIMITIVE& _primitive, StraightSkeleton& _skeleton)
      {
        Skeleton<PRIMITIVE>()(_primitive,_skeleton);
      }

      template<typename PRIMITIVE>
      StraightSkeleton skeleton(const PRIMITIVE& _primitive)
      {
        StraightSkeleton _skeleton;
        Skeleton<PRIMITIVE>()(_primitive,_skeleton);
        return _skeleton;
      }
    }
  }
