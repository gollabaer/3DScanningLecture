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
	vec.reserve(3000000);
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
	QPushButton *quitButton = new QPushButton(tr("Quit"));
	quitButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *loadButton = new QPushButton(tr("Load"));
	loadButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *rangequeryButton = new QPushButton(tr("Range"));
	rangequeryButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	QPushButton *nnqueryButton = new QPushButton(tr("NN-Query"));
	nnqueryButton->setFont(QFont("Times", 12, QFont::AnyStyle));

	glWidget = new MainGLWidget();
	GLfloat initialVertexAttributes[] = { 0 };
	glWidget->vertices = initialVertexAttributes;
	glWidget->colors = initialVertexAttributes;
	glWidget->count = 0;

	labelCloudBounds = new QLabel("---", this);
	labelCloudBounds->setMaximumHeight(60);

	glWidget->resize(640, 380);

	QHBoxLayout *layoutMain = new QHBoxLayout();

	QVBoxLayout *layoutButtons = new QVBoxLayout();

	connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadPoints()));
	connect(rangequeryButton, SIGNAL(clicked()), this, SLOT(rangeQuery()));
	connect(nnqueryButton, SIGNAL(clicked()), this, SLOT(nnQuery()));

	layoutButtons->addWidget(labelCloudBounds);
	layoutButtons->addWidget(loadButton);
	layoutButtons->addWidget(rangequeryButton);
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
	sStream << bbox[0] << ":" << bbox[18] << "\n" << bbox[1] << ":" << bbox[19] << "\n" << bbox[2] << ":" << bbox[20];

	std::string ts = sStream.str();
	
	this->glWidget->vertices = points.data();
	this->glWidget->count = points.size();

	labelCloudBounds->setText("Building kdTree...");

	this->_kdTree = kdTree(points, 100, 3);

	labelCloudBounds->setText(QString(ts.c_str()));
}

void MainApplication::rangeQuery(){
	QString str = QInputDialog::getText(this, "Input points:", "x1 y1 z1 x2 y2 z2");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 6) return;

	std::vector<float> v1 ,v2;
	
	v1.push_back(strList.at(0).toFloat());
	v1.push_back(strList.at(1).toFloat());
	v1.push_back(strList.at(2).toFloat());
	
	v2.push_back(strList.at(3).toFloat());
	v2.push_back(strList.at(4).toFloat());
	v2.push_back(strList.at(5).toFloat());

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


void MainApplication::nnQuery(){
	QString str = QInputDialog::getText(this, "Input point:", "x1 y1 z1");
	if (!(str != NULL && !str.isEmpty()))return;
	QStringList strList = str.split(" ");

	if (strList.size() != 3) return;

	std::vector<float> v1;

	v1.push_back(strList.at(0).toFloat());
	v1.push_back(strList.at(1).toFloat());
	v1.push_back(strList.at(2).toFloat());

	int ind_NN = _kdTree.nearestNeighbor(v1);
	
	for (int i = 0; i < glWidget->count; i++)
		glWidget->colors[i] = 0.5f;


	glWidget->colors[ind_NN] = 1.0f;
	glWidget->colors[ind_NN + 1] = 0.0f;
	glWidget->colors[ind_NN + 2] = 0.0f;
	

}