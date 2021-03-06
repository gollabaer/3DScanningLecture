#include "MainApplication.h"
#include "Algorithms.h"

#include <QSlider>
#include <QVBoxLayout>
#include <QPushButton>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <math.h>
#include <QGridLayout>
#include <QToolBox>
#include <QGroupBox>
#include <QDoubleValidator>
#include <assert.h>
#include <limits>  // std::numeric_limits


std::vector<Point3d> xyzFileToVec(std::string source){

	FILE* file = 0;
	std::vector<Point3d> points;
	int error = fopen_s(&file, source.c_str(), "rt"); //r= read, t=text
	if (error != 0)
	{
		std::cout << "file " << source.c_str() << " could not be opened!" << std::endl;
		return points; //nothing can be done else -> end function
	}
	std::cout << "reading file: " << source.c_str() << std::endl;
	while (!feof(file)) //as long we have not reached the end-of-file
	{
		Point3d point;
		int items = fscanf_s(file, "%lf %lf %lf\n", &point.x, &point.y, &point.z);
		if (items != 3) //we ecpected that 3 values have been read (except we are already at the end of file)
		{
			std::cout << "file format error" << std::endl;
			break; //abort while loop
		}
		else
		{
			points.push_back(point); //add the current point to our point vector
		}
	}
	//dont forget to close to file
	fclose(file);

	return points;
}


