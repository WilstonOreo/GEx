#pragma once

#include "gex/bsp.hpp"
#include "gex/algorithm/sqrDistance.hpp"
#include "Inserter.hpp"

namespace gex
{
    namespace algorithm
    {
      namespace nearest
      {
        namespace 
        {
          template<typename PRIMITIVE>
          struct State
          {
            typedef PRIMITIVE primitive_type;
            typedef typename primitive_type::bounds_type bounds_type;
    
            TBD_PROPERTY_REF(bounds_type,bounds)
          }; 
        }

        template<
          typename PRIMITIVE,
          typename CONTAINER,
          typename PRIM_DISTANCE_FUNCTOR = algorithm::SqrDistance<PRIMITIVE,PRIMITIVE>,
          typename NODE_DISTANCE_FUNCTOR = algorithm::SqrDistance<PRIMITIVE, typename PRIMITIVE::bounds_type> 
        >
        struct Visitor : bsp::Visitor<State<PRIMITIVE>>
        {
          typedef PRIMITIVE primitive_type;
          typedef CONTAINER container_type;
          typedef State<primitive_type> state_type;
          typedef Inserter<PRIMITIVE,CONTAINER,PRIM_DISTANCE_FUNCTOR> inserter_type;

          Visitor(const primitive_type* _primitive,
                  container_type& _container) :
            primitive_(_primitive),
            container_(_container)
          {
          }

          template<typename TREE>
          bool root(const TREE& _tree)
          {
            this->state().bounds(_tree.bounds());
            return true;
          }

        template<typename TREE, typename INNERNODE>
          WhichNodes inner(const TREE& _tree, const INNERNODE& _innerNode, state_type& _stateToPush)
          {
              /// Split current node's bounds in to a left and right one
              bounds_type _left, _right;
              state_.bounds().split(state_.node()->inner().splitPos(),
                                    state_.node()->inner().axis(),
                                    _left,_right);

              /// Calculate primitives squared distance to node's bounds
              auto _leftDist = NODE_DISTANCE_FUNCTOR()(*primitive_,_left),
                   _rightDist = NODE_DISTANCE_FUNCTOR()(*primitive_,_right);
              bool _leftFirst = _leftDist <= _rightDist;
              scalar_type _minDist = container_.minDistance();

              bool _traverseLeft  = _leftDist <= _minDist;
              bool _traverseRight = _rightDist <= _minDist;

              if (_leftFirst)
              {
                _state.bounds(_left);
                if (_traverseRight) 
                {
                  _stateToPush.bounds(_right);
                  return inherited_type::LEFT_RIGHT;
                }
                return inherited_type::LEFT;
              } else
              {
                _state.bounds(_right);
                if (_traverseLeft)
                {
                  _stateToPush.bounds(_left);
                  return inherited_type::RIGHT_LEFT;
                }
                return inherited_type::RIGHT;
              }
          }

          template<typename TREE, typename LEAFNODE>
          bool leaf(const TREE& _tree, const LEAFNODE& _leafNode)
          {
            _tree.nodes().forEachLeafPrimitive(
                _leafNode,
                inserter_type(conainer_,primitive_));
          }
        };
      }
    }
  }
}
