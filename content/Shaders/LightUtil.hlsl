#define MaxLights 16

struct DirectionalLight
{
    float4 Color;      // xyz: color * TempRGB, w: intensity
    float3 Direction;
    float pad;
};

struct PointLight
{
    float4 Color;     // xyz: color * TempRGB, w: intensity
    float3 Position;
    float FalloffStart;
    float3 TempRGB;   // padding actually 
    float FalloffEnd; // range
};

struct SpotLight
{
    float4 Color;    // xyz: color * TempRGB, w: intensity
    float3 Direction;
    float FalloffStart;
    float3 Position;
    float FalloffEnd; // range
};

struct PointLightInstanceData
{
    float4x4 World;
    PointLight Light;
};

struct SpotLightInstanceData
{
    float4x4 World;
    SpotLight Light;
};

struct MaterialData
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float4x4 MatTransform;
    uint DiffuseMapIndex;
    uint NormalMapIndex;
    uint RoughnessMapIndex;
    uint MetallicMapIndex;
};

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess; // 1 - roughness
};

float3 SchlickApproximation(float3 fresnelR0, float3 halfVec, float3 lightDir)
{
    float cosIncidentAngle = saturate(dot(halfVec, lightDir)); // clamp between [0, 1] -> [0, 90] degrees
    float f0 = 1.0f - cosIncidentAngle;
    float3 reflection = fresnelR0 + (1.0f - fresnelR0) * (f0 * f0 * f0 * f0 * f0);
    return reflection;
}

// Check F.Luna Introduction to DirectX12 paragraph 8.8 to see lighting model details.
float3 BlinnPhong(float3 lightStrength, float3 lightDir, float3 N, float3 viewDir, Material mat)
{
    const float m = mat.Shininess * 256.0f;
    
    float3 halfVec = normalize(viewDir + lightDir);
    
    float3 schlick = SchlickApproximation(mat.FresnelR0, halfVec, lightDir);
    float normFactor = (m + 8.0f) / 8.0f;
    float nh = pow(max(dot(halfVec, N), 0.0f), m);
    
    float3 specAlbedo = schlick * normFactor * nh;
    
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);
    
    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

float3 ComputeDirLight(DirectionalLight L, float3 N, float3 viewDir, Material mat, float shadowFactor)
{
    float3 lightDirection = normalize(-L.Direction);
    float NdotL = max(dot(lightDirection, N), 0.0f);
    float3 lightStrength = NdotL * L.Color.xyz * L.Color.w;
    return BlinnPhong(lightStrength, lightDirection, N, viewDir, mat) * shadowFactor;
}

float CalcAttenuation(float distance, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - distance) / (falloffEnd - falloffStart));
}

float3 ComputePointLight(PointLight L, float3 N, float3 posW, float3 viewDir, Material mat)
{
    float3 lightVec = L.Position - posW;
    
    float d = length(lightVec);
    
    if (d > L.FalloffEnd)
        return 0.0f.xxx;
    
    float3 lightDir = lightVec / d;
    float NdotL = max(dot(lightDir, N), 0.0f);
    float attenuation = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    float3 lightStrength = L.Color.xyz * L.Color.w * NdotL * attenuation;
    
    return BlinnPhong(lightStrength, lightDir, N, viewDir, mat);
}

float3 ComputeSpotLight(SpotLight L, Material mat, float3 posW, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
    float3 lightVec = L.Position - posW;

    // The distance from surface to light.
    float d = length(lightVec);

    // Range test.
    if (d > L.FalloffEnd)
        return 0.0f.xxx;

    // Normalize the light vector.
    float3 lightDir = lightVec / d;

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightDir, normal), 0.0f);
    float3 lightStrength = L.Color.xyz * ndotl;

    // Attenuate light by distance.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // Scale by spotlight
    float spotFactor = pow(max(dot(-lightDir, L.Direction), 0.0f), L.Color.w);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightDir, normal, toEye, mat);
}

// The idea is to calculate every light object's illumination strength related to it's type specific parameters
// and to pass it in BlinnPhong model function

// FORWARD SHADING APPROACH

//float4 ComputeLight(Light gLights[MaxLights], float3 N, float3 posW, float3 viewDir, Material mat, float shadowFactor)
//{
//    float3 litColor = 0.0f;
    
//#if NUM_DIR_LIGHTS
    
//    float3 dirLight = 0.f;
    
//    [unroll]
//    for (int i = 0; i < NUM_DIR_LIGHTS; i++)
//    {
//        // stuff with shadows supposed to work with only one directional light
//        dirLight += CalcDirLight(gLights[i], N, viewDir, mat, shadowFactor);
//    }
//    litColor += dirLight;
//#endif
    
//#if NUM_POINT_LIGHTS
    
//    float3 pointLight = 0.f;
    
//    [unroll]
//    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i++)
//    {
//        pointLight += CalcPointLight(gLights[i], N, posW, viewDir, mat);

//    }
//    litColor += pointLight;
//#endif
    
//#if NUM_SPOT_LIGHTS
    
//    float3 spotLight = 0.f;
    
//    [unroll]
//    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; i++)
//    {
//        spotLight += CalcSpotLight(gLights[i], N, posW, viewDir, mat);
//    }
//    litColor += spotLight;
//#endif
    
//    return float4(litColor, 0.0f);
//}