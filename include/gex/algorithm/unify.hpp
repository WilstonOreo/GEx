#pragma once

#include "union.hpp"
#include "intersects.hpp"

namespace gex
{
    namespace algorithm
    {
      template<
        typename PRIMITIVES, 
        typename UNION = Union<
          typename PRIMITIVES::value_type,
          typename PRIMITIVES::value_type>>
      struct Unify
      {
        void operator()(const PRIMITIVES& _in, PRIMITIVES& _out)
        {
          typedef typename PRIMITIVES::value_type primitive_type;
          std::list<primitive_type> _input;
          for (auto& p : _in ) _input.push_back(p);

          for (auto i = _input.begin(); i != _input.end(); ++i)
          {
            auto j = i;
            ++j;
            for (; j != _input.end(); ++j)
            {
              if (!(i->bounds() && j->bounds())) continue;

              PRIMITIVES _newPrimitives;
              UNION()(*i,*j,_newPrimitives);
              if (_newPrimitives.size() == 1)
              {
                _newPrimitives.back().update();
                *i = _newPrimitives.back();
                _input.erase(j);
                j = i;
              }
            }
          }
          _out.insert(_out.end(),_input.begin(),_input.end());
        }
      };

      template<typename PRIMITIVES>
      void unify(const PRIMITIVES& _input, PRIMITIVES& _output)
      {
        Unify<PRIMITIVES>()(_input,_output);
      }

      template<typename PRIMITIVES>
      PRIMITIVES unify(const PRIMITIVES& _input)
      {
        PRIMITIVES _output;
        Unify<PRIMITIVES>()(_input,_output);
        return _output;
      }
    }
  }
