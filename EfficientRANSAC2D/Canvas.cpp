#include "Canvas.h"
#include <QPainter>
#include <iostream>
#include <QFileInfoList>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>
#include <QResizeEvent>
#include "../EfficientRANSAC2D_NoGUI/RightAngleSimplification.h"
#include "../EfficientRANSAC2D_NoGUI/ShapeFit.h"
#include "../EfficientRANSAC2D_NoGUI/ShapeFitLayer.h"
#include "../EfficientRANSAC2D_NoGUI/SymmetryLineDetector.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent) {
	ctrlPressed = false;
	shiftPressed = false;
}

void Canvas::loadImage(const QString& filename) {
	orig_image = QImage(filename).convertToFormat(QImage::Format_Grayscale8);

	image_scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
	image = orig_image.scaled(orig_image.width() * image_scale, orig_image.height() * image_scale);
	polygons.clear();
	shapes.clear();
	contours.clear();
	symmetry_lines.clear();
	symmetry_polygons.clear();
	symmetry_lines_signal.clear();
	update();
}

void Canvas::saveImage(const QString& filename) {
	grab().save(filename);
}

void Canvas::detectContours() {
	polygons.clear();
	shapes.clear();
	contours.clear();
	symmetry_lines.clear();
	symmetry_polygons.clear();
	symmetry_lines_signal.clear();

	if (orig_image.isNull()) return;

	cv::Mat_<uchar> mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	polygons = util::findContours(mat, 40, false, true, false);
}

void Canvas::detectCurves(int num_iterations, int min_points, float max_error_ratio_to_radius, float cluster_epsilon, float min_angle, float min_radius, float max_radius) {
	detectContours();

	// detect circles
	efficient_ransac::EfficientRANSAC er;
	shapes.resize(polygons.size());
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() >= 100) {
			shapes[i] = er.detectCircles(polygons[i].contour, num_iterations, min_points, max_error_ratio_to_radius, cluster_epsilon, min_angle, min_radius, max_radius);
		}
	}
}

void Canvas::detectLines(int num_iterations, int min_points, float max_error, float cluster_epsilon, float min_length, float angle_threshold) {
	detectContours();

	// detect principal orientation
	float principal_orientation = OrientationEstimator::estimate(polygons);

	// use the principal orientation, +45, +90, +135 degrees as principal orientations
	std::vector<float> principal_orientations;
	for (int i = 0; i < 4; i++) {
		principal_orientations.push_back(principal_orientation + CV_PI * i / 4);
	}

	// detect lines based on the principal orientations
	efficient_ransac::EfficientRANSAC er;
	shapes.resize(polygons.size());
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() >= 100) {
			shapes[i] = er.detectLines(polygons[i].contour, num_iterations, min_points, max_error, cluster_epsilon, min_length, angle_threshold, principal_orientations, {});
		}
	}
}

void Canvas::detectCurvesLines(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold) {
	detectContours();

	// detect principal orientation
	float principal_orientation = OrientationEstimator::estimate(polygons);

	// use the principal orientation, +45, +90, +135 degrees as principal orientations
	std::vector<float> principal_orientations;
	for (int i = 0; i < 4; i++) {
		principal_orientations.push_back(principal_orientation + CV_PI * i / 4);
	}

	// detect circles and lines
	efficient_ransac::EfficientRANSAC er;
	shapes.resize(polygons.size());
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() >= 100) {
			shapes[i] = er.detect(polygons[i].contour, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, principal_orientations);
		}
	}
}

void Canvas::generateContours(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLine, float iouThreahold, float symmetryWeight, bool bUseRaOpt, float raThreahold, float raWeight, bool bUseParallelOpt, float papallelThreahold, float parallelWeight, bool bUseAccuracyOpt, float accuracyWeight, bool bUseOneLayer, bool bUserLayers)
{
	// optimization for one polygon
	if (!bUseOneLayer && !bUserLayers){
		std::cout << "OPT for polygon" << std::endl;
		generateContoursPolygon(curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold, bUseSymmetryLine, iouThreahold, symmetryWeight, bUseRaOpt, raThreahold, raWeight, bUseParallelOpt, papallelThreahold, parallelWeight, bUseAccuracyOpt, accuracyWeight);
	}
		// optimization for all polyons in one layer
	if (bUseOneLayer && !bUserLayers)
	{
		std::cout << "OPT for one layer" << std::endl;
		generateContoursLayer(curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold, bUseSymmetryLine, iouThreahold, symmetryWeight, bUseRaOpt, raThreahold, raWeight, bUseParallelOpt, papallelThreahold, parallelWeight, bUseAccuracyOpt, accuracyWeight);
	}
}

