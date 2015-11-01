#include "mainwindow.h"
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <kdTree.h>

#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QtCore/qmath.h>

#include "mainglwidget.h"

double xsum , ysum, zsum ;

struct doublepair{
	public:
		double x, y;
};

doublepair xminmax, yminmax, zminmax;

std::vector<float> xyzFileToVec(std::string source){
	
	xminmax.x = 0;
	xminmax.y = 0;
	yminmax.x = 0;
	yminmax.y = 0;
	zminmax.x = 0;
	zminmax.y = 0;

	xsum = 0;
	ysum = 0;
	zsum = 0;

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
		xsum += x;
		ysum += y;
		zsum += z;
		vec.push_back(x);
		vec.push_back(y);
		vec.push_back(z);
		c++;
	}

	xsum /= (vec.size() / 3);
	ysum /= (vec.size() / 3);
	zsum /= (vec.size() / 3);

	return vec;
}


GLfloat* vertices;
uint count;



int main(int argc, char **argv)
{
	// load file
	std::vector<float> verticesLoaded = xyzFileToVec("C:/all.xyz");
	// number of vertices*3
	count = verticesLoaded.size(); 
	// number of vertices
	vertices = verticesLoaded.data();
	GLfloat *colors = new GLfloat[count];
	for (int i = 0; i < count; ++i)
		colors[i] = 1.;


	kdTree KD = kdTree(verticesLoaded, 8, 3);
	QVector3D p1 = QVector3D(-10.0, 5, -7);
	QVector3D p2 = QVector3D(8, -3, 9);
	std::vector<int> query = KD.rangeQuery(p1, p2);

	for (std::vector<int>::iterator it = query.begin(); it != query.end(); ++it)
	{
		colors[*it * 3] = 0.4;
	}


	//start QT GUI
	QApplication app(argc, argv);

	/*The format includes the size of the color buffers, red, green, and blue; 
	the size of the alpha buffer; the size of the depth and stencil buffers;
	and number of samples per pixel for multisampling.
	In addition, the format contains surface configuration parameters such as OpenGL profile
	and version for rendering, whether or not to enable stereo buffers, and swap behaviour.*/
	//QSurfaceFormat format;
	// Set the preferred number of samples per pixel when multisampling is enabled.
	//format.setSamples(16);

	MainGLWidget window;
	// pass vertices to QT
	window.vertices = vertices;
	// pass size of vertice vector to QT
	window.count = count;
	// set Modelmatrix to Identity 
	window.model.setToIdentity();
	// ...
	window.projection.perspective(60.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	// QVector3D(-100, 20, 0) Abstand von Zentrum
	window.view.lookAt(QVector3D(xsum, ysum, zsum) + QVector3D(0, 0, -100), QVector3D(xsum, ysum, zsum), QVector3D(0, 1, 0));
	// sums = mean values of all vertices
	window.center = QVector3D(xsum, ysum, zsum);

	window.colors = colors;
	// pass format to QT
	//window.setFormat(format);
	// change window size
	window.resize(640, 480);
	// show window
	window.show();



	return app.exec();
}

