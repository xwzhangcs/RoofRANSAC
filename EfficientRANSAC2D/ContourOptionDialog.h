#pragma once

#include <QDialog>
#include "ui_ContourOptionDialog.h"

class ContourOptionDialog : public QDialog {
	Q_OBJECT

private:
	Ui::ContourOptionDialog ui;

public:
	ContourOptionDialog(QWidget *parent = Q_NULLPTR);
	~ContourOptionDialog();

	int getCurveNumIterations();
	int getCurveMinPoints();
	float getCurveMaxErrorRatioToRadius();
	float getCurveClusterEpsilon();
	float getCurveMinAngle();
	float getCurveMinRadius();
	float getCurveMaxRadius();

	int getLineNumIterations();
	int getLineMinPoints();
	float getLineMaxError();
	float getLineClusterEpsilon();
	float getLineMinLength();
	float getLineAngleThreshold();

	float getContourMaxError();
	float getContourAngleThreshold();

	float getIOUThreshold();
	bool getUseSymmetryLineOpt();
	float getSymmetryWeight();

	bool getUseRaOpt();
	float getRaThreshold();
	float getRaWeight();

	bool getUseParallelOpt();
	float getParallelThreshold();
	float getParallelWeight();

	bool getUseAccuracyOpt();
	float getAccuracyWeight();

public slots:
	void onOK();
	void onCancel();
	void onUseSymmetryLineOpt();
	void onUseRaOpt();
	void onUseParallelOpt();
	void onUseAccuracyOpt();
};
