#include <QWidget>
#include <qmath.h>
#include "Tree3d.h"
#include "Point3d.h"
#include <vector>
#include "mainglwidget.h"
#include "Matrix.h"
#include <qlabel.h>
#include <list>
#include <QLineEdit>
#include <fstream>
#include <sstream>

#include <chrono>

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
	QLabel* labelFitting;
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
	QLineEdit *rSmoothing;
	QLineEdit *rThinning;

private slots:
	void loadPoints();
	void rangeQuery();
	void radiusQuery();
	void smoothPointCloud();
	void nnQuery();
	void colorPointsByDistance();
	void applyThinning();
	void fitLine();
	void fitPlane();
	void fitSphere();
	
	inline void setColor(float color)
	{
		for (int i = 0; i < glWidget->count; i++)
			glWidget->colors[i] = color;
	}

	inline void setColor(float red, float green, float blue)
	{
		for (int i = 0; i < glWidget->count / 3; i++)
		{
			glWidget->colors[i * 3] = red;
			glWidget->colors[i * 3 + 1] = green;
			glWidget->colors[i * 3 + 2] = blue;
		}
	}

	inline void setColor(std::vector<int>::iterator begin, std::vector<int>::iterator end, float red, float green, float blue)
	{
		for (; begin != end; ++begin)
		{
			glWidget->colors[*begin * 3] = red;
			glWidget->colors[*begin * 3 + 1] = green;
			glWidget->colors[*begin * 3 + 2] = blue;
		}
	}
	
	inline void setColor(int index, float red, float green, float blue)
	{
		glWidget->colors[index * 3] = red;
		glWidget->colors[index * 3 + 1] = green;
		glWidget->colors[index * 3 + 2] = blue;
	}

	inline void stopTimer(std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1)
	{
		auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
		std::chrono::duration<double> t = t2 - t1;
		std::stringstream sStream;
		sStream.precision(5);
		sStream << t.count() << " seconds";
		labelTime->setText(QString(sStream.str().c_str()));
	}
};

