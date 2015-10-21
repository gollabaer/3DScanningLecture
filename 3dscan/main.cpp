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

class TriangleWindow : public OpenGLWindow
{
public:
	TriangleWindow();

	void initialize() Q_DECL_OVERRIDE;
	void render() Q_DECL_OVERRIDE;

private:
	GLuint loadShader(GLenum type, const char *source);

	GLuint m_posAttr;
	GLuint m_colAttr;
	GLuint m_matrixUniform;

	QOpenGLShaderProgram *m_program;
	int m_frame;
};

TriangleWindow::TriangleWindow()
	: m_program(0)
	, m_frame(0)
{
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

	TriangleWindow window;
	window.setFormat(format);
	window.resize(640, 480);
	window.show();

	window.setAnimating(true);

	return app.exec();
}

static const char *vertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"uniform highp mat4 matrix;\n"
"void main() {\n"
"   col = vec4(1.0,1.0,1.0,1.0);\n"
"   gl_Position = matrix * posAttr;\n"
"}\n";

static const char *fragmentShaderSource =
"varying lowp vec4 col;\n"
"void main() {\n"
"   gl_FragColor = col;\n"
"}\n";

GLuint TriangleWindow::loadShader(GLenum type, const char *source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	return shader;
}

void TriangleWindow::initialize()
{
	m_program = new QOpenGLShaderProgram(this);
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();
	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");
}

void TriangleWindow::render()
{
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();

	QMatrix4x4 matrix;
	matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 200.0f);

	QMatrix4x4 view;
	view.lookAt(QVector3D(-100, 30, 0) + QVector3D(xsum, ysum, zsum), QVector3D(xsum, ysum, zsum), QVector3D(0, 1, 0));
	matrix = matrix * view;

	m_program->setUniformValue(m_matrixUniform, matrix);

	
	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f
	};

	glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(m_colAttr, 1, GL_FLOAT, GL_FALSE, 0, colors);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_POINTS, 0, count/3);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	m_program->release();

	++m_frame;
}