MainApplication::MainApplication(QWidget *parent) : QWidget(parent)
{

	const int textSize = 12;
	const int toolBoxWidth = 160;
	const int toolBoxWidgetsWidth = 140;
	const int toolBoxSubWidgetsWidth = 120;
	QSize textEditSize = QSize(40, 30);
	int windowHeight = 500;
	int windowWidth = 800;

	/*---- Buttons ----*/
	QPushButton *rangeQueryButton = new QPushButton(tr("Range"));
	rangeQueryButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *radiusQueryButton = new QPushButton(tr("Radius"));
	radiusQueryButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *loadButton = new QPushButton(tr("Load"));
	loadButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *nnQueryButton = new QPushButton(tr("NN-Query"));
	nnQueryButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *smoothingButton = new QPushButton(tr("Smooth"));
	smoothingButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *distanceColorMapButton = new QPushButton(tr("ColorbyDist"));
	distanceColorMapButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *thinningButton = new QPushButton(tr("Thinning"));
	thinningButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *lineFittingButton = new QPushButton(tr("fit Line"));
	lineFittingButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *planeFittingButton = new QPushButton(tr("fit Plane"));
	planeFittingButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	QPushButton *sphereFittingButton = new QPushButton(tr("fit Sphere"));
	sphereFittingButton->setFont(QFont("Times", textSize, QFont::AnyStyle));

	connect(loadButton, SIGNAL(clicked()), this, SLOT(loadPoints()));
	connect(rangeQueryButton, SIGNAL(clicked()), this, SLOT(rangeQuery()));
	connect(radiusQueryButton, SIGNAL(clicked()), this, SLOT(radiusQuery()));
	connect(smoothingButton, SIGNAL(clicked()), this, SLOT(smoothPointCloud()));
	connect(nnQueryButton, SIGNAL(clicked()), this, SLOT(nnQuery()));
	connect(distanceColorMapButton, SIGNAL(clicked()), this, SLOT(colorPointsByDistance()));
	connect(thinningButton, SIGNAL(clicked()), this, SLOT(applyThinning()));
	connect(lineFittingButton, SIGNAL(clicked()), this, SLOT(fitLine()));
	connect(planeFittingButton, SIGNAL(clicked()), this, SLOT(fitPlane()));
	connect(sphereFittingButton, SIGNAL(clicked()), this, SLOT(fitSphere()));

	/*---- Labels ----*/ 
	labelCloudBounds = new QLabel("---", this);
	labelCloudBounds->setMaximumHeight(60);

	labelPoints = new QLabel("---", this);
	labelPoints->setMaximumHeight(60);

	labelTime = new QLabel("---", this);
	labelTime->setMaximumHeight(60);

	labelFitting = new QLabel("p: dir:", this);
	labelFitting->setMaximumHeight(120);

	/*---- Text Edits ----*/
	QDoubleValidator *validDouble = new QDoubleValidator();
	minXRange = new QLineEdit();
	minXRange->setMaximumSize(textEditSize);
	minXRange->setValidator(validDouble);
	maxXRange = new QLineEdit();
	maxXRange->setMaximumSize(textEditSize);
	maxXRange->setValidator(validDouble);
	minYRange = new QLineEdit();
	minYRange->setMaximumSize(textEditSize);
	minYRange->setValidator(validDouble);
	maxYRange = new QLineEdit();
	maxYRange->setMaximumSize(textEditSize);
	maxYRange->setValidator(validDouble);
	minZRange = new QLineEdit();
	minZRange->setMaximumSize(textEditSize);
	minZRange->setValidator(validDouble);
	maxZRange = new QLineEdit();
	maxZRange->setMaximumSize(textEditSize);
	maxZRange->setValidator(validDouble);

	xRadius = new QLineEdit();
	xRadius->setMaximumSize(textEditSize);
	xRadius->setValidator(validDouble);
	yRadius = new QLineEdit();
	yRadius->setMaximumSize(textEditSize);
	yRadius->setValidator(validDouble);
	zRadius = new QLineEdit();
	zRadius->setMaximumSize(textEditSize);
	zRadius->setValidator(validDouble);
	rRadius = new QLineEdit();
	rRadius->setMaximumSize(textEditSize);
	rRadius->setValidator(validDouble);

	xNeighbour = new QLineEdit();
	xNeighbour->setMaximumSize(textEditSize);
	xNeighbour->setValidator(validDouble);
	yNeighbour = new QLineEdit();
	yNeighbour->setMaximumSize(textEditSize);
	yNeighbour->setValidator(validDouble);
	zNeighbour = new QLineEdit();
	zNeighbour->setMaximumSize(textEditSize);
	zNeighbour->setValidator(validDouble);

	rSmoothing = new QLineEdit();
	rSmoothing->setMaximumSize(textEditSize);
	rSmoothing->setMaximumWidth(toolBoxSubWidgetsWidth);
	rSmoothing->setValidator(validDouble);

	rThinning = new QLineEdit();
	rThinning->setMaximumSize(textEditSize);
	rThinning->setMaximumWidth(toolBoxSubWidgetsWidth);
	rThinning->setValidator(validDouble);
	
	/*---- Tool Box and Tool Box Widgets ----*/
	QToolBox *toolBox = new QToolBox();

	//Load
	QVBoxLayout *layoutLoad = new QVBoxLayout();
	layoutLoad->addWidget(loadButton);
	QWidget* LoadWidget = new QWidget();
	LoadWidget->setLayout(layoutLoad);
	LoadWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(LoadWidget, "Load Data");
	
	// Range Query
	QGridLayout *layoutRangeTextEdits = new QGridLayout();
	layoutRangeTextEdits->addWidget(minXRange,0,0,0);
	layoutRangeTextEdits->addWidget(maxXRange,0,1,0);
	layoutRangeTextEdits->addWidget(minYRange,1,0,0);
	layoutRangeTextEdits->addWidget(maxYRange,1,1,0);
	layoutRangeTextEdits->addWidget(minZRange,2,0,0);
	layoutRangeTextEdits->addWidget(maxZRange,2,1,0);

	QWidget* RangeTextEditsWidget = new QWidget();
	RangeTextEditsWidget->setLayout(layoutRangeTextEdits);
	RangeTextEditsWidget->setFixedWidth(toolBoxSubWidgetsWidth);

	QVBoxLayout *layoutRange = new QVBoxLayout();
	layoutRange->addWidget(RangeTextEditsWidget);
	layoutRange->addWidget(rangeQueryButton);	

	QWidget* RangeWidget = new QWidget();
	RangeWidget->setLayout(layoutRange);
	RangeWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(RangeWidget, "Range Query");

	// Radius Query
	QGridLayout *layoutRadiusTextEdits = new QGridLayout();
	layoutRadiusTextEdits->addWidget(xRadius, 0, 0, 0);
	layoutRadiusTextEdits->addWidget(yRadius, 0, 1, 0);
	layoutRadiusTextEdits->addWidget(zRadius, 0, 3, 0);
	layoutRadiusTextEdits->addWidget(rRadius, 1, 1, 0);

	QWidget* RadiusTextEditsWidget = new QWidget();
	RadiusTextEditsWidget->setLayout(layoutRadiusTextEdits);
	RadiusTextEditsWidget->setFixedWidth(toolBoxSubWidgetsWidth);

	QVBoxLayout *layoutRadius = new QVBoxLayout();
	layoutRadius->addWidget(RadiusTextEditsWidget);
	layoutRadius->addWidget(radiusQueryButton);

	QWidget* RadiusWidget = new QWidget();
	RadiusWidget->setLayout(layoutRadius);
	RadiusWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(RadiusWidget, "Radius Query");

	// NN Query
	QGridLayout *layoutNNTextEdits = new QGridLayout();
	layoutNNTextEdits->addWidget(xNeighbour, 0, 0, 0);
	layoutNNTextEdits->addWidget(yNeighbour, 0, 1, 0);
	layoutNNTextEdits->addWidget(zNeighbour, 0, 3, 0);

	QWidget* NNTextEditsWidget = new QWidget();
	NNTextEditsWidget->setLayout(layoutNNTextEdits);
	NNTextEditsWidget->setFixedWidth(toolBoxSubWidgetsWidth);

	QVBoxLayout *layoutNN = new QVBoxLayout();
	layoutNN->addWidget(NNTextEditsWidget);
	layoutNN->addWidget(nnQueryButton);

	QWidget* NNWidget = new QWidget();
	NNWidget->setLayout(layoutNN);
	NNWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(NNWidget, "Nearest Neighbour");

	// Thinning
	QVBoxLayout *layoutThinning = new QVBoxLayout();
	layoutThinning->addWidget(rThinning);
	layoutThinning->addWidget(thinningButton);
	
	QWidget* ThinningWidget = new QWidget();
	ThinningWidget->setLayout(layoutThinning);
	ThinningWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(ThinningWidget, "Thinning");

	// Smoothing
	QVBoxLayout *layoutSmoothing = new QVBoxLayout();
	layoutSmoothing->addWidget(rSmoothing);
	layoutSmoothing->addWidget(smoothingButton);

	QWidget* SmoothingWidget = new QWidget();
	SmoothingWidget->setLayout(layoutSmoothing);
	SmoothingWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(SmoothingWidget, "Smoothing");

	// Fitting
	QVBoxLayout *layoutFitting = new QVBoxLayout();
	layoutFitting->addWidget(labelFitting);
	layoutFitting->addWidget(planeFittingButton);
	layoutFitting->addWidget(lineFittingButton);
	layoutFitting->addWidget(sphereFittingButton);

	QWidget* FittingWidget = new QWidget();
	FittingWidget->setLayout(layoutFitting);
	FittingWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(FittingWidget, "Fitting");

	// Color
	QVBoxLayout *layoutColorByDist = new QVBoxLayout();
	layoutColorByDist->addWidget(distanceColorMapButton);

	QWidget* ColorByDistWidget = new QWidget();
	ColorByDistWidget->setLayout(layoutColorByDist);
	ColorByDistWidget->setFixedWidth(toolBoxWidgetsWidth);
	toolBox->addItem(ColorByDistWidget, "Color by Distance");

	/*---- Data Group Box ----*/
	QGroupBox *dataBox = new QGroupBox(tr("Data"));
	QVBoxLayout *layoutDataBox = new QVBoxLayout;
	layoutDataBox->addWidget(labelPoints);
	layoutDataBox->addWidget(labelCloudBounds);
	dataBox->setLayout(layoutDataBox);

	/*---- Side Bar ----*/
	QVBoxLayout *layoutSideBar = new QVBoxLayout();

	layoutSideBar->addWidget(dataBox);
	layoutSideBar->addWidget(toolBox);
	layoutSideBar->addWidget(labelTime);
	
	QWidget* sideBarWidget = new QWidget();
	sideBarWidget->setLayout(layoutSideBar);
	sideBarWidget->setFixedWidth(toolBoxWidth);
	
	/*---- Main Widget ----*/
	glWidget = new MainGLWidget();
	glWidget->resize(windowWidth, windowHeight);
	glWidget->setMinimumWidth(windowWidth);
	glWidget->setMinimumHeight(windowHeight);

	QHBoxLayout *layoutMain = new QHBoxLayout();
	layoutMain->addWidget(glWidget);
	layoutMain->addWidget(sideBarWidget);

	setLayout(layoutMain);
}

