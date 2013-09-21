#pragma once

namespace gex
{
  namespace algorithm
  {
    namespace strategy 
    {
      struct Simple {};

      struct Threshold 
      {
        Threshold(double _threshold) :
          threshold_(_threshold) {}
      
        TBD_PROPERTY_REF(double,threshold)
      };

      struct ThresholdWithReverse : Threshold 
      {
        ThresholdWithReverse(double _threshold) :
          Threshold(_threshold) {}
      };

      template<typename POINT>
      struct DefectRings : Threshold
      {
        typedef prim::MultiLineString<POINT> output_type;

        DefectRings(output_type& _out, double _threshold, double _maxDist) : 
          Threshold(_threshold),
          maxDist_(_maxDist),
          out_(_out) {}

        double const& maxDist() const
        {
          return maxDist_;
        }

        template<typename LINESTRING>
        void push_back(const LINESTRING& _lineString)
        {
          out_.push_back(_lineString);
        }

      private:
        double maxDist_;
        output_type& out_;
      };    
    }

    namespace functor
    {
      template<typename IN_PRIMITIVE, typename OUT_PRIMITIVE, typename STRATEGY>
      struct Join {};
    }
  }
}
