#pragma once

#include "State.hpp"
#include "gex/bsp/node/Intersection.hpp"
#include "gex/bsp/node/BoundsExtension.hpp"
#include "gex/bsp/container/PrimitiveContainer.hpp"

namespace gex
{
  namespace bsp
  {
    namespace build
    {
      struct NoExclusion
      {
        template<typename PRIMITIVE, typename STATE>
        bool operator()(const PRIMITIVE& _primitive, const STATE& _state)
        {
          return false;
        }
      };

      template<typename NODE,
               typename PRIMITIVE,
               typename PRIM_EXCLUDE_FUNCTOR = NoExclusion,
               typename PRIM_NODE_INTERSECTION = node::Intersection<PRIMITIVE>,
               typename PRIM_BOUNDS_EXT = node::BoundsExtension<PRIMITIVE> >
      struct StateWithPrimitives : State<NODE>
      {
        typedef PRIMITIVE primitive_type;
        typedef container::PrimitiveContainer<primitive_type> input_type;
        typedef container::PrimitivePtrContainer<primitive_type> primitive_cntr_type;

        GEX_BSP_NODE_TYPES(NODE)

        typedef State<node_type> state_base_type;

        void init(const input_type& _input, unsigned _stackPos)
        {
          if (_stackPos == 0)
          {
            // Reserve appropriate number of primitives
            primitives_.clear();
            primitives_.reserve(_input.size());
            bounds_type& _bounds = state_base_type::nodeGeometry().bounds();
            _bounds = bounds_type();
            // Fill initial primitive list with pointers of input objects
            // and calculate bounds on the fly
for (const auto& _primitive : _input)
            {
              PRIM_BOUNDS_EXT()(_primitive,_bounds);
              primitives_.push_back(&_primitive);
            }
          }
          else
          {
            // Reserves n / 2^p items on stack
            // n = number of primitives (_input.size()), p = _stackPos-1
            // Ensures that memory is allocated a priori and
            // memory requirements are independent from MAX_DEPTH of a kd-tree
            primitives_.reserve(_input.size() >> (_stackPos-1));
          }
        }

        void change(const inner_node_type& _innerNode,
                    StateWithPrimitives& _stateToPush)
        {
          _stateToPush.primitives().clear();
          primitive_cntr_type& _primitives = primitives();

          // Insert objects of current state into left and right subnode
          auto _leftIt = primitives_.begin();
for (auto& _primitive : primitives_)
          {
            if (PRIM_EXCLUDE_FUNCTOR()(_primitive,*this)) continue;

            auto _result = PRIM_NODE_INTERSECTION()(*_primitive,state_base_type::nodeGeometry());
            if (_result.right()) _stateToPush.primitives().push_back(_primitive);
            if (_result.left())
            {
              *_leftIt = _primitive;
              ++_leftIt;
            }
          }
          // Erase remaining objects at back of container
          _primitives.erase(_leftIt,_primitives.end());

          state_base_type::change(_innerNode,_stateToPush);
        }

        primitive_cntr_type& primitives()
        {
          return primitives_;
        }

        const primitive_cntr_type& primitives() const
        {
          return primitives_;
        }

        TBD_PROPERTY(const primitive_type*,primitive)
      protected:
        primitive_cntr_type primitives_;
      };
    }
  }
}
