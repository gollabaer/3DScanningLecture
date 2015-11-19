#include "mainwindow.h"
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include "MainApplication.h"

#include <Tree3d.h>

#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>
#include <QtCore/qmath.h>


struct doublepair{
	public:
		double x, y;
};




int main(int argc, char **argv)
{
	
	
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

	 MainApplication window;

	
	window.show();



	return app.exec();
}

