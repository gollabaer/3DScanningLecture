#include "MainApplication.h"
#include <mainglwidget.h>
#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>

/*
MainApplication::MainApplication()
{
}
*/

MainApplication::MainApplication(QWidget *parent) : QWidget(parent)
{
	QPushButton *quit = new QPushButton(tr("Quit"));
	quit->setFont(QFont("Times", 18, QFont::Bold));

	QSlider *slider = new QSlider(Qt::Horizontal);
	slider->setRange(0, 99);
	slider->setValue(0);

	MainGLWidget *gl = new MainGLWidget();
	
	std::vector<float> vertices;
	vertices.clear();
	vertices.push_back(10);
	vertices.push_back(10);
	vertices.push_back(10);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(0);
	vertices.push_back(-10);
	vertices.push_back(-10);
	vertices.push_back(-10);
	gl->count = vertices.size();
	gl->vertices = vertices.data();
	gl->model.setToIdentity();
	gl->projection.perspective(60.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	gl->view.lookAt(QVector3D(0, 0, 0) + QVector3D(0, 0, -2), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
	gl->center = QVector3D(0, 0, 0);
	gl->colors = new GLfloat[gl->count];
	gl->colors[0] = 1;
	gl->colors[1] = 0;
	gl->colors[2] = 0;
	gl->colors[3] = 1;
	gl->colors[4] = 0;
	gl->colors[5] = 0;
	gl->colors[6] = 1;
	gl->colors[7] = 0;
	gl->colors[8] = 0;
	gl->resize(50, 50);

	this->resize(500, 400);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(quit);
	layout->addWidget(gl);
	layout->addWidget(slider);
	setLayout(layout);
}

MainApplication::~MainApplication()
{
}

