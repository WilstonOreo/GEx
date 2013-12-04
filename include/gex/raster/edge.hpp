#pragma once

#include <gex/prim.hpp>
#include <gex/index.hpp>
#include "for_each.hpp"

namespace gex
{
  namespace raster
  {
    namespace strategy
    {
      namespace edge
      {
        struct Filter {};
        struct Points 
        {
          Points(float _threshold, float _collapse = 0.0) : 
            threshold_(_threshold),
            collapse_(_collapse) {}

          TBD_PROPERTY_RO(float,threshold)
          TBD_PROPERTY_RO(float,collapse)
        };
        struct PointsRTree : Points 
        {
          PointsRTree(float _threshold, float _collapse = 0.0) : 
            Points(_threshold,_collapse) {}
        };
      }
    }
    namespace functor
    {
      template<typename IMAGE, typename STRATEGY> struct Edge {};
    }
    
    using functor::Edge;

    template<typename IN, typename STRATEGY, typename OUT>
    void edge(const IN& _in, STRATEGY _s, OUT& _out)
    {
      Edge<IN,STRATEGY>()(_in,_s,_out);
    }
    
    template<typename IN, typename STRATEGY>
    typename Edge<IN,STRATEGY>::output_type edge(const IN& _in, STRATEGY _s)
    {
      typename Edge<IN,STRATEGY>::output_type _out;
      edge(_in,_s,_out);
      return _out;
    }

    namespace functor
    {
      using namespace strategy::edge;
      
      /// Computes edge filter for an image
      template<typename IMAGE>
      struct Edge<IMAGE,Filter>
      {
        typedef IMAGE output_type;

        template<typename STRATEGY>
        void operator()(const IMAGE& _in, STRATEGY, output_type& _out)
        {
          typedef typename output_type::pixel_type pixel_type;
          _out.clear();
          _out.resize(_in.width(),_in.height());

          for_each(_in,strategy::for_each::Diff(),[&](
              const pixel_type& _pixel, 
              int _horz, int _vert, int _diag,
            size_t _x, size_t _y)
          {
            _out(_x,_y) = (std::abs(_horz)+std::abs(_vert)) / 2;
          });
        }
      };

      template<typename IMAGE>
      struct Edge<IMAGE,Points>
      {
        typedef MultiPoint2 output_type;

        template<typename STRATEGY>
        void operator()(const IMAGE& _in, STRATEGY _s, output_type& _out)
        {
          typedef typename IMAGE::pixel_type pixel_type; 
          float _invMax = 1.0 / pixel::max<pixel_type>();

          for_each(_in,strategy::for_each::Diff(),[&](
              const pixel_type& _pixel, 
              int _horz, int _vert, int _diag,
            size_t _x, size_t _y)
          {
            float _h = _horz * _invMax;
            float _v = _vert * _invMax;
            if (std::abs(_h) > _s.threshold() || std::abs(_v) > _s.threshold())
              _out.emplace_back(_x + _h,_y + _v);
          });
        }
      };
      
      template<typename IMAGE>
      struct Edge<IMAGE,PointsRTree>
      {
        typedef gex::index::RTree<std::pair<gex::Point2,size_t>> rtree_type;
        typedef std::pair<MultiPoint2,rtree_type> output_type;

        template<typename STRATEGY>
        void operator()(const IMAGE& _in, STRATEGY _s, output_type& _out)
        {
          typedef typename IMAGE::pixel_type pixel_type; 
          float _invMax = 1.0 / pixel::max<pixel_type>();

          _out.first = edge(_in,Points(_s.threshold(),_s.collapse())); 

          _out.second.clear();
          for (size_t _i = 0; _i < _out.first.size(); ++_i)
            _out.second.insert(std::make_pair(_out.first[_i],_i));
        }
      };
    }

  }
}
