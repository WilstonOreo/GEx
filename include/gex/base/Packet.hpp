#pragma once

#include "gex/misc.hpp"
#include <array>

namespace gex
{
    namespace base
    {
      template<size_t N, typename SCALAR>
      struct Packet
      {
        static constexpr size_t size()
        {
          return N;
        }

        typedef SCALAR scalar_type;
        typedef std::array<scalar_type,size()> array_type;
        typedef uint32_t cmp_result_type;

        Packet() {}
        Packet(const scalar_type& _s) 
        {
          vectorize(_s);
        }

        /// Methods to access coordinate value in a certain dimension
        scalar_type& operator[] (size_t i)
        {
          return data_[i];
        }
        const scalar_type& operator[] (size_t i) const
        {
          return data_[i];
        }        

        void operator += ( const Packet& _p)
        {}

        void operator -= ( const Packet& _p)
        {}

        friend Packet& operator+( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& operator-( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& operator*( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& operator*( const scalar_type& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& operator*( const Packet& _lhs, const scalar_type& _rhs)
        {
        }

        friend Packet& operator/( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& sqrt( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& rsqrt( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend Packet& rsqrt( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend cmp_result_type operator<( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend cmp_result_type operator>( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend cmp_result_type operator<=( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend cmp_result_type operator>=( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend cmp_result_type operator==( const Packet& _lhs, const Packet& _rhs)
        {
        }

        friend cmp_result_type operator!=( const Packet& _lhs, const Packet& _rhs)
        {
        }

        TBD_PROPERTY_REF(array_type,data)
      };
    }
  }
