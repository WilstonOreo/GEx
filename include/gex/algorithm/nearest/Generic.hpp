#pragma once

#include "gex/bsp.hpp"
#include "Inserter.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace nearest
    {
      template<
      typename CONTAINER,
               typename PRIM_DISTANCE_FUNCTOR,
               typename INPUT>
      struct Generic
      {
        typedef Inserter<CONTAINER,PRIM_DISTANCE_FUNCTOR> inserter_type;
        typedef CONTAINER container_type;
        typedef typename inserter_type::primitive_type primitive_type;
        typedef typename container_type::result_type result_type;
        typedef INPUT input_type;
        typedef std::set<primitive_type const*> excluded_type;

        Generic(const input_type& _primitives) : primitives_(_primitives)
        {
          //       kdTree_.build(_primitives);
        }

        void clear()
        {
          excluded_.clear();
        }

        result_type find(const primitive_type& _primitive)
        {
          container_type _container;
          inserter_type _inserter(_container,&_primitive);

          excluded_.insert(&_primitive);

//          std::cout << excluded_.size() << " " << primitives_.size() << std::endl;

for (const auto& p: primitives_)
          {
            if (!excluded(p)) _inserter(&p);
          }
          //        kdTree_.traversal(visitor_);
          return _container.nearest();
        }

        bool excluded(const primitive_type& _primitive) const
        {
          return excluded_.count(&_primitive);
        }

        const excluded_type& excluded() const
        {
          return excluded_;
          //     return visitor_.excluded();
        }

        excluded_type& excluded()
        {
          return excluded_;
          //   return visitor_.excluded();
        }

      private:
        const input_type& primitives_;
        excluded_type excluded_;
      };
    }
  }
}

