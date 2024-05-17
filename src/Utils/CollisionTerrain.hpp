#ifndef COLLISIONTERRAIN_HPP
#define COLLISIONTERRAIN_HPP

class CollisionTerrain
{
public:
    CollisionTerrain(float size, std::vector<glm::vec3> vertices, const glm::mat4 &worldToModel);

    glm::vec3 GroundAt(const glm::vec3 &worldPosition) const;

private:
    float _size;
    std::vector<glm::vec3> _vertices;
    unsigned int _gridSize = 0;
    glm::mat4 _worldToModel;
};



#endif
