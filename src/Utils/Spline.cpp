#include "Spline.hpp"

Spline::Spline(std::initializer_list<glm::vec3> points, bool loop)
    : _points(points), _loop(loop)
{}

glm::vec3 Spline::Evaluate(float t) const
{
    float bigT = t * (static_cast<float>(_points.size()) - 1.0f + (_loop ? 1.0f : 0.0f));
    int index1 = static_cast<int>(bigT);
    float smallT = bigT - floor(bigT);

    auto p0 = GetPointAt(index1 - 1);
    auto p1 = GetPointAt(index1);
    auto p2 = GetPointAt(index1 + 1);
    auto p3 = GetPointAt(index1 + 2);

    float t2 = smallT * smallT;
    float t3 = t2 * smallT;

    float a = -0.5f * t3 + t2 - 0.5f * smallT;
    float b = 1.5f * t3 - 2.5f * t2 + 1.0f;
    float c = -1.5f * t3 + 2.0f * t2 + 0.5f * smallT;
    float d = 0.5f * t3 - 0.5f * t2;

    return p0 * a + p1 * b + p2 * c + p3 * d;
}

const glm::vec3 & Spline::GetPointAt(int index) const {
    if (index < 0)
        return _loop ? _points[(index % _points.size() + _points.size()) % _points.size()] : _points.front();
    if (index >= _points.size())
        return _loop ? _points[index % _points.size()] : _points.back();

    return _points[index];
}