void Canvas::generateContoursPolygon(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLine, float iouThreahold, float symmetryWeight, bool bUseRaOpt, float raThreahold, float raWeight, bool bUseParallelOpt, float papallelThreahold, float parallelWeight, bool bUseAccuracyOpt, float accuracyWeight){
	detectContours();

	// detect principal orientation
	float principal_orientation = OrientationEstimator::estimate(polygons);

	// use the principal orientation, +45, +90, +135 degrees as principal orientations
	std::vector<float> principal_orientations;
	for (int i = 0; i < 4; i++) {
		principal_orientations.push_back(principal_orientation + CV_PI * i / 4);
	}

	// detect circles and lines
	efficient_ransac::EfficientRANSAC er;
	shapes.resize(polygons.size());
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() >= 100) {
			shapes[i] = er.detect(polygons[i].contour, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, principal_orientations);
		}
	}

	// generate contours
	std::vector<std::vector<cv::Point2f>> contours_pre;
	contours.resize(shapes.size());
	contours_pre.resize(shapes.size());
	symmetry_lines.resize(shapes.size());
	symmetry_lines_signal.resize(shapes.size());
	for (int i = 0; i < shapes.size(); i++)
		symmetry_lines_signal[i] = false;
	// find sparse contours
	cv::Mat_<uchar> mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	std::vector<util::Polygon> sparse_polygons = util::findContours(mat, 40, true, false, true);
	sparse_polygons_test = util::findContours(mat, 40, true, false, true);
	std::cout << "sparse_polygons.size() is " << sparse_polygons.size() << std::endl;
	std::cout << "polygons.size() is " << polygons.size() << std::endl;
	for (int i = 0; i < shapes.size(); i++) {
		//std::cout << "shape size is " << shapes[i].size() << std::endl;
		if (shapes[i].size() == 0)
			continue;
		std::sort(shapes[i].begin(), shapes[i].end());
		//ContourGenerator::generate(polygons[i], shapes[i], contours[i], contour_max_error, contour_angle_threshold);
		ContourGenerator::generate(polygons[i], shapes[i], contours_pre[i], contour_max_error, contour_angle_threshold);
		if (contours_pre[i].size() == 0)
			continue;
		// shapefit
		std::vector<cv::Point2f> sparse_contour;
		if (polygons.size() == sparse_polygons.size())
			sparse_contour = util::simplifyContour(sparse_polygons[i].contour, 2);
		else{
			// find corresponding polygon
			float min_x = std::numeric_limits<float>::max();
			int index = 0;
			for (int j = 0; j < sparse_polygons.size(); j++)
			{
				float distance = util::calculatePoLIS(sparse_polygons[j].contour, polygons[i].contour);
				if (distance < min_x){
					min_x = distance;
					index = j;
				}
			}
			std::cout << i << " is corresponding to " << index << std::endl;
			sparse_contour = util::simplifyContour(sparse_polygons[index].contour, 2);
		}

		// symmetry line
		bool bvalid_symmetry_line = false;
		if (bUseSymmetryLine){
			std::cout << "iouThreahold is " << iouThreahold << std::endl;
			symmetry_lines[i] = SymmetryLineDetector::fitSymmetryLine(sparse_contour);
			std::vector<cv::Point2f> symmetry_polygon;
			for (int j = 0; j < sparse_contour.size(); j++) {
				symmetry_polygon.push_back(util::mirrorPoint(symmetry_lines[i][0], symmetry_lines[i][1], sparse_contour[j]));
			}

			// calculate IOU between mirror polygon and original polygon
			float similarity_iou = util::calculateIOU(sparse_contour, symmetry_polygon);
			if (similarity_iou >= iouThreahold * 0.01){
				std::cout << i << " is here" << std::endl;
				bvalid_symmetry_line = true;
				symmetry_lines_signal[i] = true;
			}
			else{
				bvalid_symmetry_line = false;
				symmetry_lines_signal[i] = false;
			}
		}
		contours[i] = ShapeFit::fit(sparse_contour, contours_pre[i], bUseRaOpt, raThreahold, raWeight, bUseParallelOpt, papallelThreahold, parallelWeight, bvalid_symmetry_line, symmetry_lines[i], symmetryWeight, bUseAccuracyOpt, accuracyWeight);
		////////// DEBUG //////////
		// calculate IOU
		std::cout << "Polygon " << i << std::endl;
		if (util::isSimple(polygons[i].contour) && util::isSimple(contours[i]))
			std::cout << "IOU = " << util::calculateIOU(polygons[i].contour, contours[i]) << std::endl;
		else
			std::cout << "IOU = " << util::calculateIOUbyImage(polygons[i].contour, contours[i], 1000) << std::endl;
		std::cout << "#vertices = " << contours[i].size() << std::endl;
	}
}

