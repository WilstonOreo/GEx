#pragma once

#include "gex/bsp.hpp"

namespace gex
{
  namespace io
  {
    namespace svg
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

        template <typename PRIMITIVE>
        struct DrawPrimitive
        { 
          Buffer& buffer_;
              
          DrawPrimitive(Buffer& _buffer) : buffer_(_buffer) {}
          void operator()(const PRIMITIVE* _prim)
          {
            Draw<PRIMITIVE>()(*_prim,"stroke:orange",buffer_);
          }
        };

      }

      template<typename PRIMITIVE>
      struct BSPVisitor : bsp::Visitor<State<PRIMITIVE>>
      {
        typedef PRIMITIVE primitive_type;
        typedef State<primitive_type> state_type;
        typedef bsp::Visitor<state_type> visitor_type;
        typedef typename visitor_type::WhichNodes WhichNodes;

        BSPVisitor(const Style& _style, Buffer& _buffer) : 
          style_(_style), 
          drawLeafPrimitives_(true),
          buffer_(_buffer)
        {
        }

        template<typename TREE>
        bool root(const TREE& _tree)
        {
          this->state().bounds(_tree.bounds());
          Draw<typename TREE::bounds_type>()(_tree.bounds(),style_,buffer_);
          return true;
        }

        template<typename TREE, typename INNERNODE>
        WhichNodes inner(const TREE& _tree, const INNERNODE& _innerNode, state_type& _stateToPush)
        {
          typedef typename primitive_type::point_type point_type;
          typedef typename primitive_type::bounds_type bounds_type;
          typedef typename primitive_type::scalar_type scalar_type;

          auto& _state = this->state();

          bounds_type _left, _right;
          base::Axis _axis = _innerNode.axis();
          scalar_type _splitPos = _innerNode.splitPos();
          _state.bounds().split(_splitPos,_axis,_left,_right);

          std::cout << _left.min() << _left.max() << std::endl;
          std::cout << _right.min() << _right.max() << std::endl;

          Segment _segment;

          if (_axis == base::X)
            _segment(point_type(_splitPos,_state.bounds().min().y()),
                     point_type(_splitPos,_state.bounds().max().y()));
          else
          if (_axis == base::Y)
            _segment(point_type(_state.bounds().min().x(),_splitPos),
                     point_type(_state.bounds().max().x(),_splitPos));

          Draw<Segment>()(_segment,style_,buffer_);

          _state.bounds(_left);
          _stateToPush.bounds(_right);
          return visitor_type::LEFT_RIGHT;
        }

        template<typename TREE, typename LEAFNODE>
        bool leaf(const TREE& _tree, const LEAFNODE& _leafNode)
        {
          if (drawLeafPrimitives())
          {
            _tree.nodes().forEachLeafPrimitive(_leafNode,DrawPrimitive<primitive_type>(buffer_));
          }
          return false;
        }

        TBD_PROPERTY_REF(Style,style)
        TBD_PROPERTY(bool,drawLeafPrimitives)
      private:
        Buffer& buffer_;
      };
    }
  }
}

