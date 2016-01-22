#ifndef MAINGL_WIDGET_H
#define MAINGL_WIDGET_H

#include <vector>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <qopenglshaderprogram.h>
#include "Point3d.h"
#include "Camera.h"
#include <gl/GLU.h>

namespace Primitives
{
	struct Quad
	{
		Point3d a;
		Point3d b;
		Point3d c;
		Point3d d;

		Quad()
		{
			this->a = Point3d();
			this->b = Point3d();
			this->c = Point3d();
			this->d = Point3d();
		}
		Quad(Point3d A, Point3d B, Point3d C, Point3d D)
		{
			this->a = A;
			this->b = B;
			this->c = C;
			this->d = D;
		}
	};

	struct Line
	{
		Point3d a;
		Point3d b;

		Line()
		{
			this->a = Point3d();
			this->b = Point3d();
		}

		Line(Point3d a, Point3d b)
		{
			this->a = a;
			this->b = b;
		}
	};

	struct Sphere
	{
		Point3d p;
		double r;

		Sphere()
		{
			this->p = Point3d();
			this->r = double();
		}
		Sphere(Point3d P, double R)
		{
			this->p = P;
			this->r = R;
		}
	};
}

class MainGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	public:
		MainGLWidget(QWidget *parent = 0);
		~MainGLWidget();
		Camera cam;
		uint count;
		GLfloat* colors;
		std::vector<Point3d>* m_vertices;
		std::vector<Point3d>* m_normals;

		void setFittedPlane(Point3d a, Point3d b, Point3d c, Point3d d);
		void setFittedLine(Point3d a, Point3d b);
		void setFittedSphere(Point3d p, double r);

	protected:
		void initializeGL() Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;
		void resizeGL(int width, int height) Q_DECL_OVERRIDE;
		void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
		void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

		void drawPlane();
		void drawLine();
		void drawSphere();

	private:
		QOpenGLVertexArrayObject m_vao;
		QOpenGLBuffer m_glBuffer;
		QOpenGLShaderProgram *m_program;
		QMatrix4x4 m_proj;
		QMatrix4x4 m_camera;
		QMatrix4x4 m_world;
		QPointF oldMousePosition;
		GLuint m_posAttr;
		GLuint m_normalAttr;
		GLuint m_colAttr;
		GLuint m_modelViewUniform;
		GLuint m_normalUniform;
		GLuint m_projectionUniform;

		// TODO find better solution
		bool drawFittedPlane;
		bool drawFittedLine;
		bool drawFittedSphere;
		Primitives::Quad fittedPlane;
		Primitives::Line fittedLine;
		Primitives::Sphere fittedSphere;
};

#endif // MAINGL_WIDGET_H