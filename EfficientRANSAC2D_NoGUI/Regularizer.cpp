#include "Regularizer.h"
#include <QTextStream>
#include "../EfficientRANSAC2D_NoGUI/ShapeFit.h"
#include "../EfficientRANSAC2D_NoGUI/ShapeFitLayer.h"
#include "../EfficientRANSAC2D_NoGUI/ShapeFitLayers.h"
#include "../EfficientRANSAC2D_NoGUI/ShapeFitLayersInter.h"
#include "../EfficientRANSAC2D_NoGUI/ShapeFitLayersAll.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/document.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/writer.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/stringbuffer.h"

Regularizer::Regularizer(){

}

void Regularizer::regularizerForLayer(QString fileName, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, QString config_file){
	Layer input;
	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	bool bUseIntra = false;
	bool bUseSymmetryLineOpt = false;
	float iouThreahold = 0.0f;
	bUseIntra = doc["UseIntra"].GetBool();
	std::cout << "bUseIntra is " << bUseIntra << std::endl;
	if (bUseIntra){
		rapidjson::Value& algs = doc["IntraOpt"];
		rapidjson::Value& algs_subs = algs["Symmetry"];
		bUseSymmetryLineOpt = algs_subs["UseOpt"].GetBool();
		iouThreahold = algs_subs["IouThreshold"].GetFloat();
		file.close();
	}
	else{
		file.close();
	}
	createLayer(fileName, input, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold, bUseSymmetryLineOpt, iouThreahold);
	generateContoursLayer(input, config_file);
	// output contour points
	// before post_processing
	{
		std::cout << "------------before post_processing---------" << std::endl;
		for (int i = 0; i < input.contours[0].size(); i++){
			std::cout << "point "<< i << " "<< input.contours[0][i] << std::endl;
		}
	}
	// after post_processing
	{
		std::cout << "------------after post_processing---------" << std::endl;
		post_processing(input, 10);
		for (int i = 0; i < input.contours[0].size(); i++){
			std::cout << "point " << i << " " << input.contours[0][i] << std::endl;
		}
	}
	saveImage(input, 1, 1, "../test");
}

