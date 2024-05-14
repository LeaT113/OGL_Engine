#include "CollisionTerrain.hpp"
#include <algorithm>
#include <format>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

CollisionTerrain::CollisionTerrain(float size, std::vector<glm::vec3> vertices, const glm::mat4 &worldToModel)
    : _size(size), _vertices(std::move(vertices)), _worldToModel(worldToModel)
{
    std::ranges::sort(_vertices, [](const glm::vec3& v1, const glm::vec3& v2)
    {
        if (std::fabs(v1.z - v2.z) < 1e-3f)
            return v1.x > v2.x;

        return v1.z > v2.z;
    });
    _gridSize = sqrt(_vertices.size());
}

glm::vec3 CollisionTerrain::GroundAt(const glm::vec3 &worldPosition) const
{
    auto localPos = _worldToModel * glm::vec4(worldPosition, 1.0f);

    auto gridX = (0.5f - localPos.x / _size) * static_cast<float>(_gridSize - 1);
    auto gridZ = (0.5f - localPos.z / _size) * static_cast<float>(_gridSize - 1);

    int x0 = static_cast<int>(gridX);
    int z0 = static_cast<int>(gridZ);
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    // Ensure indices are within bounds
    x0 = std::clamp(x0, 0, static_cast<int>(_gridSize - 1));
    x1 = std::clamp(x1, 0, static_cast<int>(_gridSize - 1));
    z0 = std::clamp(z0, 0, static_cast<int>(_gridSize - 1));
    z1 = std::clamp(z1, 0, static_cast<int>(_gridSize - 1));

    // Calculate vertex indices
    int idx00 = z0 * _gridSize + x0;
    int idx10 = z0 * _gridSize + x1;
    int idx01 = z1 * _gridSize + x0;
    int idx11 = z1 * _gridSize + x1;

    // Get the heights at the four surrounding points
    auto h00 = _vertices[idx00];
    auto h10 = _vertices[idx10];
    auto h01 = _vertices[idx01];
    auto h11 = _vertices[idx11];

    // Interpolate heights
    float tx = gridX - x0;
    float tz = gridZ - z0;
    auto heightPos = (1 - tx) * ((1 - tz) * h00 + tz * h01) +
                   tx * ((1 - tz) * h10 + tz * h11);
    float height = heightPos.y;

    // Return the interpolated ground position in world coordinates
    glm::vec3 localGroundPos(localPos.x, height, localPos.z);
    return glm::vec3(glm::inverse(_worldToModel) * glm::vec4(localGroundPos, 1.0f));
}
