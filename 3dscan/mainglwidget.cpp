
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
	,m_program(0)
	, oldMousePosition(-1, -1)
{

}

MainGLWidget::~MainGLWidget(){}

void MainGLWidget::initializeGL(){
	m_program = new QOpenGLShaderProgram(this);
	initializeOpenGLFunctions();
	m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_program->link();
	m_posAttr = m_program->attributeLocation("posAttr");
	m_colAttr = m_program->attributeLocation("colAttr");
	m_matrixUniform = m_program->uniformLocation("matrix");
	qrotation = QQuaternion(1, 0, 0, 0);
	rotationMode = 0; // 0 = simple rotation, 1 = trackball

}


void MainGLWidget::paintGL() {
	// set up viewport
	glViewport(0, 0, width(), height());

	// clear background
	glClear(GL_COLOR_BUFFER_BIT);

	m_program->bind();

	QMatrix4x4 matrix = projection * view * model;

	m_program->setUniformValue(m_matrixUniform, matrix);


	glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, vertices);
	glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_POINTS, 0, count / 3);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);


	m_program->release();

}
void MainGLWidget::resizeGL(int width, int height) {
	projection.setToIdentity();
	projection.perspective(60.0f, float(width) / float(height), 0.1f, 1000.0f);
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
		if (rotationMode > 0) rotationMode = 0;
		else rotationMode = 1;
	}
}

void MainGLWidget::mouseMoveEvent(QMouseEvent *event) {
	/*Change Model matrix according to mouse movement when left mouse button is pressed*/
	if (event->buttons() & Qt::LeftButton){
		// check if position values are valid
		if (oldMousePosition.x() != -1 && oldMousePosition.y() != -1){
			// quaterion q is used to store additional rotation
			QQuaternion q;
			if (rotationMode == 0) // simple rotation
			{
				//difference of new and old mouse position
				float xDiff = oldMousePosition.x() - event->x();
				float yDiff = oldMousePosition.y() - event->y();

				q = q.fromEulerAngles(QVector3D(yDiff, -xDiff, 0));
				//q2 = q2.fromEulerAngles(QVector3D(0, 0, -yDiff));
			}
			else if (rotationMode == 1) //trackball
			{
				// define center of window
				float centerX = float(this->width() / 2);
				float centerY = float(this->height() / 2);

				//difference of new and old mouse position
				float oldX = (oldMousePosition.x() - centerX) / centerX;
				float oldY = (oldMousePosition.y() - centerY) / centerY;
				float newX = (event->x() - centerX) / centerX;
				float newY = (event->y() - centerY) / centerY;

				//model.rotate(2, -xDiff, 0, -yDiff);
				float rotation[4];
				// get rotation as quaterion from trackball function
				gfs_gl_trackball(rotation, newX, newY, oldX, oldY);
				q = QQuaternion(QVector4D(rotation[0], rotation[1], rotation[2], rotation[3]));
			}
			// combine new rotation and current rotation
			qrotation = q  * qrotation;
			QMatrix4x4 m;
			m.rotate(qrotation);
			model.setToIdentity();
			// translate to center for rotation
			model.translate(center);
			model = model * m;
			//move back to former position
			model.translate(-center);
		}
		// set current mouse position as old
		oldMousePosition.setX(event->x());
		oldMousePosition.setY(event->y());
	}
	update();
}

	