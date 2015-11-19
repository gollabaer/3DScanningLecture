#include <QWidget>
#include <qmath.h>
#include "Tree3d.h"
#include "Point3d.h"
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
	Tree3d _Tree3d;
	std::vector<Point3d> points;
	MainGLWidget * glWidget;
	QLabel* labelCloudBounds;
	

private slots:
	void loadPoints();
	void rangeQuery();
	void nnQuery();
};

