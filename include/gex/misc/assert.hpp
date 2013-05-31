#pragma once

#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

/// assertion in gex library
#define GEX_ASSERT(x) BOOST_ASSERT(x)
/// static assertion in gex library
#define GEX_STATIC_ASSERT(x) BOOST_STATIC_ASSERT(x)
/// assertion specific to mark unimplemented code sections
#define GEX_NOT_IMPLEMENTED() GEX_ASSERT(BOOST_CURRENT_FUNCTION == "method not implemented yet")
/// assertion specfitc to mark untested code sections
#define GEX_NOT_TESTED() GEX_ASSERT(BOOST_CURRENT_FUNCTION == "method never called before")