void Canvas::generateContoursLayer(int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold, float symmetryWeight, bool bUseRaOpt, float raThreahold, float raWeight, bool bUseParallelOpt, float papallelThreahold, float parallelWeight, bool bUseAccuracyOpt, float accuracyWeight){
	detectContours();

	// detect principal orientation
	float principal_orientation = OrientationEstimator::estimate(polygons);

	// use the principal orientation, +45, +90, +135 degrees as principal orientations
	std::vector<float> principal_orientations;
	for (int i = 0; i < 4; i++) {
		principal_orientations.push_back(principal_orientation + CV_PI * i / 4);
	}

	// detect circles and lines
	efficient_ransac::EfficientRANSAC er;
	shapes.resize(polygons.size());
	for (int i = 0; i < polygons.size(); i++) {
		if (polygons[i].contour.size() >= 100) {
			shapes[i] = er.detect(polygons[i].contour, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, principal_orientations);
		}
	}

	// generate contours
	std::vector<std::vector<cv::Point2f>> contours_pre;
	contours.resize(shapes.size());
	contours_pre.resize(shapes.size());
	std::vector<std::vector<cv::Point2f>> sparse_contours;
	sparse_contours.resize(shapes.size());
	symmetry_lines.resize(shapes.size());
	symmetry_lines_signal.resize(shapes.size());
	for (int i = 0; i < shapes.size(); i++)
		symmetry_lines_signal[i] = false;
	// find sparse contours
	cv::Mat_<uchar> mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	std::vector<util::Polygon> sparse_polygons = util::findContours(mat, 40, true, false, true);
	sparse_polygons_test = util::findContours(mat, 40, true, false, true);
	std::cout << "sparse_polygons.size() is " << sparse_polygons.size() << std::endl;
	std::cout << "polygons.size() is " << polygons.size() << std::endl;

	for (int i = 0; i < shapes.size(); i++) {
		//std::cout << "shape size is " << shapes[i].size() << std::endl;
		if (shapes[i].size() == 0)
			continue;
		std::sort(shapes[i].begin(), shapes[i].end());
		//ContourGenerator::generate(polygons[i], shapes[i], contours[i], contour_max_error, contour_angle_threshold);
		ContourGenerator::generate(polygons[i], shapes[i], contours_pre[i], contour_max_error, contour_angle_threshold);
		if (contours_pre[i].size() == 0)
			continue;

		// shapefit
		if (polygons.size() == sparse_polygons.size())
			sparse_contours[i] = util::simplifyContour(sparse_polygons[i].contour, 2);
		else{
			// find corresponding polygon
			float min_x = std::numeric_limits<float>::max();
			int index = 0;
			for (int j = 0; j < sparse_polygons.size(); j++)
			{
				float distance = util::calculatePoLIS(sparse_polygons[j].contour, polygons[i].contour);
				if (distance < min_x){
					min_x = distance;
					index = j;
				}
			}
			std::cout << i << " is corresponding to " << index << std::endl;
			sparse_contours[i] = util::simplifyContour(sparse_polygons[index].contour, 2);
		}

		// symmetry line
		bool bvalid_symmetry_line = false;
		if (bUseSymmetryLineOpt){

			std::vector<cv::Point2f> computed_symmetry_line = SymmetryLineDetector::fitSymmetryLine(sparse_contours[i]);
			std::vector<cv::Point2f> symmetry_polygon;
			for (int j = 0; j < sparse_contours[i].size(); j++) {
				symmetry_polygon.push_back(util::mirrorPoint(computed_symmetry_line[0], computed_symmetry_line[1], sparse_contours[i][j]));
			}

			// calculate IOU between mirror polygon and original polygon
			float similarity_iou = util::calculateIOU(sparse_contours[i], symmetry_polygon);
			if (similarity_iou >= iouThreahold * 0.01){
				std::cout << i << " is here" << std::endl;
				symmetry_lines[i] = computed_symmetry_line;
				bvalid_symmetry_line = true;
				symmetry_lines_signal[i] = true;
			}
			else{
				symmetry_lines[i].clear();
				bvalid_symmetry_line = false;
				symmetry_lines_signal[i] = false;
			}
		}
	}
	// if all contours sizes are 0
	bool bValidLayer = false;
	for (int i = 0; i < contours_pre.size(); i++){
		if (contours_pre[i].size() != 0){
			bValidLayer = true;
			break;
		}
	}
	if (!bValidLayer)
		return;
	// Here we have all original polygons, all initial points and all symmetry lines
	// We call optimization below
	contours = ShapeFitLayer::fit(sparse_contours, contours_pre, bUseRaOpt, raThreahold, raWeight, bUseParallelOpt, papallelThreahold, parallelWeight, bUseSymmetryLineOpt, symmetry_lines, symmetryWeight, bUseAccuracyOpt, accuracyWeight);
	for (int i = 0; i < contours.size(); i++){
		////////// DEBUG //////////
		// calculate IOU
		if (contours[i].size() != 0){
			std::cout << "Polygon " << i << std::endl;
			std::cout << "contour size is " << contours[i].size() << std::endl;
			if (util::isSimple(polygons[i].contour) && util::isSimple(contours[i]))
				std::cout << "IOU = " << util::calculateIOU(polygons[i].contour, contours[i]) << std::endl;
			else
				std::cout << "IOU = " << util::calculateIOUbyImage(polygons[i].contour, contours[i], 1000) << std::endl;
			std::cout << "#vertices = " << contours[i].size() << std::endl;
			std::cout << "-----------------------" << std::endl;
		}
	}
}

