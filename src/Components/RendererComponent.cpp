#include "RendererComponent.hpp"
#include "../Scene/Entity.hpp"
#include "../Systems/RenderSystem.hpp"
#include "../OGL/Graphics.hpp"
#include "../Systems/ResourceDatabase.hpp"


RendererComponent::RendererComponent(const Entity& owner, const Mesh* mesh, const Material* material)
	: Component(owner), _mesh(mesh), _material(material)
{
	RenderSystem::RegisterRenderer(this);
}

RendererComponent::RendererComponent(const Entity& owner) : Component(owner)
{}

const Mesh& RendererComponent::GetMesh() const
{
	return *_mesh;
}

const Material& RendererComponent::GetMaterial() const
{
	return *_material;
}

bool RendererComponent::IsInstanced() const
{
	return !_instancingTransforms.empty();
}

const std::vector<glm::mat4>& RendererComponent::GetInstancingTransforms() const
{
	return _instancingTransforms;
}

const VertexBuffer* RendererComponent::GetInstancingTransformsBuffer() const
{
	return _instancingTransformsBuffer.Access();
}

void RendererComponent::SetInstancingTransforms(std::vector<glm::mat4> transforms)
{
	// TODO Allow disabling of instancing
	_instancingTransforms = std::move(transforms);
	_instancingTransformsBuffer = Handle<VertexBuffer>::Make(sizeof(glm::mat4) * _instancingTransforms.size());
	_instancingTransformsBuffer->InsertAttribute(VertexAttribute::InstanceTransform, &_instancingTransforms[0], _instancingTransforms.size());

	Graphics::Bind(_mesh->GetVertexArray());
	Graphics::Bind(*_instancingTransformsBuffer);

	auto attribute = static_cast<unsigned int>(VertexAttribute::InstanceTransform);
	auto attributeInfo = GetVertexAttributeInfo(VertexAttribute::InstanceTransform);
	for (unsigned int i = 0; i < 4; i++)
	{
		glVertexAttribPointer(attribute + i, attributeInfo.size, attributeInfo.type, attributeInfo.normalized, attributeInfo.byteSize, (void *) (i * sizeof(glm::vec4)));
		glEnableVertexAttribArray(attribute + i);
		glVertexAttribDivisor(attribute + i, 1);
	}

	Graphics::Bind(VertexArray::None);
}