MainApplication::~MainApplication()
{
//delete glWidget;
}

void MainApplication::loadPoints(){

	labelCloudBounds->setText("Loading...");
	labelTime->setText(QString("---"));

	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer

	QString fstr = QFileDialog::getOpenFileName(this, tr("Open File"), "c:/", tr("Point Files (*.xyz)"));
	
	if (!(fstr != NULL && !fstr.isEmpty())){
		labelCloudBounds->setText("---");
		return;
	}

	// read points from file
	std::string str = fstr.toStdString();
	this->points = xyzFileToVec(str.c_str());

	labelPoints->setText(QString::number(points.size()) + " points");
	
	// Bounding Box
	std::vector<float> bbox = this->glWidget->cam.init(points, 640, 380);

	std::stringstream sStream;
	sStream.precision(2);
	sStream << bbox[0] << ":" << bbox[18] << "\n" << bbox[1] << ":" << bbox[19] << "\n" << bbox[2] << ":" << bbox[20];

	std::string boundingBoxDimensions = sStream.str();
	std::vector<Point3d> normals;
	double normalRadius = 1;



	// hand pointers to vertex data to glWidget
	this->glWidget->m_vertices = &(this->points);
	
	this->glWidget->count = points.size() * 3;

	// set up color array 
	this->glWidget->colors = new GLfloat[points.size() * 3];
	setColor(1.0);

	// build up the kd-Tree
	labelCloudBounds->setText("Building Tree3d...");
	this->_Tree3d = Tree3d(points, 100);
	this->_Tree3d.computeNormals(normalRadius, normals);
	this->glWidget->m_normals->resize(normals.size());
	std::copy(normals.begin(), normals.end(), this->glWidget->m_normals->begin());
	stopTimer(t1);

	labelCloudBounds->setText(QString(boundingBoxDimensions.c_str()));
}

