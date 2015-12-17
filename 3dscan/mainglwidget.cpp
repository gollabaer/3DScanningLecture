
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
	, m_vvboPointCloud(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
	, m_cvboPointCloud(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
	, m_vvboLine(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
	, m_cvboLine(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
	, m_vvboPlane(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
	, m_cvboPlane(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))
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

	// create VBOs for positondata
	m_vvboPointCloud.create();
	m_vvboLine.create();
	m_vvboPlane.create();

	// create VBOs for Colordata
	m_cvboPointCloud.create();
	m_cvboLine.create();
	m_cvboPlane.create();

	// create VAOs
	m_vaoPointcloud.create();
	m_vaoLine.create();
	m_vaoPlane.create();
}


void MainGLWidget::paintGL() {
	
	if (count == 0) return;
	// set up viewport
	glViewport(0, 0, width(), height());

	// clear background
	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();

	QMatrix4x4 matrix = cam.getCombinedMatrix();

	m_program->setUniformValue(m_matrixUniform, matrix);


	/*glVertexAttribPointer(m_posAttr, 3, GL_DOUBLE, GL_FALSE, sizeof(Point3d), &(m_vertices->operator[](0)));
	glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

	glEnableVertexAttribArray(m_posAttr);
	glEnableVertexAttribArray(m_colAttr);

	glDrawArrays(GL_POINTS, 0, m_vertices->size());

	glDisableVertexAttribArray(m_posAttr);
	glDisableVertexAttribArray(m_colAttr);*/

	m_vaoPointcloud.bind();

	glDrawArrays(GL_POINTS, 0, m_vertices->size());
	m_vaoPointcloud.release();


	m_program->release();

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

void MainGLWidget::setPointCloud(std::vector<Point3d>* cloudPoints)
{
	m_vertices = cloudPoints;
	m_vaoPointcloud.bind(); //sets the Vertex Array Object current to the OpenGL context so we can write attributes to it
	
	// set VertexData
	m_vvboPointCloud.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_vvboPointCloud.bind();
	m_vvboPointCloud.allocate(&(m_vertices->operator[](0)), m_vertices->size() * sizeof(Point3d));
	glEnableVertexAttribArray(m_posAttr);
	glVertexAttribPointer(m_posAttr, 3, GL_DOUBLE, GL_FALSE, sizeof(Point3d), &(m_vertices->operator[](0))/*change to 0?*/);
	
	// set ColorData
	m_cvboPointCloud.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_cvboPointCloud.bind();
	m_cvboPointCloud.allocate(colors, count * sizeof(GLfloat));
	glEnableVertexAttribArray(m_colAttr);
	glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors/*change to 0?*/);

	// unbind Buffer and array objects
	m_vvboPointCloud.release();
	m_cvboPointCloud.release();
	m_vaoPointcloud.release();
}

void MainGLWidget::setLine(const std::vector<Point3d>& linePoints)
{

}

void MainGLWidget::setPLane(const std::vector<Point3d>& planePoints)
{

}