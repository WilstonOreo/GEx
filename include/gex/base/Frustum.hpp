#pragma once

namespace gex
{
    namespace base
    {
      template<typename MODEL>
      struct Frustum
      {
        typedef RayPacket<4,MODEL> boundingrays_type;

        template<
          typename VIEWPORT,
          typename MODELVIEW_MATRIX, 
          typename PROJECTION_MATRIX
        >
        void generateBoundingRays(point_type)
        {
        }

        TBD_PROPERTY_REF(boundingrays_type,boundingRays)
      };
    }
  }
