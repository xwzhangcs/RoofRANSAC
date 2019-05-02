#include "Layer.h"
#include "../EfficientRANSAC2D_NoGUI/SymmetryLineDetector.h"

Layer::Layer() {

}

void Layer::detectContours(){
	orig_image = QImage(image_name).convertToFormat(QImage::Format_Grayscale8);
	polygons.clear();
	shapes.clear();
	contours.clear();
	symmetry_lines.clear();

	if (orig_image.isNull()) return;

	cv::Mat_<uchar> mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	polygons = util::findContours(mat, 40, false, true, false);
}

void Layer::generateLayer(QString filename, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold){
	image_name = filename;
	detectContours();
	std::cout << "filename is " << filename.toUtf8().constData() << std::endl;
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
			cv::Rect bbox = cv::boundingRect(polygons[i].contour);
			line_min_points = line_min_points * 0.01 * polygons[i].contour.size();
			line_cluster_epsilon = line_cluster_epsilon * 0.01 * polygons[i].contour.size();
			line_min_length = line_min_length * 0.01 * sqrt(bbox.width * bbox.width + bbox.height * bbox.height);
			std::cout << "line_min_points is " << line_min_points << std::endl;
			std::cout << "line_cluster_epsilon is " << line_cluster_epsilon << std::endl;
			std::cout << "line_min_length is " << line_min_length << std::endl;
			shapes[i] = er.detect(polygons[i].contour, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, principal_orientations);
		}
	}

	// generate contours
	contours.resize(shapes.size());
	sparse_contours.resize(shapes.size());
	symmetry_lines.resize(shapes.size());

	// find sparse contours
	cv::Mat_<uchar> mat = cv::Mat(orig_image.height(), orig_image.width(), CV_8UC1, orig_image.bits(), orig_image.bytesPerLine()).clone();
	std::vector<util::Polygon> sparse_polygons = util::findContours(mat, 40, true, false, true);
	std::cout << "sparse_polygons.size() is " << sparse_polygons.size() << std::endl;
	std::cout << "polygons.size() is " << polygons.size() << std::endl;

	for (int i = 0; i < shapes.size(); i++) {
		//std::cout << "shape size is " << shapes[i].size() << std::endl;
		if (shapes[i].size() == 0)
			continue;
		std::sort(shapes[i].begin(), shapes[i].end());
		ContourGenerator::generate(polygons[i], shapes[i], contours[i], contour_max_error, contour_angle_threshold);
		if (contours[i].size() == 0)
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
			}
			else{
				symmetry_lines[i].clear();
				bvalid_symmetry_line = false;
			}
		}
	}
}
