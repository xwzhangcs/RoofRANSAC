#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
#include "Util.h"
//#include "../util/BuildingLayer.h"
//#include "../util/ContourUtils.h"


class RightAngleSimplification {
protected:
	RightAngleSimplification() {}

public:
	static util::Polygon simplify(const util::Polygon& polygon, int resolution, float orientation, float min_hole_ratio, bool optimization);

private:
	static std::tuple<float, int, int> simplifyContour(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, int resolution, float orientation, bool optimization);
	static double simplifyContour(const std::vector<cv::Point2f>& contour, std::vector<cv::Point2f>& result, int resolution, float angle, int dx, int dy, bool refine, bool vertex_refinement);

	static double optimizeVertices(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour);
	static std::vector<cv::Point> proposedContour(const std::vector<cv::Point>& contour, std::map<int, int>& x_map, std::map<int, int>& y_map);
	static double optimizeBBox(const std::vector<cv::Point>& contour, std::vector<cv::Point>& simplified_contour);
	static std::vector<cv::Point> proposedBBox(const std::vector<cv::Point>& contour, int x1, int x2, int y1, int y2, int new_x1, int new_x2, int new_y1, int new_y2);

	static std::vector<util::Polygon> findContours(const cv::Mat_<uchar>& img);
	static void refineContour(std::vector<cv::Point2f>& polygon);
};

