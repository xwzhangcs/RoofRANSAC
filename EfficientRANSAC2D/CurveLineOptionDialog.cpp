#include "CurveLineOptionDialog.h"

CurveLineOptionDialog::CurveLineOptionDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);

	ui.lineEditCurveNumIterations->setText("200000");
	ui.lineEditCurveMinPoints->setText("200");
	ui.lineEditCurveMaxErrorRatioToRadius->setText("0.02");
	ui.lineEditCurveClusterEpsilon->setText("30");
	ui.lineEditCurveMinAngle->setText("90");
	ui.lineEditCurveMinRadius->setText("80");
	ui.lineEditCurveMaxRadius->setText("400");

	ui.lineEditLineNumIterations->setText("20000");
	ui.lineEditLineMinPoints->setText("10");
	ui.lineEditLineMaxError->setText("2");
	ui.lineEditLineClusterEpsilon->setText("8");
	ui.lineEditLineMinLength->setText("15");
	ui.lineEditAngleThreshold->setText("15");

	connect(ui.pushButtonOK, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(ui.pushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

}

CurveLineOptionDialog::~CurveLineOptionDialog() {
}

int CurveLineOptionDialog::getCurveNumIterations() {
	return ui.lineEditCurveNumIterations->text().toFloat();
}

int CurveLineOptionDialog::getCurveMinPoints() {
	return ui.lineEditCurveMinPoints->text().toFloat();
}

float CurveLineOptionDialog::getCurveMaxErrorRatioToRadius() {
	return ui.lineEditCurveMaxErrorRatioToRadius->text().toFloat();
}

float CurveLineOptionDialog::getCurveClusterEpsilon() {
	return ui.lineEditCurveClusterEpsilon->text().toFloat();
}

float CurveLineOptionDialog::getCurveMinAngle() {
	return ui.lineEditCurveMinAngle->text().toFloat();
}

float CurveLineOptionDialog::getCurveMinRadius() {
	return ui.lineEditCurveMinRadius->text().toFloat();
}

float CurveLineOptionDialog::getCurveMaxRadius() {
	return ui.lineEditCurveMaxRadius->text().toFloat();
}

int CurveLineOptionDialog::getLineNumIterations() {
	return ui.lineEditLineNumIterations->text().toFloat();
}

int CurveLineOptionDialog::getLineMinPoints() {
	return ui.lineEditLineMinPoints->text().toFloat();
}

float CurveLineOptionDialog::getLineMaxError() {
	return ui.lineEditLineMaxError->text().toFloat();
}

float CurveLineOptionDialog::getLineClusterEpsilon() {
	return ui.lineEditLineClusterEpsilon->text().toFloat();
}

float CurveLineOptionDialog::getLineMinLength() {
	return ui.lineEditLineMinLength->text().toFloat();
}

float CurveLineOptionDialog::getAngleThreshold() {
	return ui.lineEditAngleThreshold->text().toFloat();
}

void CurveLineOptionDialog::onOK() {
	accept();
}

void CurveLineOptionDialog::onCancel() {
	reject();
}
