#pragma once

#include "gex/misc/wrap_boost_geometry_begin.hpp"
#include <boost/geometry.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <boost/geometry/algorithms/simplify.hpp>
#include <boost/geometry/algorithms/difference.hpp>
#include <boost/geometry/algorithms/disjoint.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/perimeter.hpp>

#include "gex/misc/wrap_boost_geometry_end.hpp"

namespace gex
{
  namespace algorithm
  {
    using boost::geometry::difference;
    using boost::geometry::correct;
    using boost::geometry::perimeter;
    using boost::geometry::envelope;
    using boost::geometry::convex_hull;
    using boost::geometry::simplify;
    using boost::geometry::within;
    using boost::geometry::area;
    using boost::geometry::disjoint;
  }
}
