#ifndef LIGHTINGSYSTEM_HPP
#define LIGHTINGSYSTEM_HPP
#include "../Components/LightComponent.hpp"
#include "../Core/HybridSingleton.hpp"
#include "../OGL/UniformBuffer.hpp"
#include "../OGL/ShaderStructs.hpp"
#include "../Resources/Texture.hpp"


class Texture;
class Shader;

class LightingSystem : HybridSingleton<LightingSystem>
{
public:
    static constexpr int MAX_POINT_LIGHTS = 4;
    static constexpr int MAX_SPOT_LIGHTS = 4;
    static constexpr auto SHADOW_TEX_FORMAT = Texture::Format::Depth16;
    static constexpr int MAX_2D_SHADOWS = 2;
    static constexpr int MAX_CUBE_SHADOWS = 2;
    static constexpr int DIRECT_LIGHT_SHADOW_RESOLUTION = 4096;
    static constexpr int POINT_LIGHT_SHADOW_RESOLUTION = 256;
    static constexpr int SPOT_LIGHT_SHADOW_RESOLUTION = 1024;

    LightingSystem();

    static void RegisterLight(LightComponent *light);

    static void UpdateLights();

    static std::vector<LightComponent*> GetShadowCasters();
    static Texture& GetShadowTexture(LightComponent::Type lightType, int index);
    static void SetShadowMatrix(LightComponent::Type lightType, int index, glm::mat4 matrix);
    static void BindShadowMaps(const Shader& shader);

private:
    static constexpr int AMBIENT_LIGHT_DATA_OFFSET = 0;
    static constexpr int DIRECT_LIGHT_DATA_OFFSET = AMBIENT_LIGHT_DATA_OFFSET + sizeof(AmbientLight);
    static constexpr int POINT_LIGHT_DATA_OFFSET = DIRECT_LIGHT_DATA_OFFSET + sizeof(DirectLight);
    static constexpr int SPOT_LIGHT_DATA_OFFSET = POINT_LIGHT_DATA_OFFSET + sizeof(PointLight) * MAX_POINT_LIGHTS;
    static constexpr int DATA_LENGTH = SPOT_LIGHT_DATA_OFFSET + sizeof(SpotLight) * MAX_SPOT_LIGHTS;

    std::vector<LightComponent*> _lights;
    uint8_t _data[DATA_LENGTH];
    std::unique_ptr<UniformBuffer> _lightsUniformBuffer;

    std::array<glm::mat4, MAX_2D_SHADOWS + MAX_CUBE_SHADOWS> _shadowMatrices;
    std::array<Handle<Texture>, MAX_2D_SHADOWS> _shadowMaps2D;
    std::array<Handle<Texture>, MAX_CUBE_SHADOWS> _shadowMapsCube;
};



#endif
