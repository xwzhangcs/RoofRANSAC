#include "ContourOptionDialog.h"

ContourOptionDialog::ContourOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditCurveNumIterations->setText("200000");
	ui.lineEditCurveMinPoints->setText("200");
	ui.lineEditCurveMaxErrorRatioToRadius->setText("0.02");
	ui.lineEditCurveClusterEpsilon->setText("30");
	ui.lineEditCurveMinAngle->setText("90");
	ui.lineEditCurveMinRadius->setText("80");
	ui.lineEditCurveMaxRadius->setText("400");

	ui.lineEditLineNumIterations->setText("20000");
	ui.lineEditLineMinPoints->setText("30");
	ui.lineEditLineMaxError->setText("5");
	ui.lineEditLineClusterEpsilon->setText("20");
	ui.lineEditLineMinLength->setText("30");
	ui.lineEditLineAngleThreshold->setText("15");

	ui.lineEditContourMaxError->setText("20");
	ui.lineEditContourAngleThreshold->setText("10");
	ui.checkBoxSymmetryLine->setChecked(false);
	ui.lineEditIOUThreshold->setText("90");
	ui.lineEditSymmetryWeight->setText("0.25");

	ui.checkBoxRA->setChecked(false);
	ui.lineEditRAthreshold->setText("10");
	ui.lineEditRaWeight->setText("0.25");

	ui.checkBoxParallel->setChecked(false);
	ui.lineEditParallelThreshold->setText("10");
	ui.lineEditParallelWeight->setText("0.25");

	ui.checkBoxAccuracy->setChecked(false);
	ui.lineEditAccuracyWeight->setText("0.25");

	connect(ui.checkBoxSymmetryLine, SIGNAL(clicked()), this, SLOT(onUseSymmetryLineOpt()));
	connect(ui.checkBoxRA, SIGNAL(clicked()), this, SLOT(onUseRaOpt()));
	connect(ui.checkBoxParallel, SIGNAL(clicked()), this, SLOT(onUseParallelOpt()));
	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));
	connect(ui.checkBoxAccuracy, SIGNAL(clicked()), this, SLOT(onUseAccuracyOpt()));

	onUseSymmetryLineOpt();
	onUseRaOpt();
	onUseParallelOpt();
	onUseAccuracyOpt();
}

ContourOptionDialog::~ContourOptionDialog() {
}

int ContourOptionDialog::getCurveNumIterations() {
	return ui.lineEditCurveNumIterations->text().toInt();
}

int ContourOptionDialog::getCurveMinPoints() {
	return ui.lineEditCurveMinPoints->text().toInt();
}

float ContourOptionDialog::getCurveMaxErrorRatioToRadius() {
	return ui.lineEditCurveMaxErrorRatioToRadius->text().toFloat();
}

float ContourOptionDialog::getCurveClusterEpsilon() {
	return ui.lineEditCurveClusterEpsilon->text().toFloat();
}

float ContourOptionDialog::getCurveMinAngle() {
	return ui.lineEditCurveMinAngle->text().toFloat();
}

float ContourOptionDialog::getCurveMinRadius() {
	return ui.lineEditCurveMinRadius->text().toFloat();
}

float ContourOptionDialog::getCurveMaxRadius() {
	return ui.lineEditCurveMaxRadius->text().toFloat();
}

int ContourOptionDialog::getLineNumIterations() {
	return ui.lineEditLineNumIterations->text().toInt();
}

int ContourOptionDialog::getLineMinPoints() {
	return ui.lineEditLineMinPoints->text().toInt();
}

float ContourOptionDialog::getLineMaxError() {
	return ui.lineEditLineMaxError->text().toFloat();
}

float ContourOptionDialog::getLineClusterEpsilon() {
	return ui.lineEditLineClusterEpsilon->text().toFloat();
}

float ContourOptionDialog::getLineMinLength() {
	return ui.lineEditLineMinLength->text().toFloat();
}

float ContourOptionDialog::getLineAngleThreshold() {
	return ui.lineEditLineAngleThreshold->text().toFloat();
}

float ContourOptionDialog::getContourMaxError() {
	return ui.lineEditContourMaxError->text().toFloat();
}

float ContourOptionDialog::getContourAngleThreshold() {
	return ui.lineEditContourAngleThreshold->text().toFloat();
}

float ContourOptionDialog::getIOUThreshold(){
	return ui.lineEditIOUThreshold->text().toFloat();
}

bool ContourOptionDialog::getUseSymmetryLineOpt(){
	return ui.checkBoxSymmetryLine->isChecked();
}

float ContourOptionDialog::getSymmetryWeight(){
	return ui.lineEditSymmetryWeight->text().toFloat();
}

bool ContourOptionDialog::getUseRaOpt(){
	return ui.checkBoxRA->isChecked();
}

float ContourOptionDialog::getRaThreshold(){
	return ui.lineEditRAthreshold->text().toFloat();
}

float ContourOptionDialog::getRaWeight(){
	return ui.lineEditRaWeight->text().toFloat();
}


bool ContourOptionDialog::getUseParallelOpt(){
	return ui.checkBoxParallel->isChecked();
}

float ContourOptionDialog::getParallelThreshold(){
	return ui.lineEditParallelThreshold->text().toFloat();
}

float ContourOptionDialog::getParallelWeight(){
	return ui.lineEditParallelWeight->text().toFloat();
}

bool ContourOptionDialog::getUseAccuracyOpt(){
	return ui.checkBoxAccuracy->isChecked();
}

float ContourOptionDialog::getAccuracyWeight(){
	return ui.lineEditAccuracyWeight->text().toFloat();
}

void ContourOptionDialog::onUseSymmetryLineOpt() {
	ui.lineEditIOUThreshold->setEnabled(ui.checkBoxSymmetryLine->isChecked());
	ui.lineEditSymmetryWeight->setEnabled(ui.checkBoxSymmetryLine->isChecked());
}

void ContourOptionDialog::onUseRaOpt(){
	ui.lineEditRAthreshold->setEnabled(ui.checkBoxRA->isChecked());
	ui.lineEditRaWeight->setEnabled(ui.checkBoxRA->isChecked());
}

void ContourOptionDialog::onUseParallelOpt(){
	ui.lineEditParallelThreshold->setEnabled(ui.checkBoxParallel->isChecked());
	ui.lineEditParallelWeight->setEnabled(ui.checkBoxParallel->isChecked());
}

void ContourOptionDialog::onUseAccuracyOpt(){
	ui.lineEditAccuracyWeight->setEnabled(ui.checkBoxAccuracy->isChecked());
}

void ContourOptionDialog::onOK() {
	accept();
}

void ContourOptionDialog::onCancel() {
	reject();
}
