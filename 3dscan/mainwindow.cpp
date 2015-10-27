#include "mainwindow.h"

#include <QtCore/QCoreApplication>

#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>

#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include <qopenglpaintdevice.h>
#include <qpainter.h>

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





OpenGLWindow::OpenGLWindow(QWindow *parent)
	: QWindow(parent)
	, m_update_pending(false)
	, m_animating(false)
	, m_context(0)
	, m_device(0)
	, m_program(0)
	, m_frame(0)
{
	setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
	delete m_device;
}



void OpenGLWindow::renderLater()
{
	if (!m_update_pending) {
		m_update_pending = true;
		QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
	}
}

bool OpenGLWindow::event(QEvent *event)
{
	switch (event->type()) {
	case QEvent::UpdateRequest:
		m_update_pending = false;
		renderNow();
		return true;
	default:
		return QWindow::event(event);
	}
}

void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
	Q_UNUSED(event);

	if (isExposed())
		renderNow();
}

void OpenGLWindow::renderNow()
{
	if (!isExposed())
		return;

	bool needsInitialize = false;

	if (!m_context) {
		m_context = new QOpenGLContext(this);
		m_context->setFormat(requestedFormat());
		m_context->create();

		needsInitialize = true;
	}

	m_context->makeCurrent(this);

	if (needsInitialize) {
		initializeOpenGLFunctions();
		initialize();
	}

	render();

	m_context->swapBuffers(this);

	if (m_animating)
		renderLater();
}

void OpenGLWindow::setAnimating(bool animating)
{
	m_animating = animating;

	if (animating)
		renderLater();
}

GLuint OpenGLWindow::loadShader(GLenum type, const char *source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	return shader;
}

void OpenGLWindow::initialize()
{
	m_program = new QOpenGLShaderProgram(this);
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();
	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");

}


void OpenGLWindow::render()
{
	const qreal retinaScale = devicePixelRatio();
	glViewport(0, 0, width() * retinaScale, height() * retinaScale);

	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();
	
	QMatrix4x4 matrix = projection * view * model;

	m_program->setUniformValue(m_matrixUniform, matrix);


	GLfloat colors[] = {
		1.0f, 0.0f, 0.0f
	};

	glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(m_colAttr, 1, GL_FLOAT, GL_FALSE, 0, colors);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_POINTS, 0, count / 3);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	m_program->release();

	++m_frame;
}

void  OpenGLWindow::mouseMoveEvent(QMouseEvent* event){
	if(event->buttons() & Qt::LeftButton){
		if (oldMousePosition.x() != -1 && oldMousePosition.y() != -1){

			float xDiff = oldMousePosition.x() - event->x();
			float yDiff = oldMousePosition.x() - event->y();

			model.translate(center);
			model.rotate(2, -xDiff, 0, -yDiff);
			model.translate(-center);


		}
		else{
			oldMousePosition.setX(event->x());
			oldMousePosition.setY( event->y());
		}
	}
}


void  OpenGLWindow::mouseReleaseEvent(QMouseEvent* event){
	oldMousePosition.setX(-1);
	oldMousePosition.setY(-1);
}