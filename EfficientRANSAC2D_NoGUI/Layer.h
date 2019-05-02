#pragma once

#include <vector>
#include <QWidget>
#include "Util.h"
#include "../EfficientRANSAC2D_NoGUI/CurveDetector.h"
#include "../EfficientRANSAC2D_NoGUI/LineDetector.h"
#include "../EfficientRANSAC2D_NoGUI/OrientationEstimator.h"
#include "../EfficientRANSAC2D_NoGUI/PrimitiveShape.h"
#include "../EfficientRANSAC2D_NoGUI/ContourGenerator.h"
#include "../EfficientRANSAC2D_NoGUI/EfficientRANSAC.h"

class Layer {
public:
	QString image_name;
	QImage orig_image;
	std::vector<util::Polygon> polygons;
	std::vector<std::vector<std::pair<int, std::shared_ptr<efficient_ransac::PrimitiveShape>>>> shapes;
	std::vector<std::vector<cv::Point2f>> contours;
	std::vector<std::vector<cv::Point2f>> sparse_contours;
	std::vector<std::vector<cv::Point2f>> symmetry_lines;
	float top_height;
	float bottom_height;
	std::vector<int> parents;
	std::vector<int> children;
	bool bUseRaOptValid;
	bool bUseParallelOptValid;
	bool bUseSymmetryOptValid;
	bool bUseAccuracyOptValid;
	bool bUsePointOptValid;
	bool bUseSegOptValid;

public:
	Layer();

	void detectContours();
	void generateLayer(QString filename, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold);
};