void Regularizer::regularizerForLayers(const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_infoint, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, QString config_file){
	std::vector<Layer> input_layers;
	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	bool bUseIntra = false;
	bool bUseSymmetryLineOpt = false;
	float iouThreahold = 0.0f;
	bUseIntra = doc["UseIntra"].GetBool();
	std::cout << "bUseIntra is " << bUseIntra << std::endl;
	if (bUseIntra){
		rapidjson::Value& algs = doc["IntraOpt"];
		rapidjson::Value& algs_subs = algs["Symmetry"];
		bUseSymmetryLineOpt = algs_subs["UseOpt"].GetBool();
		iouThreahold = algs_subs["IouThreshold"].GetFloat();
		file.close();
	}
	else{
		file.close();
	}
	createLayers(fileNameList, height_infoint, tree_info, input_layers, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold, bUseSymmetryLineOpt, iouThreahold);
	generateContoursLayers(input_layers, config_file);
	for (int i = 0; i < input_layers.size(); i++){
		saveImage(input_layers[i], i, 1, "../test");
		//std::cout << "layer " << i << " top height is " << input_layers[i].top_height << " bot height is " << input_layers[i].bottom_height << std::endl;
	}
	// verify point opt
	/*{
		//before opt
		std::cout << "----------before opt-----------" << std::endl;
		for (int i = 0; i < input_layers[0].contours_pre[0].size(); i++){
			for (int j = 0; j < input_layers[1].contours_pre[0].size(); j++){
				cv::Point2f src_init_p = input_layers[0].contours_pre[0][i];
				cv::Point2f des_init_p = input_layers[1].contours_pre[0][j];
				float dis_init = cv::norm(src_init_p - des_init_p);
				std::cout << "dis_init from " << i << " to " << j << " is " << dis_init << std::endl;
			}
		}
		//after opt
		std::cout << "----------after opt-----------" << std::endl;
		for (int i = 0; i < input_layers[0].contours_snap[0].size(); i++){
			for (int j = 0; j < input_layers[1].contours_snap[0].size(); j++){
				cv::Point2f src_init_p = input_layers[0].contours_snap[0][i];
				cv::Point2f des_init_p = input_layers[1].contours_snap[0][j];
				float dis_init = cv::norm(src_init_p - des_init_p);
				std::cout << "dis_init from " << i << " to " << j << " is " << dis_init << std::endl;
			}
		}
	}*/

	// verify seg opt
	/*{
		//before opt
		std::cout << "----------before opt-----------" << std::endl;
		int total_seg_src = input_layers[0].contours_pre[0].size();
		int total_seg_des = input_layers[1].contours_pre[0].size();
		for (int i = 0; i < input_layers[0].contours_pre[0].size(); i++)
		{
			for (int j = 0; j < input_layers[1].contours_pre[0].size(); j++)
			{
				cv::Point2f src_init_start = input_layers[0].contours_pre[0][i];
				cv::Point2f src_init_end = input_layers[0].contours_pre[0][(i + 1) % total_seg_src];
				cv::Point2f des_init_start = input_layers[1].contours_pre[0][j];
				cv::Point2f des_init_end = input_layers[1].contours_pre[0][(j + 1) % total_seg_des];
				// distance check
				float dis_init = util::distance(src_init_start, src_init_end, des_init_start, des_init_end);
				std::cout << "dis_check ( " << i << " , " << (i + 1) % total_seg_src << ")  to (" << j << ", " << (j + 1) % total_seg_des << ") is " << dis_init << std::endl;
				if (dis_init > 10){
					std::cout << "dis_check failed" << std::endl;
					continue;
				}
				// angle check
				float angle_init = util::lineLineAngle(src_init_start, src_init_end, des_init_start, des_init_end);
				std::cout << "angle_check ( " << i << " , " << (i + 1) % total_seg_src << ")  to (" << j << ", " << (j + 1) % total_seg_des << ") is " << angle_init << std::endl;
				if (!(abs(angle_init) <= 10 || abs(180 - angle_init) <= 10)){
					std::cout << "angle_check failed" << std::endl;
					continue;
				}
			}
		}
		//after opt
		std::cout << "----------after opt-----------" << std::endl;
		total_seg_src = input_layers[0].contours_snap[0].size();
		total_seg_des = input_layers[1].contours_snap[0].size();
		for (int i = 0; i < input_layers[0].contours_snap[0].size(); i++)
		{
			for (int j = 0; j < input_layers[1].contours_snap[0].size(); j++)
			{
				cv::Point2f src_init_start = input_layers[0].contours_snap[0][i];
				cv::Point2f src_init_end = input_layers[0].contours_snap[0][(i + 1) % total_seg_src];
				cv::Point2f des_init_start = input_layers[1].contours_snap[0][j];
				cv::Point2f des_init_end = input_layers[1].contours_snap[0][(j + 1) % total_seg_des];
				// distance check
				float dis_init = util::distance(src_init_start, src_init_end, des_init_start, des_init_end);
				std::cout << "dis_check ( " << i << " , " << (i + 1) % total_seg_src << ")  to (" << j << ", " << (j + 1) % total_seg_des << ") is " << dis_init << std::endl;
				if (dis_init > 10){
					std::cout << "dis_check failed" << std::endl;
					continue;
				}
				// angle check
				float angle_init = util::lineLineAngle(src_init_start, src_init_end, des_init_start, des_init_end);
				std::cout << "angle_check ( " << i << " , " << (i + 1) % total_seg_src << ")  to (" << j << ", " << (j + 1) % total_seg_des << ") is " << angle_init << std::endl;
				if (!(abs(angle_init) <= 10 || abs(180 - angle_init) <= 10)){
					std::cout << "angle_check failed" << std::endl;
					continue;
				}
			}
		}
	}*/
}

