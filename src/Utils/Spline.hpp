#ifndef SPLINE_H
#define SPLINE_H

class Spline
{
public:
    Spline(std::initializer_list<glm::vec3> points, bool loop);

    glm::vec3 Evaluate(float t) const;

private:
    const std::vector<glm::vec3> _points;
    bool _loop;

    const glm::vec3 &GetPointAt(int index) const;
};



#endif
