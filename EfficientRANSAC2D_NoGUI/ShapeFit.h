#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <dlib/optimization.h>
#include "Util.h"

class ShapeFit {
		
	typedef dlib::matrix<double, 0, 1> column_vector;

	class BFGSSolver {
	private:
		std::vector<cv::Point2f> target_polygon;
		std::vector<cv::Point2f> init_polygon;
		bool bUseRaOpt;
		float angle_threshold_RA;
		float raWeight;
		bool bUseParallelOpt;
		float angle_threshold_parallel;
		float parallelWeight;
		bool bUseSymmetryLineOpt;
		std::vector<cv::Point2f> symmetry_line;
		float symmetryWeight;
		bool bUseAccuracy;
		float accuracyWeight;

	public:
		BFGSSolver(const std::vector<cv::Point2f>& target_polygon, const std::vector<cv::Point2f>& init_polygon, bool bUseRaOpt, float angle_threshold_RA, float raWeight, bool bUseParallelOpt, float angle_threshold_parallel, float parallelWeight, bool bUseSymmetryLineOpt, const std::vector<cv::Point2f>& symmetry_line, float symmetryWeight, bool bUseAccuracyOpt, float accuracyWeight) {
			this->target_polygon = target_polygon;
			this->init_polygon = init_polygon;
			this->bUseRaOpt = bUseRaOpt;
			this->angle_threshold_RA = angle_threshold_RA;
			this->raWeight = raWeight;
			this->bUseParallelOpt = bUseParallelOpt;
			this->angle_threshold_parallel = angle_threshold_parallel;
			this->parallelWeight = parallelWeight;
			this->bUseSymmetryLineOpt = bUseSymmetryLineOpt;
			this->symmetry_line = symmetry_line;
			this->symmetryWeight = symmetryWeight;
			this->bUseAccuracy = bUseAccuracyOpt;
			this->accuracyWeight = accuracyWeight;
			
		}

		double operator() (const column_vector& arg) const {
			std::vector<cv::Point2f> polygon;
			for (int i = 0; i < arg.size() / 2; i++) {
				polygon.push_back(cv::Point2f(arg(i * 2), arg(i * 2 + 1)));
			}

			try {		
				float score = 0.0f;
				// RA opt function
				if (bUseRaOpt){
					//std::cout << "use RA opt " << angle_threshold_RA << ", "<< raWeight<< std::endl;
					score += util::calculateScore(polygon, init_polygon, angle_threshold_RA) * raWeight;
					//return 0.5 * util::calculateScore(polygon, init_polygon, angle_threshold_RA) + 0.5 * util::calculateIOU(polygon, target_polygon);
				}
				// parallel opt function
				if (bUseParallelOpt){
					std::cout << "use Parallel opt " <<angle_threshold_parallel <<", "<<parallelWeight << std::endl;
					score += util::calculateAllScore(polygon, init_polygon, angle_threshold_parallel) * parallelWeight;
				}
				// symmetry opt function
				if (bUseSymmetryLineOpt){
					std::vector<cv::Point2f> polygon_symmetry;
					cv::Point2f a = symmetry_line[0];
					cv::Point2f b = symmetry_line[1];
					for (int i = 0; i < polygon.size(); i++) {
						polygon_symmetry.push_back(util::mirrorPoint(a, b, polygon[i]));
					}
					float iou = 0.0f;
					if (!util::isSimple(polygon) || !util::isSimple(polygon_symmetry)){
						std::cout << "image method" << std::endl;
						iou = util::calculateIOUbyImage(polygon, polygon_symmetry, 1000);
					}
					else{
						iou = util::calculateIOU(polygon, polygon_symmetry);
						std::cout << "cgal method" << std::endl;
					}
					std::cout << "During OPT, IOU is " << iou << std::endl;
					score += iou * symmetryWeight;

				}
				// accuracy function
				if(bUseAccuracy){
					std::cout << "use accuracy opt " << accuracyWeight<<std::endl;
					if (!util::isSimple(polygon) || !util::isSimple(target_polygon)){
						std::cout << "image method" << std::endl;
						score += util::calculateIOUbyImage(polygon, target_polygon, 1000) * accuracyWeight;
					}
					else{
						std::cout << "cgal method" << std::endl;
						score += util::calculateIOU(polygon, target_polygon) * accuracyWeight;
					}

				}
				return score;
			}
			catch (...) {
				std::cout << "exception" << std::endl;
				return 0;
			}
		}
	};

protected:
	ShapeFit();
	~ShapeFit();

public:
	static std::vector<cv::Point2f> fit(const std::vector<cv::Point2f>& target_polygon, const std::vector<cv::Point2f>& ini_points, bool bUseRaOpt, float angle_threshold_RA, float raWeight, bool bUseParallelOpt, float angle_threshold_parallel, float parallelWeight, bool bValidSymmetryLine, const std::vector<cv::Point2f>& symmetry_line, float symmetryWeight, bool bUseAccuracyOpt, float accuracyWeight);
	static bool validRAorParallel(const std::vector<cv::Point2f>& polygon, bool bUseRaOpt, int ra_angle_threshold, bool bUseParallelOpt, int parallel_angle_threshold);
};

