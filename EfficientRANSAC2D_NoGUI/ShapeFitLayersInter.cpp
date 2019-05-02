#include "ShapeFitLayersInter.h"
#include <QFileDialog>
#include <QTextStream>
#include "../EfficientRANSAC2D_NoGUI/rapidjson/document.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/writer.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/stringbuffer.h"

ShapeFitLayersInter::ShapeFitLayersInter() {
}

ShapeFitLayersInter::~ShapeFitLayersInter() {
}

void ShapeFitLayersInter::fit(std::vector<Layer>& layers, QString config_file)
{
	bool bUseIntra = false;
	bool bUseInter = false;
	bool bUseRaOpt = false;
	float angle_threshold_RA = 0.0f;
	float raWeight = 0.0f;
	bool bUseParallelOpt = false;
	float angle_threshold_parallel = 0.0f;
	float parallelWeight = 0.0f;
	bool bUseSymmetryLineOpt = false;
	float symmetryWeight = 0.0f;
	bool bUseAccuracyOpt = false;
	float accuracyWeight = 0.0f;
	bool bUsePointSnapOpt = false;
	float pointDisThreshold = 0.0f;
	float pointWeight = 0.0f;
	bool bUseSegSnapOpt = false;
	float segDisThreshold = 0.0f;
	float segAngleThreshold = 0.0f;
	float segWeight = 0.0f;

	QFile file(config_file);
	if (!file.open(QIODevice::ReadOnly)) {
		std::cerr << "File was not readable: " << std::endl;
		return;
	}
	QTextStream in(&file);
	rapidjson::Document doc;
	doc.Parse(in.readAll().toUtf8().constData());
	bUseIntra = doc["UseIntra"].GetBool();
	bUseInter = doc["UseInter"].GetBool();
	/*if (bUseIntra){
		rapidjson::Value& algs = doc["IntraOpt"];
		//ra
		rapidjson::Value& algs_ra = algs["RA"];
		bUseRaOpt = algs_ra["UseOpt"].GetBool();
		angle_threshold_RA = algs_ra["AngleThreshold"].GetFloat();
		raWeight = algs_ra["Weight"].GetFloat();
		//symmetry
		rapidjson::Value& algs_symmetry = algs["Symmetry"];
		bUseSymmetryLineOpt = algs_symmetry["UseOpt"].GetBool();
		symmetryWeight = algs_symmetry["Weight"].GetFloat();
		// parallel
		rapidjson::Value& algs_parallel = algs["Parallel"];
		bUseParallelOpt = algs_parallel["UseOpt"].GetBool();
		angle_threshold_parallel = algs_parallel["AngleThreshold"].GetFloat();
		parallelWeight = algs_parallel["Weight"].GetFloat();
		// accuracy
		rapidjson::Value& algs_accuracy = algs["Accuracy"];
		bUseAccuracyOpt = algs_accuracy["UseOpt"].GetBool();
		accuracyWeight = algs_accuracy["Weight"].GetFloat();
	}*/
	if (bUseInter){
		rapidjson::Value& algs = doc["InterOpt"];
		// point snap
		rapidjson::Value& algs_point = algs["PointSnap"];
		bUsePointSnapOpt = algs_point["UseOpt"].GetBool();
		pointDisThreshold = algs_point["DisThreshold"].GetFloat();
		pointWeight = algs_point["Weight"].GetFloat();
		// seg snap
		rapidjson::Value& algs_seg = algs["SegSnap"];
		bUseSegSnapOpt = algs_seg["UseOpt"].GetBool();
		segDisThreshold = algs_seg["DisThreshold"].GetFloat();
		segAngleThreshold = algs_seg["AngleThreshold"].GetFloat();
		segWeight = algs_seg["Weight"].GetFloat();
	}
	file.close();
	if (!bUseInter){
		return;
	}
	{
		std::cout << "bUsePoint " << bUsePointSnapOpt << " Point threshold is " << pointDisThreshold << " Point weight is " << pointWeight << std::endl;
		std::cout << "bUseSeg " << bUseSegSnapOpt << " seg angle is " << segAngleThreshold << " seg weight is " << segWeight << std::endl;
	}
	float min_x = std::numeric_limits<float>::max();
	float min_y = std::numeric_limits<float>::max();
	float max_x = -std::numeric_limits<float>::max();
	float max_y = -std::numeric_limits<float>::max();
	for (int k = 0; k < layers.size(); k++){
		for (int i = 0; i < layers[k].sparse_contours.size(); i++) {
			for (int j = 0; j < layers[k].sparse_contours[i].size(); j++){
				min_x = std::min(min_x, layers[k].sparse_contours[i][j].x);
				min_y = std::min(min_y, layers[k].sparse_contours[i][j].y);
				max_x = std::max(max_x, layers[k].sparse_contours[i][j].x);
				max_y = std::max(max_y, layers[k].sparse_contours[i][j].y);
			}
		}
	}
	for (int k = 0; k < layers.size(); k++){
		for (int i = 0; i < layers[k].contours.size(); i++) {
			for (int j = 0; j < layers[k].contours[i].size(); j++){
				min_x = std::min(min_x, layers[k].contours[i][j].x);
				min_y = std::min(min_y, layers[k].contours[i][j].y);
				max_x = std::max(max_x, layers[k].contours[i][j].x);
				max_y = std::max(max_y, layers[k].contours[i][j].y);
			}
		}
	}
	if (bUseSymmetryLineOpt){
		for (int k = 0; k < layers.size(); k++){
			for (int i = 0; i < layers[k].symmetry_lines.size(); i++){
				for (int j = 0; j < layers[k].symmetry_lines[i].size(); j++){
					min_x = std::min(min_x, layers[k].symmetry_lines[i][j].x);
					min_y = std::min(min_y, layers[k].symmetry_lines[i][j].y);
					max_x = std::max(max_x, layers[k].symmetry_lines[i][j].x);
					max_y = std::max(max_y, layers[k].symmetry_lines[i][j].y);
				}
			}
		}
	}
	float max_unit = std::max(max_x - min_x, max_y - min_y);
	Config config(bUseIntra, bUseInter, bUseRaOpt, angle_threshold_RA, raWeight, bUseParallelOpt, angle_threshold_parallel, parallelWeight, bUseSymmetryLineOpt, symmetryWeight, bUseAccuracyOpt, accuracyWeight, bUsePointSnapOpt, pointDisThreshold / max_unit, pointWeight, bUseSegSnapOpt, segDisThreshold / max_unit, segAngleThreshold, segWeight);

	std::vector<layer_polygons> normalized_polygons(layers.size());
	for (int k = 0; k < layers.size(); k++){
		normalized_polygons[k].resize(layers[k].sparse_contours.size());
		for (int i = 0; i < layers[k].sparse_contours.size(); i++) {
			normalized_polygons[k][i].resize(layers[k].sparse_contours[i].size());
			for (int j = 0; j < layers[k].sparse_contours[i].size(); j++){
				normalized_polygons[k][i][j] = cv::Point2f((layers[k].sparse_contours[i][j].x - min_x) / max_unit, (layers[k].sparse_contours[i][j].y - min_y) / max_unit);
			}
		}
	}

	// normalized_symmetry_line
	std::vector<layer_polygons> normalized_symmetry_lines;

	if (bUseSymmetryLineOpt){
		normalized_symmetry_lines.resize(layers.size());
		for (int k = 0; k < layers.size(); k++){
			normalized_symmetry_lines[k].resize(layers[k].symmetry_lines.size());
			for (int i = 0; i < layers[k].symmetry_lines.size(); i++) {
				normalized_symmetry_lines[k][i].resize(layers[k].symmetry_lines[i].size());
				for (int j = 0; j < layers[k].symmetry_lines[i].size(); j++){
					normalized_symmetry_lines[k][i][j] = cv::Point2f((layers[k].symmetry_lines[i][j].x - min_x) / max_unit, (layers[k].symmetry_lines[i][j].y - min_y) / max_unit);
				}
			}
		}
	}


	int total_points = 0;
	std::vector<layer_polygons> normalized_polygons_init(layers.size());
	for (int k = 0; k < layers.size(); k++){
		normalized_polygons_init[k].resize(layers[k].contours.size());
		for (int i = 0; i < layers[k].contours.size(); i++) {
			normalized_polygons_init[k][i].resize(layers[k].contours[i].size());
			total_points += layers[k].contours[i].size();
			for (int j = 0; j < layers[k].contours[i].size(); j++){
				normalized_polygons_init[k][i][j] = cv::Point2f((layers[k].contours[i][j].x - min_x) / max_unit, (layers[k].contours[i][j].y - min_y) / max_unit);
			}
		}
	}

	std::vector<std::pair<float, float>> layers_height;
	layers_height.resize(layers.size());
	for (int k = 0; k < layers.size(); k++){
		layers_height[k] = std::make_pair(layers[k].top_height, layers[k].bottom_height);
		std::cout << "layers_height " << k << ", top height is " << layers_height[k].first << ", bot height is " << layers_height[k].second << std::endl;
	}
	// tree info
	std::vector<std::pair<std::vector<int>, std::vector<int>>> tree_info;
	tree_info.resize(layers.size());
	for (int k = 0; k < layers.size(); k++){
		tree_info[k] = std::make_pair(layers[k].parents, layers[k].children);
		if (tree_info[k].first.size() == 0)
			std::cout << "tree_info " << k << ", no parent nodes" << std::endl;
		else{
			for (int j = 0; j < tree_info[k].first.size(); j++)
				std::cout << "tree_info " << k << ", parent nodes are " << tree_info[k].first[j]<<", ";
			std::cout << std::endl;
		}
		if (tree_info[k].second.size() == 0)
			std::cout << "tree_info " << k << ", no children nodes" << std::endl;
		else{
			for (int j = 0; j < tree_info[k].second.size(); j++)
				std::cout << "tree_info " << k << ", children nodes are " << tree_info[k].second[j] << ", ";
			std::cout << std::endl;
		}
	}
	try {
		std::cout << "total points is " << total_points << std::endl;
		column_vector starting_point(total_points * 2);
		int start_index = 0;
		for (int k = 0; k < normalized_polygons_init.size(); k++){
			for (int i = 0; i < normalized_polygons_init[k].size(); i++){
				for (int j = 0; j < normalized_polygons_init[k][i].size(); j++){
					starting_point((j + start_index) * 2) = normalized_polygons_init[k][i][j].x;
					starting_point((j + start_index) * 2 + 1) = normalized_polygons_init[k][i][j].y;
				}
				start_index += normalized_polygons_init[k][i].size();
			}
		}
		// test point
		/*{
			float score = 0.0f;
			std::cout << "normalized_polygons_init size is " << normalized_polygons_init.size() << std::endl;
			std::cout << "config.pointDisThreshold is " << config.pointDisThreshold << std::endl;
			std::cout << "max_unit is " << max_unit << std::endl;
			for (int k = 0; k < normalized_polygons_init.size(); k++){
				for (int i = 0; i < normalized_polygons_init[k].size(); i++){
					if (normalized_polygons_init[k][i].size() != 0){
						std::cout << "--------------------------" << std::endl;
						// optimization score for one polygon
						if (config.bUsePointSnapOpt){
							int valid_num = 0;
							std::cout << "Point Snap" << std::endl;
							for (int j = 0; j < tree_info[k].second.size(); j++){
								std::cout << "layer "<< k << " children nodes from layer " << tree_info[k].second[j] << std::endl;
								float score_tmp = util::calculateScorePointOpt(normalized_polygons_init[k][i], normalized_polygons_init[k][i], normalized_polygons_init[tree_info[k].second[j]], normalized_polygons_init[tree_info[k].second[j]], config.pointDisThreshold);
								if (abs(score_tmp) > 0.1){
									valid_num++;
									score += score_tmp;
								}
							}
							for (int j = 0; j < tree_info[k].first.size(); j++){
								std::cout << "layer " << k << " parents nodes from layer " << tree_info[k].first[j] << std::endl;
								float score_tmp = util::calculateScorePointOpt(normalized_polygons_init[k][i], normalized_polygons_init[k][i], normalized_polygons_init[tree_info[k].first[j]], normalized_polygons_init[tree_info[k].first[j]], config.pointDisThreshold);
								if (abs(score_tmp) > 0.1){
									valid_num++;
									score += score_tmp;
								}
							}
							std::cout << "valid_num is " << valid_num << std::endl;
							if (valid_num > 0){
								score = score / valid_num;
								score = score * config.pointWeight;
							}
							
						}
						std::cout << "--------------------------" << std::endl;
					}
				}
			}
		}*/
		// test segs
		/*{
			float score = 0.0f;
			std::cout << "normalized_polygons_init size is " << normalized_polygons_init.size() << std::endl;
			std::cout << "config.pointDisThreshold is " << config.pointDisThreshold << std::endl;
			std::cout << "config.segAngleThreshold is " << config.segAngleThreshold << std::endl;
			std::cout << "max_unit is " << max_unit << std::endl;
			for (int k = 0; k < normalized_polygons_init.size(); k++){
				for (int i = 0; i < normalized_polygons_init[k].size(); i++){
					if (normalized_polygons_init[k][i].size() != 0){
						std::cout << "--------------------------" << std::endl;
						// optimization score for one polygon
						if (config.bUseSegSnapOpt){
							int valid_num = 0;
							std::cout << "Seg Snap" << std::endl;
							for (int j = 0; j < tree_info[k].second.size(); j++){
								std::cout << "layer " << k << " children nodes from layer " << tree_info[k].second[j] << std::endl;
								float score_tmp = util::calculateScoreSegOpt(normalized_polygons_init[k][i], normalized_polygons_init[k][i], normalized_polygons_init[tree_info[k].second[j]], normalized_polygons_init[tree_info[k].second[j]], config.segDisThreshold, config.segAngleThreshold);
								if (abs(score_tmp) > 0.1){
									valid_num++;
									score += score_tmp;
								}
							}
							for (int j = 0; j < tree_info[k].first.size(); j++){
								std::cout << "layer " << k << " parents nodes from layer " << tree_info[k].first[j] << std::endl;
								float score_tmp = util::calculateScoreSegOpt(normalized_polygons_init[k][i], normalized_polygons_init[k][i], normalized_polygons_init[tree_info[k].first[j]], normalized_polygons_init[tree_info[k].first[j]], config.segDisThreshold, config.segAngleThreshold);
								if (abs(score_tmp) > 0.1){
									valid_num++;
									score += score_tmp;
								}
							}
							if (valid_num > 0){
								score = score / valid_num;
								score = score * config.segWeight;
							}
						}
						std::cout << "--------------------------" << std::endl;
					}
				}
			}
		}*/
		BFGSSolver solver(normalized_polygons, normalized_polygons_init, normalized_symmetry_lines, layers_height, tree_info, config);
		if (true)
			find_max_using_approximate_derivatives(dlib::bfgs_search_strategy(), dlib::objective_delta_stop_strategy(1e-7), solver, starting_point, 1, 0.0001);
		else
			find_min_using_approximate_derivatives(dlib::bfgs_search_strategy(), dlib::objective_delta_stop_strategy(1e-7), solver, starting_point, 0, 0.0001);
		start_index = 0;
		std::vector<layer_polygons> new_layers_contours;
		new_layers_contours.resize(layers.size());
		for (int k = 0; k < layers.size(); k++){
			new_layers_contours[k].resize(layers[k].contours.size());
			for (int i = 0; i < layers[k].contours.size(); i++) {
				new_layers_contours[k][i].resize(layers[k].contours[i].size());
				for (int j = 0; j < layers[k].contours[i].size(); j++){
					new_layers_contours[k][i][j].x = starting_point((j + start_index) * 2) * max_unit + min_x;
					new_layers_contours[k][i][j].y = starting_point((j + start_index) * 2 + 1) * max_unit + min_y;
				}
				start_index += layers[k].contours[i].size();
			}
		}
		// reset contours
		for (int k = 0; k < layers.size(); k++){
			layers[k].contours = new_layers_contours[k];
		}
	}
	catch (char* ex) {
		std::cout << ex << std::endl;
	}
	catch (std::exception& ex) {
		std::cout << ex.what() << std::endl;
	}
	catch (...) {
		std::cout << "BFGS optimization failure." << std::endl;
	}

}

