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
#include <limits>
#include <chrono>
#include <math.h>


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
	const int textSize = 12;

	QPushButton *quitButton = new QPushButton(tr("Quit"));
	quitButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *loadButton = new QPushButton(tr("Load"));
	loadButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *rangequeryButton = new QPushButton(tr("Range"));
	rangequeryButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *radiusQueryButton = new QPushButton(tr("Radius"));
	radiusQueryButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *smoothingButton = new QPushButton(tr("Smooth"));
	smoothingButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *nnqueryButton = new QPushButton(tr("NN-Query"));
	nnqueryButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *distanceColorMapButton = new QPushButton(tr("ColorbyDist"));
	distanceColorMapButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *thinningMapButton = new QPushButton(tr("Thinning"));
	thinningMapButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	glWidget = new MainGLWidget();

	labelCloudBounds = new QLabel("---", this);
	labelCloudBounds->setMaximumHeight(60);

	labelTime = new QLabel("---", this);
	labelTime->setMaximumHeight(60);

	glWidget->resize(640, 380);

	QHBoxLayout *layoutMain = new QHBoxLayout();

	QVBoxLayout *layoutButtons = new QVBoxLayout();

	connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadPoints()));
	connect(rangequeryButton, SIGNAL(clicked()), this, SLOT(rangeQuery()));
	connect(radiusQueryButton, SIGNAL(clicked()), this, SLOT(radiusQuery()));
	connect(smoothingButton, SIGNAL(clicked()), this, SLOT(smoothPointCloud()));
	connect(nnqueryButton, SIGNAL(clicked()), this, SLOT(nnQuery()));
	connect(distanceColorMapButton, SIGNAL(clicked()), this, SLOT(colorPointsByDistance()));
	connect(thinningMapButton, SIGNAL(clicked()), this, SLOT(applyThinning()));


	layoutButtons->addWidget(labelCloudBounds);
	layoutButtons->addWidget(loadButton);
	layoutButtons->addWidget(rangequeryButton);
	layoutButtons->addWidget(radiusQueryButton);
	layoutButtons->addWidget(smoothingButton);
	layoutButtons->addWidget(nnqueryButton);
	layoutButtons->addWidget(distanceColorMapButton);
	layoutButtons->addWidget(thinningMapButton);
	layoutButtons->addWidget(labelTime);
	

	QWidget* buttonWidget = new QWidget();
	buttonWidget->setLayout(layoutButtons);
	buttonWidget->setFixedWidth(140);
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

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer

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

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream tStream;
	tStream.precision(5);
	tStream << t.count() << " seconds";
	labelTime->setText(QString(tStream.str().c_str()));

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

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer
	
	quvec = _Tree3d.rangeQuery(v1, v2);

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream sStream;
	sStream.precision(5);
	sStream  << t.count() << " seconds";
	labelTime->setText(QString(sStream.str().c_str()));

	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 0.3f;

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

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer

	quvec = _Tree3d.radiusQuery(queryPoint, radius);

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream sStream;
	sStream.precision(5);
	sStream << t.count() << " seconds";
	labelTime->setText(QString(sStream.str().c_str()));

	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 0.3;

	for (std::vector<int>::iterator it = quvec.begin(); it != quvec.end(); ++it)
	{
		glWidget->colors[*it * 3] = 0.9;
		glWidget->colors[*it * 3 + 1] = 0;
		glWidget->colors[*it * 3 + 2] = 0.2;
	}
}

void colordistance(const std::vector<Point3d> &other,  Tree3d &tree, GLfloat* outColor){
	
	std::vector<double> distances = tree.calculateDistance(other);

	double maxDist = 0;
	double minDist = std::numeric_limits<double>::max();
	for (auto it = distances.begin(); it != distances.end(); ++it)
	{
		maxDist = (*it > maxDist) ? *it : maxDist;
		minDist = (*it < minDist) ? *it : minDist;
	}

	float tempColor;
	for (int i = 0; i < distances.size(); i++){
		tempColor = distances[i];
		tempColor /= maxDist;
		tempColor = !(1.0f<tempColor) ? tempColor : 1.0f;

		outColor[i * 3 + 0] = tempColor;
		outColor[i * 3 + 1] = 0.0f;
		outColor[i * 3 + 2] = 1.0f - tempColor;
	}
}

void MainApplication::colorPointsByDistance(){

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer

	colordistance(this->_Tree3d.getPoints(), this->trees.front(), this->glWidget->colors);

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream sStream;
	sStream.precision(5);
	sStream << t.count() << " seconds";
	labelTime->setText(QString(sStream.str().c_str()));

	this->points = _Tree3d.getPoints();
}

void MainApplication::smoothPointCloud()
{
	QString str = QInputDialog::getText(this, "Input radius","r");
	if (!(str != NULL && !str.isEmpty())) return;

	double radius = str.toDouble();

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer

	std::vector<Point3d> smoothedCloud = _Tree3d.applySmoothing(radius);

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream sStream;
	sStream.precision(5);
	sStream << t.count() << " seconds";
	labelTime->setText(QString(sStream.str().c_str()));

	this->trees.push_back(Tree3d(smoothedCloud, 100));
	
	this->points = smoothedCloud;
}

void MainApplication::nnQuery()
{
	QString str = QInputDialog::getText(this, "Input point:", "x1 y1 z1");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 3) return;
	
	labelCloudBounds->setText("Locating NN..");
	
	Point3d v1 = Point3d(strList.at(0).toFloat(), strList.at(1).toFloat(), strList.at(2).toFloat());

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer

	int ind_NN = _Tree3d.nearestNeighbour(v1);

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream sStream;
	sStream.precision(5);
	sStream << t.count() << " seconds";
	labelTime->setText(QString(sStream.str().c_str()));

	
	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 0.3f;


	glWidget->colors[ind_NN * 3] = 0.0f;
	glWidget->colors[ind_NN * 3 + 1] = 1.0f;
	glWidget->colors[ind_NN * 3 + 2] = 0.0f;
	
	labelCloudBounds->setText("Found NN!");
}



void MainApplication::applyThinning()
{
	QString str = QInputDialog::getText(this, "Input radius", "r");
	if (!(str != NULL && !str.isEmpty())) return;

	double radius = str.toDouble();

	auto t1 = std::chrono::high_resolution_clock::now(); //start timer

	_Tree3d.applyThinningByRadius(radius);

	auto t2 = std::chrono::high_resolution_clock::now(); // stop timer
	std::chrono::duration<double> t = t2 - t1;
	std::stringstream sStream;
	sStream.precision(5);
	sStream << t.count() << " seconds";
	labelTime->setText(QString(sStream.str().c_str()));

	this->points = _Tree3d.getThinnedPoints();
}