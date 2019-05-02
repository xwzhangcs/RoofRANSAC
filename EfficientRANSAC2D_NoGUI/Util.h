#pragma once

#include <vector>
#include <QWidget>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>

namespace util {

	typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;

	class Polygon {
	public:
		std::vector<cv::Point2f> contour;
		std::vector<std::vector<cv::Point2f>> holes;

	public:
		Polygon();
	};

	std::vector<Polygon> findContours(const cv::Mat_<uchar>& image, int threshold, bool simplify, bool allow_diagonal, bool dilate);
	std::vector<cv::Point2f> removeRedundantPoint(const std::vector<cv::Point2f>& polygon);
	std::vector<cv::Point2f> simplifyContour(const std::vector<cv::Point2f>& polygon, float epsilon);
	cv::Rect boundingBox(const std::vector<cv::Point2f>& polygon);
	cv::Rect boundingBox(const std::vector<cv::Point>& polygon);
	void createImageFromContour(int width, int height, const std::vector<cv::Point>& contour, const cv::Point& offset, cv::Mat_<uchar>& result, bool erode = true);
	float calculateIOU(const cv::Mat_<uchar>& img, const cv::Mat_<uchar>& img2);
	float calculateIOU(const std::vector<cv::Point2f>& polygon1, const std::vector<cv::Point2f>& polygon2);
	float calculateIOUbyImage(const std::vector<cv::Point2f>& polygon1, const std::vector<cv::Point2f>& polygon2);
	float calculateIOUbyImage(const std::vector<cv::Point2f>& polygon1, const std::vector<cv::Point2f>& polygon2, int image_size);
	double calculatePoLIS(const std::vector<cv::Point2f>& polygon1, const std::vector<cv::Point2f>& polygon2);
	float area(const CGAL::Polygon_with_holes_2<Kernel>& pwh);
	float area(const std::vector<cv::Point2f>& polygon);
	bool isSimple(const std::vector<cv::Point2f>& polygon);
	bool isSimple(const std::vector<cv::Point>& polygon);
	void counterClockwise(std::vector<cv::Point2f>& polygon);
	void transform(Polygon& polygon, const cv::Mat_<float>& M);
	void transform(std::vector<cv::Point2f>& polygon, const cv::Mat_<float>& M);
	bool withinPolygon(const cv::Point2f& pt, const Polygon& polygon);
	bool withinPolygon(const cv::Point2f& pt, const std::vector<cv::Point2f>& polygon);
	bool withinPolygon(const std::vector<cv::Point2f>& inner_polygon, const std::vector<cv::Point2f>& outer_polygon);
	bool lineLineIntersection(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, const cv::Point2f& d, double *tab, double *tcd, bool segment_only, cv::Point2f& int_pt);

	// score
	float calculateScore(const std::vector<cv::Point2f>& polygon, const std::vector<cv::Point2f>& init_polygon, int angle_threshold);
	float calculateAllScore(const std::vector<cv::Point2f>& polygon, const std::vector<cv::Point2f>& init_polygon, int angle_threshold);
	float scoreFun(float angle, float threshold);
	float lineLineAngle(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, const cv::Point2f& d);

	// distance
	float distance(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, bool segment_only);
	float distance(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c);
	float distance(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c, const cv::Point2f& d);

	// compute mirror point
	cv::Point2f mirrorPoint(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c);

	// score for point snap
	float calculateScorePointOpt(const std::vector<cv::Point2f>& src_polygon, const std::vector<cv::Point2f>& init_src_polygon, const std::vector<std::vector<cv::Point2f>>& des_layer_polygons, const std::vector<std::vector<cv::Point2f>>& des_ini_layer_polygons, float dis_threshold);
	float calculateScoreSegOpt(const std::vector<cv::Point2f>& src_polygon, const std::vector<cv::Point2f>& init_src_polygon, const std::vector<std::vector<cv::Point2f>>& des_layer_polygons, const std::vector<std::vector<cv::Point2f>>& des_ini_layer_polygons, float dis_threshold, float angle_threshold);
	void write_log(QString filename, QString content);
}