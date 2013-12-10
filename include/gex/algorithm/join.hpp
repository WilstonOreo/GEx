#pragma once

#include <gex/index.hpp>
#include "convert.hpp"
#include "distance.hpp"
#include "connect.hpp"
#include "reverse.hpp"
#include "functor/Join.hpp"
#include "functor/JoinSegmentsToRings.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      namespace detail
      {
        template<typename PRIMITIVE>
        using RTreeNode = gex::index::RTreeNode<typename PRIMITIVE::bounds_type,PRIMITIVE const*>;

        template<typename PRIMITIVE>
        using RTree = gex::index::RTree<RTreeNode<PRIMITIVE>,4>;

        template<typename PRIMITIVE> struct Bounds {};

        template<typename POINT> 
        struct Bounds<prim::Segment<POINT>>
        {
          template<typename IN>
          typename IN::bounds_type operator()(const IN& _seg)
          {
            typename IN::bounds_type _bounds;
            _bounds.extend(_seg.front());
            _bounds.extend(_seg.back());
            return _bounds;
          }
        };
        
        template<typename POINT> 
        struct Bounds<prim::LineString<POINT>> : 
          Bounds<prim::Segment<POINT>> {};

        template<typename PRIMITIVES>
        auto join_generate_rtree(const PRIMITIVES& _primitives) ->  
          RTree<typename PRIMITIVES::value_type>
        {
          typedef typename PRIMITIVES::value_type primitive_type;
          RTree<primitive_type> _rtree;
          std::vector<RTreeNode<primitive_type>> _nodes;
          _nodes.reserve(_primitives.size());
          for (auto& _primitive : _primitives) 
          {
            _nodes.emplace_back(Bounds<primitive_type>()(_primitive),&_primitive);
          }
          _rtree.insert(_nodes.begin(),_nodes.end());
          return _rtree;
        }

        template<
          typename PRIMITIVE, 
          typename RTREE, 
          typename EXCLUDED, 
          typename EPSILON, 
          typename JUNCTION,
          typename LINESTRING>
        bool join_linestring(const PRIMITIVE& _primitive,
                             const RTREE& _rtree, EXCLUDED& _excluded, EPSILON _eps, JUNCTION _j,
                             LINESTRING& _lineString, bool _reverse)
        { 
          typedef RTreeNode<PRIMITIVE> node_type;
          if (_lineString.empty())
          {
            connect(_lineString,_primitive,_j);
          }
          auto _current = &_primitive;

          while(1)
          {
            _excluded.insert(_current);
            std::vector<node_type> _candidates;
            PRIMITIVE const* _finalCandidate = nullptr;
            bool _finalCandidateFlip = false; // P1 of segment is near linestring and not P0 (when flip = true)
            EPSILON _minDist = _eps * _eps;

            _rtree.query(gex::index::nearest(_lineString.back(),4), std::back_inserter(_candidates));
            for (auto& _candidate : _candidates)
            {
              if (_excluded.find(_candidate.second) != _excluded.end()) continue;

              auto _frontDist = algorithm::sqrDistance(_candidate.second->front(),_lineString.back());
              
              if (_frontDist <= _minDist) 
              {
                _finalCandidate = _candidate.second;
                _minDist = _frontDist;
                _finalCandidateFlip = false;
              }
              if (!_reverse) continue;
              auto _backDist = algorithm::sqrDistance(_candidate.second->back(),_lineString.back());
              if (_backDist <= _minDist) 
              {
                _finalCandidate = _candidate.second;
                _minDist = _backDist;
                _finalCandidateFlip = true;
              }
            }
            if (_finalCandidate)
            {
              PRIMITIVE _seg = *_finalCandidate;
              _current = _finalCandidate;
              if (_finalCandidateFlip)
              {
                reverse(_seg);
              }
              connect(_lineString,_seg,_j);
            } else 
              return _lineString.size() > 1;
          }
        }
      }


      template<typename POINT>
      struct Join<prim::MultiSegment<POINT>,prim::MultiLineString<POINT>,strategy::join::Threshold>
      {
        template<typename IN, typename OUT, typename STRATEGY, typename JUNCTION>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, JUNCTION _j)
        {
          auto _rtree = detail::join_generate_rtree(_in);
          std::set<typename IN::value_type const*> _excluded;
          for ( auto& _segment : _in )
          {
            if (_rtree.empty()) return;
            typename OUT::value_type _lineString;
            if (_excluded.find(&_segment) != _excluded.end()) 
              continue;

            if (detail::join_linestring(_segment,_rtree,_excluded,_strategy.threshold(),_j,_lineString,false))
            {
              _out.push_back(_lineString);
            }
          }
        }
      };
      
      template<typename POINT>
      struct Join<prim::MultiSegment<POINT>,prim::MultiLineString<POINT>,strategy::join::ThresholdWithReverse>
      {
        template<typename IN, typename OUT, typename STRATEGY, typename JUNCTION>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, JUNCTION _j)
        {
          auto _rtree = detail::join_generate_rtree(_in);
          std::set<typename IN::value_type const*> _excluded;
          for ( auto& _segment : _in )
          {
            if (_rtree.empty()) return;
            typename OUT::value_type _lineString;
            if (_excluded.find(&_segment) != _excluded.end()) 
              continue;

            if (detail::join_linestring(_segment,_rtree,_excluded,_strategy.threshold(),_j,_lineString,true))
            {
              reverse(_lineString);
              detail::join_linestring(_segment,_rtree,_excluded,_strategy.threshold(),_j,_lineString,true);            
              _out.push_back(_lineString);
            }
          }
        }
      };

      template<typename POINT, typename STRATEGY>
      struct Join<prim::MultiLineString<POINT>,prim::MultiLineString<POINT>,STRATEGY> : 
        Join<prim::MultiSegment<POINT>,prim::MultiLineString<POINT>,STRATEGY> {};

      template<typename POINT, typename POLYGON, typename STRATEGY>
      struct Join<prim::MultiRing<POINT>,prim::MultiPolygon<POLYGON>,STRATEGY> 
      {
        template<typename IN, typename OUT, typename STRATEGY_FUNCTOR, typename JUNCTION> 
        void operator()(const IN& _in, OUT& _out, STRATEGY_FUNCTOR _strategy, JUNCTION _j)
        {
          converter::convert(_in,_out);
        }
      };

      template<typename POINT, typename POLYGON, typename STRATEGY>
      struct Join<prim::MultiSegment<POINT>,prim::MultiPolygon<POLYGON>,STRATEGY> 
      {
        template<typename IN, typename OUT, typename STRATEGY_FUNCTOR, typename JUNCTION> 
        void operator()(const IN& _in, OUT& _out, STRATEGY_FUNCTOR _strategy, JUNCTION _j)
        {
          typedef prim::MultiRing<POINT> rings_type;
          rings_type _rings;
          Join<IN,rings_type,STRATEGY>()(_in,_rings,_strategy,_j);
          Join<rings_type,OUT,STRATEGY>()(_rings,_out,_strategy,_j);
        }
      };

      template<typename POINT, typename STRATEGY>
      struct Join<prim::MultiLineString<POINT>,prim::MultiRing<POINT>,STRATEGY> : 
        Join<prim::MultiSegment<POINT>,prim::MultiRing<POINT>,STRATEGY> 
      {};

      /// Join several linestrings to a single one
      template<typename POINT>
      struct Join<prim::MultiSegment<POINT>,prim::LineString<POINT>,strategy::join::Simple>
      {
      public:
        template<typename IN, typename OUT, typename STRATEGY, typename JUNCTION_FUNCTOR>
        void operator()(const IN& _in, OUT& _out, STRATEGY _strategy, JUNCTION_FUNCTOR _j)
        {
          for (auto& _prim : _in)
          {
            connect(_out,_prim,_j);
          }
        }
      };

      template<typename POINT, typename STRATEGY>
      struct Join<prim::MultiLineString<POINT>,prim::LineString<POINT>,STRATEGY> :
              Join<prim::MultiSegment<POINT>,prim::LineString<POINT>,STRATEGY> {};
    }

    using functor::Join;
    
    template<typename IN, typename OUT, typename STRATEGY, typename JUNCTION>
    void join(const IN& _in, OUT& _out, STRATEGY _strategy, JUNCTION _junction)
    {
      Join<IN,OUT,STRATEGY>()(_in,_out,_strategy,_junction);
    }
    
    template<typename IN, typename OUT, typename STRATEGY>
    void join(const IN& _in, OUT& _out, STRATEGY _strategy)
    {
      join(_in,_out,_strategy,junction::Straight<IN,OUT>());
    } 
  }

  using algorithm::join;
}
