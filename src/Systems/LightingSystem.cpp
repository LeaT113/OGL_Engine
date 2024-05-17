#include "LightingSystem.hpp"
#include "../OGL/Graphics.hpp"

LightingSystem::LightingSystem() = default;

void LightingSystem::RegisterLight(LightComponent* light)
{
    Instance()._lights.push_back(light);
}

void LightingSystem::UpdateLights()
{
    if(Instance()._lightsUniformBuffer == nullptr) {
        Instance()._lightsUniformBuffer = std::make_unique<UniformBuffer>(sizeof(_data));
        Graphics::Bind(*Instance()._lightsUniformBuffer, 2);
    }

    int pointLightIndex = 0;
    int spotLightIndex = 0;

    for (const auto light : Instance()._lights)
    {
        switch(light->GetType())
        {
            case LightComponent::Type::Ambient:
                *reinterpret_cast<AmbientLight*>(Instance()._data + AMBIENT_LIGHT_DATA_OFFSET) = light->GetAmbientLight();
                break;

            case LightComponent::Type::Direct:
                *reinterpret_cast<DirectLight*>(Instance()._data + DIRECT_LIGHT_DATA_OFFSET) = light->GetDirectLight();
                break;

            case LightComponent::Type::Point:
                if(pointLightIndex >= MAX_POINT_LIGHTS)
                    continue;
                *reinterpret_cast<PointLight*>(Instance()._data + POINT_LIGHT_DATA_OFFSET + sizeof(PointLight) * pointLightIndex++) = light->GetPointLight();
                break;

            case LightComponent::Type::Spot:
                if(spotLightIndex >= MAX_SPOT_LIGHTS)
                    continue;
                *reinterpret_cast<SpotLight*>(Instance()._data + SPOT_LIGHT_DATA_OFFSET + sizeof(SpotLight) * spotLightIndex++) = light->GetSpotLight();
                break;
        }
    }

    Instance()._lightsUniformBuffer->SetData(0, sizeof(_data), &Instance()._data);
}

std::vector<LightComponent*> LightingSystem::GetShadowCasters()
{
    std::vector<LightComponent*> shadowCasters;

    for (auto light : Instance()._lights)
        if (light->IsShadowCasting())
            shadowCasters.push_back(light);

    return shadowCasters;
}

Texture& LightingSystem::GetShadowTexture(LightComponent::Type lightType, int index)
{
    switch (lightType)
    {
    case LightComponent::Type::Ambient:
        throw std::runtime_error("LightingSystem Ambient lights do not have shadows");

    case LightComponent::Type::Direct:
        throw std::runtime_error("LightingSystem Attempted to access shadow for unsupported light");

    case LightComponent::Type::Point:
        if (Instance()._shadowMapsCube[index].Access() == nullptr)
            Instance()._shadowMapsCube[index] = Handle<Texture>::Make(Texture::Type::TexCubemap, SHADOW_TEX_FORMAT, Texture::Params {
                .width = POINT_LIGHT_SHADOW_RESOLUTION, .height = POINT_LIGHT_SHADOW_RESOLUTION,
                .settings = { .mipmaps = false, .tiling = Texture::Tiling::Extend }
            });

        return *Instance()._shadowMapsCube[index];

    case LightComponent::Type::Spot:
        if (Instance()._shadowMaps2D[index].Access() == nullptr)
            Instance()._shadowMaps2D[index] = Handle<Texture>::Make(Texture::Type::Tex2D, SHADOW_TEX_FORMAT, Texture::Params {
                .width = SPOT_LIGHT_SHADOW_RESOLUTION, .height = SPOT_LIGHT_SHADOW_RESOLUTION,
                .settings = { .mipmaps = false, .tiling = Texture::Tiling::Clamp }
            });

        return *Instance()._shadowMaps2D[index];
    }

    throw std::runtime_error("LightingSystem Invalid light type");
}

void LightingSystem::SetShadowMatrix(LightComponent::Type lightType, int index, glm::mat4 matrix)
{
    if (lightType == LightComponent::Type::Direct || lightType == LightComponent::Type::Spot)
        Instance()._shadowMatrices[index] = matrix;
    else if (lightType == LightComponent::Type::Point)
        Instance()._shadowMatrices[MAX_2D_SHADOWS + index] = matrix;
    else
        throw std::runtime_error("LightingSystem Attempted to set shadow matrix for unsupported light");
}

void LightingSystem::BindShadowMaps(const Shader& shader)
{
    for (int i = 0; i < MAX_2D_SHADOWS; ++i)
    {
        auto textureUnit = shader.GetTextureUnit(std::format("_ShadowMaps2D[{}]", i));
        auto tex = Instance()._shadowMaps2D[i].Access();
        if (textureUnit != -1 && tex != nullptr)
            Graphics::Bind(*tex, textureUnit);
    }

    for (int i = 0; i < MAX_CUBE_SHADOWS; ++i)
    {
        auto textureUnit = shader.GetTextureUnit(std::format("_ShadowMapsCube[{}]", i));
        auto tex = Instance()._shadowMapsCube[i].Access();
        if (textureUnit != -1 && tex != nullptr)
            Graphics::Bind(*tex, textureUnit);
    }

    for (int i = 0; i < MAX_2D_SHADOWS + MAX_CUBE_SHADOWS; ++i)
    {
        auto location = shader.GetUniformLocation(std::format("_ShadowMatrices[{}]", i));
        if (location != -1)
            Shader::SetMat4(location, Instance()._shadowMatrices[i]);
    }
}