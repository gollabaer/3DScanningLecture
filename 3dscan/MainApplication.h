#include <QWidget>
#include <qmath.h>
#include "kdTree.h"
#include <vector>
#include "mainglwidget.h"
#include <qlabel.h>

#pragma once

class MainApplication : public QWidget
{
	Q_OBJECT

public:
	//MainApplication();
	MainApplication(QWidget *parent = 0);
	~MainApplication();

private:
	kdTree _kdTree;
	std::vector<float> points;
	MainGLWidget * glWidget;
	QLabel* labelCloudBounds;
	

private slots:
	void loadPoints();
	void rangeQuery();
	void nnQuery();
};

