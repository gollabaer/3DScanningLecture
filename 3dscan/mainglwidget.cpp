
#include "mainglwidget.h"
#include <QMouseEvent>

static const char *vertexShaderSource =
"attribute highp vec4 posAttr;\n"
"attribute highp vec4 normAttr;\n"
"attribute lowp vec4 colAttr;\n"
"varying lowp vec4 col;\n"
"varying highp vec4 veetex;\n"
"varying highp vec4 normal;\n"
"uniform highp mat4 modelView;\n"
"uniform highp mat4 projection;\n"
"uniform highp mat4 normalMatrix;\n"
"void main() {\n"
"   col = colAttr;\n"
"   gl_Position = projection * modelView * posAttr;\n"
"   vertex = modelView * posAttr;\n"
"   normal = normalize(normalMatrix * normalAttr)\n"
"}\n";

static const char *fragmentShaderSource =
"varying lowp vec4 col;\n"
"varying highp vec4 nromal;\n"
"varying highp vec4 vertex;\n"
"void main() {\n"
"	//our own definitions\n"
"	vec3 lightPosition = vec3(1.0, 1.0, 1.0);         //in normalized coordinates (1,1,1). I want the light coming diagonal from the back\n"
"	vec4 ambientColor = vec4(0.1, 0.1, 0.1, 1.0);   //ambient (surrounding) color is quite dark \n"
"	//vec4 diffuseColor=vec4(0.4, 0.4, 0.4, 1.0); //you might give the diffuse light a certain fixed color, but I want to take the color that we define outside by glColor (transferred from Vertex shader using \"varying\" variable FrontColor)\n"
"	vec4 specularColor = vec4(0.7, 0.7, 0.7, 1.0);  //Shiny surfaces create reflecting spots, the color of these spots should be bright\n"
"	float shininess = 100.0;                        //Shininess - the higher the reflective the surface (is a parameter of the specular term)\n"
"\n"
"	vec3 N = normalize(normal);                        //if not already done outside, we normalize the normal vector here\n"
"\n"
"	vec3 L = normalize(lightPosition - vertex);        //L is the vector from the 3D position to the light position (v is 3D and was transferred from the vertex shader using a \"varying\" variable)\n"
"	//vec3 L = normalize( vec3(1,1,1) );\n"
"\n"
"	vec3 E = normalize(-vertex);                       // is the vector from 3d position to the camera. We are in Eye Coordinates, so EyePos is (0,0,0) \n"
"	vec3 R = normalize(-reflect(L, N));           // For a given incident vector I and surface normal N reflect returns the reflection direction calculated as I - 2.0 * dot(N, I) * N. N should be normalized.\n"
"\n"
"	//calculate Ambient Term: \n"
"	vec4 Iamb = ambientColor;                     // we just assign our own definition. The variable Iamb is useless it is just for sticking to the names given in the Phong formula  \n"
"\n"
"	//calculate Diffuse Term:                     // the amount of diffuse reflected light depends on the surface normal N and the light position L\n"
"	vec4 Idiff = FrontColor * max(abs(dot(N, L)), 0.0);  //we take our object color as diffuse color  !! ABS was added by our own as a trick to overcome inconsistent normal orientation !!\n"
"\n"
"	Idiff = clamp(Idiff, 0.0, 1.0);               //make sure that Idiff ranges between 0 and 1\n"
"\n"
"	// calculate Specular Term: taken from Phong formula\n"
"	vec4 Ispec = specularColor * pow(max(dot(R, E), 0.0), 0.03* shininess);\n"
"	Ispec = clamp(Ispec, 0.0, 1.0);\n"
"\n"
"	gl_FragColor = Iamb + Idiff + Ispec; //gl_FragColor is the color that the fragment (aka pixel) on the screen is assigned to. In the Phong model its the sum of ambient,diffuse and specular reflection terms\n"
"}";



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
	m_modelViewUniform = m_program->uniformLocation("modelView");
	m_projectionUniform = m_program->uniformLocation("projection");
	m_normalUniform = m_program->uniformLocation("normalMatrix");
}


void MainGLWidget::paintGL() {
	
	if (count == 0) return;
	// set up viewport
	glViewport(0, 0, width(), height());

	// clear background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	m_program->bind();

	QMatrix4x4 modelView = cam.getViewMatrix() * cam.getModelMatrix();
	QMatrix4x4 projection = cam.getProjMatrix();
	QMatrix4x4 normalMatrix = modelView.inverted().transposed();

	m_program->setUniformValue(m_modelViewUniform, modelView);
	m_program->setUniformValue(m_projectionUniform, projection);
	m_program->setUniformValue(m_normalUniform, normalMatrix);


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
	if (drawFittedPlane)
	{
		drawPlane();
	}
	if (drawFittedSphere)
	{
		drawSphere();
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
void MainGLWidget::setFittedSphere(Point3d p, double r)
{
	drawFittedSphere = true;
	this->fittedSphere = Primitives::Sphere(p, r);
}

void MainGLWidget::drawLine()
{
	glColor3f(0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	QMatrix4x4 modelView = cam.getViewMatrix() * cam.getModelMatrix();
	glLoadMatrixf(modelView.data());
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

	glBegin(GL_LINE_STRIP);
	glVertex3d(fittedPlane.a.x, fittedPlane.a.y, fittedPlane.a.z);
	glVertex3d(fittedPlane.b.x, fittedPlane.b.y, fittedPlane.b.z);
	glVertex3d(fittedPlane.c.x, fittedPlane.c.y, fittedPlane.c.z);
	glVertex3d(fittedPlane.d.x, fittedPlane.d.y, fittedPlane.d.z);
	glVertex3d(fittedPlane.a.x, fittedPlane.a.y, fittedPlane.a.z);
	glVertex3d(fittedPlane.c.x, fittedPlane.c.y, fittedPlane.c.z);
	glVertex3d(fittedPlane.b.x, fittedPlane.b.y, fittedPlane.b.z);
	glVertex3d(fittedPlane.d.x, fittedPlane.d.y, fittedPlane.d.z);
	glEnd();
		
}

void MainGLWidget::drawSphere()
{
	glColor3f(0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	QMatrix4x4 modelView = cam.getViewMatrix() * cam.getModelMatrix();
	glLoadMatrixf(modelView.data());
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cam.getProjMatrix().data());

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(this->fittedSphere.p.x, this->fittedSphere.p.y, this->fittedSphere.p.z);
	
	GLUquadric* quad = gluNewQuadric();
	gluSphere(quad, this->fittedSphere.r , 30, 30);
	gluDeleteQuadric(quad);

	glPopMatrix();
}