#include "RenderSystem.hpp"

#include <algorithm>
#include <glm/gtx/norm.hpp>

#include "LightingSystem.hpp"
#include "ResourceDatabase.hpp"
#include "TimeKeeper.hpp"
#include "../Scene/Entity.hpp"
#include "../OGL/Graphics.hpp"
#include "../OGL/UniformBuffer.hpp"


RenderSystem::RenderSystem()
    : _renderBuffer(0, 0, { FrameBuffer::RenderFormat::RGBA16F, FrameBuffer::RenderFormat::Depth24Stencil8 }),
    _opaqueBuffer(0, 0, { FrameBuffer::RenderFormat::RGBA16F, FrameBuffer::RenderFormat::Depth24Stencil8 }),
    _shadowBuffer(0, 0, { FrameBuffer::RenderFormat::Depth16 }),
    _postprocessMaterial(Handle<Material>::Empty())
{}

void RenderSystem::SetRenderSize(int width, int height)
{
    _width = width;
    _height = height;

    _renderBuffer.Resize(width, height);
    _opaqueBuffer.Resize(width, height);
    auto shadowRes = std::max(LightingSystem::DIRECT_LIGHT_SHADOW_RESOLUTION, std::max(LightingSystem::POINT_LIGHT_SHADOW_RESOLUTION, LightingSystem::SPOT_LIGHT_SHADOW_RESOLUTION));
    _shadowBuffer.Resize(shadowRes, shadowRes);

	if(!_camera)
		return;
	_camera->SetWindowDimensions(width, height);
}

void RenderSystem::SetRenderCamera(CameraComponent *camera)
{
	_camera = camera;
    _camera->SetWindowDimensions(_width, _height);
}

void RenderSystem::RegisterRenderer(RendererComponent *renderer)
{
    if (renderer->GetMaterial().GetShader().IsOpaque())
	    Instance()._opaqueRenderers.push_back(renderer);
    else
        Instance()._transparentRenderers.push_back(renderer);
}

