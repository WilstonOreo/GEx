#pragma once

#include "Static.hpp"
#include "PrimitiveContainer.hpp"

namespace gex
{
  namespace bsp
  {
    namespace container
    {
      /// A StaticContainer holds nodes, primitive lists and defines how setup node attributes
      template<typename NODE, typename PRIMITIVE>
      struct StaticWithPrimitives : Static<NODE>
      {
        typedef NODE node_type;
        typedef typename node_type::inner_type inner_node_type;
        typedef typename node_type::leaf_type leaf_node_type;

        typedef Static<node_type> base_cntr_type;

        typedef PRIMITIVE primitive_type;
        typedef PrimitivePtrContainer<primitive_type> primitive_cntr_type;
        typedef typename primitive_cntr_type::const_iterator prim_const_iterator;
        typedef typename primitive_cntr_type::iterator prim_iterator;

        template<typename BUILD_STATE>
        void init(const BUILD_STATE& _state)
        {
          // Reserve appropriate number of nodes
          base_cntr_type::nodes_.reserve(_state.primitives().size());
          base_cntr_type::template init<BUILD_STATE>(_state);
        }

        template<typename BUILD_STATE>
        inner_node_type& insertInner(const BUILD_STATE& _state)
        {
          //   base_cntr_type::template insertInner<BUILD_STATE>(_state);
          if (_state.nodeIndex() >= base_cntr_type::nodes_.size())
            base_cntr_type::nodes_.resize(base_cntr_type::nodes_.size()+2);

          inner_node_type& _inner = base_cntr_type::nodes_[_state.nodeIndex()].inner();

          uint32_t _index = base_cntr_type::nodes_.size();

          _inner.setup(_index,
                       _state.nodeGeometry().axis(),
                       _state.nodeGeometry().splitPos());
          return _inner;
        }

        template<typename BUILD_STATE>
        leaf_node_type& insertLeaf(const BUILD_STATE& _state)
        {
          if (_state.nodeIndex() >= base_cntr_type::nodes_.size())
            base_cntr_type::nodes_.resize(base_cntr_type::nodes_.size()+1);

          leaf_node_type& _leaf = base_cntr_type::nodes_[_state.nodeIndex()].leaf();

          //  base_cntr_type::template insertLeaf<BUILD_STATE>(_state);

          // Each leaf has a range of primitives as attributes,
          // get it from primitive container
          _leaf.begin(primitives_.size());
          _leaf.end(primitives_.size()+_state.primitives().size());

          // Insert state's primitives into primitive container
          primitives_.insert(primitives_.end(),
                             _state.primitives().begin(),
                             _state.primitives().end());
          return _leaf;
        }

        /// Returns two iterators which determine the range where leaf's primitives are stored
        void leafRange(const leaf_node_type& _leaf,
                       prim_const_iterator& _begin,
                       prim_const_iterator& _end) const
        {
          _begin = primitives_.begin() + _leaf.begin();
          _end = primitives_.begin() + _leaf.end();
        }

        template<typename FUNCTOR>
        void forEachLeafPrimitive(const leaf_node_type& _leaf, FUNCTOR _f) const
        {
          prim_const_iterator _begin, _end;
          leafRange(_leaf,_begin,_end);
          prim_const_iterator it = _begin;
          for (it = _begin ; it != _end ; ++it) _f(*it);
        }

        template<typename FUNCTOR>
        void forEachLeafPrimitive(const leaf_node_type& _leaf, FUNCTOR _f)
        {
          prim_const_iterator _begin, _end;
          leafRange(_leaf,_begin,_end);
          prim_const_iterator it = _begin;
          for (it = _begin ; it != _end ; ++it) _f(*it);
        }

      protected:
        primitive_cntr_type primitives_;
      };
    }
  }
}
