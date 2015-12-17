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



class MainGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

	public:
		MainGLWidget(QWidget *parent = 0);
		~MainGLWidget();
		Camera cam;
		uint count;
		GLfloat* colors;

		/// set the vertices of the Pointcloud to be displayed
		void setPointCloud(std::vector<Point3d>* cloudPoints);
		/// set the vertices of the (fitted) Line to be displayed
		void setLine(const std::vector<Point3d>& linePoints);
		/// set the vertices of the (fitted) Plane to be displayed
		void setPLane(const std::vector<Point3d>& planePoints);

	protected:
		void initializeGL() Q_DECL_OVERRIDE;
		void paintGL() Q_DECL_OVERRIDE;
		void resizeGL(int width, int height) Q_DECL_OVERRIDE;
		void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
		void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
		void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

	private:
		QOpenGLVertexArrayObject m_vaoPointcloud;
		QOpenGLBuffer m_vvboPointCloud;
		QOpenGLBuffer m_cvboPointCloud;
		QOpenGLVertexArrayObject m_vaoLine;
		QOpenGLBuffer m_vvboLine;
		QOpenGLBuffer m_cvboLine;
		QOpenGLVertexArrayObject m_vaoPlane;
		QOpenGLBuffer m_vvboPlane;
		QOpenGLBuffer m_cvboPlane;
		QOpenGLBuffer m_glBuffer;
		QOpenGLShaderProgram *m_program;
		QMatrix4x4 m_proj;
		QMatrix4x4 m_camera;
		QMatrix4x4 m_world;
		QPointF oldMousePosition;
		GLuint m_posAttr;
		GLuint m_colAttr;
		GLuint m_matrixUniform;
		std::vector<Point3d> m_lineVertices;
		std::vector<Point3d> m_planeVertices;
		std::vector<Point3d>* m_vertices;
};

#endif // MAINGL_WIDGET_H