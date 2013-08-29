#pragma once

#include "Generic.hpp"
#include "Ring.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      template<typename POINT>
      struct Converter<prim::Segment<POINT>,prim::LineString<POINT>>
      {
        typedef prim::Segment<POINT> input_type;
        typedef prim::LineString<POINT> output_type;

        void operator()(const input_type& _in, output_type& _out)
        {
          _out.clear();
          _out.push_back(_in.front());
          _out.push_back(_in.back());
        }
      };

      /// Convert multiple Segments to a MultiLineString
      template<typename POINT>
      struct Converter<prim::MultiSegment<POINT>,prim::MultiLineString<POINT>>
      {
        typedef prim::MultiSegment<POINT> input_type;
        typedef prim::MultiLineString<POINT> output_type;
        void operator()(const input_type& _in, output_type& _out) 
        {
          typedef prim::Segment<POINT> segment_type;
          typedef prim::LineString<POINT> linestring_type;
          _out.clear();
          for (auto& _segment : _in)
          {
            linestring_type _ls;
            Converter<segment_type,linestring_type>()(_segment,_ls);
            _out.push_back(_ls);
          }
        }
      };
    }
  }
}