void MainApplication::rangeQuery()
{
	labelTime->setText(QString("---"));

	Point3d v1 = Point3d(minXRange->text().toDouble(), minYRange->text().toDouble(), minZRange->text().toDouble());
	Point3d	v2 = Point3d(maxXRange->text().toDouble(), maxYRange->text().toDouble(), maxZRange->text().toDouble());
	std::vector<int> quvec;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer
	
	quvec = _Tree3d.rangeQuery(v1, v2);

	stopTimer(t1);
	setColor(0.3);
	setColor(quvec.begin(), quvec.end(), 0.9, 0, 0.2);

	glWidget->update();
}

void MainApplication::radiusQuery()
{
	labelTime->setText(QString("---"));

	Point3d queryPoint = Point3d(xRadius->text().toDouble(), yRadius->text().toDouble(), zRadius->text().toDouble());
	double radius = rRadius->text().toDouble();

	std::vector<int> quvec;
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer

	quvec = _Tree3d.radiusQuery(queryPoint, radius);

	stopTimer(t1);
	setColor(0.3);
	setColor(quvec.begin(), quvec.end(), 0.9, 0, 0.2);

	glWidget->update();
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

	double tempColor;
	for (int i = 0; i < distances.size(); i++){
		tempColor = distances[i];
		tempColor /= maxDist;
		tempColor = !(1.0f<tempColor) ? tempColor : 1.0f;

		outColor[i * 3 + 0] = tempColor;
		outColor[i * 3 + 1] = 0.0f;
		outColor[i * 3 + 2] = 1.0f - tempColor;
	}
}

