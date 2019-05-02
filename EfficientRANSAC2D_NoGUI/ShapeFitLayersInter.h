#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <dlib/optimization.h>
#include "Util.h"
#include "Layer.h"
#include "Config.h"

class ShapeFitLayersInter {
		
	typedef dlib::matrix<double, 0, 1> column_vector;
	typedef std::vector<std::vector<cv::Point2f>> layer_polygons;

	class BFGSSolver {
	private:
		std::vector<layer_polygons> target_layers_polygons;
		std::vector<layer_polygons> init_layers_polygons;
		std::vector<layer_polygons> layers_symmetry_lines;
		std::vector<std::pair<float, float>> layers_height;
		std::vector<std::pair<std::vector<int>, std::vector<int>>> tree_info;
		Config config;

	public:
		BFGSSolver(const std::vector<layer_polygons>& target_layers_polygons, const std::vector<layer_polygons>& init_layers_polygons, const std::vector<layer_polygons>& layers_symmetry_lines, const std::vector<std::pair<float, float>>& layers_height, const std::vector<std::pair<std::vector<int>, std::vector<int>>> tree_info, Config& config) {
			this->target_layers_polygons = target_layers_polygons;
			this->init_layers_polygons = init_layers_polygons;
			this->layers_symmetry_lines = layers_symmetry_lines;
			this->tree_info = tree_info;
			this->config = config;
		}

		double operator() (const column_vector& arg) const {
			std::vector<layer_polygons> polygons;
			polygons.resize(init_layers_polygons.size());
			int start_index = 0;
			for (int k = 0; k < init_layers_polygons.size(); k++){
				polygons[k].resize(init_layers_polygons[k].size());
				for (int i = 0; i < init_layers_polygons[k].size(); i++){
					for (int j = 0; j < init_layers_polygons[k][i].size(); j++) {
						polygons[k][i].push_back(cv::Point2f(arg((j + start_index) * 2), arg((j + start_index) * 2 + 1)));
					}
					start_index += init_layers_polygons[k][i].size();
				}
			}
			try {
				float score = 0.0f;
				int valid_polygons = 0;
				for (int k = 0; k < init_layers_polygons.size(); k++){
					for (int i = 0; i < init_layers_polygons[k].size(); i++){
						if (init_layers_polygons[k][i].size() != 0){
							float score_point = 0.0f;
							float score_seg = 0.0f;
							// optimization score for one polygon
							if (config.bUsePointSnapOpt){
								int valid_num = 0;
								std::cout << "Point Snap" << std::endl;
								/*for (int j = 0; j < tree_info[k].second.size(); j++){
									std::cout << "children nodes" << std::endl;
									float score_tmp = util::calculateScorePointOpt(polygons[k][i], init_layers_polygons[k][i], polygons[tree_info[k].second[j]], init_layers_polygons[tree_info[k].second[j]], config.pointDisThreshold);
									if (abs(score_tmp) > 0.1){
										valid_num++;
										score_point += score_tmp;
									}
								}*/
								for (int j = 0; j < tree_info[k].first.size(); j++){
									std::cout << "parent nodes" << std::endl;
									float score_tmp = util::calculateScorePointOpt(polygons[k][i], init_layers_polygons[k][i], polygons[tree_info[k].first[j]], init_layers_polygons[tree_info[k].first[j]], config.pointDisThreshold);
									if (abs(score_tmp) > 0.1){
										valid_num++;
										score_point += score_tmp;
									}
								}
								std::cout << "valid_num is " << valid_num << std::endl;
								if (valid_num > 0){
									score_point = score_point / valid_num;
									score_point = score_point * config.pointWeight;
								}
							}
							if (config.bUseSegSnapOpt){
								int valid_num = 0;
								std::cout << "Seg Snap" << std::endl;
								/*for (int j = 0; j < tree_info[k].second.size(); j++){
									float score_tmp = util::calculateScoreSegOpt(polygons[k][i], init_layers_polygons[k][i], polygons[tree_info[k].second[j]], init_layers_polygons[tree_info[k].second[j]], config.segDisThreshold, config.segAngleThreshold);
									if (abs(score_tmp) > 0.1){
										valid_num++;
										score_seg += score_tmp;
									}
								}*/
								for (int j = 0; j < tree_info[k].first.size(); j++){
									float score_tmp = util::calculateScoreSegOpt(polygons[k][i], init_layers_polygons[k][i], polygons[tree_info[k].first[j]], init_layers_polygons[tree_info[k].first[j]], config.segDisThreshold, config.segAngleThreshold);
									if (abs(score_tmp) > 0.1){
										valid_num++;
										score_seg += score_tmp;
									}
								}
								if (valid_num > 0){
									score_seg = score_seg / valid_num;
									score_seg = score_seg * config.segWeight;
								}
							}
							score += score_point + score_seg;
							if (abs(score) > 0.1){
								valid_polygons++;
							}
						}
					}
				}
				if (valid_polygons == 0)
					score = 0.0f;
				else
					score = score / valid_polygons;
				std::cout << "valid_polygons is " << valid_polygons<<std::endl;
				std::cout << "score is " << score << std::endl;
				std::cout << "---------------- " << std::endl;
				return score;
			}
			catch (...) {
				std::cout << "exception" << std::endl;
				return 0;
			}
		}
	};

protected:
	ShapeFitLayersInter();
	~ShapeFitLayersInter();

public:
	static void fit(std::vector<Layer>& layers, QString config_file);
	static bool validRAorParallel(const std::vector<cv::Point2f>& polygon, bool bUseRaOpt, int ra_angle_threshold, bool bUseParallelOpt, int parallel_angle_threshold);
};

