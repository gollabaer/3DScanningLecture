#include "mainwindow.h"

#include <QtCore/QCoreApplication>

#include <QtWidgets/QApplication>
#include <QtGui/QGuiApplication>

#include <QtGui/QScreen>
#include <QtCore/qmath.h>
#include <qopenglpaintdevice.h>
#include <qpainter.h>

/*The vertex and fragment shaders are relatively simple,
doing vertex transformation and interpolated vertex coloring.*/

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
	, oldMousePosition(-1, -1)
{
	// the window is to be used for OpenGL rendering
	setSurfaceType(QWindow::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
	delete m_device;
}



void OpenGLWindow::renderLater()
{
	/*The renderLater() function simply puts an update request event on the event loop,
	which leads to renderNow() being called once the event gets processed.*/
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
	/*The exposeEvent() is the notification to the window that its exposure,
	meaning visibility, on the screen has changed.*/
	Q_UNUSED(event);

	if (isExposed())
		renderNow();
}

void OpenGLWindow::renderNow()
{
	// end function if window is not visible
	if (!isExposed())
		return;

	bool needsInitialize = false;

	/*QOpenGLContext represents the OpenGL state of an underlying OpenGL context.
	To set up a context, set its screen and format such that they match those of the surface
	or surfaces with which the context is meant to be used.*/
	if (!m_context) {
		m_context = new QOpenGLContext(this);
		m_context->setFormat(requestedFormat());
		m_context->create();

		needsInitialize = true;
	}

	m_context->makeCurrent(this);

	if (needsInitialize) {
		// intern OpenGL function
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
	/*render() is called at the vertical refresh rate*/
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
	// see declaration of sources on top
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();
	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");
	qrotation = QQuaternion(1, 0, 0, 0);

}


void OpenGLWindow::render()
{
	// set up viewport
	glViewport(0, 0, width() , height());

	// clear background
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
	/*Change Model matrix according to mouse movement when left mouse button is pressed*/
	if(event->buttons() & Qt::LeftButton){
		// check if position values are valid
		if (oldMousePosition.x() != -1 && oldMousePosition.y() != -1){

			//difference of new and old mouse position
			float xDiff = oldMousePosition.x() - event->x();
			float yDiff = oldMousePosition.y() - event->y();


			QQuaternion q1;
			q1 = q1.fromEulerAngles(QVector3D(0, -xDiff, -yDiff));
			qrotation = q1  * qrotation;
			QMatrix4x4 m;
			m.rotate(qrotation);
			// set  back model
			model.setToIdentity();
			// translate to center for rotation
			model.translate(center);
			// rotation
			model =model * m ;
			//move back to former position
			model.translate(-center);
			
		}
			oldMousePosition.setX(event->x());
			oldMousePosition.setY(event->y());
		
	}
}


void  OpenGLWindow::mouseReleaseEvent(QMouseEvent* event){
	/*clear mouse position*/
	oldMousePosition.setX(-1);
	oldMousePosition.setY(-1);
}