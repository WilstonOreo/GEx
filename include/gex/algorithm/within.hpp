#pragma once

#include <boost/geometry/algorithms/within.hpp>
#include <boost/geometry/algorithms/disjoint.hpp>
#include <boost/geometry/geometries/linestring.hpp>

namespace gex
{
    namespace algorithm
    {
      template <typename RING>
      bool within(const RING& _ring1, const RING& _ring2)
      {
        if (!inside(_ring1.bounds(),_ring2.bounds())) return false;

        typedef typename RING::point_type point_type;
        if (!boost::geometry::within(_ring1.front(), _ring2)) return false;

        using namespace boost::geometry::model;

        return boost::geometry::disjoint(linestring<point_type>(_ring1.begin(), _ring1.end()),
                                         linestring<point_type>(_ring2.begin(), _ring2.end()));
      }
    }
  }