void MainApplication::colorPointsByDistance()
{
	labelTime->setText(QString("---"));

	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer

	colordistance(this->_Tree3d.getPoints(), this->trees.front(), this->glWidget->colors);

	stopTimer(t1);
	this->points = _Tree3d.getPoints();

	glWidget->update();
}

void MainApplication::smoothPointCloud()
{
	labelTime->setText(QString("---"));

	double radius = rSmoothing->text().toDouble();
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer

	std::vector<Point3d> smoothedCloud = _Tree3d.applySmoothing(radius);

	stopTimer(t1);
	this->trees.push_back(Tree3d(smoothedCloud, 100));
	this->points = smoothedCloud;
	setColor(1.0);

	glWidget->update();
}

void MainApplication::nnQuery()
{
	labelTime->setText(QString("---"));

	Point3d v1 = Point3d(xNeighbour->text().toDouble(), yNeighbour->text().toDouble(), zNeighbour->text().toDouble());
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer

	int ind_NN = _Tree3d.nearestNeighbour(v1);

	stopTimer(t1);
	setColor(0.3);
	setColor(ind_NN, 0, 1.0, 0);

	glWidget->update();
}


void MainApplication::applyThinning()
{
	labelTime->setText(QString("---"));

	double radius = rThinning->text().toDouble();
	std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> t1 = std::chrono::high_resolution_clock::now(); //start timer

	_Tree3d.applyThinningByRadius(radius);

	stopTimer(t1);
	this->points = _Tree3d.getThinnedPoints();
	labelPoints->setText(QString::number(points.size()) + " points");
	setColor(1.0);

	glWidget->update();
}

void MainApplication::fitLine()
{
	Point3d p;
	Point3d dir;
	std::vector<Point3d> boundaries = std::vector<Point3d>();
	algorithms::fitLine(this->_Tree3d.getPoints(), p, dir, &boundaries);

	this->glWidget->setFittedLine(boundaries[0], boundaries[1]);

	std::stringstream ss;
	ss << "p: " << p.toString() << std::endl << "dir: " << dir.toString();
	labelFitting->setText(QString::fromStdString(ss.str()));
}

void MainApplication::fitPlane()
{
	Point3d p;
	Point3d norm;
	std::vector<Point3d> boundaries = std::vector<Point3d>();
	algorithms::fitPlane(this->_Tree3d.getPoints(), p, norm, &boundaries);
	
	this->glWidget->setFittedPlane(boundaries[0], boundaries[1], boundaries[2], boundaries[3]);

	std::stringstream ss;
	ss << "p: " << p.toString() << " norm: " << norm.toString();
	labelFitting->setText(QString::fromStdString(ss.str()));
}

void MainApplication::fitSphere()
{
	Point3d center;
	double radius;
	algorithms::fitSphere(this->_Tree3d.getPoints(), center, radius);

	this->glWidget->setFittedSphere(center, radius);

	std::stringstream ss;
	ss << "center: " << center.toString() << " radius: " << radius;
	labelFitting->setText(QString::fromStdString(ss.str()));
}