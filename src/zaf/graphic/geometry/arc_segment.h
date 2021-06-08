#pragma once

#include <zaf/graphic/point.h>

namespace zaf {

enum class SweepDirection {
	CounterClockwise = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
	Clockwise = D2D1_SWEEP_DIRECTION_CLOCKWISE,
};


enum class ArcSize {
	Small = D2D1_ARC_SIZE_SMALL,
	Large = D2D1_ARC_SIZE_LARGE,
};


class ArcSegment {
public:
	Point end_point;
	float x_radius{};
	float y_radius{};
	float rotation_angle{};
	SweepDirection sweep_direction{ SweepDirection::Clockwise };
	ArcSize arc_size{ D2D1_ARC_SIZE_SMALL };
};

}