#include <QWidget>
#include <qmath.h>
#include "Tree3d.h"
#include "Point3d.h"
#include <vector>
#include "mainglwidget.h"
#include <qlabel.h>
#include <list>
#include <QLineEdit>

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
	QLineEdit *minXRange;
	QLineEdit *maxXRange;
	QLineEdit *minYRange;
	QLineEdit *maxYRange;
	QLineEdit *minZRange;
	QLineEdit *maxZRange;
	QLineEdit *xRadius;
	QLineEdit *yRadius;
	QLineEdit *zRadius;
	QLineEdit *rRadius;
	QLineEdit *xNeighbour;
	QLineEdit *yNeighbour;
	QLineEdit *zNeighbour;

private slots:
	void loadPoints();
	void rangeQuery();
	void radiusQuery();
	void smoothPointCloud();
	void nnQuery();
	void colorPointsByDistance();
	void applyThinning();
};

