#include "MainWindow.h"
#include <QFileDialog>
#include "CurveOptionDialog.h"
#include "LineOptionDialog.h"
#include "CurveLineOptionDialog.h"
#include "ContourOptionDialog.h"
#include <QTextStream>
#include "../EfficientRANSAC2D_NoGUI/Regularizer.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/document.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/writer.h"
#include "../EfficientRANSAC2D_NoGUI/rapidjson/stringbuffer.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	setCentralWidget(&canvas);
	
	// create tool bar for file menu
	ui.mainToolBar->addAction(ui.actionOpen);
	ui.mainToolBar->addAction(ui.actionSave);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addAction(ui.actionGenerateContours);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(onSave()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionDetectCurves, SIGNAL(triggered()), this, SLOT(onDetectCurves()));
	connect(ui.actionDetectLines, SIGNAL(triggered()), this, SLOT(onDetectLines()));
	connect(ui.actionDetectCurvesLines, SIGNAL(triggered()), this, SLOT(onDetectCurvesLines()));
	connect(ui.actionGenerateContours, SIGNAL(triggered()), this, SLOT(onGenerateContours()));
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load image..."), "", tr("Image files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;

	setWindowTitle("Efficient RANSAC 2D - " + filename);
	
	canvas.loadImage(filename);
	canvas.update();
}

void MainWindow::onSave() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save image..."), "", tr("Image files (*.png *.jpg *.bmp)"));
	if (filename.isEmpty()) return;

	canvas.saveImage(filename);
	canvas.update();
}

void MainWindow::onDetectCurves() {
	CurveOptionDialog dlg;
	if (dlg.exec()) {
		canvas.detectCurves(dlg.getNumIterations(), dlg.getMinPoints(), dlg.getMaxErrorRatioToRadius(), dlg.getClusterEpsilon(), dlg.getMinAngle() / 180.0 * CV_PI, dlg.getMinRadius(), dlg.getMaxRadius());
		canvas.update();
	}
}

void MainWindow::onDetectLines() {
	LineOptionDialog dlg;
	if (dlg.exec()) {
		canvas.detectLines(dlg.getNumIterations(), dlg.getMinPoints(), dlg.getMaxError(), dlg.getClusterEpsilon(), dlg.getMinLength(), dlg.getAngleThreshold() / 180.0 * CV_PI);
		canvas.update();
	}
}

void MainWindow::onDetectCurvesLines() {
	CurveLineOptionDialog dlg;
	if (dlg.exec()) {
		canvas.detectCurvesLines(dlg.getCurveNumIterations(), dlg.getCurveMinPoints(), dlg.getCurveMaxErrorRatioToRadius(), dlg.getCurveClusterEpsilon(), dlg.getCurveMinAngle() / 180.0 * CV_PI, dlg.getCurveMinRadius(), dlg.getCurveMaxRadius(), dlg.getLineNumIterations(), dlg.getLineMinPoints(), dlg.getLineMaxError(), dlg.getLineClusterEpsilon(), dlg.getLineMinLength(), dlg.getAngleThreshold() / 180.0 * CV_PI);
		canvas.update();
	}
}

void MainWindow::onGenerateContours() {
	ContourOptionDialog dlg;
	if (dlg.exec()) {
		//check the sum weight equals 1
		float weight = 0.0f;
		if (dlg.getUseSymmetryLineOpt())
			weight += dlg.getSymmetryWeight();
		if (dlg.getUseRaOpt())
			weight += dlg.getRaWeight();
		if (dlg.getUseParallelOpt())
			weight += dlg.getParallelWeight();
		if (dlg.getUseAccuracyOpt())
			weight += dlg.getAccuracyWeight();
		if (abs(weight - 1.0f) < 0.0001)
		{
			std::cout << "Success!!" << std::endl;
		}
		else{
			std::cout << "Please check weight assignment!!!" << std::endl;
			return;
		}
		canvas.generateContours(dlg.getCurveNumIterations(), dlg.getCurveMinPoints(), dlg.getCurveMaxErrorRatioToRadius(), dlg.getCurveClusterEpsilon(), dlg.getCurveMinAngle() / 180.0 * CV_PI, dlg.getCurveMinRadius(), dlg.getCurveMaxRadius(), dlg.getLineNumIterations(), dlg.getLineMinPoints(), dlg.getLineMaxError(), dlg.getLineClusterEpsilon(), dlg.getLineMinLength(), dlg.getLineAngleThreshold() / 180.0 * CV_PI, dlg.getContourMaxError(), dlg.getContourAngleThreshold() / 180.0 * CV_PI, dlg.getUseSymmetryLineOpt(), dlg.getIOUThreshold(), dlg.getSymmetryWeight(), dlg.getUseRaOpt(), dlg.getRaThreshold(), dlg.getRaWeight(), dlg.getUseParallelOpt(), dlg.getParallelThreshold(), dlg.getParallelWeight(), dlg.getUseAccuracyOpt(), dlg.getAccuracyWeight());
		canvas.update();
	}
}

QString readStringValue(const rapidjson::Value& node, const char* key) {
	if (node.HasMember(key) && node[key].IsString()) {
		return QString(node[key].GetString());
	}
	else {
		throw "Could not read string from node";
	}
}