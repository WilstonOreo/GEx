#pragma once

#include <gex/prim/LineString.hpp>
#include <gex/prim/Segment.hpp>
#include "distance.hpp"

namespace gex
{
  namespace algorithm
  {
    namespace junction
    {
      namespace detail
      {
        template<typename POINT, typename OUTPUT>
        void straight_points(const POINT& _a, const POINT& _b, OUTPUT& _output)
        {
          if (sqrDistance(_a,_b) > 0.0)
            _output.push_back(_a);
          _output.push_back(_b);
        }
      }

      template<typename PRIM_A, typename PRIM_B>
      struct Straight
      {
        template<typename A, typename B, typename OUTPUT>
        void operator()(const A& _a, const B& _b, OUTPUT& _output)
        {
          detail::straight_points(_a.back(),_b.front(),_output);
        }
      };
/*
      template<typename POINT>
      struct Straight<prim::LineString<POINT>,prim::Segment<POINT>> :
              Straight<prim::Segment<POINT>,prim::Segment<POINT>> {};

      template<typename POINT>
      struct Straight<prim::Segment<POINT>,prim::LineString<POINT>> :
              Straight<prim::Segment<POINT>,prim::Segment<POINT>> {};

      template<typename POINT>
      struct Straight<prim::LineString<POINT>,prim::LineString<POINT>> :
              Straight<prim::Segment<POINT>,prim::Segment<POINT>> {};
      
      template<typename POINT>
      struct Straight<prim::MultiLineString<POINT>,prim::MultiLineString<POINT>> :
              Straight<prim::Segment<POINT>,prim::Segment<POINT>> {};
*/
    }

    namespace functor
    {
      template<typename A, typename B>
      struct Connect {};

      template<typename POINT>
      struct Connect<POINT,POINT>
      {
        typedef POINT point_type;
        typedef prim::LineString<POINT> output_type;

        template<typename JUNCTION>
        void operator()(const point_type& _a, const point_type& _b, output_type& _output, JUNCTION _j)
        {
          _j(_a,_b,_output);
        }
      };

      template<typename POINT>
      struct Connect<prim::Segment<POINT>,prim::Segment<POINT>>
      {
        typedef prim::LineString<POINT> output_type;
        typedef prim::Segment<POINT> segment_type;

        template<typename JUNCTION>
        void operator()(const segment_type& _a, const segment_type& _b, output_type& _output, JUNCTION _j)
        {
          _output.push_back(_a.front());
          JUNCTION(_a,_b,_output);
          _output.push_back(_b.back());
        }
      };

      template<typename POINT>
      struct Connect<prim::LineString<POINT>,prim::Segment<POINT>>
      {
        typedef prim::Segment<POINT> segment_type;
        typedef prim::LineString<POINT> linestring_type;
        typedef linestring_type output_type;

        template<typename JUNCTION>
        void operator()(output_type& _out, const segment_type& _a, JUNCTION _j)
        {
          if (_out.empty())
          {
            _out.push_back(_a.front());
            _out.push_back(_a.back());
            return;
          }
          std::cout << _out.size() << std::endl;
          _j(_out,_a,_out);
          _out.push_back(_a.back());
        }

        template<typename JUNCTION>
        void operator()(const linestring_type& _a, const segment_type& _b, output_type& _out, JUNCTION _j)
        {
          if (_a.empty())
          {
            _out.push_back(_a.front());
            _out.push_back(_a.back());
            return;
          }
          _out.insert(_out.end(),_a.begin(),_a.end()-1);
          _j(_a,_b,_out);
          _out.push_back(_b.back());
        }
      };

      template<typename POINT>
      struct Connect<prim::Segment<POINT>,prim::LineString<POINT>>
      {
        typedef prim::Segment<POINT> segment_type;
        typedef prim::LineString<POINT> linestring_type;
        typedef linestring_type output_type;

        template<typename JUNCTION>
        void operator()(const segment_type& _a, const linestring_type& _b, output_type& _out, JUNCTION _j)
        {
          if (_b.empty())
          {
            _out.push_back(_a.front());
            _out.push_back(_a.back());
            return;
          }
          _out.push_back(_a.front());
          _j(_a,_b,_out);
          _out.insert(_out.end(),_a.begin()+1,_a.end());
        }
      };

      template<typename POINT>
      struct Connect<prim::LineString<POINT>,prim::LineString<POINT>>
      {
        typedef prim::LineString<POINT> output_type;
        typedef prim::LineString<POINT> linestring_type;

        template<typename JUNCTION>
        void operator()(const linestring_type& _a, const linestring_type& _b, output_type& _output, JUNCTION _j)
        {
          if (_a.empty())
          {
            _output = _b;
            return;
          }
          if (_b.empty())
          {
            _output = _a;
            return;
          }

          _output.insert(_output.end(),_a.begin(),_a.end()-1);
          _j(_a,_b,_output);
          _output.insert(_output.end(),_b.begin()+1,_b.end());
        }

        template<typename JUNCTION>
        void operator()(linestring_type& _a, const linestring_type& _b, JUNCTION _j)
        {
          if (_b.empty())
          {
            return;
          }
          if (_a.empty())
          {
            _a = _b;
            return;
          }

          _j(_a,_b,_a);
          _a.insert(_a.end(),_b.begin()+1,_b.end());
        }
      };
    }

    using functor::Connect;

    template<typename A, typename B, typename C>
    void connect(const A& _a, const B& _b, C& _c)
    {
      Connect<A,B>()(_a,_b,_c,junction::Straight<A,B>());
    }

    template<typename A, typename B>
    void connect(A& _a, const B& _b)
    {
      Connect<A,B>()(_a,_b,junction::Straight<A,B>());
    }

    template<typename A, typename B, typename C, typename JUNCTION>
    void connect(const A& _a, const B& _b, C& _c, JUNCTION _j)
    {
      Connect<A,B>()(_a,_b,_c,_j);
    }

    template<typename A, typename B, typename JUNCTION>
    void connect(A& _a, const B& _b, JUNCTION _j)
    {
      Connect<A,B>()(_a,_b,_j);
    }
  }

  using algorithm::connect;
}