bool ShapeFitLayersInter::validRAorParallel(const std::vector<cv::Point2f>& polygon, bool bUseRaOpt, int ra_angle_threshold, bool bUseParallelOpt, int parallel_angle_threshold){
	int total_segments = polygon.size();
	if (bUseRaOpt){
		for (int i = 0; i < total_segments; i++){
			int first_start = i;
			int first_end = (i + 1) % total_segments;
			int second_start = (i + 1) % total_segments;
			int second_end = (i + 2) % total_segments;

			// init angles
			cv::Point2f a_init = cv::Point2f(polygon[first_start].x, polygon[first_start].y);
			cv::Point2f b_init = cv::Point2f(polygon[first_end].x, polygon[first_end].y);
			cv::Point2f c_init = cv::Point2f(polygon[second_start].x, polygon[second_start].y);;
			cv::Point2f d_init = cv::Point2f(polygon[second_end].x, polygon[second_end].y);
			float angle_init = util::lineLineAngle(a_init, b_init, c_init, d_init);
			//std::cout << i << " angle is " << angle_init << std::endl;
			// check
			if (abs(angle_init - 45) <= ra_angle_threshold || abs(angle_init - 90) <= ra_angle_threshold || abs(angle_init - 135) <= ra_angle_threshold){
				return true;
			}
		}
		return false;
	}
	if (bUseParallelOpt){
		for (int i = 0; i < total_segments - 1; i++){
			for (int j = i + 1; j < total_segments; j++){
				int first_start = i;
				int first_end = (i + 1) % total_segments;
				int second_start = j;
				int second_end = (j + 1) % total_segments;

				// init angles
				cv::Point2f a_init = cv::Point2f(polygon[first_start].x, polygon[first_start].y);
				cv::Point2f b_init = cv::Point2f(polygon[first_end].x, polygon[first_end].y);
				cv::Point2f c_init = cv::Point2f(polygon[second_start].x, polygon[second_start].y);;
				cv::Point2f d_init = cv::Point2f(polygon[second_end].x, polygon[second_end].y);
				float angle_init = util::lineLineAngle(a_init, b_init, c_init, d_init);
				//std::cout << i <<" to "<<j <<" angle is " << angle_init << std::endl;
				// check
				if (abs(angle_init) <= parallel_angle_threshold || abs(angle_init - 180) <= parallel_angle_threshold)
					return true;
			}
		}
		return false;
	}
}