void Regularizer::regularizerMultiRunsForLayers(const QString output_path, const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, QString detect_file, QString config_file){
	std::vector<Layer> input_layers;

	// read detect file
	int curve_num_iterations = 0;
	int curve_min_points = 0;
	float curve_max_error_ratio_to_radius = 0.0f;
	float curve_cluster_epsilon = 0.0f;
	float curve_min_angle = 0.0f;
	float curve_min_radius = 0.0f;
	float curve_max_radius = 0.0f;

	int line_num_iterations = 0;
	int line_min_points = 0;
	float line_max_error = 0.0f;
	float line_cluster_epsilon = 0.0f;
	float line_min_length = 0.0f;
	float line_angle_threshold = 0.0f;

	float contour_max_error = 0.0f;
	float contour_angle_threshold = 0.0f;

	QFile file_detect(detect_file);
	if (file_detect.open(QIODevice::ReadOnly)) {
		QTextStream in(&file_detect);
		rapidjson::Document doc;
		doc.Parse(in.readAll().toUtf8().constData());
		int num_layers = 0;
		num_layers = doc["number"].GetInt();
		std::cout << "num_layers is " << num_layers << std::endl;
		input_layers.resize(num_layers);
		for (int i = 1; i <= num_layers; i++){
			QString current_detect_layer = "layer" + QString::number(i);
			QString current_detect_file = doc[current_detect_layer.toUtf8().constData()].GetString();
			std::cout << "current_detect_file is " << current_detect_file.toUtf8().constData() << std::endl;
			QFile file(current_detect_file);
			if (!file.open(QIODevice::ReadOnly)) {
				std::cerr << "File was not readable: " << std::endl;
				return;
			}
			QTextStream in(&file);
			rapidjson::Document doc;
			doc.Parse(in.readAll().toUtf8().constData());
			//curve
			rapidjson::Value& algs_curve = doc["Curve"];
			curve_num_iterations = algs_curve["iterations"].GetInt();
			curve_min_points = algs_curve["min_points"].GetInt();
			curve_max_error_ratio_to_radius = algs_curve["max_error_ratio_to_radius"].GetFloat();
			curve_cluster_epsilon = algs_curve["cluster_epsilon"].GetFloat();
			curve_min_angle = algs_curve["min_angle"].GetFloat();
			curve_min_radius = algs_curve["min_radius"].GetFloat();
			curve_max_radius = algs_curve["max_radius"].GetFloat();
			//line
			rapidjson::Value& algs_line = doc["Line"];
			line_num_iterations = algs_line["iterations"].GetInt();
			line_min_points = algs_line["min_points"].GetInt();
			line_max_error = algs_line["max_error"].GetFloat();
			line_cluster_epsilon = algs_line["cluster_epsilon"].GetFloat();
			line_min_length = algs_line["min_length"].GetFloat();
			line_angle_threshold = algs_line["angle_threshold"].GetFloat();
			//contour
			rapidjson::Value& algs_contour = doc["Contour"];
			contour_max_error = algs_contour["max_error"].GetFloat();
			contour_angle_threshold = algs_contour["angle_threshold"].GetFloat();
			/*std::cout << "curve_num_iterations is " << curve_num_iterations << std::endl;
			std::cout << "curve_max_error_ratio_to_radius is " << curve_max_error_ratio_to_radius << std::endl;
			std::cout << "curve_min_radius is " << curve_min_radius << std::endl;
			std::cout << "line_num_iterations is " << line_num_iterations << std::endl;
			std::cout << "line_max_error is " << line_max_error << std::endl;
			std::cout << "line_min_length is " << line_min_length << std::endl;
			std::cout << "contour_max_error is " << contour_max_error << std::endl;*/
			createLayer(fileNameList[i - 1], input_layers[i - 1], curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle / 180.0 * CV_PI, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold / 180.0 * CV_PI, contour_max_error, contour_angle_threshold / 180.0 * CV_PI, true, 90);
			input_layers[i - 1].top_height = height_info[i - 1].first;
			input_layers[i - 1].bottom_height = height_info[i - 1].second;
			input_layers[i - 1].parents = tree_info[i - 1].first;
			input_layers[i - 1].children = tree_info[i - 1].second;
			file.close();
		}
	}
	else{
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	file_detect.close();
	// check the number of runs
	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	int num_runs = 0;
	num_runs = doc["number"].GetInt();
	std::cout << "num_runs is " << num_runs << std::endl;
	//
	for (int i = 1; i <= num_runs; i++){
		QString current_config_version = "config" + QString::number(i);
		QString current_config_file = doc[current_config_version.toUtf8().constData()].GetString();
		std::cout << "current_config_file is " << current_config_file.toUtf8().constData() << std::endl;
		QFile file(current_config_file);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cerr << "File was not readable: " << std::endl;
			return;
		}
		QTextStream in(&file);
		rapidjson::Document doc;
		doc.Parse(in.readAll().toUtf8().constData());
		bool bUseIntra = doc["UseIntra"].GetBool();
		bool bUseInter = doc["UseInter"].GetBool();
		std::cout << "intra is " << bUseIntra << ", inter is " << bUseInter << std::endl;
		file.close();
		if (bUseIntra && !bUseInter){
			std::vector<QString> layers_image_files;
			layers_image_files.resize(input_layers.size());
			for (int j = 0; j < input_layers.size(); j++){
				generateContoursLayer(input_layers[j], current_config_file);
				post_processing(input_layers[j], 10);
				layers_image_files[j] = saveImage(input_layers[j], j, i, output_path);
				
			}
			// overlay images
			overlayImages(layers_image_files, output_path + "/run_" + QString::number(i) + "_overlay.png");
		}
		else if (bUseInter){
			//std::vector<QString> layers_image_files;
			//layers_image_files.resize(input_layers.size());
			//ShapeFitLayersInter::fit(input_layers, current_config_file);
			//for (int j = 0; j < input_layers.size(); j++){
			//	post_processing(input_layers[j], 10);
			//	layers_image_files[j] = saveImage(input_layers[j], j, i);
			//}
			//// overlay images
			//overlayImages(layers_image_files, "../test/run_" + QString::number(i) + "_overlay.png");
			std::vector<QString> layers_image_files;
			layers_image_files.resize(input_layers.size());
			ShapeFitLayersAll::fit(input_layers, current_config_file);
			for (int j = 0; j < input_layers.size(); j++){
				post_processing(input_layers[j], 10);
				layers_image_files[j] = saveImage(input_layers[j], j, i, output_path);
			}
			// overlay images
			overlayImages(layers_image_files, output_path + "/run_" + QString::number(i) + "_overlay.png");
		}
		else{
			// do nothing
		}
		
	}
	file.close();

}

