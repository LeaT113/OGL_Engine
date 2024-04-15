#include "LightingSystem.hpp"
#include "../OGL/Graphics.hpp"

LightingSystem::LightingSystem()
{
    //_lightsUniformBuffer = std::make_unique<UniformBuffer>(sizeof(_lightsStruct));
}

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
            case LightComponent::Ambient:
                *reinterpret_cast<AmbientLight*>(Instance()._data + AMBIENT_LIGHT_DATA_OFFSET) = light->GetAmbientLight();
                break;

            case LightComponent::Direct:
                *reinterpret_cast<DirectLight*>(Instance()._data + DIRECT_LIGHT_DATA_OFFSET) = light->GetDirectLight();
                break;

            case LightComponent::Point:
                if(pointLightIndex >= MAX_POINT_LIGHTS)
                    continue;
                *reinterpret_cast<PointLight*>(Instance()._data + POINT_LIGHT_DATA_OFFSET + sizeof(PointLight) * pointLightIndex++) = light->GetPointLight();
                break;

            case LightComponent::Spot:
                if(spotLightIndex >= MAX_SPOT_LIGHTS)
                    continue;
                *reinterpret_cast<SpotLight*>(Instance()._data + SPOT_LIGHT_DATA_OFFSET + sizeof(SpotLight) * spotLightIndex++) = light->GetSpotLight();
                break;
        }
    }

    Instance()._lightsUniformBuffer->SetData(0, sizeof(_data), &Instance()._data);
}
