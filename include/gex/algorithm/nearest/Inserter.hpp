#pragma once

namespace gex
{
  namespace algorithm
  {
    namespace nearest
    {
      template<
      typename CONTAINER,
               typename PRIM_DISTANCE_FUNCTOR>
      struct Inserter
      {
        typedef CONTAINER container_type;
        typedef typename CONTAINER::primitive_type primitive_type;

        Inserter(container_type& _container,
                 const primitive_type* _primitive) :
          container_(_container),
          primitive_(_primitive)
        {}

        void operator()(primitive_type const* _primitive)
        {
          if ( _primitive == nullptr) return;

          auto _distance = PRIM_DISTANCE_FUNCTOR()(*primitive_,*_primitive);
          if (container_.minDistance() < _distance) return;

          container_.insert(_distance,_primitive);
        }

      private:
        container_type& container_;
        const primitive_type* primitive_;
      };
    }
  }
}


