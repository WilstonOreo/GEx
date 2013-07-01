#pragma once

#include "gex/misc.hpp"
#include "Ray.hpp"
#include "Range.hpp"

#include <vector>
#include <limits>

namespace gex
{
    namespace base
    {
      /** @brief Rectangular bounding area or volume
       * @tparam MODEL Defines the scalar_type and the number of dimensions
       */
      template<class MODEL>
      struct Bounds : MODEL
      {
        GEX_MODEL(MODEL)
        typedef base::Point<MODEL> point_type;
        typedef base::Vec<MODEL> vec_type;
        typedef base::Ray<MODEL> ray_type;
        typedef base::Range<scalar_type> range_type;

        /// Maximum corner
        static const scalar_type maxmax()
        {
          return MODEL::scalarMax();
        }
        /// Minimum corner
        static const scalar_type minmin()
        {
          return MODEL::scalarMin();
        }

        /// Default constructor
        Bounds()
        {
          min_.vectorize(maxmax());
          max_.vectorize(minmin());
        }
        /** @brief Initialize bounds by two opposite corner points
         * @param _min minimum coordinate
         * @param _max maximum coordinate
         */
        Bounds(const point_type& _min, const point_type& _max)
        {
          operator()(_min,_max);
        }

        /** @brief Extend this bounds so that it includes the given bounds
         * @param _that Bounds to include
         * @attention assertion when this or _that are not valid
         */
        void extend(const Bounds& _that)
        {
          GEX_ASSERT(valid() && _that.valid());
          GEX_FOREACH_DIM(i)
          {
            min_[i] = std::min(_that.min_[i],min_[i]);
            max_[i] = std::max(_that.max_[i],max_[i]);
          }
        }
        /** @brief Extend this bounds so that it includes the given point
         * @param _that Point to include
         * @attention assertion when this is not valid
         */
        void extend(const point_type& _that)
        {
          GEX_ASSERT(valid());
          GEX_FOREACH_DIM(i)
          {
            min_[i] = std::min(_that[i],min_[i]);
            max_[i] = std::max(_that[i],max_[i]);
          }
        }

        /** @brief Return axis with largest extent
         * @return axis with largest extent
         */
        base::Axis dominantAxis() const
        {
          return size().dominantAxis() ;
        }

        static constexpr size_t numCorners()
        {
          return 1 << dimensions();
        }

        /// Retrieve the corners of bounds with as a vector of points
        std::array<point_type,numCorners()> corners() const
        {
          size_t i = 0;
          std::array<point_type,numCorners()> _corners;
          for (auto& _corner : _corners)
          {
            GEX_FOREACH_DIM(_axis)
            _corner[_axis] = i & (1 << _axis) ? max_[_axis] : min_[_axis];
            ++i;
          }
          return _corners;
        }

        /** @brief Set to the smallest bounds that include two points
         * @param _first first point
         * @param _second second point
         */
        void operator()(const point_type& _first, const point_type& _second)
        {
          min_ = _first;
          max_ = _second;
          validate();
        }

        /** @brief Return the size of this bounds
         * @return size of this bounds as vector
         */
        vec_type size() const
        {
          GEX_ASSERT(valid());
          return max_ - min_;
        }

        range_type range(Axis _axis) const
        {
          return range_type(min_[_axis],max_[_axis]);
        }


        typedef Bounds<Model<dimensions()-1,scalar_type>> proj_bounds_type;
        typedef Bounds<Model<dimensions()+1,scalar_type>> exp_bounds_type;

        /// Return projected bounds
        proj_bounds_type project(Axis _z) const
        {
          return proj_bounds_type(min().project(_z),max().project(_z));
        }

        exp_bounds_type expand(Axis _z, const range_type& _range) const
        {
          return exp_bounds_type(min().expand(_z,_range.min()),
                                 max().expand(_z,_range.max()));
        }

        /** @brief Return radius of this bounds
         * @return radius as scalar
         */
        scalar_type radius() const
        {
          return size().length()/2;
        }
        /** @brief Return center of this bounds
         * @return center as point
         */
        const point_type center() const
        {
          return 0.5*(max_.vec() + min_.vec());
        }

        /** @brief Split bounds in two halves
         * @param _splitPos scalar position on the given axis where to split this bounds
         * @param _axis axis to split along
         * @param _first [OUT] first half result
         * @param _second [OUT] second half result
         * @return true if _first and _second become the same, otherwise false
         */
        bool split(scalar_type _splitPos, base::Axis _axis, Bounds& _first, Bounds& _second) const
        {
          GEX_ASSERT(valid());
          // split position must be within bounds!
          GEX_ASSERT(_splitPos >= min_[_axis] && _splitPos <= max_[_axis]);
          // calculate bounds
          point_type _min = min_, _max = max_;
          _min[_axis] = _splitPos;
          _max[_axis] = _splitPos;
          _second(_min,max_);
          _first(min_,_max);
          // check if bounds has no extension on the given axis
          return min_[_axis] == max_[_axis];
        }
        /// normalize the coordinates in that bounds
        void validate()
        {
          GEX_FOREACH_DIM(i)
          {
            if (!valid_dim(i))
              std::swap(min_[i],max_[i]);
          }
        }
        /** @brief check if the coordinates in this bounds are normalized
         * @return true if coordinates are valid
         */
        bool valid() const
        {
          GEX_FOREACH_DIM(i)
          {
            if (!valid_dim(i))
              return false;
          }
          return true;
        }
        /** @brief check if some coordinates in this bounds are normalized
         * @param i dimension of the coordinate to check
         * @return true if coordinates are valid
         */
        bool valid_dim(int i) const
        {
          /// @todo I don't understand why you have to check min_ for maxmax and max_ for minmin. Please explain!
          if( min_[i] != maxmax() &&  max_[i] != minmin() )
            if (min_[i] > max_[i] )
              return false;
          return true;
        }
        friend Bounds operator*(const Bounds& _bounds, const Matrix<Model<dimensions()+1,scalar_type>> _matrix)
        {
          point_type _min = _bounds.min()*_matrix,
                     _max = _bounds.max()*_matrix;
          return Bounds(_min,_max);
        }

        template<class ARCHIVE>
        void serialize( ARCHIVE& _ar, const unsigned int _fileVersion )
        {
          _ar & min_;
          _ar & max_;
        }

        TBD_PROPERTY_REF_MON(point_type,min,validate)
        TBD_PROPERTY_REF_MON(point_type,max,validate)
      };
    }
  }


