#pragma once

#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <DirectXColors.h>

#include "DirectXTK12/Src/d3dx12.h"
#include "d3dcompiler.h"
#include "DirectXCollision.h"

#include "FreyaCoreDefines.h"
#include "FreyaCoreTypes.h"
#include "aliases.h"

#include <EASTL/shared_ptr.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/queue.h>
#include <EASTL/string.h>

using Microsoft::WRL::ComPtr;

inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr)
        : std::runtime_error(HrToString(hr))
        , m_hr(hr)
    {
    }
    HRESULT Error() const
    {
        return m_hr;
    }

private:
    const HRESULT m_hr;
};

#define SAFE_RELEASE(p) \
    if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

inline Blainn::Color HexToColor(const std::string &hex)
{
    std::string clean = hex;
    if (clean[0] == '#') clean = clean.substr(1);

    if (clean.length() != 8) return Blainn::Color(Colors::White);

    unsigned int argb;
    if (!(std::istringstream(clean) >> std::hex >> argb)) return Blainn::Color(Colors::White);

    uint8_t r = argb >> 16 & 0xFF;
    uint8_t g = argb >> 8 & 0xFF;
    uint8_t b = argb >> 0 & 0xFF;
    uint8_t a = argb >> 24 & 0xFF;

    return Blainn::Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}