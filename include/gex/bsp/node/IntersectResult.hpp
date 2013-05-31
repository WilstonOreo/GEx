#pragma once

namespace gex
{
  namespace bsp
  {
    namespace node
    {
      /// Struct which holds the result of an intersection test between split plane and primitive
      struct IntersectResult
      {
      public:
        IntersectResult(bool _left = false, bool _right = false)
        {
          set(_left,_right);
        }

        void set(bool _left, bool _right)
        {
          result_ = _left | (_right << 1);
        }

        bool left()
        {
          return result_ & 1;
        }
        bool right()
        {
          return result_ & 2;
        }
        bool both()
        {
          return left() && right();
        }
      private:
        int result_;
      };
    }
  }
}
