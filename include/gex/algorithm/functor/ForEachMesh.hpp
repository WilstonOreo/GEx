#pragma once

#include "ForEach.hpp"
#include "gex/prim/Triangle.hpp"
#include "gex/comp/TriangleMesh.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename POINT, bool IS_CONST>
      struct ForEach<POINT,prim::Triangle<POINT>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(POINT,prim::Triangle<POINT>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _tri, FUNCTOR f)
        {
          for (auto& _p : _tri.points())
            f(_p);
        }
      };

      template<typename POINT, typename SCALAR, bool IS_CONST>
      struct ForEach<prim::Triangle<POINT>,comp::TriangleMesh<SCALAR>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(prim::Triangle<POINT>,comp::TriangleMesh<SCALAR>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _mesh, FUNCTOR f)
        {
          for (auto& _tri : _mesh.triangles())
            f(_tri);
        }
      };

      /////////////////////////////////////////////////////////////
      // ForEach Ring functor
      template<typename POINT, bool IS_CONST>
      struct ForEach<POINT,comp::TriangleMesh<typename POINT::Scalar>,IS_CONST>
      {
        GEX_ALGORITHM_FOREACH_TYPEDEFS(POINT,comp::TriangleMesh<typename POINT::Scalar>)

        template<typename FUNCTOR>
        void operator()(prim_ref_type _mesh, FUNCTOR f)
        {
          for (auto& _tri : _mesh.triangles())
          {
            typedef typename tbd::AddConstIf<POINT,IS_CONST>::ref_type point_ref_type;
            ForEach<POINT,prim::Triangle<POINT>,IS_CONST>()(_tri,[&](point_ref_type _p)
            {
              f(_p);
            });
          }
        }
      };
    }
  }
}
