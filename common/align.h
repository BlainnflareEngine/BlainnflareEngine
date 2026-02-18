#include "helpers.h"
#include <concepts>
#include <type_traits>

namespace Blainn
{

bool IsPowerOfTwo(std::integral auto val)
{
    return val > 0 && (val & (val - 1)) == 0;
}

template <typename T1, typename T2>
inline typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type AlignUp(T1 val, T2 alignment)
{
    static_assert(std::is_unsigned<T1>::value == std::is_unsigned<T2>::value, "Both types must be signed or unsigned");
    static_assert(!std::is_pointer<T1>::value && !std::is_pointer<T2>::value, "Types must not be pointers");
    BF_ASSERT(IsPowerOfTwo(alignment), "Alignment {} must be a power of 2", alignment);

    using T = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;
    return (static_cast<T>(val) + static_cast<T>(alignment - 1)) & ~static_cast<T>(alignment - 1);
}

template <typename PtrType, typename AlignType>
inline PtrType* AlignUp(PtrType* val, AlignType alignment)
{
    return reinterpret_cast<PtrType*>(AlignUp(reinterpret_cast<uintptr_t>(val), static_cast<uintptr_t>(alignment)));
}

template <typename PtrType>
inline PtrType* AlignUpPtr(PtrType* val)
{
    return reinterpret_cast<PtrType*>(AlignUp(reinterpret_cast<uintptr_t>(val), static_cast<uintptr_t>(alignof(PtrType))));
}

template <typename T1, typename T2>
inline typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type AlignDown(T1 val, T2 alignment)
{
    static_assert(std::is_unsigned<T1>::value == std::is_unsigned<T2>::value, "both types must be signed or unsigned");
    static_assert(!std::is_pointer<T1>::value && !std::is_pointer<T2>::value, "types must not be pointers");
    VERIFY(IsPowerOfTwo(alignment), "Alignment {} must be a power of 2", alignment);

    using T = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;
    return static_cast<T>(val) & ~static_cast<T>(alignment - 1);
}

template <typename PtrType, typename AlignType>
inline PtrType* AlignDown(PtrType* val, AlignType alignment)
{
    return reinterpret_cast<PtrType*>(AlignDown(reinterpret_cast<uintptr_t>(val), static_cast<uintptr_t>(alignment)));
}

template <typename PtrType>
inline PtrType* AlignDownPtr(PtrType* val)
{
    return reinterpret_cast<PtrType*>(AlignDown(reinterpret_cast<uintptr_t>(val), static_cast<uintptr_t>(alignof(PtrType))));
}

template <typename T1, typename T2>
inline typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type AlignDownNonPw2(T1 val, T2 alignment)
{
    VERIFY_EXPR(alignment > 0);
    static_assert(std::is_unsigned<T1>::value == std::is_unsigned<T2>::value, "both types must be signed or unsigned");
    static_assert(!std::is_pointer<T1>::value && !std::is_pointer<T2>::value, "types must not be pointers");

    using T = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;
    return static_cast<T>(val) - (static_cast<T>(val) % static_cast<T>(alignment));
}

template <typename T1, typename T2>
inline typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type AlignUpNonPw2(T1 val, T2 alignment)
{
    VERIFY_EXPR(alignment > 0);
    static_assert(std::is_unsigned<T1>::value == std::is_unsigned<T2>::value, "both types must be signed or unsigned");
    static_assert(!std::is_pointer<T1>::value && !std::is_pointer<T2>::value, "types must not be pointers");

    using T = typename std::conditional<sizeof(T1) >= sizeof(T2), T1, T2>::type;
    T tmp   = static_cast<T>(val) + static_cast<T>(alignment - 1);
    return tmp - (tmp % static_cast<T>(alignment));
}

}