/*
  ==============================================================================

    DistSrc.h
    Created: 3 Jun 2021 1:31:04pm
    Author:  Fin Scholefield

  ==============================================================================
*/

#pragma once

namespace Data
{
    template <typename Type>
    struct DP
    {
    public:
        DP(Type _dr, Type _b, Type _m, Type _de, double _samplerate = 44100)
        {updt_dr(_dr); updt_b(_b); updt_m(_m); updt_de(_de); samplerate = _samplerate ;}
        
        Type drive;
        Type buzz;
        Type mod;
        Type depth;
        
        void updt_dr(Type _dr) { drive = _dr; }
        void updt_b(Type _b) { buzz = _b; }
        void updt_m(Type _m) { mod = _m; }
        void updt_de(Type _de) { depth = _de; }
        void updt_sr(double _sr) { samplerate = _sr; }
        
        double samplerate;
        
    };
}


namespace DistSrc
{
    
template <typename Type>
    static Type drive(Type input, Data::DP<Type>& distParams)
    {
        return std::tanh(input * distParams.drive);
    }
    
template <typename Type>
    static Type phaseDist_1(Type input, Data::DP<Type>& distParams)
    {
        return std::atan(input - distParams.mod) + (std::abs(distParams.mod) * input);
    }

template <typename Type>
    static Type phaseDist_2(Type input, Data::DP<Type>& distParams)
    {
        return input + (distParams.mod / distParams.drive);
    }

template <typename Type>
    static Type eulerSine(Type input, Data::DP<Type>& distParams)
    {
        const Type i = distParams.buzz;
        const Type euler = std::pow(juce::MathConstants<Type>::euler, (Type)(-1.0 * i * (std::pow(input, 2))));
        
        return std::sin(input * euler * ((Type)9.0 * i));
    }

template <typename Type>
    static Type phaseInterpolation(Type input, Data::DP<Type>& distParams)
    {
        const Type i = distParams.buzz;
        return (phaseDist_1(input * i, distParams) * eulerSine(input, distParams)) + (phaseDist_2(input * ((Type)1.0 - i), distParams));
    }

template <typename Type>
    static Type phaseModulation(Type input, double modPos, Data::DP<Type>& distParams)
    {
        const Type d = distParams.depth;
        const Type m = distParams.mod;
        
        return input * std::tanh(d * std::sin(input + m * modPos));
    }

template <typename Type>
    static Type buzzsawPhaseshaper(Type input, Data::DP<Type>& distParams)
    {
        const Type pi = phaseInterpolation(input, distParams);
        
        return drive(pi, distParams);
    }
};

