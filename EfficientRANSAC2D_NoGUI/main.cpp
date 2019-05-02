#include <iostream>
#include "CurveDetector.h"
#include "LineDetector.h"
#include "PrimitiveShape.h"
#include "OrientationEstimator.h"
#include "ContourGenerator.h"

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " <input image file> <output image file>" << std::endl;
		return -1;
	}

	// load image
	cv::Mat image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

	// find contours
	std::vector<util::Polygon> polygons = util::findContours(image, 40, false, true, false);

	// detect principal orientation
	float principal_orientation = OrientationEstimator::estimate(polygons);

	// use the principal orientation, +45, +90, +135 degrees as principal orientations
	std::vector<float> principal_orientations;
	for (int i = 0; i < 4; i++) {
		principal_orientations.push_back(principal_orientation + CV_PI * i / 4);
	}

	// detect circles and lines
	efficient_ransac::EfficientRANSAC er;
	std::vector<std::vector<std::pair<int, std::shared_ptr<efficient_ransac::PrimitiveShape>>>> shapes(polygons.size());
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() >= 100) {
			shapes[i] = er.detect(polygons[i].contour, 200000, 200, 0.02, 30, 90, 80, 400, 20000, 30, 5, 20, 30, 15, principal_orientations);
		}
	}

	// generate contours
	std::vector<std::vector<cv::Point2f>> contours(shapes.size());
	for (int i = 0; i < shapes.size(); i++) {
		std::sort(shapes[i].begin(), shapes[i].end());
		ContourGenerator::generate(polygons[i], shapes[i], contours[i], 20, 10);
	}

	if (contours.size() > 0) {
		// generate output image
		cv::Mat result(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
		for (auto& polygon : polygons) {
			std::vector<cv::Point> pol;
			for (auto& pt : polygon.contour) pol.push_back(pt);
			cv::polylines(result, pol, true, cv::Scalar(0, 0, 0), 1);
			for (auto& hole : polygon.holes) {
				std::vector<cv::Point> pol;
				for (auto& pt : hole) pol.push_back(pt);
				cv::polylines(result, pol, true, cv::Scalar(0, 0, 0), 1);
			}
		}
		std::vector<std::vector<cv::Point>> pols;
		for (auto& contour : contours) {
			std::vector<cv::Point> pol;
			for (auto& pt : contour) {
				pol.push_back(cv::Point(pt.x, pt.y));
			}
			pols.push_back(pol);
		}
		cv::fillPoly(result, pols, cv::Scalar(255, 0, 0));

		cv::imwrite(argv[2], result);
	}

	return 0;
}