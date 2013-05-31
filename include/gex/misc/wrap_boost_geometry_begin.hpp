// wrap boost geometry includes to prevent some concept bugs in boost that are detected by clang only
// use togehter with wrap_boost_geometry_end.hpp
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
#pragma clang diagnostic ignored "-Wunused-function"
#endif

