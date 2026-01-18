//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"

namespace Blainn
{
struct TextureHandle;
enum class TextureType;

class Material : public FileSystemObject
{
public:
    Material() = default;
    Material(const Path &path, const eastl::string &shader);
    virtual ~Material() override;

    virtual void Move() override;
    virtual void Delete() override;
    virtual void Copy() override;

    void SetTexture(const eastl::shared_ptr<TextureHandle> &textureHandle, TextureType type);
    TextureHandle &GetTextureHandle(TextureType type);

    void SetMaterialTransform(const Mat4 &matTransform)
    {
        m_materialTransform = matTransform;
    }

    const Mat4 &GetMaterialTransform() const { return m_materialTransform; }


    void SetAlbedoColor(const Color& color);
    void SetNormalScale(float scale);
    void SetRoughnessScale(float roughness);
    void SetMetallicScale(float metallic);

    Color GetDefaultAldedo()const { return m_albedoColor; };
    float GetDefaultNormaScale()const { return m_normalScale; };
    float GetDefaultMetallicScale()const { return m_metallicScale; };
    float GetDefaultRougnessScale()const { return m_roughnessScale; };

    bool AreTexturesLoaded();

    void SetShader(const eastl::string &shader);
    const eastl::string &GetShader() const;

    BOOL HasTexture(TextureType type) const;

    void MarkFramesDirty()
    {
        NumFramesDirty = kNumFramesMarkDirty;
    };

    void FrameResetDirtyFlags()
    {
        NumFramesDirty > 0 ? --NumFramesDirty : NumFramesDirty;
    }

    bool IsFramesDirty() const
    {
        // TODO : replace with actual logic
        return true; //NumFramesDirty > 0;
    }
private:

    Mat4 m_materialTransform = Mat4::Identity;

    eastl::shared_ptr<TextureHandle> m_albedoTexture = nullptr;
    eastl::shared_ptr<TextureHandle> m_normalTexture = nullptr;
    eastl::shared_ptr<TextureHandle> m_metallicTexture = nullptr;
    eastl::shared_ptr<TextureHandle> m_roughnessTexture = nullptr;
    eastl::shared_ptr<TextureHandle> m_aoTexture = nullptr;
    eastl::string m_shader = "";
    Color m_albedoColor = Color(1, 1, 1, 1);
    float m_normalScale = 1.0f;
    float m_metallicScale = 0.5f;
    float m_roughnessScale = 0.5f;

    bool m_bAreTexturesLoaded = false;

    inline static const int kNumFramesMarkDirty = 3;
    int NumFramesDirty = kNumFramesMarkDirty; // NumFrameResources
};
} // namespace Blainn
