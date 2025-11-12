#pragma once

namespace Blainn
{
    class FreyaMath
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

        static inline const XMVECTOR ForwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        static inline const XMVECTOR RightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        static inline const XMVECTOR UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        static inline const XMVECTOR ZeroVector = XMVectorZero();
        static inline const XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    };
}