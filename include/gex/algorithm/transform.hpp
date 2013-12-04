#pragma once

namespace gex
{
  namespace algorithm
  {
    GEX_FUNCTOR_WITH_STRATEGIES(Transform,transform_copy)
    {
      template<typename BOUNDS>
      struct Bounds 
      {
        typedef BOUNDS bounds_type;
        Bounds(const bounds_type& _bounds) : 
          bounds_(_bounds) {}

        TBD_PROPERTY_REF(bounds_type,bounds)
      };

      template<typename BOUNDS>
      struct BoundsInv : Bounds<BOUNDS> 
      {
        typedef BOUNDS bounds_type;
        BoundsInv(const BOUNDS& _bounds) : 
          Bounds<bounds_type>(_bounds) {}
      };
    }

    template<typename IN, typename STRATEGY>
    void transform(IN& _in, STRATEGY _s)
    {
      Transform<IN,STRATEGY>()(_in,_s);
    }
    
    namespace functor
    {
      using namespace strategy::transform;

      template<>
      struct Transform<Point2,Bounds<Bounds2>>
      {
      };

      template<>
      struct Transform<gex::Point2,BoundsInv<Bounds2>>
      {
      };
      
      template<>
      struct Transform<Bounds2,Bounds<Bounds2>>
      {
      };

      template<>
      struct Transform<Bounds2,BoundsInv<Bounds2>>
      {
      };
      
      template<typename POINT, typename STRATEGY>
      struct Transform<prim::Segment<POINT>,STRATEGY>
      {
        typedef prim::Segment<POINT> output_type;

        template<typename IN, typename S>
        void operator()(IN& _in, S _s)
        {
          for_each<POINT>(_in,[](POINT& _point)
          {
            _point = transform(_point,_s);
          });
        }

        template<typename IN, typename S, typename OUT>
        void operator()(const IN& _in, S _s, OUT& _out)
        {
          OUT _out = _in;
          this->operator()(_out,_s);
        }
      };
       
      template<typename POINT, typename STRATEGY>
      struct Transform<prim::LineString<POINT>,STRATEGY> : 
        Transform<prim::Segment<POINT>,STRATEGY>
      {
        typedef prim::LineString<POINT> output_type;
      };
       
      template<typename POINT, typename STRATEGY>
      struct Transform<prim::Ring<POINT>,STRATEGY> : 
        Transform<prim::LineString<POINT>,STRATEGY>
      {
        typedef prim::Ring<POINT> output_type;
      };
      
      template<typename POINT, typename STRATEGY>
      struct Transform<prim::MultiLineString<POINT>,STRATEGY> : 
        Transform<prim::LineString<POINT>,STRATEGY>
      {
        typedef prim::MultiLineString<POINT> output_type;
      };
       
      template<typename RING, typename STRATEGY>
      struct Transform<prim::Polygon<RING>,STRATEGY> : 
        Transform<RING,STRATEGY>
      {
        typedef prim::Polygon<RING> output_type;
      };

      template<typename POLYGON, typename STRATEGY>
      struct Transform<prim::MultiPolygon<POLYGON>,STRATEGY> : 
        Transform<POLYGON,STRATEGY>
      {
        typedef prim::MultiPolygon<POLYGON> output_type;
      };
    }
  }

  using algorithm::transform;
  using algorithm::transform_copy;
}
