#pragma once

#include "union.hpp"
#include "intersects.hpp"

namespace gex
{
    namespace algorithm
    {
      struct Unify
      {
        template<typename PRIMITIVES>
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

              auto&& _newPrimitives = *i | *j;
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
        Unify()(_input,_output);
      }

      template<typename PRIMITIVES>
      PRIMITIVES unify(const PRIMITIVES& _input)
      {
        PRIMITIVES _output;
        Unify()(_input,_output);
        return _output;
      }
    }
  }
