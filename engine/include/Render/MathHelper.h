#pragma once

namespace Blainn
{
    class MathHelper
    {
    public:
        template<typename T>
        static T Min(const T& a, const T& b)
        {
            return (a > b) ? (b) : (a);
        }
        
        template<typename T>
        static T Max(const T& a, const T& b)
        {
            return (a > b) ? (a) : (b);
        }

        template<typename T>
        static T Clamp(const T& value, const T& min, const T& max)
        {
	        return Min(Max(value, min), max);
        }
    };
}