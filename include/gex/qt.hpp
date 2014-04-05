#pragma once

// Gex Qt Wrappers
#include <QPoint>
#include <QRect>
#include "base.hpp"


namespace gex
{
  namespace qt
  {
    template<typename PRIMITIVE>
    struct Adapter {};

    template<typename PRIMITIVE>
    struct AdaptType {};

#define GEX_QT_ADAPT_TYPE(in_type,out_type)\
      template<> struct AdaptType<in_type>\
      {\
        typedef out_type type;\
      };

    GEX_QT_ADAPT_TYPE(gex::Point2,QPointF)
    GEX_QT_ADAPT_TYPE(QPointF,gex::Point2)
    
    GEX_QT_ADAPT_TYPE(gex::Bounds2,QRectF)
    GEX_QT_ADAPT_TYPE(QRectF,gex::Bounds2)


    template<typename PRIMITIVE>
    void adapt(const PRIMITIVE& _in, typename AdaptType<PRIMITIVE>::type& _out)
    { 
      Adapter<PRIMITIVE>()(_in,_out);
    }

    template<typename PRIMITIVE>
    typename AdaptType<PRIMITIVE>::type adapt(const PRIMITIVE& _in)
    {
      typename AdaptType<PRIMITIVE>::type _out;
      adapt<PRIMITIVE>(_in,_out);
      return _out;
    }
 
#define GEX_QT_ADAPTER(primitive)\
      template<>\
      struct Adapter<primitive>\
      {\
        typedef primitive in_type;\
        typedef AdaptType<primitive>::type out_type;\
        inline void operator()(const in_type&, out_type&);\
      };\
      void Adapter<primitive>::operator()(const in_type& _in, out_type& _out)

    GEX_QT_ADAPTER(gex::Point2)
    {
      _out = QPointF(_in.x(),_in.y());
    }
    
    GEX_QT_ADAPTER(QPointF)
    {
      _out = gex::Point2(_in.x(),_in.y());
    }

    GEX_QT_ADAPTER(gex::Bounds2)
    {
      _out = QRectF(adapt(_in.min()),adapt(_in.max()));
    }

    GEX_QT_ADAPTER(QRectF)
    {
      _out = gex::Bounds2(adapt(_in.topLeft()),adapt(_in.bottomRight()));
    }
  }
}
