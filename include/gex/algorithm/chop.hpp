#pragma once

#include <gex/algorithm/step.hpp>

namespace gex
{
  namespace algorithm
  {
    namespace functor
    {
      template<typename PRIMITIVE>
      struct Chop {};

      template<typename POINT>
      struct Chop<prim::Segment<POINT>>
      {
        typedef POINT point_type;
        typedef prim::Segment<POINT> segment_type;
        typedef segment_type output_type;

        template<typename MARKERS, typename OUTPUT>
        void operator()(
            const segment_type& _segment, 
            const MARKERS& _markers,
            OUTPUT& _out)
        {
          bool _mode = false;
          output_type _seg(_segment);
          step(_segment,_markers,
          [&](const point_type& _p)
          {
            _seg[_mode] = _p;
            if (distance(_seg.p0(),_seg.p1()) > 0)
            {
              _out.push_back(_seg);
              if (!_mode)
              {
                std::swap(_out.back()[0],_out.back()[1]);
              }
            }
            _mode = !_mode;
          },
          [&](const point_type& _p)
          {
            _seg.p1() = _p;
          });
          _seg[_mode] = _segment.p1();
          _out.push_back(_seg);
          if (!_mode)
          {
            std::swap(_out.back()[0],_out.back()[1]);
          }
        }
      };

      template<typename POINT>
      struct Chop<prim::LineString<POINT>>
      {
        typedef POINT point_type;
        typedef prim::LineString<POINT> linestring_type;
        typedef linestring_type output_type;

        template<typename MARKERS, typename OUTPUT>
        void operator()(const linestring_type& _lineString,
            const MARKERS& _markers,
            OUTPUT& _out)
        {
          if (_markers.empty()) 
          {
            _out.push_back(_lineString);
            return;
          }
          output_type _ls;
          step(_lineString,_markers,
          [&](const point_type& _p)
          {
            _ls.push_back(_p);
            if (!_ls.empty())
            {
              _out.push_back(_ls);
            }
            _ls.clear();
            _ls.push_back(_p);
          },
          [&](const point_type& _p)
          {
            _ls.push_back(_p);
          });
        }
      };
      
      template<typename POINT>
      struct Chop<prim::Ring<POINT>>
      {
        typedef POINT point_type;
        typedef prim::Ring<POINT> ring_type;
        typedef prim::LineString<POINT> output_type;

        template<typename MARKERS, typename OUTPUT>
        void operator()(const ring_type& _ring,
            const MARKERS& _markers,
            OUTPUT& _out)
        {
          output_type _r;
          step(_ring,_markers,
          [&](const point_type& _p)
          {
            _r.push_back(_p);
            if (!_r.empty())
            {
              _out.push_back(_r);
            }
            _r.clear();
            _r.push_back(_p);
          },
          [&](const point_type& _p)
          {
            _r.push_back(_p);
          });

          if (_out.empty()) return;
          _out.front().insert(_out.front().begin(),_r.begin(),_r.end());
        }
      };

    }

    using functor::Chop;

    template<typename PRIMITIVE, typename MARKERS, typename OUTPUT>
    void chop(const PRIMITIVE& _primitive, const MARKERS& _markers, OUTPUT& _output)
    {
      Chop<PRIMITIVE>()(_primitive,_markers,_output);
    }

    template<typename PRIMITIVE, typename MARKERS>
    std::vector<typename Chop<PRIMITIVE>::output_type>
      chop(const PRIMITIVE& _primitive, const MARKERS& _markers)
    {
      std::vector<typename Chop<PRIMITIVE>::output_type> _output;
      chop(_primitive,_markers,_output);
      return _output;
    }
  }
}


