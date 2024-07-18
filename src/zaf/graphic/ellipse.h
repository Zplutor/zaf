#pragma once 

#include <zaf/base/hash.h>
#include <zaf/graphic/point.h>
#include <zaf/object/equality.h>
#include <zaf/object/property_support.h>

namespace zaf {
    
class Ellipse : public Object {
public:
    ZAF_OBJECT;
    ZAF_DECLARE_EQUALITY;

public:
    Ellipse() : x_radius(0), y_radius(0) { }

    Ellipse(float x, float y, float x_radius, float y_radius) :
        position(x, y),
        x_radius(x_radius),
        y_radius(y_radius) {
    
    }

    Ellipse(const Point& position, float x_radius, float y_radius) : 
        position(position),
        x_radius(x_radius),
        y_radius(y_radius) {

    }

    void Inflate(float radius) {

        x_radius += radius;
        y_radius += radius;
    }

    void Inflate(float x_radius, float y_radius) {

        this->x_radius += x_radius;
        this->y_radius += y_radius;
    }

    void AddOffset(const Point& offset) {
        position.AddOffset(offset);
    }

    void SubtractOffset(const Point& offset) {
        position.SubtractOffset(offset);
    }

    D2D1_ELLIPSE ToD2D1ELLIPSE() const {

        D2D1_ELLIPSE ellipse = { 0 };
        ellipse.point = position.ToD2D1POINT2F();
        ellipse.radiusX = x_radius;
        ellipse.radiusY = y_radius;
        return ellipse;
    }

    std::wstring ToString() const override;

    const Point& Position() const {
        return position;
    }

    void SetPosition(const Point& position) {
        this->position = position;
    }

    float XRadius() const {
        return x_radius;
    }

    void SetXRadius(float x_radius) {
        this->x_radius = x_radius;
    }

    float YRadius() const {
        return y_radius;
    }

    void SetYRadius(float y_radius) {
        this->y_radius = y_radius;
    }

public:
    Point position;
    float x_radius;
    float y_radius;
};

ZAF_OBJECT_BEGIN(Ellipse);
ZAF_OBJECT_PROPERTY(Position);
ZAF_OBJECT_PROPERTY(XRadius);
ZAF_OBJECT_PROPERTY(YRadius);
ZAF_OBJECT_END;


bool operator==(const Ellipse& ellipse1, const Ellipse& ellipse2);
bool operator<(const Ellipse& ellipse1, const Ellipse& ellipse2);

ZAF_DEFINE_RELATION_OPERATORS(Ellipse);

}


namespace std {
template<>
struct hash<zaf::Ellipse> {
    std::size_t operator()(const zaf::Ellipse& ellipse) {
        return zaf::CalculateHash(ellipse.position, ellipse.x_radius, ellipse.y_radius);
    }
};
}