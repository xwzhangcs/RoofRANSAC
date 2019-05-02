#include "Config.h"

Config::Config() {

}
Config::Config(bool bUseIntra, bool bUseInter, bool bUseRaOpt, float angle_threshold_RA, float raWeight, bool bUseParallelOpt, float angle_threshold_parallel, float parallelWeight,
	bool bUseSymmetryLineOpt, float symmetryWeight, bool bUseAccuracyOpt, float accuracyWeight, bool bUsePointSnapOpt, float pointDisThreshold, float pointWeight, bool bUseSegSnapOpt, float segDisThreshold, float segAngleThreshold, float segWeight){
	this->bUseIntra = bUseIntra;
	this->bUseInter = bUseInter;
	this->bUseRaOpt = bUseRaOpt;
	this->angle_threshold_RA = angle_threshold_RA;
	this->raWeight = raWeight;
	this->bUseParallelOpt = bUseParallelOpt;
	this->angle_threshold_parallel = angle_threshold_parallel;
	this->parallelWeight = parallelWeight;
	this->bUseSymmetryLineOpt = bUseSymmetryLineOpt;
	this->symmetryWeight = symmetryWeight;
	this->bUseAccuracyOpt = bUseAccuracyOpt;
	this->accuracyWeight = accuracyWeight;
	this->bUsePointSnapOpt = bUsePointSnapOpt;
	this->pointDisThreshold = pointDisThreshold;
	this->pointWeight = pointWeight;
	this->bUseSegSnapOpt = bUseSegSnapOpt;
	this->segDisThreshold = segDisThreshold;
	this->segAngleThreshold = segAngleThreshold;
	this->segWeight = segWeight;
}

Config::Config(bool bUseIntra, float intraWeight, bool bUseInter, float interWeight, bool bUseRaOpt, float angle_threshold_RA, float raWeight, bool bUseParallelOpt, float angle_threshold_parallel, float parallelWeight,
	bool bUseSymmetryLineOpt, float symmetryWeight, bool bUseAccuracyOpt, float accuracyWeight, bool bUsePointSnapOpt, float pointDisThreshold, float pointWeight, bool bUseSegSnapOpt, float segDisThreshold, float segAngleThreshold, float segWeight){
	this->bUseIntra = bUseIntra;
	this->intraWeight = intraWeight;
	this->bUseInter = bUseInter;
	this->interWeight = interWeight;
	this->bUseRaOpt = bUseRaOpt;
	this->angle_threshold_RA = angle_threshold_RA;
	this->raWeight = raWeight;
	this->bUseParallelOpt = bUseParallelOpt;
	this->angle_threshold_parallel = angle_threshold_parallel;
	this->parallelWeight = parallelWeight;
	this->bUseSymmetryLineOpt = bUseSymmetryLineOpt;
	this->symmetryWeight = symmetryWeight;
	this->bUseAccuracyOpt = bUseAccuracyOpt;
	this->accuracyWeight = accuracyWeight;
	this->bUsePointSnapOpt = bUsePointSnapOpt;
	this->pointDisThreshold = pointDisThreshold;
	this->pointWeight = pointWeight;
	this->bUseSegSnapOpt = bUseSegSnapOpt;
	this->segDisThreshold = segDisThreshold;
	this->segAngleThreshold = segAngleThreshold;
	this->segWeight = segWeight;
}