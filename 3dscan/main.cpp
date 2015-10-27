#include "mainwindow.h"
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QtCore/qmath.h>

double xsum = 0, ysum = 0, zsum = 0;

struct doublepair{
	public:
		double x, y;
};

doublepair xminmax, yminmax, zmnmax;

std::vector<float> xyzFileToVec(std::string source){
	
	std::vector<float> vec; 
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

	std::vector<float> verticesLoaded = xyzFileToVec("C:/all.xyz");
	count = verticesLoaded.size();
	vertices = verticesLoaded.data();

	QGuiApplication app(argc, argv);

	QSurfaceFormat format;
	format.setSamples(16);

	OpenGLWindow window;
	window.vertices = vertices;
	window.count = count;
	window.model.setToIdentity();
	window.projection.perspective(60.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	window.view.lookAt(QVector3D(xsum, ysum, zsum) + QVector3D(-100, 20, 0), QVector3D(xsum, ysum, zsum), QVector3D(0, 1, 0));
	window.center = QVector3D(xsum, ysum, zsum);

	window.setFormat(format);
	window.resize(640, 480);
	window.show();

	window.setAnimating(true);

	return app.exec();
}

