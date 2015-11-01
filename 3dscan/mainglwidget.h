#include <vector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <qopenglshaderprogram.h>
#include <qmath.h>
extern "C" {
#include "trackball.h"
}

struct Camera{

public:

	std::vector<float> init(std::vector<float> &points, float width, float height){
		float xmin = points[0], xmax = points[0];
		float ymin = points[1], ymax = points[1];
		float zmin = points[2], zmax = points[2];

		for (int i = 0; i < points.size(); i += 3){
			xmin = (points[i] < xmin) ? points[i] : xmin;
			xmax = (points[i] > xmax) ? points[i] : xmax;
		}

		for (int i = 1; i < points.size(); i += 3){
			ymin = (points[i] < ymin) ? points[i] : ymin;
			ymax = (points[i] > ymax) ? points[i] : ymax;
		}

		for (int i = 2; i < points.size(); i += 3){
			zmin = (points[i] < zmin) ? points[i] : zmin;
			zmax = (points[i] > zmax) ? points[i] : zmax;
		}

		float dist = 2 * (zmax - zmin);

		std::vector<float> box;
		box.push_back(xmin); box.push_back(ymin); box.push_back(zmin);
		box.push_back(xmin); box.push_back(ymin); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymin); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymin); box.push_back(zmin);

		box.push_back(xmin); box.push_back(ymax); box.push_back(zmin);
		box.push_back(xmin); box.push_back(ymax); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymax); box.push_back(zmax);
		box.push_back(xmax); box.push_back(ymax); box.push_back(zmin);

		init((xmax - xmin) / 2.0f + xmin, (ymax - ymin) / 2.0f +ymin, (zmax - zmin) / 2.0f + zmin, dist, width, height);

		zoomfactor = dist / 200.0f;

		return box;
	}

	void init(float x, float y, float z, float d, float w, float h){
		center = QVector3D(x, y, z);
		dir = QVector3D(0, 0, d);

		model.setToIdentity();
		projection.perspective(60.0f, w / h, 0.1f, 1000.0f);
		view.lookAt(center - dir, center, QVector3D(0, 1, 0));

		distance = d;
	}

	void rotate(QQuaternion q){
		qrotation = q * qrotation;
		update();
	}

	void rotate(float x, float y, float xO, float yO){
		// define center of window
		float centerX = float(width / 2);
		float centerY = float(height / 2);

		//difference of new and old mouse position
		float oldX = (xO - centerX) / centerX;
		float oldY = (yO - centerY) / centerY;
		float newX = (x - centerX) / centerX;
		float newY = (y - centerY) / centerY;

		//model.rotate(2, -xDiff, 0, -yDiff);
		float rotation[4];
		// get rotation as quaterion from trackball function
		gfs_gl_trackball(rotation, newX, newY, oldX, oldY);
		QQuaternion q = QQuaternion(QVector4D(rotation[0], rotation[1], rotation[2], rotation[3]));
		rotate(q);
	}

	void zoom(float dX){
		distance = ((distance + dX) > 1.0f) ? (distance + dX) : 1.0f;
		update();
	}

	void resize(int w, int h){
		width = w;
		height = h;
		update();
	}

	QMatrix4x4 getCombinedMatrix(){
		return projection * view * model;
	}

	QMatrix4x4 getModelMatrix(){
		return model;
	}

	QMatrix4x4 getViewMatrix(){
		return view;
	}

	QMatrix4x4 getProjMatrix(){
		return projection;
	}

private:
	QMatrix4x4 model, view, projection;
	QQuaternion qrotation;
	QVector3D center, dir;
	float distance;
	float width, height;
	float zoomfactor;
	
	void update(){
		QMatrix4x4 m1;
		m1.rotate(qrotation);

		model.setToIdentity();
		model.translate(center);
		model = model * m1;
		model.translate(-center);

		projection.setToIdentity();
		projection.perspective(60.0f, width / height, 0.1f, 1000.0f);

		view.setToIdentity();
		view.lookAt(center - dir, center, QVector3D(0, 1, 0));
	}
	
};

class MainGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	public:
		MainGLWidget(QWidget *parent = 0);
		~MainGLWidget();
		Camera cam;
		uint count;
		GLfloat * vertices;
		GLfloat * colors;

	protected:
		void initializeGL() Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;
		void resizeGL(int width, int height) Q_DECL_OVERRIDE;
		void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

	private:
		QOpenGLVertexArrayObject m_vao;
		QOpenGLBuffer m_glBuffer;
		QOpenGLShaderProgram *m_program;
		QMatrix4x4 m_proj;
		QMatrix4x4 m_camera;
		QMatrix4x4 m_world;
		QPointF oldMousePosition;
		GLuint m_posAttr;
		GLuint m_colAttr;
		GLuint m_matrixUniform;
};