void Regularizer::regularizerAll(const QString output_path, const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, QString detect_file, QString config_file){
	std::vector<Layer> input_layers;
	// read detect file
	int curve_num_iterations = 0;
	int curve_min_points = 0;
	float curve_max_error_ratio_to_radius = 0.0f;
	float curve_cluster_epsilon = 0.0f;
	float curve_min_angle = 0.0f;
	float curve_min_radius = 0.0f;
	float curve_max_radius = 0.0f;

	int line_num_iterations = 0;
	int line_min_points = 0;
	float line_max_error = 0.0f;
	float line_cluster_epsilon = 0.0f;
	float line_min_length = 0.0f;
	float line_angle_threshold = 0.0f;

	float contour_max_error = 0.0f;
	float contour_angle_threshold = 0.0f;

	QFile file_detect(detect_file);
	if (file_detect.open(QIODevice::ReadOnly)) {
		QTextStream in(&file_detect);
		rapidjson::Document doc;
		doc.Parse(in.readAll().toUtf8().constData());
		int num_layers = 0;
		//num_layers = doc["number"].GetInt();
		num_layers = fileNameList.size();
		std::cout << "num_layers is " << num_layers << std::endl;
		input_layers.resize(num_layers);
		for (int i = 1; i <= num_layers; i++){
			QString current_detect_layer = "layer" + QString::number(i);
			QString current_detect_file = doc[current_detect_layer.toUtf8().constData()].GetString();
			std::cout << "current_detect_file is " << current_detect_file.toUtf8().constData() << std::endl;
			QFile file(current_detect_file);
			if (!file.open(QIODevice::ReadOnly)) {
				std::cerr << "File was not readable: " << std::endl;
				return;
			}
			QTextStream in(&file);
			rapidjson::Document doc;
			doc.Parse(in.readAll().toUtf8().constData());
			//curve
			rapidjson::Value& algs_curve = doc["Curve"];
			curve_num_iterations = algs_curve["iterations"].GetInt();
			curve_min_points = algs_curve["min_points"].GetInt();
			curve_max_error_ratio_to_radius = algs_curve["max_error_ratio_to_radius"].GetFloat();
			curve_cluster_epsilon = algs_curve["cluster_epsilon"].GetFloat();
			curve_min_angle = algs_curve["min_angle"].GetFloat();
			curve_min_radius = algs_curve["min_radius"].GetFloat();
			curve_max_radius = algs_curve["max_radius"].GetFloat();
			//line
			rapidjson::Value& algs_line = doc["Line"];
			line_num_iterations = algs_line["iterations"].GetInt();
			line_min_points = algs_line["min_points"].GetInt();
			line_max_error = algs_line["max_error"].GetFloat();
			line_cluster_epsilon = algs_line["cluster_epsilon"].GetFloat();
			line_min_length = algs_line["min_length"].GetFloat();
			line_angle_threshold = algs_line["angle_threshold"].GetFloat();
			//contour
			rapidjson::Value& algs_contour = doc["Contour"];
			contour_max_error = algs_contour["max_error"].GetFloat();
			contour_angle_threshold = algs_contour["angle_threshold"].GetFloat();
			/*std::cout << "curve_num_iterations is " << curve_num_iterations << std::endl;
			std::cout << "curve_max_error_ratio_to_radius is " << curve_max_error_ratio_to_radius << std::endl;
			std::cout << "curve_min_radius is " << curve_min_radius << std::endl;
			std::cout << "line_num_iterations is " << line_num_iterations << std::endl;
			std::cout << "line_max_error is " << line_max_error << std::endl;
			std::cout << "line_min_length is " << line_min_length << std::endl;
			std::cout << "contour_max_error is " << contour_max_error << std::endl;*/
			createLayer(fileNameList[i - 1], input_layers[i - 1], curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle / 180.0 * CV_PI, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold / 180.0 * CV_PI, contour_max_error, contour_angle_threshold / 180.0 * CV_PI, true, 80);
			input_layers[i - 1].top_height = height_info[i - 1].first;
			input_layers[i - 1].bottom_height = height_info[i - 1].second;
			input_layers[i - 1].parents = tree_info[i - 1].first;
			input_layers[i - 1].children = tree_info[i - 1].second;
			file.close();
		}
	}
	else{
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	file_detect.close();
	// check the number of runs
	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	int num_runs = 0;
	num_runs = doc["number"].GetInt();
	std::cout << "num_runs is " << num_runs << std::endl;
	//
	if (true){
		for (int j = 0; j < input_layers.size(); j++){
			saveImage(input_layers[j], j, 0, output_path);
		}
		std::vector<QString> layers_image_files;
		layers_image_files.resize(input_layers.size());
		for (int j = 0; j < input_layers.size(); j++){
			layers_image_files[j] = saveImage(input_layers[j], j, 0, output_path);
		}
		// overlay images
		overlayImages(layers_image_files, output_path + "/run_" + QString::number(0) + "_overlay.png");
		//for (int j = 0; j < input_layers.size(); j++){
		//	saveImage(input_layers[j], j, 0, output_path);
		//}
		//float average_iou = 0;
		//float num_polygons = 0;
		//int average_vertices = 0;
		//for (int k = 0; k < input_layers.size(); k++){
		//	for (int i = 0; i < input_layers[k].contours.size(); i++){
		//		////////// DEBUG //////////
		//		// calculate IOU
		//		float iou = 0.0f;
		//		if (input_layers[k].contours[i].size() != 0){
		//			if (util::isSimple(input_layers[k].polygons[i].contour) && util::isSimple(input_layers[k].contours[i])){
		//				iou = util::calculateIOU(input_layers[k].polygons[i].contour, input_layers[k].contours[i]);
		//				std::cout << "IOU = " << iou << std::endl;
		//			}
		//			else{
		//				iou = util::calculateIOUbyImage(input_layers[k].polygons[i].contour, input_layers[k].contours[i], 1000);
		//				std::cout << "IOU = " << iou << std::endl;
		//			}
		//			average_vertices += input_layers[k].contours[i].size();
		//			average_iou += iou;
		//			num_polygons++;
		//			std::cout << "-----------------------" << std::endl;
		//		}
		//	}
		//}
		//QString filename_new = "../test/info_iou.txt";
		//QFile file_new(filename_new);
		//if (file_new.open(QIODevice::ReadWrite | QIODevice::Append))
		//{
		//	QTextStream stream(&file_new);
		//	if (num_polygons != 0){
		//		stream << average_iou / num_polygons << "," << 1.0 * average_vertices / num_polygons << endl;
		//	}
		//}
		//file_new.close();
	}
	//
	for (int i = 1; i <= num_runs; i++){
		QString current_config_version = "config" + QString::number(i);
		QString current_config_file = doc[current_config_version.toUtf8().constData()].GetString();
		std::cout << "current_config_file is " << current_config_file.toUtf8().constData() << std::endl;

		QFile file(current_config_file);
		if (!file.open(QIODevice::ReadOnly)) {
			std::cerr << "File was not readable: " << std::endl;
			return;
		}
		QTextStream in(&file);
		rapidjson::Document doc;
		doc.Parse(in.readAll().toUtf8().constData());
		bool bUseIntra = doc["UseIntra"].GetBool();
		bool bUseInter = doc["UseInter"].GetBool();
		std::cout << "intra is " << bUseIntra << ", inter is " << bUseInter << std::endl;
		file.close();
		if (bUseIntra && !bUseInter){
			std::vector<QString> layers_image_files;
			layers_image_files.resize(input_layers.size());
			for (int j = 0; j < input_layers.size(); j++){
				generateContoursLayer(input_layers[j], current_config_file);
				post_processing(input_layers[j], 10);
				layers_image_files[j] = saveImage(input_layers[j], j, i, output_path);

			}
			// overlay images
			overlayImages(layers_image_files, output_path + "/run_" + QString::number(i) + "_overlay.png");
		}
		else if (bUseInter){
			std::vector<QString> layers_image_files;
			layers_image_files.resize(input_layers.size());
			ShapeFitLayersAll::fit(input_layers, current_config_file);
			for (int j = 0; j < input_layers.size(); j++){
				post_processing(input_layers[j], 10);
				layers_image_files[j] = saveImage(input_layers[j], j, i, output_path);
			}
			// overlay images
			overlayImages(layers_image_files, output_path + "/run_" + QString::number(i) + "_overlay.png");
		}
		else{
			// do nothing
		}

	}
	if (num_runs > 0)
	{
		float average_iou = 0;
		float num_polygons = 0;
		int average_vertices = 0;
		for (int k = 0; k < input_layers.size(); k++){
			for (int i = 0; i < input_layers[k].contours.size(); i++){
				////////// DEBUG //////////
				// calculate IOU
				float iou = 0.0f;
				if (input_layers[k].contours[i].size() != 0){
					if (util::isSimple(input_layers[k].polygons[i].contour) && util::isSimple(input_layers[k].contours[i])){
						iou = util::calculateIOU(input_layers[k].polygons[i].contour, input_layers[k].contours[i]);
						std::cout << "IOU = " << iou << std::endl;
					}
					else{
						iou = util::calculateIOUbyImage(input_layers[k].polygons[i].contour, input_layers[k].contours[i], 1000);
						std::cout << "IOU = " << iou << std::endl;
					}
					average_vertices += input_layers[k].contours[i].size();
					average_iou += iou;
					num_polygons++;
					std::cout << "-----------------------" << std::endl;
				}
			}
		}
		QString filename_new = "../test/info_iou.txt";
		QFile file_new(filename_new);
		if (file_new.open(QIODevice::ReadWrite | QIODevice::Append))
		{
			QTextStream stream(&file_new);
			if (num_polygons != 0){
				stream << average_iou / num_polygons << "," << 1.0 * average_vertices / num_polygons << endl;
			}
		}
		file_new.close();
	}
	file.close();

}


void Regularizer::createLayer(QString fileName, Layer & layer, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold){
	layer.generateLayer(fileName, curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold, bUseSymmetryLineOpt, iouThreahold);
}

void Regularizer::createLayers(const std::vector<QString> &fileNameList, const std::vector<std::pair<float, float>>& height_info, const std::vector<std::pair<std::vector<int>, std::vector<int>>>& tree_info, std::vector<Layer>& layers, int curve_num_iterations, int curve_min_points, float curve_max_error_ratio_to_radius, float curve_cluster_epsilon, float curve_min_angle, float curve_min_radius, float curve_max_radius, int line_num_iterations, int line_min_points, float line_max_error, float line_cluster_epsilon, float line_min_length, float line_angle_threshold, float contour_max_error, float contour_angle_threshold, bool bUseSymmetryLineOpt, float iouThreahold){
	int num_layers = fileNameList.size();
	layers.resize(num_layers);
	for (int i = 0; i < num_layers; i++){
		createLayer(fileNameList[i], layers[i], curve_num_iterations, curve_min_points, curve_max_error_ratio_to_radius, curve_cluster_epsilon, curve_min_angle, curve_min_radius, curve_max_radius, line_num_iterations, line_min_points, line_max_error, line_cluster_epsilon, line_min_length, line_angle_threshold, contour_max_error, contour_angle_threshold, bUseSymmetryLineOpt, iouThreahold);
		layers[i].top_height = height_info[i].first;
		layers[i].bottom_height = height_info[i].second;
		layers[i].parents = tree_info[i].first;
		layers[i].children = tree_info[i].second;
	}
}


void Regularizer::generateContoursLayer(Layer& input_layer, QString config_file){

	// if all contours sizes are 0
	bool bValidLayer = false;
	for (int i = 0; i < input_layer.contours.size(); i++){
		if (input_layer.contours[i].size() != 0){
			bValidLayer = true;
			break;
		}
	}
	if (!bValidLayer){
		std::cout << "no need to use optimization" << std::endl;
		return;
	}
	bool bUseRaOpt = false;
	bool bUseParallelOpt = false;
	bool bUseSymmetryLineOpt = false;
	bool bUseAccuracyOpt = false;
	float raThreahold = 0.0f;
	float raWeight = 0.0f;
	float parallelThreahold = 0.0f;
	float parallelWeight = 0.0f;
	float symmetryWeight = 0.0f;
	float accuracyWeight = 0.0f;
	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	bool bUseIntra = doc["UseIntra"].GetBool();
	if (bUseIntra){
		rapidjson::Value& algs = doc["IntraOpt"];
		//ra
		rapidjson::Value& algs_ra = algs["RA"];
		bUseRaOpt = algs_ra["UseOpt"].GetBool();
		raThreahold = algs_ra["AngleThreshold"].GetFloat();
		raWeight = algs_ra["Weight"].GetFloat();
		//symmetry
		rapidjson::Value& algs_symmetry = algs["Symmetry"];
		bUseSymmetryLineOpt = algs_symmetry["UseOpt"].GetBool();
		symmetryWeight = algs_symmetry["Weight"].GetFloat();
		// parallel
		rapidjson::Value& algs_parallel = algs["Parallel"];
		bUseParallelOpt = algs_parallel["UseOpt"].GetBool();
		parallelThreahold = algs_parallel["AngleThreshold"].GetFloat();
		parallelWeight = algs_parallel["Weight"].GetFloat();
		// accuracy
		rapidjson::Value& algs_accuracy = algs["Accuracy"];
		bUseAccuracyOpt = algs_accuracy["UseOpt"].GetBool();
		accuracyWeight = algs_accuracy["Weight"].GetFloat();
		//{
		//	std::cout << "bUseRa " << bUseRaOpt << " ra angle is " << raThreahold << " ra weight is " << raWeight << std::endl;
		//	std::cout << "bUseParallel " << bUseParallelOpt << " Parallel angle is " << parallelThreahold << " Parallel weight is " << parallelWeight << std::endl;
		//	std::cout << "bUseSymmetry " << bUseSymmetryLineOpt << " Symmetry weight is " << symmetryWeight << std::endl;
		//	std::cout << "bUseAccuracy " << bUseAccuracyOpt << " Accuracy weight is " << accuracyWeight << std::endl;
		//}
		file.close();
	}
	else{
		file.close();
		return;
	}
	// Here we have all original polygons, all initial points and all symmetry lines
	// We call optimization below
	input_layer.contours = ShapeFitLayer::fit(input_layer.sparse_contours, input_layer.contours, bUseRaOpt, raThreahold, raWeight, bUseParallelOpt, parallelThreahold, parallelWeight, bUseSymmetryLineOpt, input_layer.symmetry_lines, symmetryWeight, bUseAccuracyOpt, accuracyWeight);
	for (int i = 0; i < input_layer.contours.size(); i++){
		////////// DEBUG //////////
		// calculate IOU
		if (input_layer.contours[i].size() != 0){
			std::cout << "Pre_processing" << std::endl;
			std::cout << "Polygon " << i << std::endl;
			if (util::isSimple(input_layer.polygons[i].contour) && util::isSimple(input_layer.contours[i]))
				std::cout << "IOU = " << util::calculateIOU(input_layer.polygons[i].contour, input_layer.contours[i]) << std::endl;
			else
				std::cout << "IOU = " << util::calculateIOUbyImage(input_layer.polygons[i].contour, input_layer.contours[i], 1000) << std::endl;
			std::cout << "#vertices = " << input_layer.contours[i].size() << std::endl;
			std::cout << "-----------------------" << std::endl;
		}
	}
}

void Regularizer::generateContoursLayers(std::vector<Layer>& input_layers, QString config_file){
	// first do intra layer optimization
	for (int i = 0; i < input_layers.size(); i++){
		generateContoursLayer(input_layers[i], config_file);
		post_processing(input_layers[i], 10);
		for (int j = 0; j < input_layers[i].contours.size(); j++){
			////////// DEBUG //////////
			// calculate IOU
			if (input_layers[i].contours[j].size() != 0){
				std::cout << "Post_processing" << std::endl;
				std::cout << "Polygon " << j << std::endl;
				if (util::isSimple(input_layers[i].polygons[j].contour) && util::isSimple(input_layers[i].contours[j]))
					std::cout << "IOU = " << util::calculateIOU(input_layers[i].polygons[j].contour, input_layers[i].contours[j]) << std::endl;
				else
					std::cout << "IOU = " << util::calculateIOUbyImage(input_layers[i].polygons[j].contour, input_layers[i].contours[j], 1000) << std::endl;
				std::cout << "#vertices = " << input_layers[i].contours[j].size() << std::endl;
				std::cout << "-----------------------" << std::endl;
			}
		}
		saveImage(input_layers[i], i, 1, "../test");
	}
	// inter layer optimizaiton
	ShapeFitLayersInter::fit(input_layers, config_file);
}

void Regularizer::post_processing(Layer & layer, float angle_threshold){
	std::vector<std::vector<cv::Point2f>> new_contours;
	new_contours.resize(layer.contours.size());
	for (int i = 0; i < layer.contours.size(); i++){
		if (layer.contours[i].size() < 4){
			new_contours[i] = layer.contours[i];
			continue;
		}
		int total_segments = layer.contours[i].size();
		// note: start from -1
		for (int j = 0; j < layer.contours[i].size(); j++){
			int first_start = (j - 1 + total_segments) % total_segments;
			int first_end = (j) % total_segments;
			int second_start = (j) % total_segments;
			int second_end = (j + 1) % total_segments;
			cv::Point2f a = layer.contours[i][first_start];
			cv::Point2f b = layer.contours[i][first_end];
			cv::Point2f c = layer.contours[i][second_start];
			cv::Point2f d = layer.contours[i][second_end];
			float angle = util::lineLineAngle(a, b, c, d);
			if (abs(angle) <= angle_threshold || abs(angle - 180) <= angle_threshold){
				continue;
			}
			else{
				new_contours[i].push_back(b);
			}
		}
	}
	layer.contours.clear();
	layer.contours = new_contours;
}

QString Regularizer::saveImage(Layer & layer, int index, int level, QString output_path){
	QImage src = layer.orig_image;
	QImage image(QSize(src.width(), src.height()), QImage::Format_RGB32);
	QPixmap pixmap;
	QPainter painter(&image);
	painter.setBrush(QBrush(Qt::white));
	painter.fillRect(QRectF(0, 0, src.width(), src.height()), Qt::white);
	painter.setPen(QPen(QColor(0, 0, 0), 1));
	for (auto& polygon : layer.polygons) {
		QPolygon pgon;
		for (auto& p : polygon.contour) {
			pgon.push_back(QPoint(p.x, p.y));
		}
		painter.drawPolygon(pgon);
		for (auto& hole : polygon.holes) {
			QPolygon pgon;
			for (auto& p : hole) {
				pgon.push_back(QPoint(p.x, p.y));
			}
			painter.drawPolygon(pgon);
		}
	}

	if (layer.contours.size() != 0){
		for (auto& contour : layer.contours) {
			//painter.setPen(QPen(QColor(255, 0, 0), 3));
			painter.setPen(QPen(QColor(0, rand() % 256, rand() % 256), 3));
			QPolygon pol;
			for (int i = 0; i < contour.size(); i++) {
				pol.push_back(QPoint(contour[i].x, contour[i].y));
			}
			painter.drawPolygon(pol);
		}
	}

	//if (layer.symmetry_lines.size() != 0){
	//	for (int i = 0; i < layer.symmetry_lines.size(); i++){
	//		if (layer.symmetry_lines[i].size() == 0) continue;
	//		//std::cout << "paint symmetry line" << std::endl;
	//		painter.setPen(QPen(QColor(255, 0, 0), 3));
	//		cv::Point2f p1 = layer.symmetry_lines[i][0];
	//		cv::Point2f p2 = layer.symmetry_lines[i][1];
	//		painter.drawLine(p1.x, p1.y, p2.x , p2.y);
	//	}
	//}
	painter.end();

	QString image_file = output_path + "/run_" + QString::number(level) + "_contours_" + QString::number(index) + ".png";
	image.save(image_file);

	//{
	//	int i = 1;
	//	painter.setPen(QPen(QColor(rand() % 256, rand() % 256, rand() % 256), 3));
	//	cv::Point2f p1 = layer.contours_pre[0][i];
	//	cv::Point2f p2 = layer.contours_pre[0][(i + 1) % layer.contours_pre[0].size()];
	//	painter.drawLine(p1.x, p1.y, p2.x, p2.y);
	//}
	return image_file;
}

void Regularizer::overlayImages(const std::vector<QString>& image_files, QString output_file){
	if (image_files.size() <= 1)
		std::cout <<"No need to do overlay action!!!" << std::endl;
	QPixmap base;
	base.load(image_files[0]);
	QPainter painter;
	painter.begin(&base);
	//Assume they have the same size
	for (int k = 1; k < image_files.size(); k++){
		QImage overlay;
		overlay.load(image_files[k]);
		for (int i = 0; i < base.width(); i++){
			for (int j = 0; j < base.height(); j++){
				QColor clrCurrent(overlay.pixel(i, j));
				if (clrCurrent.red() == 0 && clrCurrent.green() >= 0 && clrCurrent.blue() >= 0){
					//if (clrCurrent.red() == 255 && clrCurrent.green() == 0 && clrCurrent.blue() == 0){
					painter.setPen(clrCurrent);
					painter.drawPoint(i, j);
				}
				else{

				}
			}
		}
	}
	painter.end();
	base.save(output_file);
}
