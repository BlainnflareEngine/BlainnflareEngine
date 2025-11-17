#pragma once

#include "Render/ShadowMap.h"

class CascadeShadowMap : public ShadowMap
{
public:
    CascadeShadowMap(ID3D12Device *device, UINT width, UINT height, UINT cascadesCount);
    CascadeShadowMap(const CascadeShadowMap &lhs) = delete;
    CascadeShadowMap &operator=(const CascadeShadowMap &lhs) = delete;

    virtual ~CascadeShadowMap() noexcept override;

protected:
    virtual void CreateDescriptors() override;

private:
    void CreateResource();
};