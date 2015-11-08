#include "MainApplication.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include <qfiledialog.h>
#include <qinputdialog.h>

#include <sstream>
#include <string>
#include <fstream>
#include <sstream>

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

	labelCloudBounds = new QLabel("---", this);
	labelCloudBounds->setMaximumHeight(60);

	glWidget->resize(640, 380);

	QHBoxLayout *layoutMain = new QHBoxLayout();

	QVBoxLayout *layoutButtons = new QVBoxLayout();

	connect(quit, SIGNAL(clicked()), this, SLOT(quit()));
	connect(load, SIGNAL(clicked()), this, SLOT(loadPoints()));
	connect(rangequery, SIGNAL(clicked()), this, SLOT(rangeQuery()));

	layoutButtons->addWidget(labelCloudBounds);
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

	labelCloudBounds->setText("Loading...");
	QString fstr = QFileDialog::getOpenFileName(this, tr("Open File"), "c:/", tr("Point Files (*.xyz)"));
	
	if (!(fstr != NULL && !fstr.isEmpty())){
		labelCloudBounds->setText("---");
		return;
	}

	std::string str = fstr.toStdString();

	this->points = xyzFileToVec(str.c_str());
	
	
	this->glWidget->colors = new GLfloat[points.size()];
	for (int i = 0; i < points.size(); i++)
		this->glWidget->colors[i] = (1.0f);
	std::vector<float> bbox = this->glWidget->cam.init(points, 640, 380);

	std::stringstream sStream;
	sStream.precision(2);
	sStream << bbox[0] << ":" << bbox[21] << "\n" << bbox[1] << ":" << bbox[22] << "\n" << bbox[2] << ":" << bbox[23];

	std::string ts = sStream.str();

	labelCloudBounds->setText(QString(ts.c_str()));

	this->glWidget->vertices = points.data();
	this->glWidget->count = points.size();

	this->_kdTree = kdTree(points, 100, 3);
}

void MainApplication::rangeQuery(){
	QString str = QInputDialog::getText(this, "Input points:", "x1 y1 z1 x2 y2 z2");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 6) return;

	QVector3D v1 = QVector3D(0, 0, 0);
	QVector3D v2 = QVector3D(0, 0, 0);

	v1.setX(strList.at(0).toFloat());
	v1.setY(strList.at(1).toFloat());
	v1.setZ(strList.at(2).toFloat());
	
	v2.setX(strList.at(3).toFloat());
	v2.setY(strList.at(4).toFloat());
	v2.setZ(strList.at(5).toFloat());

	std::vector<int> quvec;
	quvec = _kdTree.rangeQuery(v1, v2);

	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 1;

	for (std::vector<int>::iterator it = quvec.begin(); it != quvec.end(); ++it)
	{

		glWidget->colors[*it] = 0.9;
		glWidget->colors[*it + 1] = 0;
		glWidget->colors[*it + 2] = 0.2;
	}

}