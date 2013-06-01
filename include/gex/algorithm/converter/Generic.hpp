#pragma once

#include <vector>
#include "gex/algorithm/boost.hpp"
#include "gex/prim.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace converter
    {
      namespace detail
      {
        template<typename SEGMENT>
        struct SegmentConverter
        {
          SegmentConverter(std::vector<SEGMENT>& _segments) :
            segments_(_segments) {}

          template<class T> void operator()( const T& _segment)
          {
            segments_.push_back(SEGMENT(_segment.first,_segment.second));
          }
          std::vector<SEGMENT>& segments_;
        };
      }

      template<typename IN, typename OUT>
      struct Converter
      {
        void operator()(const IN& _in, OUT& _out)
        {
          boost::geometry::convert(_in,_out);
        }
      };

      template<typename IN, typename OUT>
      void convert(const IN& _in, OUT& _out)
      {
        converter::Converter<IN,OUT>()(_in,_out);
      }

      template<typename OUT, typename IN>
      OUT convert(const IN& _in)
      {
        OUT _out;
        convert<IN,OUT>(_in,_out);
        return _out;
      }


#define GEX_CONVERTER(in,out)\
        template<>\
        struct Converter<in,out>\
        {\
          void operator()(const in& _in, out& _out);\
        };\
        void Converter<in,out>::operator()(const in& _in, out& _out)

      template<typename IN, typename OUT>
      struct Converter<std::vector<IN>,std::vector<OUT>>
      {
        void operator()(const std::vector<IN>& _in, std::vector<OUT>& _out)
        {
          _out.clear();
          _out.reserve(_in.size());
          for (IN& _tIn : _in)
          {
            OUT _tOut;
            Converter<IN,OUT>(_tIn,_tOut);
            _out.push_back(_tOut);
          }
          return _out;
        }
      };
    }
  }
}