void RenderSystem::Render()
{
    // Set core uniforms
    if (Instance()._coreUniformBuffer == nullptr)
    {
        Instance()._coreUniformBuffer = std::make_unique<UniformBuffer>(sizeof(float) + sizeof(glm::vec2)*2);
        Graphics::Bind(*Instance()._coreUniformBuffer, 1);
    }
    glm::vec2 screenSize(Instance()._width, Instance()._height);
    Instance()._coreUniformBuffer->SetData(0, sizeof(glm::vec2), &screenSize);
    auto cameraNearFar = Instance()._camera->GetNearFar();
    Instance()._coreUniformBuffer->SetData(sizeof(glm::vec2), sizeof(glm::vec2), &cameraNearFar);
    auto time = static_cast<float>(TimeKeeper::TimeSinceStartup());
    Instance()._coreUniformBuffer->SetData(sizeof(glm::vec2) * 2, sizeof(float), &time);

    // Shadows
    if (Instance()._shadowShader == nullptr)
    {
        Instance()._shadowShader = ResourceDatabase::GetShader("ShadowShader.glsl");
        Instance()._pointShadowShader = ResourceDatabase::GetShader("PointShadowShader.glsl");
    }

    auto& shadowBuffer = Instance()._shadowBuffer;
    Graphics::Bind(shadowBuffer);
    Entity shadowCameraEnt("ShadowCamera");
    shadowCameraEnt.AddComponent<TransformComponent>()
                    .AddComponent<CameraComponent>(CameraComponent::ProjectionType::Perspective, 90);
    CameraComponent& shadowCamera = *shadowCameraEnt.GetComponent<CameraComponent>();
    auto shadowCasters = LightingSystem::GetShadowCasters();

    int shadow2dCount = 0;
    int shadowCubeCount = 0;
    for (auto light : shadowCasters)
    {
        shadowBuffer.Clear();

        if (light->GetType() == LightComponent::Type::Direct)
        {
            throw std::runtime_error("Direct light shadows not implemented");
        }
        else if (light->GetType() == LightComponent::Type::Point)
        {
            if (shadowCubeCount >= LightingSystem::MAX_CUBE_SHADOWS)
            {
                light->SetShadowIndex(-1);
                continue;
            }

            shadowCamera.SetWindowDimensions(LightingSystem::POINT_LIGHT_SHADOW_RESOLUTION, LightingSystem::POINT_LIGHT_SHADOW_RESOLUTION);
            glViewport(0, 0, LightingSystem::POINT_LIGHT_SHADOW_RESOLUTION, LightingSystem::POINT_LIGHT_SHADOW_RESOLUTION);
            shadowCamera.GetTransform()->Position(light->GetTransform()->Position());
            shadowCamera.SetFOV(90);

            auto &shadowTexture = LightingSystem::GetShadowTexture(light->GetType(), shadowCubeCount);

            // Render cubemap
            static std::vector<std::pair<glm::vec3, glm::vec3>> cubemapDirections = {
                {glm::vec3(1, 0, 0), glm::vec3(0, -1, 0)},
                {glm::vec3(-1, 0, 0), glm::vec3(0, -1, 0)},
                {glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)},
                {glm::vec3(0, -1, 0), glm::vec3(0, 0, -1)},
                {glm::vec3(0, 0, 1), glm::vec3(0, -1, 0)},
                {glm::vec3(0, 0, -1), glm::vec3(0, -1, 0)}
            };
            for (int i = 0; i < 6; i++)
            {
                shadowBuffer.Clear();

                shadowCamera.GetTransform()->LookAt(cubemapDirections[i].first, cubemapDirections[i].second);
                for (auto renderer : Instance()._opaqueRenderers)
                {
                    auto name = renderer->GetMaterial().Name();
                    if (renderer->GetMaterial().Name() == "SkyboxMat") // TODO Temp
                        continue;
                    Graphics::RenderWithShader(*renderer, shadowCamera, *Instance()._pointShadowShader);
                }

                shadowTexture.Copy(shadowBuffer, i);
            }

            // Pass metadata
            shadowCamera.GetTransform()->Rotation(glm::quat(glm::vec3(0)));
            LightingSystem::SetShadowMatrix(light->GetType(), shadowCubeCount, shadowCamera.View());
            light->SetShadowIndex(shadowCubeCount++);
        }
        if (light->GetType() == LightComponent::Type::Spot)
        {
            if (shadow2dCount >= LightingSystem::MAX_2D_SHADOWS)
            {
                light->SetShadowIndex(-1);
                continue;
            }

            shadowCamera.SetWindowDimensions(LightingSystem::SPOT_LIGHT_SHADOW_RESOLUTION, LightingSystem::SPOT_LIGHT_SHADOW_RESOLUTION);
            glViewport(0, 0, LightingSystem::SPOT_LIGHT_SHADOW_RESOLUTION, LightingSystem::SPOT_LIGHT_SHADOW_RESOLUTION);
            shadowCamera.GetTransform()->AlignWith(*light->GetTransform());
            shadowCamera.SetFOV(light->GetSpotAngles().second);

            // Render depth
            shadowBuffer.Clear();
            for (auto renderer : Instance()._opaqueRenderers)
            {
                auto name = renderer->GetMaterial().Name();
                if (renderer->GetMaterial().Name() == "SkyboxMat") // TODO Temp
                    continue;
                Graphics::RenderWithShader(*renderer, shadowCamera, *Instance()._shadowShader);
            }

            // Pass to lighting system
            auto &shadowTex = LightingSystem::GetShadowTexture(light->GetType(), shadow2dCount);
            shadowTex.Copy(shadowBuffer);
            LightingSystem::SetShadowMatrix(light->GetType(), shadow2dCount, shadowCamera.Projection() * shadowCamera.View());
            light->SetShadowIndex(shadow2dCount++);
        }
    }

    // Opaque pass
    auto& renderBuffer = Instance()._renderBuffer;
    auto& opaqueBuffer = Instance()._opaqueBuffer;
    Graphics::Bind(renderBuffer);
    renderBuffer.Clear();
    glViewport(0, 0, Instance()._width, Instance()._height);
    for (auto renderer : Instance()._opaqueRenderers)
    {
        Graphics::Render(*renderer, *Instance()._camera);
    }

    // Copy opaque buffer
    opaqueBuffer.Clear();
    Graphics::Blit(renderBuffer, opaqueBuffer);

    // Transparent pass
    Graphics::Bind(renderBuffer);
    std::ranges::sort(Instance()._transparentRenderers, [](RendererComponent* a, RendererComponent* b)
    {
        auto dist1 = length2(a->GetTransform()->Position() - Instance()._camera->GetTransform()->Position());
        auto dist2 = length2(b->GetTransform()->Position() - Instance()._camera->GetTransform()->Position());
        return dist1 > dist2;
    });
    for (auto renderer : Instance()._transparentRenderers)
    {
        auto& shader = renderer->GetMaterial().GetShader();
        shader.SetTextures(opaqueBuffer.GetColorTexture(), opaqueBuffer.GetDepthTexture(), ResourceDatabase::GetTexture("Skybox/Night")->GetBindID());

        Graphics::Render(*renderer, *Instance()._camera);
    }

    // Post process
    if (Instance()._postprocessMaterial.Access() == nullptr)
        Instance()._postprocessMaterial = Handle<Material>::Make(*ResourceDatabase::GetShader("PostprocessShader.glsl"), "PostprocessMaterial");
    Graphics::Blit(renderBuffer, FrameBuffer::None, *Instance()._postprocessMaterial);
}