#include <QWidget>
#include <qmath.h>
#include "Tree3d.h"
#include "Point3d.h"
#include <vector>
#include "mainglwidget.h"
#include <qlabel.h>
#include <list>

#pragma once

class MainApplication : public QWidget
{
	Q_OBJECT

public:
	//MainApplication();
	MainApplication(QWidget *parent = 0);
	~MainApplication();

private:
	std::list<Tree3d> trees;
	Tree3d _Tree3d;
	std::vector<Point3d> points;
	MainGLWidget * glWidget;
	QLabel* labelCloudBounds;
	QLabel* labelTime;
	QLabel* labelPoints;

private slots:
	void loadPoints();
	void rangeQuery();
	void radiusQuery();
	void smoothPointCloud();
	void nnQuery();
	void colorPointsByDistance();
	void applyThinning();
};

