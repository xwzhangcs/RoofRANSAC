#include <vector>
#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include "../EfficientRANSAC2D_NoGUI/Layer.h"

class Regularizer{

public:
	Regularizer();
	void regularizerForLayer(QString fileName, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, QString config_file);
	void regularizerForLayers(const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, QString config_file);
	void createLayer(QString fileName, Layer & layer, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold);
	void createLayers(const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, std::vector<Layer>& layers, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold);
	void generateContoursLayer(Layer& input_layer, QString config_file);
	void generateContoursLayers(std::vector<Layer>& input_layers, QString config_file);
	QString saveImage(Layer & layer, int index, int level, QString output_path);
	void post_processing(Layer & layer, float angle_threshold);
	void regularizerMultiRunsForLayers(const QString output_path, const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, QString detect_file, QString config_file);
	void regularizerAll(const QString output_path, const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, QString detect_file, QString config_file);
	//void saveImages();
	void overlayImages(const std::vector<QString>& image_files, QString output_file);
};
