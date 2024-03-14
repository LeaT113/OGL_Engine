#ifndef LIGHTINGSYSTEM_HPP
#define LIGHTINGSYSTEM_HPP
#include <memory>
#include <vector>

#include "../Components/LightComponent.hpp"
#include "../Core/HybridSingleton.hpp"
#include "../OGL/UniformBuffer.hpp"
#include "../OGL/ShaderStructs.hpp"


class LightingSystem : HybridSingleton<LightingSystem>
{
public:
    LightingSystem();

    static void RegisterLight(LightComponent *light);

    static void UpdateLights();

private:
    static constexpr int MAX_POINT_LIGHTS = 4;
    static constexpr int MAX_SPOT_LIGHTS = 4;

    static constexpr int AMBIENT_LIGHT_DATA_OFFSET = 0;
    static constexpr int DIRECT_LIGHT_DATA_OFFSET = AMBIENT_LIGHT_DATA_OFFSET + sizeof(AmbientLight);
    static constexpr int POINT_LIGHT_DATA_OFFSET = DIRECT_LIGHT_DATA_OFFSET + sizeof(DirectLight);
    static constexpr int SPOT_LIGHT_DATA_OFFSET = POINT_LIGHT_DATA_OFFSET + sizeof(PointLight) * MAX_POINT_LIGHTS;
    static constexpr int DATA_LENGTH = SPOT_LIGHT_DATA_OFFSET + sizeof(SpotLight) * MAX_SPOT_LIGHTS;


    std::vector<LightComponent*> _lights;
    uint8_t _data[DATA_LENGTH];
    std::unique_ptr<UniformBuffer> _lightsUniformBuffer;
};



#endif
