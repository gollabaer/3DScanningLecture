#include "MainApplication.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include<qfiledialog.h>

#include <sstream>
#include <string>
#include <fstream>

std::vector<float> xyzFileToVec(std::string source){


	std::vector<float> vec;
	vec.reserve(9000000);
	std::fstream fs;
	fs.open(source.c_str(), std::ios::in);
	ulong c = 0;

	for (std::string line; std::getline(fs, line);)
	{
		std::istringstream in(line);
		float x, y, z;
		in >> x >> y >> z;

		vec.push_back(x);
		vec.push_back(y);
		vec.push_back(z);
		c++;
	}

	return vec;
}


MainApplication::MainApplication(QWidget *parent) : QWidget(parent)
{
	QPushButton *quit = new QPushButton(tr("Quit"));
	quit->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *load = new QPushButton(tr("Load"));
	load->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *rangequery = new QPushButton(tr("Range"));
	rangequery->setFont(QFont("Times", 12, QFont::AnyStyle));

	glWidget = new MainGLWidget();
	glWidget->vertices = {0};
	glWidget->colors = {0};
	glWidget->count = 0;

	glWidget->resize(640, 380);

	QHBoxLayout *layoutMain = new QHBoxLayout();

	QVBoxLayout *layoutButtons = new QVBoxLayout();

	connect(quit, SIGNAL(clicked()), this, SLOT(quit()));
	connect(load, SIGNAL(clicked()), this, SLOT(loadPoints()));

	layoutButtons->addWidget(load);
	layoutButtons->addWidget(rangequery);
	layoutButtons->addWidget(quit);

	QWidget* buttonWidget = new QWidget();
	buttonWidget->setLayout(layoutButtons);
	buttonWidget->setFixedWidth(120);
	glWidget->setMinimumWidth(640);
	glWidget->setMinimumHeight(380);
	layoutMain->addWidget(glWidget);
	layoutMain->addWidget(buttonWidget);

	setLayout(layoutMain);
}

MainApplication::~MainApplication()
{
}

void MainApplication::loadPoints(){

	QString fstr = QFileDialog::getOpenFileName(this, tr("Open File"), "c:/", tr("Point Files (*.xyz)"));
	
	if (!(fstr != NULL && !fstr.isEmpty()))return;

	std::string str = fstr.toStdString();

	this->points = xyzFileToVec(str.c_str());
	
	
	this->glWidget->colors = new GLfloat[points.size()];
	for (int i = 0; i < points.size(); i++)
		this->glWidget->colors[i] = (1.0f);
	this->glWidget->cam.init(points, 640, 380);
	this->glWidget->vertices = points.data();
	this->glWidget->count = points.size();
}