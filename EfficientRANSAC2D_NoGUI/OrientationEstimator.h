#pragma once

#include <vector>
#include "Util.h"

class OrientationEstimator {
public:
	OrientationEstimator() {}

public:
	static float estimate(const std::vector<util::Polygon>& polygons);
	static float estimate(const std::vector<cv::Point2f>& polygon);
};

