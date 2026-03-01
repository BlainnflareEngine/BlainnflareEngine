#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <random>
#include <limits>
#include <EASTL/functional.h>

namespace Blainn {

struct UUID final
{
public:
    using UnderlyingType = uint64_t;

    static constexpr uint32_t CountOfCharacters()
    {
        return sizeof(UnderlyingType) * 2 + 1;
    }

    UUID() noexcept : m_UUID(0) {}
    UUID(const UUID&) noexcept = default;
    UUID& operator=(const UUID&) noexcept = default;

    UUID(UnderlyingType v) noexcept : m_UUID(v) {}

    // Construct from 8 raw bytes (little-endian)
    explicit UUID(const uint8_t* bytes) noexcept {
        std::memcpy(&m_UUID, bytes, sizeof(UnderlyingType));
    }

    // Construct from binary string (expects size == 8)
    explicit UUID(const std::string& binaryBytes) noexcept {
        std::memcpy(&m_UUID, binaryBytes.data(), sizeof(UnderlyingType));
    }

    static UUID fromStrFactory(const std::string& s) {
        return UUID(parse_hex(s));
    }

    static UUID fromStrFactory(const char* raw) {
        return UUID(parse_hex(raw));
    }

    void fromStr(const char* s) {
        m_UUID = parse_hex(s);
    }

    std::string bytes() const {
        std::string out;
        out.resize(sizeof(UnderlyingType));
        std::memcpy(out.data(), &m_UUID, sizeof(UnderlyingType));
        return out;
    }

    void bytes(std::string& out) const {
        out.resize(sizeof(UnderlyingType));
        std::memcpy(out.data(), &m_UUID, sizeof(UnderlyingType));
    }

    void bytes(uint8_t out[sizeof(UnderlyingType)]) const noexcept {
        std::memcpy(out, &m_UUID, sizeof(UnderlyingType));
    }

    void bytes(char out[sizeof(UnderlyingType)]) const noexcept {
        std::memcpy(out, &m_UUID, sizeof(UnderlyingType));
    }

    // returns string in hex
    std::string str() const {
        char buf[CountOfCharacters()];
        to_hex(m_UUID, buf);
        return std::string(buf);
    }

    void str(char* out) const noexcept {
        to_hex(m_UUID, out);
    }

    void str(std::string& out) const {
        out.resize(CountOfCharacters());
        to_hex(m_UUID, out.data());
    }

    size_t hash() const noexcept {
        // splitmix64 finalizer for robust hashing
        UnderlyingType x = m_UUID;
        x += 0x9e3779b97f4a7c15ull;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
        x ^= (x >> 31);
        return static_cast<size_t>(x);
    }

    operator UnderlyingType() noexcept { return m_UUID; }
    operator const UnderlyingType() const noexcept { return m_UUID; }

    // Comparisons
    friend bool operator==(const UUID& lhs, const UUID& rhs) noexcept { return lhs.m_UUID == rhs.m_UUID; }
    friend bool operator!=(const UUID& lhs, const UUID& rhs) noexcept { return lhs.m_UUID != rhs.m_UUID; }

private:
    static void to_hex(UnderlyingType v, char* out) noexcept
    {
        static constexpr char lut[] = "0123456789abcdef";
        for (int i = CountOfCharacters() - 2; i >= 0; --i) {
            out[i] = lut[v & 0xF];
            v >>= 4;
        }
        out[CountOfCharacters() - 1] = 0;
    }

    static UnderlyingType parse_hex(std::string_view s)
    {
        UnderlyingType v = 0;
        int n = 0;
        for (char c : s) {
            if (c == ' ' || c == '\t') continue;
            int d =
                (c >= '0' && c <= '9') ? (c - '0') :
                (c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
                (c >= 'A' && c <= 'F') ? (c - 'A' + 10) : -1;
            if (d < 0)
            {
                v = 0;
                break;
            }
            v = (v << 4) | static_cast<UnderlyingType>(d);
            if (++n == CountOfCharacters() - 1) break;
        }
        return v;
    }

private:
    UnderlyingType m_UUID;
};

// Generator with the same conceptual role as before
template <typename RNG>
class UUIDGenerator
{
public:
    UUIDGenerator()
        : generator(new RNG(std::random_device()()))
        , distribution((std::numeric_limits<uint64_t>::min)(), (std::numeric_limits<uint64_t>::max)()) {}

    UUIDGenerator(uint64_t seed)
        : generator(new RNG(seed))
        , distribution((std::numeric_limits<uint64_t>::min)(), (std::numeric_limits<uint64_t>::max)()) {}

    UUIDGenerator(RNG& gen)
        : generator(&gen)
        , distribution((std::numeric_limits<uint64_t>::min)(), (std::numeric_limits<uint64_t>::max)()) {}

    UUID getUUID() {
        uint64_t v = distribution(*generator);
        if (v == 0) v = 1; // if you use 0 as "null"
        return UUID(v);
    }

private:
    RNG* generator;
    std::uniform_int_distribution<uint64_t> distribution;
};

} // namespace Blainn

namespace std {
template <> struct hash<Blainn::UUID> {
    size_t operator()(const Blainn::UUID& uuid) const noexcept { return uuid.hash(); }
};
}

namespace eastl {
template <> struct hash<Blainn::UUID> {
    std::size_t operator()(const Blainn::UUID& uuid) const { return uuid.hash(); }
};
}
