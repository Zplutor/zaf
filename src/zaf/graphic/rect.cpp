#include <zaf/graphic/rect.h>
#include <algorithm>
#include <limits>
#include <zaf/serialization/data_node.h>
#include <zaf/serialization/properties.h>

#undef max
#undef min

namespace zaf {

void Rect::Intersect(const Rect& other) {

	float left = std::max(position.x, other.position.x);
	float top = std::max(position.y, other.position.y);
	float right = std::min(position.x + size.width, other.position.x + other.size.width);
	float bottom = std::min(position.y + size.height, other.position.y + other.size.height);

	if ((left < right) && (top < bottom)) {
		position.x = left;
		position.y = top;
		size.width = right - left;
		size.height = bottom - top;
	}
	else {
		position.x = 0;
		position.y = 0;
		size.width = 0;
		size.height = 0;
	}
}


void Rect::Subtract(const Rect& other) {

    Rect intersection = Rect::Intersect(*this, other);
    if (intersection == *this) {
        position.x = 0;
        position.y = 0;
        size.width = 0;
        size.height = 0;
        return;
    }

    if (intersection.size.width == size.width) {
        if (intersection.position.y == position.y) {
            position.y = intersection.position.y + intersection.size.height;
            size.height -= intersection.size.height;
        }
        else if (intersection.position.y + intersection.size.height == position.y + size.height) {
            size.height -= intersection.size.height;
        }
    }
    else if (intersection.size.height == size.height) {
        if (intersection.position.x == position.x) {
            position.x = intersection.position.x + intersection.size.width;
            size.width -= intersection.size.width;
        }
        else if (intersection.position.x + intersection.size.width == position.x + size.width) {
            size.width -= intersection.size.width;
        }
    }
}


bool Rect::IsInfinite() const {

    float max_value = std::numeric_limits<float>::infinity();
    float min_value = -max_value;

    return 
        position.x == min_value &&
        position.y == min_value && 
        size.width == max_value &&
        size.height == max_value;
}


void Rect::SerializeToDataNode(DataNode& data_node) const {

    data_node.AddChild(property::Position, position.Serialize());
    data_node.AddChild(property::Size, size.Serialize());
}


bool Rect::DeserializeFromDataNode(const DataNode& data_node) {

    data_node.EnumerateKeyedChildren([this](const std::wstring& key, const DataNode& data_node) {
    
        if (key == property::Position) {
            position.Deserialize(data_node);
        }
        else if (key == property::Size) {
            size.Deserialize(data_node);
        }
    });

    return true;
}


ZAF_DEFINE_TYPE_NAME(Rect);

const Rect Rect::Infinite(
    -std::numeric_limits<float>::infinity(),
    -std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::infinity(),
    std::numeric_limits<float>::infinity());

}