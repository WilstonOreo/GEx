#pragma once

#include "Ring.hpp"

namespace gex
{
    namespace prim
    {
      template<typename RING> 
      struct Polygon
      {
        typedef RING ring_type;
        typedef ring_type sub_primitive_type;
        typedef std::vector<ring_type> holes_type;
        typedef typename ring_type::size_type size_type;
        typedef typename ring_type::bounds_type bounds_type;
        typedef typename ring_type::scalar_type scalar_type;
        typedef typename ring_type::model_type model_type;
        typedef typename ring_type::point_type point_type;
        typedef typename ring_type::range_type range_type;

        Polygon() :
          correct_(false)
        {
        }

        Polygon(const ring_type& _boundary) :
          correct_(false),
          boundary_(_boundary)
        {
          update();
        }

        Polygon(const ring_type& _boundary, const holes_type& _holes) :
          correct_(false),
          boundary_(_boundary),
          holes_(_holes)
        {
          update();
        }

        void add(const ring_type& _ring)
        {
          switch (_ring.location())
          {
          case ring_type::INNER:
            holes_.push_back(_ring);
            break;
          case ring_type::OUTER:
            boundary_ = _ring;
            break;
          }
        }

        template<class ARCHIVE>
        void serialize( ARCHIVE& _ar, const unsigned int _fileVersion )
        {
          _ar & boundary_;
          _ar & holes_;
        }

        void update()
        {
          if (correct_) return;
          boundary_.update();
          for( auto& _ring : holes_ ) _ring.update();

          boost::geometry::correct(*this);
          bounds_= boundary_.bounds();
        }

        bounds_type& bounds() 
        {
          correct_ = false;
          return bounds_; 
        }

        ring_type& boundary() 
        {
          correct_ = false;
          return boundary_; 
        }

        holes_type& holes()
        {
          correct_ = false;
          return holes_;
        }

        size_type countPoints() const
        {
          size_type _count=0;
          _count += boundary().size();
          for( auto& _h : holes() )
            _count += _h.size();
          return _count;
        }

        TBD_PROPERTY_RO(bool,correct)
        TBD_PROPERTY_REF_RO(ring_type,boundary)
        TBD_PROPERTY_REF_RO(bounds_type,bounds)
        TBD_PROPERTY_REF_RO(holes_type,holes)
      };
    }
  }
