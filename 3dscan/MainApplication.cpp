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

std::vector<Point3d> xyzFileToVec(std::string source){


	std::vector<Point3d> vec;
	vec.reserve(3000000);
	std::fstream fs;
	fs.open(source.c_str(), std::ios::in);
	ulong c = 0;

	for (std::string line; std::getline(fs, line);)
	{
		std::istringstream in(line);
		float x, y, z;
		in >> x >> y >> z;

		vec.push_back(Point3d(x,y,z));
		c++;
	}

	return vec;
}


MainApplication::MainApplication(QWidget *parent) : QWidget(parent)
{
	QPushButton *quitButton = new QPushButton(tr("Quit"));
	quitButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *loadButton = new QPushButton(tr("Load"));
	loadButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *rangequeryButton = new QPushButton(tr("Range"));
	rangequeryButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *radiusQueryButton = new QPushButton(tr("Radius"));
	radiusQueryButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *nnqueryButton = new QPushButton(tr("NN-Query"));
	nnqueryButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	glWidget = new MainGLWidget();

	labelCloudBounds = new QLabel("---", this);
	labelCloudBounds->setMaximumHeight(60);

	glWidget->resize(640, 380);

	QHBoxLayout *layoutMain = new QHBoxLayout();

	QVBoxLayout *layoutButtons = new QVBoxLayout();

	connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadPoints()));
	connect(rangequeryButton, SIGNAL(clicked()), this, SLOT(rangeQuery()));
	connect(radiusQueryButton, SIGNAL(clicked()), this, SLOT(radiusQuery()));
	connect(nnqueryButton, SIGNAL(clicked()), this, SLOT(nnQuery()));

	layoutButtons->addWidget(labelCloudBounds);
	layoutButtons->addWidget(loadButton);
	layoutButtons->addWidget(rangequeryButton);
	layoutButtons->addWidget(radiusQueryButton);
	layoutButtons->addWidget(nnqueryButton);
	layoutButtons->addWidget(quitButton);
	

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
//delete glWidget;
}

void MainApplication::loadPoints(){

	labelCloudBounds->setText("Loading...");
	QString fstr = QFileDialog::getOpenFileName(this, tr("Open File"), "c:/", tr("Point Files (*.xyz)"));
	
	if (!(fstr != NULL && !fstr.isEmpty())){
		labelCloudBounds->setText("---");
		return;
	}

	// read points from file
	std::string str = fstr.toStdString();
	this->points = xyzFileToVec(str.c_str());	
	
	// set up color array and bounding box
	this->glWidget->colors = new GLfloat[points.size() * 3];
	for (int i = 0; i < points.size() * 3; i++)
		this->glWidget->colors[i] = (1.0f);
	std::vector<float> bbox = this->glWidget->cam.init(points, 640, 380);

	std::stringstream sStream;
	sStream.precision(2);
	sStream << bbox[0] << ":" << bbox[18] << "\n" << bbox[1] << ":" << bbox[19] << "\n" << bbox[2] << ":" << bbox[20];

	std::string boundingBoxDimensions = sStream.str();

	// hand pointers to vertex data to glWidget
	this->glWidget->m_vertices = &(this->points);
	this->glWidget->count = points.size() * 3;

	// build up the kd-Tree
	labelCloudBounds->setText("Building Tree3d...");
	this->_Tree3d = Tree3d(points, 100);

	labelCloudBounds->setText(QString(boundingBoxDimensions.c_str()));
}

void MainApplication::rangeQuery(){
	QString str = QInputDialog::getText(this, "Input points:", "x1 y1 z1 x2 y2 z2");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 6) return;

	Point3d v1 = Point3d(strList.at(0).toFloat(), strList.at(1).toFloat(), strList.at(2).toFloat());
	Point3d	v2 = Point3d(strList.at(3).toFloat(), strList.at(4).toFloat(), strList.at(5).toFloat());

	std::vector<int> quvec;
	quvec = _Tree3d.rangeQuery(v1, v2);

	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 0.5f;

	for (std::vector<int>::iterator it = quvec.begin(); it != quvec.end(); ++it)
	{
		glWidget->colors[*it * 3] = 0.9;
		glWidget->colors[*it * 3 + 1] = 0;
		glWidget->colors[*it * 3 + 2] = 0.2;
	}
}

void MainApplication::radiusQuery(){
	QString str = QInputDialog::getText(this, "Input point and radius:", "x y z r");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 4) return;

	Point3d queryPoint = Point3d(strList.at(0).toFloat(), strList.at(1).toFloat(), strList.at(2).toFloat());
	float radius = strList.at(3).toFloat();

	std::vector<int> quvec;
	quvec = _Tree3d.radiusQuery(queryPoint, radius);

	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 1;

	for (std::vector<int>::iterator it = quvec.begin(); it != quvec.end(); ++it)
	{
		glWidget->colors[*it * 3] = 0.9;
		glWidget->colors[*it * 3 + 1] = 0;
		glWidget->colors[*it * 3 + 2] = 0.2;
	}
}


void MainApplication::nnQuery()
{
	QString str = QInputDialog::getText(this, "Input point:", "x1 y1 z1");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 3) return;
	
	labelCloudBounds->setText("Locating NN..");
	
	Point3d v1 = Point3d(strList.at(0).toFloat(), strList.at(1).toFloat(), strList.at(2).toFloat());

	int ind_NN = _Tree3d.nearestNeighbour(v1);
	
	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 0.1f;


	glWidget->colors[ind_NN * 3] = 0.0f;
	glWidget->colors[ind_NN * 3 + 1] = 1.0f;
	glWidget->colors[ind_NN * 3 + 2] = 0.0f;
	
	labelCloudBounds->setText("Found NN!");
}