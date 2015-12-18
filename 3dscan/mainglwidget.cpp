
#include "mainglwidget.h"
#include <QMouseEvent>

static const char *vertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"uniform highp mat4 matrix;\n"
"void main() {\n"
"   col = colAttr;\n"
"   gl_Position = matrix * posAttr;\n"
"}\n";

static const char *fragmentShaderSource =
"varying lowp vec4 col;\n"
"void main() {\n"
"   gl_FragColor = col;\n"
"}\n";



MainGLWidget::MainGLWidget(QWidget *parent)
	:QOpenGLWidget(parent)
	, m_program(0)
	, cam(Camera())
	, oldMousePosition(-1, -1)
	, m_vertices(nullptr)
	, colors(nullptr)
	, count(0)
	, drawFittedLine(false)
	, drawFittedPlane(false)
{

}

MainGLWidget::~MainGLWidget()
{
	//delete[] colors;
}

void MainGLWidget::initializeGL(){
	m_program = new QOpenGLShaderProgram(this);
	initializeOpenGLFunctions();
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();
	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");
}


void MainGLWidget::paintGL() {
	
	if (count == 0) return;
	// set up viewport
	glViewport(0, 0, width(), height());

	// clear background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	m_program->bind();

	QMatrix4x4 matrix = cam.getCombinedMatrix();

	m_program->setUniformValue(m_matrixUniform, matrix);


	glVertexAttribPointer(m_posAttr, 3, GL_DOUBLE, GL_FALSE, sizeof(Point3d), &(m_vertices->operator[](0)));
	glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

	glEnableVertexAttribArray(m_posAttr);
	glEnableVertexAttribArray(m_colAttr);

	
	glDrawArrays(GL_POINTS, 0, m_vertices->size());

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	m_program->release();

	if (drawFittedLine)
	{
		drawLine();
	}
	else if (drawFittedPlane)
	{
		drawPlane();
	}

}
void MainGLWidget::resizeGL(int width, int height) {
	cam.resize(width, height);
}

void MainGLWidget::mouseReleaseEvent(QMouseEvent *event){	
	/*clear mouse position*/
	oldMousePosition.setX(-1);
	oldMousePosition.setY(-1);
}

void MainGLWidget::keyPressEvent(QKeyEvent* event)
{
	/* Change rotation method if key r is pressed*/
	if (event->key() == Qt::Key_R)
	{

	}
}

void MainGLWidget::mouseMoveEvent(QMouseEvent *event) {
	/*Change Model matrix according to mouse movement when left mouse button is pressed*/
	if (event->buttons() & Qt::LeftButton){
		// check if position values are valid
		if (oldMousePosition.x() != -1 && oldMousePosition.y() != -1){
			// quaterion q is used to store additional rotation
			cam.rotate(event->x(), event->y(), oldMousePosition.x(), oldMousePosition.y());
		}
		// set current mouse position as old
		oldMousePosition.setX(event->x());
		oldMousePosition.setY(event->y());
	}
	update();
	event->accept();
}

void MainGLWidget::wheelEvent(QWheelEvent *event) {
	QPoint angle = event->angleDelta();
	int vertical_angle = angle.y() / 8;
	
	cam.incVFov(vertical_angle / -3);
	update();
	event->accept();
}

void MainGLWidget::setFittedPlane(Point3d a, Point3d b, Point3d c, Point3d d)
{
	drawFittedPlane = true;
	this->fittedPlane = Primitives::Quad(a, b, c, d);
}
void MainGLWidget::setFittedLine(Point3d a, Point3d b)
{
	drawFittedLine = true;
	this->fittedLine = Primitives::Line(a, b);
}

void MainGLWidget::drawLine()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cam.getModelMatrix().data());
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.getProjMatrix().data());

	glBegin(GL_LINES);
	glVertex3d(fittedLine.a.x, fittedLine.a.y, fittedLine.a.z);
	glVertex3d(fittedLine.b.x, fittedLine.b.y, fittedLine.b.z);
	glEnd();
}

void MainGLWidget::drawPlane()
{
	glColor3f(1, 0, 0);
	glMatrixMode(GL_MODELVIEW);
	QMatrix4x4 modelView = cam.getViewMatrix() * cam.getModelMatrix();
	glLoadMatrixf(modelView.data());
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.getProjMatrix().data());

	glBegin(GL_QUADS);
	glVertex3d(fittedPlane.a.x, fittedPlane.a.y, fittedPlane.a.z);
	glVertex3d(fittedPlane.b.x, fittedPlane.b.y, fittedPlane.b.z);
	glVertex3d(fittedPlane.c.x, fittedPlane.c.y, fittedPlane.c.z);
	glVertex3d(fittedPlane.d.x, fittedPlane.d.y, fittedPlane.d.z);
	glEnd();
		
}