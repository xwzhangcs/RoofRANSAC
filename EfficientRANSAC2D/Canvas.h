#ifndef CANVAS_H
#define CANVAS_H

#include <vector>
#include <QWidget>
#include <QKeyEvent>
#include "../EfficientRANSAC2D_NoGUI/CurveDetector.h"
#include "../EfficientRANSAC2D_NoGUI/LineDetector.h"
#include "../EfficientRANSAC2D_NoGUI/OrientationEstimator.h"
#include "../EfficientRANSAC2D_NoGUI/PrimitiveShape.h"
#include "../EfficientRANSAC2D_NoGUI/ContourGenerator.h"
#include "../EfficientRANSAC2D_NoGUI/EfficientRANSAC.h"

class Canvas : public QWidget {
private:
	QImage orig_image;
	QImage image;
	float image_scale;
	std::vector<util::Polygon> polygons;
	std::vector<std::vector<std::pair<int, std::shared_ptr<efficient_ransac::PrimitiveShape>>>> shapes;
	std::vector<std::vector<cv::Point2f>> contours;
	std::vector<std::vector<cv::Point2f>> symmetry_lines;
	std::vector<bool> symmetry_lines_signal;
	std::vector<std::vector<cv::Point2f>> symmetry_polygons;
	std::vector<util::Polygon> sparse_polygons_test;

	bool ctrlPressed;
	bool shiftPressed;
	
public:
	Canvas(QWidget *parent = NULL);

	void loadImage(const QString& filename);
	void saveImage(const QString& filename);
	void detectContours();
	void detectCurves(int num_iterations, int min_points, float max_error_ratio_to_radius, float cluster_epsilon, float min_angle, float min_radius, float max_radius);
	void detectLines(int num_iterations, int min_points, float max_error, float cluster_epsilon, float min_length, float angle_threshold);
	void detectCurvesLines(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold);
	void generateContours(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLine, float iouThreahold, float symmetryWeight, bool bUseRaOpt, float raThreahold, float raWeight, bool bUseParallelOpt, float papallelThreahold, float parallelWeight, bool bUseAccuracyOpt, float accuracyWeight, bool bUseOneLayer, bool bUserLayers);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

	//
	void generateContoursPolygon(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLine, float iouThreahold, float symmetryWeight, bool bUseRaOpt, float raThreahold, float raWeight, bool bUseParallelOpt, float papallelThreahold, float parallelWeight, bool bUseAccuracyOpt, float accuracyWeight);
	void generateContoursLayer(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLine, float iouThreahold, float symmetryWeight, bool bUseRaOpt, float raThreahold, float raWeight, bool bUseParallelOpt, float papallelThreahold, float parallelWeight, bool bUseAccuracyOpt, float accuracyWeight);
	//void generateContoursLayers();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent* e);
	void resizeEvent(QResizeEvent *e);
};

#endif // CANVAS_H