void Canvas::keyPressEvent(QKeyEvent* e) {
	ctrlPressed = false;
	shiftPressed = false;

	if (e->modifiers() & Qt::ControlModifier) {
		ctrlPressed = true;
	}
	if (e->modifiers() & Qt::ShiftModifier) {
		shiftPressed = true;
	}

	switch (e->key()) {
	case Qt::Key_Space:
		break;
	}

	update();
}

void Canvas::keyReleaseEvent(QKeyEvent* e) {
	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = false;
		break;
	case Qt::Key_Shift:
		shiftPressed = false;
		break;
	default:
		break;
	}
}

void Canvas::paintEvent(QPaintEvent *event) {
	if (!image.isNull()) {
		QPainter painter(this);

		if (polygons.size() == 0) {
			painter.drawImage(0, 0, image);
		}
		else if (contours.size() == 0) {
			painter.fillRect(0, 0, width(), height(), QColor(255, 255, 255));

			painter.setPen(QPen(QColor(0, 0, 0), 1));
			for (auto& polygon : polygons) {
				QPolygon pgon;
				for (auto& p : polygon.contour) {
					pgon.push_back(QPoint(p.x * image_scale, p.y * image_scale));
				}
				painter.drawPolygon(pgon);
				for (auto& hole : polygon.holes) {
					QPolygon pgon;
					for (auto& p : hole) {
						pgon.push_back(QPoint(p.x * image_scale, p.y * image_scale));
					}
					painter.drawPolygon(pgon);
				}
			}

			for (auto& list : shapes) {
				for (auto& shape : list) {
					if (efficient_ransac::Circle* circle = dynamic_cast<efficient_ransac::Circle*>(shape.second.get())) {
						painter.setPen(QPen(QColor(255, 192, 192), 1));
						for (int i = 0; i < circle->points().size(); i++) {
							painter.drawRect(circle->points()[i].x * image_scale - 1, circle->points()[i].y * image_scale - 1, 2, 2);
						}

						painter.setPen(QPen(QColor(255, 0, 0), 3));
						float start_angle = std::min(circle->startAngle(), circle->endAngle());
						painter.drawArc((circle->center().x - circle->radius()) * image_scale, (circle->center().y - circle->radius()) * image_scale, circle->radius() * 2 * image_scale, circle->radius() * 2 * image_scale, -start_angle / CV_PI * 180 * 16, -circle->angleRange() / CV_PI * 180 * 16);
					}
					else if (efficient_ransac::Line* line = dynamic_cast<efficient_ransac::Line*>(shape.second.get())) {
						painter.setPen(QPen(QColor(192, 192, 255), 1));
						for (int i = 0; i < line->points().size(); i++) {
							painter.drawRect(line->points()[i].x * image_scale - 1, line->points()[i].y * image_scale - 1, 2, 2);
						}

						if (line->isRightAngle()) {
							painter.setPen(QPen(QColor(0, 192, 0), 3));
						}
						else {
							painter.setPen(QPen(QColor(0, 0, 255), 3));
						}
						cv::Point2f p1 = line->startPoint();
						cv::Point2f p2 = line->endPoint();
						painter.drawLine(p1.x * image_scale, p1.y * image_scale, p2.x * image_scale, p2.y * image_scale);
					}
				}
			}
		}
		else {
			painter.fillRect(0, 0, width(), height(), QColor(255, 255, 255));

			painter.setPen(QPen(QColor(0, 0, 0), 1));
			for (auto& polygon : polygons) {
				QPolygon pgon;
				for (auto& p : polygon.contour) {
					pgon.push_back(QPoint(p.x * image_scale, p.y * image_scale));
				}
				painter.drawPolygon(pgon);
				for (auto& hole : polygon.holes) {
					QPolygon pgon;
					for (auto& p : hole) {
						pgon.push_back(QPoint(p.x * image_scale, p.y * image_scale));
					}
					painter.drawPolygon(pgon);
				}
			}

			for (auto& contour : contours) {
				painter.setPen(QPen(QColor(0, 0, 255), 3));
				QPolygon pol;
				for (int i = 0; i < contour.size(); i++) {
					pol.push_back(QPoint(contour[i].x * image_scale, contour[i].y * image_scale));
				}
				painter.drawPolygon(pol);
			}

			//// test sparse polygon
			//{
			//	painter.setPen(QPen(QColor(255, 0, 0), 3));
			//	QPolygon pol;
			//	for (int i = 0; i < sparse_polygons_test[7].contour.size(); i++) {
			//		pol.push_back(QPoint(sparse_polygons_test[7].contour[i].x * image_scale, sparse_polygons_test[7].contour[i].y * image_scale));
			//	}
			//	painter.drawPolygon(pol);
			//}

			//// test polygon
			//{
			//	painter.setPen(QPen(QColor(0, 255, 0), 3));
			//	QPolygon pol;
			//	for (int i = 0; i < polygons[5].contour.size(); i++) {
			//		pol.push_back(QPoint(polygons[5].contour[i].x * image_scale, polygons[5].contour[i].y * image_scale));
			//	}
			//	painter.drawPolygon(pol);
			//}
		}
		// draw symmetry line
		if (symmetry_lines.size() != 0){
			for (int i = 0; i < symmetry_lines.size(); i++){
				if (symmetry_lines[i].size() == 0 || !symmetry_lines_signal[i]) continue;
				//std::cout << "paint symmetry line" << std::endl;
				painter.setPen(QPen(QColor(255, 0, 0), 3));
				cv::Point2f p1 = symmetry_lines[i][0];
				cv::Point2f p2 = symmetry_lines[i][1];
				painter.drawLine(p1.x * image_scale, p1.y * image_scale, p2.x * image_scale, p2.y * image_scale);
			}
			// draw symmetry polygons
			for (int i = 0; i < symmetry_polygons.size(); i++){
				painter.setPen(QPen(QColor(0, 0, 255), 3));
				if (symmetry_polygons[i].size() == 0 || !symmetry_lines_signal[i]) continue;
				QPolygon pol;
				for (int j = 0; j < symmetry_polygons[i].size(); j++) {
					pol.push_back(QPoint(symmetry_polygons[i][j].x * image_scale, symmetry_polygons[i][j].y * image_scale));
				}
				painter.drawPolygon(pol);
			}
		}
	}
}

void Canvas::mousePressEvent(QMouseEvent* e) {
	update();
}

void Canvas::resizeEvent(QResizeEvent *e) {
	if (!orig_image.isNull()) {
		image_scale = std::min((float)width() / orig_image.width(), (float)height() / orig_image.height());
		image = orig_image.scaled(orig_image.width() * image_scale, orig_image.height() * image_scale);
	}
}

